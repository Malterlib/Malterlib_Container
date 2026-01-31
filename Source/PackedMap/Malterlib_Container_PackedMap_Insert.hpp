// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey, typename ...tfp_CParam>
	constexpr TCPackedMapResult<t_CKey, t_CValue> TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Insert(tf_CKey &&_Key, tfp_CParam && ...p_Params)
	{
		return operator()(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CParam>(p_Params)...);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey, typename ...tfp_CParam>
	constexpr t_CValue &TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::operator[] (tf_CKey &&_Key, tfp_CParam && ...p_Params)
	{
		return operator()(fg_Forward<tf_CKey>(_Key), fg_Forward<tfp_CParam>(p_Params)...).m_Value;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr t_CValue const &TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::operator[] (tf_CKey &&_Key) const
	{
		auto const *pValue = f_FindEqual(fg_Forward<tf_CKey>(_Key));
		if (!pValue)
			DMibError("Key not found in PackedMap");
		return *pValue;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey, typename ...tfp_CParam>
	constexpr TCPackedMapResult<t_CKey, t_CValue> TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::operator () (tf_CKey &&_Key, tfp_CParam && ...p_Params)
	{
		if (!mp_pData)
			fp_AllocInitial();

		while (true)
		{
			auto *pData = mp_pData;
			DMibFastCheck(pData);

			// Find position
			auto FindResult = fsp_Find(pData, mp_Compare, _Key);

			// If already exists, return it
			if (FindResult.m_bExists)
				return {pData->m_pKeys[FindResult.m_iSlot], pData->m_pValues[FindResult.m_iSlot], false};

			mint iSegment = FindResult.m_iSegment;
			auto *pMeta = pData->m_pSegmentMeta;

			// Check if segment is full
			if (pMeta[iSegment].m_Count >= mcp_SegmentSize)
			{
				// Rebalance and retry
				fp_RebalanceFromSegment(pData, iSegment);
				// Refresh after potential rebalance/resize
				pData = mp_pData;
				DMibFastCheck(pData);
				pMeta = pData->m_pSegmentMeta;
				if (pMeta[iSegment].m_Count >= mcp_SegmentSize)
				{
					mint nNewSegments = pData->m_nSegments * 2;
					fp_Resize(nNewSegments);
				}
				continue;
			}

			// Shift to make room and insert
			mint Count = pMeta[iSegment].m_Count;

			auto *pKeys = pData->m_pKeys;
			auto *pValues = pData->m_pValues;

			mint iInsertSlot;

			if (Count == 0)
			{
				if (fsp_IsOddSegment(iSegment))
					iInsertSlot = fsp_GetSegmentStart(iSegment);
				else
					iInsertSlot = fsp_GetSegmentEnd(iSegment);
			}
			else
				iInsertSlot = fp_ShiftRightInSegment(pData, iSegment, FindResult.m_iLocalPos);

			// Insert key and value with exception safety guards
			// InsertCleanup: if key/value construction fails after a shift, destroy the
			// shifted elements to close the gap and keep the segment contiguous.
			constexpr bool c_bNothrowInsert = noexcept(t_CKey(fg_Forward<tf_CKey>(_Key))) && noexcept(t_CValue(fg_Forward<tfp_CParam>(p_Params)...));
			auto InsertCleanup = g_OnScopeExit / [&]
				{
					if constexpr (!c_bNothrowInsert)
					{
						if (Count > 0)
						{
							if (fsp_IsOddSegment(iSegment))
							{
								mint nToDestroy = Count - FindResult.m_iLocalPos;
								for (mint i = 0; i < nToDestroy; ++i)
								{
									pKeys[iInsertSlot + 1 + i].~t_CKey();
									pValues[iInsertSlot + 1 + i].~t_CValue();
								}
								pMeta[iSegment].m_Count -= (uint16)nToDestroy;
								pData->m_nElements -= nToDestroy;
							}
							else
							{
								mint iShiftStart = fsp_GetSegmentFirstSlot(iSegment, Count) - 1;
								for (mint i = 0; i < FindResult.m_iLocalPos; ++i)
								{
									pKeys[iShiftStart + i].~t_CKey();
									pValues[iShiftStart + i].~t_CValue();
								}
								pMeta[iSegment].m_Count -= (uint16)FindResult.m_iLocalPos;
								pData->m_nElements -= FindResult.m_iLocalPos;
							}
						}
					}
				}
			;
			new(&pKeys[iInsertSlot]) t_CKey(fg_Forward<tf_CKey>(_Key));
			auto KeyGuard = g_OnScopeExit / [&]
				{
					if constexpr (!c_bNothrowInsert)
						pKeys[iInsertSlot].~t_CKey();
				}
			;
			new(&pValues[iInsertSlot]) t_CValue(fg_Forward<tfp_CParam>(p_Params)...);
			KeyGuard.f_Clear();
			InsertCleanup.f_Clear();
			++pMeta[iSegment].m_Count;
			++pData->m_nElements;

#ifdef DMibContractConfigure_CheckEnabled
			{
				t_CKey const *pPrevKey = nullptr;
				t_CKey const *pNextKey = nullptr;
				fp_GetPrevNextKeys(pData, iSegment, FindResult.m_iLocalPos, pPrevKey, pNextKey);
				if (pPrevKey)
				{
					DMibCheck(mp_Compare(*pPrevKey, pKeys[iInsertSlot]) < 0);
				}
				if (pNextKey)
				{
					DMibCheck(mp_Compare(pKeys[iInsertSlot], *pNextKey) < 0);
				}
			}
#endif

			// Update calibrator tree counts
			fp_UpdateCalibratorCountOnDelta(pData, iSegment, 1);

			// Update static index
			fp_UpdateStaticIndexEntry(pData, iSegment);

			// Record for adaptive (if enabled)
			if constexpr (t_Options.m_bAdaptive)
			{
				t_CKey const *pPrevKey = nullptr;
				t_CKey const *pNextKey = nullptr;
				if (pData->m_nElements > 1)
					fp_GetPrevNextKeys(pData, iSegment, FindResult.m_iLocalPos, pPrevKey, pNextKey);
				fp_RecordInsertion(pData, iSegment, pPrevKey, pNextKey);
			}

			if (fp_SegmentNeedsRebalance(pData, iSegment))
			{
				// Compute rank before rebalance invalidates segment layout
				mint Rank = fsp_ComputeRank(pData, iSegment, FindResult.m_iLocalPos);
				fp_RebalanceFromSegment(pData, iSegment);

				pData = mp_pData;
				DMibFastCheck(pData);
				// Root-level τh enforcement after rebalance
				if constexpr (mcp_bUseFixedPoint)
				{
					if (pData->m_nElements > fsp_ScaledMulFloor(mcp_RootUpperBoundScaled, pData->m_Capacity))
					{
						mint nNewSegments = pData->m_nSegments * 2;
						fp_Resize(nNewSegments);
					}
				}
				else
				{
					if (pfp64(pData->m_nElements) > mc_RootUpperBound * pfp64(pData->m_Capacity))
					{
						mint nNewSegments = pData->m_nSegments * 2;
						fp_Resize(nNewSegments);
					}
				}

				pData = mp_pData;
				DMibFastCheck(pData);
				mint iSlot = fsp_FindSlotByRank(pData, Rank);
				return {pData->m_pKeys[iSlot], pData->m_pValues[iSlot], true};
			}

			// Root-level τh enforcement: ensure global density stays within bounds
			// This is a safety net in addition to the multi-level check in fp_SegmentNeedsRebalance
			if constexpr (mcp_bUseFixedPoint)
			{
				if (pData->m_nElements > fsp_ScaledMulFloor(mcp_RootUpperBoundScaled, pData->m_Capacity))
				{
					mint Rank = fsp_ComputeRank(pData, iSegment, FindResult.m_iLocalPos);
					mint nNewSegments = pData->m_nSegments * 2;
					fp_Resize(nNewSegments);
					pData = mp_pData;
					DMibFastCheck(pData);
					mint iSlot = fsp_FindSlotByRank(pData, Rank);
					return {pData->m_pKeys[iSlot], pData->m_pValues[iSlot], true};
				}
			}
			else
			{
				if (pfp64(pData->m_nElements) > mc_RootUpperBound * pfp64(pData->m_Capacity))
				{
					mint Rank = fsp_ComputeRank(pData, iSegment, FindResult.m_iLocalPos);
					mint nNewSegments = pData->m_nSegments * 2;
					fp_Resize(nNewSegments);
					pData = mp_pData;
					DMibFastCheck(pData);
					mint iSlot = fsp_FindSlotByRank(pData, Rank);
					return {pData->m_pKeys[iSlot], pData->m_pValues[iSlot], true};
				}
			}

			return {pKeys[iInsertSlot], pValues[iInsertSlot], true};
		}
	}
}
