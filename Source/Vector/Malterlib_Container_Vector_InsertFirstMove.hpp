// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertFirst(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector)
	{
		//static_assert(NTraits::cIsNothrowConstructibleWith<t_CData, NTraits::TCAddRValueReference<t_CData>>, "For exception safety type must be noexcept movable");
		mint AddLen = _Vector.f_GetLen();
		t_CData *pArray = fp_MakeRoomBegin(AddLen);
		tf_CData *pSrcArray = _Vector.f_GetArray();

		NPrivate::fg_MoveArray(pArray, pSrcArray, AddLen);

		return pArray;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertFirst(t_CData &&_Data)
	{
		//static_assert(NTraits::cIsNothrowConstructibleWith<NTraits::TCRemoveReference<t_CData>, NTraits::TCAddRValueReference<t_CData>>, "For exception safety type must be noexcept movable");
		t_CData *pArray = fp_MakeRoomBegin(1);

#if DMibEnableSafeCheck > 0
		auto Cleanup = g_OnScopeExit / [&]
			{
				DMibFastCheck(false); // Move should not throw
			}
		;
#endif

		new((void *)(pArray)) t_CData(fg_Move(_Data));

#if DMibEnableSafeCheck > 0
		Cleanup.f_Clear();
#endif

		return pArray[0];
	}
}
