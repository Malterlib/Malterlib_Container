// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	// INVARIANT: All reads of key-stored index entries MUST check validity first via
	// CPackedMapData::f_GetIndexEntryValid() before accessing the entry value. For slot-index storage,
	// the capacity sentinel serves as the validity marker. For key storage, a separate
	// validity bitmask is used because a default-constructed key could be a valid separator.

	// Compare an index entry with a key - returns true if entry < key
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_CompareIndexEntry(CPackedMapData const *_pData, umint _iEntry, t_CKey const &_Key) const noexcept
	{
		// Empty/invalid entries are treated as +infinity (never less than any key)
		if (!_pData->f_GetIndexEntryValid(_iEntry))
			return false;  // +infinity

		if constexpr (mcp_bIndexStoresKeys)
			return mp_Compare(_pData->m_pIndex[_iEntry], _Key) < 0;
		else
			return mp_Compare(_pData->m_pKeys[_pData->m_pIndex[_iEntry]], _Key) < 0;
	}

	// Build a single level of the index from its children
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_BuildIndexLevel(CPackedMapData *_pData, umint _iLevel) noexcept
	{
		if (_iLevel == 0 || _iLevel >= _pData->m_nStaticIndexLevels)
			return;

		auto const *pLevelOffsets = _pData->m_pLevelOffsets;
		umint iChildLevel = _iLevel - 1;
		umint iChildOffset = pLevelOffsets[iChildLevel];
		umint iThisOffset = pLevelOffsets[_iLevel];
		umint ChildSize = iThisOffset - iChildOffset;
		umint iThisEndOffset = (_iLevel + 1 < _pData->m_nStaticIndexLevels) ? pLevelOffsets[_iLevel + 1] : _pData->m_nStaticIndexTotalEntries;
		umint ThisSize = iThisEndOffset - iThisOffset;

		auto *pIndex = _pData->m_pIndex;

		for (umint iParent = 0; iParent < ThisSize; ++iParent)
		{
			// Child range for this parent entry
			umint iChildStart = iParent * mcp_Fanout;
			umint iChildEnd = fg_Min(iChildStart + mcp_Fanout, ChildSize);

			// Find the min (leftmost valid) child entry
			umint iMinChild = 0;
			bool bFoundValid = false;

			for (umint iChild = iChildStart; iChild < iChildEnd; ++iChild)
			{
				if (_pData->f_GetIndexEntryValid(iChildOffset + iChild))
				{
					iMinChild = iChild;
					bFoundValid = true;
					break;
				}
			}

			umint iParentEntry = iThisOffset + iParent;

			if (!bFoundValid)
			{
				// All children invalid - mark parent as invalid
				_pData->f_ClearIndexEntry(iParentEntry);
			}
			else
			{
				// Copy the min child's separator and mark valid
				_pData->f_SetIndexEntry(iParentEntry, pIndex[iChildOffset + iMinChild]);
			}
		}
	}

	// Rebuild the static index for all segments
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_RebuildStaticIndex(CPackedMapData *_pData) noexcept
	{
		auto *pMeta = _pData->m_pSegmentMeta;
		auto *pKeys = _pData->m_pKeys;
		umint nSegments = _pData->m_nSegments;

		// Step A: Fill leaf separators (level 0) and set validity
		for (umint iSeg = 0; iSeg < nSegments; ++iSeg)
		{
			umint Count = pMeta[iSeg].m_Count;

			if (Count == 0)
			{
				// Empty segment - mark as invalid
				_pData->f_ClearIndexEntry(iSeg);
			}
			else
			{
				// Get min key slot and set entry as valid
				umint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
				if constexpr (mcp_bIndexStoresKeys)
					_pData->f_SetIndexEntry(iSeg, pKeys[iFirst]);
				else
					_pData->f_SetIndexEntry(iSeg, iFirst);
			}
		}

		// Step B: Build upper levels
		for (umint iLevel = 1; iLevel < _pData->m_nStaticIndexLevels; ++iLevel)
			fp_BuildIndexLevel(_pData, iLevel);
	}

	// Rebuild the static index for a range of segments
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_RebuildStaticIndexRange(CPackedMapData *_pData, umint _iStartSeg, umint _iEndSeg) noexcept
	{
		auto *pMeta = _pData->m_pSegmentMeta;
		auto *pIndex = _pData->m_pIndex;
		auto *pKeys = _pData->m_pKeys;

		auto const *pLevelOffsets = _pData->m_pLevelOffsets;

		// Step A: Update leaf entries in range and set validity
		for (umint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
		{
			umint Count = pMeta[iSeg].m_Count;

			if (Count == 0)
			{
				_pData->f_ClearIndexEntry(iSeg);
			}
			else
			{
				umint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
				if constexpr (mcp_bIndexStoresKeys)
					_pData->f_SetIndexEntry(iSeg, pKeys[iFirst]);
				else
					_pData->f_SetIndexEntry(iSeg, iFirst);
			}
		}

		// Step B: Update parent levels
		// Compute parent range that covers [_iStartSeg, _iEndSeg) and rebuild those entries
		umint iRangeStart = _iStartSeg;
		umint iRangeEnd = _iEndSeg;

		for (umint iLevel = 1; iLevel < _pData->m_nStaticIndexLevels; ++iLevel)
		{
			// Compute parent indices that cover the child range
			umint iParentStart = iRangeStart >> mcp_FanoutBits;
			umint iParentEnd = (iRangeEnd + mcp_Fanout - 1) >> mcp_FanoutBits;

			umint iChildLevel = iLevel - 1;
			umint iChildOffset = pLevelOffsets[iChildLevel];
			umint iThisOffset = pLevelOffsets[iLevel];
			umint ChildSize = iThisOffset - iChildOffset;

			for (umint iParent = iParentStart; iParent < iParentEnd; ++iParent)
			{
				// Child range for this parent entry
				umint iChildStart = iParent * mcp_Fanout;
				umint iChildEnd = fg_Min(iChildStart + mcp_Fanout, ChildSize);

				// Find the min (leftmost valid) child entry
				umint iMinChild = 0;
				bool bFoundValid = false;

				for (umint iChild = iChildStart; iChild < iChildEnd; ++iChild)
				{
					if (_pData->f_GetIndexEntryValid(iChildOffset + iChild))
					{
						iMinChild = iChild;
						bFoundValid = true;
						break;
					}
				}

				umint iParentEntry = iThisOffset + iParent;

				if (!bFoundValid)
				{
					// All children invalid - mark parent as invalid
					_pData->f_ClearIndexEntry(iParentEntry);
				}
				else
				{
					// Copy the min child's separator and mark valid
					_pData->f_SetIndexEntry(iParentEntry, pIndex[iChildOffset + iMinChild]);
				}
			}

			// Update range for next level
			iRangeStart = iParentStart;
			iRangeEnd = iParentEnd;
		}
	}

	// Update a single static index entry and its ancestors
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_UpdateStaticIndexEntry(CPackedMapData *_pData, umint _iSegment) noexcept
	{
		auto *pMeta = _pData->m_pSegmentMeta;
		auto *pIndex = _pData->m_pIndex;
		auto *pKeys = _pData->m_pKeys;

		auto const *pLevelOffsets = _pData->m_pLevelOffsets;

		// Step 1: Update leaf entry for _iSegment and set validity
		umint Count = pMeta[_iSegment].m_Count;

		if (Count == 0)
		{
			_pData->f_ClearIndexEntry(_iSegment);
		}
		else
		{
			umint iFirst = fsp_GetSegmentFirstSlot(_iSegment, Count);

			if constexpr (mcp_bIndexStoresKeys)
				_pData->f_SetIndexEntry(_iSegment, pKeys[iFirst]);
			else
				_pData->f_SetIndexEntry(_iSegment, iFirst);
		}

		// Step 2: Update ancestors
		umint iCurrent = _iSegment;

		for (umint iLevel = 1; iLevel < _pData->m_nStaticIndexLevels; ++iLevel)
		{
			// Parent index
			umint iParent = iCurrent >> mcp_FanoutBits;

			umint iChildLevel = iLevel - 1;
			umint iChildOffset = pLevelOffsets[iChildLevel];
			umint iThisOffset = pLevelOffsets[iLevel];
			umint ChildSize = iThisOffset - iChildOffset;

			// Child range for this parent
			umint iChildStart = iParent * mcp_Fanout;
			umint iChildEnd = fg_Min(iChildStart + mcp_Fanout, ChildSize);

			// Find min (leftmost valid) child
			umint iMinChild = 0;
			bool bFoundValid = false;

			umint iParentEntry = iThisOffset + iParent;

			bool bOldParentValid = _pData->f_GetIndexEntryValid(iParentEntry);

			for (umint iChild = iChildStart; iChild < iChildEnd; ++iChild)
			{
				if (_pData->f_GetIndexEntryValid(iChildOffset + iChild))
				{
					iMinChild = iChild;
					bFoundValid = true;
					break;
				}
			}

			if (!bFoundValid)
			{
				// All children invalid - early exit if parent was already invalid
				if (!bOldParentValid)
					return;

				_pData->f_ClearIndexEntry(iParentEntry);
			}
			else
			{
				// Early exit if parent value and validity didn't change
				CIndexEntry const &NewEntry = pIndex[iChildOffset + iMinChild];
				if (bOldParentValid)
				{
					if constexpr (mcp_bIndexStoresKeys)
					{
						if (mp_Compare(pIndex[iParentEntry], NewEntry) == 0)
							return;
					}
					else
					{
						if (pIndex[iParentEntry] == NewEntry)
							return;
					}
				}

				_pData->f_SetIndexEntry(iParentEntry, NewEntry);
			}

			iCurrent = iParent;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_FindRebalanceWindow(CPackedMapData const *_pData, umint _iSegment) const noexcept
	 	-> NStorage::TCTuple<umint, umint, umint>
	{
		umint nSegments = _pData->m_nSegments;
		auto const *pMeta = _pData->m_pSegmentMeta;

		// Calibrator tree approach: use power-of-2 aligned windows at each level
		// Level 0: window size 1 (single segment)
		// Level k: window size 2^k, aligned to 2^k boundaries
		umint nLevels = fp_CalibratorLevelCount(nSegments);
		umint TotalCapacity = nSegments * mcp_SegmentSize;
		umint nTotalElements = _pData->m_nElements;
		bool bNeedSpaceForInsert = pMeta[_iSegment].m_Count >= mcp_SegmentSize;

		// Helper to count elements for partial windows at end of array using level-0 calibrator counts.
		// This is O(partial window size) but only used for the tail - the paper accepts this tradeoff.
		auto fCountPartialWindow = [&](umint _iStart, umint _iEnd) -> umint
			{
				umint nElements = 0;
				for (umint iElement = _iStart; iElement < _iEnd; ++iElement)
					nElements += fp_GetCalibratorWindowCount(_pData, 0, iElement);
				return nElements;
			}
		;

		// Determine how high we must expand due to existing density violations on the path.
		// If a window at level h violates [rho(h), tau(h)], redistributing only within that same
		// window cannot fix it (its total element count is unchanged), so we must choose level > h.
		umint iMinAcceptLevel = 0;
		umint WindowSize = 1;
		umint WindowShift = 0;

		for (umint iLevel = 0; iLevel < nLevels; ++iLevel)
		{
			umint iWindow = _iSegment >> WindowShift;
			umint iWindowStart = iWindow * WindowSize;
			umint iWindowEnd = fg_Min(iWindowStart + WindowSize, nSegments);

			bool bIsFullWindow = (iWindowEnd == iWindowStart + WindowSize);
			umint nElements;
			if (bIsFullWindow)
				nElements = fp_GetCalibratorWindowCount(_pData, iLevel, iWindow);
			else
				nElements = fCountPartialWindow(iWindowStart, iWindowEnd);

			umint Capacity = (iWindowEnd - iWindowStart) * mcp_SegmentSize;
			umint nProjectedElements = nElements;
			if (bNeedSpaceForInsert && nProjectedElements < Capacity)
				++nProjectedElements;

			umint nMinElems, nMaxElems;
			fp_GetLevelElementBounds(iLevel, nLevels, Capacity, nMinElems, nMaxElems);
			umint nUpperLimit = bNeedSpaceForInsert ? nMaxElems : (nMaxElems + 1);

			bool bUpperViolation = nProjectedElements > nUpperLimit;
			bool bLowerViolation = false;

			if (nMinElems > 0 && nProjectedElements > 0 && nProjectedElements + 1 < nMinElems)
			{
				// Check if global density is high enough to enforce lower bound
				// GlobalDensity >= RootLowerBound ⟺ nTotalElements >= ceil(RootLowerBound * TotalCapacity)
				// ExpectedWindowElements >= 1.0 ⟺ nTotalElements * Capacity >= TotalCapacity
				umint nWindowSegments = iWindowEnd - iWindowStart;
				umint nScaledExpectedWindowElements;
				bool bExpectedWindowCouldContainElement = fg_MultiplyOverflow(nTotalElements, nWindowSegments, nScaledExpectedWindowElements) || (nScaledExpectedWindowElements >= nSegments);

				if constexpr (mcp_bUseFixedPoint)
					bLowerViolation = bExpectedWindowCouldContainElement && (nTotalElements >= fsp_ScaledMulCeil(mcp_RootLowerBoundScaled, TotalCapacity));
				else
					bLowerViolation = bExpectedWindowCouldContainElement && (pfp64(nTotalElements) >= mc_RootLowerBound * TotalCapacity);
			}

			if (bUpperViolation || bLowerViolation)
				iMinAcceptLevel = fg_Max(iMinAcceptLevel, iLevel + 1);

			WindowSize *= 2;
			++WindowShift;
		}

		// For full-segment insert pressure, prefer larger windows than the minimal
		// violating level. This reduces rebalance frequency on sequential runs.
		if (bNeedSpaceForInsert)
		{
			umint nExtraLevels = 3;
			if (iMinAcceptLevel + nExtraLevels < nLevels)
				iMinAcceptLevel += nExtraLevels;
			else
				iMinAcceptLevel = nLevels - 1;
		}
		else if (nSegments > 1)
		{
			// A delete-side rebalance on a single segment is a no-op: redistribution cannot
			// change that segment's element count, so it cannot repair a lower-bound violation.
			iMinAcceptLevel = fg_Max(iMinAcceptLevel, umint(1));
		}

		// Start at level 0 and pick the first feasible level that is high enough to fix violations.
		WindowSize = 1;
		WindowShift = 0;

		for (umint iLevel = 0; iLevel < nLevels; ++iLevel)
		{
			umint iWindow = _iSegment >> WindowShift;
			umint iWindowStart = iWindow * WindowSize;
			umint iWindowEnd = fg_Min(iWindowStart + WindowSize, nSegments);

			bool bIsFullWindow = (iWindowEnd == iWindowStart + WindowSize);
			umint nElements;
			if (bIsFullWindow)
				nElements = fp_GetCalibratorWindowCount(_pData, iLevel, iWindow);
			else
				nElements = fCountPartialWindow(iWindowStart, iWindowEnd);

			umint Capacity = (iWindowEnd - iWindowStart) * mcp_SegmentSize;

			umint nProjectedElements = nElements;
			if (bNeedSpaceForInsert && nProjectedElements < Capacity)
				++nProjectedElements;

			umint nMinElems, nMaxElems;
			fp_GetLevelElementBounds(iLevel, nLevels, Capacity, nMinElems, nMaxElems);
			umint nUpperLimit = bNeedSpaceForInsert ? nMaxElems : (nMaxElems + 1);
			bool bAcceptWindow = (nProjectedElements <= nUpperLimit) && (nProjectedElements + 1 >= nMinElems);

			// If the target segment is full, don't accept a single-segment window.
			// We must expand to find slack or trigger a resize.
			if (bAcceptWindow && WindowSize == 1 && bNeedSpaceForInsert)
				bAcceptWindow = false;

			if (bAcceptWindow && iLevel >= iMinAcceptLevel)
				return {iWindowStart, iWindowEnd, iMinAcceptLevel};

			WindowSize *= 2;
			++WindowShift;
		}

		// If we get here, need to use the entire array (and possibly resize)
		return {0, nSegments, iMinAcceptLevel};
	}

	// Rebalance starting from a segment
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_RebalanceFromSegment(CPackedMapData *_pData, umint _iSegment)
	{
		if constexpr (t_Options.m_bStats)
			fp_StatsAddRebalance();

		DMibFastCheck(_pData->m_Capacity > 0);

		auto [iStart, iEnd, iFirstViolationLevel] = fp_FindRebalanceWindow(_pData, _iSegment);

		// Enforce root-level density bound globally
		if constexpr (mcp_bUseFixedPoint)
		{
			if (_pData->m_nElements > fsp_ScaledMulFloor(mcp_RootUpperBoundScaled, _pData->m_Capacity))
			{
				fp_Resize(_pData->m_nSegments * 2);
				return;
			}
		}
		else
		{
			if (pfp64(_pData->m_nElements) > mc_RootUpperBound * pfp64(_pData->m_Capacity))
			{
				fp_Resize(_pData->m_nSegments * 2);
				return;
			}
		}

		// Calculate density using calibrator tree counts
		// For aligned power-of-2 windows, this is O(1). For partial windows, O(partial size).
		umint WindowSize = iEnd - iStart;
		umint nElements = 0;

		// Check if window is aligned and full (power of 2 and aligned to that boundary)
		bool bIsPowerOfTwo = (WindowSize > 0) && ((WindowSize & (WindowSize - 1)) == 0);
		bool bIsAligned = bIsPowerOfTwo && ((iStart & (WindowSize - 1)) == 0);
		bool bIsFull = (iEnd == iStart + WindowSize);

		if (bIsPowerOfTwo && bIsAligned && bIsFull)
		{
			// Calculate level from window size: level = log2(WindowSize)
			umint iLevel = 0;
			umint Size = WindowSize;
			while (Size > 1)
			{
				Size >>= 1;
				++iLevel;
			}

			umint iWindow = iStart >> iLevel;

			nElements = fp_GetCalibratorWindowCount(_pData, iLevel, iWindow);
		}
		else
		{
			// Non-aligned or partial window: sum level-0 counts
			for (umint iElement = iStart; iElement < iEnd; ++iElement)
				nElements += fp_GetCalibratorWindowCount(_pData, 0, iElement);
		}

		umint WindowCapacity = WindowSize * mcp_SegmentSize;

		// If density is still too high after expanding to full array, resize
		if constexpr (mcp_bUseFixedPoint)
		{
			if ((iEnd - iStart == _pData->m_nSegments) && (nElements > fsp_ScaledMulFloor(mcp_RootUpperBoundScaled, WindowCapacity)))
			{
				// Double the number of segments
				umint nNewSegments = _pData->m_nSegments * 2;
				fp_Resize(nNewSegments);
				return;
			}
		}
		else
		{
			if ((iEnd - iStart == _pData->m_nSegments) && (pfp64(nElements) > (mc_RootUpperBound * pfp64(WindowCapacity))))
			{
				// Double the number of segments
				umint nNewSegments = _pData->m_nSegments * 2;
				fp_Resize(nNewSegments);
				return;
			}
		}

		if constexpr (t_Options.m_bStats)
			fp_StatsAddRebalanceWindow(nElements, WindowSize);

		// Calculate the global base level for this window (ceil(log2(window size)))
		umint iWindowLevel = (umint)(fg_GetHighestBitSet(WindowSize - 1) + 1);

		if constexpr (t_Options.m_bStats)
			fp_StatsAddRebalanceDecision(_pData->m_pSegmentMeta && _pData->m_pSegmentMeta[_iSegment].m_Count >= mcp_SegmentSize, iFirstViolationLevel, iWindowLevel);

		// Get global level count for threshold computation
		umint nGlobalLevels = fp_CalibratorLevelCount(_pData->m_nSegments);

		// Redistribute elements within the window
		if constexpr (t_Options.m_bAdaptive)
			fp_AdaptiveRedistribute(_pData, iStart, iEnd, iWindowLevel, nGlobalLevels);
		else
			fp_EvenRedistribute(_pData, iStart, iEnd, iWindowLevel, nGlobalLevels);

		// Update calibrator counts for affected range: O(W log n) instead of O(n)
		fp_UpdateCalibratorCountsRange(_pData, iStart, iEnd);

		// Rebuild index for affected segments
		fp_RebuildStaticIndexRange(_pData, iStart, iEnd);
	}

	// Evenly redistribute elements across segments using calibrator tree
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_EvenRedistribute
		(
			CPackedMapData *_pData
			, umint _iStartSeg
			, umint _iEndSeg
			, umint _iWindowLevel
			, umint _nGlobalLevels
		)
	{
		if constexpr (t_Options.m_bStats)
			fp_StatsAddEvenRedistribute();

		auto *pMeta = _pData->m_pSegmentMeta;
		auto *pKeys = _pData->m_pKeys;
		auto *pValues = _pData->m_pValues;

		// Count total elements in the window
		umint nTotalElements = 0;
		for (umint iSegment = _iStartSeg; iSegment < _iEndSeg; ++iSegment)
			nTotalElements += pMeta[iSegment].m_Count;

		if (nTotalElements == 0)
			return;

		umint nSegments = _iEndSeg - _iStartSeg;

		// Allocate combined buffer for all temporary data
		// Use stack for small rebalances (most delete-triggered rebalances are 2-8 segments)
		constexpr static umint c_nBufferAlign = fg_Max(alignof(umint), alignof(t_CKey), alignof(t_CValue), alignof(CSegmentMeta));

		umint nTargetsBytes = nSegments * sizeof(umint);
		umint nKeysBytes = nTotalElements * sizeof(t_CKey);
		umint nValuesBytes = nTotalElements * sizeof(t_CValue);
		umint nOldMetaBytes = 0;
		umint nOldPrefixBytes = 0;
		if constexpr (mcp_bDetectorStoresKeys)
		{
			nOldMetaBytes = nSegments * sizeof(CSegmentMeta);
			nOldPrefixBytes = (nSegments + 1) * sizeof(umint);
		}

		umint nTargetsAligned = fg_AlignUp(nTargetsBytes, c_nBufferAlign);
		umint nKeysAligned = fg_AlignUp(nKeysBytes, c_nBufferAlign);
		umint nValuesAligned = fg_AlignUp(nValuesBytes, c_nBufferAlign);
		umint nOldMetaAligned = fg_AlignUp(nOldMetaBytes, c_nBufferAlign);
		umint TotalBufferSize = nTargetsAligned + nKeysAligned + nValuesAligned + nOldMetaAligned + nOldPrefixBytes;

		constexpr umint c_nStackThreshold = 16384;
		alignas(c_nBufferAlign) uint8 aStackBuffer[c_nStackThreshold];

		uint8 *pBuffer;
		umint AllocatedBufferSize;
		if (TotalBufferSize <= c_nStackThreshold)
		{
			pBuffer = aStackBuffer;
			AllocatedBufferSize = 0;
		}
		else
		{
			pBuffer = (uint8 *)mp_Allocator.f_AllocAligned(TotalBufferSize, c_nBufferAlign);
			AllocatedBufferSize = TotalBufferSize;
		}

		if constexpr (t_Options.m_bStats)
			fp_StatsAddRebalanceAlloc(AllocatedBufferSize > 0);

		umint *pTargets = (umint *)pBuffer;
		t_CKey *pTempKeys = (t_CKey *)(pBuffer + nTargetsAligned);
		t_CValue *pTempValues = (t_CValue *)(pBuffer + nTargetsAligned + nKeysAligned);

		CSegmentMeta *pOldMeta = nullptr;
		umint OldMetaCount = 0;
		umint *pOldPrefix = nullptr;
		auto CleanupBuffer = g_OnScopeExit / [&]
			{
				if constexpr (mcp_bDetectorStoresKeys)
				{
					for (umint iSegment = 0; iSegment < OldMetaCount; ++iSegment)
						pOldMeta[iSegment].~CSegmentMeta();
				}
				if (AllocatedBufferSize > 0)
					mp_Allocator.f_Free(pBuffer, AllocatedBufferSize);
			}
		;

		if constexpr (mcp_bDetectorStoresKeys)
		{
			pOldMeta = (CSegmentMeta *)(pBuffer + nTargetsAligned + nKeysAligned + nValuesAligned);
			pOldPrefix = (umint *)(pBuffer + nTargetsAligned + nKeysAligned + nValuesAligned + nOldMetaAligned);
			pOldPrefix[0] = 0;
			for (umint iSegment = 0; iSegment < nSegments; ++iSegment)
			{
				new(&pOldMeta[iSegment]) CSegmentMeta(pMeta[_iStartSeg + iSegment]);
				++OldMetaCount;
				pOldPrefix[iSegment + 1] = pOldPrefix[iSegment] + pMeta[_iStartSeg + iSegment].m_Count;
			}
		}

		for (umint iSegment = 0; iSegment < nSegments; ++iSegment)
			pTargets[iSegment] = 0;

		fp_DistributeElements(pTargets, 0, nSegments, nTotalElements, 0, _iWindowLevel, _nGlobalLevels);

		// Collect all elements
		umint iTempPos = 0;
		if constexpr (mcp_bNothrowElementMove)
		{
			for (umint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
			{
				umint Count = pMeta[iSeg].m_Count;
				if (Count == 0)
					continue;

				umint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
				for (umint iSegmentSlot = 0; iSegmentSlot < Count; ++iSegmentSlot)
				{
					umint iSlot = iFirst + iSegmentSlot;
					new(&pTempKeys[iTempPos]) t_CKey(fg_Move(pKeys[iSlot]));
					new(&pTempValues[iTempPos]) t_CValue(fg_Move(pValues[iSlot]));
					if constexpr (t_Options.m_bStats)
						fp_StatsAddRebalanceMoves(1);
					pKeys[iSlot].~t_CKey();
					pValues[iSlot].~t_CValue();
					++iTempPos;
				}
				pMeta[iSeg].m_Count = 0;
			}
		}
		else
		{
			// Collect without destroying sources first, so on exception no gaps exist
			auto CollectCleanup = g_OnScopeExit / [&]
				{
					for (umint i = 0; i < iTempPos; ++i)
					{
						pTempKeys[i].~t_CKey();
						pTempValues[i].~t_CValue();
					}
				}
			;

			for (umint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
			{
				umint Count = pMeta[iSeg].m_Count;
				if (Count == 0)
					continue;

				umint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
				for (umint iSegmentSlot = 0; iSegmentSlot < Count; ++iSegmentSlot)
				{
					umint iSlot = iFirst + iSegmentSlot;
					new(&pTempKeys[iTempPos]) t_CKey(fg_Move(pKeys[iSlot]));
					auto KeyGuard = g_OnScopeExit / [&]
						{
							pTempKeys[iTempPos].~t_CKey();
						}
					;
					new(&pTempValues[iTempPos]) t_CValue(fg_Move(pValues[iSlot]));
					KeyGuard.f_Clear();
					if constexpr (t_Options.m_bStats)
						fp_StatsAddRebalanceMoves(1);
					++iTempPos;
				}
			}

			CollectCleanup.f_Clear();

			// Destroy sources now that all are safely in temp
			for (umint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
			{
				umint Count = pMeta[iSeg].m_Count;
				if (Count == 0)
					continue;

				umint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
				for (umint iSegmentSlot = 0; iSegmentSlot < Count; ++iSegmentSlot)
				{
					umint iSlot = iFirst + iSegmentSlot;
					pKeys[iSlot].~t_CKey();
					pValues[iSlot].~t_CValue();
				}
				pMeta[iSeg].m_Count = 0;
			}
		}

		// Redistribute elements according to target cardinalities
		iTempPos = 0;

		// Cleanup guard: on exception during placement, destroy remaining temp elements
		// and recalculate m_nElements from actual segment counts
		umint nElementsOutsideWindow = _pData->m_nElements - nTotalElements;
		auto PlaceCleanup = g_OnScopeExit / [&]
			{
				if constexpr (!mcp_bNothrowElementMove)
				{
					for (umint i = iTempPos; i < nTotalElements; ++i)
					{
						pTempKeys[i].~t_CKey();
						pTempValues[i].~t_CValue();
					}
					umint nActual = 0;
					for (umint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
						nActual += pMeta[iSeg].m_Count;
					_pData->m_nElements = nElementsOutsideWindow + nActual;
				}
			}
		;

		umint iOldOverlapStart = 0;
		umint nRemapOverlapChecks = 0;
		umint nRemapTransfers = 0;
		for (umint iRelSeg = 0; iRelSeg < nSegments; ++iRelSeg)
		{
			umint iSeg = _iStartSeg + iRelSeg;
			umint TargetCount = pTargets[iRelSeg];
			umint iElemStart = iTempPos;
			umint iElemEnd = iElemStart + TargetCount;

			// Don't place more than we have remaining
			TargetCount = fg_Min(TargetCount, nTotalElements - iTempPos);
			iElemEnd = iElemStart + TargetCount;

			if (TargetCount > 0)
			{
				umint iFirst = fsp_GetSegmentFirstSlot(iSeg, TargetCount);

				if constexpr (mcp_bNothrowElementMove)
				{
					for (umint iSegmentSlot = 0; iSegmentSlot < TargetCount; ++iSegmentSlot)
					{
						umint iSlot = iFirst + iSegmentSlot;
						new(&pKeys[iSlot]) t_CKey(fg_Move(pTempKeys[iTempPos]));
						new(&pValues[iSlot]) t_CValue(fg_Move(pTempValues[iTempPos]));
						if constexpr (t_Options.m_bStats)
							fp_StatsAddRebalanceMoves(1);
						pTempKeys[iTempPos].~t_CKey();
						pTempValues[iTempPos].~t_CValue();
						++iTempPos;
					}
				}
				else
				{
					pMeta[iSeg].m_Count = (uint16)TargetCount;
					umint nPlacedInSeg = 0;
					auto SegCleanup = g_OnScopeExit / [&]
						{
							for (umint i = 0; i < nPlacedInSeg; ++i)
							{
								pKeys[iFirst + i].~t_CKey();
								pValues[iFirst + i].~t_CValue();
							}
							pMeta[iSeg].m_Count = 0;
						}
					;
					for (umint iSegmentSlot = 0; iSegmentSlot < TargetCount; ++iSegmentSlot)
					{
						umint iSlot = iFirst + iSegmentSlot;
						new(&pKeys[iSlot]) t_CKey(fg_Move(pTempKeys[iTempPos]));
						auto KeyGuard = g_OnScopeExit / [&]
							{
								pKeys[iSlot].~t_CKey();
							}
						;
						new(&pValues[iSlot]) t_CValue(fg_Move(pTempValues[iTempPos]));
						KeyGuard.f_Clear();
						if constexpr (t_Options.m_bStats)
							fp_StatsAddRebalanceMoves(1);
						pTempKeys[iTempPos].~t_CKey();
						pTempValues[iTempPos].~t_CValue();
						++nPlacedInSeg;
						++iTempPos;
					}
					SegCleanup.f_Clear();
				}
			}

			if constexpr (mcp_bDetectorStoresKeys)
			{
				fp_ResetDetectorState(_pData, iSeg);
				if (TargetCount > 0)
				{
					while (iOldOverlapStart < nSegments && pOldPrefix[iOldOverlapStart + 1] <= iElemStart)
						++iOldOverlapStart;

					umint iBestOld = 0;
					bool bBestFound = false;
					umint nBestOverlap = 0;
					for (umint iOld = iOldOverlapStart; iOld < nSegments && pOldPrefix[iOld] < iElemEnd; ++iOld)
					{
						if constexpr (t_Options.m_bStats)
							++nRemapOverlapChecks;
						umint iOverlapStart = fg_Max(pOldPrefix[iOld], iElemStart);
						umint iOverlapEnd = fg_Min(pOldPrefix[iOld + 1], iElemEnd);
						umint nOverlap = (iOverlapEnd > iOverlapStart) ? (iOverlapEnd - iOverlapStart) : (umint)0;
						if (nOverlap > nBestOverlap)
						{
							nBestOverlap = nOverlap;
							iBestOld = iOld;
							bBestFound = true;
						}
					}

					if (bBestFound)
					{
						fp_CopyDetectorStateScaled(pMeta[iSeg], pOldMeta[iBestOld], nBestOverlap, pOldMeta[iBestOld].m_Count);
						if constexpr (t_Options.m_bStats)
							++nRemapTransfers;
					}
					if constexpr (mcp_bNothrowElementMove)
						pMeta[iSeg].m_Count = (uint16)TargetCount;
				}
			}
			else if constexpr (t_Options.m_bAdaptive)
			{
				fp_ResetDetectorState(_pData, iSeg);
				if constexpr (mcp_bNothrowElementMove)
					pMeta[iSeg].m_Count = (uint16)TargetCount;
			}
			else
			{
				if constexpr (mcp_bNothrowElementMove)
					pMeta[iSeg].m_Count = (uint16)TargetCount;
			}
		}

		PlaceCleanup.f_Clear();
		if constexpr (mcp_bDetectorStoresKeys && t_Options.m_bStats)
			fp_StatsAddDetectorRemap(nSegments, nRemapOverlapChecks, nRemapTransfers);

#if DMibDebug
		DMibCheck(fp_ValidateRedistributeDensityBounds(_pData, _iStartSeg, _iEndSeg, nTotalElements, _nGlobalLevels, "Even"));
#endif
	}
}
