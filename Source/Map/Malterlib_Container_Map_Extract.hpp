// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_ExtractAndInsert(TCMap &_Map, CUserData *_pData)
		requires (!NTraits::cIsReference<t_CValue>) //This function is not supported when mapping reference types
	{
		f_Insert(_Map.f_Extract(_pData));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Extract(tf_CKey &&_Key) -> CNodeHandle
	{
		CNodeDestructive *pNode = mp_Tree.f_FindEqualAndRemove(_Key, mp_Compare);
		return CNodeHandle(pNode, mp_Allocator);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Extract(CUserData *_pData) -> CNodeHandle
	{
		umint Offset = CNode::fs_GetOffset();
		CNodeDestructive *pNode = (CNodeDestructive *)(((uint8 *)_pData) - Offset);
		mp_Tree.f_Remove(pNode, mp_Compare);
		return CNodeHandle(pNode, mp_Allocator);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_FOnNode>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_ExtractAll(tf_FOnNode &&_fOnNode)
	{
		mp_Tree.f_DeleteAll
			(
				[&](CNodeDestructive *_pNode)
				{
					_fOnNode(CNodeHandle(_pNode, mp_Allocator));
				}
			)
		;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fp_ExtractNoRebalance(CNodeDestructive *_pParent, CNodeDestructive *_pToRemove) -> CNodeHandle
	{
		if (_pParent)
			mp_Tree.f_RemoveNoRebalance(_pParent, _pToRemove);
		else
			mp_Tree.f_RemoveNoRebalanceRoot(_pToRemove);
		return CNodeHandle(_pToRemove, mp_Allocator);
	}
}
