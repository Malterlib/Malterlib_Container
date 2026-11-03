// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CIndex>
	auto TCRegistryStringTable<t_CStr, t_CIndex>::CIndex::CCompare_Index::operator () (CIndex const &_Node) const -> t_CIndex const &
	{
		return _Node.m_Index;
	}

	template <typename t_CStr, typename t_CIndex>
	template <typename tf_CStream>
	void TCRegistryStringTable<t_CStr, t_CIndex>::CIndex::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_Index;
	}

	template <typename t_CStr, typename t_CIndex>
	template <typename tf_CStream>
	void TCRegistryStringTable<t_CStr, t_CIndex>::CIndex::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_Index;
	}

	template <typename t_CStr, typename t_CIndex>
	TCRegistryStringTable<t_CStr, t_CIndex>::TCRegistryStringTable()
		: mp_CurrentIndex(0)
	{
	}

	template <typename t_CStr, typename t_CIndex>
	template <typename tf_CStream>
	void TCRegistryStringTable<t_CStr, t_CIndex>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << mp_CurrentIndex;
		_Stream << mp_Strings;
	}

	template <typename t_CStr, typename t_CIndex>
	template <typename tf_CStream>
	void TCRegistryStringTable<t_CStr, t_CIndex>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> mp_CurrentIndex;
		_Stream >> mp_Strings;
		for (auto iString = mp_Strings.f_GetIterator(); iString; ++iString)
			mp_IndexToString.f_Insert(*iString);
	}

	template <typename t_CStr, typename t_CIndex>
	template <typename tf_CStream, typename tf_CStr>
	void TCRegistryStringTable<t_CStr, t_CIndex>::f_FeedString(tf_CStream &_Stream, tf_CStr const &_String)
	{
		_Stream << f_GetIndex(_String);
	}

	template <typename t_CStr, typename t_CIndex>
	template <typename tf_CStream, typename tf_CStr>
	void TCRegistryStringTable<t_CStr, t_CIndex>::f_ConsumeString(tf_CStream &_Stream, tf_CStr &_String) const
	{
		t_CIndex Index;
		_Stream >> Index;
		t_CStr const *pStr = f_GetString(Index);
		if (pStr)
			_String = *pStr;
		else
			DMibErrorFile("Invalid string index in stream");
	}

	template <typename t_CStr, typename t_CIndex>
	template <typename tf_CStr>
	t_CIndex TCRegistryStringTable<t_CStr, t_CIndex>::f_GetIndex(tf_CStr const &_String)
	{
		auto Mapped = mp_Strings(_String);
		CIndex &Index = *Mapped;
		if (Mapped.f_WasCreated())
		{
			Index.m_Index = mp_CurrentIndex;
			++mp_CurrentIndex;
			mp_IndexToString.f_Insert(Index);
		}

		return Index.m_Index;
	}

	template <typename t_CStr, typename t_CIndex>
	t_CStr const *TCRegistryStringTable<t_CStr, t_CIndex>::f_GetString(t_CIndex const &_Index) const
	{
		CIndex const *pIndex = mp_IndexToString.f_FindEqual(_Index);
		if (pIndex)
			return &mp_Strings.fs_GetKey(pIndex);
		return nullptr;
	}
}
