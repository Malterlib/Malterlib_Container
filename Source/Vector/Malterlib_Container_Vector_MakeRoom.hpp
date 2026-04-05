// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::fp_MakeRoomBegin(umint _Len)
	{
		if (_Len == 0)
			return nullptr;
		umint OldLen = f_GetLen();
		umint NewLen = OldLen + _Len;
		if (NewLen < OldLen)
			DMibErrorListBoundCheck("Vector length would have overflowed");

		if (fsp_NeedReallocGrow(NewLen, mp_StaticData.m_pData))
		{
			CVectorData *pNewData = fp_AllocDataGrow(NewLen);
			t_CData *pOldArray = f_GetArray();
			t_CData *pNewArray = pNewData->f_GetData();
			// Construct all new datas with copy constructor from old datas

			NPrivate::fg_MoveArray(pNewArray + _Len, pOldArray, OldLen);

			DMibFastCheck(!mp_StaticData.m_pData || pNewData->m_AllocSize != mp_StaticData.m_pData->m_AllocSize || !fp_Allocator().f_DeterministicSize());

			auto pOldData = mp_StaticData.m_pData;
			// Save new array
			pNewData->m_Length = NewLen;
			mp_StaticData.m_pData = pNewData;

			if (pOldData)
			{
				if (OldLen)
					NPrivate::fg_DestroyArray(pOldArray, OldLen, OldLen);
				fp_FreeData(pOldData);
			}

			return pNewData->f_GetData();
		}
		else
		{
			t_CData *pOldArray = f_GetArray();

			// Move old data
			NPrivate::fg_MoveDestroyOverlappingArrayReverse(pOldArray + _Len, pOldArray, OldLen);

			mp_StaticData.m_pData->m_Length = NewLen;
		}
		return mp_StaticData.m_pData->f_GetData();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fp_MakeRoomBeginUndo(umint _Len)
	{
		if (_Len == 0)
			return;
		umint OldLen = f_GetLen();
		umint NewLen = OldLen - _Len;

		t_CData *pOldArray = f_GetArray();
		// Move old data
		NPrivate::fg_MoveDestroyOverlappingArray(pOldArray, pOldArray + _Len, OldLen);
		mp_StaticData.m_pData->m_Length = NewLen;
	}


	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::fp_MakeRoomMiddle(umint _iStart, umint _Len)
	{
		if (_Len == 0)
			return nullptr;
		umint OldLen = f_GetLen();
		umint NewLen = OldLen + _Len;
		if (NewLen < OldLen)
			DMibErrorListBoundCheck("Vector length would have overflowed");

		if (fsp_NeedReallocGrow(NewLen, mp_StaticData.m_pData))
		{
			CVectorData *pNewData = fp_AllocDataGrow(NewLen);
			t_CData *pOldArray = f_GetArray();
			t_CData *pNewArray = pNewData->f_GetData();

			// Move old
			NPrivate::fg_MoveArray(pNewArray, pOldArray, _iStart);
			// Move old after hole
			if (_iStart < OldLen)
				NPrivate::fg_MoveArray(pNewArray + _iStart + _Len, pOldArray + _iStart, OldLen - _iStart);

			DMibFastCheck(!mp_StaticData.m_pData || pNewData->m_AllocSize != mp_StaticData.m_pData->m_AllocSize || !fp_Allocator().f_DeterministicSize());
			auto pOldData = mp_StaticData.m_pData;
			pNewData->m_Length = NewLen;
			mp_StaticData.m_pData = pNewData;

			if (pOldData)
			{
				if (OldLen)
					NPrivate::fg_DestroyArray(pOldArray, OldLen, OldLen);
				fp_FreeData(pOldData);
			}
			return pNewData->f_GetData();
		}
		else
		{
			t_CData *pOldArray = f_GetArray();

			// Move old data
			if (_iStart < OldLen)
				NPrivate::fg_MoveDestroyOverlappingArrayReverse(pOldArray + _Len + _iStart, pOldArray + _iStart, OldLen - _iStart);

			mp_StaticData.m_pData->m_Length = NewLen;
		}
		return mp_StaticData.m_pData->f_GetData();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fp_MakeRoomMiddleUndo(umint _iStart, umint _Len)
	{
		if (_Len == 0)
			return;

		t_CData *pOldArray = f_GetArray();

		umint OldLen = f_GetLen();
		umint NewLen = OldLen - _Len;

		if (_iStart < NewLen)
			NPrivate::fg_MoveDestroyOverlappingArray(pOldArray + _iStart, pOldArray + _iStart + _Len, NewLen - _iStart);

		mp_StaticData.m_pData->m_Length = NewLen;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::fp_MakeRoom(umint _Len)
	{
		umint NewLen = _Len;
		if constexpr (t_COptions::mc_bShrink)
		{
			if (NewLen == 0)
			{
				f_Clear();
				return nullptr;
			}
		}

		umint OldLen = f_GetLen();
		if (fsp_NeedRealloc(NewLen, mp_StaticData.m_pData))
		{
			CVectorData *pNewData = fp_AllocDataGrow(NewLen);
			t_CData *pOldArray = f_GetArray();
			t_CData *pNewArray = pNewData->f_GetData();
			umint CurrentLength = OldLen;

			umint OldRemainingCells = fg_Min(NewLen, CurrentLength);

			// Construct all new datas with copy constructor from old datas
			NPrivate::fg_MoveArray(pNewArray, pOldArray, OldRemainingCells);

			DMibFastCheck(!mp_StaticData.m_pData || pNewData->m_AllocSize != mp_StaticData.m_pData->m_AllocSize || !fp_Allocator().f_DeterministicSize());

			// Save new array
			auto pOldData = mp_StaticData.m_pData;
			pNewData->m_Length = OldRemainingCells;
			mp_StaticData.m_pData = pNewData;

			if (pOldData)
			{
				if (CurrentLength)
					NPrivate::fg_DestroyArray(pOldArray, CurrentLength, CurrentLength);
				fp_FreeData(pOldData);
			}
			return pNewData->f_GetData();
		}
		else
			DMibFastCheck(mp_StaticData.m_pData);
		return mp_StaticData.m_pData->f_GetData();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::fp_MakeNewRoom(umint _Len)
	{
		umint NewLen = _Len;
		if constexpr (t_COptions::mc_bShrink)
		{
			if (NewLen == 0)
			{
				f_Clear();
				return nullptr;
			}
		}

		umint OldLen = f_GetLen();
		if (fsp_NeedRealloc(NewLen, mp_StaticData.m_pData))
		{
			CVectorData *pNewData = fp_AllocDataGrow(NewLen);
			t_CData *pOldArray = f_GetArray();
			umint CurrentLength = OldLen;

			DMibFastCheck(!mp_StaticData.m_pData || pNewData->m_AllocSize != mp_StaticData.m_pData->m_AllocSize || !fp_Allocator().f_DeterministicSize());
			// Destroy old array

			// Save new array
			auto pOldData = mp_StaticData.m_pData;
			mp_StaticData.m_pData = pNewData;

			if (pOldData)
			{
				if (CurrentLength)
					NPrivate::fg_DestroyArray(pOldArray, CurrentLength, CurrentLength);
				fp_FreeData(pOldData);
			}
		}
		else
		{
			if (OldLen)
				NPrivate::fg_DestroyArray(f_GetArray(), OldLen, mp_StaticData.m_pData->m_Length);
		}
		return mp_StaticData.m_pData->f_GetData();
	}
}
