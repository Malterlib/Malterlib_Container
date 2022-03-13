// Copyright © 2022 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename tf_CContainer>
	auto fg_VectorFromContainer(tf_CContainer &&_Container)
	{
		using CValueType = typename NTraits::TCRemoveReferenceAndQualifiers<decltype(*begin(_Container))>::CType;
		return TCVector<CValueType>::fs_FromContainer(fg_Forward<tf_CContainer>(_Container));
	}
}
