// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CIndex>
	struct TCRegistryStringTable
	{
		TCRegistryStringTable();

		t_CStr const *f_GetString(t_CIndex const &_Index) const;

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		template <typename tf_CStream, typename tf_CStr>
		void f_FeedString(tf_CStream &_Stream, tf_CStr const &_String);
		template <typename tf_CStream, typename tf_CStr>
		void f_ConsumeString(tf_CStream &_Stream, tf_CStr &_String) const;

		template <typename tf_CStr>
		t_CIndex f_GetIndex(tf_CStr const &_String);

	private:
		struct CIndex
		{
			struct CCompare_Index
			{
				inline_small t_CIndex const &operator () (CIndex const &_Node) const;
			};

			template <typename tf_CStream>
			void f_Feed(tf_CStream &_Stream) const;
			template <typename tf_CStream>
			void f_Consume(tf_CStream &_Stream);

			t_CIndex m_Index;
			NIntrusive::TCAVLLink<> m_IndexLink;
		};

		NContainer::TCMap<t_CStr, CIndex> m_Strings;
		NIntrusive::TCAVLTree<&CIndex::m_IndexLink, typename CIndex::CCompare_Index> m_IndexToString;
		t_CIndex m_CurrentIndex;
	};
}

#include "Malterlib_Container_Registry_StringTable.hpp"
