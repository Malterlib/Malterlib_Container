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
		mp_Name = fg_Move(_Other.mp_Name);
	}

	template <typename t_CKey, typename t_CStr>
	COrdering_Partial TCRegistryKeyStr<t_CKey, t_CStr>::operator <=> (TCRegistryKeyStr const &_Right) const noexcept
	{
		return mp_Name <=> _Right.mp_Name;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CKey>
	COrdering_Partial TCRegistryKeyStr<t_CKey, t_CStr>::operator <=> (tf_CKey const &_Right) const noexcept
	{
		return mp_Name <=> _Right;
	}

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Copy(TCRegistryKeyStr const &_Src)
	{
		mp_Name = _Src.mp_Name;
	}

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Set(t_CKey const &_Str)
	{
		mp_Name = _Str;
	}

	template <typename t_CKey, typename t_CStr>
	t_CKey const &TCRegistryKeyStr<t_CKey, t_CStr>::f_GetName() const
	{
		return mp_Name;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << mp_Name;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> mp_Name;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const
	{
		_StringTable.f_FeedString(_Stream, mp_Name);
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStr<t_CKey, t_CStr>::f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable)
	{
		_StringTable.f_ConsumeString(_Stream, mp_Name);
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CRegistry, typename tf_CStr>
	auto TCRegistryKeyStr<t_CKey, t_CStr>::fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind) -> tf_CRegistry *
	{
		return _This.mp_Children.m_Tree.f_FindEqual(_ToFind);
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CIterator, typename tf_CStr>
	void TCRegistryKeyStr<t_CKey, t_CStr>::fs_FindIterator(tf_CIterator &_Iterator, tf_CStr const &_ToFind)
	{
		_Iterator.f_FindEqualForward(_ToFind);
	}

	//
	// TCRegistryKeyStrMulti
	//

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Move(TCRegistryKeyStrMulti &&_Other)
	{
		mp_GenSequence = _Other.mp_GenSequence;
		mp_Name = fg_Move(_Other.mp_Name);
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	TCRegistryKeyStrMulti<t_CKey, t_CStr>::TCRegistryKeyStrMulti(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent)
		: mp_Sequence(_pParent ? _pParent->mp_Key.mp_GenSequence++ : 0)
	{
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_NewSequence(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent)
	{
		mp_Sequence = _pParent->mp_Key.mp_GenSequence++;
	}

	template <typename t_CKey, typename t_CStr>
	COrdering_Partial TCRegistryKeyStrMulti<t_CKey, t_CStr>::operator <=> (TCRegistryKeyStrMulti const &_Right) const noexcept
	{
		if (auto Result = mp_Name <=> _Right.mp_Name; Result != 0)
			return Result;

		return mp_Sequence <=> _Right.mp_Sequence;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CKey>
	COrdering_Partial TCRegistryKeyStrMulti<t_CKey, t_CStr>::operator <=> (tf_CKey const &_Right) const noexcept
	{
		if (auto Result = mp_Name <=> _Right; Result != 0)
			return Result;

		if (mp_Sequence > 0)
			return COrdering_Partial::greater;

		return COrdering_Partial::equivalent;
	}

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Copy(TCRegistryKeyStrMulti const &_Src)
	{
		mp_Name = _Src.mp_Name;
	}

	template <typename t_CKey, typename t_CStr>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Set(t_CStr const &_Str)
	{
		mp_Name = _Str;
	}

	template <typename t_CKey, typename t_CStr>
	t_CKey const &TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_GetName() const
	{
		return mp_Name;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << mp_Name;
		_Stream << mp_Sequence;
		_Stream << mp_GenSequence;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> mp_Name;
		_Stream >> mp_Sequence;
		_Stream >> mp_GenSequence;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const
	{
		_StringTable.f_FeedString(_Stream, mp_Name);
		_Stream << mp_Sequence;
		_Stream << mp_GenSequence;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable)
	{
		_StringTable.f_ConsumeString(_Stream, mp_Name);
		_Stream >> mp_Sequence;
		_Stream >> mp_GenSequence;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CRegistry, typename tf_CStr>
	tf_CRegistry *TCRegistryKeyStrMulti<t_CKey, t_CStr>::fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind)
	{
		tf_CRegistry *pNode = _This.mp_Children.m_Tree.f_FindSmallestGreaterThanEqual(_ToFind);
		if (pNode && pNode->mp_Key.mp_Name == _ToFind)
			return pNode;
		return nullptr;
	}

	template <typename t_CKey, typename t_CStr>
	template <typename tf_CIterator, typename tf_CStr>
	void TCRegistryKeyStrMulti<t_CKey, t_CStr>::fs_FindIterator(tf_CIterator &_Iterator, tf_CStr const &_ToFind)
	{
		_Iterator.f_FindSmallestGreaterThanEqualForward(_ToFind);
		if (_Iterator && _Iterator->mp_Key.mp_Name != _ToFind)
			_Iterator.f_Clear();
	}

	//
	// TCRegistryKeyStrPreserve
	//

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Move(TCRegistryKeyStrPreserve &&_Other)
	{
		mp_GenSequence = _Other.mp_GenSequence;
		mp_Name = fg_Move(_Other.mp_Name);
		mp_Parsed = _Other.mp_Parsed;
		mp_bForceEscapedKey = _Other.mp_bForceEscapedKey;
		mp_bForceEscapedValue = _Other.mp_bForceEscapedValue;
		mp_Location = fg_Move(_Other.mp_Location);
		mp_ValueLocation = fg_Move(_Other.mp_ValueLocation);
		for (umint iLocation = 0; iLocation < ERegistryWhiteSpaceLocation_Max; ++iLocation)
			mp_WhiteSpace[iLocation] = fg_Move(_Other.mp_WhiteSpace[iLocation]);
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::TCRegistryKeyStrPreserve(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent)
		: mp_Sequence(_pParent ? _pParent->mp_Key.mp_GenSequence++ : 0)
		, mp_bForceEscapedKey(false)
		, mp_bForceEscapedValue(false)
	{
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CKey, typename tf_CData, ERegistryFlag tf_Flags, typename tf_CStr>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_NewSequence(TCRegistry<tf_CKey, tf_CData, tf_Flags, tf_CStr> *_pParent)
	{
		mp_Sequence = _pParent->mp_Key.mp_GenSequence++;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CKey>
	COrdering_Partial TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::operator <=> (tf_CKey const &_Right) const noexcept
	{
		if (auto Result = mp_Name <=> _Right; Result != 0)
			return Result;

		if (mp_Sequence > 0)
			return COrdering_Partial::greater;

		return COrdering_Partial::equivalent;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	COrdering_Partial TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::operator <=> (TCRegistryKeyStrPreserve const &_Right) const noexcept
	{
		if (auto Result = mp_Name <=> _Right.mp_Name; Result != 0)
			return Result;

		return mp_Sequence <=> _Right.mp_Sequence;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Copy(TCRegistryKeyStrPreserve const &_Src)
	{
		mp_Name = _Src.mp_Name;
		mp_Location = _Src.mp_Location;
		mp_ValueLocation = _Src.mp_ValueLocation;
		mp_Parsed = _Src.mp_Parsed;
		mp_bForceEscapedKey = _Src.mp_bForceEscapedKey;
		mp_bForceEscapedValue = _Src.mp_bForceEscapedValue;

		for (umint iLocation = 0; iLocation < ERegistryWhiteSpaceLocation_Max; ++iLocation)
			mp_WhiteSpace[iLocation] = _Src.mp_WhiteSpace[iLocation];
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Set(t_CKey const &_Key)
	{
		mp_Name = _Key;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	t_CKey const &TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetName() const
	{
		return mp_Name;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CStream>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << mp_Name;
		_Stream << mp_Sequence;
		_Stream << mp_GenSequence;

		if constexpr (t_Flags & ERegistryFlag_FullLocation)
		{
			_Stream << mp_Location;
			_Stream << mp_ValueLocation;

			uint32 Flags = uint32(mp_bForceEscapedKey) | uint32(mp_bForceEscapedValue) << 1;
			_Stream << Flags;
		}
		else
		{
			_Stream << mp_Location.m_File;
			uint32 Flags = mp_Location.m_Line | mp_bForceEscapedKey << 30 | mp_bForceEscapedValue << 31;
			_Stream << Flags;
		}

		for (umint iLocation = 0; iLocation < ERegistryWhiteSpaceLocation_Max; ++iLocation)
			_Stream << mp_WhiteSpace[iLocation];

		_Stream << mp_Parsed;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CStream>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> mp_Name;
		_Stream >> mp_Sequence;
		_Stream >> mp_GenSequence;

		if constexpr (t_Flags & ERegistryFlag_FullLocation)
		{
			_Stream >> mp_Location;
			_Stream >> mp_ValueLocation;

			uint32 Flags;
			_Stream >> Flags;
			mp_bForceEscapedKey = Flags & 1u;
			mp_bForceEscapedValue = (Flags >> 1u) & 1u;
		}
		else
		{
			_Stream >> mp_Location.m_File;
			uint32 Flags;
			_Stream >> Flags;
			mp_Location.m_Line = Flags & DMibBitRangeTyped(0, 29, uint32);
			mp_bForceEscapedKey = (Flags & DMibBitRangeTyped(30, 30, uint32)) >> 30;
			mp_bForceEscapedValue = (Flags & DMibBitRangeTyped(31, 31, uint32)) >> 31;
		}

		for (umint iLocation = 0; iLocation < ERegistryWhiteSpaceLocation_Max; ++iLocation)
			_Stream >> mp_WhiteSpace[iLocation];

		_Stream >> mp_Parsed;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const
	{
		_StringTable.f_FeedString(_Stream, mp_Name);
		_Stream << mp_Sequence;
		_Stream << mp_GenSequence;
		_StringTable.f_FeedString(_Stream, mp_Location.m_File);

		if constexpr (t_Flags & ERegistryFlag_FullLocation)
		{
			_Stream << mp_Location.m_Line;
			_Stream << mp_Location.m_Column;
			_Stream << mp_Location.m_Character;

			_StringTable.f_FeedString(_Stream, mp_ValueLocation.m_File);
			_Stream << mp_ValueLocation.m_Line;
			_Stream << mp_ValueLocation.m_Column;
			_Stream << mp_ValueLocation.m_Character;

			uint32 Flags = uint32(mp_bForceEscapedKey) | uint32(mp_bForceEscapedValue) << 1;
			_Stream << Flags;
		}
		else
		{
			uint32 Flags = mp_Location.m_Line | mp_bForceEscapedKey << 30 | mp_bForceEscapedValue << 31;
			_Stream << Flags;
		}

		for (umint iLocation = 0; iLocation < ERegistryWhiteSpaceLocation_Max; ++iLocation)
			_StringTable.f_FeedString(_Stream, mp_WhiteSpace[iLocation]);

		_Stream << mp_Parsed;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable)
	{
		_StringTable.f_ConsumeString(_Stream, mp_Name);
		_Stream >> mp_Sequence;
		_Stream >> mp_GenSequence;
		_StringTable.f_ConsumeString(_Stream, mp_Location.m_File);

		if constexpr (t_Flags & ERegistryFlag_FullLocation)
		{
			_Stream >> mp_Location.m_Line;
			_Stream >> mp_Location.m_Column;
			_Stream >> mp_Location.m_Character;

			_StringTable.f_ConsumeString(_Stream, mp_ValueLocation.m_File);
			_Stream >> mp_ValueLocation.m_Line;
			_Stream >> mp_ValueLocation.m_Column;
			_Stream >> mp_ValueLocation.m_Character;

			uint32 Flags;
			_Stream >> Flags;
			mp_bForceEscapedKey = Flags & 1u;
			mp_bForceEscapedValue = (Flags >> 1u) & 1u;
		}
		else
		{
			uint32 Flags;
			_Stream >> Flags;
			mp_Location.m_Line = Flags & DMibBitRangeTyped(0, 29, uint32);
			mp_bForceEscapedKey = (Flags & DMibBitRangeTyped(30, 30, uint32)) >> 30;
			mp_bForceEscapedValue = (Flags & DMibBitRangeTyped(31, 31, uint32)) >> 31;
		}

		for (umint iLocation = 0; iLocation < ERegistryWhiteSpaceLocation_Max; ++iLocation)
			_StringTable.f_ConsumeString(_Stream, mp_WhiteSpace[iLocation]);

		_Stream >> mp_Parsed;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CRegistry, typename tf_CStr>
	tf_CRegistry *TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::fs_FindEqual(tf_CRegistry const &_This, tf_CStr const &_ToFind)
	{
		tf_CRegistry *pNode = _This.mp_Children.m_Tree.f_FindSmallestGreaterThanEqual(_ToFind);
		if (pNode && pNode->mp_Key.mp_Name == _ToFind)
			return pNode;
		return nullptr;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	template <typename tf_CIterator, typename tf_CStr>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::fs_FindIterator(tf_CIterator &_Iterator, tf_CStr const &_ToFind)
	{
		_Iterator.f_FindSmallestGreaterThanEqualForward(_ToFind);
		if (_Iterator && _Iterator->mp_Key.mp_Name != _ToFind)
			_Iterator.f_Clear();
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetLocation(CLocation const &_Location)
	{
		mp_Location = _Location;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetValueLocation(CValueLocation const &_Location)
	{
		mp_ValueLocation = _Location;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	auto TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetLocation() const -> CLocation const &
	{
		return mp_Location;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	auto TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetValueLocation() const -> CValueLocation const &
	{
		return mp_ValueLocation;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	bool TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetParsed(ERegistryWhiteSpaceLocation _Location) const
	{
		return mp_Parsed.f_GetBit(_Location);
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetParsed(ERegistryWhiteSpaceLocation _Location, bool _bParsed)
	{
		mp_Parsed.f_SetBit(_Location, _bParsed);
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	bool TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetForceEscapedKey() const
	{
		return mp_bForceEscapedKey;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	bool TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetForceEscapedValue() const
	{
		return mp_bForceEscapedValue;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetForceEscapedKey(bool _bForced)
	{
		mp_bForceEscapedKey = _bForced;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetForceEscapedValue(bool _bForced)
	{
		mp_bForceEscapedValue = _bForced;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	void TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_SetWhiteSpace(ERegistryWhiteSpaceLocation _Location, t_CStr const &_Str)
	{
		mp_WhiteSpace[_Location] = _Str;
	}

	template <typename t_CKey, typename t_CStr, ERegistryFlag t_Flags>
	t_CStr const &TCRegistryKeyStrPreserve<t_CKey, t_CStr, t_Flags>::f_GetWhiteSpace(ERegistryWhiteSpaceLocation _Location) const
	{
		return mp_WhiteSpace[_Location];
	}

	template <typename tf_CKey>
	auto fg_RegistryNameStringForPath(tf_CKey const &_Key)
	{
		return _Key;
	}
}
