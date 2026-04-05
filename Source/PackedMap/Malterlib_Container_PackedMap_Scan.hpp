// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	// Scan-optimized traversal over clustered segments
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_FOnEntry>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Scan(tf_FOnEntry &&_fOnEntry) const
	{
		constexpr static bool c_bEarlyExit = requires(t_CKey const &_Key, t_CValue const &_Value)
			{
				{ _fOnEntry(_Key, _Value) } -> NTraits::cIsSame<bool>;
			}
		;

		auto const *pData = mp_pData;
		if (!pData)
			return;

		auto const *pMeta = pData->m_pSegmentMeta;
		auto const *pKeys = pData->m_pKeys;
		auto const *pValues = pData->m_pValues;
		umint nSegments = pData->m_nSegments;

		for (umint iSeg = 0; iSeg < nSegments; iSeg += 2)
		{
			umint iSeg0 = iSeg;
			umint iSeg1 = iSeg + 1;

			umint Count0 = pMeta[iSeg0].m_Count;
			umint Count1 = (iSeg1 < nSegments) ? pMeta[iSeg1].m_Count : 0;

			if (Count0 == 0 && Count1 == 0)
				continue;

			if (Count1 == 0 || iSeg1 >= nSegments)
			{
				if (Count0 == 0)
					continue;

				umint iFirst0 = fsp_GetSegmentFirstSlot(iSeg0, Count0);
				for (umint iSegmentSlot = 0; iSegmentSlot < Count0; ++iSegmentSlot)
				{
					umint iSlot = iFirst0 + iSegmentSlot;
					if constexpr (c_bEarlyExit)
					{
						if (!_fOnEntry(pKeys[iSlot], pValues[iSlot]))
							return;
					}
					else
						_fOnEntry(pKeys[iSlot], pValues[iSlot]);
				}
				continue;
			}

			if (Count0 == 0)
			{
				umint iFirst1 = fsp_GetSegmentFirstSlot(iSeg1, Count1);
				for (umint iSegmentSlot = 0; iSegmentSlot < Count1; ++iSegmentSlot)
				{
					umint iSlot = iFirst1 + iSegmentSlot;
					if constexpr (c_bEarlyExit)
					{
						if (!_fOnEntry(pKeys[iSlot], pValues[iSlot]))
							return;
					}
					else
						_fOnEntry(pKeys[iSlot], pValues[iSlot]);
				}
				continue;
			}

			umint iFirst0 = fsp_GetSegmentFirstSlot(iSeg0, Count0);
			umint iFirst1 = fsp_GetSegmentFirstSlot(iSeg1, Count1);
			umint iLast1 = iFirst1 + Count1 - 1;

			// Segments are clustered: this range is contiguous in memory
			for (umint iSlot = iFirst0; iSlot <= iLast1; ++iSlot)
			{
				if constexpr (c_bEarlyExit)
				{
					if (!_fOnEntry(pKeys[iSlot], pValues[iSlot]))
						return;
				}
				else
					_fOnEntry(pKeys[iSlot], pValues[iSlot]);
			}
		}
	}
}
