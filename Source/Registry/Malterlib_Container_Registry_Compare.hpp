// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::operator == (TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> const &_Other) const
	{
		if (!(f_GetName() == _Other.f_GetName()))
			return false;
		if (!(f_GetThisValue() == _Other.f_GetThisValue()))
			return false;
		auto Iter = f_GetChildIterator();
		auto IterOther = _Other.f_GetChildIterator();
		while (Iter && IterOther)
		{
			if (!(*Iter == *IterOther))
				return false;
			++Iter;
			++IterOther;
		}
		if (IterOther || Iter)
			return false;
		return true;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::operator < (TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> const &_Other) const
	{
		NMisc::ECompareResult Compare = NMisc::fg_Compare(f_GetName(), _Other.f_GetName());
		Compare = NMisc::fg_Compare(Compare, f_GetThisValue(), _Other.f_GetThisValue());
		if (Compare)
			return Compare < 0;

		auto Iter = f_GetChildIterator();
		auto IterOther = _Other.f_GetChildIterator();
		while (Iter && IterOther)
		{
			Compare = NMisc::fg_Compare(*Iter, *IterOther);
			if (Compare)
				return Compare < 0;
			++Iter;
			++IterOther;
		}
		if (IterOther && !Iter)
			return false;
		if (!IterOther && Iter)
			return true;
		return false;
	}
}
