// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small bool TCPackedMapResult<t_CKey, t_CValue>::f_WasCreated() const noexcept
	{
		return m_bInserted;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small t_CKey const &TCPackedMapResult<t_CKey, t_CValue>::f_Key() const noexcept
	{
		return m_Key;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small t_CValue &TCPackedMapResult<t_CKey, t_CValue>::f_Value() noexcept
	{
		return m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small t_CValue const &TCPackedMapResult<t_CKey, t_CValue>::f_Value() const noexcept
	{
		return m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small TCPackedMapResult<t_CKey, t_CValue>::operator t_CValue &() noexcept
	{
		return m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small t_CValue &TCPackedMapResult<t_CKey, t_CValue>::f_GetResult() noexcept
	{
		return m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small t_CValue &TCPackedMapResult<t_CKey, t_CValue>::operator * () noexcept
	{
		return m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small t_CKey const &TCPackedMapRef<t_CKey, t_CValue>::f_Key() const noexcept
	{
		return m_Key;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small t_CValue &TCPackedMapRef<t_CKey, t_CValue>::f_Value() noexcept
	{
		return m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small t_CValue const &TCPackedMapRef<t_CKey, t_CValue>::f_Value() const noexcept
	{
		return m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small TCPackedMapRef<t_CKey, t_CValue>::operator t_CValue &() noexcept
	{
		return m_Value;
	}

	template <typename t_CKey, typename t_CValue>
	mark_nodebug constexpr inline_small TCPackedMapRef<t_CKey, t_CValue>::operator t_CValue const &() const noexcept
	{
		return m_Value;
	}
}
