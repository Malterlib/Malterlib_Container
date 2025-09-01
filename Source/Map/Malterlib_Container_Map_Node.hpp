// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue>
	template <typename tf_CKey, typename... tfp_CArg>
	inline_small TCMapNode<t_CKey, t_CValue>::TCMapNode(tf_CKey &&_Key, tfp_CArg && ... p_Args)
		: mp_Key(fg_Forward<tf_CKey>(_Key))
		, mp_Value(fg_Forward<tfp_CArg>(p_Args)...)
	{
	}

	template <typename t_CKey, typename t_CValue>
	inline_small mint TCMapNode<t_CKey, t_CValue>::fs_GetOffset()
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
	constexpr inline_small auto TCMapNode<t_CKey, t_CValue>::f_Key() const noexcept -> const t_CKey &
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
	constexpr inline_small auto TCMapNode<t_CKey, t_CValue>::f_Value() const & noexcept -> const t_CValue &
	{
		return mp_Value;
	}

	template <typename t_CKey, typename t_CValue>
	template <mint tf_iValue>
	constexpr decltype(auto) TCMapNode<t_CKey, t_CValue>::get() const & noexcept
	{
		if constexpr (tf_iValue == 0)
			return f_Key();
		else
			return f_Value();
	}

	template <typename t_CKey, typename t_CValue>
	template <mint tf_iValue>
	constexpr decltype(auto) TCMapNode<t_CKey, t_CValue>::get() & noexcept
	{
		if constexpr (tf_iValue == 0)
			return f_Key();
		else
			return f_Value();
	}

	template <typename t_CKey, typename t_CValue>
	template <mint tf_iValue>
	constexpr decltype(auto) TCMapNode<t_CKey, t_CValue>::get() && noexcept
	{
		if constexpr (tf_iValue == 0)
			return f_Key();
		else
			return fg_Move(*this).f_Value();
	}
}
