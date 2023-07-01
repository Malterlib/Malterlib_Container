// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <typename t_CMap, EMapIteratorAccess t_Access, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
	struct TCMapIterator
	{
		using CMap = t_CMap;

	private:
		friend t_CMap;

		using CNode = typename t_CMap::CNode;
		using CKey = typename t_CMap::CKey;
		using CUserData = typename TCChooseType
			<
				t_Access == EMapIteratorAccess_Value
				, typename t_CMap::CUserData
				, typename TCChooseType
				<
					t_Access == EMapIteratorAccess_Key
					, CKey const
					, CNode
				>::CType
			>::CType
		;
		using CAVLTree = typename t_CMap::CAVLTree;
		using CMapQualified = typename NTraits::TCSetConst<t_CMap, t_bConst>::CType;
		using CUserDataQualified = typename TCChooseType<t_bConst, typename NTraits::TCAddConst<CUserData>::CType, CUserData>::CType;
		using CNodeCompare = typename t_CMap::CNodeCompare;

	public:
		TCMapIterator();
		TCMapIterator(CMapQualified &_Map);
		template <typename tf_CKey>
		TCMapIterator(CMapQualified &_Map, const tf_CKey &_Key)
			requires (!t_bReverse) // Not supported for reverse iterators
		;

		TCMapIterator &operator = (CMapQualified &_Map);

		inline_medium mint f_GetLen() const;
		inline_medium void f_Next();
		inline_medium void f_Prev()
			requires (t_bBidirectional) // Only available when bidirectional
		;
		void f_Remove();

		CKey const &f_GetKey() const
			requires (t_Access == EMapIteratorAccess_Value)
		;

		explicit operator bool () const;

		inline_small CUserDataQualified *f_GetCurrent() const;
		inline_small operator CUserDataQualified *() const;
		inline_small CUserDataQualified * operator ->() const;

		inline_small void operator ++();
		inline_small void operator --()
			requires (t_bBidirectional) // Only available when bidirectional
		;
	
	private:
		typename CAVLTree::CIterator mp_Iter;
		CMapQualified *mp_pMap;
	};
}
