// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	struct CMapSet
	{
		constexpr auto operator <=> (CMapSet const &_Other) const noexcept = default;
	};

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	struct TCSet;

	template <typename t_CKey>
	struct TCMapNode<t_CKey, CMapSet> : private CMapNodeBase
	{
		using CKey = t_CKey;
		using CValue = t_CKey;

		inline_small static umint fs_GetOffset();

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		inline_small const t_CKey &f_Key() const &;
		inline_small const t_CKey &f_Value() const &;

		template <umint tf_iValue>
		constexpr decltype(auto) get() const & noexcept;

		constexpr static bool mc_bHasValue = false;

	private:
		template <typename t_CNode2, typename t_CKey2>
		friend struct NPrivate::TCMapNodeCompare_Default;

		template
		<
			typename t_CNode2
			, typename t_CCompare2
#if defined(DCompiler_MSVC) || defined(DCompiler_clang_cl)
			, bool t_bIsEmpty
#endif
		>
		friend struct NPrivate::TCMapNodeCompare_Custom;

		template <typename t_CMap2, EMapIteratorAccess t_Access2, EMapIteratorFlags t_Flags2>
		friend struct NPrivate::TCMapIterator;

		template <typename t_CDestination2, bool t_bIsRef2>
		friend struct NPrivate::TCMapCopy;

		template <typename t_CNode>
		friend struct NPrivate::TCMapMapper;

		template <typename t_CMap>
		friend struct NPrivate::TCMapConditionalMapper;

		template <typename t_CNode2, typename t_CAllocator2>
		friend struct TCMapNodeHandle;

		template <typename t_CKey2, typename t_CValue2, typename t_CCompare2, typename t_CAllocator2>
		friend struct TCMap;

		template <typename t_CKey2, typename t_CCompare2, typename t_CAllocator2>
		friend struct TCSet;

		template <auto t_pLinkMember2, typename t_CCompare2, typename t_CAllocator2, typename t_COverrideNodeType2>
		friend class NIntrusive::TCAVLTreeAggregate;

		template <typename t_CKey2, typename t_CValue2>
		friend struct TCDestructiveMapNode;

		inline_small TCMapNode() = default;

		template <typename tf_CKey>
		explicit inline_small TCMapNode(tf_CKey &&_Key);
		template <typename tf_CKey, typename tf_CArg0>
		explicit inline_small TCMapNode(tf_CKey &&_Key, tf_CArg0 &&_Arg0);

		DMibNoUniqueAddress t_CKey mp_Key;
	};

	template <typename t_CKey>
	struct TCDestructiveMapNode<t_CKey, CMapSet> : public TCMapNode<t_CKey, CMapSet>
	{
		using CBase = TCMapNode<t_CKey, CMapSet>;
		using CBase::CBase;

		using CBase::get;
		using CBase::f_Key;
		using CBase::f_Value;
		using CBase::f_Feed;
		using CBase::f_Consume;

		constexpr inline_small t_CKey &f_Key() & noexcept
		{
			return this->mp_Key;
		}

		constexpr inline_small t_CKey &&f_Key() && noexcept
		{
			return fg_Move(this->mp_Key);
		}

		constexpr inline_small t_CKey &f_Value() & noexcept
		{
			return this->mp_Key;
		}

		constexpr inline_small t_CKey &&f_Value() && noexcept
		{
			return fg_Move(this->mp_Key);
		}
	};
}

namespace NMib::NContainer::NPrivate
{
	template <typename t_CType>
	struct TCIsSet
	{
		static constexpr bool mc_bValue = false;
	};

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	struct TCIsSet<TCSet<t_CKey, t_CCompare, t_CAllocator>>
	{
		static constexpr bool mc_bValue = true;
	};

	template <typename t_CKey>
	struct TCMapUserData<t_CKey, NContainer::CMapSet>
	{
		using CType = t_CKey const;
	};
}

namespace std
{
	template <typename t_CKey>
	struct tuple_size<NMib::NContainer::TCMapNode<t_CKey, NMib::NContainer::CMapSet>>
		: public integral_constant<size_t, 1>
	{
	};

	template <typename t_CKey>
	struct tuple_element<0, NMib::NContainer::TCMapNode<t_CKey, NMib::NContainer::CMapSet>>
	{
		using type = const t_CKey;
	};
}
