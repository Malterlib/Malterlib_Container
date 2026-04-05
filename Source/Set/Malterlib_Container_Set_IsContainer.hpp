// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "../Malterlib_Container_Shared.h"

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	struct TCIsContainer<TCSet<t_CKey, t_CCompare, t_CAllocator>>
	{
		static constexpr bool mc_Value = true;
	};
}
