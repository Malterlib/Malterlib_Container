// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fs_GetKey(CUserData const *_pData) -> t_CKey const &
		requires (!NTraits::cIsReference<t_CValue>) //This function is not supported when mapping reference types
	{
		umint Offset = CNode::fs_GetOffset();
		const CNode *pNode = (const CNode *)(((uint8 *)_pData) - Offset);
		return pNode->mp_Key;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::fs_GetKey(const CUserData &_Data) -> t_CKey const &
		requires (!NTraits::cIsReference<t_CValue>) //This function is not supported when mapping reference types
	{
		umint Offset = CNode::fs_GetOffset();
		const CNode *pNode = (const CNode *)(((uint8 *)(&_Data)) - Offset);;
		return pNode->mp_Key;
	}
}
