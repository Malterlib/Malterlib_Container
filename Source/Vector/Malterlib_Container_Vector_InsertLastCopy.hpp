// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(std::initializer_list<t_CData> const &_Elements)
	{
		umint iFirstElement = f_GetLen();
		for (auto &Element : _Elements)
			f_Insert(Element);
		return f_GetArray() + iFirstElement;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector)
	{
		umint PrevLen = f_GetLen();
		umint AddLen = _Vector.f_GetLen();
		umint NewLen = PrevLen + AddLen;
		if (NewLen < PrevLen)
			DMibErrorListBoundCheck("Vector length would have overflowed");
		t_CData *pArray = fp_MakeRoom(NewLen);
		tf_CData const *pSrcArray = _Vector.f_GetArray();

		umint nCopied = 0;

		auto Cleanup = g_OnScopeExit / [&]
			{
				if (nCopied)
					NPrivate::fg_DestroyArray(pArray + PrevLen, nCopied, nCopied);
			}
		;

		NPrivate::fg_CopyArray(pArray + PrevLen, pSrcArray, AddLen, nCopied);

		Cleanup.f_Clear();

		if (mp_StaticData.m_pData)
			mp_StaticData.m_pData->m_Length = NewLen;

		return pArray + PrevLen;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector)
	{
		return f_Insert(fg_Const(_Vector));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(const t_CData *_pData, umint _Len)
	{
		umint PrevLen = f_GetLen();
		umint NewLen = PrevLen + _Len;
		if (NewLen < PrevLen)
			DMibErrorListBoundCheck("Vector length would have overflowed");
		t_CData *pArray = fp_MakeRoom(NewLen);

		umint nCopied = 0;

		auto Cleanup = g_OnScopeExit / [&]
			{
				if (nCopied)
					NPrivate::fg_DestroyArray(pArray + PrevLen, nCopied, nCopied);
			}
		;

		NPrivate::fg_CopyArray(pArray + PrevLen, _pData, _Len, nCopied);

		Cleanup.f_Clear();

		if (mp_StaticData.m_pData)
			mp_StaticData.m_pData->m_Length = NewLen;

		return pArray + PrevLen;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertMove(t_CData *_pData, umint _Len)
	{
		umint PrevLen = f_GetLen();
		umint NewLen = PrevLen + _Len;
		if (NewLen < PrevLen)
			DMibErrorListBoundCheck("Vector length would have overflowed");
		t_CData *pArray = fp_MakeRoom(NewLen);

		NPrivate::fg_MoveArray(pArray + PrevLen, _pData, _Len);

		if (mp_StaticData.m_pData)
			mp_StaticData.m_pData->m_Length = NewLen;

		return pArray + PrevLen;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(t_CData *_pData, umint _Len)
	{
		return f_Insert((t_CData const *)_pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_never t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::fp_Insert(t_CData const &_Data)
	{
		umint PrevLen = f_GetLen();
		DMibFastCheck(PrevLen < TCLimitsInt<umint>::mc_Max);
		t_CData *pArray = fp_MakeRoom(PrevLen + 1) + PrevLen;
		new((void *)(pArray)) t_CData(_Data);
		++mp_StaticData.m_pData->m_Length;
		return *pArray;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(const t_CData &_Data)
	{
		umint PrevLen = f_GetLen();
		auto pData = mp_StaticData.m_pData;
		if (fsp_CanGrow(PrevLen + 1, pData))
		{
			DMibFastCheck(pData);
			t_CData *pArray = pData->f_GetData() + PrevLen;
			new((void *)(pArray)) t_CData(_Data);
			++pData->m_Length;
			return *pArray;
		}
		return fp_Insert(_Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_Insert(t_CData &_Data)
	{
		return f_Insert(fg_Const(_Data));
	}
}
