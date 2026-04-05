// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::TCRegistry(TCRegistry const &_Source)
		: mp_Key((TCRegistry *)nullptr) // No parent
		, mp_pParent(nullptr)
	{
		mp_Key.f_Copy(_Source.mp_Key);
		mp_Data = _Source.mp_Data;
		auto Iter = _Source.mp_Children.f_GetIterator();

		while (Iter)
		{
			TCRegistry *pChild;
			pChild = fg_ConstructObject<TCRegistry>(NMemory::CDefaultAllocator(), this);
			*pChild = *Iter;
			++Iter;
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::operator = (TCRegistry const &_Source) -> TCRegistry &
	{
		if (mp_pParent && mp_ChildLink.f_IsInTree())
			mp_pParent->mp_Children.f_Remove(this);
		mp_Key.f_Copy(_Source.mp_Key);

		if (mp_pParent)
		{
			TCRegistry *pOldChild = mp_pParent->mp_Children.m_Tree.f_FindEqual(mp_Key);
			if (pOldChild)
				f_DeleteChild(pOldChild);
			mp_pParent->mp_Children.f_Insert(this);
		}
		mp_Data = _Source.mp_Data;
		mp_Children.m_Tree.f_DeleteAllDefiniteType();

		auto Iter = _Source.mp_Children.f_GetIterator();
		while (Iter)
		{
			TCRegistry *pChild;
			pChild = fg_ConstructObject<TCRegistry>(NMemory::CDefaultAllocator(), this);
			*pChild = *Iter;
			++Iter;
		}

		return *this;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fp_Copy(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> const &_Source)
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
			for (umint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
			{
				f_SetWhiteSpace((ERegistryWhiteSpaceLocation)i, _Source.f_GetWhiteSpace((ERegistryWhiteSpaceLocation)i));
				mp_Key.f_SetParsed((ERegistryWhiteSpaceLocation)i, _Source.mp_Key.f_GetParsed((ERegistryWhiteSpaceLocation)i));
			}
		}

		for (auto iChild = _Source.f_GetChildIterator(); iChild; ++iChild)
		{
			auto pChild = f_CreateChildNoPath(iChild->f_GetName());
			pChild->fp_Copy(*iChild);
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::operator = (TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> const &_Source) -> TCRegistry &
	{
		f_SetName(_Source.f_GetName());
		fp_Copy(_Source);

		return *this;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetContents(TCRegistry const &_Source) -> TCRegistry &
	{
		mp_Data = _Source.mp_Data;
		mp_Children.m_Tree.f_DeleteAllDefiniteType();

		auto Iter = _Source.mp_Children.f_GetIterator();
		while (Iter)
		{
			TCRegistry *pChild;
			pChild = fg_ConstructObject<TCRegistry>(NMemory::CDefaultAllocator(), this);
			*pChild = *Iter;
			++Iter;
		}

		return *this;
	}
}
