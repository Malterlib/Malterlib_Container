// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey>
	template <typename tf_CKey>
	inline_small TCMapNode<t_CKey, CMapSet>::TCMapNode(tf_CKey &&_Key)
		: mp_Key(fg_Forward<tf_CKey>(_Key))
	{
	}

	template <typename t_CKey>
	template <typename tf_CKey, typename tf_CArg0>
	inline_small TCMapNode<t_CKey, CMapSet>::TCMapNode(tf_CKey &&_Key, tf_CArg0 &&_Arg0)
		: mp_Key(fg_Forward<tf_CKey>(_Key))
	{
	}

	template <typename t_CKey>
	inline_small umint TCMapNode<t_CKey, CMapSet>::fs_GetOffset()
	{
		return DMibPOffsetOf(TCMapNode, mp_Key);
	}

	template <typename t_CKey>
	template <typename tf_CStream>
	void TCMapNode<t_CKey, CMapSet>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << mp_Key;
	}

	template <typename t_CKey>
	template <typename tf_CStream>
	void TCMapNode<t_CKey, CMapSet>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> mp_Key;
	}

	template <typename t_CKey>
	inline_small const t_CKey &TCMapNode<t_CKey, CMapSet>::f_Key() const &
	{
		return mp_Key;
	}

	template <typename t_CKey>
	inline_small const t_CKey &TCMapNode<t_CKey, CMapSet>::f_Value() const &
	{
		return mp_Key;
	}

	template <typename t_CKey>
	template <umint tf_iValue>
	constexpr decltype(auto) TCMapNode<t_CKey, CMapSet>::get() const & noexcept
	{
		return f_Key();
	}
}
