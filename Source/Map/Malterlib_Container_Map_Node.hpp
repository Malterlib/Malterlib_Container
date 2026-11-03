// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue>
	template <typename tf_CKey, typename ...tfp_CArg>
	inline_small TCMapNode<t_CKey, t_CValue>::TCMapNode(tf_CKey &&_Key, tfp_CArg && ...p_Args)
		: mp_Key(fg_Forward<tf_CKey>(_Key))
		, mp_Value(fg_Forward<tfp_CArg>(p_Args)...)
	{
	}

	template <typename t_CKey, typename t_CValue>
	inline_small umint TCMapNode<t_CKey, t_CValue>::fs_GetOffset()
	{
		return DMibPOffsetOf(TCMapNode, mp_Value);
	}

	template <typename t_CKey, typename t_CValue>
	template <typename tf_CStream>
	void TCMapNode<t_CKey, t_CValue>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << mp_Key;
		_Stream << mp_Value;
	}

	template <typename t_CKey, typename t_CValue>
	template <typename tf_CStream>
	void TCMapNode<t_CKey, t_CValue>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> mp_Key;
		_Stream >> mp_Value;
	}

	template <typename t_CKey, typename t_CValue>
	constexpr inline_small auto TCMapNode<t_CKey, t_CValue>::f_Key() const & noexcept -> t_CKey const &
	{
		return mp_Key;
	}

	template <typename t_CKey, typename t_CValue>
	constexpr inline_small auto TCMapNode<t_CKey, t_CValue>::f_Value() & noexcept -> t_CValue &
	{
		return mp_Value;
	}

	template <typename t_CKey, typename t_CValue>
	constexpr inline_small auto TCMapNode<t_CKey, t_CValue>::f_Value() && noexcept -> t_CValue &&
	{
		return fg_Move(mp_Value);
	}

	template <typename t_CKey, typename t_CValue>
	constexpr inline_small auto TCMapNode<t_CKey, t_CValue>::f_Value() const & noexcept -> t_CValue const &
	{
		return mp_Value;
	}

	template <typename t_CKey, typename t_CValue>
	template <umint tf_iValue>
	constexpr decltype(auto) TCMapNode<t_CKey, t_CValue>::get() const & noexcept
	{
		if constexpr (tf_iValue == 0)
			return f_Key();
		else
			return f_Value();
	}

	template <typename t_CKey, typename t_CValue>
	template <umint tf_iValue>
	constexpr decltype(auto) TCMapNode<t_CKey, t_CValue>::get() & noexcept
	{
		if constexpr (tf_iValue == 0)
			return f_Key();
		else
			return f_Value();
	}

	template <typename t_CKey, typename t_CValue>
	template <umint tf_iValue>
	constexpr decltype(auto) TCMapNode<t_CKey, t_CValue>::get() && noexcept
	{
		if constexpr (tf_iValue == 0)
			return f_Key();
		else
			return fg_Move(*this).f_Value();
	}

	template <typename t_CKey, typename t_CValue>
	constexpr inline_small t_CKey &TCDestructiveMapNode<t_CKey, t_CValue>::f_Key() & noexcept
	{
		return this->mp_Key;
	}

	template <typename t_CKey, typename t_CValue>
	constexpr inline_small t_CKey &&TCDestructiveMapNode<t_CKey, t_CValue>::f_Key() && noexcept
	{
		return fg_Move(this->mp_Key);
	}

	template <typename t_CNode, typename t_CAllocator>
	TCMapNodeHandle<t_CNode, t_CAllocator>::TCMapNodeHandle(TCMapNodeHandle &&_Other) noexcept
		: mp_pNode(fg_Exchange(_Other.mp_pNode, nullptr))
		, mp_Allocator(fg_Move(_Other.mp_Allocator))
	{
		if constexpr (NTraits::cIsPointer<CAllocatorStore>)
			_Other.mp_Allocator = nullptr;
	}

	template <typename t_CNode, typename t_CAllocator>
	TCMapNodeHandle<t_CNode, t_CAllocator>::~TCMapNodeHandle()
	{
		if (mp_pNode)
			fg_DeleteObjectDefiniteType(fp_Allocator(), mp_pNode);
	}

	template <typename t_CNode, typename t_CAllocator>
	void TCMapNodeHandle<t_CNode, t_CAllocator>::f_Clear()
	{
		if (mp_pNode)
			fg_DeleteObjectDefiniteType(fp_Allocator(), fg_Exchange(mp_pNode, nullptr));
	}

	template <typename t_CNode, typename t_CAllocator>
	auto TCMapNodeHandle<t_CNode, t_CAllocator>::operator = (TCMapNodeHandle &&_Other) noexcept -> TCMapNodeHandle &
	{
		f_Clear();

		mp_Allocator = fg_Move(_Other.mp_Allocator);
		if constexpr (NTraits::cIsPointer<CAllocatorStore>)
			_Other.mp_Allocator = nullptr;

		mp_pNode = fg_Exchange(_Other.mp_pNode, nullptr);

		return *this;
	}

	template <typename t_CNode, typename t_CAllocator>
	bool TCMapNodeHandle<t_CNode, t_CAllocator>::f_IsEmpty() const noexcept
	{
		return !mp_pNode;
	}

	template <typename t_CNode, typename t_CAllocator>
	TCMapNodeHandle<t_CNode, t_CAllocator>::operator bool () const noexcept
	{
		return !!mp_pNode;
	}

	template <typename t_CNode, typename t_CAllocator>
	inline_small auto TCMapNodeHandle<t_CNode, t_CAllocator>::f_Key() const noexcept -> CKey &
	{
		DMibFastCheck(mp_pNode);
		return mp_pNode->mp_Key;
	}

	template <typename t_CNode, typename t_CAllocator>
	inline_small auto TCMapNodeHandle<t_CNode, t_CAllocator>::f_Value() const noexcept -> CValue &
	{
		DMibFastCheck(mp_pNode);
		if constexpr (t_CNode::mc_bHasValue)
			return mp_pNode->mp_Value;
		else
			return mp_pNode->mp_Key;
	}

	template <typename t_CNode, typename t_CAllocator>
	inline_small auto TCMapNodeHandle<t_CNode, t_CAllocator>::operator * () const noexcept -> CValue &
	{
		return f_Value();
	}

	template <typename t_CNode, typename t_CAllocator>
	inline_small auto TCMapNodeHandle<t_CNode, t_CAllocator>::operator -> () const noexcept -> CValue *
	{
		return &f_Value();
	}

	template <typename t_CNode, typename t_CAllocator>
	t_CAllocator &TCMapNodeHandle<t_CNode, t_CAllocator>::f_GetAllocator() const
	{
		return fp_Allocator();
	}

	template <typename t_CNode, typename t_CAllocator>
	t_CAllocator &TCMapNodeHandle<t_CNode, t_CAllocator>::fp_Allocator() const
	{
		if constexpr (NTraits::cIsPointer<CAllocatorStore>)
			return *mp_Allocator;
		else
			return (t_CAllocator &)mp_Allocator;
	}

	template <typename t_CNode, typename t_CAllocator>
	TCMapNodeHandle<t_CNode, t_CAllocator>::TCMapNodeHandle(t_CNode *_pNode, t_CAllocator &_Allocator)
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
}
