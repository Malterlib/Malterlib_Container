// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	TCSetWithPool<t_CKey, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::TCSetWithPool()
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	TCSetWithPool<t_CKey, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::TCSetWithPool(TCSetWithPool &&_Other)
		: CSuper((CSuper &&)fg_Move(_Other))
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	TCSetWithPool<t_CKey, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::TCSetWithPool(TCSetWithPool const &_Other)
		: CSuper((CSuper const &)_Other)
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	template <typename tf_COther>
	TCSetWithPool<t_CKey, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::TCSetWithPool(tf_COther &&_Other)
		: CSuper(fg_Forward<tf_COther>(_Other))
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	auto TCSetWithPool<t_CKey, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::operator = (TCSetWithPool &&_Other) -> TCSetWithPool &
	{
		*((CSuper *)this) = ((CSuper &&)fg_Move(_Other));
		return *this;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	auto TCSetWithPool<t_CKey, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::operator = (TCSetWithPool const &_Other) -> TCSetWithPool &
	{
		*((CSuper *)this) = ((CSuper const &)_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	template <typename tf_COther>
	auto TCSetWithPool<t_CKey, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>::operator = (tf_COther &&_Other) -> TCSetWithPool &
	{
		*((CSuper *)this) = fg_Forward<tf_COther>(_Other);
		return *this;
	}
}
