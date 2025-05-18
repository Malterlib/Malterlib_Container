// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertFirst()
	{
		//static_assert(NTraits::cIsNothrowConstructibleWith<t_CData, t_CData &&, "For exception safety type must be noexcept movable");

		t_CData *pArray = fp_MakeRoomBegin(1);

		auto Cleanup = g_OnScopeExit / [&]
			{
				fp_MakeRoomBeginUndo(1);
			}
		;

		new((void *)(pArray)) t_CData();

		Cleanup.f_Clear();

		return pArray[0];
	}
}
