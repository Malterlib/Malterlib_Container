// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	// If you send 0 as _Len the remaining part of the list will be copied
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Replace(const TCVector &_Source, const mint _StartInSource, const mint _Len, const mint _StartInList, const mint _MinLen)
	{
		mint Len;
		if (!_Len)
			Len = _Source.f_GetLen() - _StartInSource;
		else
			Len = _Len;

		DMibSafeCheck(_StartInSource + Len <= _Source.f_GetLen(), "Attempting to copy to much from source array");

		mint NewLen = fg_Max(fg_Max(_StartInList + Len, f_GetLen()), _MinLen);

		CVectorData *pNewData;
		if (NewLen != f_GetLen())
		{
			// New list
			pNewData = fp_AllocData(NewLen);

			t_CData *pOldArray = f_GetArray();
			const t_CData *pSourceArray = _Source.f_GetArray();
			t_CData *pNewArray = pNewData->f_GetData();

			mint CurrentLength = f_GetLen();
			mint OldRemainingCells = fg_Min(CurrentLength, _StartInList);
			mint MaxLen = fg_Max(_StartInList + Len, CurrentLength);

			mint nConstructed0 = 0;
			mint nConstructed1 = 0;
			mint nCopied = 0;

			auto Cleanup = [&]
				{
					if (nConstructed0)
						NPrivate::fg_DestroyArray(pNewArray + OldRemainingCells, nConstructed0, nConstructed0);
					if (nConstructed1)
						NPrivate::fg_DestroyArray(pNewArray + MaxLen, nConstructed1, nConstructed1);
					if (nCopied)
						NPrivate::fg_DestroyArray(pNewArray + _StartInList, nCopied, nCopied);
					fp_FreeData(pNewData);
				}
			;

			if (_StartInList > OldRemainingCells)
				NPrivate::fg_ConstructArray(pNewArray + OldRemainingCells, _StartInList - OldRemainingCells, nConstructed0);

			if (NewLen > MaxLen)
				NPrivate::fg_ConstructArray(pNewArray + MaxLen, NewLen - MaxLen, nConstructed1);

			// Construct all new datas with copy constructor from source datas
			NPrivate::fg_CopyArray(pNewArray + _StartInList, pSourceArray + _StartInSource, Len, nCopied);

			// Construct all new datas with copy constructor from old datas
			NPrivate::fg_MoveArray(pNewArray, pOldArray, OldRemainingCells);

			// Move the rest of the datas
			if (CurrentLength > _StartInList + Len)
				NPrivate::fg_MoveArray(pNewArray + _StartInList + Len, pOldArray + _StartInList + Len, (CurrentLength - (_StartInList + Len)));

			Cleanup.f_Clear();

			auto pOldData = mp_StaticData.m_pData;
			// Save new array
			mp_StaticData.m_pData = pNewData;
			mp_StaticData.m_pData->m_Length = NewLen;

			// Destroy old array
			if (pOldData)
			{
				NPrivate::fg_DestroyArray(pOldArray, CurrentLength);
				fp_FreeData(pOldData);
			}
		}
		else
		{
			// We can fit everything in the old array

			t_CData *pOldArray = f_GetArray();
			const t_CData *pSourceArray = _Source.f_GetArray();

			DMibSafeCheck((_StartInList + NewLen) == f_GetLen(), "Must align");

			fg_DestroyCopyArray(pOldArray + _StartInList, pSourceArray + _StartInSource - _StartInList, Len);
		}
	}
}
