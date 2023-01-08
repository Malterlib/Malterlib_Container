// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	struct CMapNodeBase
	{
		NIntrusive::TCAVLLink<> m_Link;
	};

	template <typename t_CKey, typename t_CValue>
	struct TCMapNode : public CMapNodeBase
	{
		using CKey = t_CKey;
		using CValue = t_CValue;

		TCMapNode() = default;

		template <typename tf_CKey, typename... tfp_CArg>
		explicit inline_small TCMapNode(tf_CKey &&_Key, tfp_CArg && ... p_Args);

		inline_small static mint fs_GetOffset();

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		inline_small t_CValue &f_Value();
		inline_small const t_CValue &f_Value() const;

		constexpr static bool mc_bHasValue = true;

		DMibNoUniqueAddress t_CKey m_Key;
		DMibNoUniqueAddress t_CValue m_Value;
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
			return mp_pNode->m_Key;
		}

		inline_small CValue &f_Value() const noexcept
		{
			DMibFastCheck(mp_pNode);
			if constexpr (t_CNode::mc_bHasValue)
				return mp_pNode->m_Value;
			else
				return mp_pNode->m_Key;
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
