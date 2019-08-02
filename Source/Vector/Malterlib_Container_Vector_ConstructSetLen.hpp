// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	TCVector<t_CData, t_CAllocator, t_COptions>::TCVector(mint _nItems)
	{
		mint NewLen = _nItems;

		if (NewLen == 0)
			return;

		mint AllocSize = NewLen;
		CVectorData *pNewData = fp_AllocData(AllocSize);
		t_CData *pNewArray = pNewData->f_GetData();

		mint nConstructed = 0;

		auto Cleanup = g_OnScopeExit > [&]
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
