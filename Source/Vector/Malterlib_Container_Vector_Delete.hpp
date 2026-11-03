// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_DeleteAll()
	{
		umint Len = f_GetLen();
		t_CData *pArray = f_GetArray();
		for (smint iElement = Len - 1; iElement >= 0; --iElement)
		{
			fg_DeleteObject(NMemory::CDefaultAllocator(), pArray[iElement]);
			--mp_StaticData.m_pData->m_Length;
		}

		if (mp_StaticData.m_pData)
		{
			fp_FreeData(mp_StaticData.m_pData);
			mp_StaticData.m_pData = nullptr;
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_DeleteAllDefiniteType()
	{
		umint Len = f_GetLen();
		t_CData *pArray = f_GetArray();
		for (smint iElement = Len - 1; iElement >= 0; --iElement)
		{
			fg_DeleteObjectDefiniteType(NMemory::CDefaultAllocator(), pArray[iElement]);
			--mp_StaticData.m_pData->m_Length;
		}

		if (mp_StaticData.m_pData)
		{
			fp_FreeData(mp_StaticData.m_pData);
			mp_StaticData.m_pData = nullptr;
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CAllocator, typename tf_CData>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fsp_DeleteAllAllocatorHelper(tf_CData *_pData)
	{
		fg_DeleteObject(tf_CAllocator(), _pData);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CAllocator>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_DeleteAllAllocator()
	{
		umint Len = f_GetLen();
		t_CData *pArray = f_GetArray();
		for (smint iElement = Len - 1; iElement >= 0; --iElement)
		{
			fsp_DeleteAllAllocatorHelper<tf_CAllocator>(pArray[iElement]);
			--mp_StaticData.m_pData->m_Length;
		}

		if (mp_StaticData.m_pData)
		{
			fp_FreeData(mp_StaticData.m_pData);
			mp_StaticData.m_pData = nullptr;
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CAllocator, typename tf_CData>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fsp_DeleteAllAllocatorHelperDefiniteType(tf_CData *_pData)
	{
		fg_DeleteObjectDefiniteType(tf_CAllocator(), _pData);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CAllocator>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_DeleteAllAllocatorDefiniteType()
	{
		umint Len = f_GetLen();
		t_CData *pArray = f_GetArray();
		for (smint iElement = Len - 1; iElement >= 0; --iElement)
		{
			fsp_DeleteAllAllocatorHelperDefiniteType<tf_CAllocator>(pArray[iElement]);
			--mp_StaticData.m_pData->m_Length;
		}

		if (mp_StaticData.m_pData)
		{
			fp_FreeData(mp_StaticData.m_pData);
			mp_StaticData.m_pData = nullptr;
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CDeleter>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_DeleteAllDeleter()
	{
		umint Len = f_GetLen();
		t_CData *pArray = f_GetArray();
		for (smint iElement = Len - 1; iElement >= 0; --iElement)
		{
			tf_CDeleter::fs_Delete(pArray[iElement]);
			--mp_StaticData.m_pData->m_Length;
		}

		if (mp_StaticData.m_pData)
		{
			fp_FreeData(mp_StaticData.m_pData);
			mp_StaticData.m_pData = nullptr;
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fp_DeleteAllDeleterHelper(tf_CData *_pData)
	{
		f_DeleteAllDeleter<tf_CData>();
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_DeleteAllDeleter()
	{
		fp_DeleteAllDeleterHelper((t_CData)0);
	}
}
