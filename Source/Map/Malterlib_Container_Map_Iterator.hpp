// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator() const -> CIteratorConst 
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator() -> CIterator 
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorReverse() const -> CIteratorReverseConst 
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorReverse() -> CIteratorReverse 
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorBidirectional() const -> CIteratorBidirectionalConst 
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorBidirectional() -> CIteratorBidirectional 
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorBidirectionalReverse() const -> CIteratorBidirectionalReverseConst
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorBidirectionalReverse() -> CIteratorBidirectionalReverse
	{
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator(tf_CKey &&_Key) -> CIterator
	{
		return {*this, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator(tf_CKey &&_Key) const -> CIteratorConst
	{
		return {*this, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) -> CIterator
	{
		CIterator Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key));
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) const -> CIteratorConst
	{
		CIteratorConst Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key));
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) -> CIterator
	{
		CIterator Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key));
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) const -> CIteratorConst
	{
		CIteratorConst Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key));
		return Iterator;
	}

}

namespace NMib::NContainer::NPrivate
{
	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::TCMapIterator()
	{
		mp_pMap = nullptr;
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::TCMapIterator(CMapQualified &_Map)
	{
		mp_pMap = &_Map;
		if constexpr (t_bReverse)
			mp_Iter.f_StartBackward(_Map.mp_Tree);
		else
			mp_Iter.f_StartForward(_Map.mp_Tree);
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	template <typename tf_CKey>
	TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::TCMapIterator(CMapQualified &_Map, const tf_CKey &_Key)
		requires (!t_bReverse) // Not supported for reverse iterators
	{
		mp_pMap = &_Map;
		mp_Iter.f_InitForSearch(_Map.mp_Tree);
		mp_Iter.f_FindEqualForward(fg_Forward<tf_CKey>(_Key));
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	auto TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::operator = (CMapQualified &_Map) -> TCMapIterator &
	{
		mp_pMap = &_Map;
		if constexpr (t_bReverse)
			mp_Iter.f_StartBackward(_Map.mp_Tree);
		else
			mp_Iter.f_StartForward(_Map.mp_Tree);
		return *this;
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	inline_medium mint TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::f_GetLen() const
	{
		return mp_Iter.f_GetLen();
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	inline_medium void TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::f_Next()
	{
		if constexpr (t_bBidirectional)
		{
			if constexpr (t_bReverse)
				mp_Iter.f_PrevBidirectional();
			else
				mp_Iter.f_NextBidirectional();
		}
		else
		{
			if constexpr (t_bReverse)
				mp_Iter.f_Prev();
			else
				mp_Iter.f_Next();
		}
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	inline_medium void TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::f_Prev()
		requires (t_bBidirectional) // Only available when bidirectional
	{
		if constexpr (t_bReverse)
			mp_Iter.f_NextBidirectional();
		else
			mp_Iter.f_PrevBidirectional();

	}
	
	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	void TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::f_Remove()
	{
		mp_Iter.f_DeleteAllocatorDefiniteType(mp_pMap->mp_Tree, CNodeCompare(), mp_pMap->mp_Allocator);
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	inline_small auto TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::f_GetCurrent() const -> CUserDataQualified *
	{
		CNode *pNode = mp_Iter;
		if (pNode)
			return &pNode->f_Value();
		return nullptr;
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	auto TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::f_GetKey() const -> CKey const &
	{
		return mp_Iter->m_Key;
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::operator TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::CUserDataQualified * () const
	{
		return f_GetCurrent();
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	inline_small auto TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::operator ->() const -> CUserDataQualified *
	{
		return f_GetCurrent();
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	inline_small void TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::operator ++()
	{
		f_Next();
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	inline_small void TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::operator --()
		requires (t_bBidirectional) // Only available when bidirectional
	{
		f_Prev();
	}

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	TCMapIterator<t_CMap, t_bReverse, t_bConst, t_bBidirectional>::operator bool () const
	{
		return f_GetCurrent() != nullptr;
	}
}
