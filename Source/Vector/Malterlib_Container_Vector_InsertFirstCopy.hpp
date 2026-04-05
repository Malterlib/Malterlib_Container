// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertFirst(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector)
	{
		//static_assert(NTraits::cIsNothrowConstructibleWith<t_CData, t_CData &&>, "For exception safety type must be noexcept movable");

		umint AddLen = _Vector.f_GetLen();
		t_CData *pArray = fp_MakeRoomBegin(AddLen);
		tf_CData const *pSrcArray = _Vector.f_GetArray();

		umint nCopied = 0;

		auto Cleanup = g_OnScopeExit / [&]
			{
				if (nCopied)
					NPrivate::fg_DestroyArray(pArray, nCopied, nCopied);

				fp_MakeRoomBeginUndo(AddLen);
			}
		;

		NPrivate::fg_CopyArray(pArray, pSrcArray, AddLen, nCopied);

		Cleanup.f_Clear();

		return pArray;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertFirst(TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector)
	{
		return f_InsertFirst(fg_Const(_Vector));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertFirst(const t_CData *_pData, umint _Len)
	{
		//static_assert(NTraits::cIsNothrowConstructibleWith<t_CData, t_CData &&>, "For exception safety type must be noexcept movable");
		t_CData *pArray = fp_MakeRoomBegin(_Len);

		umint nCopied = 0;

		auto Cleanup = g_OnScopeExit / [&]
			{
				if (nCopied)
					NPrivate::fg_DestroyArray(pArray, nCopied, nCopied);

				fp_MakeRoomBeginUndo(_Len);
			}
		;

		NPrivate::fg_CopyArray(pArray, _pData, _Len, nCopied);

		Cleanup.f_Clear();

		return pArray;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertFirstMove(t_CData *_pData, umint _Len)
	{
		t_CData *pArray = fp_MakeRoomBegin(_Len);

		NPrivate::fg_MoveArray(pArray, _pData, _Len);

		return pArray;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertFirst(t_CData *_pData, umint _Len)
	{
		return f_InsertFirst((t_CData const *)_pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertFirst(const t_CData &_Data)
	{
		//static_assert(NTraits::cIsNothrowConstructibleWith<t_CData, t_CData &&>, "For exception safety type must be noexcept movable");
		t_CData *pArray = fp_MakeRoomBegin(1);

		auto Cleanup = g_OnScopeExit / [&]
			{
				fp_MakeRoomBeginUndo(1);
			}
		;

		new((void *)(pArray)) t_CData(_Data);

		Cleanup.f_Clear();

		return pArray[0];
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertFirst(t_CData &_Data)
	{
		return f_InsertFirst(fg_Const(_Data));
	}
}
