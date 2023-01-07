// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "../Malterlib_Container_Shared.h"

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	struct TCIsContainer<TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>>
	{
		static constexpr bool mc_Value = true;
	};
}
