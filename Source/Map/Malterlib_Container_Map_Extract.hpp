// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_ExtractAndInsert(TCMap &_Map, CUserData *_pData)
		requires (!NTraits::TCIsReference<t_CValue>::mc_Value) //This function is not supported when mapping reference types
	{
		mint Offset = CNode::fs_GetOffset();
		CNode *pNode = (CNode *)(((uint8 *)_pData) - Offset);
		_Map.mp_Tree.f_Remove(pNode);
		mp_Tree.f_Insert(pNode);
	}
}
