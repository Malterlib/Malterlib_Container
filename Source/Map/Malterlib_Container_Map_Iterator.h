// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	struct TCMapIterator
	{
		using CMap = t_CMap;

	private:
		friend t_CMap;

		using CNodePublic = typename t_CMap::CNode;
		using CNodeDestructive = typename t_CMap::CNodeDestructive;
		using CKey = typename t_CMap::CKey;
		using CValue = typename CNodePublic::CValue;
		using CNodeHandle = typename t_CMap::CNodeHandle;

		using CUserData = TCConditional
			<
				t_Access == EMapIteratorAccess_Value
				, typename t_CMap::CUserData
				, TCConditional
				<
					t_Access == EMapIteratorAccess_Key
					, TCConditional<!!(t_Flags & EMapIteratorFlags_Destructive), CKey, CKey const>
					, TCConditional<!!(t_Flags & EMapIteratorFlags_Destructive), CNodeDestructive, CNodePublic>
				>
			>
		;
		using CAVLTree = typename t_CMap::CAVLTree;
		using CMapQualified = NTraits::TCSetConst<t_CMap, !!(t_Flags & EMapIteratorFlags_Const)>;
		using CUserDataQualified = TCConditional<!!(t_Flags & EMapIteratorFlags_Const) && !(t_Flags & EMapIteratorFlags_Destructive), NTraits::TCAddConst<CUserData>, CUserData>;
		using CNodeCompare = typename t_CMap::CNodeCompare;

	public:
		TCMapIterator()
			requires (!(t_Flags & EMapIteratorFlags_Destructive))
		;

		TCMapIterator(CMapQualified &_Map)
			requires (!(t_Flags & EMapIteratorFlags_Destructive))
		;

		TCMapIterator(t_CMap &&_Map)
			requires (!!(t_Flags & EMapIteratorFlags_Destructive))
		;

		template <typename tf_CKey>
		TCMapIterator(CMapQualified &_Map, const tf_CKey &_Key)
			requires (!(t_Flags & EMapIteratorFlags_Reverse) && !(t_Flags & EMapIteratorFlags_Destructive))
		;

		TCMapIterator(TCMapIterator const &_Other)
			requires (!(t_Flags & EMapIteratorFlags_Destructive))
		= default;
		TCMapIterator &operator = (TCMapIterator const &_Other)
			requires (!(t_Flags & EMapIteratorFlags_Destructive))
		= default;

		TCMapIterator(TCMapIterator &&_Other)
			requires (!!(t_Flags & EMapIteratorFlags_Destructive))
			: mp_Iter(_Other.mp_Iter)
			, mp_pMap(fg_Exchange(_Other.mp_pMap, nullptr))
		{
		}

		TCMapIterator(TCMapIterator const &_Other)
			requires (!!(t_Flags & EMapIteratorFlags_Destructive))
		= delete;
		TCMapIterator &operator = (TCMapIterator const &_Other)
			requires (!!(t_Flags & EMapIteratorFlags_Destructive))
		= delete;

		TCMapIterator &operator = (TCMapIterator &&_Other)
			requires (!!(t_Flags & EMapIteratorFlags_Destructive))
		{
			if (mp_pMap)
				mp_pMap->f_Clear();
			mp_Iter = _Other.mp_Iter;
			mp_pMap = fg_Exchange(_Other.mp_pMap, nullptr);
			return *this;
		}

		~TCMapIterator()
		{
			if constexpr (t_Flags & EMapIteratorFlags_Destructive)
			{
				if (mp_pMap)
					mp_pMap->f_Clear();
			}
		}

		TCMapIterator f_GetIterator() const
			requires (!(t_Flags & EMapIteratorFlags_Destructive))
		{
			TCMapIterator Iterator;
			Iterator.mp_Iter = mp_Iter;
			Iterator.mp_pMap = mp_pMap;
			return Iterator;
		}

		TCMapIterator &operator = (CMapQualified &_Map)
			requires (!(t_Flags & EMapIteratorFlags_Destructive))
		;

		inline_medium umint f_GetLen() const;
		inline_medium void f_Next();
		inline_medium void f_Prev()
			requires (!!(t_Flags & EMapIteratorFlags_Bidirectional))
		;

		void f_Remove()
			requires (!(t_Flags & EMapIteratorFlags_Destructive))
		;

		CNodeHandle f_ExtractNode()
			requires (!!(t_Flags & EMapIteratorFlags_Destructive) && !!(t_Flags & EMapIteratorFlags_Bidirectional))
		;

		CKey const &f_GetKey() const
			requires (t_Access == EMapIteratorAccess_Value)
		;

		constexpr inline_always explicit operator bool () const;

		inline_small CUserDataQualified *f_GetCurrent() const;
		inline_small operator CUserDataQualified *() const;
		inline_small CUserDataQualified * operator ->() const;

		inline_small void operator ++();
		inline_small void operator --()
			requires (!!(t_Flags & EMapIteratorFlags_Bidirectional))
		;

	private:
		typename CAVLTree::CIterator mp_Iter;
		CMapQualified *mp_pMap;
	};
}
