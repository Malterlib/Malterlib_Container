// Copyright © 2025 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CThis, typename tf_FPredicate>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_Filter(this tf_CThis &&_This, tf_FPredicate &&_fPredicate)
	{
		TCVector Ret;
		if (_This.f_IsEmpty())
			return Ret;

		mint iElement = 0;
		for (auto &Element : _This)
		{
			if (NPrivate::fg_InvokeVectorIteratorFunctor(_fPredicate, fg_ForwardAs<tf_CThis>(Element), iElement, fg_Forward<tf_CThis>(_This)))
				Ret.f_Insert(fg_ForwardAs<tf_CThis>(Element));
			++iElement;
		}

		return Ret;
	}
}
