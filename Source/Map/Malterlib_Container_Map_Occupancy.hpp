// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	bool TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_IsEmpty() const
	{
		return mp_Tree.f_IsEmpty();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	bool TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_HasOneElement() const
	{
		return mp_Tree.f_HasOneElement();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	bool TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_HasMoreThanOneElement() const
	{
		return !mp_Tree.f_IsEmpty() && !mp_Tree.f_HasOneElement();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	mint TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetLen() const
	{
		CIteratorConst Iter = *this;
		return Iter.f_GetLen();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	mint TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_GetDepth() const
	{
		return mp_Tree.f_GetDepth();
	}
}
