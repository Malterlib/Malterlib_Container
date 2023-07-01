// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <typename t_CNode, typename t_CKey>
	struct TCMapNodeCompare_Default;

	template <typename t_CNode, typename t_CCompare>
	struct TCMapNodeCompare_Custom;

	template <typename t_CMap, bool t_bReverse, bool t_bConst, bool t_bBidirectional>
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
	struct TCMapNode : private CMapNodeBase
	{
		using CKey = t_CKey;
		using CValue = t_CValue;

		inline_small static mint fs_GetOffset();

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		inline_small const t_CKey &f_Key() const;
		inline_small t_CValue &f_Value() &;
		inline_small t_CValue &&f_Value() &&;
		inline_small const t_CValue &f_Value() const &;

		constexpr static bool mc_bHasValue = true;

	private:
		template <typename t_CNode2, typename t_CKey2>
		friend struct NPrivate::TCMapNodeCompare_Default;

		template <typename t_CNode2, typename t_CCompare2>
		friend struct NPrivate::TCMapNodeCompare_Custom;

		template <typename t_CMap2, bool t_bReverse2, bool t_bConst2, bool t_bBidirectional2>
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

		TCMapNode() = default;

		template <typename tf_CKey, typename... tfp_CArg>
		explicit inline_small TCMapNode(tf_CKey &&_Key, tfp_CArg && ... p_Args);

		DMibNoUniqueAddress t_CKey mp_Key;
		DMibNoUniqueAddress t_CValue mp_Value;
	};

	template <typename t_CNode, typename t_CAllocator>
	struct TCMapNodeHandle
	{
		using CNode = t_CNode;
		using CKey = typename t_CNode::CKey;
		using CValue = typename t_CNode::CValue;
		using CAllocator = t_CAllocator;

		TCMapNodeHandle() = default;

		TCMapNodeHandle(TCMapNodeHandle &&_Other) noexcept
			: mp_pNode(fg_Exchange(_Other.mp_pNode, nullptr))
		{
		}

		~TCMapNodeHandle()
		{
			if (mp_pNode)
				fg_DeleteObjectDefiniteType(mp_Allocator, mp_pNode);
		}

		void f_Clear()
		{
			if (mp_pNode)
				fg_DeleteObjectDefiniteType(mp_Allocator, fg_Exchange(mp_pNode, nullptr));
		}

		TCMapNodeHandle &operator = (TCMapNodeHandle &&_Other) noexcept
		{
			mp_Allocator = fg_Move(_Other.mp_Allocator);
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

		t_CAllocator f_GetAllocator() const
		{
			return mp_Allocator;
		}

	private:
		template <typename t_CKey2, typename t_CValue2, typename t_CCompare2, typename t_CAllocator2>
		friend struct TCMap;

		TCMapNodeHandle(t_CNode *_pNode, t_CAllocator const &_Allocator)
			: mp_pNode(_pNode)
			, mp_Allocator(_Allocator)
		{
		}

		DMibNoUniqueAddress t_CNode *mp_pNode = nullptr;
		DMibNoUniqueAddress t_CAllocator mp_Allocator;
	};
}
