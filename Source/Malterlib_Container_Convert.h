// Copyright © 2019 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Core/Core>

namespace NMib::NContainer
{
	template <typename tf_CToContainer, typename tf_CFromContainer>
	tf_CToContainer fg_ConvertContainer(tf_CFromContainer &&_Container);
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif

#include "Malterlib_Container_Convert.hpp"
