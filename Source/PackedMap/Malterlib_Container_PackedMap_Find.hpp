// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	// Find which segment a key belongs to using the multi-level static index
	// Uses top-down descent through B+-tree-like index structure
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr mint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_FindSegmentForKey
		(
			CPackedMapData const *_pData
			, t_CCompare const &_Compare
			, tf_CKey const &_Key
		)
		noexcept
	{
		fsp_StaticAssertNothrowCompareProbeToStoredKey<tf_CKey>();

		if (!_pData || _pData->m_nSegments == 0)
			return 0;

		mint nSegments = _pData->m_nSegments;
		auto const *pIndex = _pData->m_pIndex;
		auto const *pLevelOffsets = _pData->m_pLevelOffsets;
		uint16 nLevels = _pData->m_nStaticIndexLevels;

		// For single-level index, scan the leaf separators directly.
		if (nLevels == 1)
		{
			if (nSegments == 1)
				return 0;

			mint iLastValid = 0;
			bool bFoundValid = false;

			for (mint iEntry = 0; iEntry < nSegments; ++iEntry)
			{
				if (!_pData->f_GetIndexEntryValid(iEntry))
					continue;

				t_CKey const *pSeparatorKey;
				if constexpr (mcp_bIndexStoresKeys)
					pSeparatorKey = &pIndex[iEntry];
				else
					pSeparatorKey = &_pData->m_pKeys[pIndex[iEntry]];

				auto CompResult = _Compare(_Key, *pSeparatorKey);
				if (CompResult < 0)
					break;

				iLastValid = iEntry;
				bFoundValid = true;
			}

			return bFoundValid ? iLastValid : 0;
		}

		// Start from root (top level)
		mint iCurrent = 0;  // Index within current level

		// Descend from root to leaf level
		for (mint iLevel = nLevels - 1; iLevel > 0; --iLevel)
		{
			mint iChildLevel = iLevel - 1;
			mint iChildOffset = pLevelOffsets[iChildLevel];
			mint ChildSize = pLevelOffsets[iLevel] - iChildOffset;

			// Calculate children range for current index at this level
			mint iChildStart = iCurrent * mcp_Fanout;
			mint iChildEnd = fg_Min(iChildStart + mcp_Fanout, ChildSize);

			mint iLastValid = iChildStart;
			bool bFoundValid = false;

			// Linear scan within fanout to find last valid separator <= key
			{
				for (mint iChild = iChildStart; iChild < iChildEnd; ++iChild)
				{
					mint iEntry = iChildOffset + iChild;
					if (!_pData->f_GetIndexEntryValid(iEntry))
						continue;

					t_CKey const *pSeparatorKey;
					if constexpr (mcp_bIndexStoresKeys)
					{
						pSeparatorKey = &pIndex[iEntry];
					}
					else
					{
						pSeparatorKey = &_pData->m_pKeys[pIndex[iEntry]];
					}

					auto CompResult = _Compare(_Key, *pSeparatorKey);
					if (CompResult < 0)
						break;

					iLastValid = iChild;
					bFoundValid = true;
				}
			}

			if (!bFoundValid)
				iCurrent = iChildStart;
			else
				iCurrent = iLastValid;
		}

		// iCurrent is now the segment index
		return fg_Min(iCurrent, nSegments - 1);
	}

	// Find leftmost valid leaf in a subtree rooted at (_iLevel, _iEntry)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr mint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_FindLeftmostValidLeaf(CPackedMapData const *_pData, mint _iLevel, mint _iEntry) noexcept
	{
		if (!_pData)
			return 0;

		auto const *pLevelOffsets = _pData->m_pLevelOffsets;
		uint16 nLevels = _pData->m_nStaticIndexLevels;

		mint iLevel = _iLevel;
		mint iEntry = _iEntry;

		while (iLevel > 0)
		{
			mint iChildLevel = iLevel - 1;
			mint iChildOffset = pLevelOffsets[iChildLevel];
			mint iChildEndOffset = (iChildLevel + 1 < nLevels) ? pLevelOffsets[iChildLevel + 1] : _pData->m_nStaticIndexTotalEntries;
			mint ChildSize = iChildEndOffset - iChildOffset;
			mint iChildStart = iEntry * mcp_Fanout;
			mint iChildEnd = fg_Min(iChildStart + mcp_Fanout, ChildSize);

			bool bFound = false;
			for (mint iChild = iChildStart; iChild < iChildEnd; ++iChild)
			{
				mint iChildEntry = iChildOffset + iChild;
				if (_pData->f_GetIndexEntryValid(iChildEntry))
				{
					iEntry = iChild;
					iLevel = iChildLevel;
					bFound = true;
					break;
				}
			}

			if (!bFound)
				return _pData->m_nSegments;
		}

		return iEntry;
	}

	// Find rightmost valid leaf in a subtree rooted at (_iLevel, _iEntry)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr mint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_FindRightmostValidLeaf(CPackedMapData const *_pData, mint _iLevel, mint _iEntry) noexcept
	{
		if (!_pData)
			return 0;

		auto const *pLevelOffsets = _pData->m_pLevelOffsets;
		uint16 nLevels = _pData->m_nStaticIndexLevels;

		mint iLevel = _iLevel;
		mint iEntry = _iEntry;

		while (iLevel > 0)
		{
			mint iChildLevel = iLevel - 1;
			mint iChildOffset = pLevelOffsets[iChildLevel];
			mint iChildEndOffset = (iChildLevel + 1 < nLevels) ? pLevelOffsets[iChildLevel + 1] : _pData->m_nStaticIndexTotalEntries;
			mint ChildSize = iChildEndOffset - iChildOffset;
			mint iChildStart = iEntry * mcp_Fanout;
			mint iChildEnd = fg_Min(iChildStart + mcp_Fanout, ChildSize);

			bool bFound = false;
			for (mint iChild = iChildEnd; iChild-- > iChildStart; )
			{
				mint iChildEntry = iChildOffset + iChild;
				if (_pData->f_GetIndexEntryValid(iChildEntry))
				{
					iEntry = iChild;
					iLevel = iChildLevel;
					bFound = true;
					break;
				}
			}

			if (!bFound)
				return _pData->m_nSegments;
		}

		return iEntry;
	}

	// Find previous non-empty segment using the static index (O(log n))
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr mint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_FindPrevNonEmptySegment(CPackedMapData const *_pData, mint _iSegment) noexcept
	{
		if (!_pData || _iSegment <= 0)
			return _pData ? _pData->m_nSegments : 0;

		mint nSegments = _pData->m_nSegments;
		mint iLeaf = fg_Min(_iSegment - 1, nSegments - 1);
		auto const *pMeta = _pData->m_pSegmentMeta;

		uint16 nLevels = _pData->m_nStaticIndexLevels;
		auto const *pLevelOffsets = _pData->m_pLevelOffsets;

		if (_pData->f_GetIndexEntryValid(pLevelOffsets[0] + iLeaf))
			return (pMeta[iLeaf].m_Count > 0) ? iLeaf : nSegments;

		mint iLevel = 0;
		mint iIndex = iLeaf;

		while (true)
		{
			mint iParent = iIndex >> mcp_FanoutBits;
			mint iChildStart = iParent * mcp_Fanout;

			for (mint iChild = iIndex; iChild-- > iChildStart; )
			{
				mint iEntry = pLevelOffsets[iLevel] + iChild;
				if (!_pData->f_GetIndexEntryValid(iEntry))
					continue;

				mint iLeafResult = fsp_FindRightmostValidLeaf(_pData, iLevel, iChild);
				if (iLeafResult < nSegments && pMeta[iLeafResult].m_Count > 0)
					return iLeafResult;
				return nSegments;
			}

			if (iLevel + 1 >= nLevels)
				break;

			iIndex = iParent;
			++iLevel;
		}

		return nSegments;
	}

	// Find next non-empty segment using the static index (O(log n))
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr mint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_FindNextNonEmptySegment(CPackedMapData const *_pData, mint _iSegment) noexcept
	{
		if (!_pData)
			return 0;

		mint nSegments = _pData->m_nSegments;
		mint iLeaf = _iSegment + 1;
		if (iLeaf >= nSegments)
			return nSegments;
		auto const *pMeta = _pData->m_pSegmentMeta;

		uint16 nLevels = _pData->m_nStaticIndexLevels;
		auto const *pLevelOffsets = _pData->m_pLevelOffsets;

		if (_pData->f_GetIndexEntryValid(pLevelOffsets[0] + iLeaf))
			return (pMeta[iLeaf].m_Count > 0) ? iLeaf : nSegments;

		mint iLevel = 0;
		mint iIndex = iLeaf;

		while (true)
		{
			mint iParent = iIndex >> mcp_FanoutBits;
			mint iChildStart = iParent * mcp_Fanout;
			mint iLevelOffset = pLevelOffsets[iLevel];
			mint iLevelEndOffset = (iLevel + 1 < nLevels) ? pLevelOffsets[iLevel + 1] : _pData->m_nStaticIndexTotalEntries;
			mint LevelSize = iLevelEndOffset - iLevelOffset;
			mint iChildEnd = fg_Min(iChildStart + mcp_Fanout, LevelSize);

			for (mint iChild = iIndex + 1; iChild < iChildEnd; ++iChild)
			{
				mint iEntry = iLevelOffset + iChild;
				if (!_pData->f_GetIndexEntryValid(iEntry))
					continue;

				mint iLeafResult = fsp_FindLeftmostValidLeaf(_pData, iLevel, iChild);
				if (iLeafResult < nSegments && pMeta[iLeafResult].m_Count > 0)
					return iLeafResult;
				return nSegments;
			}

			if (iLevel + 1 >= nLevels)
				break;

			iIndex = iParent;
			++iLevel;
		}

		return nSegments;
	}

	// Binary search within a segment
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_BinarySearchInSegment
		(
			CPackedMapData const *_pData
			, t_CCompare const &_Compare
			, mint _iSegment
			, tf_CKey const &_Key
		)
		noexcept -> CSearchResult
	{
		fsp_StaticAssertNothrowCompareProbeToStoredKey<tf_CKey>();

		auto const *pMeta = _pData->m_pSegmentMeta;
		auto const *pKeys = _pData->m_pKeys;

		mint Count = pMeta[_iSegment].m_Count;
		if (Count == 0)
			return {0, false};

		mint iFirst = fsp_GetSegmentFirstSlot(_iSegment, Count);

		// Binary search within the segment
		mint iLeft = 0;
		mint iRight = Count;

		while (iLeft < iRight)
		{
			mint iMid = (iLeft + iRight) / 2;
			mint iSlot = iFirst + iMid;

			auto CompResult = _Compare(_Key, pKeys[iSlot]);
			if (CompResult < 0)
				iRight = iMid;
			else if (CompResult > 0)
				iLeft = iMid + 1;
			else
				return {iMid, true};
		}

		// Not found, return insertion position
		return {iLeft, false};
	}

	// Find a key's position (segment + local position)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_Find
		(
			CPackedMapData const *_pData
			, t_CCompare const &_Compare
			, tf_CKey const &_Key
		)
		noexcept -> CFindResult
	{
		if (!_pData)
			return {0, 0, 0, false};

		// Find segment
		mint iSegment = fsp_FindSegmentForKey(_pData, _Compare, _Key);

		// Binary search within segment
		auto SearchResult = fsp_BinarySearchInSegment(_pData, _Compare, iSegment, _Key);

		mint Count = _pData->m_pSegmentMeta[iSegment].m_Count;
		mint iFirst = Count > 0 ? fsp_GetSegmentFirstSlot(iSegment, Count) : fsp_GetSegmentStart(iSegment);
		mint iSlot = iFirst + SearchResult.m_iLocalPos;

		return {iSegment, SearchResult.m_iLocalPos, iSlot, SearchResult.m_bExists};
	}

	// Compute rank (sorted position) via calibrator tree prefix sum: O(log n)
	// Walk from leaf to root, adding left sibling counts when node is a right child
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr mint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_ComputeRank(CPackedMapData const *_pData, mint _iSegment, mint _iLocalPos) noexcept
	{
		DMibFastCheck(_pData);

		mint Rank = _iLocalPos;
		CCalibratorCount const *pCounts = _pData->m_pCalibratorCounts;
		mint const *pOffsets = _pData->m_pCalibratorOffsets;
		uint16 nLevels = _pData->m_nCalibratorTreeLevels;

		if (pCounts && nLevels > 0)
		{
			mint iNode = _iSegment;
			for (uint16 iLevel = 0; iLevel + 1 < nLevels; ++iLevel)
			{
				if (iNode & 1)
					Rank += (mint)pCounts[pOffsets[iLevel] + iNode - 1];
				iNode >>= 1;
			}
		}
		else
		{
			auto const *pMeta = _pData->m_pSegmentMeta;
			for (mint iSegment = 0; iSegment < _iSegment; ++iSegment)
				Rank += pMeta[iSegment].m_Count;
		}

		return Rank;
	}

	// Find absolute slot for the nth element in sorted order via calibrator tree descent: O(log n)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr mint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_FindSlotByRank(CPackedMapData const *_pData, mint _Rank) noexcept
	{
		DMibFastCheck(_pData);

		CCalibratorCount const *pCounts = _pData->m_pCalibratorCounts;
		mint const *pOffsets = _pData->m_pCalibratorOffsets;
		uint16 nLevels = _pData->m_nCalibratorTreeLevels;

		mint iSegment = 0;
		if (pCounts && nLevels > 1)
		{
			// Descend calibrator tree from root to find target segment
			for (uint16 iLevel = nLevels; iLevel-- > 1; )
			{
				mint iLeftChild = iSegment * 2;
				mint nLeftCount = (mint)pCounts[pOffsets[iLevel - 1] + iLeftChild];

				if (_Rank < nLeftCount)
					iSegment = iLeftChild;
				else
				{
					_Rank -= nLeftCount;
					iSegment = iLeftChild + 1;
				}
			}
		}

		mint Count = _pData->m_pSegmentMeta[iSegment].m_Count;
		return fsp_GetSegmentFirstSlot(iSegment, Count) + _Rank;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr mint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_FindEqualSlot
		(
			CPackedMapData const *_pData
			, t_CCompare const &_Compare
			, tf_CKey const &_Key
		)
		noexcept
	{
		fsp_StaticAssertNothrowCompareProbeToStoredKey<tf_CKey>();

		if (!_pData)
			return 0;

		mint iSegment = fsp_FindSegmentForKey(_pData, _Compare, _Key);
		auto const *pMeta = _pData->m_pSegmentMeta;
		mint Count = pMeta[iSegment].m_Count;
		if (Count == 0)
			return _pData->m_Capacity;

		auto const *pKeys = _pData->m_pKeys;
		mint iFirst = fsp_GetSegmentFirstSlot(iSegment, Count);
		mint iLeft = 0;
		mint iRight = Count;

		while (iLeft < iRight)
		{
			mint iMid = (iLeft + iRight) / 2;
			mint iSlot = iFirst + iMid;

			auto CompResult = _Compare(_Key, pKeys[iSlot]);
			if (CompResult < 0)
				iRight = iMid;
			else if (CompResult > 0)
				iLeft = iMid + 1;
			else
				return iSlot;
		}

		return _pData->m_Capacity;
	}

	// Check if key exists
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr bool TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Exists(tf_CKey &&_Key) const noexcept
	{
		auto *pData = mp_pData;
		if (!pData)
			return false;

		return fsp_FindEqualSlot(pData, mp_Compare, _Key) < pData->m_Capacity;
	}

	// Find equal (const version)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr t_CValue const *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindEqual(tf_CKey &&_Key) const noexcept
	{
		auto *pData = mp_pData;
		if (!pData)
			return nullptr;

		mint iSlot = fsp_FindEqualSlot(pData, mp_Compare, _Key);
		if (iSlot >= pData->m_Capacity)
			return nullptr;

		return &pData->m_pValues[iSlot];
	}

	// Find equal (non-const version)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr t_CValue *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindEqual(tf_CKey &&_Key) noexcept
	{
		auto *pData = mp_pData;
		if (!pData)
			return nullptr;

		mint iSlot = fsp_FindEqualSlot(pData, mp_Compare, _Key);
		if (iSlot >= pData->m_Capacity)
			return nullptr;

		return &pData->m_pValues[iSlot];
	}

	// Find smallest greater than or equal
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr t_CValue *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindSmallestGreaterThanEqual(tf_CKey &&_Key) noexcept
	{
		auto *pData = mp_pData;
		if (!pData)
			return nullptr;

		auto Result = fsp_Find(pData, mp_Compare, _Key);

		auto *pMeta = pData->m_pSegmentMeta;
		auto *pValues = pData->m_pValues;

		// If found exact match, return it
		if (Result.m_bExists)
			return &pValues[Result.m_iSlot];

		// Otherwise, Result.m_iLocalPos is the insertion position
		mint Count = pMeta[Result.m_iSegment].m_Count;

		// If insertion position is within segment, return element at that position
		if (Result.m_iLocalPos < Count)
		{
			mint iFirst = fsp_GetSegmentFirstSlot(Result.m_iSegment, Count);
			return &pValues[iFirst + Result.m_iLocalPos];
		}

		// Look for first element in the next non-empty segment via the static index
		mint iSeg = fsp_FindNextNonEmptySegment(pData, Result.m_iSegment);
		if (iSeg < pData->m_nSegments)
		{
			mint iFirst = fsp_GetSegmentFirstSlot(iSeg, pMeta[iSeg].m_Count);
			return &pValues[iFirst];
		}

		return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr t_CValue const *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindSmallestGreaterThanEqual(tf_CKey &&_Key) const noexcept
	{
		return const_cast<TCPackedMap *>(this)->f_FindSmallestGreaterThanEqual(fg_Forward<tf_CKey>(_Key));
	}

	// Find largest less than or equal
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr t_CValue *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindLargestLessThanEqual(tf_CKey &&_Key) noexcept
	{
		auto *pData = mp_pData;
		if (!pData)
			return nullptr;

		auto Result = fsp_Find(pData, mp_Compare, _Key);

		auto *pMeta = pData->m_pSegmentMeta;
		auto *pValues = pData->m_pValues;

		// If found exact match, return it
		if (Result.m_bExists)
			return &pValues[Result.m_iSlot];

		// Otherwise, Result.m_iLocalPos is insertion position, so previous element is largest <= key
		if (Result.m_iLocalPos > 0)
		{
			mint Count = pMeta[Result.m_iSegment].m_Count;
			mint iFirst = fsp_GetSegmentFirstSlot(Result.m_iSegment, Count);
			return &pValues[iFirst + Result.m_iLocalPos - 1];
		}

		// Look for the last element in the previous non-empty segment via the static index
		mint iSeg = fsp_FindPrevNonEmptySegment(pData, Result.m_iSegment);
		if (iSeg < pData->m_nSegments)
		{
			mint Count = pMeta[iSeg].m_Count;
			mint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
			return &pValues[iFirst + Count - 1];
		}

		return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey>
	constexpr t_CValue const *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindLargestLessThanEqual(tf_CKey &&_Key) const noexcept
	{
		return const_cast<TCPackedMap *>(this)->f_FindLargestLessThanEqual(fg_Forward<tf_CKey>(_Key));
	}

	// Find smallest
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr t_CValue *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindSmallest() noexcept
	{
		auto *pData = mp_pData;
		if (!pData || pData->m_nElements == 0)
			return nullptr;

		auto *pMeta = pData->m_pSegmentMeta;
		auto *pValues = pData->m_pValues;

		for (mint iSeg = 0; iSeg < pData->m_nSegments; ++iSeg)
		{
			if (pMeta[iSeg].m_Count > 0)
			{
				mint iFirst = fsp_GetSegmentFirstSlot(iSeg, pMeta[iSeg].m_Count);
				return &pValues[iFirst];
			}
		}

		return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr t_CValue const *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindSmallest() const noexcept
	{
		return const_cast<TCPackedMap *>(this)->f_FindSmallest();
	}

	// Find largest
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr t_CValue *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindLargest() noexcept
	{
		auto *pData = mp_pData;
		if (!pData || pData->m_nElements == 0)
			return nullptr;

		auto *pMeta = pData->m_pSegmentMeta;
		auto *pValues = pData->m_pValues;

		// Use safe reverse loop pattern for unsigned mint
		for (mint iSeg = pData->m_nSegments; iSeg-- > 0; )
		{
			if (pMeta[iSeg].m_Count > 0)
			{
				mint Count = pMeta[iSeg].m_Count;
				mint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
				return &pValues[iFirst + Count - 1];
			}
		}

		return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr t_CValue const *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindLargest() const noexcept
	{
		return const_cast<TCPackedMap *>(this)->f_FindLargest();
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr t_CKey const *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindSmallestKey() const noexcept
	{
		auto const *pData = mp_pData;
		if (!pData || pData->m_nElements == 0)
			return nullptr;

		auto const *pMeta = pData->m_pSegmentMeta;
		auto const *pKeys = pData->m_pKeys;

		for (mint iSeg = 0; iSeg < pData->m_nSegments; ++iSeg)
		{
			if (pMeta[iSeg].m_Count > 0)
			{
				mint iFirst = fsp_GetSegmentFirstSlot(iSeg, pMeta[iSeg].m_Count);
				return &pKeys[iFirst];
			}
		}

		return nullptr;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr t_CKey const *TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_FindLargestKey() const noexcept
	{
		auto const *pData = mp_pData;
		if (!pData || pData->m_nElements == 0)
			return nullptr;

		auto const *pMeta = pData->m_pSegmentMeta;
		auto const *pKeys = pData->m_pKeys;

		// Use safe reverse loop pattern for unsigned mint
		for (mint iSeg = pData->m_nSegments; iSeg-- > 0; )
		{
			if (pMeta[iSeg].m_Count > 0)
			{
				mint Count = pMeta[iSeg].m_Count;
				mint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
				return &pKeys[iFirst + Count - 1];
			}
		}

		return nullptr;
	}
}
