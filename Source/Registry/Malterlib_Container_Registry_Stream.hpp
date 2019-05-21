 // Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::fp_FixChildren()
	{
		auto Iter = mp_Children.f_GetIterator();
		while (Iter)
		{
			Iter->mp_pParent = this;
			++Iter;
		}
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStream>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_Feed(tf_CStream &_Stream) const
	{
		mp_Key.f_Feed(_Stream);
		_Stream << mp_Data;
		_Stream << mp_Children;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStream>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_Consume(tf_CStream &_Stream)
	{
		mp_Key.f_Consume(_Stream);
		_Stream >> mp_Data;
		_Stream >> mp_Children;
		fp_FixChildren();
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_Feed(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> &_StringTable) const
	{
		mp_Key.f_Feed(_Stream, _StringTable);
		_StringTable.f_FeedString(_Stream, mp_Data);
		mint Len = mp_Children.m_Tree.f_GetLen();
		fg_FeedLenToStream(_Stream, Len);

		for (auto iChild = mp_Children.f_GetIterator()
			;iChild
			;++iChild)
		{
			(*iChild).f_Feed(_Stream, _StringTable);
		}
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStream, typename tf_CIndex>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_Consume(tf_CStream &_Stream, TCRegistryStringTable<t_CStr, tf_CIndex> const &_StringTable)
	{
		mp_Key.f_Consume(_Stream, _StringTable);
		_StringTable.f_ConsumeString(_Stream, mp_Data);
		uint64 nItems;
		fg_ConsumeLenFromStream(_Stream, nItems);
		fg_CheckLengthLimit(_Stream, nItems);
		while (nItems)
		{
			TCRegistry *pChild = DMibNew TCRegistry(this);
			auto Cleanup = g_OnScopeExit > [&]
				{
					delete pChild;
				}
			;
			pChild->f_Consume(_Stream, _StringTable);
			Cleanup.f_Clear();
			mp_Children.f_Insert(pChild);
			--nItems;
		}
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStream>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_FeedWithStringTable(tf_CStream &_Stream) const
	{
		NStream::TCBinaryStreamNull<> NullStream;
		TCRegistryStringTable<t_CStr, uint32> StringTable;
		// Build string table;
		f_Feed(NullStream, StringTable);

		// Save string table
		_Stream << StringTable;

		// Feed for real
		f_Feed(_Stream, StringTable);
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	template <typename tf_CStream>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_FeedWithStringTable(tf_CStream &_Stream)
	{
		TCRegistryStringTable<t_CStr, uint32> StringTable;
		_Stream >> StringTable;
		f_Consume(_Stream, StringTable);
	}
}
