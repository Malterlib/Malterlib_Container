// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

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
