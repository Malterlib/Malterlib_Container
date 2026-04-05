// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

namespace NMib::NContainer
{
	//
	// TCRegistryKeyStr
	//

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	TCRegistryKeyStr<t_CKey, t_CStr>::TCRegistryKeyStr(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent)
	{
	}

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Move(TCRegistryKeyStr &&_Other)
	{
		m_Name = fg_Move(_Other.m_Name);
	}

	template <typename t_CKey, typename t_CStr>
	COrdering_Partial TCRegistryKeyStr<t_CKey, t_CStr>::operator <=> (TCRegistryKeyStr const &_Right) const noexcept
	{
		return m_Name <=> _Right.m_Name;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CKey>
	COrdering_Partial TCRegistryKeyStr<t_CKey, t_CStr>::operator <=> (tf_CKey const &_Right) const noexcept
	{
		return m_Name <=> _Right;
	}

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Copy(TCRegistryKeyStr const &_Src)
	{
		m_Name = _Src.m_Name;
	}

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Set(t_CKey const &_Str)
	{
		m_Name = _Str;
	}

	template <typename t_CKey, typename t_CStr>
	t_CKey const &TCRegistryKeyStr<t_CKey, t_CStr>::f_GetName() const
	{
		return m_Name;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_Name;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_Name;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const
	{
		_StringTable.f_FeedString(_Stream, m_Name);
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable)
	{
		_StringTable.f_ConsumeString(_Stream, m_Name);
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CRegistry, typename tf_CStr>
	auto TCRegistryKeyStr<t_CKey, t_CStr>::fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind) -> tf_CRegistry *
	{
		return _This.mp_Children.m_Tree.f_FindEqual(_ToFind);
	}

	template <typename t_CKey, typename t_CStr>
	template <typename t_CIterator, typename tf_CStr>
	void TCRegistryKeyStr<t_CKey, t_CStr>::fs_FindIterator(t_CIterator &_Iterator, tf_CStr const &_ToFind)
	{
		_Iterator.f_FindEqualForward(_ToFind);
	}

	//
	// TCRegistryKeyStrMulti
	//

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Move(TCRegistryKeyStrMulti &&_Other)
	{
		m_GenSequence = _Other.m_GenSequence;
		m_Name = fg_Move(_Other.m_Name);
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	TCRegistryKeyStrMulti<t_CKey, t_CStr>::TCRegistryKeyStrMulti(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent)
		: m_Sequence(_pParent ? _pParent->mp_Key.m_GenSequence++ : 0)
	{
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_NewSequence(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent)
	{
		m_Sequence = _pParent->mp_Key.m_GenSequence++;
	}

	template <typename t_CKey, typename t_CStr>
	COrdering_Partial TCRegistryKeyStrMulti<t_CKey, t_CStr>::operator <=> (TCRegistryKeyStrMulti const &_Right) const noexcept
	{
		if (auto Result = m_Name <=> _Right.m_Name; Result != 0)
			return Result;

		return m_Sequence <=> _Right.m_Sequence;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CKey>
	COrdering_Partial TCRegistryKeyStrMulti<t_CKey, t_CStr>::operator <=> (tf_CKey const &_Right) const noexcept
	{
		if (auto Result = m_Name <=> _Right; Result != 0)
			return Result;

		if (m_Sequence > 0)
			return COrdering_Partial::greater;

		return COrdering_Partial::equivalent;
	}

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Copy(TCRegistryKeyStrMulti const &_Src)
	{
		m_Name = _Src.m_Name;
	}

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Set(t_CStr const &_Str)
	{
		m_Name = _Str;
	}

	template <typename t_CKey, typename t_CStr>
	t_CKey const &TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_GetName() const
	{
		return m_Name;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_Name;
		_Stream << m_Sequence;
		_Stream << m_GenSequence;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_Name;
		_Stream >> m_Sequence;
		_Stream >> m_GenSequence;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const
	{
		_StringTable.f_FeedString(_Stream, m_Name);
		_Stream << m_Sequence;
		_Stream << m_GenSequence;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable)
	{
		_StringTable.f_ConsumeString(_Stream, m_Name);
		_Stream >> m_Sequence;
		_Stream >> m_GenSequence;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CRegistry, typename tf_CStr>
	tf_CRegistry *TCRegistryKeyStrMulti<t_CKey, t_CStr>::fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind)
	{
		tf_CRegistry *pNode = _This.mp_Children.m_Tree.f_FindSmallestGreaterThanEqual(_ToFind);
		if (pNode && pNode->mp_Key.m_Name == _ToFind)
			return pNode;
		return nullptr;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename t_CIterator, typename tf_CStr>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::fs_FindIterator(t_CIterator &_Iterator, tf_CStr const &_ToFind)
	{
		_Iterator.f_FindSmallestGreaterThanEqualForward(_ToFind);
		if (_Iterator && _Iterator->mp_Key.m_Name != _ToFind)
			_Iterator.f_Clear();
	}

	//
	// TCRegistryKeyStrPreserve
	//

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Move(TCRegistryKeyStrPreserve &&_Other)
	{
		m_GenSequence = _Other.m_GenSequence;
		m_Name = fg_Move(_Other.m_Name);
		m_Parsed = _Other.m_Parsed;
		m_bForceEscapedKey = _Other.m_bForceEscapedKey;
		m_bForceEscapedValue = _Other.m_bForceEscapedValue;
		m_Location = fg_Move(_Other.m_Location);
		for (umint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
			m_WhiteSpace[i] = fg_Move(_Other.m_WhiteSpace[i]);
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::TCRegistryKeyStrPreserve
		(
			TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent
		)
		: m_Sequence(_pParent ? _pParent->mp_Key.m_GenSequence++ : 0)
		, m_bForceEscapedKey(false)
		, m_bForceEscapedValue(false)
	{
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_NewSequence(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent)
	{
		m_Sequence = _pParent->mp_Key.m_GenSequence++;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CKey>
	COrdering_Partial TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::operator <=> (tf_CKey const &_Right) const noexcept
	{
		if (auto Result = m_Name <=> _Right; Result != 0)
			return Result;

		if (m_Sequence > 0)
			return COrdering_Partial::greater;

		return COrdering_Partial::equivalent;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	COrdering_Partial TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::operator <=> (TCRegistryKeyStrPreserve const &_Right) const noexcept
	{
		if (auto Result = m_Name <=> _Right.m_Name; Result != 0)
			return Result;

		return m_Sequence <=> _Right.m_Sequence;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Copy(TCRegistryKeyStrPreserve const &_Src)
	{
		m_Name = _Src.m_Name;
		m_Location = _Src.m_Location;
		m_Parsed = _Src.m_Parsed;
		m_bForceEscapedKey = _Src.m_bForceEscapedKey;
		m_bForceEscapedValue = _Src.m_bForceEscapedValue;

		for (umint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
			m_WhiteSpace[i] = _Src.m_WhiteSpace[i];
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Set(t_CKey const &_Key)
	{
		m_Name = _Key;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	t_CKey const &TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetName() const
	{
		return m_Name;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CStream>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_Name;
		_Stream << m_Sequence;
		_Stream << m_GenSequence;

		if constexpr (t_Flags & ERegistryFlag_FullLocation)
		{
			_Stream << m_Location;
			_Stream << m_ValueLocation;

			uint32 Flags = uint32(m_bForceEscapedKey) | uint32(m_bForceEscapedValue) << 1;
			_Stream << Flags;
		}
		else
		{
			_Stream << m_Location.m_File;
			uint32 Flags = m_Location.m_Line | m_bForceEscapedKey << 30 | m_bForceEscapedValue << 31;
			_Stream << Flags;
		}

		for (umint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
			_Stream << m_WhiteSpace[i];

		_Stream << m_Parsed;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CStream>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_Name;
		_Stream >> m_Sequence;
		_Stream >> m_GenSequence;

		if constexpr (t_Flags & ERegistryFlag_FullLocation)
		{
			_Stream >> m_Location;
			_Stream >> m_ValueLocation;

			uint32 Flags;
			_Stream >> Flags;
			m_bForceEscapedKey = Flags & 1u;
			m_bForceEscapedValue = (Flags >> 1u) & 1u;
		}
		else
		{
			_Stream >> m_Location.m_File;
			uint32 Flags;
			_Stream >> Flags;
			m_Location.m_Line = Flags & DMibBitRangeTyped(0, 29, uint32);
			m_bForceEscapedKey = (Flags & DMibBitRangeTyped(30, 30, uint32)) >> 30;
			m_bForceEscapedValue = (Flags & DMibBitRangeTyped(31, 31, uint32)) >> 31;
		}

		for (umint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
			_Stream >> m_WhiteSpace[i];

		_Stream >> m_Parsed;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const
	{
		_StringTable.f_FeedString(_Stream, m_Name);
		_Stream << m_Sequence;
		_Stream << m_GenSequence;
		_StringTable.f_FeedString(_Stream, m_Location.m_File);

		if constexpr (t_Flags & ERegistryFlag_FullLocation)
		{
			_Stream << m_Location.m_Line;
			_Stream << m_Location.m_Column;
			_Stream << m_Location.m_Character;

			_StringTable.f_FeedString(_Stream, m_ValueLocation.m_File);
			_Stream << m_ValueLocation.m_Line;
			_Stream << m_ValueLocation.m_Column;
			_Stream << m_ValueLocation.m_Character;

			uint32 Flags = uint32(m_bForceEscapedKey) | uint32(m_bForceEscapedValue) << 1;
			_Stream << Flags;
		}
		else
		{
			uint32 Flags = m_Location.m_Line | m_bForceEscapedKey << 30 | m_bForceEscapedValue << 31;
			_Stream << Flags;
		}

		for (umint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
			_StringTable.f_FeedString(_Stream, m_WhiteSpace[i]);

		_Stream << m_Parsed;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable)
	{
		_StringTable.f_ConsumeString(_Stream, m_Name);
		_Stream >> m_Sequence;
		_Stream >> m_GenSequence;
		_StringTable.f_ConsumeString(_Stream, m_Location.m_File);

		if constexpr (t_Flags & ERegistryFlag_FullLocation)
		{
			_Stream >> m_Location.m_Line;
			_Stream >> m_Location.m_Column;
			_Stream >> m_Location.m_Character;

			_StringTable.f_ConsumeString(_Stream, m_ValueLocation.m_File);
			_Stream >> m_ValueLocation.m_Line;
			_Stream >> m_ValueLocation.m_Column;
			_Stream >> m_ValueLocation.m_Character;

			uint8 Flags;
			_Stream >> Flags;
			m_bForceEscapedKey = Flags & 1u;
			m_bForceEscapedValue = (Flags >> 1u) & 1u;
		}
		else
		{
			uint32 Flags;
			_Stream >> Flags;
			m_Location.m_Line = Flags & DMibBitRangeTyped(0, 29, uint32);
			m_bForceEscapedKey = (Flags & DMibBitRangeTyped(30, 30, uint32)) >> 30;
			m_bForceEscapedValue = (Flags & DMibBitRangeTyped(31, 31, uint32)) >> 31;
		}

		for (umint i = 0; i < ERegistryWhiteSpaceLocation_Max; ++i)
			_StringTable.f_ConsumeString(_Stream, m_WhiteSpace[i]);

		_Stream >> m_Parsed;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CRegistry, typename tf_CStr>
	tf_CRegistry *TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind)
	{
		tf_CRegistry *pNode = _This.mp_Children.m_Tree.f_FindSmallestGreaterThanEqual(_ToFind);
		if (pNode && pNode->mp_Key.m_Name == _ToFind)
			return pNode;
		return nullptr;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename t_CIterator, typename tf_CStr>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::fs_FindIterator(t_CIterator &_Iterator, tf_CStr const &_ToFind)
	{
		_Iterator.f_FindSmallestGreaterThanEqualForward(_ToFind);
		if (_Iterator && _Iterator->mp_Key.m_Name != _ToFind)
			_Iterator.f_Clear();
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetLocation(CLocation const &_Location)
	{
		m_Location = _Location;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetValueLocation(CValueLocation const &_Location)
	{
		m_Location = _Location;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	auto TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetLocation() const -> CLocation const &
	{
		return m_Location;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	auto TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetValueLocation() const -> CValueLocation const &
	{
		return m_Location;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	bool TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetParsed(ERegistryWhiteSpaceLocation _Location) const
	{
		return m_Parsed.f_GetBit(_Location);
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetParsed(ERegistryWhiteSpaceLocation _Location, bool _bParsed)
	{
		m_Parsed.f_SetBit(_Location, _bParsed);
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	bool TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetForceEscapedKey() const
	{
		return m_bForceEscapedKey;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	bool TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetForceEscapedValue() const
	{
		return m_bForceEscapedValue;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetForceEscapedKey(bool _bForced)
	{
		m_bForceEscapedKey = _bForced;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetForceEscapedValue(bool _bForced)
	{
		m_bForceEscapedValue = _bForced;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str)
	{
		m_WhiteSpace[_Location] = _Str;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	t_CStr const &TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetWhiteSpace(ERegistryWhiteSpaceLocation _Location) const
	{
		return m_WhiteSpace[_Location];
	}

	template <typename tf_CKey>
	auto fg_RegistryNameStringForPath(tf_CKey const &_Key)
	{
		return _Key;
	}
}
