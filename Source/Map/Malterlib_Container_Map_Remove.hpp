// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Remove(CUserData *_pData)
		requires (!NTraits::TCIsReference<t_CValue>::mc_Value) //This function is not supported when mapping reference types
	{
		mint Offset = CNode::fs_GetOffset();
		CNode *pNode = (CNode *)(((uint8 *)_pData) - Offset);
		mp_Tree.f_Remove(pNode);
		fg_DeleteObjectDefiniteType(mp_Allocator, pNode);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	bool TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_TryRemovePointerBasedComparison(CUserData *_pData)
		requires (!NTraits::TCIsReference<t_CValue>::mc_Value) //This function is not supported when mapping reference types
	{
		mint Offset = CNode::fs_GetOffset();
		CNode *pNode = (CNode *)(((uint8 *)_pData) - Offset);
		auto *pRemoved = mp_Tree.f_FindEqualAndRemove(pNode->mp_Key);
		if (!pRemoved)
			return false;
		DMibFastCheck(pRemoved == pNode);
		DMibFastCheck(!mp_Tree.f_FindEqual(pNode->mp_Key));
		fg_DeleteObjectDefiniteType(mp_Allocator, pRemoved);
		return true;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	bool TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Remove(tf_CKey &&_Key)
	{
		CNode *pNode = mp_Tree.f_FindEqualAndRemove(_Key);
		if (pNode)
		{
			fg_DeleteObjectDefiniteType(mp_Allocator, pNode);
			return true;
		}
		return false;
	}
}
