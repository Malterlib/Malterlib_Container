// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	t_CData const &TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetThisValue() const
	{
		return mp_Data;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	t_CData &TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetThisValue()
	{
		return mp_Data;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetThisValue(const t_CData &_Data)
	{
		mp_Data = _Data;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	t_CData const &TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetValue(t_CStr _Str, const t_CData &_Default) const
	{

		const TCRegistry *pRegistry = fp_GetChildParse(_Str, fg_NullPtr<t_CStr>());
		if (pRegistry)
		{
			return pRegistry->mp_Data;
		}
		else
			return _Default;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	t_CData const &TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetValue(t_CStr const &_Str) const
	{
		t_CStr Str = _Str;
		TCRegistry const *pRegistry = fp_GetChildParse(Str, fg_NullPtr<t_CStr>());
		if (pRegistry)
		{
			return pRegistry->mp_Data;
		}
		else
			DMibErrorRegistry(typename t_CStr::CFormat("No such key '{}'") << _Str);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	t_CData TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetValueMove(t_CStr const &_Str)
	{
		t_CStr Str = _Str;
		TCRegistry const *pRegistry = fp_GetChildParse(Str, fg_NullPtr<t_CStr>());
		if (pRegistry)
		{
			return fg_Move(pRegistry->mp_Data);
		}
		else
			DMibErrorRegistry(typename t_CStr::CFormat("No such key '{}'") << _Str);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	t_CData const &TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetValueNoPath(t_CKey const &_Name) const
	{
		TCRegistry const *pRegistry = CRegistryKey::fs_FindEqual(*this, _Name);
		if (pRegistry)
		{
			return pRegistry->mp_Data;
		}
		else
			DMibErrorRegistry(typename t_CStr::CFormat("No such key '{}'") << _Name);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	t_CData TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetValueNoPathMove(t_CKey const &_Name)
	{
		TCRegistry *pRegistry = CRegistryKey::fs_FindEqual(*this, _Name);
		if (pRegistry)
		{
			return fg_Move(pRegistry->mp_Data);
		}
		else
			DMibErrorRegistry(typename t_CStr::CFormat("No such key '{}'") << _Name);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	t_CData const &TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetValueNoPath(t_CKey const &_Name, const t_CData &_Default) const
	{
		TCRegistry const *pRegistry = CRegistryKey::fs_FindEqual(*this, _Name);
		if (pRegistry)
		{
			return pRegistry->mp_Data;
		}
		else
			return _Default;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetValueIfExists(t_CStr _Str, t_CData & _OutValue) const
	{
		TCRegistry const *pRegistry = fp_GetChildParse(_Str, fg_NullPtr<t_CStr>());
		if (pRegistry)
		{
			_OutValue = pRegistry->mp_Data;
			return true;
		}
		else
			return false;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetValue(t_CStr _Name, const t_CData &_Data) -> TCRegistry *
	{
		t_CStr NotFound;
		TCRegistry *pPrev = this;
		TCRegistry *pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);

		while (!pChild)
		{
			pChild = fg_ConstructObject<TCRegistry>(NMemory::CDefaultAllocator(), pPrev);
			pChild->mp_Key.f_Set(NotFound);
			pPrev->mp_Children.f_Insert(pChild);
			pPrev = pChild;
			if (_Name.f_IsEmpty())
				break;
			pChild = pPrev->fp_GetChildParse(_Name, &NotFound, &pPrev);
		}
		pChild->mp_Data = _Data;

		return pChild;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetValueNoPath(t_CKey const &_Name, const t_CData &_Data) -> TCRegistry *
	{
		TCRegistry *pChild = CRegistryKey::fs_FindEqual(*this, _Name);

		if (!pChild)
		{
			pChild = fg_ConstructObject<TCRegistry>(NMemory::CDefaultAllocator(), this);
			pChild->mp_Key.f_Set(_Name);
			mp_Children.f_Insert(pChild);
		}
		pChild->mp_Data = _Data;

		return pChild;
	}
}
