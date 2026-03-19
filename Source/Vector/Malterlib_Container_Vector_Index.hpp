// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsPosValid(aint _Index) const
	{
		if (_Index < 0)
			return false;
		if (_Index >= (aint)f_GetLen())
			return false;
		return true;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::operator [](umint _Index)
	{
		fsp_CheckBounds(f_GetLen(), _Index);
		return (mp_StaticData.m_pData->f_GetData())[_Index];
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small const t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::operator [](umint _Index) const
	{
		fsp_CheckBounds(f_GetLen(), _Index);
		return (mp_StaticData.m_pData->f_GetData())[_Index];
	}
}
