// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey>
	constexpr inline_small t_CKey &TCDestructiveMapNode<t_CKey, CMapSet>::f_Key() & noexcept
	{
		return this->mp_Key;
	}

	template <typename t_CKey>
	constexpr inline_small t_CKey &&TCDestructiveMapNode<t_CKey, CMapSet>::f_Key() && noexcept
	{
		return fg_Move(this->mp_Key);
	}

	template <typename t_CKey>
	constexpr inline_small t_CKey &TCDestructiveMapNode<t_CKey, CMapSet>::f_Value() & noexcept
	{
		return this->mp_Key;
	}

	template <typename t_CKey>
	constexpr inline_small t_CKey &&TCDestructiveMapNode<t_CKey, CMapSet>::f_Value() && noexcept
	{
		return fg_Move(this->mp_Key);
	}
}
