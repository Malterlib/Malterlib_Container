// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStr>
	bool TCRegistry<t_CStr, t_CData, t_Flags>::fsp_OnlyWhiteSpace(tf_CStr const &_Str)
	{
		for (auto const *pParse = _Str.f_GetStr(); *pParse; ++pParse)
		{
			if (!NStr::fg_CharIsWhiteSpace(*pParse))
				return false;
		}
		return true;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str)
	{
		DMibFastCheck(f_IsValidWhiteSpace(_Location, _Str));
		mp_Key.f_SetWhiteSpace(_Location, _Str);
		mp_Key.f_SetParsed(_Location, true);
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation _Location)
	{
		mp_Key.f_SetParsed(_Location, true);
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::f_GetFile() const -> decltype(fg_GetType<CRegistryKey &>().f_GetFile())
	{
		return mp_Key.f_GetFile();
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_SetFile(t_CStr const &_File)
	{
		return mp_Key.f_SetFileLine(_File, mp_Key.f_GetLine());
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_SetLine(t_CStr const &_Line)
	{
		return mp_Key.f_SetFileLine(mp_Key.f_GetFile(), _Line);
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::f_GetLine() const -> decltype(fg_GetType<CRegistryKey &>().f_GetLine())
	{
		return mp_Key.f_GetLine();
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::f_GetForceEscapedKey() const -> decltype(fg_GetType<CRegistryKey &>().f_GetForceEscapedKey())
	{
		return mp_Key.f_GetForceEscapedKey();
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::f_GetForceEscapedValue() const -> decltype(fg_GetType<CRegistryKey &>().f_GetForceEscapedValue())
	{
		return mp_Key.f_GetForceEscapedValue();
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_SetForceEscapedKey(bool _bForced)
	{
		return mp_Key.f_SetForceEscapedKey(_bForced);
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_SetForceEscapedValue(bool _bForced)
	{
		return mp_Key.f_SetForceEscapedValue(_bForced);
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStr>
	bool TCRegistry<t_CStr, t_CData, t_Flags>::f_IsValidWhiteSpace(ERegistryWhiteSpaceLocation _Location, tf_CStr const &_Str)
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

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_SetWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str)
	{
		DMibFastCheck(f_IsValidWhiteSpace(_Location, _Str));
		mp_Key.f_SetWhiteSpace(_Location, _Str);
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	bool TCRegistry<t_CStr, t_CData, t_Flags>::f_HasScope() const
	{
		return !mp_Children.m_Tree.f_IsEmpty() ||
			(
				mc_bSupportWhiteSpace
				&&
				(
					!fsp_OnlyWhiteSpace(mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeStart))
					|| !fsp_OnlyWhiteSpace(mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeStart))
					|| !fsp_OnlyWhiteSpace(mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeEnd))
					|| !fsp_OnlyWhiteSpace(mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeEnd))
				)
			)
		;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::f_GetWhiteSpace(ERegistryWhiteSpaceLocation _Location) const
		-> decltype(fg_GetType<CRegistryKey &>().f_GetWhiteSpace(_Location))
	{
		return mp_Key.f_GetWhiteSpace(_Location);
	}
}
