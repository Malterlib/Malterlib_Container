// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename tf_CToContainer, typename tf_CFromContainer>
	tf_CToContainer fg_ConvertContainer(tf_CFromContainer &&_Container)
	{
		tf_CToContainer Container;

		if constexpr (NTraits::cIsRValueReference<tf_CFromContainer &&>)
		{
			for (auto &Value : _Container)
				Container.f_Insert(fg_Move(Value));
		}
		else
		{
			for (auto &Value : _Container)
				Container.f_Insert(Value);
		}

		return Container;
	}
}
