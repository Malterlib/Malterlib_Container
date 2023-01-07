// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator += (TCMap const &_Other) -> TCMap &
	{
		DMibSafeCheck(this != &_Other, "Must not add same object to itself");
		fp_CopyAddAll(_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator += (TCMap &&_Other) -> TCMap &
	{
		DMibSafeCheck(this != &_Other, "Must not add same object to itself");
		fp_MoveAddAll(_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator -= (TCMap const &_Other) -> TCMap &
	{
		DMibSafeCheck(this != &_Other, "Must not remove same object from itself");
		CIteratorConst Iter = _Other.f_GetIterator();
		while (Iter)
		{
			f_Remove(Iter.f_GetKey());
			++Iter;
		}

		return *this;
	}
}
