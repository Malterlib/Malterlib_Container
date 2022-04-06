// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_never t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::fp_Insert()
	{
		aint PrevLen = f_GetLen();
		t_CData *pArray = fp_MakeRoom(PrevLen + 1);
		new((void *)(pArray + PrevLen)) t_CData();
		++mp_StaticData.m_pData->m_Length;
		return pArray[PrevLen];
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert()
	{
		mint PrevLen = f_GetLen();
		auto pData = mp_StaticData.m_pData;
		if (fsp_CanGrow(PrevLen + 1, pData))
		{
			t_CData *pArray = pData->f_GetData() + PrevLen;
			new((void *)(pArray)) t_CData();
			++pData->m_Length;
			return *pArray;
		}
		return fp_Insert();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CType, typename... tfp_CParams>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::fp_Insert(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
	{
		aint PrevLen = f_GetLen();
		t_CData *pArray = fp_MakeRoom(PrevLen + 1) + PrevLen;
		_CreateParams.template f_Create<t_CData>(NMemory::TCAllocator_Placement<sizeof(t_CData)>((void *)(pArray)));
		++mp_StaticData.m_pData->m_Length;
		return *pArray;

	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CType, typename... tfp_CParams>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
	{
		mint PrevLen = f_GetLen();
		auto pData = mp_StaticData.m_pData;
		if (fsp_CanGrow(PrevLen + 1, pData))
		{
			t_CData *pArray = pData->f_GetData() + PrevLen;
			_CreateParams.template f_Create<t_CData>(NMemory::TCAllocator_Placement<sizeof(t_CData)>((void *)(pArray)));
			++pData->m_Length;
			return *pArray;
		}
		return fp_Insert(fg_Move(_CreateParams));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_AddArrayAtEnd(mint _Size)
	{
		aint PrevLen = f_GetLen();
		t_CData *pArray = fp_MakeRoom(PrevLen + _Size);
		mint nAdded = 0;

		auto Cleanup = g_OnScopeExit / [&]
			{
				if (nAdded)
					NPrivate::fg_DestroyArray(pArray + PrevLen, nAdded, nAdded);
			}
		;

		for (mint i = 0; i < _Size; ++i)
		{
			new((void *)(pArray + PrevLen + i)) t_CData();
			++nAdded;
		}

		Cleanup.f_Clear();

		if (mp_StaticData.m_pData)
			mp_StaticData.m_pData->m_Length += _Size;
		return pArray + PrevLen;
	}
}
