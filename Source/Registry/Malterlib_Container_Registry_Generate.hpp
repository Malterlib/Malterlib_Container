// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	template <typename tf_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fsp_ReplaceWithWhitespace(tf_CStr &o_Str)
	{
		for (auto pParse = o_Str.f_GetStrUniqueWritable(); *pParse; ++pParse)
		{
			if (!NStr::fg_CharIsWhiteSpace(*pParse))
				*pParse = ' ';
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	template <bool tf_bEscapeNewLines, typename tf_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fsp_GetEscapedStrAppend(const t_CStr &_Str, tf_CStr &_Dest, bool _bForceEscape, const t_CStr &_PreData, ch8 const *_pNewLine)
	{
		bool bNeedEscape = _bForceEscape || _Str.f_IsEmpty();

		if (!bNeedEscape)
		{
			mint Len = _Str.f_GetLen();

			typename t_CStr::CMaxChar Current, Prev = 0;
			for (mint i = 0; i < Len; ++i, (Prev = Current))
			{
				Current = _Str.f_GetAt(i);
				if (Current == '\"' || Current == '{' || Current == '}' || Current == '#' || Current == '\\')
				{
					bNeedEscape = true;
					break;
				}
				else if (Prev == '/' && (Current == '*' || Current == '/'))
				{
					// Strings containing comments.
					bNeedEscape = true;
					break;
				}
				else if (Current <= ' ')
				{
					bNeedEscape = true;
					break;
				}
				else if constexpr (tf_bEscapeNewLines)
				{
					if (Current == '\n')
					{
						bNeedEscape = true;
						break;
					}
				}
			}
		}
		if (bNeedEscape)
		{
			if constexpr (tf_bEscapeNewLines)
			{
				mint Len = _Str.f_GetLen();
				mint iStart = 0;
				for (mint i = 0; i < Len; ++i)
				{
					typename t_CStr::CMaxChar Current = _Str.f_GetAt(i);
					if (Current == '\n')
					{
						_Dest += _Str.f_Extract(iStart, (i+1)-iStart).f_EscapeStr("\\\"\r\n\t", "\\\"rnt");
						_Dest += "\\";
						_Dest += _pNewLine;
						_Dest += _PreData;
						iStart = i+1;
					}
				}
				_Dest += _Str.f_Extract(iStart, Len - iStart).f_EscapeStr("\\\"\r\n\t", "\\\"rnt");
			}
			else
				_Dest += _Str.f_EscapeStr();
		}
		else
			_Dest += _Str;
	}
	
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	template <bool tf_bIncludeFileLine, bool tf_bEscapeNewLines, typename tf_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fspr_GenerateStr(tf_CStr &_Stream, mint _Level, const TCRegistry *_pReg, ch8 const *_pNewLine)
	{
		t_CStr PreData;
		if constexpr (tf_bIncludeFileLine && mc_bSupportLocation)
		{
			auto &Location = _pReg->mp_Key.f_GetLocation();
			PreData = typename t_CStr::CFormat("{} ") << Location;
		}

		PreData.f_AddChars('\t', _Level);

		bool bForceEscaped = false;
		if constexpr (mc_bSupportWhiteSpace)
			bForceEscaped = _pReg->f_GetForceEscapedValue();
		bool bHasScope = _pReg->f_HasScope();

		bool bValueIsEmpty;
		if constexpr (TCRegistry_CustomKeyValue<t_CKey, t_CData>::mc_bDefault)
			bValueIsEmpty = _pReg->mp_Data.f_IsEmpty();
		else
			bValueIsEmpty = TCRegistry_CustomKeyValue<t_CKey, t_CData>::fs_ValueIsEmpty(_pReg->mp_Data, bForceEscaped || !bHasScope);

		const ch8 *Space = " ";
		const ch8 *NewLine = _pNewLine;

		t_CStr PreDataValue;
		if constexpr (mc_bSupportWhiteSpace)
		{
			auto WhiteSpace = _pReg->mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey);

			if ((!WhiteSpace.f_IsEmpty() || _pReg->mp_Key.f_GetParsed(ERegistryWhiteSpaceLocation_BeforeKey)) && !fsp_OnlyWhiteSpace(WhiteSpace))
			{
				PreDataValue = WhiteSpace;
				_Stream += WhiteSpace;
			}
			else
			{
				PreDataValue = PreData;
				_Stream += PreData;
			}
		}
		else
		{
			PreDataValue = PreData;
			_Stream += PreData;
		}

		fsp_ReplaceWithWhitespace(PreDataValue);
		bool bForceEscape = false;
		if constexpr (mc_bSupportWhiteSpace)
			bForceEscape = _pReg->f_GetForceEscapedKey();

		auto PreKeyLen = _Stream.f_GetLen();

		if constexpr (TCRegistry_CustomKeyValue<t_CKey, t_CData>::mc_bDefaultKey)
			fsp_GetEscapedStrAppend<tf_bEscapeNewLines>(_pReg->f_GetName(), _Stream, bForceEscape, PreDataValue, _pNewLine);
		else
			TCRegistry_CustomKeyValue<t_CKey, t_CData>::fs_GenerateKey(_Stream, _pReg->f_GetName(), bForceEscape, _Level, PreDataValue);

		PreDataValue.f_AddStr(_Stream.f_GetStr() + PreKeyLen, _Stream.f_GetLen() - PreKeyLen);

		if (!bValueIsEmpty || !bHasScope || bForceEscaped)
		{
			if constexpr (mc_bSupportWhiteSpace)
			{
				auto WhiteSpace = _pReg->mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_Between);

				if (!WhiteSpace.f_IsEmpty() || (_pReg->mp_Key.f_GetParsed(ERegistryWhiteSpaceLocation_Between) && (!_pReg->mp_Children.m_Tree.f_IsEmpty() || !bValueIsEmpty)))
				{
					aint iFind = WhiteSpace.f_FindCharReverse('\n');
					if (iFind >= 0)
						PreDataValue = WhiteSpace.f_Extract(iFind + 1);
					else
						PreDataValue += WhiteSpace;
					_Stream += WhiteSpace;
				}
				else
				{
					PreDataValue += Space;
					_Stream += Space;
				}
			}
			else
			{
				PreDataValue += Space;
				_Stream += Space;
			}

			fsp_ReplaceWithWhitespace(PreDataValue);
			if constexpr (TCRegistry_CustomKeyValue<t_CKey, t_CData>::mc_bDefault)
				fsp_GetEscapedStrAppend<tf_bEscapeNewLines>(_pReg->mp_Data, _Stream, bForceEscaped, PreDataValue, _pNewLine);
			else
				TCRegistry_CustomKeyValue<t_CKey, t_CData>::fs_Generate(_Stream, _pReg->mp_Data, bForceEscaped || !bHasScope, _Level, PreDataValue);
		}

		if constexpr (mc_bSupportWhiteSpace)
		{
			auto WhiteSpace = _pReg->mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_After);

			if (!WhiteSpace.f_IsEmpty() || _pReg->mp_Key.f_GetParsed(ERegistryWhiteSpaceLocation_After))
				_Stream += WhiteSpace;
			else
				_Stream += NewLine;
		}
		else
			_Stream += NewLine;


		if (bHasScope)
		{
			if constexpr (mc_bSupportWhiteSpace)
			{
				auto WhiteSpace = _pReg->mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeStart);

				if ((!WhiteSpace.f_IsEmpty() || _pReg->mp_Key.f_GetParsed(ERegistryWhiteSpaceLocation_BeforeChildScopeStart)) && !fsp_OnlyWhiteSpace(WhiteSpace))
					_Stream += WhiteSpace;
				else
					_Stream += PreData;
			}
			else
				_Stream += PreData;

			_Stream += "{";

			if constexpr (mc_bSupportWhiteSpace)
			{
				auto WhiteSpace = _pReg->mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeStart);

				if (!WhiteSpace.f_IsEmpty() || _pReg->mp_Key.f_GetParsed(ERegistryWhiteSpaceLocation_AfterChildScopeStart))
					_Stream += WhiteSpace;
				else
					_Stream += NewLine;
			}
			else
				_Stream += NewLine;

			auto Iter = _pReg->mp_Children.f_GetIterator();
			while (Iter)
			{
				fspr_GenerateStr<tf_bIncludeFileLine, tf_bEscapeNewLines>(_Stream, _Level + 1, Iter, _pNewLine);
				++Iter;
			}

			if constexpr (mc_bSupportWhiteSpace)
			{
				auto WhiteSpace = _pReg->mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeEnd);

				if ((!WhiteSpace.f_IsEmpty() || _pReg->mp_Key.f_GetParsed(ERegistryWhiteSpaceLocation_BeforeChildScopeEnd)) && !fsp_OnlyWhiteSpace(WhiteSpace))
					_Stream += WhiteSpace;
				else
					_Stream += PreData;
			}
			else
				_Stream += PreData;

			_Stream += "}";

			if constexpr (mc_bSupportWhiteSpace)
			{
				auto WhiteSpace = _pReg->mp_Key.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeEnd);

				if (!WhiteSpace.f_IsEmpty() || _pReg->mp_Key.f_GetParsed(ERegistryWhiteSpaceLocation_AfterChildScopeEnd))
					_Stream += WhiteSpace;
				else
					_Stream += NewLine;
			}
			else
				_Stream += NewLine;
		}
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	template <typename tf_CStr, bool tf_bIncludeFileLine, bool tf_bEscapeNewLines>
	tf_CStr TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GenerateStr(ch8 const *_pNewLine) const
	{
		tf_CStr Temp;
		auto Iter = mp_Children.f_GetIterator();
		while (Iter)
		{
			fspr_GenerateStr<tf_bIncludeFileLine, tf_bEscapeNewLines>(Temp, 0, Iter, _pNewLine);
			++Iter;
		}
		return Temp;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	t_CStr TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GenerateStr(ch8 const *_pNewLine) const
	{
		return f_GenerateStr<t_CStr, 0, 1>(_pNewLine);
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	t_CStr TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GenerateStrLax(ch8 const *_pNewLine) const
	{
		return f_GenerateStr<t_CStr, 0, 0>(_pNewLine);
	}
}
