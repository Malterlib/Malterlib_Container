// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template
	<
		typename t_CKey
		, typename t_CCompare = NMib::CSort_Default
		, typename t_CAllocator = NMib::NMemory::CAllocator_Virtual
		, umint t_GrowSize = 128
		, typename t_CPoolType = NMib::NMemory::CPoolType_FreeableSmall
		, typename t_CLockType = NMib::NThread::CNoLock
	>
	struct TCSetWithPool
		: public TCSet
			<
				t_CKey
				, t_CCompare
				, NMemory::TCPoolAllocator<TCMapNode<t_CKey, CMapSet>, t_GrowSize, t_CAllocator, t_CPoolType, t_CLockType>
			>
	{
		using CSuper = TCSet
			<
				t_CKey
				, t_CCompare
				, NMemory::TCPoolAllocator<TCMapNode<t_CKey, CMapSet>, t_GrowSize, t_CAllocator, t_CPoolType, t_CLockType>
			>
		;
	public:
		TCSetWithPool()
		{
		}
		TCSetWithPool(TCSetWithPool &&_Other)
			: CSuper((CSuper &&)fg_Move(_Other))
		{
		}
		TCSetWithPool(TCSetWithPool const &_Other)
			: CSuper((CSuper const &)_Other)
		{
		}
		template <typename tf_COther>
		TCSetWithPool(tf_COther &&_Other)
			: CSuper(fg_Forward<tf_COther>(_Other))
		{
		}

		TCSetWithPool &operator = (TCSetWithPool &&_Other)
		{
			*((CSuper *)this) = ((CSuper &&)fg_Move(_Other));
			return *this;
		}
		TCSetWithPool &operator = (TCSetWithPool const &_Other)
		{
			*((CSuper *)this) = ((CSuper const &)_Other);
			return *this;
		}
		template <typename tf_COther>
		TCSetWithPool &operator = (tf_COther &&_Other)
		{
			*((CSuper *)this) = fg_Forward<tf_COther>(_Other);
			return *this;
		}
	};
}

namespace NMib::NContainer::NPrivate
{
	template <typename t_CKey, typename t_CCompare, typename t_CAllocator, umint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	struct TCIsSet<TCSetWithPool<t_CKey, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>>
	{
		static constexpr bool mc_bValue = true;
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
