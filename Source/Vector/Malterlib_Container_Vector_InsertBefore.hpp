// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(umint _Position)
	{
		umint PrevLen = f_GetLen();
		fsp_CheckBounds(PrevLen + 1, _Position);
		t_CData *pArray = fp_MakeRoomMiddle(_Position, 1);

		auto Cleanup = g_OnScopeExit / [&]
			{
				fp_MakeRoomMiddleUndo(_Position, 1);
			}
		;

		new((void *)(pArray + _Position)) t_CData();

		Cleanup.f_Clear();

		return pArray[_Position];
	}
}
