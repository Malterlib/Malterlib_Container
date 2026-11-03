// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Core/Core>
#include <Mib/Container/Map>

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CRegionData = NMib::CVoidTag>
	struct TCRegionData : public t_CRegionData
	{
		TCRegionData();
		TCRegionData(TCRegionData const &_Other);
		TCRegionData(TCRegionData &&_Other);
		t_CKey const &f_End() const;
		t_CKey &f_End();
		t_CRegionData &f_Data();
		t_CRegionData const &f_Data() const;

		DMibListLinkD_Link(TCRegionData, m_Link);

	private:
		t_CKey mp_End;
	};

	template <typename t_CKey, typename t_CRegionData = NMib::CVoidTag, typename t_CAllocator = NMemory::CAllocator_Heap>
	struct TCRegions
	{
	private:
		// Declared before the public section, which uses them.
		struct CRegionData;
		using CMap = TCMap<t_CKey, CRegionData, CSort_Default, t_CAllocator>;

	public:
		using CIterator = typename CMap::CIterator;
		using CIteratorConst = typename CMap::CIteratorConst;

		void f_Clear();
		bool f_IsEmpty() const;
		CIterator f_GetIterator();
		CIteratorConst f_GetIterator() const;
		CIterator f_GetIterator(t_CKey const &_Start);
		CIteratorConst f_GetIterator(t_CKey const &_Start) const;
		CIterator f_GetIteratorLower(t_CKey const &_Start);
		CIteratorConst f_GetIteratorLower(t_CKey const &_Start) const;
		CRegionData const *f_GetData(t_CKey const &_Key) const;
		void f_MakeRegion(t_CKey const &_Start, t_CKey const &_End);

		template <class tf_CTransform>
		void f_MakeRegion(t_CKey const &_Start, t_CKey const &_End, tf_CTransform const &_Transform);

	private:
		struct CRegionData : public TCRegionData<t_CKey, t_CRegionData>
		{
			CRegionData();
			CRegionData(CRegionData const &_Other);
			CRegionData(CRegionData &&_Other);
			t_CKey const &f_Start() const;
		};

		CMap mp_Regions;
	};
}

#ifndef DMibPNoShortCuts
using namespace NMib::NContainer;
#endif

#include "Malterlib_Container_Regions.hpp"
