// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	// Remove by key
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Remove(tf_CKey &&_Key)
	{
		if (!mp_pData)
			return false;

		auto *pData = mp_pData;
		auto FindResult = fsp_Find(pData, mp_Compare, _Key);
		if (!FindResult.m_bExists)
			return false;

		fp_RemoveAtPosition(pData, FindResult.m_iSegment, FindResult.m_iLocalPos);
		return true;
	}

	// Remove by value pointer
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Remove(t_CValue *_pValue)
	{
		if (!_pValue || !mp_pData)
			return;

		auto *pData = mp_pData;
		auto *pMeta = pData->m_pSegmentMeta;
		auto *pValues = pData->m_pValues;

		// Find which segment and position this value belongs to
#ifdef DMibContractConfigure_RequireEnabled
		mint pValuesAddress = (mint)pValues;
		mint ValueAddress = (mint)_pValue;
		mint iByteOffset = ValueAddress - pValuesAddress;
		mint ValuesByteSize = mint(pData->m_Capacity) * sizeof(t_CValue);
		bool bAddressInRange = ValueAddress >= pValuesAddress && iByteOffset < ValuesByteSize;
		bool bOnValueBoundary = bAddressInRange && iByteOffset % sizeof(t_CValue) == 0;

		DMibRequire(bAddressInRange && bOnValueBoundary);
#endif
		mint iSlot = _pValue - pValues;

		mint iSegment = iSlot / mcp_SegmentSize;
		mint Count = pMeta[iSegment].m_Count;

		DMibRequire(Count > 0);

		mint iFirst = fsp_GetSegmentFirstSlot(iSegment, Count);

		DMibRequire(iSlot >= iFirst);

		mint iLocalPos = iSlot - iFirst;

		DMibRequire(iLocalPos < Count);

		fp_RemoveAtPosition(pData, iSegment, iLocalPos);
	}

	// Shared remove implementation
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_RemoveAtPosition(CPackedMapData *_pData, mint _iSegment, mint _iLocalPos)
	{
		auto *pData = _pData;
		auto *pMeta = pData->m_pSegmentMeta;
		auto *pKeys = pData->m_pKeys;
		auto *pValues = pData->m_pValues;

		mint Count = pMeta[_iSegment].m_Count;
		mint iFirst = fsp_GetSegmentFirstSlot(_iSegment, Count);
		mint iRemoveSlot = iFirst + _iLocalPos;

		// Record deletion for adaptive detector BEFORE the element is destroyed
		// and the gap is shifted closed. The shift moves elements within the
		// segment, which would invalidate raw pointers into the shifted region.
		// fp_RecordDeletion only touches detector metadata (timestamps, counters)
		// and does not depend on element counts or positions.
		t_CKey const *pPrevKey = nullptr;
		t_CKey const *pNextKey = nullptr;
		if constexpr (t_Options.m_bAdaptive)
		{
			fp_GetPrevNextKeys(pData, _iSegment, _iLocalPos, pPrevKey, pNextKey);
			fp_RecordDeletion(pData, _iSegment, pPrevKey, pNextKey);
		}

		// Destroy the element
		pKeys[iRemoveSlot].~t_CKey();
		pValues[iRemoveSlot].~t_CValue();
		fp_ClearDetectorSlotAt(pData, _iSegment, iRemoveSlot);

		// Shift elements to fill the gap
		fp_ShiftLeftInSegment(pData, _iSegment, _iLocalPos);

		--pMeta[_iSegment].m_Count;
		--pData->m_nElements;

		// Update calibrator tree counts
		fp_UpdateCalibratorCountOnDelta(pData, _iSegment, -1);

		// Update static index
		fp_UpdateStaticIndexEntry(pData, _iSegment);

		if (fp_SegmentNeedsRebalanceAfterDelete(pData, _iSegment))
			fp_RebalanceFromSegment(pData, _iSegment);

		// Refresh after potential rebalance/resize
		pData = mp_pData;
		if (!pData)
			return;

		// Check for shrinking
		if constexpr (t_Options.m_bShrink)
		{
			if constexpr (mcp_bUseFixedPoint)
			{
				mint ScaledShrinkThreshold = mcp_ResizeThresholdScaled;
				if constexpr (t_Options.m_bScanOriented)
					ScaledShrinkThreshold = mcp_DensityScale / 2;  // 0.50

				if (pData->m_nSegments > t_Options.m_MinSegments && pData->m_nElements < fsp_ScaledMulCeil(ScaledShrinkThreshold, pData->m_Capacity))
				{
					mint nNewSegments = fg_Max((mint)(pData->m_nSegments / 2), t_Options.m_MinSegments);
					fp_Resize(nNewSegments);
				}
			}
			else
			{
				pfp64 Threshold = mc_ResizeThreshold;
				if constexpr (t_Options.m_bScanOriented)
					Threshold = 0.50;

				if (pData->m_nSegments > t_Options.m_MinSegments && pfp64(pData->m_nElements) < Threshold * pfp64(pData->m_Capacity))
				{
					mint nNewSegments = fg_Max((mint)(pData->m_nSegments / 2), t_Options.m_MinSegments);
					fp_Resize(nNewSegments);
				}
			}
		}
	}

	// Remove by const value pointer
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Remove(t_CValue const *_pValue)
	{
		f_Remove(const_cast<t_CValue *>(_pValue));
	}
}
