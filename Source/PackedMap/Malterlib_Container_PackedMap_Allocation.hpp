// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	// Calculate size of validity bitmask for key-stored entries (1 bit per entry)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_IndexValidityArraySizeBytes(umint _nTotalEntries) noexcept
	{
		// Round up to whole bytes: (n + 7) / 8
		return (_nTotalEntries + 7) / 8;
	}

	// Compute the multi-level index layout for a given number of segments
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_ComputeIndexLayout(umint _nSegments) noexcept -> CIndexLayout
	{
		CIndexLayout Layout = {};

		if (_nSegments == 0)
			return Layout;

		// Level 0 has one entry per segment
		Layout.m_LevelSizes[0] = _nSegments;
		Layout.m_LevelOffsets[0] = 0;
		Layout.m_nLevels = 1;

		// Build upper levels until we reach a single root entry
		umint PrevSize = _nSegments;
		umint Offset = _nSegments;

		while (PrevSize > 1 && Layout.m_nLevels < mcp_MaxLevels)
		{
			// Next level size = ceil(prevSize / fanout)
			umint NextSize = (PrevSize + mcp_Fanout - 1) >> mcp_FanoutBits;

			Layout.m_LevelSizes[Layout.m_nLevels] = NextSize;
			Layout.m_LevelOffsets[Layout.m_nLevels] = Offset;

			Offset += NextSize;
			PrevSize = NextSize;
			++Layout.m_nLevels;
		}

		Layout.m_nTotalEntries = Offset;
		return Layout;
	}

	// Calculate the allocation size for a given number of segments
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_CalculateAllocSize(umint _nSegments, umint &o_Alignment) const noexcept
	{
		umint Capacity = _nSegments * mcp_SegmentSize;

		o_Alignment = fg_Max(alignof(CPackedMapData), alignof(CCalibratorCount), alignof(CSegmentMeta), alignof(umint), alignof(CSegmentMeta), alignof(t_CKey), alignof(t_CValue));

		// Start with header
		umint Size = sizeof(CPackedMapData);

		// Align for static index entries
		Size = fg_AlignUp(Size, alignof(CIndexEntry));
		Size += fsp_StaticIndexSizeBytes(_nSegments);

		// Align for calibrator tree counts
		Size = fg_AlignUp(Size, alignof(CCalibratorCount));
		Size += fp_CalibratorTreeSizeBytes(_nSegments);

		// Align for segment metadata
		Size = fg_AlignUp(Size, alignof(CSegmentMeta));
		Size += _nSegments * sizeof(CSegmentMeta);

		// Align for keys array
		Size = fg_AlignUp(Size, alignof(t_CKey));
		Size += Capacity * sizeof(t_CKey);

		// Align for values array
		Size = fg_AlignUp(Size, alignof(t_CValue));
		Size += Capacity * sizeof(t_CValue);

		return Size;
	}

	// Calculate the size of the static index in bytes (entries + level offsets + validity bitmask)
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr umint TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fsp_StaticIndexSizeBytes(umint _nSegments) noexcept
	{
		if (_nSegments == 0)
			return 0;

		CIndexLayout Layout = fsp_ComputeIndexLayout(_nSegments);

		// Size for all index entries
		umint Size = Layout.m_nTotalEntries * sizeof(CIndexEntry);

		// Size for level offsets array
		Size = fg_AlignUp(Size, alignof(umint));
		Size += Layout.m_nLevels * sizeof(umint);

		// Size for validity bitmask (only for key-stored entries)
		if constexpr (mcp_bIndexStoresKeys)
		{
			Size = fg_AlignUp(Size, alignof(uint8));
			Size += fsp_IndexValidityArraySizeBytes(Layout.m_nTotalEntries);
		}

		return Size;
	}

	// Set up the cached pointers after allocation
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_SetupCachedPointers(CPackedMapData *_pData) noexcept
	{
		umint nSegments = _pData->m_nSegments;
		umint Capacity = nSegments * mcp_SegmentSize;

		// Compute index layout
		CIndexLayout Layout = fsp_ComputeIndexLayout(nSegments);

		// Compute calibrator layout
		CCalibratorLayout CalibLayout = fp_ComputeCalibratorLayout(nSegments);

		// Store layout info in header
		_pData->m_nStaticIndexLevels = Layout.m_nLevels;
		_pData->m_nStaticIndexTotalEntries = Layout.m_nTotalEntries;

		_pData->m_nCalibratorTreeLevels = CalibLayout.m_nLevels;
		_pData->m_nCalibratorTreeTotalEntries = CalibLayout.m_nTotalEntries;

		// Calculate pointer positions
		uint8 *pBase = (uint8 *)_pData;
		umint Offset = sizeof(CPackedMapData);

		// Static index entries
		Offset = fg_AlignUp(Offset, (umint)alignof(CIndexEntry));
		_pData->m_pIndex = (CIndexEntry *)(pBase + Offset);
		Offset += Layout.m_nTotalEntries * sizeof(CIndexEntry);

		// Level offsets array
		Offset = fg_AlignUp(Offset, (umint)alignof(umint));
		_pData->m_pLevelOffsets = (umint *)(pBase + Offset);
		Offset += Layout.m_nLevels * sizeof(umint);

		// Fill level offsets
		for (uint16 iLevel = 0; iLevel < Layout.m_nLevels; ++iLevel)
			_pData->m_pLevelOffsets[iLevel] = Layout.m_LevelOffsets[iLevel];

		// Validity bitmask for key-stored entries
		if constexpr (mcp_bIndexStoresKeys)
		{
			Offset = fg_AlignUp(Offset, (umint)alignof(uint8));
			_pData->m_IndexValidStorage = (uint8 *)(pBase + Offset);
			Offset += fsp_IndexValidityArraySizeBytes(Layout.m_nTotalEntries);
		}

		// Calibrator tree counts
		Offset = fg_AlignUp(Offset, (umint)alignof(CCalibratorCount));
		_pData->m_pCalibratorCounts = (CCalibratorCount *)(pBase + Offset);
		Offset += CalibLayout.m_nTotalEntries * sizeof(CCalibratorCount);

		// Calibrator level offsets array
		Offset = fg_AlignUp(Offset, (umint)alignof(umint));
		_pData->m_pCalibratorOffsets = (umint *)(pBase + Offset);
		Offset += CalibLayout.m_nLevels * sizeof(umint);

		// Fill calibrator level offsets
		for (uint16 iLevel = 0; iLevel < CalibLayout.m_nLevels; ++iLevel)
			_pData->m_pCalibratorOffsets[iLevel] = CalibLayout.m_LevelOffsets[iLevel];

		// Segment metadata
		Offset = fg_AlignUp(Offset, (umint)alignof(CSegmentMeta));
		_pData->m_pSegmentMeta = (CSegmentMeta *)(pBase + Offset);
		Offset += nSegments * sizeof(CSegmentMeta);

		// Keys array
		Offset = fg_AlignUp(Offset, (umint)alignof(t_CKey));
		_pData->m_pKeys = (t_CKey *)(pBase + Offset);
		Offset += Capacity * sizeof(t_CKey);

		// Values array
		Offset = fg_AlignUp(Offset, (umint)alignof(t_CValue));
		_pData->m_pValues = (t_CValue *)(pBase + Offset);
	}

	// Allocate initial storage
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_AllocInitial(umint _nSegments)
	{
		using namespace NStr;
		DMibFastCheck(!mp_pData);

		umint nSegments = fg_Max(_nSegments, t_Options.m_MinSegments);
		if (nSegments < 1)
			nSegments = 1;

		// Enforce capacity limit from mc_MaxCapacityBits
		if (nSegments > (umint(1) << t_Options.m_MaxCapacityBits) / mcp_SegmentSize)
			DMibError("PackedMap capacity exceeded (mc_MaxCapacityBits = {})"_f << t_Options.m_MaxCapacityBits);

		umint Alignment = 0;
		umint AllocSize = fp_CalculateAllocSize(nSegments, Alignment);

		// Allocate memory
		void *pMemory = mp_Allocator.f_AllocAlignedWithSize(AllocSize, Alignment, EAllocationFlag_WillFreeWithSize);
		if (!pMemory)
			DMibErrorMemory("Failed to allocate PackedMap");

		// Initialize header
		mp_pData = new(pMemory) CPackedMapData();
		auto *pData = mp_pData;
		pData->m_AllocSize = AllocSize;
		pData->m_nSegments = nSegments;
		pData->m_Capacity = nSegments * mcp_SegmentSize;
		pData->m_nElements = 0;

		// Set up cached pointers
		fp_SetupCachedPointers(pData);

		// Initialize segment metadata
		for (umint iSegment = 0; iSegment < nSegments; ++iSegment)
		{
			new(&pData->m_pSegmentMeta[iSegment]) CSegmentMeta();
		}

		// Mark all index entries as empty
		if constexpr (mcp_bIndexStoresKeys)
		{
			// For key storage, clear validity bitmask (all entries invalid)
			umint nValidityBytes = fsp_IndexValidityArraySizeBytes(pData->m_nStaticIndexTotalEntries);
			NMemory::fg_MemClear(pData->m_IndexValidStorage, nValidityBytes);
		}
		else
		{
			// Slot index storage uses implicit-lifetime umint entries; assignment starts lifetime.
			for (umint iIndex = 0; iIndex < pData->m_nStaticIndexTotalEntries; ++iIndex)
				pData->m_pIndex[iIndex] = pData->m_Capacity;  // Invalid sentinel (umint)
		}

		// Initialize calibrator counts to zero (all segments empty)
		for (umint iEntry = 0; iEntry < pData->m_nCalibratorTreeTotalEntries; ++iEntry)
			pData->m_pCalibratorCounts[iEntry] = 0;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_ReserveForElementCount(umint _nElements)
	{
		if (_nElements <= 0)
			return;

		umint CapacityNeeded = fsp_ScaledDivCeil(_nElements, mcp_RootUpperBoundScaled);
		umint nRequiredSegments = (CapacityNeeded + mcp_SegmentSize - 1) / mcp_SegmentSize;
		nRequiredSegments = fg_Max(nRequiredSegments, t_Options.m_MinSegments);
		if (nRequiredSegments < 1)
			nRequiredSegments = 1;

		if (!mp_pData || mp_pData->m_Capacity < CapacityNeeded)
			fp_Resize(nRequiredSegments);
	}

	// Resize to a new number of segments
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_Resize(umint _nNewSegments)
	{
		using namespace NStr;
		if constexpr (t_Options.m_bStats)
			fp_StatsAddResize();

		if (_nNewSegments < t_Options.m_MinSegments)
			_nNewSegments = t_Options.m_MinSegments;
		if (_nNewSegments < 1)
			_nNewSegments = 1;

		// Enforce capacity limit from mc_MaxCapacityBits
		if (_nNewSegments > (umint(1) << t_Options.m_MaxCapacityBits) / mcp_SegmentSize)
			DMibError("PackedMap capacity exceeded (mc_MaxCapacityBits = {})"_f << t_Options.m_MaxCapacityBits);

		auto *pOldData = mp_pData;
		if (!pOldData)
		{
			fp_AllocInitial(_nNewSegments);
			return;
		}

		if (_nNewSegments == pOldData->m_nSegments)
			return;

		// Allocate new storage
		umint Alignment = 0;
		umint NewAllocSize = fp_CalculateAllocSize(_nNewSegments, Alignment);
		void *pNewMemory = mp_Allocator.f_AllocAlignedWithSize(NewAllocSize, Alignment, EAllocationFlag_WillFreeWithSize);
		if (!pNewMemory)
			DMibErrorMemory("Failed to resize PackedMap");

		// Initialize new header
		auto *pNewData = new(pNewMemory) CPackedMapData();
		pNewData->m_AllocSize = NewAllocSize;
		pNewData->m_nSegments = _nNewSegments;
		pNewData->m_Capacity = _nNewSegments * mcp_SegmentSize;
		pNewData->m_nElements = 0;
		if constexpr (t_Options.m_bAdaptive)
			pNewData->m_TimeCounter = pOldData->m_TimeCounter;

		fp_SetupCachedPointers(pNewData);

		// Initialize segment metadata
		for (umint iSegment = 0; iSegment < _nNewSegments; ++iSegment)
			new(&pNewData->m_pSegmentMeta[iSegment]) CSegmentMeta();

		// Mark all index entries as empty initially
		if constexpr (mcp_bIndexStoresKeys)
		{
			// For key storage, clear validity bitmask (all entries invalid)
			umint nValidityBytes = fsp_IndexValidityArraySizeBytes(pNewData->m_nStaticIndexTotalEntries);
			NMemory::fg_MemClear(pNewData->m_IndexValidStorage, nValidityBytes);
		}
		else
		{
			// Slot index storage uses implicit-lifetime umint entries; assignment starts lifetime.
			for (umint iEntry = 0; iEntry < pNewData->m_nStaticIndexTotalEntries; ++iEntry)
				pNewData->m_pIndex[iEntry] = pNewData->m_Capacity;  // Invalid sentinel (umint)
		}

		// Initialize calibrator counts to zero
		for (umint iEntry = 0; iEntry < pNewData->m_nCalibratorTreeTotalEntries; ++iEntry)
			pNewData->m_pCalibratorCounts[iEntry] = 0;

		// Guard pNewData until ownership is transferred to mp_pData
		auto NewDataGuard = g_OnScopeExit / [&]
			{
				pNewData->f_DestroyIndexEntries();
				for (umint iSegment = 0; iSegment < _nNewSegments; ++iSegment)
					pNewData->m_pSegmentMeta[iSegment].~CSegmentMeta();
				mp_Allocator.f_Free(pNewData, NewAllocSize);
			}
		;

		// Collect all elements from old storage
		umint nTotalElements = pOldData->m_nElements;
		if (nTotalElements > 0)
		{
			umint KeySize = nTotalElements * sizeof(t_CKey);
			umint ValueSize = nTotalElements * sizeof(t_CValue);
			CScratchAllocator ScratchAlloc;
			t_CKey *pTempKeys = (t_CKey *)ScratchAlloc.f_AllocAligned(KeySize, alignof(t_CKey));
			auto TempKeysGuard = g_OnScopeExit / [&]
				{
					ScratchAlloc.f_Free(pTempKeys, KeySize);
				}
			;
			t_CValue *pTempValues = (t_CValue *)ScratchAlloc.f_AllocAligned(ValueSize, alignof(t_CValue));
			TempKeysGuard.f_Clear();

			auto Cleanup = g_OnScopeExit / [&]
				{
					ScratchAlloc.f_Free(pTempKeys, KeySize);
					ScratchAlloc.f_Free(pTempValues, ValueSize);
				}
			;

			// Extract elements in key order
			umint iTemp = 0;
			if constexpr (mcp_bNothrowElementMove)
			{
				for (umint iSeg = 0; iSeg < pOldData->m_nSegments; ++iSeg)
				{
					umint Count = pOldData->m_pSegmentMeta[iSeg].m_Count;
					if (Count == 0)
						continue;

					umint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
					for (umint iEntry = 0; iEntry < Count; ++iEntry)
					{
						umint iSlot = iFirst + iEntry;
						new(&pTempKeys[iTemp]) t_CKey(fg_Move(pOldData->m_pKeys[iSlot]));
						new(&pTempValues[iTemp]) t_CValue(fg_Move(pOldData->m_pValues[iSlot]));
						if constexpr (t_Options.m_bStats)
							fp_StatsAddResizeMoves(1);
						pOldData->m_pKeys[iSlot].~t_CKey();
						pOldData->m_pValues[iSlot].~t_CValue();
						++iTemp;
					}

					pOldData->m_pSegmentMeta[iSeg].m_Count = 0;
				}
				pOldData->m_nElements = 0;
			}
			else
			{
				// Collect without destroying sources; on exception revert to old data
				auto CollectCleanup = g_OnScopeExit / [&]
					{
						for (umint i = 0; i < iTemp; ++i)
						{
							pTempKeys[i].~t_CKey();
							pTempValues[i].~t_CValue();
						}
						// NewDataGuard handles pNewData cleanup
					}
				;

				for (umint iSeg = 0; iSeg < pOldData->m_nSegments; ++iSeg)
				{
					umint Count = pOldData->m_pSegmentMeta[iSeg].m_Count;
					if (Count == 0)
						continue;

					umint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
					for (umint iEntry = 0; iEntry < Count; ++iEntry)
					{
						umint iSlot = iFirst + iEntry;
						new(&pTempKeys[iTemp]) t_CKey(fg_Move(pOldData->m_pKeys[iSlot]));
						auto KeyGuard = g_OnScopeExit / [&]
							{
								pTempKeys[iTemp].~t_CKey();
							}
						;
						new(&pTempValues[iTemp]) t_CValue(fg_Move(pOldData->m_pValues[iSlot]));
						KeyGuard.f_Clear();
						if constexpr (t_Options.m_bStats)
							fp_StatsAddResizeMoves(1);
						++iTemp;
					}
				}

				CollectCleanup.f_Clear();

				// Destroy sources now that all are safely in temp
				for (umint iSeg = 0; iSeg < pOldData->m_nSegments; ++iSeg)
				{
					umint Count = pOldData->m_pSegmentMeta[iSeg].m_Count;
					if (Count == 0)
						continue;

					umint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
					for (umint iEntry = 0; iEntry < Count; ++iEntry)
					{
						umint iSlot = iFirst + iEntry;
						pOldData->m_pKeys[iSlot].~t_CKey();
						pOldData->m_pValues[iSlot].~t_CValue();
					}
					pOldData->m_pSegmentMeta[iSeg].m_Count = 0;
				}
				pOldData->m_nElements = 0;
			}

			// Guard temp elements between collection and placement: if the Targets
			// allocation below throws, destroy collected temp elements so the
			// Cleanup guard can safely free the raw buffers.
			auto DistributeGuard = g_OnScopeExit / [&]
				{
					for (umint i = 0; i < iTemp; ++i)
					{
						pTempKeys[i].~t_CKey();
						pTempValues[i].~t_CValue();
					}
					iTemp = 0;
				}
			;

			// Distribute counts using density bounds (even redistribution)
			TCVector<umint, CScratchAllocator> Targets;
			Targets.f_SetLen(_nNewSegments);
			auto pTargets = Targets.f_GetArray();
			NMemory::fg_MemClear(pTargets, Targets.f_GetLen() * sizeof(umint));

			umint nGlobalLevels = fp_CalibratorLevelCount(_nNewSegments);
			umint iWindowLevel = (umint)(fg_GetHighestBitSet(_nNewSegments - 1) + 1);

			fp_DistributeElements(pTargets, 0, _nNewSegments, nTotalElements, 0, iWindowLevel, nGlobalLevels);

			DistributeGuard.f_Clear();

			// Place elements according to targets
			umint iCursor = 0;

			// On exception during placement, destroy remaining temp and clean up new allocation
			auto PlaceCleanup = g_OnScopeExit / [&]
				{
					if constexpr (!mcp_bNothrowElementMove)
					{
						// Destroy remaining temp elements
						for (umint i = iCursor; i < nTotalElements; ++i)
						{
							pTempKeys[i].~t_CKey();
							pTempValues[i].~t_CValue();
						}
						// Destroy placed elements in new storage (tracked by incremental m_Count)
						for (umint iSeg = 0; iSeg < _nNewSegments; ++iSeg)
						{
							umint SegCount = pNewData->m_pSegmentMeta[iSeg].m_Count;
							if (SegCount > 0)
							{
								umint iFirst = fsp_GetSegmentFirstSlot(iSeg, SegCount);
								for (umint iEntry = 0; iEntry < SegCount; ++iEntry)
								{
									pNewData->m_pKeys[iFirst + iEntry].~t_CKey();
									pNewData->m_pValues[iFirst + iEntry].~t_CValue();
								}
							}
						}
						// NewDataGuard handles pNewData cleanup
					}
				}
			;

			for (umint iSeg = 0; iSeg < _nNewSegments; ++iSeg)
			{
				umint Count = pTargets[iSeg];
				Count = fg_Min(Count, nTotalElements - iCursor);

				if (Count > 0)
				{
					umint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
					if constexpr (mcp_bNothrowElementMove)
					{
						for (umint iEntry = 0; iEntry < Count; ++iEntry)
						{
							umint iSlot = iFirst + iEntry;
							new(&pNewData->m_pKeys[iSlot]) t_CKey(fg_Move(pTempKeys[iCursor]));
							new(&pNewData->m_pValues[iSlot]) t_CValue(fg_Move(pTempValues[iCursor]));
							pTempKeys[iCursor].~t_CKey();
							pTempValues[iCursor].~t_CValue();
							++iCursor;
						}
						pNewData->m_pSegmentMeta[iSeg].m_Count = (uint16)Count;
					}
					else
					{
						pNewData->m_pSegmentMeta[iSeg].m_Count = (uint16)Count;
						umint nPlacedInSeg = 0;
						auto SegCleanup = g_OnScopeExit / [&]
							{
								for (umint i = 0; i < nPlacedInSeg; ++i)
								{
									pNewData->m_pKeys[iFirst + i].~t_CKey();
									pNewData->m_pValues[iFirst + i].~t_CValue();
								}
								pNewData->m_pSegmentMeta[iSeg].m_Count = 0;
							}
						;
						for (umint iEntry = 0; iEntry < Count; ++iEntry)
						{
							umint iSlot = iFirst + iEntry;
							new(&pNewData->m_pKeys[iSlot]) t_CKey(fg_Move(pTempKeys[iCursor]));
							auto KeyGuard = g_OnScopeExit / [&]
								{
									pNewData->m_pKeys[iSlot].~t_CKey();
								}
							;
							new(&pNewData->m_pValues[iSlot]) t_CValue(fg_Move(pTempValues[iCursor]));
							KeyGuard.f_Clear();
							pTempKeys[iCursor].~t_CKey();
							pTempValues[iCursor].~t_CValue();
							++nPlacedInSeg;
							++iCursor;
						}
						SegCleanup.f_Clear();
					}
				}
				else if constexpr (mcp_bNothrowElementMove)
					pNewData->m_pSegmentMeta[iSeg].m_Count = (uint16)Count;

				if constexpr (t_Options.m_bAdaptive)
					fp_ResetDetectorState(pNewData, iSeg);
			}

			PlaceCleanup.f_Clear();
		}

		// Switch to new data
		mp_pData = pNewData;
		pNewData->m_nElements = nTotalElements;
		NewDataGuard.f_Clear();

		pOldData->f_DestroyIndexEntries();

		// Destroy old segment metadata (needed for optional detector keys)
		for (umint iSegment = 0; iSegment < pOldData->m_nSegments; ++iSegment)
			pOldData->m_pSegmentMeta[iSegment].~CSegmentMeta();

		// Free old data
		mp_Allocator.f_Free(pOldData, pOldData->m_AllocSize);

		// Rebuild static index for new structure
		fp_RebuildStaticIndex(pNewData);

		// Initialize calibrator counts from segment metadata
		fp_InitializeCalibratorCounts(pNewData);
	}
}
