// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

namespace NMib::NContainer
{
	//
	// TCRegistryKeyStr
	//

	template <typename t_CStr>
	template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
	TCRegistryKeyStr<t_CStr>::TCRegistryKeyStr(TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent)
	{
	}

	template <typename t_CStr>
	void TCRegistryKeyStr<t_CStr>::f_Move(TCRegistryKeyStr &&_Other)
	{
		m_Name = fg_Move(_Other.m_Name);
	}

	template <typename t_CStr>
	template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
	void TCRegistryKeyStr<t_CStr>::f_NewSequence(TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent)
	{
	}

	template <typename t_CStr>
	auto TCRegistryKeyStr<t_CStr>::f_CompareKey(TCRegistryKeyStr const &_Right) const -> CRet
	{
		return m_Name.f_Cmp(_Right.m_Name);
	}

	template <typename t_CStr>
	template <typename tf_CKey>
	auto TCRegistryKeyStr<t_CStr>::f_Compare(tf_CKey const &_Right) const -> CRet
	{
		return m_Name.f_Cmp(_Right);
	}

	template <typename t_CStr>
	void TCRegistryKeyStr<t_CStr>::f_Copy(TCRegistryKeyStr const &_Src)
	{
		m_Name = _Src.m_Name;
	}

	template <typename t_CStr>
	void TCRegistryKeyStr<t_CStr>::f_Set(t_CStr const &_Str)
	{
		m_Name = _Str;
	}

	template <typename t_CStr>
	t_CStr const &TCRegistryKeyStr<t_CStr>::f_GetName() const
	{
		return m_Name;
	}

	template <typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStr<t_CStr>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_Name;
	}

	template <typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStr<t_CStr>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_Name;
	}

	template <typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStr<t_CStr>::f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const
	{
		_StringTable.f_FeedString(_Stream, m_Name);
	}

	template <typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStr<t_CStr>::f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable)
	{
		_StringTable.f_ConsumeString(_Stream, m_Name);
	}

	template <typename t_CStr>
	template <typename tf_CRegistry, typename tf_CStr>
	auto TCRegistryKeyStr<t_CStr>::fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind) -> tf_CRegistry *
	{
		return _This.mp_Children.m_Tree.f_FindEqual(_ToFind);
	}

	template <typename t_CStr>
	template <typename t_CIterator, typename tf_CStr>
	void TCRegistryKeyStr<t_CStr>::fs_FindIterator(t_CIterator &_Iterator, tf_CStr const &_ToFind)
	{
		_Iterator.f_FindEqualForward(_ToFind);
	}

	template <typename t_CStr>
	void TCRegistryKeyStr<t_CStr>::f_SetFileLine(t_CStr const &_File, int32 _Line)
	{
	}

	template <typename t_CStr>
	t_CStr TCRegistryKeyStr<t_CStr>::f_GetFile() const
	{
		return t_CStr();
	}

	template <typename t_CStr>
	int32 TCRegistryKeyStr<t_CStr>::f_GetLine() const
	{
		return 0;
	}

	template <typename t_CStr>
	bool TCRegistryKeyStr<t_CStr>::f_GetParsed(ERegistryWhiteSpaceLocation _Location) const
	{
		return false;
	}

	template <typename t_CStr>
	void TCRegistryKeyStr<t_CStr>::f_SetParsed(ERegistryWhiteSpaceLocation _Location, bool _bParsed)
	{
	}

	template <typename t_CStr>
	void TCRegistryKeyStr<t_CStr>::f_SetForceEscapedKey(bool _bForced)
	{
	}

	template <typename t_CStr>
	void TCRegistryKeyStr<t_CStr>::f_SetForceEscapedValue(bool _bForced)
	{
	}

	template <typename t_CStr>
	bool TCRegistryKeyStr<t_CStr>::f_GetForceEscapedKey() const
	{
		return false;
	}

	template <typename t_CStr>
	bool TCRegistryKeyStr<t_CStr>::f_GetForceEscapedValue() const
	{
		return false;
	}

	template <typename t_CStr>
	void TCRegistryKeyStr<t_CStr>::f_SetWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str)
	{
	}

	template <typename t_CStr>
	t_CStr TCRegistryKeyStr<t_CStr>::f_GetWhiteSpace(ERegistryWhiteSpaceLocation _Location) const
	{
		return t_CStr();
	}

	//
	// TCRegistryKeyStrMulti
	//

	template <typename t_CStr>
	void TCRegistryKeyStrMulti<t_CStr>::f_Move(TCRegistryKeyStrMulti &&_Other)
	{
		m_GenSequence = _Other.m_GenSequence;
		m_Name = fg_Move(_Other.m_Name);
	}

	template <typename t_CStr>
	template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
	TCRegistryKeyStrMulti<t_CStr>::TCRegistryKeyStrMulti(TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent)
	{
		if (_pParent)
			m_Sequence = _pParent->mp_Key.m_GenSequence++;
		else
			m_Sequence = 0;
		m_GenSequence = 0;
	}

	template <typename t_CStr>
	template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
	void TCRegistryKeyStrMulti<t_CStr>::f_NewSequence(TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent)
	{
		m_Sequence = _pParent->mp_Key.m_GenSequence++;
	}

	template <typename t_CStr>
	auto TCRegistryKeyStrMulti<t_CStr>::f_CompareKey(TCRegistryKeyStrMulti const &_Right) const -> CRet
	{
		CRet Ret = m_Name.f_Cmp(_Right.m_Name);
		if (Ret)
			return Ret;

		if (m_Sequence > _Right.m_Sequence)
			return 1;
		else if (m_Sequence < _Right.m_Sequence)
			return -1;
		return 0;
	}

	template <typename t_CStr>
	template <typename tf_CKey>
	auto TCRegistryKeyStrMulti<t_CStr>::f_Compare(tf_CKey const &_Right) const -> CRet
	{
		CRet Ret = m_Name.f_Cmp(_Right);
		if (Ret)
			return Ret;

		if (m_Sequence > 0)
			return 1;
/*				else if (m_Sequence < 0)
			return -1;*/
		return 0;
	}

	template <typename t_CStr>
	void TCRegistryKeyStrMulti<t_CStr>::f_Copy(TCRegistryKeyStrMulti const &_Src)
	{
		m_Name = _Src.m_Name;
	}

	template <typename t_CStr>
	void TCRegistryKeyStrMulti<t_CStr>::f_Set(t_CStr const &_Str)
	{
		m_Name = _Str;
	}

	template <typename t_CStr>
	t_CStr const &TCRegistryKeyStrMulti<t_CStr>::f_GetName() const
	{
		return m_Name;
	}

	template <typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStrMulti<t_CStr>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_Name;
		_Stream << m_Sequence;
		_Stream << m_GenSequence;
	}

	template <typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStrMulti<t_CStr>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_Name;
		_Stream >> m_Sequence;
		_Stream >> m_GenSequence;
	}

	template <typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrMulti<t_CStr>::f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const
	{
		_StringTable.f_FeedString(_Stream, m_Name);
		_Stream << m_Sequence;
		_Stream << m_GenSequence;
	}

	template <typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrMulti<t_CStr>::f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable)
	{
		_StringTable.f_ConsumeString(_Stream, m_Name);
		_Stream >> m_Sequence;
		_Stream >> m_GenSequence;
	}

	template <typename t_CStr>
	template <typename tf_CRegistry, typename tf_CStr>
	tf_CRegistry *TCRegistryKeyStrMulti<t_CStr>::fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind)
	{
		tf_CRegistry *pNode = _This.mp_Children.m_Tree.f_FindSmallestGreaterThanEqual(_ToFind);
		if (pNode && pNode->mp_Key.m_Name == _ToFind)
			return pNode;
		return nullptr;
	}

	template <typename t_CStr>
	template <typename t_CIterator, typename tf_CStr>
	void TCRegistryKeyStrMulti<t_CStr>::fs_FindIterator(t_CIterator &_Iterator, tf_CStr const &_ToFind)
	{
		_Iterator.f_FindSmallestGreaterThanEqualForward(_ToFind);
		if (_Iterator && _Iterator->mp_Key.m_Name != _ToFind)
			_Iterator.f_Clear();
	}

	template <typename t_CStr>
	void TCRegistryKeyStrMulti<t_CStr>::f_SetFileLine(t_CStr const &_File, int32 _Line)
	{
	}

	template <typename t_CStr>
	t_CStr TCRegistryKeyStrMulti<t_CStr>::f_GetFile() const
	{
		return t_CStr();
	}

	template <typename t_CStr>
	int32 TCRegistryKeyStrMulti<t_CStr>::f_GetLine() const
	{
		return 0;
	}

	template <typename t_CStr>
	bool TCRegistryKeyStrMulti<t_CStr>::f_GetParsed(ERegistryWhiteSpaceLocation _Location) const
	{
		return false;
	}

	template <typename t_CStr>
	void TCRegistryKeyStrMulti<t_CStr>::f_SetParsed(ERegistryWhiteSpaceLocation _Location, bool _bParsed)
	{
	}

	template <typename t_CStr>
	void TCRegistryKeyStrMulti<t_CStr>::f_SetForceEscapedKey(bool _bForced)
	{
	}

	template <typename t_CStr>
	void TCRegistryKeyStrMulti<t_CStr>::f_SetForceEscapedValue(bool _bForced)
	{
	}

	template <typename t_CStr>
	bool TCRegistryKeyStrMulti<t_CStr>::f_GetForceEscapedKey() const
	{
		return false;
	}

	template <typename t_CStr>
	bool TCRegistryKeyStrMulti<t_CStr>::f_GetForceEscapedValue() const
	{
		return false;
	}

	template <typename t_CStr>
	void TCRegistryKeyStrMulti<t_CStr>::f_SetWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str)
	{
	}

	template <typename t_CStr>
	t_CStr TCRegistryKeyStrMulti<t_CStr>::f_GetWhiteSpace(ERegistryWhiteSpaceLocation _Location) const
	{
		return t_CStr();
	}

	//
	// TCRegistryKeyStrPreserve
	//

	template <typename t_CStr>
	void TCRegistryKeyStrPreserve<t_CStr>::f_Move(TCRegistryKeyStrPreserve &&_Other)
	{
		m_GenSequence = _Other.m_GenSequence;
		m_Name = fg_Move(_Other.m_Name);
		m_Parsed = _Other.m_Parsed;
		m_bForceEscapedKey = _Other.m_bForceEscapedKey;
		m_bForceEscapedValue = _Other.m_bForceEscapedValue;
		m_Line = _Other.m_Line;
		m_File = fg_Move(_Other.m_File);
		for (mint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
			m_WhiteSpace[i] = fg_Move(_Other.m_WhiteSpace[i]);
	}

	template <typename t_CStr>
	template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
	TCRegistryKeyStrPreserve<t_CStr>::TCRegistryKeyStrPreserve
		(
		 	TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent
		)
	{
		if (_pParent)
			m_Sequence = _pParent->mp_Key.m_GenSequence++;
		else
			m_Sequence = 0;
		m_GenSequence = 0;
		m_Line = 0;
		m_bForceEscapedKey = false;
		m_bForceEscapedValue = false;
	}

	template <typename t_CStr>
	template <typename tf_CStr, typename tf_CData, ERegistryFlag tf_Flags>
	void TCRegistryKeyStrPreserve<t_CStr>::f_NewSequence
		(
		 	TCRegistry<tf_CStr, tf_CData, tf_Flags> *_pParent
		)
	{
		m_Sequence = _pParent->mp_Key.m_GenSequence++;
	}

	template <typename t_CStr>
	template <typename tf_CKey>
	auto TCRegistryKeyStrPreserve<t_CStr>::f_Compare(tf_CKey const &_Right) const -> CRet
	{
		CRet Ret = m_Name.f_Cmp(_Right);
		if (Ret)
			return Ret;

		if (m_Sequence > 0)
			return 1;
/*				else if (m_Sequence < 0)
			return -1;*/
		return 0;
	}

	template <typename t_CStr>
	auto TCRegistryKeyStrPreserve<t_CStr>::f_CompareKey(TCRegistryKeyStrPreserve const &_Right) const -> CRet
	{
		CRet Ret = m_Name.f_Cmp(_Right.m_Name);
		if (Ret)
			return Ret;

		if (m_Sequence > _Right.m_Sequence)
			return 1;
		else if (m_Sequence < _Right.m_Sequence)
			return -1;
		return 0;
	}

	template <typename t_CStr>
	void TCRegistryKeyStrPreserve<t_CStr>::f_Copy(TCRegistryKeyStrPreserve const &_Src)
	{
		m_Name = _Src.m_Name;
		m_File = _Src.m_File;
		m_Line = _Src.m_Line;
		m_Parsed = _Src.m_Parsed;
		m_bForceEscapedKey = _Src.m_bForceEscapedKey;
		m_bForceEscapedValue = _Src.m_bForceEscapedValue;

		for (mint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
			m_WhiteSpace[i] = _Src.m_WhiteSpace[i];
	}

	template <typename t_CStr>
	void TCRegistryKeyStrPreserve<t_CStr>::f_Set(t_CStr const &_Str)
	{
		m_Name = _Str;
	}

	template <typename t_CStr>
	t_CStr const &TCRegistryKeyStrPreserve<t_CStr>::f_GetName() const
	{
		return m_Name;
	}

	template <typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStrPreserve<t_CStr>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_Name;
		_Stream << m_Sequence;
		_Stream << m_GenSequence;
		_Stream << m_File;
		uint32 Flags = m_Line | m_bForceEscapedKey << 30 | m_bForceEscapedValue << 31;
		_Stream << Flags;

		for (mint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
		{
			_Stream << m_WhiteSpace[i];
		}

		_Stream << m_Parsed;
	}

	template <typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStrPreserve<t_CStr>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_Name;
		_Stream >> m_Sequence;
		_Stream >> m_GenSequence;
		_Stream >> m_File;

		uint32 Flags;
		_Stream >> Flags;
		m_Line = Flags & DMibBitRangeTyped(0, 29, uint32);
		m_bForceEscapedKey = (Flags & DMibBitRangeTyped(30, 30, uint32)) >> 30;
		m_bForceEscapedValue = (Flags & DMibBitRangeTyped(31, 31, uint32)) >> 31;

		for (mint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
		{
			_Stream >> m_WhiteSpace[i];
		}

		_Stream >> m_Parsed;
	}

	template <typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrPreserve<t_CStr>::f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const
	{
		_StringTable.f_FeedString(_Stream, m_Name);
		_Stream << m_Sequence;
		_Stream << m_GenSequence;
		_StringTable.f_FeedString(_Stream, m_File);
		uint32 Flags = m_Line | m_bForceEscapedKey << 30 | m_bForceEscapedValue << 31;
		_Stream << Flags;

		for (mint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
		{
			_StringTable.f_FeedString(_Stream, m_WhiteSpace[i]);
		}

		_Stream << m_Parsed;
	}

	template <typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrPreserve<t_CStr>::f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable)
	{
		_StringTable.f_ConsumeString(_Stream, m_Name);
		_Stream >> m_Sequence;
		_Stream >> m_GenSequence;
		_StringTable.f_ConsumeString(_Stream, m_File);

		uint32 Flags;
		_Stream >> Flags;
		m_Line = Flags & DMibBitRangeTyped(0, 29, uint32);
		m_bForceEscapedKey = (Flags & DMibBitRangeTyped(30, 30, uint32)) >> 30;
		m_bForceEscapedValue = (Flags & DMibBitRangeTyped(31, 31, uint32)) >> 31;

		for (mint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
		{
			_StringTable.f_ConsumeString(_Stream, m_WhiteSpace[i]);
		}

		_Stream >> m_Parsed;
	}

	template <typename t_CStr>
	template <typename tf_CRegistry, typename tf_CStr>
	tf_CRegistry *TCRegistryKeyStrPreserve<t_CStr>::fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind)
	{
		tf_CRegistry *pNode = _This.mp_Children.m_Tree.f_FindSmallestGreaterThanEqual(_ToFind);
		if (pNode && pNode->mp_Key.m_Name == _ToFind)
			return pNode;
		return nullptr;
	}

	template <typename t_CStr>
	template <typename t_CIterator, typename tf_CStr>
	void TCRegistryKeyStrPreserve<t_CStr>::fs_FindIterator(t_CIterator &_Iterator, tf_CStr const &_ToFind)
	{
		_Iterator.f_FindSmallestGreaterThanEqualForward(_ToFind);
		if (_Iterator && _Iterator->mp_Key.m_Name != _ToFind)
			_Iterator.f_Clear();
	}

	template <typename t_CStr>
	void TCRegistryKeyStrPreserve<t_CStr>::f_SetFileLine(t_CStr const &_File, int32 _Line)
	{
		m_File = _File;
		m_Line = _Line;
	}

	template <typename t_CStr>
	t_CStr const &TCRegistryKeyStrPreserve<t_CStr>::f_GetFile() const
	{
		return m_File;
	}

	template <typename t_CStr>
	int32 TCRegistryKeyStrPreserve<t_CStr>::f_GetLine() const
	{
		return m_Line;
	}

	template <typename t_CStr>
	bool TCRegistryKeyStrPreserve<t_CStr>::f_GetParsed(ERegistryWhiteSpaceLocation _Location) const
	{
		return m_Parsed.f_GetBit(_Location);
	}

	template <typename t_CStr>
	void TCRegistryKeyStrPreserve<t_CStr>::f_SetParsed(ERegistryWhiteSpaceLocation _Location, bool _bParsed)
	{
		m_Parsed.f_SetBit(_Location, _bParsed);
	}

	template <typename t_CStr>
	bool TCRegistryKeyStrPreserve<t_CStr>::f_GetForceEscapedKey() const
	{
		return m_bForceEscapedKey;
	}

	template <typename t_CStr>
	bool TCRegistryKeyStrPreserve<t_CStr>::f_GetForceEscapedValue() const
	{
		return m_bForceEscapedValue;
	}

	template <typename t_CStr>
	void TCRegistryKeyStrPreserve<t_CStr>::f_SetForceEscapedKey(bool _bForced)
	{
		m_bForceEscapedKey = _bForced;
	}

	template <typename t_CStr>
	void TCRegistryKeyStrPreserve<t_CStr>::f_SetForceEscapedValue(bool _bForced)
	{
		m_bForceEscapedValue = _bForced;
	}

	template <typename t_CStr>
	void TCRegistryKeyStrPreserve<t_CStr>::f_SetWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str)
	{
		m_WhiteSpace[_Location] = _Str;
	}

	template <typename t_CStr>
	t_CStr const &TCRegistryKeyStrPreserve<t_CStr>::f_GetWhiteSpace(ERegistryWhiteSpaceLocation _Location) const
	{
		return m_WhiteSpace[_Location];
	}
}
