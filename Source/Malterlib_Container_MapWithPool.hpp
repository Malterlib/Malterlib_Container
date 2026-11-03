// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	TCMapWithPool<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::TCMapWithPool()
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	TCMapWithPool<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::TCMapWithPool(TCMapWithPool &&_Other)
		: CSuper((CSuper &&)fg_Move(_Other))
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	TCMapWithPool<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::TCMapWithPool(TCMapWithPool const &_Other)
		: CSuper((CSuper const &)_Other)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	template <typename tf_COther>
	TCMapWithPool<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::TCMapWithPool(tf_COther &&_Other)
		: CSuper(fg_Forward<tf_COther>(_Other))
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	auto TCMapWithPool<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::operator = (TCMapWithPool &&_Other) -> TCMapWithPool &
	{
		*((CSuper *)this) = ((CSuper &&)fg_Move(_Other));
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	auto TCMapWithPool<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::operator = (TCMapWithPool const &_Other) -> TCMapWithPool &
	{
		*((CSuper *)this) = ((CSuper const &)_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	template <typename tf_COther>
	auto TCMapWithPool<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::operator = (tf_COther &&_Other) -> TCMapWithPool &
	{
		*((CSuper *)this) = fg_Forward<tf_COther>(_Other);
		return *this;
	}
}
