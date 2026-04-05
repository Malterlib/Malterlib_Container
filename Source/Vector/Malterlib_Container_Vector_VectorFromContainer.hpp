// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename tf_CContainer>
	auto fg_VectorFromContainer(tf_CContainer &&_Container)
	{
		using CValueType = NTraits::TCRemoveReferenceAndQualifiers<decltype(*begin(_Container))>;
		return TCVector<CValueType>::fs_FromContainer(fg_Forward<tf_CContainer>(_Container));
	}
}
