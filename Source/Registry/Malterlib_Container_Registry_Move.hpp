// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::TCRegistry(TCRegistry &&_Source)
		: mp_Key((TCRegistry *)nullptr)
		, mp_Data(fg_Move(_Source.mp_Data))
		, mp_pParent(nullptr)
		, mp_Children(fg_Move(_Source.mp_Children))
	{
		if (_Source.mp_pParent && _Source.mp_ChildLink.f_IsInTree())
			mp_Key.f_Copy(_Source.mp_Key); // We need to copy the key as this would break the tree
		else
			mp_Key.f_Move(fg_Move(_Source.mp_Key));
		auto Iter = mp_Children.f_GetIterator();
		while (Iter)
		{
			Iter->mp_pParent = this;
			++Iter;
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::operator = (TCRegistry &&_Source) -> TCRegistry &
	{
		if (mp_pParent && mp_ChildLink.f_IsInTree())
			mp_pParent->mp_Children.f_Remove(this);

		if (_Source.mp_pParent && _Source.mp_ChildLink.f_IsInTree())
			mp_Key.f_Copy(_Source.mp_Key); // In this case we need to copy the key otherwise the tree would break
		else
			mp_Key.f_Move(fg_Move(_Source.mp_Key));

		if (mp_pParent)
		{
			TCRegistry *pOldChild = mp_pParent->mp_Children.m_Tree.f_FindEqual(mp_Key);
			if (pOldChild)
				delete pOldChild;
			mp_pParent->mp_Children.f_Insert(this);
		}

		mp_Data = fg_Move(_Source.mp_Data);
		mp_Children.m_Tree.f_DeleteAllDefiniteType();
		mp_Children = fg_Move(_Source.mp_Children);

		for (auto &Child : mp_Children)
			Child.mp_pParent = this;

		return *this;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetContents(TCRegistry &&_Source) -> TCRegistry &
	{
		mp_Data = fg_Move(_Source.mp_Data);
		mp_Children.m_Tree.f_DeleteAllDefiniteType();
		mp_Children = fg_Move(_Source.mp_Children);
		auto Iter = mp_Children.f_GetIterator();
		while (Iter)
		{
			Iter->mp_pParent = this;
			++Iter;
		}
		return *this;
	}
}
