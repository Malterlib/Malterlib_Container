// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(umint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector)
	{
		umint PrevLen = f_GetLen();
		fsp_CheckBounds(PrevLen + 1, _Position);
		umint AddLen = _Vector.f_GetLen();
		t_CData *pArray = fp_MakeRoomMiddle(_Position, AddLen);
		tf_CData *pSrcArray = _Vector.f_GetArray();

		NPrivate::fg_MoveArray(pArray + _Position, pSrcArray, AddLen);

		return pArray + _Position;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(umint _Position, t_CData &&_Data)
	{
		umint PrevLen = f_GetLen();
		fsp_CheckBounds(PrevLen + 1, _Position);

		t_CData *pArray = fp_MakeRoomMiddle(_Position, 1);

#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit / [&]
			{
				DMibFastCheck(false); // Move should not throw
			}
		;
#endif

		new((void *)(pArray + _Position)) t_CData(fg_Move(_Data));

#if DMibEnableSafeCheck > 0
		Cleanup.f_Clear();
#endif

		return pArray[_Position];
	}
}
