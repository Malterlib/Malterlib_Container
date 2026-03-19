// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <typename t_CNode, typename t_CKey>
	struct TCMapNodeCompare_Default;

	template
	<
		typename t_CNode
		, typename t_CCompare
#if defined(DCompiler_MSVC) || defined(DCompiler_clang_cl)
		, bool t_bIsEmpty = NTraits::cIsEmpty<t_CCompare>
#endif
	>
	struct TCMapNodeCompare_Custom;

	template <typename t_CMap, EMapIteratorAccess t_Access, EMapIteratorFlags t_Flags>
	struct TCMapIterator;

	template <typename t_CDestination, bool t_bIsRef>
	struct TCMapCopy;

	template <typename t_CNode>
	struct TCMapMapper;

	template <typename t_CMap>
	struct TCMapConditionalMapper;
}

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	struct TCMap;

	template <typename t_CNode, typename t_CAllocator>
	struct TCMapNodeHandle;

	struct CMapNodeBase
	{
		NIntrusive::TCAVLLink<> m_Link;
	};

	template <typename t_CKey, typename t_CValue>
	struct TCDestructiveMapNode;

	template <typename t_CKey, typename t_CValue>
	struct TCMapNode : private CMapNodeBase
	{
		using CKey = t_CKey;
		using CValue = t_CValue;

		inline_small static umint fs_GetOffset();

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		constexpr inline_small const t_CKey &f_Key() const & noexcept;
		constexpr inline_small t_CValue &f_Value() & noexcept;
		constexpr inline_small t_CValue &&f_Value() && noexcept;
		constexpr inline_small const t_CValue &f_Value() const & noexcept;

		constexpr static bool mc_bHasValue = true;

		template <umint tf_iValue>
		constexpr decltype(auto) get() const & noexcept;
		template <umint tf_iValue>
		constexpr decltype(auto) get() & noexcept;
		template <umint tf_iValue>
		constexpr decltype(auto) get() && noexcept;

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

		template <typename t_CKey2, typename t_CValue2, typename t_CCompare2, typename t_CAllocator2>
		friend struct TCMap;

		template <typename t_CNode2, typename t_CAllocator2>
		friend struct TCMapNodeHandle;

		template <auto t_pLinkMember2, typename t_CCompare2, typename t_CAllocator2, typename t_COverrideNodeType2>
		friend class NIntrusive::TCAVLTreeAggregate;

		template <typename t_CKey2, typename t_CValue2>
		friend struct TCDestructiveMapNode;

		TCMapNode() = default;

		template <typename tf_CKey, typename... tfp_CArg>
		explicit inline_small TCMapNode(tf_CKey &&_Key, tfp_CArg && ... p_Args);

		DMibNoUniqueAddress t_CKey mp_Key;
		DMibNoUniqueAddress t_CValue mp_Value;
	};

	template <typename t_CKey, typename t_CValue>
	struct TCDestructiveMapNode : public TCMapNode<t_CKey, t_CValue>
	{
		using CBase = TCMapNode<t_CKey, t_CValue>;
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
	};

	template <typename t_CNode, typename t_CAllocator>
	struct TCMapNodeHandle
	{
		using CNode = t_CNode;
		using CKey = typename t_CNode::CKey;
		using CValue = typename t_CNode::CValue;
		using CAllocator = t_CAllocator;
		using CAllocatorStore = TCConditional
			<
				!NTraits::cIsEmpty<t_CAllocator> && (!NTraits::cIsCopyConstructible<t_CAllocator> || t_CAllocator::mc_CanBeStatic)
				, t_CAllocator *
				, t_CAllocator
			>
		;

		TCMapNodeHandle() = default;

		TCMapNodeHandle(TCMapNodeHandle &&_Other) noexcept
			: mp_pNode(fg_Exchange(_Other.mp_pNode, nullptr))
			, mp_Allocator(fg_Move(_Other.mp_Allocator))
		{
			if constexpr (NTraits::cIsPointer<CAllocatorStore>)
				_Other.mp_Allocator = nullptr;
		}

		~TCMapNodeHandle()
		{
			if (mp_pNode)
				fg_DeleteObjectDefiniteType(fp_Allocator(), mp_pNode);
		}

		void f_Clear()
		{
			if (mp_pNode)
				fg_DeleteObjectDefiniteType(fp_Allocator(), fg_Exchange(mp_pNode, nullptr));
		}

		TCMapNodeHandle &operator = (TCMapNodeHandle &&_Other) noexcept
		{
			f_Clear();

			mp_Allocator = fg_Move(_Other.mp_Allocator);
			if constexpr (NTraits::cIsPointer<CAllocatorStore>)
				_Other.mp_Allocator = nullptr;

			mp_pNode = fg_Exchange(_Other.mp_pNode, nullptr);

			return *this;
		}

		[[nodiscard]] bool f_IsEmpty() const noexcept
		{
			return !mp_pNode;
		}

		[[nodiscard]] explicit operator bool () const noexcept
		{
			return !!mp_pNode;
		}

		inline_small CKey &f_Key() const noexcept
		{
			DMibFastCheck(mp_pNode);
			return mp_pNode->mp_Key;
		}

		inline_small CValue &f_Value() const noexcept
		{
			DMibFastCheck(mp_pNode);
			if constexpr (t_CNode::mc_bHasValue)
				return mp_pNode->mp_Value;
			else
				return mp_pNode->mp_Key;
		}

		inline_small CValue &operator * () const noexcept
		{
			return f_Value();
		}

		inline_small CValue *operator -> () const noexcept
		{
			return &f_Value();
		}

		t_CAllocator &f_GetAllocator() const
		{
			return fp_Allocator();
		}

	private:
		t_CAllocator &fp_Allocator() const
		{
			if constexpr (NTraits::cIsPointer<CAllocatorStore>)
				return *mp_Allocator;
			else
				return (t_CAllocator &)mp_Allocator;
		}

		template <typename t_CKey2, typename t_CValue2, typename t_CCompare2, typename t_CAllocator2>
		friend struct TCMap;

		template <typename t_CMap2, EMapIteratorAccess t_Access2, EMapIteratorFlags t_Flags2>
		friend struct NPrivate::TCMapIterator;

		TCMapNodeHandle(t_CNode *_pNode, t_CAllocator &_Allocator)
			: mp_pNode(_pNode)
			, mp_Allocator
			(
				[&]
				{
					if constexpr (NTraits::cIsPointer<CAllocatorStore>)
						return &_Allocator;
					else
						return _Allocator;
				}
				()
			)
		{
		}

		t_CNode *mp_pNode = nullptr;
		DMibNoUniqueAddress CAllocatorStore mp_Allocator;
	};
}

namespace std
{
	template <typename t_CKey, typename t_CValue>
	struct tuple_size<NMib::NContainer::TCMapNode<t_CKey, t_CValue>>
		: public integral_constant<size_t, 2>
	{
	};

	template <typename t_CKey, typename t_CValue>
	struct tuple_element<0, NMib::NContainer::TCMapNode<t_CKey, t_CValue>>
	{
		using type = const t_CKey;
	};

	template <typename t_CKey, typename t_CValue>
	struct tuple_element<1, NMib::NContainer::TCMapNode<t_CKey, t_CValue>>
	{
		using type = t_CValue;
	};
}
