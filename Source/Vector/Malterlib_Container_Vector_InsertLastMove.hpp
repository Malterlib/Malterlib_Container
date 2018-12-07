// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector)
	{
		mint PrevLen = f_GetLen();
		mint AddLen = _Vector.f_GetLen();
		mint NewLen = PrevLen + AddLen;
		t_CData *pArray = fp_MakeRoom(NewLen);
		tf_CData *pSrcArray = _Vector.f_GetArray();

		NPrivate::fg_MoveArray(pArray + PrevLen, pSrcArray, AddLen);
		if (mp_StaticData.m_pData)
			mp_StaticData.m_pData->m_Length += AddLen;

		return pArray + PrevLen;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_never t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::fp_Insert(t_CData &&_Data)
	{
		mint PrevLen = f_GetLen();
		t_CData *pArray = fp_MakeRoom(PrevLen + 1) + PrevLen;
		new((void *)(pArray)) t_CData(fg_Move(_Data));
		++mp_StaticData.m_pData->m_Length;
		return *pArray;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(t_CData &&_Data)
	{
		mint PrevLen = f_GetLen();
		auto pData = mp_StaticData.m_pData;
		if (fsp_CanGrow(PrevLen + 1, pData))
		{
			t_CData *pArray = pData->f_GetData() + PrevLen;
			new((void *)(pArray)) t_CData(fg_Move(_Data));
			++pData->m_Length;
			return *pArray;
		}
		return fp_Insert(fg_Move(_Data));
	}
}
