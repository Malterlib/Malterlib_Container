// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "Malterlib_Container_Map.h"
#include "Set/Malterlib_Container_Set_Helpers.h"

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CCompare = NMib::CSort_Default, typename t_CAllocator = NMib::NMemory::CAllocator_Heap>
	struct TCSet : public TCMap<t_CKey, CMapSet, t_CCompare, t_CAllocator>
	{
		using CMap = TCMap<t_CKey, CMapSet, t_CCompare, t_CAllocator>;

		TCSet();
		TCSet(TCSet &&_Other);
		TCSet(TCSet const &_Other);
		TCSet(std::initializer_list<t_CKey> const &_Values);

		template <typename... tfp_CParams>
		TCSet(CAllocatorConstructTag const &, tfp_CParams && ...p_Params);

		template <typename... tfp_CParams>
		TCSet(CCompareConstructTag const &, tfp_CParams && ...p_Params);

		template <typename... tfp_CAllocatorParams, typename... tfp_CCompareParams>
		TCSet
			(
				CAllocatorConstructTag &&
				, CCompareConstructTag &&
				, TCConstruct<void, tfp_CAllocatorParams...> &&_ConstructAllocator
				, TCConstruct<void, tfp_CCompareParams...> &&_ConstructCompare
			)
		;

		template <typename tf_COther>
		TCSet(tf_COther &&_Other);
		TCSet &operator = (TCSet &&_Other);
		TCSet &operator = (TCSet const &_Other);
		template <typename tf_COther>
		TCSet &operator = (tf_COther &&_Other);

		template <typename tf_CKey>
		auto operator[] (tf_CKey &&_Key) -> typename CMap::CUserData &;

#ifdef DCompiler_MSVC_Workaround
		template <typename tf_CContainer>
		TCSet & f_AddContainer(tf_CContainer &&_Container)
			requires (!NTraits::cIsVoid<decltype(begin(fg_GetType<tf_CContainer &&>()))>)
		{
			for (auto &Value : _Container)
				(*this)[Value];
			return *this;
		}

		template <typename tf_CContainer>
		static TCSet fs_FromContainer(tf_CContainer &&_Container)
			requires (!NTraits::cIsVoid<decltype(begin(fg_GetType<tf_CContainer &&>()))>)
		{
			TCSet Return;
			Return.f_AddContainer(fg_Forward<tf_CContainer>(_Container));
			return Return;
		}
#else
		template <typename tf_CContainer>
		TCSet &f_AddContainer(tf_CContainer &&_Container)
			requires (!NTraits::cIsVoid<decltype(begin(fg_GetType<tf_CContainer &&>()))>)
		;
		template <typename tf_CContainer>
		static TCSet fs_FromContainer(tf_CContainer &&_Container)
			requires (!NTraits::cIsVoid<decltype(begin(fg_GetType<tf_CContainer &&>()))>)
		;
#endif

		template <typename t_CThis>
		TCSet f_Or(this t_CThis &&_This, TCSet const &_Right);
		template <typename t_CThis>
		TCSet f_Or(this t_CThis &&_This, TCSet &&_Right);
		template <typename t_CThis>
		TCSet operator | (this t_CThis &&_This, TCSet const &_Right);
		template <typename t_CThis>
		TCSet operator | (this t_CThis &&_This, TCSet &&_Right);

		template <typename t_CThis>
		TCSet f_And(this t_CThis &&_This, TCSet const &_Right);
		template <typename t_CThis>
		TCSet f_And(this t_CThis &&_This, TCSet &&_Right);
		template <typename t_CThis>
		TCSet operator & (this t_CThis &&_This, TCSet const &_Right);
		template <typename t_CThis>
		TCSet operator & (this t_CThis &&_This, TCSet &&_Right);

		template <typename t_CThis>
		TCSet f_Xor(this t_CThis &&_This, TCSet const &_Right);
		template <typename t_CThis>
		TCSet f_Xor(this t_CThis &&_This, TCSet &&_Right);
		template <typename t_CThis>
		TCSet operator ^ (this t_CThis &&_This, TCSet const &_Right);
		template <typename t_CThis>
		TCSet operator ^ (this t_CThis &&_This, TCSet &&_Right);

		template <typename t_CThis>
		TCSet f_Difference(this t_CThis &&_This, TCSet const &_Right);
		template <typename t_CThis>
		TCSet operator - (this t_CThis &&_This, TCSet const &_Right);

		using CIteratorDestructive = typename CMap::CKeyIteratorDestructive;
		using CIteratorReverseDestructive = typename CMap::CKeyIteratorReverseDestructive;
		using CIteratorBidirectionalDestructive = typename CMap::CKeyIteratorBidirectionalDestructive;
		using CIteratorBidirectionalReverseDestructive = typename CMap::CKeyIteratorBidirectionalReverseDestructive;

		CIteratorDestructive f_GetIteratorDestructive() &&;
		CIteratorReverseDestructive f_GetIteratorReverseDestructive() &&;
		CIteratorBidirectionalDestructive f_GetIteratorBidirectionalDestructive() &&;
		CIteratorBidirectionalReverseDestructive f_GetIteratorBidirectionalReverseDestructive() &&;

		template <typename tf_CFormatInto, typename tf_CFormatOptions>
		void f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const;
	};

	template <typename tf_CReturn, typename... tf_CParams>
	TCSet<tf_CReturn> fg_CreateSet(tf_CParams && ...p_Params);

	template <typename tf_CContainer>
	auto fg_SetFromContainer(tf_CContainer &&_Container);

	template <typename t_CType>
	concept cIsSet = NPrivate::TCIsSet<NTraits::TCRemoveReferenceAndQualifiers<t_CType>>::mc_bValue;
}

#include "Set/Malterlib_Container_Set.hpp"
#include "Set/Malterlib_Container_Set_Node.hpp"
#include "Set/Malterlib_Container_Set_SetFromContainer.hpp"
