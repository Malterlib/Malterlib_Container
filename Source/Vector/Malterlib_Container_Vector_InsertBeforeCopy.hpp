// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(mint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector)
	{
		mint PrevLen = f_GetLen();
		fsp_CheckBounds(PrevLen + 1, _Position);
		mint AddLen = _Vector.f_GetLen();
		t_CData *pArray = fp_MakeRoomMiddle(_Position, AddLen);
		tf_CData const *pSrcArray = _Vector.f_GetArray();

		mint nCopied = 0;

		auto Cleanup = g_OnScopeExit / [&]
			{
				if (nCopied)
					NPrivate::fg_DestroyArray(pArray + _Position, nCopied, nCopied);

				fp_MakeRoomMiddleUndo(_Position, nCopied);
			}
		;

		NPrivate::fg_CopyArray(pArray + _Position, pSrcArray, AddLen, nCopied);

		Cleanup.f_Clear();

		return pArray + _Position;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(mint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector)
	{
		return f_InsertBefore(_Position, fg_Const(_Vector));
	}
	
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(mint _Position, const t_CData *_pData, mint _Len)
	{
		aint PrevLen = f_GetLen();
		fsp_CheckBounds(PrevLen + 1, _Position);
		t_CData *pArray = fp_MakeRoomMiddle(_Position, _Len);

		mint nCopied = 0;

		auto Cleanup = g_OnScopeExit / [&]
			{
				if (nCopied)
					NPrivate::fg_DestroyArray(pArray + _Position, nCopied, nCopied);

				fp_MakeRoomMiddleUndo(_Position, nCopied);
			}
		;

		NPrivate::fg_CopyArray(pArray + _Position, _pData, _Len, nCopied);

		Cleanup.f_Clear();

		return pArray + _Position;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(mint _Position, t_CData *_pData, mint _Len)
	{
		return f_InsertBefore(_Position, (t_CData const *)_pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(mint _Position, const t_CData &_Data)
	{
		aint PrevLen = f_GetLen();
		fsp_CheckBounds(PrevLen + 1, _Position);
		t_CData *pArray = fp_MakeRoomMiddle(_Position, 1);
		try
		{
			new((void *)(pArray + _Position)) t_CData(_Data);
		}
		catch (...)
		{
			fp_MakeRoomMiddleUndo(_Position, 1);
			throw;
		}
		return pArray[_Position];
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(mint _Position, t_CData &_Data)
	{
		return f_InsertBefore(_Position, fg_Const(_Data));
	}
}
