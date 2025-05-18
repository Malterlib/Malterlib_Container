// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_ExtractAndInsert(TCMap &_Map, CUserData *_pData)
		requires (!NTraits::cIsReference<t_CValue>) //This function is not supported when mapping reference types
	{
		mint Offset = CNode::fs_GetOffset();
		CNode *pNode = (CNode *)(((uint8 *)_pData) - Offset);
		_Map.mp_Tree.f_Remove(pNode, _Map.mp_Compare);
		mp_Tree.f_Insert(pNode, mp_Compare);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Extract(tf_CKey &&_Key) -> CNodeHandle
	{
		CNode *pNode = mp_Tree.f_FindEqualAndRemove(_Key, mp_Compare);
		return CNodeHandle(pNode, mp_Allocator);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Extract(CUserData *_pData) -> CNodeHandle
	{
		mint Offset = CNode::fs_GetOffset();
		CNode *pNode = (CNode *)(((uint8 *)_pData) - Offset);
		mp_Tree.f_Remove(pNode, mp_Compare);
		return CNodeHandle(pNode, mp_Allocator);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_FOnNode>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_ExtractAll(tf_FOnNode &&_fOnNode)
	{
		mp_Tree.f_DeleteAll
			(
				[&](CNode *_pNode)
				{
					_fOnNode(CNodeHandle(_pNode, mp_Allocator));
				}
			)
		;
	}
}
