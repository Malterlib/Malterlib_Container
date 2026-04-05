// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename tf_CContainer>
	auto fg_SetFromContainer(tf_CContainer &&_Container)
	{
		using CValueType = NTraits::TCRemoveReferenceAndQualifiers<decltype(*begin(_Container))>;
		return TCSet<CValueType>::fs_FromContainer(fg_Forward<tf_CContainer>(_Container));
	}
}
