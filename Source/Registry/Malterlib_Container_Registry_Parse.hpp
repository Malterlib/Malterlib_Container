// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <bool tf_bAllowLineBreakInEscapedString>
	t_CStr TCRegistry<t_CStr, t_CData, t_Flags>::fsp_ParseIdentifierStr
		(
		 	ch8 const * &o_pParse
		 	, CParseContext &_ParseContext
		 	, bool &_bWasEscaped
		)
	{
		t_CStr Ret;
		_bWasEscaped = false;
		auto pParse = o_pParse;
		aint Mode = 0;
		auto pStart = pParse;
		aint iStartEscapeSequenceLine = 0;
		auto Current = *pParse;
		while (Current)
		{
			if (Mode == 0)
			{
				if (Current == '\"')
				{
					Ret.f_AddStr(pStart, pParse - pStart);
					iStartEscapeSequenceLine = _ParseContext.f_GetLine();
					pStart = pParse + 1;
					Mode = 1;
					_bWasEscaped = true;
				}
				else if (Current == '\\')
				{
					Ret.f_AddStr(pStart, pParse - pStart);
					// Parse away any whitespace
					Current = *(++pParse);
					while (Current && NStr::fg_CharIsWhiteSpace(Current))
					{
						if constexpr (mc_bSupportFileLine)
						{
							if (Current == '\n')
							{
								_ParseContext.f_AddLine();
							}
						}
						Current = *(++pParse);
					}

					if
					(
						!Current || Current == '{' || Current == '}'
						|| (Current == '/' && pParse[1] == '/')
						|| (Current == '/' && pParse[1] == '*')
					)
					{
						DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " \\ needs to be followed with a continuation of the text") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
					}

					pStart = pParse;
					continue;
				}
				else if
				(
					NStr::fg_CharIsWhiteSpace(Current) || Current == '{' || Current == '}'
					|| (Current == '/' && pParse[1] == '/')
					|| (Current == '/' && pParse[1] == '*')
				)
				{
					break;
				}
			}
			else
			{
				if constexpr (mc_bSupportFileLine)
				{
					if (Current == '\n')
					{
						_ParseContext.f_AddLine();
					}
				}

				if (Current == '\\')
				{
					auto Next = pParse[1];
					Ret.f_AddStr(pStart, pParse - pStart);
					switch (Next)
					{
					case 'r':
						Ret.f_AddChar('\r');
						break;
					case 'n':
						Ret.f_AddChar('\n');
						break;
					case 't':
						Ret.f_AddChar('\t');
						break;
					case '\\':
						Ret.f_AddChar('\\');
						break;
					case '\"':
						Ret.f_AddChar('\"');
						break;
					default:
						DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Invalid escape character") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
						break;
					}
					++pParse;
					pStart = pParse + 1;
				}
				else if (Current == '\r' || Current == '\n')
				{
					if (!tf_bAllowLineBreakInEscapedString)
						DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching end for escaped text (\") before end of line") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
				}
				else if (Current == '\"')
				{
					Ret.f_AddStr(pStart, pParse - pStart);
					Mode = 0;
					pStart = pParse + 1;
				}
			}

			Current = *(++pParse);
		}
		if (Mode == 1)
			DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching end for escaped text (\")") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
		Ret.f_AddStr(pStart, pParse - pStart);
		o_pParse = pParse;
		return Ret;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CParseContext>
	bool TCRegistry<t_CStr, t_CData, t_Flags>::fsp_ParseToEndOfLine(ch8 const * &o_pParse, tf_CParseContext &_ParseContext)
	{
		auto pParse = o_pParse + 2;
		bool bRet = false;

		auto Current = *pParse;
		while (Current)
		{
			if (Current == '\n')
			{
				if constexpr (mc_bSupportFileLine)
					_ParseContext.f_AddLine();
				++pParse;
				bRet = true;
				break;
			}
			++pParse;
			Current = *pParse;
		}

		o_pParse = pParse;
		return bRet;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CParseContext>
	bool TCRegistry<t_CStr, t_CData, t_Flags>::fsp_ParseToEndOfComment(ch8 const * &o_pParse, tf_CParseContext &_ParseContext)
	{
		auto pParse = o_pParse + 2;
		bool bRet = false;

		auto Current = *pParse;
		while (Current)
		{
			if constexpr (mc_bSupportFileLine)
			{
				if (Current == '\n')
					_ParseContext.f_AddLine();
			}

			if (Current == '*' && pParse[1] == '/')
			{
				pParse += 2;
				bRet = true;
				break;
			}
			++pParse;
			Current = *pParse;
		}

		o_pParse = pParse;
		return bRet;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <bool tf_bAllowLineBreakInEscapedString>
	ch8 const *TCRegistry<t_CStr, t_CData, t_Flags>::fpr_Parse(ch8 const *_pParse, CParseContext &_ParseContext)
	{
		ch8 const *pParse = _pParse;
		auto Current = *pParse;

		aint ParseMode = 0;

		t_CStr KeyName;
		bool bKeyNameWasEscaped = false;

		TCRegistry *pReg = nullptr;
		TCRegistry *pLastChildReg = nullptr;
		bool bHadChildren = false;
		bool bKeyWithoutLine = false;

		auto fl_FixupWhitespaceBeforeKey = [&] (ch8 const *_pBeforeParse)
		{
			if (pReg || this->f_GetParent())
			{
				t_CStr Temp = _ParseContext.f_GetNextWhiteSpace(_pBeforeParse);
				aint iFind = Temp.f_FindCharReverse('\n');
				if (iFind >= 0)
				{
					t_CStr Left = Temp.f_Left(iFind+1);
					t_CStr Right = Temp.f_Extract(iFind+1);
					if (this->f_IsValidWhiteSpace(ERegistryWhiteSpaceLocation_After, Left) && this->f_IsValidWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey, Right))
					{
						if (pReg)
						{
							if (bHadChildren)
								pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeEnd, Left);
							else
								pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After, Left);
						}
						else
							this->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeStart, Left);
						_ParseContext.f_SetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey, Right);
					}
					else
					{
						if (pReg)
							pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After);
						_ParseContext.f_SetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey, Temp);
					}
				}
				else
				{
					if (pReg)
						pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After);
					_ParseContext.f_SetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey, Temp);
				}
			}
			else
				_ParseContext.f_SetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey, _ParseContext.f_GetNextWhiteSpace(_pBeforeParse));
			_ParseContext.f_SetStartWhiteSpace(pParse);
		};

		auto fl_FixupWhitespaceBeforeValue = [&] (ch8 const *_pBeforeParse)
		{
			pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey, _ParseContext.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey));
			pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_Between, _ParseContext.f_GetNextWhiteSpace(_pBeforeParse));
			_ParseContext.f_SetStartWhiteSpace(pParse);
		};

		while (Current)
		{
			// Parse away white space
			while (NStr::fg_CharIsWhiteSpace(Current))
			{
				if constexpr (mc_bSupportFileLine)
				{
					if (Current == '\n')
					{
						bKeyWithoutLine = false;
						_ParseContext.f_AddLine();
					}
				}

				Current = *(++pParse);
			}

			if (Current == '/' && pParse[1] == '/') // Comment
			{
				fsp_ParseToEndOfLine(pParse, _ParseContext);
				bKeyWithoutLine = false;
			}
			else if (Current == '/' && pParse[1] == '*') // Comment
			{
				aint StartParseLine = _ParseContext.f_GetLine();
				if (!fsp_ParseToEndOfComment(pParse, _ParseContext))
				{
					DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No end found for block comment") << _ParseContext.f_GetFile() << StartParseLine).f_GetStr());
				}
			}
			else if (Current == '{' && (!TCRegistry_CustomValue<t_CData>::mc_bRequireStartScopeOnSeparateLine || !bKeyWithoutLine))
			{
				aint LineBeforeStart = _ParseContext.f_GetLine();
				auto pBeforeParse = pParse;
				++pParse;
				if (ParseMode == 1)
				{
					pReg = f_CreateChildNoPath(KeyName, mc_bSupportForceCreate);
					pReg->f_SetThisValue({});
					if constexpr (mc_bSupportFileLine)
						pReg->mp_Key.f_SetFileLine(_ParseContext.f_GetFile(), _ParseContext.f_GetLine());

					if constexpr (mc_bSupportWhiteSpace)
					{
						if (bKeyNameWasEscaped)
							pReg->f_SetForceEscapedKey(true);

						pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey, _ParseContext.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey));
					}

					ParseMode = 0;
				}
				else if (!pReg)
				{
					DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Children without key not supported") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
				}
				else if (pReg == pLastChildReg)
				{
					DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " You cannot specify two child sections for one key") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
				}

				if (pReg)
				{
					pLastChildReg = pReg;
					if constexpr (mc_bSupportWhiteSpace)
					{
						t_CStr Temp = _ParseContext.f_GetNextWhiteSpace(pBeforeParse);
						aint iFind = Temp.f_FindCharReverse('\n');
						if (iFind >= 0)
						{
							t_CStr Left = Temp.f_Left(iFind+1);
							t_CStr Right = Temp.f_Extract(iFind+1);
							if (f_IsValidWhiteSpace(ERegistryWhiteSpaceLocation_After, Left) && f_IsValidWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeStart, Right))
							{
								pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After, Left);
								pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeStart, Right);
								pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeStart);
							}
							else
							{
								pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After);
								pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeStart, Temp);
								pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeStart);
							}
						}
						else
						{
							pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After);
							pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeStart, Temp);
							pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeStart);
						}
						_ParseContext.f_SetStartWhiteSpace(pParse);
					}

					pParse = pReg->fpr_Parse<tf_bAllowLineBreakInEscapedString>(pParse, _ParseContext);
					Current = *pParse;
					if (Current != '}')
					{
						DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching '}' found") << _ParseContext.f_GetFile() << LineBeforeStart).f_GetStr());
					}
					++pParse;
				}

				bHadChildren = true;
				Current = *pParse;
			}
			else if (Current == '}')
			{
				if (ParseMode == 1)
					DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Mismatching key/value pairs within scope") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
				if constexpr (mc_bSupportWhiteSpace)
				{
					t_CStr Temp = _ParseContext.f_GetNextWhiteSpace(pParse);
					aint iFind = Temp.f_FindCharReverse('\n');
					if (iFind >= 0)
					{
						t_CStr Left = Temp.f_Left(iFind+1);
						t_CStr Right = Temp.f_Extract(iFind+1);
						if (f_IsValidWhiteSpace(ERegistryWhiteSpaceLocation_After, Left) && f_IsValidWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeEnd, Right))
						{
							if (pReg)
							{
								if (bHadChildren)
									pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeEnd, Left);
								else
									pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After, Left);
							}
							else
								fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeStart, Left);

							fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeEnd, Right);
						}
						else
						{
							if (pReg)
								pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After);
							fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeEnd, Temp);
						}
					}
					else
					{
						if (pReg)
							pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After);
						fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_BeforeChildScopeEnd, Temp);
					}
					_ParseContext.f_SetStartWhiteSpace(pParse+1);
				}
				return pParse;
			}
			else if (Current)
			{
				if (ParseMode == 0)
				{
					auto pBeforeParse = pParse;
					bool bWasEscaped = false;
					t_CStr Temp = fsp_ParseIdentifierStr<tf_bAllowLineBreakInEscapedString>(pParse, _ParseContext, bWasEscaped);
					if constexpr (mc_bSupportWhiteSpace)
						fl_FixupWhitespaceBeforeKey(pBeforeParse);
					bHadChildren = false;
					KeyName = Temp;
					bKeyNameWasEscaped = bWasEscaped;
					bKeyWithoutLine = true;
					ParseMode = 1;
				}
				else
				{
					if constexpr (TCRegistry_CustomValue<t_CData>::mc_bDefault)
					{
						auto pBeforeParse = pParse;
						bool bWasEscaped = false;
						t_CStr Temp = fsp_ParseIdentifierStr<tf_bAllowLineBreakInEscapedString>(pParse, _ParseContext, bWasEscaped);
						bHadChildren = false;
						pReg = f_CreateChildNoPath(KeyName, mc_bSupportForceCreate);
						if (bKeyNameWasEscaped)
							pReg->f_SetForceEscapedKey(true);
						if (bWasEscaped)
							pReg->f_SetForceEscapedValue(true);
						pReg->f_SetThisValue(Temp);
						if constexpr (mc_bSupportFileLine)
							pReg->mp_Key.f_SetFileLine(_ParseContext.f_GetFile(), _ParseContext.f_GetLine());
						if constexpr (mc_bSupportWhiteSpace)
							fl_FixupWhitespaceBeforeValue(pBeforeParse);
						ParseMode = 0;
					}
					else
					{
						try
						{
							auto pBeforeParse = pParse;
							bool bWasEscaped = false;
							auto TempValue = TCRegistry_CustomValue<t_CData>::fs_Parse(pParse, _ParseContext, bWasEscaped);

							bHadChildren = false;
							bKeyWithoutLine = false;
							pReg = f_CreateChildNoPath(KeyName, mc_bSupportForceCreate);
							if (bKeyNameWasEscaped)
								pReg->f_SetForceEscapedKey(true);
							if (bWasEscaped)
								pReg->f_SetForceEscapedValue(true);
							pReg->f_SetThisValue(fg_Move(TempValue));
							if constexpr (mc_bSupportFileLine)
								pReg->mp_Key.f_SetFileLine(_ParseContext.f_GetFile(), _ParseContext.f_GetLine());
							if constexpr (mc_bSupportWhiteSpace)
								fl_FixupWhitespaceBeforeValue(pBeforeParse);
							ParseMode = 0;
						}
						catch (NException::CException const &_Exception)
						{
							DMibError
								(
								 	NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Failed to parse value: {}")
									<< _ParseContext.f_GetFile()
									<< _ParseContext.f_GetLine()
								 	<< _Exception
								)
							;
						}
					}
				}
			}

			_ParseContext.f_SetLastAdded(pReg, bHadChildren);
			Current = *pParse;
		}

		if (ParseMode == 1)
			DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Mismatching key/value pairs within scope") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());

		return pParse;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <bool tf_bAllowLineBreakInEscapedString>
	void TCRegistry<t_CStr, t_CData, t_Flags>::fp_Parse(ch8 const *_pParse, CParseContext &_ParseContext)
	{
		ch8 const *pParse = fpr_Parse<tf_bAllowLineBreakInEscapedString>(_pParse, _ParseContext);
		if (*pParse == '}')
			DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching '{{' found") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());

		if constexpr (mc_bSupportWhiteSpace)
		{
			auto pStartWhiteSpace = _ParseContext.f_GetStartWhiteSpace();
			{
				bool bLastHadChildren = false;
				TCRegistry *pReg = _ParseContext.f_GetLastAdded(bLastHadChildren);
				if (pReg)
				{
					if (pStartWhiteSpace != pParse)
					{
						t_CStr Temp = t_CStr(pStartWhiteSpace, pParse - pStartWhiteSpace);

						if (bLastHadChildren)
							pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeEnd, Temp);
						else
							pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After, Temp);
					}
					else
					{
						if (bLastHadChildren)
							pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_AfterChildScopeEnd);
						else
							pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_After);
					}
				}

			}
		}
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_ParseStr(t_CStr const &_Text, t_CStr const &_File)
	{
		// Remove anything existing
		mp_Children.m_Tree.f_DeleteAllDefiniteType();

		CParseContext Context;

		if constexpr (mc_bSupportFileLine || mc_bSupportWhiteSpace)
		{
			Context.f_SetFile(_File);
			Context.f_SetStartParse(_Text.f_GetStr());
			mp_Key.f_SetFileLine(_File, 0);
		}

		fp_Parse<false>(_Text.f_GetStr(), Context);
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_Parse(NStream::CBinaryStream &_Stream, t_CStr const &_File)
	{
		f_ParseStr(t_CStr::fs_ReadTextStream(_Stream), _File);
	}


	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_ParseLaxStr(t_CStr const &_Text, t_CStr const &_File)
	{
		// Remove anything existing
		mp_Children.m_Tree.f_DeleteAllDefiniteType();

		CParseContext Context;

		if constexpr (mc_bSupportFileLine || mc_bSupportWhiteSpace)
		{
			mp_Key.f_SetFileLine(_File, 0);
			Context.f_SetFile(_File);
			Context.f_SetStartParse(_Text.f_GetStr());
		}

		fp_Parse<true>(_Text.f_GetStr(), Context);
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_ParseLax(NStream::CBinaryStream &_Stream, t_CStr const &_File)
	{
		f_ParseLaxStr(t_CStr::fs_ReadTextStream(_Stream), _File);
	}
}
