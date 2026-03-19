// Copyright © 2025 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	// f_Map - Transform each element
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CThis, typename tf_FTransform>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_Map(this tf_CThis &&_This, tf_FTransform &&_fTransform)
	{
		umint Len = _This.f_GetLen();
		auto pSrc = _This.f_GetArray();

		using CResult = decltype(NPrivate::fg_InvokeVectorIteratorFunctor(_fTransform, fg_ForwardAs<tf_CThis>(pSrc[0]), umint(0), fg_Forward<tf_CThis>(_This)));

		TCVector<CResult, t_CAllocator, t_COptions> Ret;

		if (!Len)
			return Ret;

		Ret.fp_MakeNewRoom(Len);
		auto pDst = Ret.f_GetArray();
		auto &NewLen = Ret.mp_StaticData.m_pData->m_Length;

		for (umint i = 0; i < Len; ++i)
		{
			new((void *)(pDst + i)) CResult(NPrivate::fg_InvokeVectorIteratorFunctor(_fTransform, fg_ForwardAs<tf_CThis>(pSrc[i]), i, fg_Forward<tf_CThis>(_This)));
			++NewLen;
		}

		return Ret;
	}
}
