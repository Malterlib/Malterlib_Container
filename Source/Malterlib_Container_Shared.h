// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Core/Core>

namespace NMib::NContainer
{
	template <typename t_CType>
	struct TCIsContainer
	{
		static constexpr bool mc_Value = false;
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
