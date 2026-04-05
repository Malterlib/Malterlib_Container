// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	//
	// CKeys Implementation
	//

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CKeys::CKeys(TCPackedMap &_Map) noexcept
		: mp_Map(_Map)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CKeys::f_GetIterator() noexcept -> CKeyIterator
	{
		return CKeyIterator(&mp_Map, mp_Map.f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CKeys::f_GetIteratorReverse() noexcept -> CKeyIteratorReverse
	{
		return CKeyIteratorReverse(&mp_Map, mp_Map.f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CKeys::f_GetIterator(tf_CKey &&_Key) noexcept -> CKeyIterator
	{
		auto FindResult = fsp_Find(mp_Map.mp_pData, mp_Map.mp_Compare, _Key);
		if (!FindResult.m_bExists)
			return CKeyIterator();

		umint Rank = TCPackedMap::fsp_ComputeRank(mp_Map.mp_pData, FindResult.m_iSegment, FindResult.m_iLocalPos);
		CKeyIterator Iter(&mp_Map, mp_Map.f_GetLen());
		Iter.m_iSegment = FindResult.m_iSegment;
		Iter.m_iLocalPos = FindResult.m_iLocalPos;
		Iter.m_iCurrent = Rank;
		return Iter;
	}

	//
	// CKeysConst Implementation
	//

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CKeysConst::CKeysConst(TCPackedMap const &_Map) noexcept
		: mp_Map(_Map)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CKeysConst::f_GetIterator() const noexcept -> CKeyIteratorConst
	{
		return CKeyIteratorConst(&mp_Map, mp_Map.f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CKeysConst::f_GetIteratorReverse() const noexcept -> CKeyIteratorReverseConst
	{
		return CKeyIteratorReverseConst(&mp_Map, mp_Map.f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CKeysConst::f_GetIterator(tf_CKey &&_Key) const noexcept -> CKeyIteratorConst
	{
		auto FindResult = fsp_Find(mp_Map.mp_pData, mp_Map.mp_Compare, _Key);
		if (!FindResult.m_bExists)
			return CKeyIteratorConst();

		umint Rank = TCPackedMap::fsp_ComputeRank(mp_Map.mp_pData, FindResult.m_iSegment, FindResult.m_iLocalPos);
		CKeyIteratorConst Iter(&mp_Map, mp_Map.f_GetLen());
		Iter.m_iSegment = FindResult.m_iSegment;
		Iter.m_iLocalPos = FindResult.m_iLocalPos;
		Iter.m_iCurrent = Rank;
		return Iter;
	}

	//
	// CEntries Implementation
	//

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CEntries::CEntries(TCPackedMap &_Map) noexcept
		: mp_Map(_Map)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CEntries::f_GetIterator() noexcept -> CKeyValueIterator
	{
		return CKeyValueIterator(&mp_Map, mp_Map.f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CEntries::f_GetIteratorReverse() noexcept -> CKeyValueIteratorReverse
	{
		return CKeyValueIteratorReverse(&mp_Map, mp_Map.f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CEntries::f_GetIterator(tf_CKey &&_Key) noexcept -> CKeyValueIterator
	{
		auto FindResult = fsp_Find(mp_Map.mp_pData, mp_Map.mp_Compare, _Key);
		if (!FindResult.m_bExists)
			return CKeyValueIterator();

		umint Rank = TCPackedMap::fsp_ComputeRank(mp_Map.mp_pData, FindResult.m_iSegment, FindResult.m_iLocalPos);
		CKeyValueIterator Iter(&mp_Map, mp_Map.f_GetLen());
		Iter.m_iSegment = FindResult.m_iSegment;
		Iter.m_iLocalPos = FindResult.m_iLocalPos;
		Iter.m_iCurrent = Rank;
		return Iter;
	}

	//
	// CEntriesConst Implementation
	//

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CEntriesConst::CEntriesConst(TCPackedMap const &_Map) noexcept
		: mp_Map(_Map)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CEntriesConst::f_GetIterator() const noexcept -> CKeyValueIteratorConst
	{
		return CKeyValueIteratorConst(&mp_Map, mp_Map.f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CEntriesConst::f_GetIteratorReverse() const noexcept -> CKeyValueIteratorReverseConst
	{
		return CKeyValueIteratorReverseConst(&mp_Map, mp_Map.f_GetLen());
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::CEntriesConst::f_GetIterator(tf_CKey &&_Key) const noexcept -> CKeyValueIteratorConst
	{
		auto FindResult = fsp_Find(mp_Map.mp_pData, mp_Map.mp_Compare, _Key);
		if (!FindResult.m_bExists)
			return CKeyValueIteratorConst();

		umint Rank = TCPackedMap::fsp_ComputeRank(mp_Map.mp_pData, FindResult.m_iSegment, FindResult.m_iLocalPos);
		CKeyValueIteratorConst Iter(&mp_Map, mp_Map.f_GetLen());
		Iter.m_iSegment = FindResult.m_iSegment;
		Iter.m_iLocalPos = FindResult.m_iLocalPos;
		Iter.m_iCurrent = Rank;
		return Iter;
	}
}
