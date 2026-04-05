// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector(umint _nItems)
	{
		umint NewLen = _nItems;

		if (NewLen == 0)
			return;

		umint AllocSize = NewLen;
		CVectorData *pNewData = fp_AllocData(AllocSize);
		t_CData *pNewArray = pNewData->f_GetData();

		umint nConstructed = 0;

		auto Cleanup = g_OnScopeExit / [&]
			{
				if (nConstructed)
					NPrivate::fg_DestroyArray(pNewArray, nConstructed, nConstructed);
				fp_FreeData(pNewData);
			}
		;

		NPrivate::fg_ConstructArray(pNewArray, NewLen, nConstructed);

		Cleanup.f_Clear();
		pNewData->m_Length = NewLen;
		mp_StaticData.m_pData = pNewData;
	}
}
