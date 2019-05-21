// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <bool tf_bAllowLineBreakInEscapedString, typename tf_CStr>
	tf_CStr TCRegistry<t_CStr, t_CData, t_Flags>::fsp_ParseIdentifierStr
		(
		 	tf_CStr const &_ToParse
		 	, aint &_Pos
		 	, CParseContext &_ParseContext
		 	, bool &_bWasEscaped
		)
	{
/*
		aint iBeforeParse = iParse;
		tf_CStr Temp;
		while (Current)
		{
			if (Current == '\\')
			{
				Temp += _ToParse.f_Extract(iStart, iParse - iStart);
				Current = _ToParse.f_GetAt(++iParse)
				while (Current && NStr::fg_CharIsWhiteSpace(Current))
					Current = _ToParse.f_GetAt(++iParse)
			}
			if (NStr::fg_CharIsWhiteSpace(Current))
				break;
			Current = _ToParse.f_GetAt(++iParse);
		}

		_ToParse.f_Extract(iStart, iParse - iStart);
		*/

		tf_CStr Ret;
		_bWasEscaped = false;
		aint iPos = _Pos;
		aint Mode = 0;
		aint iStart = iPos;
		aint iStartEscapeSequenceLine = 0;
		typename tf_CStr::CMaxChar Current = _ToParse.f_GetAt(iPos);
		while (Current)
		{
			if (Mode == 0)
			{
				if (Current == '\"')
				{
					Ret += _ToParse.f_Extract(iStart, iPos - iStart);
					iStartEscapeSequenceLine = _ParseContext.f_GetLine();
					iStart = iPos + 1;
					Mode = 1;
					_bWasEscaped = true;
				}
				else if (Current == '\\')
				{
					Ret += _ToParse.f_Extract(iStart, iPos - iStart);
					// Parse away any whitespace
					Current = _ToParse.f_GetAt(++iPos);
					while (Current && NStr::fg_CharIsWhiteSpace(Current))
					{
						if constexpr (mc_bSupportFileLine)
						{
							if (Current == '\n')
							{
								_ParseContext.f_AddLine();
							}
						}
						Current = _ToParse.f_GetAt(++iPos);
					}

					if
					(
						!Current || Current == '{' || Current == '}'
						|| (Current == '/' && _ToParse.f_GetAt(iPos+1) == '/')
						|| (Current == '/' && _ToParse.f_GetAt(iPos+1) == '*')
					)
					{
						DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " \\ needs to be followed with a continuation of the text") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
					}

					iStart = iPos;
					continue;
				}
				else if
				(
					NStr::fg_CharIsWhiteSpace(Current) || Current == '{' || Current == '}'
					|| (Current == '/' && _ToParse.f_GetAt(iPos+1) == '/')
					|| (Current == '/' && _ToParse.f_GetAt(iPos+1) == '*')
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
					typename tf_CStr::CMaxChar Next = _ToParse.f_GetAt(iPos+1);
					Ret += _ToParse.f_Extract(iStart, iPos - iStart);
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
					++iPos;
					iStart = iPos + 1;
				}
				else if (Current == '\r' || Current == '\n')
				{
					if (!tf_bAllowLineBreakInEscapedString)
						DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching end for escaped text (\") before end of line") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
				}
				else if (Current == '\"')
				{
					Ret += _ToParse.f_Extract(iStart, iPos - iStart);
					Mode = 0;
					iStart = iPos + 1;
				}
			}

			Current = _ToParse.f_GetAt(++iPos);
		}
		if (Mode == 1)
		{
			DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching end for escaped text (\")") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
		}
		Ret += _ToParse.f_Extract(iStart, iPos - iStart);
		_Pos = iPos;
		return Ret;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStr, typename tf_CParseContext>
	bool TCRegistry<t_CStr, t_CData, t_Flags>::fsp_ParseToEndOfLine(const tf_CStr &_ToParse, aint &_Pos, tf_CParseContext &_ParseContext)
	{
		aint iPos = _Pos + 2;
		bool bRet = false;

		typename tf_CStr::CMaxChar Current = _ToParse.f_GetAt(iPos);
		while (Current)
		{
			if (Current == '\n')
			{
				if constexpr (mc_bSupportFileLine)
					_ParseContext.f_AddLine();
				++iPos;
				bRet = true;
				break;
			}
			++iPos;
			Current = _ToParse.f_GetAt(iPos);
		}

		_Pos = iPos;
		return bRet;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStr, typename tf_CParseContext>
	bool TCRegistry<t_CStr, t_CData, t_Flags>::fsp_ParseToEndOfComment(const tf_CStr &_ToParse, aint &_Pos, tf_CParseContext &_ParseContext)
	{
		aint iPos = _Pos + 2;
		bool bRet = false;

		typename tf_CStr::CMaxChar Current = _ToParse.f_GetAt(iPos);
		while (Current)
		{
			if constexpr (mc_bSupportFileLine)
			{
				if (Current == '\n')
					_ParseContext.f_AddLine();
			}

			if (Current == '*' && _ToParse.f_GetAt(iPos+1) == '/')
			{
				iPos += 2;;
				bRet = true;
				break;
			}
			++iPos;
			Current = _ToParse.f_GetAt(iPos);
		}

		_Pos = iPos;
		return bRet;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <bool tf_bAllowLineBreakInEscapedString, typename tf_CStr>
	aint TCRegistry<t_CStr, t_CData, t_Flags>::fpr_Parse(const tf_CStr &_ToParse, aint _Pos, CParseContext &_ParseContext)
	{
		aint iParse = _Pos;
		typename tf_CStr::CMaxChar Current = _ToParse.f_GetAt(iParse);

		aint ParseMode = 0;

		tf_CStr KeyName;
		bool bKeyNameWasEscaped = false;

		TCRegistry *pReg = nullptr;
		TCRegistry *pLastChildReg = nullptr;
		bool bHadChildren = false;


		auto fl_FixupWhitespaceBeforeKey = [&] (aint _iBeforeParse)
		{
			if (pReg || this->f_GetParent())
			{
				tf_CStr Temp = _ParseContext.f_GetNextWhiteSpace(_ToParse, _iBeforeParse);
				aint iFind = Temp.f_FindCharReverse('\n');
				if (iFind >= 0)
				{
					tf_CStr Left = Temp.f_Left(iFind+1);
					tf_CStr Right = Temp.f_Extract(iFind+1);
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
				_ParseContext.f_SetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey, _ParseContext.f_GetNextWhiteSpace(_ToParse, _iBeforeParse));
			_ParseContext.f_SetStartWhiteSpace(iParse);
		};

		auto fl_FixupWhitespaceBeforeValue = [&] (aint _iBeforeParse)
		{
			pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey, _ParseContext.f_GetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey));
			pReg->fp_SetParsedWhiteSpace(ERegistryWhiteSpaceLocation_Between, _ParseContext.f_GetNextWhiteSpace(_ToParse, _iBeforeParse));
			_ParseContext.f_SetStartWhiteSpace(iParse);
		};

		while (Current)
		{
			// Parse away white space
			while (NStr::fg_CharIsWhiteSpace(Current))
			{
				if constexpr (mc_bSupportFileLine)
				{
					if (Current == '\n')
						_ParseContext.f_AddLine();
				}

				Current = _ToParse.f_GetAt(++iParse);
			}

			if (Current == '/' && _ToParse.f_GetAt(iParse+1) == '/') // Comment
			{
				fsp_ParseToEndOfLine(_ToParse, iParse, _ParseContext);
			}
			else if (Current == '/' && _ToParse.f_GetAt(iParse+1) == '*') // Comment
			{
				aint StartParseLine = _ParseContext.f_GetLine();
				if (!fsp_ParseToEndOfComment(_ToParse, iParse, _ParseContext))
				{
					DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No end found for block comment") << _ParseContext.f_GetFile() << StartParseLine).f_GetStr());
				}
			}
			else if (Current == '{')
			{
				aint LineBeforeStart = _ParseContext.f_GetLine();
				aint iBeforeParse = iParse;
				++iParse;
				if (ParseMode == 1)
				{
					pReg = f_CreateChildNoPath(KeyName, mc_bSupportForceCreate);
					pReg->f_SetThisValue("");
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
						tf_CStr Temp = _ParseContext.f_GetNextWhiteSpace(_ToParse, iBeforeParse);
						aint iFind = Temp.f_FindCharReverse('\n');
						if (iFind >= 0)
						{
							tf_CStr Left = Temp.f_Left(iFind+1);
							tf_CStr Right = Temp.f_Extract(iFind+1);
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
						_ParseContext.f_SetStartWhiteSpace(iParse);
					}

					iParse = pReg->fpr_Parse<tf_bAllowLineBreakInEscapedString>(_ToParse, iParse, _ParseContext);
					Current = _ToParse.f_GetAt(iParse);
					if (Current != '}')
					{
						DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching '}' found") << _ParseContext.f_GetFile() << LineBeforeStart).f_GetStr());
					}
					++iParse;
				}

				bHadChildren = true;
				Current = _ToParse.f_GetAt(iParse);
			}
			else if (Current == '}')
			{
				if (ParseMode == 1)
					DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Mismatching key/value pairs within scope") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
				if constexpr (mc_bSupportWhiteSpace)
				{
					tf_CStr Temp = _ParseContext.f_GetNextWhiteSpace(_ToParse, iParse);
					aint iFind = Temp.f_FindCharReverse('\n');
					if (iFind >= 0)
					{
						tf_CStr Left = Temp.f_Left(iFind+1);
						tf_CStr Right = Temp.f_Extract(iFind+1);
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
					_ParseContext.f_SetStartWhiteSpace(iParse+1);
				}
				return iParse;
			}
			else if (Current)
			{
				aint iBeforeParse = iParse;
				bool bWasEscaped = false;
				tf_CStr Temp = fsp_ParseIdentifierStr<tf_bAllowLineBreakInEscapedString>(_ToParse, iParse, _ParseContext, bWasEscaped);
				if (ParseMode == 0)
				{
					if constexpr (mc_bSupportWhiteSpace)
						fl_FixupWhitespaceBeforeKey(iBeforeParse);
					bHadChildren = false;
					KeyName = Temp;
					bKeyNameWasEscaped = bWasEscaped;
					ParseMode = 1;
				}
				else
				{
					bHadChildren = false;
					pReg = f_CreateChildNoPath(KeyName, mc_bSupportForceCreate);
					if (bKeyNameWasEscaped)
						pReg->f_SetForceEscapedKey(true);
					if (bWasEscaped)
						pReg->f_SetForceEscapedValue(true);
					pReg->f_SetThisValue(TCRegistry_ParseValue<tf_CStr, t_CData>(Temp, bWasEscaped));
					if constexpr (mc_bSupportFileLine)
						pReg->mp_Key.f_SetFileLine(_ParseContext.f_GetFile(), _ParseContext.f_GetLine());
					if constexpr (mc_bSupportWhiteSpace)
						fl_FixupWhitespaceBeforeValue(iBeforeParse);
					ParseMode = 0;
				}
			}

			_ParseContext.f_SetLastAdded(pReg, bHadChildren);
			Current = _ToParse.f_GetAt(iParse);
		}

		if (ParseMode == 1)
			DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " Mismatching key/value pairs within scope") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());

		return iParse;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <bool tf_bAllowLineBreakInEscapedString, typename tf_CStr>
	void TCRegistry<t_CStr, t_CData, t_Flags>::fp_Parse(const tf_CStr &_ToParse, CParseContext &_ParseContext)
	{
		aint iParse = fpr_Parse<tf_bAllowLineBreakInEscapedString>(_ToParse, 0, _ParseContext);
		{
			ch32 Current = _ToParse.f_GetAt(iParse);
			if (Current == '}')
			{
				DMibError((NMib::NStr::CStr::CFormat(DMibPFileLineFormat " No matching '{{' found") << _ParseContext.f_GetFile() << _ParseContext.f_GetLine()).f_GetStr());
			}
		}

		if constexpr (mc_bSupportWhiteSpace)
		{
			aint StartWhiteSpace = _ParseContext.f_GetStartWhiteSpace();
			{
				bool bLastHadChildren = false;
				TCRegistry *pReg = _ParseContext.f_GetLastAdded(bLastHadChildren);
				if (pReg)
				{
					if (StartWhiteSpace != iParse)
					{
						tf_CStr Temp = _ToParse.f_Extract(StartWhiteSpace , iParse - StartWhiteSpace);

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
			mp_Key.f_SetFileLine(_File, 0);
		}

		fp_Parse<false>(_Text, Context);
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
		}

		fp_Parse<true>(_Text, Context);
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_ParseLax(NStream::CBinaryStream &_Stream, t_CStr const &_File)
	{
		f_ParseLaxStr(t_CStr::fs_ReadTextStream(_Stream), _File);
	}
}
