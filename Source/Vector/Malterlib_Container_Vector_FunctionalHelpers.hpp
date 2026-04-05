// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <typename tf_FFunctor, typename tf_CElement, typename tf_CArray>
	inline_small auto fg_InvokeVectorIteratorFunctor(tf_FFunctor &&_fFunctor, tf_CElement &&_Element, umint _Index, tf_CArray &&_Array)
	{
		if constexpr
		(
			requires ()
			{
				_fFunctor(fg_Forward<tf_CElement>(_Element), _Index, fg_Forward<tf_CArray>(_Array));
			}
		)
		{
			return _fFunctor(fg_Forward<tf_CElement>(_Element), _Index, fg_Forward<tf_CArray>(_Array));
		}
		else if constexpr
		(
			requires ()
			{
				_fFunctor(fg_Forward<tf_CElement>(_Element), _Index);
			}
		)
		{
			return _fFunctor(fg_Forward<tf_CElement>(_Element), _Index);
		}
		else
			return _fFunctor(fg_Forward<tf_CElement>(_Element));
	}

	template <typename tf_FReducer, typename tf_CAccumulator, typename tf_CElement, typename tf_CArray>
	inline_small auto fg_InvokeVectorReduceFunctor(tf_FReducer &&_fReducer, tf_CAccumulator &&_Accumulator, tf_CElement &&_Element, umint _Index, tf_CArray &&_Array)
	{
		if constexpr
		(
			requires ()
			{
				_fReducer(fg_Forward<tf_CAccumulator>(_Accumulator), fg_Forward<tf_CElement>(_Element), _Index, fg_Forward<tf_CArray>(_Array));
			}
		)
		{
			return _fReducer(fg_Forward<tf_CAccumulator>(_Accumulator), fg_Forward<tf_CElement>(_Element), _Index, fg_Forward<tf_CArray>(_Array));
		}
		else if constexpr
		(
			requires ()
			{
				_fReducer(fg_Forward<tf_CAccumulator>(_Accumulator), fg_Forward<tf_CElement>(_Element), _Index);
			}
		)
		{
			return _fReducer(fg_Forward<tf_CAccumulator>(_Accumulator), fg_Forward<tf_CElement>(_Element), _Index);
		}
		else
			return _fReducer(fg_Forward<tf_CAccumulator>(_Accumulator), fg_Forward<tf_CElement>(_Element));
	}

}
