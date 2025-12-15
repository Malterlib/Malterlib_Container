// Copyright © 2025 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

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
