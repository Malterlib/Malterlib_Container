// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CReturnType>
	inline_small bool TCMapResult<t_CReturnType>::f_WasCreated() const
	{
		return mp_bWasCreated;
	}

	template <typename t_CReturnType>
	inline_small TCMapResult<t_CReturnType>::operator t_CReturnType ()
	{
		return mp_Return;
	}

	template <typename t_CReturnType>
	inline_small t_CReturnType TCMapResult<t_CReturnType>::f_GetResult()
	{
		return mp_Return;
	}

	template <typename t_CReturnType>
	inline_small t_CReturnType TCMapResult<t_CReturnType>::operator * ()
	{
		return mp_Return;
	}

	template <typename t_CReturnType>
	inline_small TCMapResult<t_CReturnType>::TCMapResult(t_CReturnType _Return, bool _bWasCreated)
		: mp_Return(_Return)
		, mp_bWasCreated(_bWasCreated)
	{
	}

}
