// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey>
	template <typename tf_CKey>
	inline_small TCMapNode<t_CKey, CMapSet>::TCMapNode(tf_CKey &&_Key)
		: m_Key(fg_Forward<tf_CKey>(_Key))
	{
	}

	template <typename t_CKey>
	template <typename tf_CKey, typename tf_CArg0>
	inline_small TCMapNode<t_CKey, CMapSet>::TCMapNode(tf_CKey &&_Key, tf_CArg0 &&_Arg0)
		: m_Key(fg_Forward<tf_CKey>(_Key))
	{
	}

	template <typename t_CKey>
	inline_small mint TCMapNode<t_CKey, CMapSet>::fs_GetOffset()
	{
		return DMibPOffsetOf(TCMapNode, m_Key);
	}

	template <typename t_CKey>
	template <typename tf_CStream>
	void TCMapNode<t_CKey, CMapSet>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_Key;
	}

	template <typename t_CKey>
	template <typename tf_CStream>
	void TCMapNode<t_CKey, CMapSet>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_Key;
	}

	template <typename t_CKey>
	inline_small t_CKey &TCMapNode<t_CKey, CMapSet>::f_Value()
	{
		return m_Key;
	}

	template <typename t_CKey>
	inline_small const t_CKey &TCMapNode<t_CKey, CMapSet>::f_Value() const
	{
		return m_Key;
	}
}
