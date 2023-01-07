// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Container_Map.h"
#include "Set/Malterlib_Container_Set_Helpers.h"

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CCompare = NMib::CSort_Default, typename t_CAllocator = NMib::NMemory::CAllocator_Heap>
	struct TCSet : public TCMap<t_CKey, CMapSet, t_CCompare, t_CAllocator>
	{
		typedef TCMap<t_CKey, CMapSet, t_CCompare, t_CAllocator> CMap;

		TCSet();
		TCSet(TCSet &&_Other);
		TCSet(TCSet const &_Other);
		TCSet(TCInitializerList<t_CKey> const &_Values);
		template <typename tf_COther>
		TCSet(tf_COther &&_Other);
		TCSet &operator = (TCSet &&_Other);
		TCSet &operator = (TCSet const &_Other);
		template <typename tf_COther>
		TCSet &operator = (tf_COther &&_Other);

		template <typename tf_CContainer, TCEnableIfType<!NTraits::TCIsVoid<decltype(begin(fg_GetType<tf_CContainer &&>()))>::mc_Value> * = nullptr>
		TCSet &f_AddContainer(tf_CContainer &&_Container);

		TCSet f_Or(TCSet const &_Other) const;
		TCSet operator | (TCSet const &_Right) const;
		TCSet f_And(TCSet const &_Other) const;

		TCSet operator & (TCSet const &_Right) const;
		TCSet f_Xor(TCSet const &_Right) const;
		TCSet operator ^ (TCSet const &_Right);
		TCSet f_Difference(TCSet const &_Right) const;
		TCSet operator - (TCSet const &_Right);

		template <typename tf_CFormatInto, typename tf_CFormatOptions>
		void f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const;
	};

	template <typename tf_CReturn, typename... tf_CParams>
	TCSet<tf_CReturn> fg_CreateSet(tf_CParams && ...p_Params);
}

#include "Set/Malterlib_Container_Set.hpp"
#include "Set/Malterlib_Container_Set_Node.hpp"
