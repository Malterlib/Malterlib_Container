// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CSource>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Replace(tf_CSource &&_Source, umint _StartInSource, umint _Len, umint _StartInList, umint _MinLen)
		requires (NTraits::cIsSame<NTraits::TCRemoveReference<tf_CSource>, TCVector>)
	{
		constexpr static bool c_bIsMove = NTraits::cIsRValueReference<tf_CSource &&>;

		umint CurrentLength = f_GetLen();
		umint Len = fg_Min(_Len, _Source.f_GetLen() - _StartInSource);
		umint NewLen = fg_Max(fg_Max(_StartInList + Len, CurrentLength), _MinLen);

		CVectorData *pNewData;
		if (NewLen != CurrentLength)
		{
			// New list
			pNewData = fp_AllocData(NewLen);

			auto *pOldArray = f_GetArray();
			auto *pSourceArray = _Source.f_GetArray();
			auto *pNewArray = pNewData->f_GetData();

			umint OldRemainingCells = fg_Min(CurrentLength, _StartInList);
			umint MaxLen = fg_Max(_StartInList + Len, CurrentLength);

			umint nConstructed0 = 0;
			umint nConstructed1 = 0;
			umint nCopied = 0;

			auto Cleanup = g_OnScopeExit / [&]
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
			if constexpr (c_bIsMove)
				NPrivate::fg_MoveArray(pNewArray + _StartInList, pSourceArray + _StartInSource, Len);
			else
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
				if (CurrentLength)
					NPrivate::fg_DestroyArray(pOldArray, CurrentLength, CurrentLength);
				fp_FreeData(pOldData);
			}
		}
		else
		{
			// We can fit everything in the old array

			auto *pOldArray = f_GetArray();
			auto *pSourceArray = _Source.f_GetArray();

			DMibSafeCheck((_StartInList + Len) <= CurrentLength, "Must align");

			if constexpr (c_bIsMove)
				NPrivate::fg_MoveOverArray(pOldArray + _StartInList, pSourceArray + _StartInSource, Len);
			else
				NPrivate::fg_CopyOverArray(pOldArray + _StartInList, pSourceArray + _StartInSource, Len);
		}
	}
}
