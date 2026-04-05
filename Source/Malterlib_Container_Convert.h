// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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
