// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	// Calibrator tree level count (binary tree over segments)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_CalibratorLevelCount(umint _nSegments) const noexcept
	{
		if (_nSegments <= 1)
			return 1;

		umint nLevels = 1;
		umint Size = _nSegments;
		while (Size > 1)
		{
			Size = (Size + 1) / 2;
			++nLevels;
		}
		return nLevels;
	}

	// Density thresholds per calibrator tree level (linear interpolation)
	// Dispatches to fixed-point or float based on mcp_bUseFixedPoint.
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_GetLevelThresholds
		(
			umint _iLevel
			, umint _nLevels
			, umint &o_ScaledLowerBound
			, umint &o_ScaledUpperBound
		) const noexcept
	{
		o_ScaledLowerBound = fp_GetLevelLowerThreshold(_iLevel, _nLevels);
		o_ScaledUpperBound = fp_GetLevelUpperThreshold(_iLevel, _nLevels);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_GetLevelLowerThreshold(umint _iLevel, umint _nLevels) const noexcept
	{
		if (_nLevels <= 1)
			return mcp_LeafLowerBoundScaled;

		if constexpr (!mcp_bUseFixedPoint)
		{
			pfp64 Threshold = pfp64(_iLevel) / pfp64(_nLevels - 1);
			return (umint)((mc_LeafLowerBound + (mc_RootLowerBound - mc_LeafLowerBound) * Threshold) * mcp_DensityScale);
		}
		else if constexpr (mcp_bUseReciprocalDivide)
			return mcp_LeafLowerBoundScaled + fsp_ReciprocalDivide(mcp_LowerBoundDeltaScaled * _iLevel, _nLevels - 1);
		else
			return mcp_LeafLowerBoundScaled + (mcp_LowerBoundDeltaScaled * _iLevel) / (_nLevels - 1);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_GetLevelUpperThreshold(umint _iLevel, umint _nLevels) const noexcept
	{
		if (_nLevels <= 1)
			return mcp_LeafUpperBoundScaled;

		if constexpr (!mcp_bUseFixedPoint)
		{
			pfp64 Threshold = pfp64(_iLevel) / pfp64(_nLevels - 1);
			return (umint)((mc_LeafUpperBound + (mc_RootUpperBound - mc_LeafUpperBound) * Threshold) * mcp_DensityScale);
		}
		else if constexpr (mcp_bUseReciprocalDivide)
			return mcp_LeafUpperBoundScaled - fsp_ReciprocalDivide(mcp_UpperBoundDeltaScaled * _iLevel, _nLevels - 1);
		else
			return mcp_LeafUpperBoundScaled - (mcp_UpperBoundDeltaScaled * _iLevel) / (_nLevels - 1);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_GetLevelDensityBounds
		(
			umint _iLevel
			, umint _nLevels
		) const noexcept -> CLevelDensityBounds
	{
		if constexpr (!mcp_bUseFixedPoint)
		{
			if (_nLevels <= 1)
				return {mc_LeafLowerBound, mc_LeafUpperBound};

			pfp64 Threshold = pfp64(_iLevel) / pfp64(_nLevels - 1);
			return
				{
					mc_LeafLowerBound + (mc_RootLowerBound - mc_LeafLowerBound) * Threshold
					, mc_LeafUpperBound + (mc_RootUpperBound - mc_LeafUpperBound) * Threshold
				}
			;
		}
		else
		{
			return
				{
					fp_GetLevelLowerThreshold(_iLevel, _nLevels)
					, fp_GetLevelUpperThreshold(_iLevel, _nLevels)
				}
			;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_ElementBoundsFromDensity
		(
			CLevelDensityBounds const &_DensityBounds
			, umint _Capacity
			, umint &o_nMinElements
			, umint &o_nMaxElements
		) const noexcept
	{
		if constexpr (!mcp_bUseFixedPoint)
		{
			o_nMaxElements = (umint)(_DensityBounds.m_UpperBound * pfp64(_Capacity));

			pfp64 LowerProduct = _DensityBounds.m_LowerBound * pfp64(_Capacity);
			o_nMinElements = (umint)LowerProduct;
			if (pfp64(o_nMinElements) < LowerProduct)
				++o_nMinElements;
		}
		else
		{
			o_nMinElements = fsp_ScaledMulCeil(_DensityBounds.m_LowerBound, _Capacity);
			o_nMaxElements = fsp_ScaledMulFloor(_DensityBounds.m_UpperBound, _Capacity);
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_GetLevelElementBounds
		(
			umint _iLevel
			, umint _nLevels
			, umint _Capacity
			, umint &o_nMinElements
			, umint &o_nMaxElements
		) const noexcept
	{
		fp_ElementBoundsFromDensity(fp_GetLevelDensityBounds(_iLevel, _nLevels), _Capacity, o_nMinElements, o_nMaxElements);
	}

	// Compute calibrator tree layout (binary tree of element counts)
	// Level 0: one entry per segment
	// Level k: ceil(level[k-1] / 2) entries, each storing sum of 2 children
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_ComputeCalibratorLayout(umint _nSegments) const noexcept -> CCalibratorLayout
	{
		CCalibratorLayout Layout = {};

		if (_nSegments == 0)
			return Layout;

		// Level 0 has one entry per segment
		Layout.m_LevelSizes[0] = _nSegments;
		Layout.m_LevelOffsets[0] = 0;
		Layout.m_nLevels = 1;

		// Build upper levels until we reach a single root entry
		umint PrevSize = _nSegments;
		umint Offset = _nSegments;

		while (PrevSize > 1 && Layout.m_nLevels < mcp_MaxCalibratorLevels)
		{
			// Next level size = ceil(prevSize / 2)
			umint NextSize = (PrevSize + 1) / 2;

			Layout.m_LevelSizes[Layout.m_nLevels] = NextSize;
			Layout.m_LevelOffsets[Layout.m_nLevels] = Offset;

			Offset += NextSize;
			PrevSize = NextSize;
			++Layout.m_nLevels;
		}

		Layout.m_nTotalEntries = Offset;
		return Layout;
	}

	// Calculate calibrator tree size in bytes (counts + level offsets)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_CalibratorTreeSizeBytes(umint _nSegments) const noexcept
	{
		if (_nSegments == 0)
			return 0;

		CCalibratorLayout Layout = fp_ComputeCalibratorLayout(_nSegments);

		// Size for all count entries
		umint Size = Layout.m_nTotalEntries * sizeof(CCalibratorCount);

		// Size for level offsets array
		Size = fg_AlignUp(Size, (umint)alignof(umint));
		Size += Layout.m_nLevels * sizeof(umint);

		return Size;
	}

	// Initialize calibrator counts from segment metadata (called after resize)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_InitializeCalibratorCounts(CPackedMapData *_pData) noexcept
	{
		auto const *pMeta = _pData->m_pSegmentMeta;
		CCalibratorCount *pCounts = _pData->m_pCalibratorCounts;
		umint const *pOffsets = _pData->m_pCalibratorOffsets;
		uint16 nLevels = _pData->m_nCalibratorTreeLevels;
		umint nSegments = _pData->m_nSegments;

		if (nLevels == 0)
			return;

		// Level 0: copy segment counts
		for (umint iSegment = 0; iSegment < nSegments; ++iSegment)
			pCounts[pOffsets[0] + iSegment] = pMeta[iSegment].m_Count;

		// Upper levels: sum children
		CCalibratorLayout Layout = fp_ComputeCalibratorLayout(nSegments);

		for (uint16 iLevel = 1; iLevel < nLevels; ++iLevel)
		{
			umint ThisSize = Layout.m_LevelSizes[iLevel];
			umint ChildSize = Layout.m_LevelSizes[iLevel - 1];
			umint iThisOffset = pOffsets[iLevel];
			umint iChildOffset = pOffsets[iLevel - 1];

			for (umint iEntry = 0; iEntry < ThisSize; ++iEntry)
			{
				umint iChild0 = iEntry * 2;
				umint iChild1 = iEntry * 2 + 1;

				CCalibratorCount nSum = pCounts[iChildOffset + iChild0];
				if (iChild1 < ChildSize)
					nSum += pCounts[iChildOffset + iChild1];

				pCounts[iThisOffset + iEntry] = nSum;
			}
		}
	}

	// Update calibrator counts for a range of segments
	// Complexity: O(W + W/2 + W/4 + ...) = O(W) for the window, O(log n) for ancestor path
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_UpdateCalibratorCountsRange(CPackedMapData *_pData, umint _iStartSeg, umint _iEndSeg) noexcept
	{
		auto const *pMeta = _pData->m_pSegmentMeta;
		CCalibratorCount *pCounts = _pData->m_pCalibratorCounts;
		umint const *pOffsets = _pData->m_pCalibratorOffsets;
		uint16 nLevels = _pData->m_nCalibratorTreeLevels;

		if (nLevels == 0)
			return;

		// Level 0: update leaf counts in range
		for (umint iSegment = _iStartSeg; iSegment < _iEndSeg; ++iSegment)
			pCounts[pOffsets[0] + iSegment] = pMeta[iSegment].m_Count;

		// Upper levels: update parent range that covers affected children
		CCalibratorLayout Layout = fp_ComputeCalibratorLayout(_pData->m_nSegments);

		umint iRangeStart = _iStartSeg;
		umint iRangeEnd = _iEndSeg;

		for (uint16 iLevel = 1; iLevel < nLevels; ++iLevel)
		{
			// Parent indices covering child range
			umint iParentStart = iRangeStart / 2;
			umint iParentEnd = (iRangeEnd + 1) / 2;

			umint ChildSize = Layout.m_LevelSizes[iLevel - 1];
			umint iChildOffset = pOffsets[iLevel - 1];
			umint iThisOffset = pOffsets[iLevel];

			for (umint iChild = iParentStart; iChild < iParentEnd; ++iChild)
			{
				umint iChild0 = iChild * 2;
				umint iChild1 = iChild * 2 + 1;

				CCalibratorCount nSum = pCounts[iChildOffset + iChild0];
				if (iChild1 < ChildSize)
					nSum += pCounts[iChildOffset + iChild1];

				pCounts[iThisOffset + iChild] = nSum;
			}

			// Shrink range for next level
			iRangeStart = iParentStart;
			iRangeEnd = iParentEnd;
		}
	}

	// Update calibrator counts when a segment count changes by delta
	// Updates path from leaf to root: O(log n)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_UpdateCalibratorCountOnDelta(CPackedMapData *_pData, umint _iSegment, int32 _nDelta) noexcept
	{
		CCalibratorCount *pCounts = _pData->m_pCalibratorCounts;
		umint const *pOffsets = _pData->m_pCalibratorOffsets;
		uint16 nLevels = _pData->m_nCalibratorTreeLevels;

		// Update path from leaf to root
		umint iIndex = _iSegment;
		for (uint16 iLevel = 0; iLevel < nLevels; ++iLevel)
		{
			umint iEntry = pOffsets[iLevel] + iIndex;

			if (_nDelta > 0)
				pCounts[iEntry] += (CCalibratorCount)_nDelta;
			else
				pCounts[iEntry] -= (CCalibratorCount)(-_nDelta);

			// Move to parent
			iIndex = iIndex / 2;
		}
	}

	// Get element count for a window at a given level
	// Window at level k starting at segment i has index = i / (2^k)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_GetCalibratorWindowCount
		(
			CPackedMapData const *_pData
			, umint _iLevel
			, umint _iWindowIndex
		)
		const noexcept
	{
		umint const *pOffsets = _pData->m_pCalibratorOffsets;
		CCalibratorCount const *pCounts = _pData->m_pCalibratorCounts;

		return pCounts[pOffsets[_iLevel] + _iWindowIndex];
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_RecordInsertion
		(
			CPackedMapData *_pData
			, umint _iSegment
			, t_CKey const *_pPrevKey
			, t_CKey const *_pNextKey
		)
		noexcept
	{
		if constexpr (!t_Options.m_bAdaptive)
			return;

		auto *pMeta = _pData->m_pSegmentMeta;
		auto &Meta = pMeta[_iSegment];
		auto *pKeys = _pData->m_pKeys;

		// Increment global time counter
		uint32 CurrentTime = ++_pData->m_TimeCounter;

		umint iQueuePos = Meta.m_iQueueHead;
		Meta.m_Timestamps[iQueuePos] = CurrentTime;
		Meta.m_iQueueHead = (uint8)((iQueuePos + 1) % mcp_DetectorQueueSize);
		if (Meta.m_QueueCount < mcp_DetectorQueueSize)
			++Meta.m_QueueCount;

		// Update sc counter (saturating): sc++ on insert
		if (Meta.m_NetBalance < (int16)mcp_NetBalanceCap)
			++Meta.m_NetBalance;
		Meta.m_LastOpSign = 1;

		// Algorithm 1 exclusive branch structure for k-counter updates:
		// - If succ_k (next key) matches kbwd.value: increment kbwd only
		// - Else if pred_k (prev key) matches kfwd.value: increment kfwd only
		// - Else: decrement both; if either hits 0, reset stored value and set counter = 1

		#if DMibDebug
		{
			if (pKeys)
			{
				if (_pPrevKey)
				{
					DMibCheck(_pPrevKey >= pKeys && _pPrevKey < pKeys + _pData->m_Capacity);
				}

				if (_pNextKey)
				{
					DMibCheck(_pNextKey >= pKeys && _pNextKey < pKeys + _pData->m_Capacity);
				}
			}
		}
		#endif

		auto fCheckMatch = [&](uint16 _KCount, auto const &_Stored, t_CKey const *_pKey) -> bool
			{
				if (_KCount == 0)
					return false;

				if (!_pKey)
					return !_Stored;

				if (!_Stored)
					return false;

				if constexpr (mcp_bDetectorStoresKeys)
					return mp_Compare(*_pKey, *_Stored) == 0;
				else
				{
					if (*_Stored < _pData->m_Capacity)
						return mp_Compare(*_pKey, pKeys[*_Stored]) == 0;

					return false;
				}
			}
		;

		auto fResetCounter = [&](uint16 &_KCount, auto &_Stored, t_CKey const *_pKey)
			{
				if (_pKey)
				{
					if constexpr (mcp_bDetectorStoresKeys)
						_Stored = *_pKey;
					else
						_Stored = _pKey - pKeys;

					_KCount = 1;
				}
				else
				{
					_Stored.f_Clear();
					_KCount = 1;
				}
			}
		;

		if constexpr (mcp_bDetectorStoresKeys)
		{
			bool bBackMatch = fCheckMatch(Meta.m_BackwardRunLength, Meta.m_DetectorKeys.m_BackwardRunLengthValue, _pNextKey);
			bool bForwMatch = fCheckMatch(Meta.m_ForwardRunLength, Meta.m_DetectorKeys.m_ForwardRunLengthValue, _pPrevKey);

			if (bBackMatch)
			{
				// succ_k matches kbwd.value: increment kbwd only
				if (Meta.m_BackwardRunLength < mcp_RunLengthCap)
					++Meta.m_BackwardRunLength;
			}
			else if (bForwMatch)
			{
				// pred_k matches kfwd.value: increment kfwd only
				if (Meta.m_ForwardRunLength < mcp_RunLengthCap)
					++Meta.m_ForwardRunLength;
			}
			else
			{
				// No match: decrement both counters
				if (Meta.m_BackwardRunLength > 0)
					--Meta.m_BackwardRunLength;
				if (Meta.m_ForwardRunLength > 0)
					--Meta.m_ForwardRunLength;

				// If counter hits 0, reset stored value and set counter = 1
				if (Meta.m_BackwardRunLength == 0)
					fResetCounter(Meta.m_BackwardRunLength, Meta.m_DetectorKeys.m_BackwardRunLengthValue, _pNextKey);
				if (Meta.m_ForwardRunLength == 0)
					fResetCounter(Meta.m_ForwardRunLength, Meta.m_DetectorKeys.m_ForwardRunLengthValue, _pPrevKey);
			}
		}
		else
		{
			bool bBackMatch = fCheckMatch(Meta.m_BackwardRunLength, Meta.m_DetectorKeys.m_BackwardRunLengthSlot, _pNextKey);
			bool bForwMatch = fCheckMatch(Meta.m_ForwardRunLength, Meta.m_DetectorKeys.m_ForwardRunLengthSlot, _pPrevKey);

			if (bBackMatch)
			{
				// succ_k matches kbwd.value: increment kbwd only
				if (Meta.m_BackwardRunLength < mcp_RunLengthCap)
					++Meta.m_BackwardRunLength;
			}
			else if (bForwMatch)
			{
				// pred_k matches kfwd.value: increment kfwd only
				if (Meta.m_ForwardRunLength < mcp_RunLengthCap)
					++Meta.m_ForwardRunLength;
			}
			else
			{
				// No match: decrement both counters
				if (Meta.m_BackwardRunLength > 0)
					--Meta.m_BackwardRunLength;
				if (Meta.m_ForwardRunLength > 0)
					--Meta.m_ForwardRunLength;

				// If counter hits 0, reset stored value and set counter = 1
				if (Meta.m_BackwardRunLength == 0)
					fResetCounter(Meta.m_BackwardRunLength, Meta.m_DetectorKeys.m_BackwardRunLengthSlot, _pNextKey);
				if (Meta.m_ForwardRunLength == 0)
					fResetCounter(Meta.m_ForwardRunLength, Meta.m_DetectorKeys.m_ForwardRunLengthSlot, _pPrevKey);
			}
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_RecordDeletion
		(
			CPackedMapData *_pData
			, umint _iSegment
			, t_CKey const *_pPrevKey
			, t_CKey const *_pNextKey
		)
		noexcept
	{
		if constexpr (!t_Options.m_bAdaptive)
			return;

		auto *pMeta = _pData->m_pSegmentMeta;
		auto &Meta = pMeta[_iSegment];
		auto *pKeys = _pData->m_pKeys;

		uint32 CurrentTime = ++_pData->m_TimeCounter;
		umint iQueuePos = Meta.m_iQueueHead;
		Meta.m_Timestamps[iQueuePos] = CurrentTime;
		Meta.m_iQueueHead = (uint8)((iQueuePos + 1) % mcp_DetectorQueueSize);
		if (Meta.m_QueueCount < mcp_DetectorQueueSize)
			++Meta.m_QueueCount;

		// sc-- (saturating) - deletions only affect the signed counter
		if (Meta.m_NetBalance > -(int16)mcp_NetBalanceCap)
			--Meta.m_NetBalance;
		Meta.m_LastOpSign = -1;

#if DMibDebug
		if (pKeys)
		{
			if (_pPrevKey)
			{
				DMibCheck(_pPrevKey >= pKeys && _pPrevKey < pKeys + _pData->m_Capacity);
			}

			if (_pNextKey)
			{
				DMibCheck(_pNextKey >= pKeys && _pNextKey < pKeys + _pData->m_Capacity);
			}
		}
#endif

		auto fCheckMatch = [&](uint16 _KCount, auto const &_Stored, t_CKey const *_pKey) -> bool
			{
				if (_KCount == 0)
					return false;

				if (!_pKey)
					return !_Stored;

				if (!_Stored)
					return false;

				if constexpr (mcp_bDetectorStoresKeys)
					return mp_Compare(*_pKey, *_Stored) == 0;
				else
				{
					if (*_Stored < _pData->m_Capacity)
						return mp_Compare(*_pKey, pKeys[*_Stored]) == 0;

					return false;
				}
			}
		;

		auto fResetCounter = [&](uint16 &_KCount, auto &_Stored, t_CKey const *_pKey)
			{
				if (_pKey)
				{
					if constexpr (mcp_bDetectorStoresKeys)
						_Stored = *_pKey;
					else
						_Stored = _pKey - pKeys;

					_KCount = 1;
				}
				else
				{
					_Stored.f_Clear();
					_KCount = 1;
				}
			}
		;

		if constexpr (mcp_bDetectorStoresKeys)
		{
			bool bBackMatch = fCheckMatch(Meta.m_BackwardRunLength, Meta.m_DetectorKeys.m_BackwardRunLengthValue, _pNextKey);
			bool bForwMatch = fCheckMatch(Meta.m_ForwardRunLength, Meta.m_DetectorKeys.m_ForwardRunLengthValue, _pPrevKey);

			if (bBackMatch)
			{
				if (Meta.m_BackwardRunLength < mcp_RunLengthCap)
					++Meta.m_BackwardRunLength;
			}
			else if (bForwMatch)
			{
				if (Meta.m_ForwardRunLength < mcp_RunLengthCap)
					++Meta.m_ForwardRunLength;
			}
			else
			{
				if (Meta.m_BackwardRunLength > 0)
					--Meta.m_BackwardRunLength;
				if (Meta.m_ForwardRunLength > 0)
					--Meta.m_ForwardRunLength;

				if (Meta.m_BackwardRunLength == 0)
					fResetCounter(Meta.m_BackwardRunLength, Meta.m_DetectorKeys.m_BackwardRunLengthValue, _pNextKey);
				if (Meta.m_ForwardRunLength == 0)
					fResetCounter(Meta.m_ForwardRunLength, Meta.m_DetectorKeys.m_ForwardRunLengthValue, _pPrevKey);
			}
		}
		else
		{
			bool bBackMatch = fCheckMatch(Meta.m_BackwardRunLength, Meta.m_DetectorKeys.m_BackwardRunLengthSlot, _pNextKey);
			bool bForwMatch = fCheckMatch(Meta.m_ForwardRunLength, Meta.m_DetectorKeys.m_ForwardRunLengthSlot, _pPrevKey);

			if (bBackMatch)
			{
				if (Meta.m_BackwardRunLength < mcp_RunLengthCap)
					++Meta.m_BackwardRunLength;
			}
			else if (bForwMatch)
			{
				if (Meta.m_ForwardRunLength < mcp_RunLengthCap)
					++Meta.m_ForwardRunLength;
			}
			else
			{
				if (Meta.m_BackwardRunLength > 0)
					--Meta.m_BackwardRunLength;
				if (Meta.m_ForwardRunLength > 0)
					--Meta.m_ForwardRunLength;

				if (Meta.m_BackwardRunLength == 0)
					fResetCounter(Meta.m_BackwardRunLength, Meta.m_DetectorKeys.m_BackwardRunLengthSlot, _pNextKey);
				if (Meta.m_ForwardRunLength == 0)
					fResetCounter(Meta.m_ForwardRunLength, Meta.m_DetectorKeys.m_ForwardRunLengthSlot, _pPrevKey);
			}
		}
	}

	// Reset detector state for a segment after rebalancing
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_ResetDetectorState(CPackedMapData *_pData, umint _iSegment)
	{
		auto &Meta = _pData->m_pSegmentMeta[_iSegment];
		Meta.m_BackwardRunLength = 0;
		Meta.m_ForwardRunLength = 0;
		Meta.m_iQueueHead = 0;
		Meta.m_QueueCount = 0;
		Meta.m_NetBalance = 0;
		Meta.m_LastOpSign = 0;
		if constexpr (mcp_bDetectorStoresKeys)
		{
			Meta.m_DetectorKeys.m_BackwardRunLengthValue.f_Clear();
			Meta.m_DetectorKeys.m_ForwardRunLengthValue.f_Clear();
		}
		else
		{
			Meta.m_DetectorKeys.m_BackwardRunLengthSlot.f_Clear();
			Meta.m_DetectorKeys.m_ForwardRunLengthSlot.f_Clear();
		}

		for (umint iTimestamp = 0; iTimestamp < mcp_DetectorQueueSize; ++iTimestamp)
			Meta.m_Timestamps[iTimestamp] = 0;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_CopyDetectorStateScaled
		(
			CSegmentMeta &_Out
			, CSegmentMeta const &_In
			, umint _Overlap
			, umint _SourceCount
		)
	{
		_Out.m_BackwardRunLength = 0;
		_Out.m_ForwardRunLength = 0;
		_Out.m_iQueueHead = 0;
		_Out.m_QueueCount = 0;
		_Out.m_NetBalance = 0;
		_Out.m_LastOpSign = 0;

		if constexpr (mcp_bDetectorStoresKeys)
		{
			_Out.m_DetectorKeys.m_BackwardRunLengthValue.f_Clear();
			_Out.m_DetectorKeys.m_ForwardRunLengthValue.f_Clear();
		}
		else
		{
			_Out.m_DetectorKeys.m_BackwardRunLengthSlot.f_Clear();
			_Out.m_DetectorKeys.m_ForwardRunLengthSlot.f_Clear();
		}

		for (umint iTimestamp = 0; iTimestamp < mcp_DetectorQueueSize; ++iTimestamp)
			_Out.m_Timestamps[iTimestamp] = 0;

		if (_Overlap <= 0 || _SourceCount <= 0)
			return;

		auto fScaleUnsigned = [&](uint16 _Value) -> uint16
			{
				return (uint16)(((umint)_Value * _Overlap) / _SourceCount);
			}
		;

		auto fScaleSigned = [&](int16 _Value) -> int16
			{
				if (_Value == 0)
					return 0;

				umint AbsScaled = ((umint)fg_Abs((int)_Value) * _Overlap) / _SourceCount;
				if (AbsScaled == 0)
					return 0;

				return (int16)(_Value > 0 ? AbsScaled : -AbsScaled);
			}
		;

		_Out.m_BackwardRunLength = fScaleUnsigned(_In.m_BackwardRunLength);
		_Out.m_ForwardRunLength = fScaleUnsigned(_In.m_ForwardRunLength);
		_Out.m_NetBalance = fScaleSigned(_In.m_NetBalance);
		_Out.m_LastOpSign = (_Out.m_NetBalance > 0) ? 1 : (_Out.m_NetBalance < 0 ? -1 : 0);

		if constexpr (mcp_bDetectorStoresKeys)
		{
			if (_Out.m_BackwardRunLength > 0 && _In.m_DetectorKeys.m_BackwardRunLengthValue)
				_Out.m_DetectorKeys.m_BackwardRunLengthValue = *_In.m_DetectorKeys.m_BackwardRunLengthValue;
			if (_Out.m_ForwardRunLength > 0 && _In.m_DetectorKeys.m_ForwardRunLengthValue)
				_Out.m_DetectorKeys.m_ForwardRunLengthValue = *_In.m_DetectorKeys.m_ForwardRunLengthValue;
		}
		else
		{
			if (_Out.m_BackwardRunLength > 0 && _In.m_DetectorKeys.m_BackwardRunLengthSlot)
				_Out.m_DetectorKeys.m_BackwardRunLengthSlot = *_In.m_DetectorKeys.m_BackwardRunLengthSlot;
			if (_Out.m_ForwardRunLength > 0 && _In.m_DetectorKeys.m_ForwardRunLengthSlot)
				_Out.m_DetectorKeys.m_ForwardRunLengthSlot = *_In.m_DetectorKeys.m_ForwardRunLengthSlot;
		}

		// Preserve only a compact recent sketch across redistribution.
		// Carrying the full queue forward makes detector history effectively immortal.
		constexpr umint c_nRemapTimestampCap = 8;
		umint QueueScaled = ((umint)_In.m_QueueCount * _Overlap) / _SourceCount;
		umint nQueueCopy = fg_Min((umint)_In.m_QueueCount, fg_Min((umint)c_nRemapTimestampCap, QueueScaled));
		if (nQueueCopy <= 0)
			return;

		umint iStart = (_In.m_iQueueHead + _In.m_QueueCount - nQueueCopy) % mcp_DetectorQueueSize;
		for (umint iTimestamp = 0; iTimestamp < nQueueCopy; ++iTimestamp)
			_Out.m_Timestamps[iTimestamp] = _In.m_Timestamps[(iStart + iTimestamp) % mcp_DetectorQueueSize];

		_Out.m_QueueCount = (uint8)nQueueCopy;
		_Out.m_iQueueHead = (uint8)(nQueueCopy % mcp_DetectorQueueSize);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_IsSegmentMarked(CPackedMapData const *_pData, umint _iSegment) const noexcept
	{
		auto const &Meta = _pData->m_pSegmentMeta[_iSegment];

		// Using θSC for k counters (paper spec)
		return Meta.m_BackwardRunLength > mcp_RunLengthHotnessThreshold || Meta.m_ForwardRunLength > mcp_RunLengthHotnessThreshold;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_FindMarkedIntervals
		(
			CPackedMapData const *_pData
			, umint _iStartSeg
			, umint _iEndSeg
			, CMarkedIntervalVector &_OutIntervals
		)
	{
		_OutIntervals.f_Clear();

		auto const *pMeta = _pData->m_pSegmentMeta;

		umint nSegments = _iEndSeg - _iStartSeg;
		if (nSegments == 0)
			return;

		auto const *pKeys = _pData->m_pKeys;

		// Collect all timestamps from segments in the window for percentile calculation
		// Use static allocator to avoid heap for small windows (covers ~128 segments)
		TCVector<uint32, NMemory::TCAllocator_Static<128 * mcp_DetectorQueueSize * sizeof(uint32), 16, CScratchAllocator>> AllTimestamps;
		AllTimestamps.f_Reserve(nSegments * mcp_DetectorQueueSize);
		uint32 MaxTimestamp = 0;

		for (umint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
		{
			auto const &Meta = pMeta[iSeg];
			for (umint iTimestamp = 0; iTimestamp < Meta.m_QueueCount; ++iTimestamp)
			{
				if constexpr (t_Options.m_bStats)
					MaxTimestamp = fg_Max(MaxTimestamp, Meta.m_Timestamps[iTimestamp]);
				AllTimestamps.f_InsertLast(Meta.m_Timestamps[iTimestamp]);
			}
		}

		if (AllTimestamps.f_GetLen() < 4)
		{
			if constexpr (t_Options.m_bStats)
				fp_StatsAddAdaptiveWindow(AllTimestamps.f_GetLen(), 0, 0, 0, 0, 0, false);
			return;
		}

		// Compute a high percentile using linear-time selection: O(QW).
		// Avoid picking the absolute maximum for typical window sizes, otherwise
		// the ">= percentile" hotness test degenerates and hot segments vanish.
		umint nTimestamps = AllTimestamps.f_GetLen();
		umint iPercentileIdx = (nTimestamps * 9) / 10;  // ~90th percentile
		if (iPercentileIdx >= nTimestamps - 1)
			iPercentileIdx = nTimestamps - 2;
		AllTimestamps.f_Sort();
		uint32 Percentile = AllTimestamps[iPercentileIdx];

		// Hotness test: a segment is hot if it contains any very recent timestamp.
		// After redistribution, queues are intentionally compact sketches, so a
		// "fraction of queue" rule becomes too strict and suppresses hotspots entirely.
		auto fIsSegmentHot = [&](umint _iSeg) -> bool
			{
				auto const &Meta = pMeta[_iSeg];
				if (Meta.m_QueueCount == 0)
					return false;

				umint nAbovePercentile = 0;
				for (umint iTimestamp = 0; iTimestamp < Meta.m_QueueCount; ++iTimestamp)
				{
					if (Meta.m_Timestamps[iTimestamp] >= Percentile)
						++nAbovePercentile;
				}

				return nAbovePercentile > 0;
			}
		;

		if constexpr (t_Options.m_bStats)
		{
			umint nSegmentsWithTimestamps = 0;
			umint nSegmentsAnyAbovePercentile = 0;
			umint nSegmentsHalfAbovePercentile = 0;
			umint nSegmentsThreeQuarterAbovePercentile = 0;
			umint nHotSegments = 0;
			for (umint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
			{
				auto const &Meta = pMeta[iSeg];
				if (Meta.m_QueueCount > 0)
				{
					++nSegmentsWithTimestamps;

					umint nAbovePercentile = 0;
					for (umint iTimestamp = 0; iTimestamp < Meta.m_QueueCount; ++iTimestamp)
					{
						if (Meta.m_Timestamps[iTimestamp] >= Percentile)
							++nAbovePercentile;
					}

					if (nAbovePercentile > 0)
						++nSegmentsAnyAbovePercentile;
					if (nAbovePercentile * 2 >= Meta.m_QueueCount)
						++nSegmentsHalfAbovePercentile;
					if (nAbovePercentile * 4 >= Meta.m_QueueCount * 3)
						++nSegmentsThreeQuarterAbovePercentile;
				}

				if (fIsSegmentHot(iSeg))
					++nHotSegments;
			}
			fp_StatsAddAdaptiveWindow
				(
					AllTimestamps.f_GetLen()
					, nSegmentsWithTimestamps
					, nSegmentsAnyAbovePercentile
					, nSegmentsHalfAbovePercentile
					, nSegmentsThreeQuarterAbovePercentile
					, nHotSegments
					, Percentile == MaxTimestamp
				)
			;
		}

		umint nTwoElementIntervals = 0;
		umint nWholeSegmentIntervals = 0;
		umint nBackwardRunLengthTriggered = 0;
		umint nForwardRunLengthTriggered = 0;
		umint nTwoElementIntervalFailures = 0;

		// Precompute cumulative element counts: O(W)
		TCVector<umint, NMemory::TCAllocator_Static<(128 + 1) * sizeof(umint), 16, CScratchAllocator>> CumulativeCounts;
		CumulativeCounts.f_Reserve(nSegments + 1);
		CumulativeCounts.f_InsertLast((umint)0);
		for (umint iSegment = 0; iSegment < nSegments; ++iSegment)
			CumulativeCounts.f_InsertLast((umint)(CumulativeCounts[iSegment] + pMeta[_iStartSeg + iSegment].m_Count));

		// O(1) lookup using prefix sums
		auto fGetWindowElementOffset = [&](umint _iSeg) -> umint
			{
				umint iRelSeg = _iSeg - _iStartSeg;
				if (iRelSeg < 0 || iRelSeg >= nSegments)
					return CumulativeCounts[nSegments];  // Total elements
				return CumulativeCounts[iRelSeg];
			}
		;

		// O(log n) lookup using fp_Find + prefix sums
		auto fFindKeyElementPos = [&](t_CKey const &_Key) -> umint
			{
				auto FindResult = fsp_Find(_pData, mp_Compare, _Key);
				if (!FindResult.m_bExists)
					return -1;

				// Check if found key is within our window
				if (FindResult.m_iSegment < _iStartSeg || FindResult.m_iSegment >= _iEndSeg)
					return -1;

				// Element position = prefix sum up to segment + local position
				umint iRelSeg = FindResult.m_iSegment - _iStartSeg;
				return CumulativeCounts[iRelSeg] + FindResult.m_iLocalPos;
			}
		;

		for (umint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
		{
			auto const &Meta = pMeta[iSeg];
			bool bHotByPercentile = fIsSegmentHot(iSeg);
			bool bHotByCounters = (Meta.m_BackwardRunLength >= mcp_RunLengthHotnessThreshold)
				|| (Meta.m_ForwardRunLength >= mcp_RunLengthHotnessThreshold)
				|| (fg_Abs((int)Meta.m_NetBalance) >= (int)mcp_RunLengthHotnessThreshold)
			;

			if (!bHotByPercentile && !bHotByCounters)
				continue;

			if (!bHotByPercentile && fg_Abs((int)Meta.m_NetBalance) < (int)mcp_RunLengthHotnessThreshold)
				continue;

			umint Count = Meta.m_Count;
			if (Count == 0)
				continue;

			// Per paper: signed score from sc counter sign
			smint SignedScore = (Meta.m_NetBalance > 0) ? (smint)1 : (Meta.m_NetBalance < 0 ? (smint)-1 : (Meta.m_LastOpSign >= 0 ? (smint)1 : (smint)-1));

			// Compute element range for this segment within the window
			umint iSegElemStart = fGetWindowElementOffset(iSeg);
			umint iSegElemEnd = iSegElemStart + Count;

			// Paper Algorithm 1: check if kbwd or kfwd exceeds θSC
			// If so, create a 2-element interval using the stored predecessor/successor key
			// Otherwise, emit the whole segment's element range

			bool bBackwardRunLengthTriggered = Meta.m_BackwardRunLength >= mcp_RunLengthHotnessThreshold;
			bool bForwardRunLengthTriggered = Meta.m_ForwardRunLength >= mcp_RunLengthHotnessThreshold;
			if constexpr (t_Options.m_bStats)
			{
				if (bBackwardRunLengthTriggered)
					++nBackwardRunLengthTriggered;
				if (bForwardRunLengthTriggered)
					++nForwardRunLengthTriggered;
			}

			umint iIntStart = iSegElemStart;
			umint iIntEnd = iSegElemEnd;

			if (bBackwardRunLengthTriggered || bForwardRunLengthTriggered)
			{
				// Try to create a 2-element interval based on stored keys
				bool bCreated2ElemInterval = false;

				if constexpr (mcp_bDetectorStoresKeys)
				{
					// kbwd stores the successor key (next key after inserted element)
					if (bBackwardRunLengthTriggered && Meta.m_DetectorKeys.m_BackwardRunLengthValue)
					{
						umint iKeyPos = fFindKeyElementPos(*Meta.m_DetectorKeys.m_BackwardRunLengthValue);
						if (iKeyPos >= 0 && iKeyPos > 0)
						{
							// 2-element interval: [predecessor, successor]
							iIntStart = iKeyPos - 1;
							iIntEnd = iKeyPos + 1;
							bCreated2ElemInterval = true;
						}
					}
					else if (bBackwardRunLengthTriggered && !Meta.m_DetectorKeys.m_BackwardRunLengthValue)
					{
						// Stable "no successor" boundary: hotspot is at the right edge.
						iIntStart = (iSegElemEnd > iSegElemStart + 1) ? (iSegElemEnd - 2) : iSegElemStart;
						iIntEnd = iSegElemEnd;
						bCreated2ElemInterval = true;
					}
					// kfwd stores the predecessor key (prev key before inserted element)
					else if (bForwardRunLengthTriggered && Meta.m_DetectorKeys.m_ForwardRunLengthValue)
					{
						umint iKeyPos = fFindKeyElementPos(*Meta.m_DetectorKeys.m_ForwardRunLengthValue);
						if (iKeyPos >= 0)
						{
							// 2-element interval: [predecessor, successor]
							umint nTotalElements = fGetWindowElementOffset(_iEndSeg);
							iIntStart = iKeyPos;
							iIntEnd = fg_Min(iKeyPos + 2, nTotalElements);
							bCreated2ElemInterval = true;
						}
					}
					else if (bForwardRunLengthTriggered && !Meta.m_DetectorKeys.m_ForwardRunLengthValue)
					{
						// Stable "no predecessor" boundary: hotspot is at the left edge.
						umint nTotalElements = fGetWindowElementOffset(_iEndSeg);
						iIntStart = iSegElemStart;
						iIntEnd = fg_Min(iSegElemStart + 2, nTotalElements);
						bCreated2ElemInterval = true;
					}
				}
				else
				{
					// Slot-based storage: convert slot to element position
					if (bBackwardRunLengthTriggered && Meta.m_DetectorKeys.m_BackwardRunLengthSlot && *Meta.m_DetectorKeys.m_BackwardRunLengthSlot < _pData->m_Capacity)
					{
						t_CKey const &Key = pKeys[*Meta.m_DetectorKeys.m_BackwardRunLengthSlot];
						umint iKeyPos = fFindKeyElementPos(Key);
						if (iKeyPos >= 0 && iKeyPos > 0)
						{
							iIntStart = iKeyPos - 1;
							iIntEnd = iKeyPos + 1;
							bCreated2ElemInterval = true;
						}
					}
					else if (bBackwardRunLengthTriggered && !Meta.m_DetectorKeys.m_BackwardRunLengthSlot)
					{
						iIntStart = (iSegElemEnd > iSegElemStart + 1) ? (iSegElemEnd - 2) : iSegElemStart;
						iIntEnd = iSegElemEnd;
						bCreated2ElemInterval = true;
					}
					else if (bForwardRunLengthTriggered && Meta.m_DetectorKeys.m_ForwardRunLengthSlot && *Meta.m_DetectorKeys.m_ForwardRunLengthSlot < _pData->m_Capacity)
					{
						t_CKey const &Key = pKeys[*Meta.m_DetectorKeys.m_ForwardRunLengthSlot];
						umint iKeyPos = fFindKeyElementPos(Key);
						if (iKeyPos >= 0)
						{
							umint nTotalElements = fGetWindowElementOffset(_iEndSeg);
							iIntStart = iKeyPos;
							iIntEnd = fg_Min(iKeyPos + 2, nTotalElements);
							bCreated2ElemInterval = true;
						}
					}
					else if (bForwardRunLengthTriggered && !Meta.m_DetectorKeys.m_ForwardRunLengthSlot)
					{
						umint nTotalElements = fGetWindowElementOffset(_iEndSeg);
						iIntStart = iSegElemStart;
						iIntEnd = fg_Min(iSegElemStart + 2, nTotalElements);
						bCreated2ElemInterval = true;
					}
				}

				// If we couldn't create a 2-element interval, fall back to whole segment
				if (!bCreated2ElemInterval)
				{
					if constexpr (t_Options.m_bStats)
						++nTwoElementIntervalFailures;
					iIntStart = iSegElemStart;
					iIntEnd = iSegElemEnd;
				}
			}

			if (iIntEnd > iIntStart)
			{
				if constexpr (t_Options.m_bStats)
				{
					if (iIntEnd - iIntStart <= 2)
						++nTwoElementIntervals;
					else
						++nWholeSegmentIntervals;
				}
				_OutIntervals.f_InsertLast(CMarkedInterval{iIntStart, iIntEnd, SignedScore});
			}
		}

		if constexpr (t_Options.m_bStats)
			fp_StatsAddAdaptiveIntervalKinds(nTwoElementIntervals, nWholeSegmentIntervals, nBackwardRunLengthTriggered, nForwardRunLengthTriggered, nTwoElementIntervalFailures);

		// Merge overlapping intervals
		auto nOutIntervals = _OutIntervals.f_GetLen();
		auto *pOutIntervals = _OutIntervals.f_GetArray();
		if (nOutIntervals > 1)
		{
			// Sort by start element position
			_OutIntervals.f_Sort([](CMarkedInterval const &_a, CMarkedInterval const &_b) { return _a.m_iStartElem <=> _b.m_iStartElem; });

			CMarkedIntervalVector Merged;
			auto *pLastMerged = &Merged.f_InsertLast(pOutIntervals[0]);

			for (umint iInterval = 1; iInterval < nOutIntervals; ++iInterval)
			{
				auto &Last = *pLastMerged;
				auto const &Current = pOutIntervals[iInterval];

				if (Current.m_iStartElem <= Last.m_iEndElem)
				{
					// Merge overlapping intervals - sum signed scores per paper
					Last.m_iEndElem = fg_Max(Last.m_iEndElem, Current.m_iEndElem);
					Last.m_SignedScore += Current.m_SignedScore;
				}
				else
					pLastMerged = &Merged.f_InsertLast(Current);
			}

			_OutIntervals = fg_Move(Merged);
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr inline_never void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_AdaptiveRedistribute
		(
			CPackedMapData *_pData
			, umint _iStartSeg
			, umint _iEndSeg
			, umint _iWindowLevel
			, umint _nGlobalLevels
		)
	{
		if constexpr (t_Options.m_bStats)
			fp_StatsAddAdaptiveRedistribute();

		auto *pData = _pData;

		auto *pMeta = pData->m_pSegmentMeta;
		auto *pKeys = pData->m_pKeys;
		auto *pValues = pData->m_pValues;

		umint nSegments = _iEndSeg - _iStartSeg;
		if (nSegments == 0)
			return;

		// Count total elements
		umint nTotalElements = 0;
		for (umint iSegment = 0; iSegment < nSegments; ++iSegment)
		{
			umint iSeg = _iStartSeg + iSegment;
			nTotalElements += pMeta[iSeg].m_Count;
		}

		if (nTotalElements == 0)
			return;

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

		umint *pOldPrefix = nullptr;
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

		CMarkedIntervalVector MarkedIntervals;
		fp_FindMarkedIntervals(pData, _iStartSeg, _iEndSeg, MarkedIntervals);

		// Debug stats for marking behavior
		if constexpr (t_Options.m_bStats)
		{
			umint nIntervalElements = 0;
			for (auto &Interval : MarkedIntervals)
				nIntervalElements += fg_Max((umint)0, Interval.m_iEndElem - Interval.m_iStartElem);

			umint nMarkedSegments = 0;
			for (umint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
			{
				auto const &Meta = pMeta[iSeg];

				bool bMarked = (Meta.m_BackwardRunLength >= mcp_RunLengthHotnessThreshold)
					|| (Meta.m_ForwardRunLength >= mcp_RunLengthHotnessThreshold)
					|| (fg_Abs((int)Meta.m_NetBalance) >= (int)mcp_RunLengthHotnessThreshold)
				;

				if (bMarked && Meta.m_Count > 0)
					++nMarkedSegments;
			}

			fp_StatsAddMarkedIntervals(MarkedIntervals.f_GetLen(), nIntervalElements, nMarkedSegments);
		}

		// Interval query helpers for O(log I) range queries
		// Allocate all 6 arrays from a single buffer (stack for small counts)
		umint IntervalCount = MarkedIntervals.f_GetLen();

		// Layout: [Starts: umint*I] [Ends: umint*I] [PrefixLen: umint*I] [PrefixScore: umint*I] [WeightedPrefixScore: umint*I] [SignedScore: umint*I]
		umint nIntMintBytes = IntervalCount * sizeof(umint);
		umint nIntMintAligned = fg_AlignUp(nIntMintBytes, c_nBufferAlign);
		umint IntervalBufferSize = 6 * nIntMintAligned;

		constexpr umint c_nIntervalStackThreshold = 4096;
		alignas(c_nBufferAlign) uint8 aIntervalStackBuffer[c_nIntervalStackThreshold];

		uint8 *pIntervalBuffer = nullptr;
		umint IntervalAllocSize = 0;
		if (IntervalCount > 0)
		{
			if (IntervalBufferSize <= c_nIntervalStackThreshold)
				pIntervalBuffer = aIntervalStackBuffer;
			else
			{
				pIntervalBuffer = (uint8 *)mp_Allocator.f_AllocAligned(IntervalBufferSize, c_nBufferAlign);
				IntervalAllocSize = IntervalBufferSize;
			}
		}

		if constexpr (t_Options.m_bStats)
		{
			if (IntervalCount > 0)
				fp_StatsAddRebalanceAlloc(IntervalAllocSize > 0);
		}

		auto CleanupIntervalBuffer = g_OnScopeExit / [&]
			{
				if (IntervalAllocSize > 0)
					mp_Allocator.f_Free(pIntervalBuffer, IntervalAllocSize);
			}
		;

		umint *pIntervalStarts = IntervalCount > 0 ? (umint *)pIntervalBuffer : nullptr;
		umint *pIntervalEnds = IntervalCount > 0 ? (umint *)(pIntervalBuffer + nIntMintAligned) : nullptr;
		umint *pIntervalPrefixLen = IntervalCount > 0 ? (umint *)(pIntervalBuffer + 2 * nIntMintAligned) : nullptr;
		smint *pIntervalPrefixScore = IntervalCount > 0 ? (smint *)(pIntervalBuffer + 3 * nIntMintAligned) : nullptr;
		smint *pIntervalWeightedPrefixScore = IntervalCount > 0 ? (smint *)(pIntervalBuffer + 4 * nIntMintAligned) : nullptr;
		smint *pIntervalSignedScore = IntervalCount > 0 ? (smint *)(pIntervalBuffer + 5 * nIntMintAligned) : nullptr;

		if (IntervalCount > 0)
		{
			umint nLenSum = 0;
			smint ScoreSum = 0;
			smint WeightedScoreSum = 0;

			for (umint iInterval = 0; iInterval < IntervalCount; ++iInterval)
			{
				auto const &Int = MarkedIntervals[iInterval];
				umint nLen = fg_Max((umint)0, Int.m_iEndElem - Int.m_iStartElem);
				pIntervalStarts[iInterval] = Int.m_iStartElem;
				pIntervalEnds[iInterval] = Int.m_iEndElem;
				pIntervalSignedScore[iInterval] = Int.m_SignedScore;

				nLenSum += nLen;
				ScoreSum += Int.m_SignedScore;
				WeightedScoreSum += Int.m_SignedScore * nLen;
				pIntervalPrefixLen[iInterval] = nLenSum;
				pIntervalPrefixScore[iInterval] = ScoreSum;
				pIntervalWeightedPrefixScore[iInterval] = WeightedScoreSum;
			}
		}

		// Binary search helpers for sorted interval arrays
		auto fLowerBound = [](umint const *_pArray, umint _nLen, umint _Value) -> umint
			{
				umint iLow = 0;
				umint iHigh = _nLen;

				while (iLow < iHigh)
				{
					umint iMid = iLow + (iHigh - iLow) / 2;
					if (_pArray[iMid] < _Value)
						iLow = iMid + 1;
					else
						iHigh = iMid;
				}

				return iLow;
			}
		;

		auto fUpperBound = [](umint const *_pArray, umint _nLen, umint _Value) -> umint
			{
				umint iLow = 0;
				umint iHigh = _nLen;

				while (iLow < iHigh)
				{
					umint iMid = iLow + (iHigh - iLow) / 2;
					if (_pArray[iMid] <= _Value)
						iLow = iMid + 1;
					else
						iHigh = iMid;
				}

				return iLow;
			}
		;

		auto fIntervalOverlapLen = [&](umint _iInterval, umint _iStart, umint _iEnd) -> umint
			{
				umint iOverlapStart = fg_Max(_iStart, pIntervalStarts[_iInterval]);
				umint iOverlapEnd = fg_Min(_iEnd, pIntervalEnds[_iInterval]);

				return (iOverlapEnd > iOverlapStart) ? (iOverlapEnd - iOverlapStart) : 0;
			}
		;

		// Returns (nOverlapElements, element-weighted score) — score uses SignedScore * overlap for edges
		// and weighted prefix sums for full intervals, all in element-weighted integer units.
		auto fIntervalRangeQuery = [&](umint _iStart, umint _iEnd) -> NStorage::TCTuple<umint, smint>
			{
				if (IntervalCount == 0 || _iEnd <= _iStart)
					return {0, 0};

				umint nIntervals = IntervalCount;
				auto *pStarts = pIntervalStarts;
				auto *pEnds = pIntervalEnds;

				umint iFirst = fUpperBound(pEnds, nIntervals, _iStart);
				umint iLast = fLowerBound(pStarts, nIntervals, _iEnd) - 1;

				if (iFirst > iLast || iFirst >= nIntervals || iLast < 0)
					return {0, 0};

				iFirst = fg_Max((umint)0, iFirst);
				iLast = fg_Min(nIntervals - 1, iLast);

				umint nTotalLen = 0;
				smint TotalScore = 0;

				if (iFirst == iLast)
				{
					umint nOverlap = fIntervalOverlapLen(iFirst, _iStart, _iEnd);
					nTotalLen = nOverlap;
					TotalScore = pIntervalSignedScore[iFirst] * (smint)nOverlap;
					return {nTotalLen, TotalScore};
				}

				umint nOverlapFirst = fIntervalOverlapLen(iFirst, _iStart, _iEnd);
				umint nOverlapLast = fIntervalOverlapLen(iLast, _iStart, _iEnd);
				nTotalLen += nOverlapFirst + nOverlapLast;
				TotalScore += pIntervalSignedScore[iFirst] * (smint)nOverlapFirst;
				TotalScore += pIntervalSignedScore[iLast] * (smint)nOverlapLast;

				if (iFirst + 1 <= iLast - 1)
				{
					umint iFullStart = iFirst + 1;
					umint iFullEnd = iLast - 1;

					umint nLenBefore = (iFullStart > 0) ? pIntervalPrefixLen[iFullStart - 1] : 0;
					umint nLenFull = pIntervalPrefixLen[iFullEnd] - nLenBefore;
					nTotalLen += nLenFull;

					smint WeightedScoreBefore = (iFullStart > 0) ? pIntervalWeightedPrefixScore[iFullStart - 1] : 0;
					smint WeightedScoreFull = pIntervalWeightedPrefixScore[iFullEnd] - WeightedScoreBefore;
					TotalScore += WeightedScoreFull;
				}

				return {nTotalLen, TotalScore};
			}
		;

		// Returns (nIntervalsBeforeSplit, interval-count-weighted score)
		auto fIntervalCountScoreBefore = [&](umint _iStart, umint _iEnd, umint _iSplit) -> NStorage::TCTuple<umint, smint>
			{
				umint nIntervals = IntervalCount;
				if (nIntervals == 0)
					return {0, 0};

				auto *pStarts = pIntervalStarts;
				auto *pEnds = pIntervalEnds;

				umint iFirst = fUpperBound(pEnds, nIntervals, _iStart);
				umint iLast = fLowerBound(pStarts, nIntervals, _iEnd) - 1;

				if (iFirst > iLast || iFirst >= nIntervals || iLast < 0)
					return {0, 0};

				iFirst = fg_Max((umint)0, iFirst);
				iLast = fg_Min(nIntervals - 1, iLast);

				umint iLeftEnd = fUpperBound(pEnds, nIntervals, _iSplit);
				iLeftEnd = fg_Min(iLeftEnd, iLast + 1);
				if (iLeftEnd <= iFirst)
					return {0, 0};

				umint Count = iLeftEnd - iFirst;
				umint iLeftLast = iLeftEnd - 1;
				smint ScoreBefore = (iFirst > 0) ? pIntervalPrefixScore[iFirst - 1] : 0;
				smint Score = pIntervalPrefixScore[iLeftLast] - ScoreBefore;
				return {Count, Score};
			}
		;

		auto fAdaptiveAssign = [&](this auto &&_fThis, umint _iStart, umint _iEnd, umint _nElements, umint _iLocalLevel, umint _iElemStart, umint _iElemEnd) -> void
			{
				umint nSegs = _iEnd - _iStart;
				if (nSegs <= 0)
					return;

				if (nSegs == 1)
				{
					DMibFastCheck(_nElements <= mcp_SegmentSize);
					pTargets[_iStart] = _nElements;
					return;
				}

				umint iMid = _iStart + nSegs / 2;
				umint nLeftSegs = iMid - _iStart;
				umint nRightSegs = _iEnd - iMid;

				umint CapLeft = nLeftSegs * mcp_SegmentSize;
				umint CapRight = nRightSegs * mcp_SegmentSize;
				umint TotalCap = CapLeft + CapRight;

				// Children of a node at level L are constrained by level L-1 thresholds.
				// localLevel = 0 means we're splitting the root window, so use (_iWindowLevel - 1).
				umint iGlobalLevel = _iWindowLevel - _iLocalLevel - 1;
				if (iGlobalLevel < 0)
					iGlobalLevel = 0;

				// Compute density bounds for subtrees.
				// Allow 1-element slack for rounding to avoid infeasible integer bounds.
				auto DensityBounds = fp_GetLevelDensityBounds(iGlobalLevel, _nGlobalLevels);
				umint nMinLeftBase, nMaxLeftBase;
				fp_ElementBoundsFromDensity(DensityBounds, CapLeft, nMinLeftBase, nMaxLeftBase);
				umint nMinRightBase, nMaxRightBase;
				fp_ElementBoundsFromDensity(DensityBounds, CapRight, nMinRightBase, nMaxRightBase);

				umint nMinLeft = (nMinLeftBase > 0) ? (nMinLeftBase - 1) : (umint)0;
				umint nMaxLeft = (nMaxLeftBase < CapLeft) ? (nMaxLeftBase + 1) : CapLeft;
				umint nMinRight = (nMinRightBase > 0) ? (nMinRightBase - 1) : (umint)0;
				umint nMaxRight = (nMaxRightBase < CapRight) ? (nMaxRightBase + 1) : CapRight;

				// Capacity-based feasible interval for left count
				umint CapLeftMin = (_nElements > CapRight) ? (_nElements - CapRight) : (umint)0;
				umint CapLeftMax = fg_Min(CapLeft, _nElements);

				// Intersect density bounds with capacity bounds
				umint nLeftMin = fg_Max(nMinLeft, CapLeftMin);
				umint nLeftMax = fg_Min(nMaxLeft, CapLeftMax);

				// Right-side bounds translated into left-side constraints
				umint nRightMin = fg_Max(nMinRight, (_nElements > CapLeft) ? (_nElements - CapLeft) : (umint)0);
				umint nRightMax = fg_Min(nMaxRight, fg_Min(CapRight, _nElements));

				umint nLeftFromRightMax = (_nElements > nRightMax) ? (_nElements - nRightMax) : (umint)0;
				umint nLeftFromRightMin = (_nElements > nRightMin) ? (_nElements - nRightMin) : (umint)0;
				umint nLeftMinFeasible = fg_Max(nLeftMin, nLeftFromRightMax);
				umint nLeftMaxFeasible = fg_Min(nLeftMax, nLeftFromRightMin);

				if (nLeftMinFeasible > nLeftMaxFeasible)
				{
					// Fallback for rare integer-infeasible cases: respect capacity and continue.
					nLeftMinFeasible = CapLeftMin;
					nLeftMaxFeasible = CapLeftMax;
				}

				// No marked intervals in this subtree: fall back to proportional split
				{
					auto [nSubtreeIntervals, SubtreeScore] = fIntervalCountScoreBefore(_iElemStart, _iElemEnd, _iElemEnd);
					(void)SubtreeScore;
					if (nSubtreeIntervals == 0)
					{
						if constexpr (t_Options.m_bStats)
							fp_StatsAddAdaptiveSplitNoIntervals();
						// nLeftSegs == nSegs >> 1, so the ratio is always ~1/2.
						// Use _nElements >> 1 and let clamping adjust.
						umint nLeft = _nElements >> 1;
						nLeft = fg_Min(fg_Max(nLeft, nLeftMinFeasible), nLeftMaxFeasible);
						umint nRight = _nElements - nLeft;

						if (nRight < nMinRightBase)
						{
							umint nMove = nMinRightBase - nRight;
							umint nCanMove = (nLeft > nLeftMinFeasible) ? (nLeft - nLeftMinFeasible) : (umint)0;
							nMove = fg_Min(nMove, nCanMove);
							nLeft -= nMove;
							nRight += nMove;
						}
						else if (nRight > nMaxRightBase)
						{
							umint nMove = nRight - nMaxRightBase;
							umint nCanMove = (nLeft < nLeftMaxFeasible) ? (nLeftMaxFeasible - nLeft) : (umint)0;
							nMove = fg_Min(nMove, nCanMove);
							nLeft += nMove;
							nRight -= nMove;
						}

						_fThis(_iStart, iMid, nLeft, _iLocalLevel + 1, _iElemStart, _iElemStart + nLeft);
						_fThis(iMid, _iEnd, nRight, _iLocalLevel + 1, _iElemStart + nLeft, _iElemEnd);
						return;
					}
				}

				// Paper Algorithm 2: partition marked intervals to balance count and score
				// nLeftSegs == nSegs >> 1, so the ratio is always ~1/2.
				umint iElemSplitBase = _iElemStart + (_nElements >> 1);
				iElemSplitBase = fg_Min(fg_Max(iElemSplitBase, _iElemStart), _iElemEnd);

				umint nIntervals = IntervalCount;
				umint iBestSplit = iElemSplitBase;
				smint BestScoreA = 0;
				smint BestScoreB = 0;
				bool bBestSet = false;

				// Special case: |u|=2 and a single "too big" interval - allow split inside
				bool bAllowSplitInside = false;
				{
					if (nSegs == 2 && nIntervals == 1)
					{
						umint iIntStart = pIntervalStarts[0];
						umint iIntEnd = pIntervalEnds[0];
						umint nIntLen = fg_Max((umint)0, iIntEnd - iIntStart);
						if (iIntStart < _iElemEnd && iIntEnd > _iElemStart && nIntLen > (_iElemEnd - _iElemStart) / 2)
						{
							bAllowSplitInside = true;
							if constexpr (t_Options.m_bStats)
								fp_StatsAddAdaptiveSplitTooBig();
						}
					}
				}

				// If there is a single small interval pinned to one subtree edge, bias that
				// edge child according to the signed score:
				// - positive score: create slack near the hot insert edge
				// - negative score: pack the hot delete edge more densely
				if (nIntervals == 1 && !bAllowSplitInside && nSegs >= 8)
				{
					umint nIntervalsLocal = IntervalCount;
					auto *pStarts = pIntervalStarts;
					auto *pEnds = pIntervalEnds;

					umint iFirst = fUpperBound(pEnds, nIntervalsLocal, _iElemStart);
					umint iLast = fLowerBound(pStarts, nIntervalsLocal, _iElemEnd) - 1;

					if (iFirst == iLast && iFirst >= 0 && iFirst < nIntervalsLocal)
					{
						umint iIntStart = fg_Max(_iElemStart, pIntervalStarts[iFirst]);
						umint iIntEnd = fg_Min(_iElemEnd, pIntervalEnds[iFirst]);
						umint nIntervalLen = (iIntEnd > iIntStart) ? (iIntEnd - iIntStart) : (umint)0;

						if (nIntervalLen > 0 && nIntervalLen <= 2)
						{
							smint EdgeScore = pIntervalSignedScore[iFirst];
							bool bEdgeRight = (iIntEnd == _iElemEnd);
							bool bEdgeLeft = (iIntStart == _iElemStart);

							if ((bEdgeLeft || bEdgeRight) && EdgeScore != 0)
							{
								umint nLeft;
								if (EdgeScore > 0)
								{
									if (bEdgeRight)
									{
										umint nRightTarget = fg_Max(nIntervalLen, nMinRightBase);
										if (nRightTarget > _nElements)
											nRightTarget = _nElements;
										nLeft = _nElements - nRightTarget;
									}
									else
										nLeft = fg_Max(nIntervalLen, nMinLeftBase);
								}
								else
								{
									// Delete hotspot: keep the hot edge child as dense as the
									// current density bounds allow so repeated deletes drain it
									// before another underflow rebalance is needed.
									nLeft = bEdgeLeft ? nLeftMaxFeasible : nLeftMinFeasible;
								}

								nLeft = fg_Min(fg_Max(nLeft, nLeftMinFeasible), nLeftMaxFeasible);
								umint nRight = _nElements - nLeft;

								_fThis(_iStart, iMid, nLeft, _iLocalLevel + 1, _iElemStart, _iElemStart + nLeft);
								_fThis(iMid, _iEnd, nRight, _iLocalLevel + 1, _iElemStart + nLeft, _iElemEnd);
								return;
							}
						}
					}
				}

				auto fEvalSplit = [&](umint _iSplit) -> void
					{
						_iSplit = fg_Min(fg_Max(_iSplit, _iElemStart), _iElemEnd);

						auto [nLeftIntervals, LeftScore] = fIntervalCountScoreBefore(_iElemStart, _iElemEnd, _iSplit);
						auto [nSubtreeIntervals, SubtreeScore] = fIntervalCountScoreBefore(_iElemStart, _iElemEnd, _iElemEnd);

						umint nRightIntervals = nSubtreeIntervals - nLeftIntervals;
						smint RightScore = SubtreeScore - LeftScore;

						if (bAllowSplitInside)
						{
							// Allow splitting the interval; use element-weighted score by overlap
							auto [nLeftLen, LeftSplitScore] = fIntervalRangeQuery(_iElemStart, _iSplit);
							auto [nRightLen, RightSplitScore] = fIntervalRangeQuery(_iSplit, _iElemEnd);
							(void)nLeftLen;
							(void)nRightLen;
							nLeftIntervals = 1;
							nRightIntervals = 1;
							LeftScore = LeftSplitScore;
							RightScore = RightSplitScore;
						}

						umint nLeftElems = _iSplit - _iElemStart;
						smint ScoreA = fg_Abs((int)(nLeftIntervals - nRightIntervals));
						smint ScoreB = fg_Abs(LeftScore - RightScore);
						smint ScoreC = fg_Abs((int)(nLeftElems - (iElemSplitBase - _iElemStart)));

						if
						(
							!bBestSet
							|| (ScoreA < BestScoreA)
							||
							(
								ScoreA == BestScoreA
								&&
								(
									ScoreB < BestScoreB
									|| (ScoreB == BestScoreB && ScoreC < fg_Abs((int)(iBestSplit - iElemSplitBase)))
								)
							)
						)
						{
							iBestSplit = _iSplit;
							BestScoreA = ScoreA;
							BestScoreB = ScoreB;
							bBestSet = true;
						}
					}
				;

				// Candidate splits: base and interval boundaries within range
				fEvalSplit(iElemSplitBase);
				for (umint iInterval = 0; iInterval < nIntervals; ++iInterval)
				{
					umint iStart = pIntervalStarts[iInterval];
					umint iEnd = pIntervalEnds[iInterval];
					if (iStart > _iElemStart && iStart < _iElemEnd)
						fEvalSplit(iStart);
					if (iEnd > _iElemStart && iEnd < _iElemEnd)
						fEvalSplit(iEnd);
				}

				umint iElemSplit = iBestSplit;

				auto [nLeftIntervalElems, LeftScore] = fIntervalRangeQuery(_iElemStart, iElemSplit);
				auto [nRightIntervalElems, RightScore] = fIntervalRangeQuery(iElemSplit, _iElemEnd);
				(void)LeftScore;
				(void)RightScore;

				// Compute free elements (not assigned to intervals)
				umint nIntervalElems = nLeftIntervalElems + nRightIntervalElems;
				umint nFreeElems = _nElements - fg_Min(nIntervalElems, _nElements);

				// Assign free elements to minimize cardinality imbalance (Algorithm 2)
				umint nLeft;
				if (nFreeElems == 0)
					nLeft = nLeftIntervalElems; // All elements are interval elements
				else
				{
					// Greedy distribution: assign free elements to the smaller side
					int64 nDiff = (int64)nRightIntervalElems - (int64)nLeftIntervalElems;
					int64 nLeftFreeSigned;
					if (nDiff >= 0)
						nLeftFreeSigned = ((int64)nFreeElems + nDiff + 1) / 2; // Left is smaller or equal, assign more free elements to left
					else
						nLeftFreeSigned = ((int64)nFreeElems + nDiff) / 2; // Right is smaller, assign fewer free elements to left

					if (nLeftFreeSigned < 0)
						nLeftFreeSigned = 0;
					if (nLeftFreeSigned > (int64)nFreeElems)
						nLeftFreeSigned = (int64)nFreeElems;

					umint nLeftFree = (umint)nLeftFreeSigned;

					nLeft = nLeftIntervalElems + nLeftFree;
				}

				// Clamp by density bounds
				nLeft = fg_Min(fg_Max(nLeft, nLeftMinFeasible), nLeftMaxFeasible);
				umint nRight = _nElements - nLeft;

				// Adjust if right violates density bounds
				if (nRight < nMinRightBase)
				{
					umint nMove = nMinRightBase - nRight;
					umint nCanMove = (nLeft > nLeftMinFeasible) ? (nLeft - nLeftMinFeasible) : (umint)0;
					nMove = fg_Min(nMove, nCanMove);
					nLeft -= nMove;
					nRight += nMove;
				}
				else if (nRight > nMaxRightBase)
				{
					umint nMove = nRight - nMaxRightBase;
					umint nCanMove = (nLeft < nLeftMaxFeasible) ? (nLeftMaxFeasible - nLeft) : (umint)0;
					nMove = fg_Min(nMove, nCanMove);
					nLeft += nMove;
					nRight -= nMove;
				}

				DMibFastCheck(nRight <= CapRight);

				_fThis(_iStart, iMid, nLeft, _iLocalLevel + 1, _iElemStart, _iElemStart + nLeft);
				_fThis(iMid, _iEnd, nRight, _iLocalLevel + 1, _iElemStart + nLeft, _iElemEnd);
			}
		;

		fAdaptiveAssign(0, nSegments, nTotalElements, 0, 0, nTotalElements);

		umint iTempPos = 0;
		if constexpr (mcp_bNothrowElementMove)
		{
			for (umint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
			{
				umint Count = pMeta[iSeg].m_Count;
				if (Count == 0)
					continue;

				umint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
				for (umint iLocal = 0; iLocal < Count; ++iLocal)
				{
					umint iSlot = iFirst + iLocal;
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
				for (umint iLocal = 0; iLocal < Count; ++iLocal)
				{
					umint iSlot = iFirst + iLocal;
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
				for (umint iLocal = 0; iLocal < Count; ++iLocal)
				{
					umint iSlot = iFirst + iLocal;
					pKeys[iSlot].~t_CKey();
					pValues[iSlot].~t_CValue();
				}
				pMeta[iSeg].m_Count = 0;
			}
		}

		// Redistribute elements according to target cardinalities
		iTempPos = 0;

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
					for (umint iLocal = 0; iLocal < TargetCount; ++iLocal)
					{
						umint iSlot = iFirst + iLocal;
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
					for (umint iLocal = 0; iLocal < TargetCount; ++iLocal)
					{
						umint iSlot = iFirst + iLocal;
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
				fp_ResetDetectorState(pData, iSeg);
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
			else
			{
				fp_ResetDetectorState(pData, iSeg);
				if constexpr (mcp_bNothrowElementMove)
					pMeta[iSeg].m_Count = (uint16)TargetCount;
			}
		}

		PlaceCleanup.f_Clear();

		if constexpr (mcp_bDetectorStoresKeys && t_Options.m_bStats)
			fp_StatsAddDetectorRemap(nSegments, nRemapOverlapChecks, nRemapTransfers);

		DMibFastCheck(iTempPos == nTotalElements);

#if DMibDebug
		DMibCheck(fp_ValidateRedistributeDensityBounds(_pData, _iStartSeg, _iEndSeg, nTotalElements, _nGlobalLevels, "Adaptive"));
#endif
	}
}
