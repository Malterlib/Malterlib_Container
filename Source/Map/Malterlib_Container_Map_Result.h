// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CReturnType>
	struct TCMapResult
	{
		mark_nodebug inline_small bool f_WasCreated() const;
		mark_nodebug inline_small operator t_CReturnType ();
		mark_nodebug inline_small t_CReturnType f_GetResult();
		mark_nodebug inline_small t_CReturnType operator *();

	private:
		template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
		friend struct TCMap;

		template <typename t_CNode>
		friend struct NPrivate::TCMapMapper;

		template <typename t_CMap>
		friend struct NPrivate::TCMapConditionalMapper;


		inline_small TCMapResult(t_CReturnType _Return, bool _bWasCreated);

		t_CReturnType mp_Return;
		bool mp_bWasCreated;
	};
}
