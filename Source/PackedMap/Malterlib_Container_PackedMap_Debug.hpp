// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Debug_Validate(NStr::CStr *_pOutError) const
	{
		using namespace NStr;
		auto const *pData = mp_pData;
		if (!pData)
			return true;

		auto const *pMeta = pData->m_pSegmentMeta;
		auto const *pKeys = pData->m_pKeys;
		auto const *pIndex = pData->m_pIndex;
		mint nSegments = pData->m_nSegments;

		auto fFail = [&](ch8 const *_pMsg) -> bool
			{
				if (_pOutError)
					*_pOutError = _pMsg;
				return false;
			}
		;

		auto fKeyToStr = [&](t_CKey const &_Key) -> NStr::CStr
			{
				if constexpr (requires { "{}"_f << _Key; })
					return "{}"_f << _Key;
				else
					return gc_Str<"<key>">;
			}
		;

		// Check counts and total elements
		mint nTotal = 0;
		for (mint iSeg = 0; iSeg < nSegments; ++iSeg)
		{
			mint Count = pMeta[iSeg].m_Count;
			if (Count > mcp_SegmentSize)
				return fFail("Segment count exceeds segment size");
			nTotal += Count;
		}

		if (nTotal != pData->m_nElements)
			return fFail("Total element count mismatch");

		// Check per-segment ordering and global ordering
		t_CKey const *pPrevKey = nullptr;
		mint iPrevSeg = 0;
		mint iPrevLocal = 0;
		for (mint iSeg = 0; iSeg < nSegments; ++iSeg)
		{
			mint Count = pMeta[iSeg].m_Count;
			if (Count == 0)
				continue;

			mint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
			for (mint iKey = 0; iKey < Count; ++iKey)
			{
				t_CKey const &Key = pKeys[iFirst + iKey];
				if (iKey > 0)
				{
					if (mp_Compare(pKeys[iFirst + iKey - 1], Key) >= 0)
					{
						if (_pOutError)
							*_pOutError = "Segment keys not strictly increasing at seg {} local {}"_f << iSeg << iKey;
						return false;
					}
				}
				if (pPrevKey)
				{
					if (mp_Compare(*pPrevKey, Key) >= 0)
					{
						if (_pOutError)
						{
							NStr::CStr PrevStr = fKeyToStr(*pPrevKey);
							NStr::CStr CurrStr = fKeyToStr(Key);
							*_pOutError = "Global key order violated: prev seg {} local {} key {}, curr seg {} local {} key {}"_f
								<< iPrevSeg << iPrevLocal << PrevStr << iSeg << iKey << CurrStr;
						}
						return false;
					}
				}
				pPrevKey = &Key;
				iPrevSeg = iSeg;
				iPrevLocal = iKey;
			}
		}

		// Validate static index leaf separators (min keys)
		for (mint iSeg = 0; iSeg < nSegments; ++iSeg)
		{
			mint Count = pMeta[iSeg].m_Count;
			bool bValid = pData->f_GetIndexEntryValid(iSeg);
			if (Count == 0)
			{
				if (bValid)
					return fFail("Index leaf valid for empty segment");
				continue;
			}

			if (!bValid)
				return fFail("Index leaf invalid for non-empty segment");

			mint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
			t_CKey const &MinKey = pKeys[iFirst];

			if constexpr (mcp_bIndexStoresKeys)
			{
				if (mp_Compare(pIndex[iSeg], MinKey) != 0)
					return fFail("Index leaf key mismatch (min key)");
			}
			else
			{
				mint iSlot = pIndex[iSeg];
				if (iSlot != iFirst)
					return fFail("Index leaf slot mismatch (min key)");
			}
		}

		// Validate density invariants at each calibrator tree level
		// According to the RMA paper, each window at each level should have density within [ρ(h), τ(h)]
		mint nLevels = fp_CalibratorLevelCount(nSegments);
		if (nLevels > 0 && nTotal > 0)
		{
			// Calculate total capacity - lower bounds only apply when we have enough elements
			mint TotalCapacity = nSegments * mcp_SegmentSize;

			// Check density bounds at each calibrator tree level
			for (mint iLevel = 0; iLevel < nLevels; ++iLevel)
			{
				mint WindowSize = (mint)1 << iLevel;
				for (mint iWin = 0; iWin < nSegments; iWin += WindowSize)
				{
					mint iEnd = fg_Min(iWin + WindowSize, nSegments);
					mint nWinElements = 0;
					for (mint iSegment = iWin; iSegment < iEnd; ++iSegment)
						nWinElements += pMeta[iSegment].m_Count;

					mint WinCapacity = (iEnd - iWin) * mcp_SegmentSize;

					mint ScaledLowerBound, ScaledUpperBound;
					fp_GetLevelThresholds(iLevel, nLevels, ScaledLowerBound, ScaledUpperBound);

					if (nWinElements > fsp_ScaledMulFloor(ScaledUpperBound + ScaledUpperBound / 100, WinCapacity))  // Allow 1% tolerance for edge cases
					{
						if (_pOutError)
							*_pOutError = "Level {} window [{},{}) elems {} exceeds tau threshold"_f << iLevel << iWin << iEnd << nWinElements;
						return false;
					}

					// Lower bounds only apply when:
					// 1. rho > 0 (some profiles disable lower bounds at leaf level)
					// 2. The global density is above the root lower bound (structure has enough elements)
					// 3. The expected elements in this window (based on proportional fill) would be >= 1
					// This prevents false failures when inserting into a nearly-empty structure
					mint nWindowSegments = iEnd - iWin;
					mint nScaledExpectedWindowElements;
					bool bExpectedWindowCouldContainElement = fg_MultiplyOverflow(nTotal, nWindowSegments, nScaledExpectedWindowElements) || (nScaledExpectedWindowElements >= nSegments);
					bool bCheckLowerBound = (ScaledLowerBound > 0) && (nTotal >= fsp_ScaledMulCeil(mcp_RootLowerBoundScaled, TotalCapacity)) && bExpectedWindowCouldContainElement;

					if (bCheckLowerBound && nWinElements < fsp_ScaledMulCeil(ScaledLowerBound - ScaledLowerBound / 100, WinCapacity) && nWinElements > 0)
					{
						if (_pOutError)
							*_pOutError = "Level {} window [{},{}) elems {} below rho threshold"_f << iLevel << iWin << iEnd << nWinElements;
						return false;
					}
				}
			}
		}

		// Validate upper index levels (parent = min valid child)
		CIndexLayout Layout = fsp_ComputeIndexLayout(nSegments);
		for (mint iLevel = 1; iLevel < pData->m_nStaticIndexLevels; ++iLevel)
		{
			mint iChildLevel = iLevel - 1;
			mint ChildSize = Layout.m_LevelSizes[iChildLevel];
			mint iChildOffset = Layout.m_LevelOffsets[iChildLevel];
			mint ThisSize = Layout.m_LevelSizes[iLevel];
			mint iThisOffset = Layout.m_LevelOffsets[iLevel];

			for (mint iParent = 0; iParent < ThisSize; ++iParent)
			{
				mint iChildStart = iParent * mcp_Fanout;
				mint iChildEnd = fg_Min(iChildStart + mcp_Fanout, ChildSize);

				mint iMinChild = 0;
				bool bFoundValid = false;
				for (mint iChild = iChildStart; iChild < iChildEnd; ++iChild)
				{
					if (pData->f_GetIndexEntryValid(iChildOffset + iChild))
					{
						iMinChild = iChild;
						bFoundValid = true;
						break;
					}
				}

				mint iParentEntry = iThisOffset + iParent;
				bool bParentValid = pData->f_GetIndexEntryValid(iParentEntry);

				if (!bFoundValid)
				{
					if (bParentValid)
						return fFail("Index parent valid with no valid children");
					continue;
				}

				if (!bParentValid)
					return fFail("Index parent invalid with valid children");

				if constexpr (mcp_bIndexStoresKeys)
				{
					if (mp_Compare(pIndex[iParentEntry], pIndex[iChildOffset + iMinChild]) != 0)
						return fFail("Index parent key mismatch");
				}
				else
				{
					if (pIndex[iParentEntry] != pIndex[iChildOffset + iMinChild])
						return fFail("Index parent slot mismatch");
				}
			}
		}

		return true;
	}

	// Validate that all calibrator-tree nodes satisfy their [rho, tau] density bounds
	// This catches redistribution errors early during development
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_ValidateRedistributeDensityBounds
		(
			CPackedMapData const *_pData
			, mint _iStartSeg
			, mint _iEndSeg
			, mint _nTotalElements
			, mint _nGlobalLevels
			, ch8 const *_pLabel
		)
		const noexcept
	{
		auto const *pMeta = _pData->m_pSegmentMeta;
		mint nSegments = _iEndSeg - _iStartSeg;
		mint nLevels = fp_CalibratorLevelCount(nSegments);

		for (mint iLevel = 0; iLevel < nLevels; ++iLevel)
		{
			mint WindowSize = (mint)1 << iLevel;

			for (mint iWin = 0; iWin < nSegments; iWin += WindowSize)
			{
				mint iEnd = fg_Min(iWin + WindowSize, nSegments);
				mint nWinElements = 0;
				for (mint iSegment = iWin; iSegment < iEnd; ++iSegment)
					nWinElements += pMeta[_iStartSeg + iSegment].m_Count;

				mint WinCapacity = (iEnd - iWin) * mcp_SegmentSize;

				mint ScaledLowerBound, ScaledUpperBound;
				// iLevel is already the absolute calibrator level within the map:
				// level 0 = single segment, increasing toward root.
				fp_GetLevelThresholds(iLevel, _nGlobalLevels, ScaledLowerBound, ScaledUpperBound);

				mint nMinElems = fsp_ScaledMulCeil(ScaledLowerBound, WinCapacity);
				mint nMaxElems = fsp_ScaledMulFloor(ScaledUpperBound, WinCapacity);

				// Upper bound: allow 1-element slack for rounding
				if (nWinElements > nMaxElems + 1)
				{
					DMibConOut
						(
							"{} density violation (upper): level={} window=[{}, {}) elems={} cap={} scaledRho={} scaledTau={} min={} max={}\n",
							_pLabel, iLevel, iWin, iEnd, nWinElements, WinCapacity, ScaledLowerBound, ScaledUpperBound, nMinElems, nMaxElems
						)
					;
					return false;
				}

				// Check lower bound only when meaningful
				mint TotalCapacity = nSegments * mcp_SegmentSize;
				mint nWindowSegments = iEnd - iWin;
				mint nScaledExpectedWindowElements;
				bool bExpectedWindowCouldContainElement = fg_MultiplyOverflow
					(
						_nTotalElements
						, nWindowSegments
						, nScaledExpectedWindowElements) || (nScaledExpectedWindowElements >= nSegments
					)
				;
				bool bCheckLowerBound = (ScaledLowerBound > 0) && bExpectedWindowCouldContainElement && (_nTotalElements >= fsp_ScaledMulCeil(mcp_RootLowerBoundScaled, TotalCapacity));

				if (bCheckLowerBound && nWinElements > 0 && nWinElements + 1 < nMinElems)
				{
					DMibConOut
						(
							"{} density violation (lower): level={} window=[{}, {}) elems={} cap={} scaledRho={} scaledTau={} min={} max={}\n",
							_pLabel, iLevel, iWin, iEnd, nWinElements, WinCapacity, ScaledLowerBound, ScaledUpperBound, nMinElems, nMaxElems
						)
					;
					return false;
				}
			}
		}
		return true;
	}
}
