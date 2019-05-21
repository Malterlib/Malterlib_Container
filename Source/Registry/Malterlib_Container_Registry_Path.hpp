// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	t_CStr TCRegistry<t_CStr, t_CData, t_Flags>::fsp_AppendPath(const t_CStr &_Path, const t_CStr &_Append)
	{
		if (_Path.f_IsEmpty())
			return _Append;
		else
			return _Path + t_CStr("/") + _Append;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::fp_GetChildParse
		(
		 	t_CStr &_Str
		 	, t_CStr *_pNotFound
		 	, TCRegistry const **_pPrev
		)
		const -> TCRegistry const *
	{
		TCRegistry const *pCurrent = this;
		while (1)
		{
			aint iPos = _Str.f_FindChar('/');

			t_CStr Temp;
			if (iPos >= 0)
			{
				Temp = _Str.f_Left(iPos);
				_Str = _Str.f_Extract(iPos + 1);
			}
			else
			{
				Temp = _Str;
				_Str.f_Clear();
			}

			TCRegistry const *pNext = CRegistryKey::fs_FindEqual(*pCurrent, Temp);
			if (!pNext)
			{
				if (_pNotFound)
					*_pNotFound = Temp;
				if (_pPrev)
					*_pPrev = pCurrent;
				return nullptr;
			}
			pCurrent = pNext;
			if (_Str.f_IsEmpty())
				break;
		}

		if (pCurrent != this)
			return pCurrent;
		return nullptr;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::fp_GetChildParse(t_CStr &_Str, t_CStr *_pNotFound, TCRegistry **_pPrev) -> TCRegistry *
	{
		TCRegistry *pCurrent = this;
		while (1)
		{
			aint iPos = _Str.f_FindChar('/');

			t_CStr Temp;
			if (iPos >= 0)
			{
				Temp = _Str.f_Left(iPos);
				_Str = _Str.f_Extract(iPos + 1);
			}
			else
			{
				Temp = _Str;
				_Str.f_Clear();
			}

			TCRegistry *pNext = CRegistryKey::fs_FindEqual(*pCurrent, Temp);
			if (!pNext)
			{
				if (_pNotFound)
					*_pNotFound = Temp;
				if (_pPrev)
					*_pPrev = pCurrent;
				return nullptr;
			}
			pCurrent = pNext;
			if (_Str.f_IsEmpty())
				break;
		}

		if (pCurrent != this)
			return pCurrent;
		return nullptr;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::f_GetParent() -> TCRegistry *
	{
		return mp_pParent;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::f_GetParent() const -> TCRegistry const *
	{
		return mp_pParent;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	t_CStr TCRegistry<t_CStr, t_CData, t_Flags>::f_GetPath() const
	{
		t_CStr Ret = f_GetName();

		TCRegistry *pParent = mp_pParent;
		while (pParent && pParent->mp_pParent)
		{
			Ret = pParent->f_GetName() + "/" + Ret;
			pParent = pParent->mp_pParent;
		}

		return Ret;
	}
}
