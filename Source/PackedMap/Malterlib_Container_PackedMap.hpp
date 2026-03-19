// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

// Include all implementation files
#include "Malterlib_Container_PackedMap_Allocation.hpp"
#include "Malterlib_Container_PackedMap_Segment.hpp"
#include "Malterlib_Container_PackedMap_Find.hpp"
#include "Malterlib_Container_PackedMap_Insert.hpp"
#include "Malterlib_Container_PackedMap_Remove.hpp"
#include "Malterlib_Container_PackedMap_BulkLoad.hpp"
#include "Malterlib_Container_PackedMap_Debug.hpp"
#include "Malterlib_Container_PackedMap_Rebalance.hpp"
#include "Malterlib_Container_PackedMap_Adaptive.hpp"
#include "Malterlib_Container_PackedMap_Iterator.hpp"
#include "Malterlib_Container_PackedMap_Compare.hpp"
#include "Malterlib_Container_PackedMap_Stream.hpp"
#include "Malterlib_Container_PackedMap_Scan.hpp"
#include "Malterlib_Container_PackedMap_Lifetime.hpp"
#include "Malterlib_Container_PackedMap_KeyAccess.hpp"
#include "Malterlib_Container_PackedMap_Format.hpp"
#include "Malterlib_Container_PackedMap_Result.hpp"
#include "Malterlib_Container_PackedMap_Stats.hpp"

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr inline_small void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CPackedMapData::f_SetIndexEntryValid(umint _iEntry, bool _bValid) noexcept
	{
		if constexpr (mcp_bIndexStoresKeys)
		{
			umint iByte = _iEntry / 8;
			uint8 Bit = 1u << (_iEntry % 8);

			if (_bValid)
				m_IndexValidStorage[iByte] |= Bit;
			else
				m_IndexValidStorage[iByte] &= ~Bit;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr inline_small bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CPackedMapData::f_GetIndexEntryValid(umint _iEntry) const noexcept
	{
		if constexpr (mcp_bIndexStoresKeys)
		{
			umint iByte = _iEntry / 8;
			uint8 Bit = 1u << (_iEntry % 8);

			return (m_IndexValidStorage[iByte] & Bit) != 0;
		}
		else
			return m_pIndex[_iEntry] < m_Capacity;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr inline_small void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CPackedMapData::f_DestroyIndexEntries() noexcept
	{
		if constexpr (mcp_bIndexStoresKeys)
		{
			for (umint iEntry = 0; iEntry < m_nStaticIndexTotalEntries; ++iEntry)
			{
				if (!f_GetIndexEntryValid(iEntry))
					continue;

				m_pIndex[iEntry].~CIndexEntry();
				f_SetIndexEntryValid(iEntry, false);
			}
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr inline_small void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CPackedMapData::f_SetIndexEntry(umint _iEntry, CIndexEntry const &_Entry) noexcept
	{
		if constexpr (mcp_bIndexStoresKeys)
		{
			if (f_GetIndexEntryValid(_iEntry))
				m_pIndex[_iEntry] = _Entry;
			else
			{
				new(&m_pIndex[_iEntry]) CIndexEntry(_Entry);
				f_SetIndexEntryValid(_iEntry, true);
			}
		}
		else
			m_pIndex[_iEntry] = _Entry;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr inline_small void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CPackedMapData::f_ClearIndexEntry(umint _iEntry) noexcept
	{
		if constexpr (mcp_bIndexStoresKeys)
		{
			if (!f_GetIndexEntryValid(_iEntry))
				return;

			m_pIndex[_iEntry].~CIndexEntry();
			f_SetIndexEntryValid(_iEntry, false);
		}
		else
			m_pIndex[_iEntry] = m_Capacity;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetLen() const noexcept
	{
		if (!mp_pData)
			return 0;

		return mp_pData->m_nElements;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetCapacity() const noexcept
	{
		if (!mp_pData)
			return 0;

		return mp_pData->m_Capacity;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_IsEmpty() const noexcept
	{
		return f_GetLen() == 0;
	}

	// Helper accessor views
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Keys() noexcept -> CKeys
	{
		return CKeys(*this);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Keys() const noexcept -> CKeysConst
	{
		return CKeysConst(*this);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Entries() noexcept -> CEntries
	{
		return CEntries(*this);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Entries() const noexcept -> CEntriesConst
	{
		return CEntriesConst(*this);
	}

	// Stats access
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_GetStats() const noexcept -> CStats const &
		requires(t_Options.m_bStats)
	{
		return mp_Stats;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_ResetStats() noexcept
		requires(t_Options.m_bStats)
	{
		mp_Stats = {};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CReciprocalTable::CReciprocalTable() noexcept
	{
		m_Values[0] = 0;
		for (umint i = 1; i < mcp_MaxCalibratorLevels; ++i)
			m_Values[i] = ((umint(1) << mcp_ReciprocalShift) + i - 1) / i;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_ReciprocalDivide(umint _Dividend, umint _Divisor) noexcept
	{
		return (_Dividend * mcp_ReciprocalTable.m_Values[_Divisor]) >> mcp_ReciprocalShift;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_ScaledMulFloor(umint _ScaledDensity, umint _Capacity) noexcept
	{
		umint Quotient = _Capacity / mcp_DensityScale;
		umint Remainder = _Capacity % mcp_DensityScale;
		return _ScaledDensity * Quotient + (_ScaledDensity * Remainder) / mcp_DensityScale;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_ScaledMulCeil(umint _ScaledDensity, umint _Capacity) noexcept
	{
		umint Quotient = _Capacity / mcp_DensityScale;
		umint Remainder = _Capacity % mcp_DensityScale;
		return _ScaledDensity * Quotient + (_ScaledDensity * Remainder + mcp_DensityScale - 1) / mcp_DensityScale;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_ScaledDivCeil(umint _nElements, umint _ScaledDensity) noexcept
	{
		umint Quotient = _nElements / _ScaledDensity;
		umint Remainder = _nElements % _ScaledDensity;
		return Quotient * mcp_DensityScale + (Remainder * mcp_DensityScale + _ScaledDensity - 1) / _ScaledDensity;
	}

	// Density-bounded recursive distribution of elements across segments.
	// Splits the [_iStart, _iEnd) range in half, computing per-level density bounds
	// to determine feasible element counts for each half, then recurses until single segments.
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_DistributeElements
		(
			umint *_pTargets
			, umint _iStart
			, umint _iEnd
			, umint _nElements
			, umint _iLocalLevel
			, umint _iWindowLevel
			, umint _nGlobalLevels
		)
		const noexcept
	{
		umint nSegs = _iEnd - _iStart;
		if (nSegs <= 0)
			return;

		if (nSegs == 1)
		{
			_pTargets[_iStart] = fg_Min(_nElements, mcp_SegmentSize);
			return;
		}

		umint iMid = _iStart + nSegs / 2;
		umint nLeftSegs = iMid - _iStart;
		umint nRightSegs = _iEnd - iMid;

		umint CapLeft = nLeftSegs * mcp_SegmentSize;
		umint CapRight = nRightSegs * mcp_SegmentSize;

		// Children of a node at level L are constrained by level L-1 thresholds.
		// _iLocalLevel = 0 means we're splitting the root window, so use (_iWindowLevel - 1).
		umint iGlobalLevel = _iWindowLevel - _iLocalLevel - 1;
		if (iGlobalLevel < 0)
			iGlobalLevel = 0;

		auto DensityBounds = fp_GetLevelDensityBounds(iGlobalLevel, _nGlobalLevels);
		umint nMinLeft, nMaxLeft;
		fp_ElementBoundsFromDensity(DensityBounds, CapLeft, nMinLeft, nMaxLeft);
		umint nMinRight, nMaxRight;
		fp_ElementBoundsFromDensity(DensityBounds, CapRight, nMinRight, nMaxRight);

		// Capacity-based feasible interval for left count
		umint CapLeftMin = (_nElements > CapRight) ? (_nElements - CapRight) : (umint)0;
		umint CapLeftMax = fg_Min(CapLeft, _nElements);

		// Intersect density bounds (both sides) with capacity bounds.
		umint nLeftMin = fg_Max(nMinLeft, CapLeftMin);
		umint nLeftMax = fg_Min(nMaxLeft, CapLeftMax);
		umint nLeftFromRightMax = (_nElements > nMaxRight) ? (_nElements - nMaxRight) : (umint)0;
		umint nLeftFromRightMin = (_nElements > nMinRight) ? (_nElements - nMinRight) : (umint)0;
		nLeftMin = fg_Max(nLeftMin, nLeftFromRightMax);
		nLeftMax = fg_Min(nLeftMax, nLeftFromRightMin);

		if (nLeftMin > nLeftMax)
		{
			// Density bounds can become integer-infeasible for tiny windows.
			// Retry with one-element slack before falling back to capacity-only.
			umint nMinLeftSlack = (nMinLeft > 0) ? (nMinLeft - 1) : (umint)0;
			umint nMaxLeftSlack = (nMaxLeft < CapLeft) ? (nMaxLeft + 1) : CapLeft;
			umint nMinRightSlack = (nMinRight > 0) ? (nMinRight - 1) : (umint)0;
			umint nMaxRightSlack = (nMaxRight < CapRight) ? (nMaxRight + 1) : CapRight;

			nLeftMin = fg_Max(nMinLeftSlack, CapLeftMin);
			nLeftMax = fg_Min(nMaxLeftSlack, CapLeftMax);
			umint nLeftFromRightMaxSlack = (_nElements > nMaxRightSlack) ? (_nElements - nMaxRightSlack) : (umint)0;
			umint nLeftFromRightMinSlack = (_nElements > nMinRightSlack) ? (_nElements - nMinRightSlack) : (umint)0;
			nLeftMin = fg_Max(nLeftMin, nLeftFromRightMaxSlack);
			nLeftMax = fg_Min(nLeftMax, nLeftFromRightMinSlack);

			if (nLeftMin > nLeftMax)
			{
				nLeftMin = CapLeftMin;
				nLeftMax = CapLeftMax;
			}
		}

		// Proportional split: nLeftSegs == nSegs >> 1, so the ratio is
		// always ~1/2. Use _nElements >> 1 and let clamping adjust.
		umint nLeft = _nElements >> 1;
		nLeft = fg_Min(fg_Max(nLeft, nLeftMin), nLeftMax);

		fp_DistributeElements(_pTargets, _iStart, iMid, nLeft, _iLocalLevel + 1, _iWindowLevel, _nGlobalLevels);
		fp_DistributeElements(_pTargets, iMid, _iEnd, _nElements - nLeft, _iLocalLevel + 1, _iWindowLevel, _nGlobalLevels);
	}
}
