// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	TCRegistry<t_CStr, t_CData, t_Flags>::TCRegistry(TCRegistry const &_Source)
		: mp_Key((TCRegistry *)nullptr) // No parent
		, mp_pParent(nullptr)
	{
		mp_Key.f_Copy(_Source.mp_Key);
		mp_Data = _Source.mp_Data;
		auto Iter = _Source.mp_Children.f_GetIterator();

		while (Iter)
		{
			TCRegistry *pChild;
			pChild = DMibNew TCRegistry(this);
			*pChild = *Iter;
			++Iter;
		}
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::operator = (TCRegistry const &_Source) -> TCRegistry &
	{
		if (mp_pParent && mp_ChildLink.f_IsInTree())
			mp_pParent->mp_Children.f_Remove(this);
		mp_Key.f_Copy(_Source.mp_Key);

		if (mp_pParent)
		{
			TCRegistry *pOldChild = mp_pParent->mp_Children.m_Tree.f_FindEqual(mp_Key);
			if (pOldChild)
				delete pOldChild;
			mp_pParent->mp_Children.f_Insert(this);
		}
		mp_Data = _Source.mp_Data;
		mp_Children.m_Tree.f_DeleteAllDefiniteType();

		auto Iter = _Source.mp_Children.f_GetIterator();
		while (Iter)
		{
			TCRegistry *pChild;
			pChild = DMibNew TCRegistry(this);
			*pChild = *Iter;
			++Iter;
		}

		return *this;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::fp_Copy(TCRegistry<tf_CStr, tf_CData, tf_Flags> const &_Source)
	{
		f_SetThisValue(_Source.f_GetThisValue());
		if constexpr (mc_bSupportLocation)
		{
			f_SetLocation(_Source.f_GetLocation());
			if constexpr (t_Flags & ERegistryFlag_FullLocation)
				f_SetValueLocation(_Source.f_GetValueLocation());
		}
		if constexpr (mc_bSupportWhiteSpace)
		{
			f_SetForceEscapedKey(_Source.f_GetForceEscapedKey());
			f_SetForceEscapedValue(_Source.f_GetForceEscapedValue());
			for (mint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
			{
				f_SetWhiteSpace((ERegistryWhiteSpaceLocation)i, _Source.f_GetWhiteSpace((ERegistryWhiteSpaceLocation)i));
				mp_Key.f_SetParsed((ERegistryWhiteSpaceLocation)i, _Source.mp_Key.f_GetParsed((ERegistryWhiteSpaceLocation)i));
			}
		}

		for (auto iChild = _Source.mp_Children.f_GetIterator(); iChild; ++iChild)
		{
			auto pChild = f_CreateChildNoPath(iChild->f_GetName());
			pChild->fp_Copy(*iChild);
		}
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::operator = (TCRegistry<tf_CStr, tf_CData, tf_Flags> const &_Source) -> TCRegistry &
	{
		f_SetName(_Source.f_GetName());
		fp_Copy(_Source);

		return *this;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::f_SetContents(TCRegistry const &_Source) -> TCRegistry &
	{
		mp_Data = _Source.mp_Data;
		mp_Children.m_Tree.f_DeleteAllDefiniteType();

		auto Iter = _Source.mp_Children.f_GetIterator();
		while (Iter)
		{
			TCRegistry *pChild;
			pChild = DMibNew TCRegistry(this);
			*pChild = *Iter;
			++Iter;
		}

		return *this;
	}
}
