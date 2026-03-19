// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small void TCVector<t_CData, t_CAllocator, t_COptions>::fsp_CheckBounds(umint _Len, umint _Position)
	{
		if constexpr (t_COptions::mc_bCheckBounds)
		{
			if (_Position >= _Len)
				DMibErrorListBoundCheck("Tried to access element outside list");
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	inline_small void TCVector<t_CData, t_CAllocator, t_COptions>::fsp_CheckOverlapping(umint _Start0, umint _Start1, umint _Len0, umint _Len1)
	{
		if constexpr (t_COptions::mc_bCheckBounds)
		{
			if (_Start0 <= _Start1)
			{
				if (_Start0 + _Len0 > _Start1)
					DMibErrorListBoundCheck("Overlapping range in move");
			}
			else
			{
				if (_Start1 + _Len1 > _Start0)
					DMibErrorListBoundCheck("Overlapping range in move");
			}
		}
	}
}
