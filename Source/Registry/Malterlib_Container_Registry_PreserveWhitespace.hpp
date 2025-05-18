// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CStr>
	bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fsp_OnlyWhiteSpace(tf_CStr const &_Str)
	{
		for (auto const *pParse = _Str.f_GetStr(); *pParse; ++pParse)
		{
			if (!NStr::fg_CharIsWhiteSpace(*pParse))
				return false;
		}
		return true;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str)
	{
		if constexpr (mc_bSupportWhiteSpace)
		{
			DMibFastCheck(f_IsValidWhiteSpace(_Location, _Str));
			mp_Key.f_SetWhiteSpace(_Location, _Str);
			mp_Key.f_SetParsed(_Location, true);
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation _Location)
	{
		if constexpr (mc_bSupportWhiteSpace)
		{
			mp_Key.f_SetParsed(_Location, true);
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <bool tf_bSupportLocation>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetLocation(CLocation const &_Location) -> TCEnableIf<tf_bSupportLocation>
	{
		return mp_Key.f_SetLocation(_Location);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <bool tf_bSupportLocation>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetValueLocation(NStr::TCParseLocation<t_CStr, true> const &_Location) -> TCEnableIf<tf_bSupportLocation>
	{
		return mp_Key.f_SetValueLocation(_Location);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <bool tf_bSupportLocation>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetLocation() const -> TCEnableIf<tf_bSupportLocation, CLocation> const &
	{
		return mp_Key.f_GetLocation();
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <bool tf_bSupportLocation>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetValueLocation() const -> TCEnableIf<tf_bSupportLocation, NStr::TCParseLocation<t_CStr, true>> const &
	{
		return mp_Key.f_GetValueLocation();
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <bool tf_bSupportWhiteSpace>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetForceEscapedKey() const -> TCEnableIf<tf_bSupportWhiteSpace, bool>
	{
		return mp_Key.f_GetForceEscapedKey();
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <bool tf_bSupportWhiteSpace>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetForceEscapedValue() const -> TCEnableIf<tf_bSupportWhiteSpace, bool>
	{
		return mp_Key.f_GetForceEscapedValue();
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <bool tf_bSupportWhiteSpace>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetForceEscapedKey(bool _bForced) -> TCEnableIf<tf_bSupportWhiteSpace>
	{
		return mp_Key.f_SetForceEscapedKey(_bForced);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <bool tf_bSupportWhiteSpace>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetForceEscapedValue(bool _bForced) -> TCEnableIf<tf_bSupportWhiteSpace>
	{
		return mp_Key.f_SetForceEscapedValue(_bForced);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <typename tf_CStr, bool tf_bSupportWhiteSpace>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_IsValidWhiteSpace(ERegistryWhiteSpaceLocation _Location, tf_CStr const &_Str) -> TCEnableIf<tf_bSupportWhiteSpace, bool>
	{
		bool bValidWhiteSpace = true;
		bool bLineCommentValid = false;

		switch (_Location)
		{
		case ERegistryWhiteSpaceLocation_BeforeKey:
		case ERegistryWhiteSpaceLocation_After:
		case ERegistryWhiteSpaceLocation_AfterChildScopeStart:
		case ERegistryWhiteSpaceLocation_AfterChildScopeEnd:
		case ERegistryWhiteSpaceLocation_Between:
			bLineCommentValid = true;
			break;

		case ERegistryWhiteSpaceLocation_BeforeChildScopeStart:
		case ERegistryWhiteSpaceLocation_BeforeChildScopeEnd:
			bLineCommentValid = false;
			break;
		case ERegistryWhiteSpaceLocation_Max:
			DMibNeverGetHere;
			break;
		}

		{
			ch8 const *pParse = _Str.f_GetStr();
			auto Current = *pParse;
			CEmptyParseContext ParseContext;

			while (Current)
			{
				// Parse away white space
				while (NStr::fg_CharIsWhiteSpace(Current))
					Current = *(++pParse);

				if (Current == '/' && pParse[1] == '/') // Comment
				{
					if (bLineCommentValid)
					{
						if (!fsp_ParseToEndOfLine(pParse, ParseContext))
						{
							if (!(_Location == ERegistryWhiteSpaceLocation_AfterChildScopeEnd && mp_pParent && !mp_pParent->mp_pParent))
								bValidWhiteSpace = false;
							break;
						}
					}
					else
					{
						bValidWhiteSpace = false;
						break;
					}
				}
				else if (Current == '/' && pParse[1] == '*') // Comment
				{
					if (!fsp_ParseToEndOfComment(pParse, ParseContext))
					{
						bValidWhiteSpace = false;
						break;
					}
				}
				else if (Current)
				{
					bValidWhiteSpace = false;
					break;
				}
				Current = *pParse;
			}
		}
		return bValidWhiteSpace;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <bool tf_bSupportWhiteSpace>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str) -> TCEnableIf<tf_bSupportWhiteSpace>
	{
		DMibFastCheck(f_IsValidWhiteSpace(_Location, _Str));
		mp_Key.f_SetWhiteSpace(_Location, _Str);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_HasScope() const
	{
		if (!mp_Children.m_Tree.f_IsEmpty())
			return true;

		if constexpr (mc_bSupportWhiteSpace)
		{
			return
				!fsp_OnlyWhiteSpace(mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeStart))
				|| !fsp_OnlyWhiteSpace(mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeStart))
				|| !fsp_OnlyWhiteSpace(mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeEnd))
				|| !fsp_OnlyWhiteSpace(mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeEnd))
			;
		}

		return false;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
		requires cCompatibleRegistryFlags<t_CStr, t_Flags>
	template <bool tf_bSupportWhiteSpace>
	auto TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetWhiteSpace(ERegistryWhiteSpaceLocation _Location) const -> TCEnableIf<tf_bSupportWhiteSpace, NStr::CStr>
	{
		return mp_Key.f_GetWhiteSpace(_Location);
	}
}
