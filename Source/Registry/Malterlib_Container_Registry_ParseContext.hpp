// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::CPreserveParseContext()
	{
		m_Line = 1;
		m_iLastStartWhitespace = 0;
		m_pLastAdded = nullptr;
		m_bLastHadChildren = false;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_SetFile(t_CStr const &_File)
	{
		m_File = _File;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	t_CStr const &TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_GetFile() const
	{
		return m_File;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	int32 TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_GetLine() const
	{
		return m_Line;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_AddLine()
	{
		++m_Line;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_SetStartWhiteSpace(aint _iPos)
	{
		m_iLastStartWhitespace = _iPos;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	aint TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_GetStartWhiteSpace() const
	{
		return m_iLastStartWhitespace;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	t_CStr TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_GetNextWhiteSpace
		(
			t_CStr const &_ToParse
			, aint _iParse
		)
	{
		return _ToParse.f_Extract(f_GetStartWhiteSpace(), _iParse - f_GetStartWhiteSpace());
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_SetWhiteSpace
		(
			ERegistryWhiteSpaceLocation _Location
			, t_CStr const &_Str
		)
	{
		m_WhiteSpace[_Location] = _Str;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	t_CStr const &TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_GetWhiteSpace
		(
			ERegistryWhiteSpaceLocation _Location
		)
		const
	{
		return m_WhiteSpace[_Location];
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_SetLastAdded
		(
			TCRegistry *_pReg
			, bool _bHadChildren
		)
	{
		m_pLastAdded = _pReg;
		m_bLastHadChildren = _bHadChildren;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::CPreserveParseContext::f_GetLastAdded
		(
			bool &_bLastHadChildren
		)
		const -> TCRegistry *
	{
		_bLastHadChildren = m_bLastHadChildren;
		return m_pLastAdded;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_SetFile(t_CStr const &_File)
	{
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	ch8 const *TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_GetFile() const
	{
		return "";
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	int32 TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_GetLine() const
	{
		return 0;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_AddLine()
	{

	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_SetStartWhiteSpace(aint _iPos)
	{

	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	aint TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_GetStartWhiteSpace() const
	{
		return -1;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_SetWhiteSpace
		(
			ERegistryWhiteSpaceLocation _Location
			, t_CStr const &_Str
		)
	{
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	t_CStr TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_GetNextWhiteSpace
		(
			t_CStr const &_ToParse
			, aint _iParse
		)
	{
		return t_CStr();
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	t_CStr TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_GetWhiteSpace
		(
			ERegistryWhiteSpaceLocation _Location
		) const
	{
		return t_CStr();
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_SetLastAdded(TCRegistry *_pReg, bool _bHadChildren)
	{
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	auto TCRegistry<t_CStr, t_CData, t_Flags>::CEmptyParseContext::f_GetLastAdded
		(
			bool &o_bLastHadChildren
		)
		const -> TCRegistry *
	{
		return nullptr;
	}
}
