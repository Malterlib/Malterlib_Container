// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_DeleteAll()
	{
		mint Len = f_GetLen();
		t_CData *pArray = f_GetArray();
		for (smint i = Len - 1; i >= 0; --i)
		{
			fg_DeleteObject(NMemory::CDefaultAllocator(), pArray[i]);
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
		mint Len = f_GetLen();
		t_CData *pArray = f_GetArray();
		for (smint i = Len - 1; i >= 0; --i)
		{
			fg_DeleteObjectDefiniteType(NMemory::CDefaultAllocator(), pArray[i]);
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
		mint Len = f_GetLen();
		t_CData *pArray = f_GetArray();
		for (smint i = Len - 1; i >= 0; --i)
		{
			fsp_DeleteAllAllocatorHelper<tf_CAllocator>(pArray[i]);
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
		mint Len = f_GetLen();
		t_CData *pArray = f_GetArray();
		for (smint i = Len - 1; i >= 0; --i)
		{
			fsp_DeleteAllAllocatorHelperDefiniteType<tf_CAllocator>(pArray[i]);
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
		mint Len = f_GetLen();
		t_CData *pArray = f_GetArray();
		for (smint i = Len - 1; i >= 0; --i)
		{
			tf_CDeleter::fs_Delete(pArray[i]);
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
