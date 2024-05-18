// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::CKeys(TCMap &_Map)
		: mp_Map(_Map)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::CKeysConst(TCMap const &_Map)
		: mp_Map(_Map)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::CEntries(TCMap &_Map)
		: mp_Map(_Map)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::CEntriesConst(TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator> const &_Map)
		: mp_Map(_Map)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Entries() -> CEntries
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Entries() const -> CEntriesConst
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Keys() -> CKeys
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Keys() const -> CKeysConst
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::f_GetIterator() -> CKeyIterator
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::f_GetIteratorReverse() -> CKeyIteratorReverse
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::f_GetIteratorBidirectional() -> CKeyIteratorBidirectional
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::f_GetIteratorBidirectionalReverse() -> CKeyIteratorBidirectionalReverse
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::f_GetIterator(tf_CKey &&_Key) -> CKeyIterator
	{
		return {mp_Map, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) -> CKeyIterator
	{
		CKeyIterator Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) -> CKeyIterator
	{
		CKeyIterator Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::f_GetIteratorBidirectional(tf_CKey &&_Key) -> CKeyIteratorBidirectional
	{
		return {mp_Map, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key) -> CKeyIteratorBidirectional
	{
		CKeyIteratorBidirectional Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeys::f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key) -> CKeyIteratorBidirectional
	{
		CKeyIteratorBidirectional Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::f_GetIterator() const -> CKeyIteratorConst
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::f_GetIteratorReverse() const -> CKeyIteratorReverseConst
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::f_GetIteratorBidirectional() const -> CKeyIteratorBidirectionalConst
	{
		return {mp_Map};
	}


	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::f_GetIteratorBidirectionalReverse() const -> CKeyIteratorBidirectionalReverseConst
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::f_GetIterator(tf_CKey &&_Key) const -> CKeyIteratorConst
	{
		return {mp_Map, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) const -> CKeyIteratorConst
	{
		CKeyIteratorConst Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) const -> CKeyIteratorConst
	{
		CKeyIteratorConst Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::f_GetIteratorBidirectional(tf_CKey &&_Key) const -> CKeyIteratorBidirectionalConst
	{
		return {mp_Map, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key) const -> CKeyIteratorBidirectionalConst
	{
		CKeyIteratorBidirectionalConst Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CKeysConst::f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key) const -> CKeyIteratorBidirectionalConst
	{
		CKeyIteratorBidirectionalConst Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::f_GetIterator() -> CKeyValueIterator
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::f_GetIteratorReverse() -> CKeyValueIteratorReverse
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::f_GetIteratorBidirectional() -> CKeyValueIteratorBidirectional
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::f_GetIteratorBidirectionalReverse() -> CKeyValueIteratorBidirectionalReverse
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::f_GetIterator(tf_CKey &&_Key) -> CKeyValueIterator
	{
		return {mp_Map, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) -> CKeyValueIterator
	{
		CKeyValueIterator Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) -> CKeyValueIterator
	{
		CKeyValueIterator Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::f_GetIteratorBidirectional(tf_CKey &&_Key) -> CKeyValueIteratorBidirectional
	{
		return {mp_Map, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key) -> CKeyValueIteratorBidirectional
	{
		CKeyValueIteratorBidirectional Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntries::f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key) -> CKeyValueIteratorBidirectional
	{
		CKeyValueIteratorBidirectional Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::f_GetIterator() const -> CKeyValueIteratorConst
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::f_GetIteratorReverse() const -> CKeyValueIteratorReverseConst
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::f_GetIteratorBidirectional() const -> CKeyValueIteratorBidirectionalConst
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::f_GetIteratorBidirectionalReverse() const -> CKeyValueIteratorBidirectionalReverseConst
	{
		return {mp_Map};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::f_GetIterator(tf_CKey &&_Key) const -> CKeyValueIteratorConst
	{
		return {mp_Map, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) const -> CKeyValueIteratorConst
	{
		CKeyValueIteratorConst Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) const -> CKeyValueIteratorConst
	{
		CKeyValueIteratorConst Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::f_GetIteratorBidirectional(tf_CKey &&_Key) const -> CKeyValueIteratorBidirectionalConst
	{
		return {mp_Map, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key) const -> CKeyValueIteratorBidirectionalConst
	{
		CKeyValueIteratorBidirectionalConst Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::CEntriesConst::f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key) const -> CKeyValueIteratorBidirectionalConst
 	{
		CKeyValueIteratorBidirectionalConst Iterator;
		Iterator.mp_pMap = &mp_Map;
		Iterator.mp_Iter.f_InitForSearch(mp_Map.mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Map.mp_Compare);
		return Iterator;
	}
}
