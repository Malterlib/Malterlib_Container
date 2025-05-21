// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CCompare = NMib::CSort_Default>
	struct TCSetWithPool : public TCSet<t_CKey, t_CCompare, NMemory::TCPoolAllocator<TCMapNode<t_CKey, CMapSet>>>
	{
		using CSuper = TCSet<t_CKey, t_CCompare, NMemory::TCPoolAllocator<TCMapNode<t_CKey, CMapSet>>>;
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
	template <typename t_CKey, typename t_CCompare>
	struct TCIsSet<TCSetWithPool<t_CKey, t_CCompare>>
	{
		static constexpr bool mc_bValue = true;
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
