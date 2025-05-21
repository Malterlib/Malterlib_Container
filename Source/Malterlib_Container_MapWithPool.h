// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template
	<
		typename t_CKey
		, typename t_CValue
		, typename t_CCompare = NMib::CSort_Default
		, typename t_CAllocator = NMib::NMemory::CAllocator_Virtual
		, mint t_GrowSize = 128
		, typename t_CPoolType = NMib::NMemory::CPoolType_FreeableSmall
		, typename t_CLockType = NMib::NThread::CNoLock
	>
	struct TCMapWithPool : public TCMap<t_CKey, t_CValue, t_CCompare, NMemory::TCPoolAllocator<TCMapNode<t_CKey, t_CValue>, t_GrowSize, t_CAllocator>>
	{
		using CSuper = TCMap<t_CKey, t_CValue, t_CCompare, NMemory::TCPoolAllocator<TCMapNode<t_CKey, t_CValue>, t_GrowSize, t_CAllocator, t_CPoolType, t_CLockType>>;
	public:
		TCMapWithPool()
		{
		}
		TCMapWithPool(TCMapWithPool &&_Other)
			: CSuper((CSuper &&)fg_Move(_Other))
		{
		}
		TCMapWithPool(TCMapWithPool const &_Other)
			: CSuper((CSuper const &)_Other)
		{
		}
		template <typename tf_COther>
		TCMapWithPool(tf_COther &&_Other)
			: CSuper(fg_Forward<tf_COther>(_Other))
		{
		}

		TCMapWithPool &operator = (TCMapWithPool &&_Other)
		{
			*((CSuper *)this) = ((CSuper &&)fg_Move(_Other));
			return *this;
		}
		TCMapWithPool &operator = (TCMapWithPool const &_Other)
		{
			*((CSuper *)this) = ((CSuper const &)_Other);
			return *this;
		}
		template <typename tf_COther>
		TCMapWithPool &operator = (tf_COther &&_Other)
		{
			*((CSuper *)this) = fg_Forward<tf_COther>(_Other);
			return *this;
		}
	};
}

namespace NMib::NContainer::NPrivate
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, mint t_GrowSize, typename t_CPoolType, typename t_CLockType>
	struct TCIsMap<TCMapWithPool<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_GrowSize, t_CPoolType, t_CLockType>>
	{
		static constexpr bool mc_bValue = true;
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
