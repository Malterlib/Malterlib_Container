// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{	
	template <typename t_CKey, typename t_CValue>
	template <typename tf_CKey, typename... tfp_CArg>
	inline_small TCMapNode<t_CKey, t_CValue>::TCMapNode(tf_CKey &&_Key, tfp_CArg && ... p_Args)
		: m_Key(fg_Forward<tf_CKey>(_Key))
		, m_Value(fg_Forward<tfp_CArg>(p_Args)...)
	{
	}

	template <typename t_CKey, typename t_CValue>
	inline_small mint TCMapNode<t_CKey, t_CValue>::fs_GetOffset()
	{
		return DMibPOffsetOf(TCMapNode, m_Value);
	}

	template <typename t_CKey, typename t_CValue>
	template <typename tf_CStream>
	void TCMapNode<t_CKey, t_CValue>::f_Feed(tf_CStream &_Stream) const
	{
		_Stream << m_Key;
		_Stream << m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	template <typename tf_CStream>
	void TCMapNode<t_CKey, t_CValue>::f_Consume(tf_CStream &_Stream)
	{
		_Stream >> m_Key;
		_Stream >> m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	inline_small auto TCMapNode<t_CKey, t_CValue>::f_Value() -> t_CValue &
	{
		return m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	inline_small auto TCMapNode<t_CKey, t_CValue>::f_Value() const -> const t_CValue &
	{
		return m_Value;
	}
}
