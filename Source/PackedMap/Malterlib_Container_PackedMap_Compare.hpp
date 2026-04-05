// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	// Advance scan cursor to the next contiguous slot range using clustered segment pairs
	template <typename tf_FGetFirstSlot>
	constexpr void fsg_PackedMapAdvanceScanRange
		(
			umint &_iSeg
			, umint &_iSlot
			, umint &_iSlotEnd
			, auto const *_pMeta
			, umint _nSegments
			, tf_FGetFirstSlot &&_fGetFirstSlot
		)
	{
		while (_iSeg < _nSegments)
		{
			umint iSeg0 = _iSeg;
			umint iSeg1 = _iSeg + 1;
			_iSeg += 2;

			umint Count0 = _pMeta[iSeg0].m_Count;
			umint Count1 = (iSeg1 < _nSegments) ? _pMeta[iSeg1].m_Count : 0;

			if (Count0 == 0 && Count1 == 0)
				continue;

			if (Count1 == 0 || iSeg1 >= _nSegments)
			{
				if (Count0 == 0)
					continue;
				_iSlot = _fGetFirstSlot(iSeg0, Count0);
				_iSlotEnd = _iSlot + Count0;
				return;
			}

			if (Count0 == 0)
			{
				_iSlot = _fGetFirstSlot(iSeg1, Count1);
				_iSlotEnd = _iSlot + Count1;
				return;
			}

			_iSlot = _fGetFirstSlot(iSeg0, Count0);
			_iSlotEnd = _fGetFirstSlot(iSeg1, Count1) + Count1;
			return;
		}
	}

	// Equality comparison
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::operator ==
		(
			TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> const &_Other
		)
		const noexcept
		(
			noexcept(fg_GetType<t_CKey const &>() == fg_GetType<tf_CKey const &>())
			&& noexcept(fg_GetType<t_CValue const &>() == fg_GetType<tf_CValue const &>())
		)
	{
		auto *pData = mp_pData;
		auto *pDataOther = _Other.mp_pData;

		umint nLen = pData ? pData->m_nElements : 0;
		umint nLenOther = pDataOther ? pDataOther->m_nElements : 0;
		if (nLen != nLenOther)
			return false;
		if (nLen == 0)
			return true;

		auto *pKeys = pData->m_pKeys;
		auto *pValues = pData->m_pValues;
		auto *pOtherKeys = pDataOther->m_pKeys;
		auto *pOtherValues = pDataOther->m_pValues;

		using COtherMap = TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options>;

		umint iSeg = 0, iSlot = 0, iSlotEnd = 0;
		umint iOtherSeg = 0, iOtherSlot = 0, iOtherSlotEnd = 0;

		for (umint iEntry = 0; iEntry < nLen; ++iEntry)
		{
			if (iSlot >= iSlotEnd)
				fsg_PackedMapAdvanceScanRange(iSeg, iSlot, iSlotEnd, pData->m_pSegmentMeta, pData->m_nSegments, fsp_GetSegmentFirstSlot);
			if (iOtherSlot >= iOtherSlotEnd)
				fsg_PackedMapAdvanceScanRange(iOtherSeg, iOtherSlot, iOtherSlotEnd, pDataOther->m_pSegmentMeta, pDataOther->m_nSegments, COtherMap::fsp_GetSegmentFirstSlot);

			if (!(pKeys[iSlot] == pOtherKeys[iOtherSlot]))
				return false;

			if (!(pValues[iSlot] == pOtherValues[iOtherSlot]))
				return false;

			++iSlot;
			++iOtherSlot;
		}

		return true;
	}

	// Three-way comparison (non-lexicographic: early out on different length)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::operator <=>
		(
			TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> const &_Other
		)
		const noexcept
		(
			noexcept(fg_GetType<t_CKey const &>() <=> fg_GetType<tf_CKey const &>())
			&& noexcept(fg_GetType<t_CValue const &>() <=> fg_GetType<tf_CValue const &>())
		)
	{
		using COrdering = TCCommonOrderingType
			<
				decltype(fg_GetType<t_CKey const &>() <=> fg_GetType<tf_CKey const &>())
				, decltype(fg_GetType<t_CValue const &>() <=> fg_GetType<tf_CValue const &>())
			>
		;

		auto *pData = mp_pData;
		auto *pDataOther = _Other.mp_pData;

		umint nLen = pData ? pData->m_nElements : 0;
		umint nLenOther = pDataOther ? pDataOther->m_nElements : 0;

		auto LenResult = nLen <=> nLenOther;
		if (LenResult != 0)
			return COrdering(LenResult);

		if (nLen == 0)
			return COrdering::equivalent;

		auto *pKeys = pData->m_pKeys;
		auto *pValues = pData->m_pValues;
		auto *pOtherKeys = pDataOther->m_pKeys;
		auto *pOtherValues = pDataOther->m_pValues;

		using COtherMap = TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options>;

		umint iSeg = 0, iSlot = 0, iSlotEnd = 0;
		umint iOtherSeg = 0, iOtherSlot = 0, iOtherSlotEnd = 0;

		for (umint iEntry = 0; iEntry < nLen; ++iEntry)
		{
			if (iSlot >= iSlotEnd)
				fsg_PackedMapAdvanceScanRange(iSeg, iSlot, iSlotEnd, pData->m_pSegmentMeta, pData->m_nSegments, fsp_GetSegmentFirstSlot);
			if (iOtherSlot >= iOtherSlotEnd)
				fsg_PackedMapAdvanceScanRange(iOtherSeg, iOtherSlot, iOtherSlotEnd, pDataOther->m_pSegmentMeta, pDataOther->m_nSegments, COtherMap::fsp_GetSegmentFirstSlot);

			auto KeyComp = pKeys[iSlot] <=> pOtherKeys[iOtherSlot];
			if (KeyComp != 0)
				return COrdering(KeyComp);

			auto ValueComp = pValues[iSlot] <=> pOtherValues[iOtherSlot];
			if (ValueComp != 0)
				return COrdering(ValueComp);

			++iSlot;
			++iOtherSlot;
		}

		return COrdering::equivalent;
	}

	// Lexicographical comparison returning a proper ordering type
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_CompareLexicographical
		(
			TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> const &_Other
		)
		const noexcept
		(
			noexcept(fg_GetType<t_CKey const &>() <=> fg_GetType<tf_CKey const &>())
			&& noexcept(fg_GetType<t_CValue const &>() <=> fg_GetType<tf_CValue const &>())
		)
	{
		using COrdering = TCCommonOrderingType
			<
				decltype(fg_GetType<t_CKey const &>() <=> fg_GetType<tf_CKey const &>())
				, decltype(fg_GetType<t_CValue const &>() <=> fg_GetType<tf_CValue const &>())
			>
		;

		auto *pData = mp_pData;
		auto *pDataOther = _Other.mp_pData;

		umint nLen = pData ? pData->m_nElements : 0;
		umint nLenOther = pDataOther ? pDataOther->m_nElements : 0;
		if (nLen == 0 || nLenOther == 0)
			return COrdering(nLen <=> nLenOther);

		auto *pKeys = pData->m_pKeys;
		auto *pValues = pData->m_pValues;
		auto *pOtherKeys = pDataOther->m_pKeys;
		auto *pOtherValues = pDataOther->m_pValues;

		using COtherMap = TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options>;

		umint iSeg = 0, iSlot = 0, iSlotEnd = 0;
		umint iOtherSeg = 0, iOtherSlot = 0, iOtherSlotEnd = 0;

		umint nMinLen = nLen < nLenOther ? nLen : nLenOther;
		for (umint iEntry = 0; iEntry < nMinLen; ++iEntry)
		{
			if (iSlot >= iSlotEnd)
				fsg_PackedMapAdvanceScanRange(iSeg, iSlot, iSlotEnd, pData->m_pSegmentMeta, pData->m_nSegments, fsp_GetSegmentFirstSlot);
			if (iOtherSlot >= iOtherSlotEnd)
				fsg_PackedMapAdvanceScanRange(iOtherSeg, iOtherSlot, iOtherSlotEnd, pDataOther->m_pSegmentMeta, pDataOther->m_nSegments, COtherMap::fsp_GetSegmentFirstSlot);

			auto KeyComp = pKeys[iSlot] <=> pOtherKeys[iOtherSlot];
			if (KeyComp != 0)
				return COrdering(KeyComp);

			auto ValueComp = pValues[iSlot] <=> pOtherValues[iOtherSlot];
			if (ValueComp != 0)
				return COrdering(ValueComp);

			++iSlot;
			++iOtherSlot;
		}

		return COrdering(nLen <=> nLenOther);
	}
}
