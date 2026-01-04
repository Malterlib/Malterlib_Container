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
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator_SmallestGreaterThanEqual(tf_CKey &&_Key) const -> CIteratorConst
	{
		CIteratorConst Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) -> CIterator
	{
		CIterator Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIterator_LargestLessThanEqual(tf_CKey &&_Key) const -> CIteratorConst
	{
		CIteratorConst Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorBidirectional(tf_CKey &&_Key) -> CIteratorBidirectional
	{
		return {*this, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorBidirectional(tf_CKey &&_Key) const -> CIteratorBidirectionalConst
	{
		return {*this, fg_Forward<tf_CKey>(_Key)};
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key) -> CIteratorBidirectional
	{
		CIteratorBidirectional Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorBidirectional_SmallestGreaterThanEqual(tf_CKey &&_Key) const -> CIteratorBidirectionalConst
	{
		CIteratorBidirectionalConst Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindSmallestGreaterThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key) -> CIteratorBidirectional
	{
		CIteratorBidirectional Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Compare);
		return Iterator;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetIteratorBidirectional_LargestLessThanEqual(tf_CKey &&_Key) const -> CIteratorBidirectionalConst
	{
		CIteratorBidirectionalConst Iterator;
		Iterator.mp_pMap = this;
		Iterator.mp_Iter.f_InitForSearch(mp_Tree);
		Iterator.mp_Iter.f_FindLargestLessThanEqualForward(fg_Forward<tf_CKey>(_Key), mp_Compare);
		return Iterator;
	}
}

namespace NMib::NContainer::NPrivate
{
	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	TCMapIterator<t_CMap, t_Access, t_Flags>::TCMapIterator()
		requires (!(t_Flags & EMapIteratorFlags_Destructive))
	{
		mp_pMap = nullptr;
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	TCMapIterator<t_CMap, t_Access, t_Flags>::TCMapIterator(CMapQualified &_Map)
		requires (!(t_Flags & EMapIteratorFlags_Destructive))
	{
		mp_pMap = &_Map;
		if constexpr (t_Flags & EMapIteratorFlags_Reverse)
			mp_Iter.f_StartBackward(_Map.mp_Tree);
		else
			mp_Iter.f_StartForward(_Map.mp_Tree);
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	TCMapIterator<t_CMap, t_Access, t_Flags>::TCMapIterator(t_CMap &&_Map)
		requires (!!(t_Flags & EMapIteratorFlags_Destructive))
	{
		if constexpr (t_Flags & EMapIteratorFlags_Reverse)
			mp_Iter.f_StartBackward(_Map.mp_Tree);
		else
			mp_Iter.f_StartForward(_Map.mp_Tree);
		mp_pMap = &_Map;
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	template <typename tf_CKey>
	TCMapIterator<t_CMap, t_Access, t_Flags>::TCMapIterator(CMapQualified &_Map, const tf_CKey &_Key)
		requires (!(t_Flags & EMapIteratorFlags_Reverse) && !(t_Flags & EMapIteratorFlags_Destructive)) // Not supported for reverse or destructive iterators
	{
		mp_pMap = &_Map;
		mp_Iter.f_InitForSearch(_Map.mp_Tree);
		mp_Iter.f_FindEqualForward(_Key, _Map.mp_Compare);
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	auto TCMapIterator<t_CMap, t_Access, t_Flags>::operator = (CMapQualified &_Map) -> TCMapIterator &
		requires (!(t_Flags & EMapIteratorFlags_Destructive))
	{
		mp_pMap = &_Map;
		if constexpr (t_Flags & EMapIteratorFlags_Reverse)
			mp_Iter.f_StartBackward(_Map.mp_Tree);
		else
			mp_Iter.f_StartForward(_Map.mp_Tree);
		return *this;
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	inline_medium mint TCMapIterator<t_CMap, t_Access, t_Flags>::f_GetLen() const
	{
		return mp_Iter.f_GetLen();
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	inline_medium void TCMapIterator<t_CMap, t_Access, t_Flags>::f_Next()
	{
		if constexpr (t_Flags & EMapIteratorFlags_Bidirectional)
		{
			if constexpr (t_Flags & EMapIteratorFlags_Reverse)
				mp_Iter.f_PrevBidirectional();
			else
				mp_Iter.f_NextBidirectional();
		}
		else
		{
			if constexpr (t_Flags & EMapIteratorFlags_Reverse)
				mp_Iter.f_Prev();
			else
				mp_Iter.f_Next();
		}
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	inline_medium void TCMapIterator<t_CMap, t_Access, t_Flags>::f_Prev()
		requires (!!(t_Flags & EMapIteratorFlags_Bidirectional)) // Only available when bidirectional
	{
		if constexpr (t_Flags & EMapIteratorFlags_Reverse)
			mp_Iter.f_NextBidirectional();
		else
			mp_Iter.f_PrevBidirectional();

	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	void TCMapIterator<t_CMap, t_Access, t_Flags>::f_Remove()
		requires (!(t_Flags & EMapIteratorFlags_Destructive)) // Not available for destructive iterators
	{
		mp_Iter.f_DeleteAllocatorDefiniteType(mp_pMap->mp_Tree, mp_pMap->mp_Compare, mp_pMap->mp_Allocator);
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	typename t_CMap::CNodeHandle TCMapIterator<t_CMap, t_Access, t_Flags>::f_ExtractNode()
		requires (!!(t_Flags & EMapIteratorFlags_Destructive) && !!(t_Flags & EMapIteratorFlags_Bidirectional))
	{
		using CTreeNode = typename t_CMap::CNodeDestructive;
		using CNodeHandle = typename t_CMap::CNodeHandle;

		DMibFastCheck(mp_Iter.f_GetCurrent()); // Cannot extract node from exhausted iterator

		// Use the AVL iterator's extraction which handles advancing and stack patching
		CTreeNode *pExtracted;
		if constexpr (t_Flags & EMapIteratorFlags_Reverse)
			pExtracted = mp_Iter.template f_ExtractCurrentNoRebalance<true>(mp_pMap->mp_Tree);
		else
			pExtracted = mp_Iter.template f_ExtractCurrentNoRebalance<false>(mp_pMap->mp_Tree);

		return CNodeHandle(pExtracted, mp_pMap->mp_Allocator);
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	inline_small auto TCMapIterator<t_CMap, t_Access, t_Flags>::f_GetCurrent() const -> CUserDataQualified *
	{
		auto *pNode = mp_Iter.f_GetCurrent();
		if (pNode)
		{
			if constexpr (t_Access == EMapIteratorAccess_Value)
				return &pNode->f_Value();
			else if constexpr (t_Access == EMapIteratorAccess_Key)
			{
				if constexpr (t_Flags & EMapIteratorFlags_Destructive)
					return &pNode->f_Key();
				else
					return &static_cast<typename t_CMap::CNode const *>(pNode)->f_Key();
			}
			else if constexpr (t_Access == EMapIteratorAccess_KeyValue)
			{
				if constexpr (t_Flags & EMapIteratorFlags_Destructive)
					return pNode;
				else
				{
					if constexpr (t_Flags & EMapIteratorFlags_Const)
						return static_cast<typename t_CMap::CNode const *>(pNode);
					else
						return static_cast<typename t_CMap::CNode *>(pNode);
				}
			}
		}
		return nullptr;
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	auto TCMapIterator<t_CMap, t_Access, t_Flags>::f_GetKey() const -> CKey const &
		requires (t_Access == EMapIteratorAccess_Value)
	{
		return mp_Iter->mp_Key;
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	TCMapIterator<t_CMap, t_Access, t_Flags>::operator TCMapIterator<t_CMap, t_Access, t_Flags>::CUserDataQualified * () const
	{
		return f_GetCurrent();
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	inline_small auto TCMapIterator<t_CMap, t_Access, t_Flags>::operator ->() const -> CUserDataQualified *
	{
		return f_GetCurrent();
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	inline_small void TCMapIterator<t_CMap, t_Access, t_Flags>::operator ++()
	{
		f_Next();
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	inline_small void TCMapIterator<t_CMap, t_Access, t_Flags>::operator --()
		requires (!!(t_Flags & EMapIteratorFlags_Bidirectional))
	{
		f_Prev();
	}

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	TCMapIterator<t_CMap, t_Access, t_Flags>::operator bool () const
	{
		return f_GetCurrent() != nullptr;
	}
}
