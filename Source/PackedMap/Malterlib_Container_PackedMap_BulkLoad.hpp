// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	// Bulk load from a sorted range of key-value entries.
	// The map must not be accessed from within the populate functor (internal invariants are
	// temporarily violated during population: counts, indexes, and calibrator state are incomplete).
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CPopulate>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_BulkLoad(mint _Count, tf_CPopulate &&_fPopulate)
	{
		// Reset existing data
		f_Clear();

		if (_Count == 0)
			return;

		mint nTotalElements = _Count;

		// Choose number of segments to keep density below tauh
		mint CapacityNeeded = fsp_ScaledDivCeil(nTotalElements, mcp_RootUpperBoundScaled);

		mint nSegments = (CapacityNeeded + mcp_SegmentSize - 1) / mcp_SegmentSize;
		nSegments = fg_Max(nSegments, t_Options.m_MinSegments);
		if (nSegments < 1)
			nSegments = 1;

		fp_AllocInitial(nSegments);

		auto *pData = mp_pData;
		auto *pKeys = pData->m_pKeys;
		auto *pValues = pData->m_pValues;

		// Bottom-up count redistribution through calibrator tree
		mint nGlobalLevels = fp_CalibratorLevelCount(nSegments);
		mint iWindowLevel = (mint)(fg_GetHighestBitSet(nSegments - 1) + 1);
		TCVector<mint, CScratchAllocator> Targets;
		Targets.f_SetLen(nSegments);
		auto pTargets = Targets.f_GetArray();

		fp_DistributeElements(pTargets, 0, nSegments, nTotalElements, 0, iWindowLevel, nGlobalLevels);

		// Place elements directly into segments via the populate functor
		mint iSeg = 0;
		mint iLocalInSeg = 0;
		mint SegCount = 0;
		mint iSegFirst = 0;

		auto fAdvanceToNextSegment = [&]() noexcept
			{
				while (iSeg < nSegments)
				{
					SegCount = pTargets[iSeg];
					if (SegCount > 0)
					{
						iSegFirst = fsp_GetSegmentFirstSlot(iSeg, SegCount);
						iLocalInSeg = 0;
						pData->m_pSegmentMeta[iSeg].m_Count = (uint16)SegCount;
						pData->m_nElements += SegCount;
						return;
					}
					pData->m_pSegmentMeta[iSeg].m_Count = 0;
					if constexpr (t_Options.m_bAdaptive)
						fp_ResetDetectorState(pData, iSeg);
					++iSeg;
				}
			}
		;

		fAdvanceToNextSegment();

#ifdef DMibContractConfigure_CheckEnabled
		mint InsertCount = 0;
		mint iPrevKeySlot = TCLimitsInt<mint>::mc_Max;
#endif
#ifdef DMibContractConfigure_CheckEnabled
		constexpr static bool c_bCanThrowCheck = true;
#else
		constexpr static bool c_bCanThrowCheck = false;
#endif
		auto fPopulateOne = [&](this auto &&_fThis, auto &&_Key, auto &&_Value) noexcept
			(
				noexcept(t_CKey(fg_Forward<decltype(_Key)>(_Key)))
				&& noexcept(t_CValue(fg_Forward<decltype(_Value)>(_Value)))
				&& !c_bCanThrowCheck
			)
			-> void
			{
				constexpr static bool c_bPopulateIsNoexcept = noexcept(_fPopulate(_fThis));
				if constexpr (c_bPopulateIsNoexcept)
				{
					constexpr static bool c_bPopulateOneNoexcept
						= noexcept(t_CKey(fg_Forward<decltype(_Key)>(_Key)))
						&& noexcept(t_CValue(fg_Forward<decltype(_Value)>(_Value)))
						&& !c_bCanThrowCheck
					;

					static_assert
						(
							c_bPopulateOneNoexcept
							, "TCPackedMap::f_BulkLoad populate functor must not be noexcept when the populate callback may throw"
						)
					;
				}

				DMibCheck(iSeg < nSegments);

#ifdef DMibContractConfigure_CheckEnabled
				// Verify sorted and unique input
				if (iPrevKeySlot != TCLimitsInt<mint>::mc_Max)
				{
					DMibCheck(mp_Compare(pKeys[iPrevKeySlot], _Key) < 0);
				}
#endif
				mint iSlot = iSegFirst + iLocalInSeg;
				constexpr static bool c_bNothrow = noexcept(t_CKey(fg_Forward<decltype(_Key)>(_Key))) && noexcept(t_CValue(fg_Forward<decltype(_Value)>(_Value)));

				new(&pKeys[iSlot]) t_CKey(fg_Forward<decltype(_Key)>(_Key));

				auto KeyGuard = g_OnScopeExit / [&]
					{
						if constexpr (!c_bNothrow)
							pKeys[iSlot].~t_CKey();
					}
				;

				new(&pValues[iSlot]) t_CValue(fg_Forward<decltype(_Value)>(_Value));

				KeyGuard.f_Clear();

#ifdef DMibContractConfigure_CheckEnabled
				iPrevKeySlot = iSlot;
				++InsertCount;
#endif
				++iLocalInSeg;

				if (iLocalInSeg >= SegCount)
				{
					if constexpr (t_Options.m_bAdaptive)
						fp_ResetDetectorState(pData, iSeg);
					++iSeg;
					fAdvanceToNextSegment();
				}
			}
		;

		if constexpr (!noexcept(_fPopulate(fPopulateOne)))
		{
			bool bPopulateSucceeded = false;
			auto PopulateCleanup = g_OnScopeExit / [&]
				{
					if (!bPopulateSucceeded)
					{
						if (iSeg < nSegments)
						{
							for (mint i = 0; i < iLocalInSeg; ++i)
							{
								pKeys[iSegFirst + i].~t_CKey();
								pValues[iSegFirst + i].~t_CValue();
							}
							pData->m_pSegmentMeta[iSeg].m_Count = 0;
						}

						f_Clear(false);
					}
				}
			;

			_fPopulate(fPopulateOne);
			bPopulateSucceeded = true;
			PopulateCleanup.f_Clear();
		}
		else
			_fPopulate(fPopulateOne);

#ifdef DMibContractConfigure_CheckEnabled
		DMibCheck(InsertCount == nTotalElements);
#endif
		// Finalize remaining empty segments
		while (iSeg < nSegments)
		{
			pData->m_pSegmentMeta[iSeg].m_Count = 0;
			if constexpr (t_Options.m_bAdaptive)
				fp_ResetDetectorState(pData, iSeg);
			++iSeg;
		}

		pData->m_nElements = nTotalElements;

		// Initialize calibrator counts from segment metadata
		fp_InitializeCalibratorCounts(pData);
		fp_RebuildStaticIndex(pData);
	}

	// Bulk insert from a sorted range of key-value entries (may be non-empty).
	// The map must not be accessed from within the populate functor (internal invariants are
	// temporarily violated during population: counts, indexes, and calibrator state are incomplete).
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CPopulate>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_BulkInsert(mint _Count, tf_CPopulate &&_fPopulate)
	{
		if (_Count == 0)
			return;

		if (!mp_pData || f_IsEmpty())
		{
			f_BulkLoad(_Count, fg_Forward<tf_CPopulate>(_fPopulate));
			return;
		}

		auto *pData = mp_pData;
		mint nSegments = pData->m_nSegments;

		TCVector<t_CKey, CScratchAllocator> NewKeys;
		TCVector<t_CValue, CScratchAllocator> NewValues;
		TCVector<mint, CScratchAllocator> NewSegments;
		NewKeys.f_Reserve(_Count);
		NewValues.f_Reserve(_Count);
		NewSegments.f_Reserve(_Count);

		TCVector<mint, CScratchAllocator> InsertCounts;
		InsertCounts.f_SetLen(nSegments);
		auto pInsertCounts = InsertCounts.f_GetArray();
		for (mint iSegment = 0; iSegment < nSegments; ++iSegment)
			pInsertCounts[iSegment] = 0;

#ifdef DMibContractConfigure_CheckEnabled
		mint PopulateCount = 0;
		t_CKey const *pPrevKey = nullptr;
#endif
#ifdef DMibContractConfigure_CheckEnabled
		constexpr static bool c_bCanThrowCheck = true;
#else
		constexpr static bool c_bCanThrowCheck = false;
#endif
		auto fPopulateOne = [&](this auto &&_fThis, auto &&_Key, auto &&_Value) noexcept
			(
				noexcept(t_CKey(fg_Forward<decltype(_Key)>(_Key)))
				&& noexcept(t_CValue(fg_Forward<decltype(_Value)>(_Value)))
				&& !c_bCanThrowCheck
			)
			-> void
			{
				constexpr static bool c_bPopulateIsNoexcept = noexcept(_fPopulate(_fThis));
				if constexpr (c_bPopulateIsNoexcept)
				{
					constexpr static bool c_bPopulateOneNoexcept
						= noexcept(t_CKey(fg_Forward<decltype(_Key)>(_Key)))
						&& noexcept(t_CValue(fg_Forward<decltype(_Value)>(_Value)))
						&& !c_bCanThrowCheck
					;

					static_assert
						(
							c_bPopulateOneNoexcept
							, "TCPackedMap::f_BulkInsert populate functor must not be noexcept when the populate callback may throw"
						)
					;
				}

#ifdef DMibContractConfigure_CheckEnabled
				fsp_StaticAssertNothrowCompareStoredKeyToProbe<decltype(_Key)>();

				// Verify sorted and unique input
				if (pPrevKey)
				{
					DMibCheck(mp_Compare(*pPrevKey, _Key) < 0);
				}
				++PopulateCount;
#endif
				auto FindResult = fsp_Find(pData, mp_Compare, _Key);
				if (!FindResult.m_bExists)
				{
					mint iSegment = FindResult.m_iSegment;
					[[maybe_unused]] auto &NewKey = NewKeys.f_InsertLast(fg_Forward<decltype(_Key)>(_Key));
#ifdef DMibContractConfigure_CheckEnabled
					pPrevKey = &NewKey;
#endif
					NewValues.f_InsertLast(fg_Forward<decltype(_Value)>(_Value));
					NewSegments.f_InsertLast(iSegment);
					++pInsertCounts[iSegment];
				}
			}
		;
		_fPopulate(fPopulateOne);

#ifdef DMibContractConfigure_CheckEnabled
		DMibCheck(PopulateCount == _Count);
#endif

		if (NewKeys.f_IsEmpty())
			return;

		auto pNewValues = NewValues.f_GetArray();
		auto pNewKeys = NewKeys.f_GetArray();
		auto nNewKeys = NewKeys.f_GetLen();
		auto pNewSegments = NewSegments.f_GetArray();
		auto nNewSegments = NewSegments.f_GetLen();

#ifdef DMibContractConfigure_CheckEnabled
		for (mint iSegment = 1; iSegment < nNewSegments; ++iSegment)
			DMibCheck(NewSegments[iSegment - 1] <= NewSegments[iSegment]);
#endif

		// Ensure global density will not exceed tauh
		if constexpr (mcp_bUseFixedPoint)
		{
			mint nTotalElements = pData->m_nElements + nNewKeys;
			if (nTotalElements > fsp_ScaledMulFloor(mcp_RootUpperBoundScaled, pData->m_Capacity))
			{
				mint nResizeSegments = pData->m_nSegments * 2;
				while (nTotalElements > fsp_ScaledMulFloor(mcp_RootUpperBoundScaled, nResizeSegments * mcp_SegmentSize))
					nResizeSegments *= 2;

				fp_Resize(nResizeSegments);
				pData = mp_pData;
				nSegments = pData->m_nSegments;
				InsertCounts.f_SetLen(nSegments);
				pInsertCounts = InsertCounts.f_GetArray();
				for (mint iSegment = 0; iSegment < nSegments; ++iSegment)
					pInsertCounts[iSegment] = 0;
				for (mint iNew = 0; iNew < nNewKeys; ++iNew)
				{
					mint iSeg = fsp_FindSegmentForKey(pData, mp_Compare, pNewKeys[iNew]);
					pNewSegments[iNew] = iSeg;
					++pInsertCounts[iSeg];
				}
			}
		}
		else
		{
			mint nTotalElements = pData->m_nElements + nNewKeys;
			if (pfp64(nTotalElements) > mc_RootUpperBound * pfp64(pData->m_Capacity))
			{
				mint nResizeSegments = pData->m_nSegments * 2;
				while (pfp64(nTotalElements) > mc_RootUpperBound * pfp64(nResizeSegments * mcp_SegmentSize))
					nResizeSegments *= 2;

				fp_Resize(nResizeSegments);
				pData = mp_pData;
				nSegments = pData->m_nSegments;
				InsertCounts.f_SetLen(nSegments);
				pInsertCounts = InsertCounts.f_GetArray();
				for (mint iSegment = 0; iSegment < nSegments; ++iSegment)
					pInsertCounts[iSegment] = 0;
				for (mint iNew = 0; iNew < nNewKeys; ++iNew)
				{
					mint iSeg = fsp_FindSegmentForKey(pData, mp_Compare, pNewKeys[iNew]);
					pNewSegments[iNew] = iSeg;
					++pInsertCounts[iSeg];
				}
			}
		}

		// Prefix sums for current counts and inserts
		TCVector<mint, CScratchAllocator> PrefixCounts;
		TCVector<mint, CScratchAllocator> PrefixInserts;
		PrefixCounts.f_SetLen(nSegments + 1);
		PrefixInserts.f_SetLen(nSegments + 1);

		auto pPrefixCounts = PrefixCounts.f_GetArray();
		auto pPrefixInserts = PrefixInserts.f_GetArray();
		pPrefixCounts[0] = 0;
		pPrefixInserts[0] = 0;

		for (mint iSegment = 0; iSegment < nSegments; ++iSegment)
		{
			pPrefixCounts[iSegment + 1] = pPrefixCounts[iSegment] + pData->m_pSegmentMeta[iSegment].m_Count;
			pPrefixInserts[iSegment + 1] = pPrefixInserts[iSegment] + pInsertCounts[iSegment];
		}

		auto fWindowCount = [&](mint _iStart, mint _iEnd) -> mint
			{
				return (PrefixCounts[_iEnd] - PrefixCounts[_iStart]) + (PrefixInserts[_iEnd] - PrefixInserts[_iStart]);
			}
		;

		auto fFindProjectedWindow = [&](mint _iSegment) -> NStorage::TCTuple<mint, mint>
			{
				mint nLevels = fp_CalibratorLevelCount(nSegments);
				mint WindowSize = 1;
				mint iWindow = _iSegment;

				for (mint iLevel = 0; iLevel < nLevels; ++iLevel)
				{
					mint iWindowStart = iWindow * WindowSize;
					mint iWindowEnd = fg_Min(iWindowStart + WindowSize, nSegments);

					mint nElements = fWindowCount(iWindowStart, iWindowEnd);
					mint Capacity = (iWindowEnd - iWindowStart) * mcp_SegmentSize;

					mint nMinElems, nMaxElems;
					fp_GetLevelElementBounds(iLevel, nLevels, Capacity, nMinElems, nMaxElems);

					bool bAccept = (Capacity > 0) && (nElements >= nMinElems && nElements <= nMaxElems);
					if (bAccept && WindowSize == 1 && (pData->m_pSegmentMeta[_iSegment].m_Count + pInsertCounts[_iSegment]) > mcp_SegmentSize)
						bAccept = false;

					if (bAccept)
						return {iWindowStart, iWindowEnd};

					WindowSize *= 2;
					iWindow /= 2;
				}

				return {0, nSegments};
			}
		;

		// Build windows for all touched segments
		TCVector<NStorage::TCTuple<mint, mint>, CScratchAllocator> Windows;
		for (mint iSeg = 0; iSeg < nSegments; ++iSeg)
		{
			if (pInsertCounts[iSeg] == 0)
				continue;

			auto Window = fFindProjectedWindow(iSeg);
			Windows.f_InsertLast(Window);
		}

		if (Windows.f_IsEmpty())
			return;

		// Merge overlapping windows
		Windows.f_Sort
			(
				[](auto const &_Left, auto const &_Right)
				{
					 return fg_Get<0>(_Left) <=> fg_Get<0>(_Right);
				}
			)
		;

		auto pWindows = Windows.f_GetArray();
		auto nWindows = Windows.f_GetLen();

		TCVector<NStorage::TCTuple<mint, mint>, CScratchAllocator> Merged;
		Merged.f_InsertLast(pWindows[0]);
		for (mint iWindow = 1; iWindow < nWindows; ++iWindow)
		{
			auto &Last = Merged.f_GetLast();
			auto const &Cur = pWindows[iWindow];
			if (fg_Get<0>(Cur) <= fg_Get<1>(Last))
				fg_Get<1>(Last) = fg_Max(fg_Get<1>(Last), fg_Get<1>(Cur));
			else
				Merged.f_InsertLast(Cur);
		}

		// Helper: rebalance window with extra elements
		auto fRebalanceWindowWithExtras = [&](mint _iStartSeg, mint _iEndSeg, mint _iNewBegin, mint _iNewEnd) -> void
			{
				mint nExisting = 0;
				for (mint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
					nExisting += pData->m_pSegmentMeta[iSeg].m_Count;

				mint nNew = _iNewEnd - _iNewBegin;
				mint nTotal = nExisting + nNew;
				if (nTotal == 0)
					return;

				CScratchAllocator ScratchAlloc;
				mint KeysSize = nTotal * sizeof(t_CKey);
				mint ValuesSize = nTotal * sizeof(t_CValue);
				t_CKey *pTempKeys = (t_CKey *)ScratchAlloc.f_AllocAligned(KeysSize, alignof(t_CKey));
				auto TempKeysGuard = g_OnScopeExit / [&]
					{
						ScratchAlloc.f_Free(pTempKeys, KeysSize);
					}
				;
				t_CValue *pTempValues = (t_CValue *)ScratchAlloc.f_AllocAligned(ValuesSize, alignof(t_CValue));
				TempKeysGuard.f_Clear();

				auto CleanupTemp = g_OnScopeExit / [&]
					{
						ScratchAlloc.f_Free(pTempKeys, KeysSize);
						ScratchAlloc.f_Free(pTempValues, ValuesSize);
					}
				;

				// Merge existing elements with new elements
				mint iExistingSeg = _iStartSeg;
				mint iExistingLocal = 0;
				mint iExistingFirst = 0;
				mint ExistingCount = 0;

				auto fAdvanceExisting = [&]() -> bool
					{
						while (iExistingSeg < _iEndSeg)
						{
							ExistingCount = pData->m_pSegmentMeta[iExistingSeg].m_Count;
							if (ExistingCount > 0)
							{
								iExistingFirst = fsp_GetSegmentFirstSlot(iExistingSeg, ExistingCount);
								iExistingLocal = 0;
								return true;
							}
							++iExistingSeg;
						}
						return false;
					}
				;

				bool bHasExisting = fAdvanceExisting();
				mint iNewCursor = _iNewBegin;
				mint iOut = 0;

				if constexpr (mcp_bNothrowElementMove)
				{
					while (iOut < nTotal)
					{
						bool bUseExisting = bHasExisting;
						if (bUseExisting && iNewCursor < _iNewEnd)
						{
							t_CKey const &KeyExisting = pData->m_pKeys[iExistingFirst + iExistingLocal];
							t_CKey const &KeyNew = pNewKeys[iNewCursor];
							if (mp_Compare(KeyNew, KeyExisting) < 0)
								bUseExisting = false;
						}
						else if (!bHasExisting && iNewCursor < _iNewEnd)
							bUseExisting = false;

						if (bUseExisting)
						{
							mint iSlot = iExistingFirst + iExistingLocal;
							new(&pTempKeys[iOut]) t_CKey(fg_Move(pData->m_pKeys[iSlot]));
							new(&pTempValues[iOut]) t_CValue(fg_Move(pData->m_pValues[iSlot]));
							pData->m_pKeys[iSlot].~t_CKey();
							pData->m_pValues[iSlot].~t_CValue();
							++iExistingLocal;
							if (iExistingLocal >= ExistingCount)
							{
								++iExistingSeg;
								bHasExisting = fAdvanceExisting();
							}
						}
						else
						{
							new(&pTempKeys[iOut]) t_CKey(fg_Move(pNewKeys[iNewCursor]));
							new(&pTempValues[iOut]) t_CValue(fg_Move(pNewValues[iNewCursor]));
							++iNewCursor;
						}
						++iOut;
					}

					// Zero segment counts so destructor won't double-destroy moved-from elements
					for (mint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
						pData->m_pSegmentMeta[iSeg].m_Count = 0;
				}
				else
				{
					auto CollectCleanup = g_OnScopeExit / [&]
						{
							for (mint i = 0; i < iOut; ++i)
							{
								pTempKeys[i].~t_CKey();
								pTempValues[i].~t_CValue();
							}
						}
					;

					while (iOut < nTotal)
					{
						bool bUseExisting = bHasExisting;
						if (bUseExisting && iNewCursor < _iNewEnd)
						{
							t_CKey const &KeyExisting = pData->m_pKeys[iExistingFirst + iExistingLocal];
							t_CKey const &KeyNew = pNewKeys[iNewCursor];
							if (mp_Compare(KeyNew, KeyExisting) < 0)
								bUseExisting = false;
						}
						else if (!bHasExisting && iNewCursor < _iNewEnd)
							bUseExisting = false;

						if (bUseExisting)
						{
							mint iSlot = iExistingFirst + iExistingLocal;
							new(&pTempKeys[iOut]) t_CKey(fg_Move(pData->m_pKeys[iSlot]));
							auto KeyGuard = g_OnScopeExit / [&]
								{
									pTempKeys[iOut].~t_CKey();
								}
							;
							new(&pTempValues[iOut]) t_CValue(fg_Move(pData->m_pValues[iSlot]));
							KeyGuard.f_Clear();
							++iExistingLocal;
							if (iExistingLocal >= ExistingCount)
							{
								++iExistingSeg;
								bHasExisting = fAdvanceExisting();
							}
						}
						else
						{
							new(&pTempKeys[iOut]) t_CKey(fg_Move(pNewKeys[iNewCursor]));
							auto KeyGuard = g_OnScopeExit / [&]
								{
									pTempKeys[iOut].~t_CKey();
								}
							;
							new(&pTempValues[iOut]) t_CValue(fg_Move(pNewValues[iNewCursor]));
							KeyGuard.f_Clear();
							++iNewCursor;
						}
						++iOut;
					}

					CollectCleanup.f_Clear();

					// Destroy existing sources now that all are safely in temp
					for (mint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
					{
						mint SegCount = pData->m_pSegmentMeta[iSeg].m_Count;
						if (SegCount == 0)
							continue;
						mint iSegFirst = fsp_GetSegmentFirstSlot(iSeg, SegCount);
						for (mint iEntry = 0; iEntry < SegCount; ++iEntry)
						{
							pData->m_pKeys[iSegFirst + iEntry].~t_CKey();
							pData->m_pValues[iSegFirst + iEntry].~t_CValue();
						}
						pData->m_pSegmentMeta[iSeg].m_Count = 0;
					}
				}

				// Guard temp elements between collection and placement: if the Targets
				// allocation or distribution below throws, destroy collected temp elements.
				mint iCursor = 0;

				auto PlaceCleanup = g_OnScopeExit / [&]
					{
						// Destroy remaining temp elements not yet placed
						for (mint i = iCursor; i < iOut; ++i)
						{
							pTempKeys[i].~t_CKey();
							pTempValues[i].~t_CValue();
						}
						// Placed elements are tracked by m_Count and handled by the destructor
						mint nActual = 0;
						for (mint iSeg = _iStartSeg; iSeg < _iEndSeg; ++iSeg)
							nActual += pData->m_pSegmentMeta[iSeg].m_Count;
						pData->m_nElements = pData->m_nElements - nExisting + nActual;
					}
				;

				// Compute even distribution targets for this window
				mint nSegmentsWindow = _iEndSeg - _iStartSeg;
				TCVector<mint, CScratchAllocator> Targets;
				Targets.f_SetLen(nSegmentsWindow);
				auto pTargets = Targets.f_GetArray();

				mint nGlobalLevels = fp_CalibratorLevelCount(nSegments);
				mint iWindowLevel = (mint)(fg_GetHighestBitSet(nSegmentsWindow - 1) + 1);

				auto fAssign = [&](this auto &&_fThis, mint _iStart, mint _iEnd, mint _nElements, mint _iLevel) -> void
					{
						mint nSegs = _iEnd - _iStart;
						if (nSegs <= 0)
							return;

						if (nSegs == 1)
						{
							pTargets[_iStart] = fg_Min(_nElements, mcp_SegmentSize);
							return;
						}

						mint iMid = _iStart + nSegs / 2;
						mint nLeftSegs = iMid - _iStart;
						mint nRightSegs = _iEnd - iMid;

						mint CapLeft = nLeftSegs * mcp_SegmentSize;
						mint CapRight = nRightSegs * mcp_SegmentSize;

						// Children of a node at level L are constrained by level L-1 thresholds.
						// _iLevel = 0 means we're splitting the root window, so use (iWindowLevel - 1).
						mint iGlobalLevel = iWindowLevel - _iLevel - 1;
						if (iGlobalLevel < 0)
							iGlobalLevel = 0;

						auto DensityBounds = fp_GetLevelDensityBounds(iGlobalLevel, nGlobalLevels);
						mint nMinLeft, nMaxLeft;
						fp_ElementBoundsFromDensity(DensityBounds, CapLeft, nMinLeft, nMaxLeft);
						mint nMinRight, nMaxRight;
						fp_ElementBoundsFromDensity(DensityBounds, CapRight, nMinRight, nMaxRight);

						mint CapLeftMin = (_nElements > CapRight) ? (_nElements - CapRight) : (mint)0;
						mint CapLeftMax = fg_Min(CapLeft, _nElements);

						mint nLeftMin = fg_Max(nMinLeft, CapLeftMin);
						mint nLeftMax = fg_Min(nMaxLeft, CapLeftMax);
						mint nLeftFromRightMax = (_nElements > nMaxRight) ? (_nElements - nMaxRight) : (mint)0;
						mint nLeftFromRightMin = (_nElements > nMinRight) ? (_nElements - nMinRight) : (mint)0;
						nLeftMin = fg_Max(nLeftMin, nLeftFromRightMax);
						nLeftMax = fg_Min(nLeftMax, nLeftFromRightMin);

						if (nLeftMin > nLeftMax)
						{
							mint nMinLeftSlack = (nMinLeft > 0) ? (nMinLeft - 1) : (mint)0;
							mint nMaxLeftSlack = (nMaxLeft < CapLeft) ? (nMaxLeft + 1) : CapLeft;
							mint nMinRightSlack = (nMinRight > 0) ? (nMinRight - 1) : (mint)0;
							mint nMaxRightSlack = (nMaxRight < CapRight) ? (nMaxRight + 1) : CapRight;

							nLeftMin = fg_Max(nMinLeftSlack, CapLeftMin);
							nLeftMax = fg_Min(nMaxLeftSlack, CapLeftMax);
							mint nLeftFromRightMaxSlack = (_nElements > nMaxRightSlack) ? (_nElements - nMaxRightSlack) : (mint)0;
							mint nLeftFromRightMinSlack = (_nElements > nMinRightSlack) ? (_nElements - nMinRightSlack) : (mint)0;
							nLeftMin = fg_Max(nLeftMin, nLeftFromRightMaxSlack);
							nLeftMax = fg_Min(nLeftMax, nLeftFromRightMinSlack);

							if (nLeftMin > nLeftMax)
							{
								nLeftMin = CapLeftMin;
								nLeftMax = CapLeftMax;
							}
						}

						// nLeftSegs == nSegs >> 1, so the ratio is always ~1/2.
						mint nLeft = _nElements >> 1;
						nLeft = fg_Min(fg_Max(nLeft, nLeftMin), nLeftMax);

						_fThis(_iStart, iMid, nLeft, _iLevel + 1);
						_fThis(iMid, _iEnd, _nElements - nLeft, _iLevel + 1);
					}
				;

				fAssign(0, nSegmentsWindow, nTotal, 0);

				// Place elements back into segments
				for (mint iRel = 0; iRel < nSegmentsWindow; ++iRel)
				{
					mint iSeg = _iStartSeg + iRel;
					mint Count = pTargets[iRel];
					Count = fg_Min(Count, nTotal - iCursor);

					if (Count > 0)
					{
						mint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
						if constexpr (mcp_bNothrowElementMove)
						{
							for (mint iSegmentSlot = 0; iSegmentSlot < Count; ++iSegmentSlot)
							{
								mint iSlot = iFirst + iSegmentSlot;
								new(&pData->m_pKeys[iSlot]) t_CKey(fg_Move(pTempKeys[iCursor]));
								new(&pData->m_pValues[iSlot]) t_CValue(fg_Move(pTempValues[iCursor]));
								pTempKeys[iCursor].~t_CKey();
								pTempValues[iCursor].~t_CValue();
								++iCursor;
							}
							pData->m_pSegmentMeta[iSeg].m_Count = (uint16)Count;
						}
						else
						{
							pData->m_pSegmentMeta[iSeg].m_Count = (uint16)Count;
							mint nPlacedInSeg = 0;
							auto SegCleanup = g_OnScopeExit / [&]
								{
									for (mint i = 0; i < nPlacedInSeg; ++i)
									{
										pData->m_pKeys[iFirst + i].~t_CKey();
										pData->m_pValues[iFirst + i].~t_CValue();
									}
									pData->m_pSegmentMeta[iSeg].m_Count = 0;
								}
							;
							for (mint iSegmentSlot = 0; iSegmentSlot < Count; ++iSegmentSlot)
							{
								mint iSlot = iFirst + iSegmentSlot;
								new(&pData->m_pKeys[iSlot]) t_CKey(fg_Move(pTempKeys[iCursor]));
								auto KeyGuard = g_OnScopeExit / [&]
									{
										pData->m_pKeys[iSlot].~t_CKey();
									}
								;
								new(&pData->m_pValues[iSlot]) t_CValue(fg_Move(pTempValues[iCursor]));
								KeyGuard.f_Clear();
								pTempKeys[iCursor].~t_CKey();
								pTempValues[iCursor].~t_CValue();
								++nPlacedInSeg;
								++iCursor;
							}
							SegCleanup.f_Clear();
						}
					}
					else if constexpr (mcp_bNothrowElementMove)
						pData->m_pSegmentMeta[iSeg].m_Count = (uint16)Count;

					if constexpr (t_Options.m_bAdaptive)
						fp_ResetDetectorState(pData, iSeg);
				}

				PlaceCleanup.f_Clear();

				fp_UpdateCalibratorCountsRange(pData, _iStartSeg, _iEndSeg);
				fp_RebuildStaticIndexRange(pData, _iStartSeg, _iEndSeg);
			}
		;

		// Process each merged window
		for (auto &MergeEntry : Merged)
		{
			mint iStart = fg_Get<0>(MergeEntry);
			mint iEnd = fg_Get<1>(MergeEntry);

			auto fLowerBound = [](mint const *_pArray, mint _nLen, mint _Value) -> mint
				{
					mint iLow = 0;
					mint iHigh = _nLen;
					while (iLow < iHigh)
					{
						mint iMid = iLow + (iHigh - iLow) / 2;
						if (_pArray[iMid] < _Value)
							iLow = iMid + 1;
						else
							iHigh = iMid;
					}
					return iLow;
				}
			;

			mint iNewBegin = fLowerBound(pNewSegments, nNewSegments, iStart);
			mint iNewEnd = fLowerBound(pNewSegments, nNewSegments, iEnd);

			fRebalanceWindowWithExtras(iStart, iEnd, iNewBegin, iNewEnd);

			// Update m_nElements after each successful window so that PlaceCleanup
			// in a later window (if it throws) adjusts from the correct running total.
			pData->m_nElements += iNewEnd - iNewBegin;
		}
	}
}
