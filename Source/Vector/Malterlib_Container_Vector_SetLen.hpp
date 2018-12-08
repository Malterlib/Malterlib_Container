// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	mint TCVector<t_CData, t_CAllocator, t_COptions>::f_Grow(mint _MinLen)
	{
		mint NewLen = _MinLen;
		mint OldLen = f_GetLen();
		if (fsp_NeedReallocGrow(NewLen, mp_StaticData.m_pData))
		{
			CVectorData *pNewData = fp_AllocDataGrow(NewLen);
			NewLen = (pNewData->m_AllocSize - sizeof(CVectorData)) / sizeof(t_CData);
			t_CData *pOldArray = f_GetArray();
			t_CData *pNewArray = pNewData->f_GetData();
			mint CurrentLength = OldLen;

			mint OldRemainingCells = fg_Min(NewLen, CurrentLength);

			// Construct all new datas with copy constructor from old datas
			NPrivate::fg_MoveArray(pNewArray, pOldArray, OldRemainingCells);

			DMibFastCheck(!mp_StaticData.m_pData || pNewData->m_AllocSize != mp_StaticData.m_pData->m_AllocSize);

			auto pOldData = mp_StaticData.m_pData;
			pNewData->m_Length = OldRemainingCells;
			mp_StaticData.m_pData = pNewData;

			if (pOldData)
			{
				NPrivate::fg_DestroyArray(pOldArray, CurrentLength, CurrentLength);
				fp_FreeData(pOldData);
			}
		}
		else
			NewLen = (mp_StaticData.m_pData->m_AllocSize - sizeof(CVectorData)) / sizeof(t_CData);

		if (NewLen > OldLen)
			NPrivate::fg_ConstructArray(f_GetArray() + OldLen, NewLen - OldLen, mp_StaticData.m_pData->m_Length);

		return NewLen;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_SetLen(mint _Len, bool _bTrim)
	{
		mint OldLen = f_GetLen();

		if (OldLen == _Len)
			return;

		mint NewLen = _Len;

		if (NewLen == 0)
			return f_Clear();

		if (_bTrim || fsp_NeedRealloc(NewLen, mp_StaticData.m_pData))
		{
			mint AllocSize = _bTrim ? NewLen : fsp_GetAllocSize(NewLen);
			CVectorData *pNewData = fp_AllocData(AllocSize);
			t_CData *pOldArray = f_GetArray();
			t_CData *pNewArray = pNewData->f_GetData();
			mint CurrentLength = OldLen;

			mint OldRemainingCells = fg_Min(NewLen, CurrentLength);

			// Construct all new datas with copy constructor from old datas

			mint nConstructed = 0;

			auto Cleanup = g_OnScopeExit > [&]
				{
					if (nConstructed)
						NPrivate::fg_DestroyArray(pNewArray + OldRemainingCells, nConstructed, nConstructed);
					fp_FreeData(pNewData);
				}
			;

			// Construct all new datas
			if (NewLen > OldRemainingCells)
				NPrivate::fg_ConstructArray(pNewArray + OldRemainingCells, NewLen - OldRemainingCells, nConstructed);

			NPrivate::fg_MoveArray(pNewArray, pOldArray, OldRemainingCells);

			Cleanup.f_Clear();
			pNewData->m_Length = NewLen;
			auto pOldData = mp_StaticData.m_pData;
			mp_StaticData.m_pData = pNewData;

			if (pOldData)
			{
				NPrivate::fg_DestroyArray(pOldArray, CurrentLength, CurrentLength);
				fp_FreeData(pOldData);
			}
		}
		else
		{
			t_CData *pOldArray = f_GetArray();

			// Construct new data
			if (NewLen > OldLen)
				NPrivate::fg_ConstructArray(pOldArray + OldLen, NewLen - OldLen, mp_StaticData.m_pData->m_Length);
			else if (OldLen > NewLen)
				NPrivate::fg_DestroyArray(pOldArray + NewLen, OldLen - NewLen, mp_StaticData.m_pData->m_Length);
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_SetAtLeastLen(mint _Len, mint _Grow)
	{
		if (f_GetLen() < _Len)
			f_SetLen(_Len + _Grow, false);
	}
}
