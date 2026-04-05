// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector)
	{
		umint PrevLen = f_GetLen();
		umint AddLen = _Vector.f_GetLen();
		umint NewLen = PrevLen + AddLen;
		t_CData *pArray = fp_MakeRoom(NewLen);
		if (NewLen < PrevLen)
			DMibErrorListBoundCheck("Vector length would have overflowed");
		tf_CData *pSrcArray = _Vector.f_GetArray();

		NPrivate::fg_MoveArray(pArray + PrevLen, pSrcArray, AddLen);
		if (mp_StaticData.m_pData)
			mp_StaticData.m_pData->m_Length += AddLen;

		return pArray + PrevLen;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_never t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::fp_Insert(t_CData &&_Data)
	{
		umint PrevLen = f_GetLen();
		DMibFastCheck(PrevLen < TCLimitsInt<umint>::mc_Max);
		t_CData *pArray = fp_MakeRoom(PrevLen + 1) + PrevLen;
		new((void *)(pArray)) t_CData(fg_Move(_Data));
		++mp_StaticData.m_pData->m_Length;
		return *pArray;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(t_CData &&_Data)
	{
		umint PrevLen = f_GetLen();
		auto pData = mp_StaticData.m_pData;
		if (fsp_CanGrow(PrevLen + 1, pData))
		{
			DMibFastCheck(pData);
			t_CData *pArray = pData->f_GetData() + PrevLen;
			new((void *)(pArray)) t_CData(fg_Move(_Data));
			++pData->m_Length;
			return *pArray;
		}
		return fp_Insert(fg_Move(_Data));
	}
}
