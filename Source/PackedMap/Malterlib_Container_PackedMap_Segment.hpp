// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_GetSegmentStart(umint _iSegment) noexcept
	{
		return _iSegment * mcp_SegmentSize;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_GetSegmentEnd(umint _iSegment) noexcept
	{
		return (_iSegment + 1) * mcp_SegmentSize - 1;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_IsOddSegment(umint _iSegment) noexcept
	{
		return (_iSegment & 1) != 0;
	}

	// Get first slot where elements start in a segment
	// Odd segments: packed left (elements at start)
	// Even segments: packed right (elements at end)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_GetSegmentFirstSlot(umint _iSegment, umint _Count) noexcept
	{
		if (fsp_IsOddSegment(_iSegment))
			return fsp_GetSegmentStart(_iSegment);
		else
			return fsp_GetSegmentEnd(_iSegment) - _Count + 1;
	}

	// Check if a segment needs rebalancing using density thresholds
	// Uses calibrator tree counts for O(log n) density queries
	// Must check ALL levels: each level has its own tau(h) threshold that decreases
	// toward root. A window can satisfy tau1 at leaf but violate tau(h) at a higher level.
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_SegmentNeedsRebalance(CPackedMapData const *_pData, umint _iSegment) const noexcept
	{
		auto const *pMeta = _pData->m_pSegmentMeta;

		umint nSegments = _pData->m_nSegments;
		umint nLevels = fp_CalibratorLevelCount(nSegments);

		// Check each calibrator tree level for overflow
		umint WindowSize = 1;
		umint iWindow = _iSegment;

		for (umint iLevel = 0; iLevel < nLevels; ++iLevel)
		{
			// Get element count from calibrator tree: O(1) per level
			umint nElements = fp_GetCalibratorWindowCount(_pData, iLevel, iWindow);

			// Calculate window bounds for capacity (window may be truncated at end)
			umint iWindowStart = iWindow * WindowSize;
			umint iWindowEnd = fg_Min(iWindowStart + WindowSize, nSegments);
			umint Capacity = (iWindowEnd - iWindowStart) * mcp_SegmentSize;

			umint nMinElems, nMaxElems;
			fp_GetLevelElementBounds(iLevel, nLevels, Capacity, nMinElems, nMaxElems);
			(void)nMinElems;

			// Upper-bound checks use 1-element slack to match invariant validation.
			if (nElements > nMaxElems + 1)
				return true;

			// Move to parent level
			WindowSize *= 2;
			iWindow /= 2;
		}

		return false;
	}

	// Check if a segment needs rebalancing after deletion due to lower-bound violation
	// Uses calibrator tree counts for O(log n) density queries instead of O(n) iteration
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_SegmentNeedsRebalanceAfterDelete(CPackedMapData const *_pData, umint _iSegment) const noexcept
	{
		umint nSegments = _pData->m_nSegments;
		umint nLevels = fp_CalibratorLevelCount(nSegments);

		// Check each calibrator tree level for underflow
		// Must check ALL levels: each level has its own rho(h) threshold that increases
		// toward root. A window can satisfy rho1 at leaf but violate rho(h) at a higher level.
		umint WindowSize = 1;
		umint iWindow = _iSegment;

		for (umint iLevel = 0; iLevel < nLevels; ++iLevel)
		{
			// Get element count from calibrator tree: O(1) per level
			umint nElements = fp_GetCalibratorWindowCount(_pData, iLevel, iWindow);

			// Calculate window bounds for capacity (window may be truncated at end)
			umint iWindowStart = iWindow * WindowSize;
			umint iWindowEnd = fg_Min(iWindowStart + WindowSize, nSegments);
			umint Capacity = (iWindowEnd - iWindowStart) * mcp_SegmentSize;

			umint nMinElems, nMaxElems;
			fp_GetLevelElementBounds(iLevel, nLevels, Capacity, nMinElems, nMaxElems);
			(void)nMaxElems;

			// If density below lower bound and rho > 0, needs rebalance
			// Guard: only enforce lower bound when meaningful
			// 1. Global density must be at or above root lower bound
			// 2. Expected elements in this window must be >= 1
			if (nMinElems > 0 && nElements < nMinElems)
			{
				umint TotalCapacity = nSegments * mcp_SegmentSize;
				umint nTotalElements = _pData->m_nElements;
				umint nWindowSegments = iWindowEnd - iWindowStart;
				umint nScaledExpectedWindowElements;
				bool bExpectedWindowCouldContainElement = fg_MultiplyOverflow(nTotalElements, nWindowSegments, nScaledExpectedWindowElements) || (nScaledExpectedWindowElements >= nSegments);

				bool bEnforceLowerBound;
				if constexpr (mcp_bUseFixedPoint)
					bEnforceLowerBound = bExpectedWindowCouldContainElement && (nTotalElements >= fsp_ScaledMulCeil(mcp_RootLowerBoundScaled, TotalCapacity));
				else
					bEnforceLowerBound = bExpectedWindowCouldContainElement && (pfp64(nTotalElements) >= mc_RootLowerBound * TotalCapacity);

				if (bEnforceLowerBound)
					return true;
			}

			// Move to parent level
			WindowSize *= 2;
			iWindow /= 2;
		}
		return false;
	}

	// Adjust detector slots for shifted elements (slot storage only)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_AdjustDetectorSlotsOnShift
		(
			CPackedMapData *_pData
			, umint _iSegment
			, umint _iStartSlot
			, umint _iEndSlot
			, umint _Delta
		)
		noexcept
	{
		if constexpr (mcp_bDetectorStoresKeys || !t_Options.m_bAdaptive)
			return;
		else
		{
			if (_iStartSlot > _iEndSlot)
				return;

			auto &Keys = _pData->m_pSegmentMeta[_iSegment].m_DetectorKeys;

			auto fAdjust = [&](NStorage::TCOptional<umint> &_Slot)
				{
					if (_Slot && *_Slot >= _iStartSlot && *_Slot <= _iEndSlot)
						*_Slot = *_Slot + _Delta;
				}
			;

			fAdjust(Keys.m_BackwardRunLengthSlot);
			fAdjust(Keys.m_ForwardRunLengthSlot);
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_ClearDetectorSlotAt(CPackedMapData *_pData, umint _iSegment, umint _iSlot) noexcept
	{
		if constexpr (mcp_bDetectorStoresKeys || !t_Options.m_bAdaptive)
			return;
		else
		{
			auto &Keys = _pData->m_pSegmentMeta[_iSegment].m_DetectorKeys;

			if (Keys.m_BackwardRunLengthSlot && *Keys.m_BackwardRunLengthSlot == _iSlot)
				Keys.m_BackwardRunLengthSlot.f_Clear();
			if (Keys.m_ForwardRunLengthSlot && *Keys.m_ForwardRunLengthSlot == _iSlot)
				Keys.m_ForwardRunLengthSlot.f_Clear();
		}
	}

	// Shift elements right within a segment to make room for insertion
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_ShiftRightInSegment(CPackedMapData *_pData, umint _iSegment, umint _iLocalPos)
	{
		auto *pMeta = _pData->m_pSegmentMeta;
		auto *pKeys = _pData->m_pKeys;
		auto *pValues = _pData->m_pValues;

		umint Count = pMeta[_iSegment].m_Count;
		bool bIsOdd = fsp_IsOddSegment(_iSegment);

		// For odd segments (packed left): shift elements right from _iLocalPos to end
		// For even segments (packed right): shift elements left to make room

		if (bIsOdd)
		{
			// Odd segment: elements at [segStart, segStart + count)
			// We need to shift [segStart + localPos, segStart + count) right by 1
			umint iSegStart = fsp_GetSegmentStart(_iSegment);
			fp_AdjustDetectorSlotsOnShift(_pData, _iSegment, iSegStart + _iLocalPos, iSegStart + Count - 1, 1);

			if constexpr
			(
				NTraits::cIsTriviallyMoveConstructible<t_CKey>
				&& NTraits::cIsTriviallyDestructible<t_CKey>
				&& NTraits::cIsTriviallyMoveConstructible<t_CValue>
				&& NTraits::cIsTriviallyDestructible<t_CValue>
			)
			{
				umint nMove = Count - _iLocalPos;
				if (nMove > 0)
				{
					NMemory::fg_MemMove(&pKeys[iSegStart + _iLocalPos + 1], &pKeys[iSegStart + _iLocalPos], nMove * sizeof(t_CKey));
					NMemory::fg_MemMove(&pValues[iSegStart + _iLocalPos + 1], &pValues[iSegStart + _iLocalPos], nMove * sizeof(t_CValue));
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(nMove);
				}
			}
			else if constexpr (mcp_bNothrowElementMove)
			{
				for (umint iSegmentSlot = Count; iSegmentSlot-- > _iLocalPos; )
				{
					umint iSrc = iSegStart + iSegmentSlot;
					umint iDst = iSrc + 1;
					new(&pKeys[iDst]) t_CKey(fg_Move(pKeys[iSrc]));
					new(&pValues[iDst]) t_CValue(fg_Move(pValues[iSrc]));
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(1);
					pKeys[iSrc].~t_CKey();
					pValues[iSrc].~t_CValue();
				}
			}
			else
			{
				// Throwing move: on exception, destroy shifted destinations to close the gap
				umint nShifted = 0;
				auto ShiftCleanup = g_OnScopeExit / [&]
					{
						for (umint i = 0; i < nShifted; ++i)
						{
							umint iSlot = iSegStart + Count - i;
							pKeys[iSlot].~t_CKey();
							pValues[iSlot].~t_CValue();
						}
						pMeta[_iSegment].m_Count -= (uint16)nShifted;
						_pData->m_nElements -= nShifted;
					}
				;

				for (umint iSegmentSlot = Count; iSegmentSlot-- > _iLocalPos; )
				{
					umint iSrc = iSegStart + iSegmentSlot;
					umint iDst = iSrc + 1;
					new(&pKeys[iDst]) t_CKey(fg_Move(pKeys[iSrc]));
					auto KeyGuard = g_OnScopeExit / [&]
						{
							pKeys[iDst].~t_CKey();
						}
					;
					new(&pValues[iDst]) t_CValue(fg_Move(pValues[iSrc]));
					KeyGuard.f_Clear();
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(1);
					pKeys[iSrc].~t_CKey();
					pValues[iSrc].~t_CValue();
					++nShifted;
				}

				ShiftCleanup.f_Clear();
			}

			return iSegStart + _iLocalPos;
		}
		else
		{
			// Even segment: elements at [segEnd - count + 1, segEnd]
			// We shift elements left to make room at the right position
			umint iFirst = fsp_GetSegmentFirstSlot(_iSegment, Count);
			fp_AdjustDetectorSlotsOnShift(_pData, _iSegment, iFirst, iFirst + _iLocalPos - 1, -1);

			if constexpr
			(
				NTraits::cIsTriviallyMoveConstructible<t_CKey>
				&& NTraits::cIsTriviallyDestructible<t_CKey>
				&& NTraits::cIsTriviallyMoveConstructible<t_CValue>
				&& NTraits::cIsTriviallyDestructible<t_CValue>
			)
			{
				umint nMove = _iLocalPos;
				if (nMove > 0)
				{
					NMemory::fg_MemMove(&pKeys[iFirst - 1], &pKeys[iFirst], nMove * sizeof(t_CKey));
					NMemory::fg_MemMove(&pValues[iFirst - 1], &pValues[iFirst], nMove * sizeof(t_CValue));
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(nMove);
				}
			}
			else if constexpr (mcp_bNothrowElementMove)
			{
				for (umint iSegmentSlot = 0; iSegmentSlot < _iLocalPos; ++iSegmentSlot)
				{
					umint iSrc = iFirst + iSegmentSlot;
					umint iDst = iSrc - 1;
					new(&pKeys[iDst]) t_CKey(fg_Move(pKeys[iSrc]));
					new(&pValues[iDst]) t_CValue(fg_Move(pValues[iSrc]));
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(1);
					pKeys[iSrc].~t_CKey();
					pValues[iSrc].~t_CValue();
				}
			}
			else
			{
				// Throwing move: on exception, destroy shifted destinations to close the gap
				umint nShifted = 0;
				auto ShiftCleanup = g_OnScopeExit / [&]
					{
						for (umint i = 0; i < nShifted; ++i)
						{
							umint iSlot = iFirst - 1 + i;
							pKeys[iSlot].~t_CKey();
							pValues[iSlot].~t_CValue();
						}
						pMeta[_iSegment].m_Count -= (uint16)nShifted;
						_pData->m_nElements -= nShifted;
					}
				;

				for (umint iSegmentSlot = 0; iSegmentSlot < _iLocalPos; ++iSegmentSlot)
				{
					umint iSrc = iFirst + iSegmentSlot;
					umint iDst = iSrc - 1;
					new(&pKeys[iDst]) t_CKey(fg_Move(pKeys[iSrc]));
					auto KeyGuard = g_OnScopeExit / [&]
						{
							pKeys[iDst].~t_CKey();
						}
					;
					new(&pValues[iDst]) t_CValue(fg_Move(pValues[iSrc]));
					KeyGuard.f_Clear();
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(1);
					pKeys[iSrc].~t_CKey();
					pValues[iSrc].~t_CValue();
					++nShifted;
				}

				ShiftCleanup.f_Clear();
			}

			return iFirst - 1 + _iLocalPos;
		}
	}

	// Shift elements left within a segment after removal
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_ShiftLeftInSegment(CPackedMapData *_pData, umint _iSegment, umint _iLocalPos)
	{
		auto *pMeta = _pData->m_pSegmentMeta;
		auto *pKeys = _pData->m_pKeys;
		auto *pValues = _pData->m_pValues;

		umint Count = pMeta[_iSegment].m_Count;
		bool bIsOdd = fsp_IsOddSegment(_iSegment);

		umint iFirst = fsp_GetSegmentFirstSlot(_iSegment, Count);

		if (bIsOdd)
		{
			// Odd segment: shift left from localPos+1 to fill the gap
			fp_AdjustDetectorSlotsOnShift(_pData, _iSegment, iFirst + _iLocalPos + 1, iFirst + Count - 1, -1);

			if constexpr
			(
				NTraits::cIsTriviallyMoveConstructible<t_CKey>
				&& NTraits::cIsTriviallyDestructible<t_CKey>
				&& NTraits::cIsTriviallyMoveConstructible<t_CValue>
				&& NTraits::cIsTriviallyDestructible<t_CValue>
			)
			{
				umint nMove = Count - _iLocalPos - 1;
				if (nMove > 0)
				{
					NMemory::fg_MemMove(&pKeys[iFirst + _iLocalPos], &pKeys[iFirst + _iLocalPos + 1], nMove * sizeof(t_CKey));
					NMemory::fg_MemMove(&pValues[iFirst + _iLocalPos], &pValues[iFirst + _iLocalPos + 1], nMove * sizeof(t_CValue));
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(nMove);
				}
			}
			else if constexpr (mcp_bNothrowElementMove)
			{
				for (umint iSegmentSlot = _iLocalPos; iSegmentSlot < Count - 1; ++iSegmentSlot)
				{
					umint iDst = iFirst + iSegmentSlot;
					umint iSrc = iDst + 1;
					new(&pKeys[iDst]) t_CKey(fg_Move(pKeys[iSrc]));
					new(&pValues[iDst]) t_CValue(fg_Move(pValues[iSrc]));
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(1);
					pKeys[iSrc].~t_CKey();
					pValues[iSrc].~t_CValue();
				}
			}
			else
			{
				// Throwing move: on exception, destroy originals after the gap to close it
				umint nShifted = 0;
				auto ShiftCleanup = g_OnScopeExit / [&]
					{
						// Destroy un-shifted originals after the gap
						for (umint iSlot = iFirst + _iLocalPos + nShifted + 1; iSlot < iFirst + Count; ++iSlot)
						{
							pKeys[iSlot].~t_CKey();
							pValues[iSlot].~t_CValue();
						}
						// Segment now has _iLocalPos + nShifted contiguous elements
						umint nLost = (Count - 1) - (_iLocalPos + nShifted);
						pMeta[_iSegment].m_Count = (uint16)(_iLocalPos + nShifted);
						_pData->m_nElements -= nLost + 1; // +1 for the already-destroyed removed element
					}
				;

				for (umint iSegmentSlot = _iLocalPos; iSegmentSlot < Count - 1; ++iSegmentSlot)
				{
					umint iDst = iFirst + iSegmentSlot;
					umint iSrc = iDst + 1;
					new(&pKeys[iDst]) t_CKey(fg_Move(pKeys[iSrc]));
					auto KeyGuard = g_OnScopeExit / [&]
						{
							pKeys[iDst].~t_CKey();
						}
					;
					new(&pValues[iDst]) t_CValue(fg_Move(pValues[iSrc]));
					KeyGuard.f_Clear();
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(1);
					pKeys[iSrc].~t_CKey();
					pValues[iSrc].~t_CValue();
					++nShifted;
				}

				ShiftCleanup.f_Clear();
			}
		}
		else
		{
			// Even segment: shift right to fill the gap
			fp_AdjustDetectorSlotsOnShift(_pData, _iSegment, iFirst, iFirst + _iLocalPos - 1, 1);

			if constexpr
			(
				NTraits::cIsTriviallyMoveConstructible<t_CKey>
				&& NTraits::cIsTriviallyDestructible<t_CKey>
				&& NTraits::cIsTriviallyMoveConstructible<t_CValue>
				&& NTraits::cIsTriviallyDestructible<t_CValue>
			)
			{
				umint nMove = _iLocalPos;
				if (nMove > 0)
				{
					NMemory::fg_MemMove(&pKeys[iFirst + 1], &pKeys[iFirst], nMove * sizeof(t_CKey));
					NMemory::fg_MemMove(&pValues[iFirst + 1], &pValues[iFirst], nMove * sizeof(t_CValue));
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(nMove);
				}
			}
			else if constexpr (mcp_bNothrowElementMove)
			{
				// Reverse loop: shift elements [iFirst .. iFirst+_iLocalPos-1] right by 1
				for (umint iSegmentSlot = _iLocalPos + 1; --iSegmentSlot > 0; )
				{
					umint iDst = iFirst + iSegmentSlot;
					umint iSrc = iDst - 1;
					new(&pKeys[iDst]) t_CKey(fg_Move(pKeys[iSrc]));
					new(&pValues[iDst]) t_CValue(fg_Move(pValues[iSrc]));
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(1);
					pKeys[iSrc].~t_CKey();
					pValues[iSrc].~t_CValue();
				}
			}
			else
			{
				// Throwing move: on exception, destroy originals before the gap to close it
				umint nShifted = 0;
				auto ShiftCleanup = g_OnScopeExit / [&]
					{
						// Destroy un-shifted originals before the gap
						for (umint iSlot = iFirst; iSlot < iFirst + _iLocalPos - nShifted; ++iSlot)
						{
							pKeys[iSlot].~t_CKey();
							pValues[iSlot].~t_CValue();
						}
						// Segment now has Count - 1 - (_iLocalPos - nShifted) elements at the right end
						umint nLost = (_iLocalPos - nShifted);
						pMeta[_iSegment].m_Count = (uint16)(Count - 1 - nLost);
						_pData->m_nElements -= nLost + 1; // +1 for the already-destroyed removed element
					}
				;

				for (umint iSegmentSlot = _iLocalPos + 1; --iSegmentSlot > 0; )
				{
					umint iDst = iFirst + iSegmentSlot;
					umint iSrc = iDst - 1;
					new(&pKeys[iDst]) t_CKey(fg_Move(pKeys[iSrc]));
					auto KeyGuard = g_OnScopeExit / [&]
						{
							pKeys[iDst].~t_CKey();
						}
					;
					new(&pValues[iDst]) t_CValue(fg_Move(pValues[iSrc]));
					KeyGuard.f_Clear();
					if constexpr (t_Options.m_bStats)
						fp_StatsAddShiftMoves(1);
					pKeys[iSrc].~t_CKey();
					pValues[iSrc].~t_CValue();
					++nShifted;
				}

				ShiftCleanup.f_Clear();
			}
		}
	}

	// Get predecessor/successor keys around a local position (after insertion)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_GetPrevNextKeys
		(
			CPackedMapData const *_pData
			, umint _iSegment
			, umint _iLocalPos
			, t_CKey const *&_pPrevKey
			, t_CKey const *&_pNextKey
		)
		const noexcept
	{
		_pPrevKey = nullptr;
		_pNextKey = nullptr;

		auto const *pMeta = _pData->m_pSegmentMeta;
		auto const *pKeys = _pData->m_pKeys;

		umint Count = pMeta[_iSegment].m_Count;
		if (Count == 0)
			return false;

		umint iFirst = fsp_GetSegmentFirstSlot(_iSegment, Count);

		if (_iLocalPos > 0)
			_pPrevKey = &pKeys[iFirst + _iLocalPos - 1];
		else
		{
			umint iPrevSeg = fsp_FindPrevNonEmptySegment(_pData, _iSegment);
			if (iPrevSeg < _pData->m_nSegments)
			{
				umint PrevCount = pMeta[iPrevSeg].m_Count;
				if (PrevCount > 0)
				{
					umint iPrevFirst = fsp_GetSegmentFirstSlot(iPrevSeg, PrevCount);
					_pPrevKey = &pKeys[iPrevFirst + PrevCount - 1];
				}
			}
		}

		if (_iLocalPos + 1 < Count)
			_pNextKey = &pKeys[iFirst + _iLocalPos + 1];
		else
		{
			umint iNextSeg = fsp_FindNextNonEmptySegment(_pData, _iSegment);
			if (iNextSeg < _pData->m_nSegments)
			{
				umint NextCount = pMeta[iNextSeg].m_Count;
				if (NextCount > 0)
				{
					umint iNextFirst = fsp_GetSegmentFirstSlot(iNextSeg, NextCount);
					_pNextKey = &pKeys[iNextFirst];
				}
			}
		}

		return _pPrevKey || _pNextKey;
	}
}
