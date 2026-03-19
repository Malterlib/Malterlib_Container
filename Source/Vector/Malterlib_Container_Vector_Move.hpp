// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Move(umint _FromPosition, umint _ToPosition, umint _Len)
	{
		if (_ToPosition == _FromPosition || _Len == 0)
			return;

		umint OldLen = f_GetLen();

		fsp_CheckOverlapping(_FromPosition, _ToPosition, _Len, _Len);
		fsp_CheckBounds(OldLen+1, _FromPosition + _Len);
		fsp_CheckBounds(OldLen+1, _ToPosition);

		// TODO: Make inplace

		CVectorData *pNewData = fp_AllocData(fsp_GetAllocSize(OldLen));
		t_CData *pOldArray = f_GetArray();
		t_CData *pNewArray = pNewData->f_GetData();

		if (_FromPosition < _ToPosition)
		{
			NPrivate::fg_MoveArray(pNewArray, pOldArray, _FromPosition);

			umint MiddleCopyLen = _ToPosition - (_FromPosition + _Len);
			umint End = _FromPosition + MiddleCopyLen;
			if (End > _FromPosition)
				NPrivate::fg_MoveArray(pNewArray + _FromPosition, pOldArray + _FromPosition + _Len, (End - _FromPosition));

			umint End2 = End + _Len;
			if (End2 > End)
				NPrivate::fg_MoveArray(pNewArray + End, pOldArray + End - MiddleCopyLen, (End2 - End));

			if (OldLen > End2)
				NPrivate::fg_MoveArray(pNewArray + End2, pOldArray + End2, (OldLen - End2));
		}
		else
		{
			NPrivate::fg_MoveArray(pNewArray, pOldArray, (_ToPosition));

			umint Diff = _FromPosition - _ToPosition;
			umint End = _ToPosition + _Len;
			if (End > _ToPosition)
				NPrivate::fg_MoveArray(pNewArray + _ToPosition, pOldArray + Diff + _ToPosition, (End - _ToPosition));

			umint End2 = End + _FromPosition - _ToPosition;

			if (End2 > End)
				NPrivate::fg_MoveArray(pNewArray + End, pOldArray + End - _Len, (End2 - End));
			if (OldLen > End2)
				NPrivate::fg_MoveArray(pNewArray + End2, pOldArray + End2, (OldLen - End2));
		}

		auto pOldData = mp_StaticData.m_pData;
		// Save new array
		pNewData->m_Length = OldLen;
		mp_StaticData.m_pData = pNewData;

		if (pOldData)
		{
			if (OldLen)
				NPrivate::fg_DestroyArray(pOldArray, OldLen, OldLen);
			fp_FreeData(pOldData);
		}
	}
}
