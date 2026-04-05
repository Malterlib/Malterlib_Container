// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData TCVector<t_CData, t_CAllocator, t_COptions>::f_Pop()
	{
		t_CData Temp = fg_Move((*this)[0]);
		f_Remove(0);
		return Temp;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData TCVector<t_CData, t_CAllocator, t_COptions>::f_PopBack()
	{
		umint CurrentLen = f_GetLen();
		umint NewLen = CurrentLen - 1;
		fsp_CheckBounds(CurrentLen, NewLen);

		t_CData *pOldArray = f_GetArray();
		t_CData Temp = fg_Move(pOldArray[NewLen]);

		if constexpr (t_COptions::mc_bShrink)
		{
			if (NewLen == 0)
			{
				f_Clear();
				return Temp;
			}
		}

		if (fsp_NeedRealloc(NewLen, mp_StaticData.m_pData)) [[unlikely]]
		{
			auto *pNewData = fp_AllocDataGrow(NewLen);

			t_CData *pOldArray = f_GetArray();
			t_CData *pNewArray = pNewData->f_GetData();

			NPrivate::fg_MoveArray(pNewArray, pOldArray, NewLen);

			DMibFastCheck(!mp_StaticData.m_pData || pNewData->m_AllocSize != mp_StaticData.m_pData->m_AllocSize || !fp_Allocator().f_DeterministicSize());
			auto pOldData = mp_StaticData.m_pData;
			pNewData->m_Length = NewLen;
			mp_StaticData.m_pData = pNewData;

			if (pOldData)
			{
				if (CurrentLen)
					NPrivate::fg_DestroyArray(pOldArray, CurrentLen, CurrentLen);
				fp_FreeData(pOldData);
			}
		}
		else
		{
			NPrivate::fg_DestroyArray(pOldArray + NewLen, 1, CurrentLen);
			mp_StaticData.m_pData->m_Length = NewLen;
		}

		return Temp;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Remove(umint _Start, umint _Len)
	{
		if (_Len == 0)
			return;
		umint CurrentLen = f_GetLen();

		fsp_CheckBounds(CurrentLen, _Start);
		fsp_CheckBounds(CurrentLen, _Start + _Len - 1);

		_Start = fg_Min(_Start, CurrentLen - 1);
		umint ToDestroyLen = fg_Min(CurrentLen - _Start, _Len);

		umint NewLen = CurrentLen - ToDestroyLen;
		if constexpr (t_COptions::mc_bShrink)
		{
			if (NewLen == 0)
				return f_Clear();
		}

		if (NewLen != CurrentLen)
		{
			if (fsp_NeedRealloc(NewLen, mp_StaticData.m_pData))
			{
				// New list
				auto *pNewData = fp_AllocDataGrow(NewLen);

				t_CData *pOldArray = f_GetArray();
				t_CData *pNewArray = pNewData->f_GetData();

				// Construct all new datas with copy constructor from old datas
				NPrivate::fg_MoveArray(pNewArray, pOldArray, _Start);
				if (NewLen > _Start)
					NPrivate::fg_MoveArray(pNewArray + _Start, pOldArray + _Start + ToDestroyLen, (NewLen - _Start));

				DMibFastCheck(!mp_StaticData.m_pData || pNewData->m_AllocSize != mp_StaticData.m_pData->m_AllocSize || !fp_Allocator().f_DeterministicSize());
				auto pOldData = mp_StaticData.m_pData;
				pNewData->m_Length = NewLen;
				mp_StaticData.m_pData = pNewData;

				if (pOldData)
				{
					if (CurrentLen)
						NPrivate::fg_DestroyArray(pOldArray, CurrentLen, CurrentLen);
					fp_FreeData(pOldData);
				}
			}
			else
			{
				t_CData *pOldArray = f_GetArray();

				// Destroy All datas that are to be destroyed
				if (ToDestroyLen)
				{
					umint nToDestroy = ToDestroyLen;
					NPrivate::fg_DestroyArray(pOldArray + _Start, ToDestroyLen, nToDestroy);
				}

				if (NewLen > _Start)
					NPrivate::fg_MoveDestroyOverlappingArray(pOldArray + _Start, pOldArray + _Start + ToDestroyLen, (NewLen - _Start));

				mp_StaticData.m_pData->m_Length = NewLen;
			}
		}
	}
}
