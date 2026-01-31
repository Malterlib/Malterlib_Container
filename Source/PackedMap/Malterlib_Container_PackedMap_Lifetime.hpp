// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	// Default constructor
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::TCPackedMap() noexcept = default;

	// Move constructor
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::TCPackedMap(TCPackedMap &&_Other) noexcept
		(
			NTraits::cIsNothrowMoveConstructible<t_CAllocator>
			&& NTraits::cIsNothrowMoveConstructible<t_CCompare>
		)
		: mp_Stats(fg_Move(_Other.mp_Stats))
		, mp_Allocator(fg_Move(_Other.mp_Allocator))
		, mp_Compare(fg_Move(_Other.mp_Compare))
		, mp_pData(fg_Exchange(_Other.mp_pData, nullptr))
	{
	}

	// Clear all elements, optionally keeping the backing allocation
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Clear(bool _bKeepBacking)
	{
		fp_Clear(_bKeepBacking, mp_Allocator);
	}

	// Clear all elements using an explicit allocator instance for the final free
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::fp_Clear(bool _bKeepBacking, t_CAllocator &_Allocator)
	{
		if (!mp_pData)
			return;

		// Destroy all elements
		auto *pData = mp_pData;
		t_CKey *pKeys = pData->m_pKeys;
		t_CValue *pValues = pData->m_pValues;
		CSegmentMeta *pMeta = pData->m_pSegmentMeta;

		for (mint iSeg = 0; iSeg < pData->m_nSegments; ++iSeg)
		{
			mint Count = pMeta[iSeg].m_Count;
			if (Count == 0)
			{
				if constexpr (t_Options.m_bAdaptive)
					fp_ResetDetectorState(pData, iSeg);

				continue;
			}

			mint iFirst = fsp_GetSegmentFirstSlot(iSeg, Count);
			for (mint iSegmentSlot = 0; iSegmentSlot < Count; ++iSegmentSlot)
			{
				mint iSlot = iFirst + iSegmentSlot;
				pKeys[iSlot].~t_CKey();
				pValues[iSlot].~t_CValue();
			}

			pMeta[iSeg].m_Count = 0;

			if constexpr (t_Options.m_bAdaptive)
				fp_ResetDetectorState(pData, iSeg);
		}

		pData->m_nElements = 0;

		if (_bKeepBacking)
		{
			// Reset the static index to the fully empty state before keeping the allocation alive.
			fp_RebuildStaticIndex(pData);

			// Reset calibrator counts (all segments now empty)
			fp_InitializeCalibratorCounts(pData);
		}
		else
		{
			// Destroy segment metadata (needed for optional detector keys)
			for (mint iSegmentSlot = 0; iSegmentSlot < pData->m_nSegments; ++iSegmentSlot)
				pData->m_pSegmentMeta[iSegmentSlot].~CSegmentMeta();

			pData->f_DestroyIndexEntries();

			// Free the allocation
			_Allocator.f_Free(pData, pData->m_AllocSize);
			mp_pData = nullptr;
		}
	}

	// Destructor
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::~TCPackedMap()
	{
		f_Clear();
	}

	// Copy constructor
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::TCPackedMap(TCPackedMap const &_Other)
		: mp_Allocator(_Other.mp_Allocator)
		, mp_Compare(_Other.mp_Compare)
	{
		if (!_Other.mp_pData || _Other.f_GetLen() == 0)
			return;

		// Allocate same number of segments
		fp_Resize(_Other.mp_pData->m_nSegments);

		// Copy elements
		auto *pData = mp_pData;
		auto const *pOtherData = _Other.mp_pData;

		// Track the current segment and how many elements were placed in it for cleanup
		mint iCleanupSeg = 0;
		mint nCleanupPlacedInSeg = 0;

		// If copy throws, destructor won't run (C++ rule for constructors), so clean up manually
		auto CtorCleanup = g_OnScopeExit / [&]
			{
				if constexpr (!mcp_bNothrowElementCopy)
				{
					// Destroy elements in fully completed segments (m_Count already set)
					for (mint iSeg = 0; iSeg < iCleanupSeg; ++iSeg)
					{
						mint SegCount = pData->m_pSegmentMeta[iSeg].m_Count;
						if (SegCount == 0)
							continue;
						mint iSegFirst = fsp_GetSegmentFirstSlot(iSeg, SegCount);
						for (mint i = 0; i < SegCount; ++i)
						{
							pData->m_pKeys[iSegFirst + i].~t_CKey();
							pData->m_pValues[iSegFirst + i].~t_CValue();
						}
					}
					// Destroy partially placed elements in current segment
					// They were placed at positions based on the FULL count layout
					if (nCleanupPlacedInSeg > 0)
					{
						mint FullCount = pOtherData->m_pSegmentMeta[iCleanupSeg].m_Count;
						mint iSegFirst = fsp_GetSegmentFirstSlot(iCleanupSeg, FullCount);
						for (mint i = 0; i < nCleanupPlacedInSeg; ++i)
						{
							pData->m_pKeys[iSegFirst + i].~t_CKey();
							pData->m_pValues[iSegFirst + i].~t_CValue();
						}
					}
					// Destroy segment metadata and index entries, free allocation
					for (mint iSeg = 0; iSeg < pData->m_nSegments; ++iSeg)
						pData->m_pSegmentMeta[iSeg].~CSegmentMeta();
					pData->f_DestroyIndexEntries();
					mp_Allocator.f_Free(pData, pData->m_AllocSize);
					mp_pData = nullptr;
				}
			}
		;

		for (mint iSeg = 0; iSeg < pOtherData->m_nSegments; ++iSeg)
		{
			mint Count = pOtherData->m_pSegmentMeta[iSeg].m_Count;
			if (Count == 0)
				continue;

			mint iFirst = _Other.fsp_GetSegmentFirstSlot(iSeg, Count);
			mint iMyFirst = fsp_GetSegmentFirstSlot(iSeg, Count);

			if constexpr (mcp_bNothrowElementCopy)
			{
				for (mint iSegmentSlot = 0; iSegmentSlot < Count; ++iSegmentSlot)
				{
					new(&pData->m_pKeys[iMyFirst + iSegmentSlot]) t_CKey(pOtherData->m_pKeys[iFirst + iSegmentSlot]);
					new(&pData->m_pValues[iMyFirst + iSegmentSlot]) t_CValue(pOtherData->m_pValues[iFirst + iSegmentSlot]);
				}
			}
			else
			{
				iCleanupSeg = iSeg;
				nCleanupPlacedInSeg = 0;

				for (mint iSegmentSlot = 0; iSegmentSlot < Count; ++iSegmentSlot)
				{
					new(&pData->m_pKeys[iMyFirst + iSegmentSlot]) t_CKey(pOtherData->m_pKeys[iFirst + iSegmentSlot]);
					auto KeyGuard = g_OnScopeExit / [&]
						{
							pData->m_pKeys[iMyFirst + iSegmentSlot].~t_CKey();
						}
					;
					new(&pData->m_pValues[iMyFirst + iSegmentSlot]) t_CValue(pOtherData->m_pValues[iFirst + iSegmentSlot]);
					KeyGuard.f_Clear();
					++nCleanupPlacedInSeg;
				}
			}
			pData->m_pSegmentMeta[iSeg].m_Count = (uint16)Count;
			pData->m_nElements += Count;
		}

		CtorCleanup.f_Clear();
		fp_RebuildStaticIndex(pData);
		fp_InitializeCalibratorCounts(pData);
	}

	// Allocator construction
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename ...tfp_CParams>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::TCPackedMap(CAllocatorConstructTag const &, tfp_CParams && ...p_Params)
		: mp_Allocator(fg_Forward<tfp_CParams>(p_Params)...)
	{
	}

	// Compare construction
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename ...tfp_CParams>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::TCPackedMap(CCompareConstructTag const &, tfp_CParams && ...p_Params)
		: mp_Compare(fg_Forward<tfp_CParams>(p_Params)...)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename ...tfp_CAllocatorParams, typename ...tfp_CCompareParams>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::TCPackedMap
		(
			CAllocatorConstructTag &&_AlloctatorTag
			, CCompareConstructTag &&_CompareTag
			, TCConstruct<void, tfp_CAllocatorParams...> &&_ConstructAllocator
			, TCConstruct<void, tfp_CCompareParams...> &&_ConstructCompare
		)
		: TCPackedMap
		(
			NMeta::TCConsecutiveIndices<sizeof...(tfp_CAllocatorParams)>()
			, NMeta::TCConsecutiveIndices<sizeof...(tfp_CCompareParams)>()
			, fg_Move(_AlloctatorTag)
			, fg_Move(_CompareTag)
			, fg_Move(_ConstructAllocator)
			, fg_Move(_ConstructCompare)
		)
	{
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename ...tfp_CAllocatorParams, typename ...tfp_CCompareParams, mint ...tp_IndicesAllocator, mint ...tp_IndicesCompare>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::TCPackedMap
		(
			NMeta::TCIndices<tp_IndicesAllocator...> const &_IndexSequenceAllocator
			, NMeta::TCIndices<tp_IndicesCompare...> const &_IndexSequuenceCompare
			, CAllocatorConstructTag &&
			, CCompareConstructTag &&
			, TCConstruct<void, tfp_CAllocatorParams...> &&_ConstructAllocator
			, TCConstruct<void, tfp_CCompareParams...> &&_ConstructCompare
		)
		: mp_Allocator(fg_Forward<tfp_CAllocatorParams>(fg_Get<tp_IndicesAllocator>(_ConstructAllocator.m_Params))...)
		, mp_Compare(fg_Forward<tfp_CCompareParams>(fg_Get<tp_IndicesCompare>(_ConstructCompare.m_Params))...)
	{
	}

	// Copy constructor from different template parameters
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::TCPackedMap(TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> const &_Other)
		: mp_Allocator
		(
			[&]() -> t_CAllocator
			{
				if constexpr (NTraits::cIsSame<t_CAllocator, tf_CAllocator>)
					return _Other.mp_Allocator;
				else
					return {};
			}
			()
		)
		, mp_Compare
		(
			[&]() -> t_CCompare
			{
				if constexpr (NTraits::cIsSame<t_CCompare, tf_CCompare>)
					return _Other.mp_Compare;
				else
					return {};
			}
			()
		)
	{
		if (_Other.f_GetLen() == 0)
			return;

		auto CtorCleanup = g_OnScopeExit / [&]
			{
				if (mp_pData)
					f_Clear(false);
			}
		;

		if constexpr (NTraits::cIsSame<t_CKey, tf_CKey> && NTraits::cIsSame<t_CCompare, tf_CCompare>)
		{
			// Reuse the source order only when both key type and comparator state match.
			f_BulkLoad
				(
					_Other.f_GetLen()
					, [&](auto &&_fInsert) noexcept(noexcept(_fInsert(fg_GetType<tf_CKey const &>(), fg_GetType<tf_CValue const &>())))
					{
						for (auto Iter = _Other.f_Entries().f_GetIterator(); Iter; ++Iter)
						{
							auto Ref = *Iter;
							_fInsert(Ref.f_Key(), Ref.f_Value());
						}
					}
				)
			;
		}
		else
		{
			fp_ReserveForElementCount(_Other.f_GetLen());

			for (auto Iter = _Other.f_Entries().f_GetIterator(); Iter; ++Iter)
			{
				auto Ref = *Iter;
				if constexpr (NTraits::cIsSame<t_CKey, tf_CKey>)
					f_Insert(Ref.f_Key(), Ref.f_Value());
				else
					f_Insert(t_CKey(Ref.f_Key()), Ref.f_Value());
			}
		}

		CtorCleanup.f_Clear();
	}

	// Move constructor from different template parameters
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
	constexpr TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::TCPackedMap(TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> &&_Other)
		: mp_Allocator
		(
			[&]() -> t_CAllocator
			{
				if constexpr (NTraits::cIsSame<t_CAllocator, tf_CAllocator>)
					return fg_Move(_Other.mp_Allocator);
				else
					return {};
			}
			()
		)
		, mp_Compare
		(
			[&]() -> t_CCompare
			{
				if constexpr (NTraits::cIsSame<t_CCompare, tf_CCompare>)
					return fg_Move(_Other.mp_Compare);
				else
					return {};
			}
			()
		)
	{
		if (_Other.f_GetLen() == 0)
			return;

		// At this point comparator state has already been moved from _Other.
		// From here on the source must be treated as invalid, so clear it even if transfer throws.
		// This because we move the keys from the source, so it will be left in an indeterminate state otherwise
		auto ClearOther = g_OnScopeExit / [&]
			{
				if constexpr (NTraits::cIsSame<t_CAllocator, tf_CAllocator>)
					_Other.fp_Clear(false, mp_Allocator);
				else
					_Other.f_Clear();
			}
		;

		auto CtorCleanup = g_OnScopeExit / [&]
			{
				if (mp_pData)
					f_Clear(false);
			}
		;

		if constexpr (NTraits::cIsSame<t_CKey, tf_CKey> && NTraits::cIsSame<t_CCompare, tf_CCompare>)
		{
			// Reuse the source order only when both key type and comparator state match.
			f_BulkLoad
				(
					_Other.f_GetLen()
					, [&](auto &&_fInsert) noexcept(noexcept(_fInsert(fg_GetType<tf_CKey &&>(), fg_GetType<tf_CValue &&>())))
					{
						for (auto Iter = _Other.f_Entries().f_GetIterator(); Iter; ++Iter)
						{
							auto Ref = *Iter;
							_fInsert(fg_Move(const_cast<tf_CKey &>(Ref.f_Key())), fg_Move(Ref.f_Value()));
						}
					}
				)
			;
		}
		else
		{
			fp_ReserveForElementCount(_Other.f_GetLen());

			for (auto Iter = _Other.f_Entries().f_GetIterator(); Iter; ++Iter)
			{
				auto Ref = *Iter;
				if constexpr (NTraits::cIsSame<t_CKey, tf_CKey>)
					f_Insert(fg_Move(const_cast<tf_CKey &>(Ref.f_Key())), fg_Move(Ref.f_Value()));
				else
					f_Insert(t_CKey(fg_Move(const_cast<tf_CKey &>(Ref.f_Key()))), fg_Move(Ref.f_Value()));
			}
		}

		CtorCleanup.f_Clear();
	}

	// Copy assignment
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::operator = (TCPackedMap const &_Other) -> TCPackedMap &
	{
		if (this == &_Other)
			return *this;

		mp_Compare = _Other.mp_Compare;
		f_Clear(false);

		if (!_Other.mp_pData || _Other.f_GetLen() == 0)
			return *this;

		// Resize to match
		if (!mp_pData || mp_pData->m_nSegments != _Other.mp_pData->m_nSegments)
			fp_Resize(_Other.mp_pData->m_nSegments);

		// Copy elements
		auto *pData = mp_pData;
		auto const *pOtherData = _Other.mp_pData;

		// Track placement for exception cleanup (same position issue as copy ctor)
		mint iAssignCleanupSeg = 0;
		mint nAssignCleanupPlacedInSeg = 0;

		auto AssignCleanup = g_OnScopeExit / [&]
			{
				if constexpr (!mcp_bNothrowElementCopy)
				{
					// Destroy fully completed segments
					for (mint iSeg = 0; iSeg < iAssignCleanupSeg; ++iSeg)
					{
						mint SegCount = pData->m_pSegmentMeta[iSeg].m_Count;
						if (SegCount == 0)
							continue;
						mint iSegFirst = fsp_GetSegmentFirstSlot(iSeg, SegCount);
						for (mint i = 0; i < SegCount; ++i)
						{
							pData->m_pKeys[iSegFirst + i].~t_CKey();
							pData->m_pValues[iSegFirst + i].~t_CValue();
						}
						pData->m_pSegmentMeta[iSeg].m_Count = 0;
					}
					// Destroy partially placed elements in current segment
					if (nAssignCleanupPlacedInSeg > 0)
					{
						mint FullCount = pOtherData->m_pSegmentMeta[iAssignCleanupSeg].m_Count;
						mint iSegFirst = fsp_GetSegmentFirstSlot(iAssignCleanupSeg, FullCount);
						for (mint i = 0; i < nAssignCleanupPlacedInSeg; ++i)
						{
							pData->m_pKeys[iSegFirst + i].~t_CKey();
							pData->m_pValues[iSegFirst + i].~t_CValue();
						}
					}
					pData->m_nElements = 0;
				}
			}
		;

		for (mint iSeg = 0; iSeg < pOtherData->m_nSegments; ++iSeg)
		{
			mint Count = pOtherData->m_pSegmentMeta[iSeg].m_Count;
			if (Count == 0)
				continue;

			mint iFirst = _Other.fsp_GetSegmentFirstSlot(iSeg, Count);
			mint iMyFirst = fsp_GetSegmentFirstSlot(iSeg, Count);

			if constexpr (mcp_bNothrowElementCopy)
			{
				for (mint iSegmentSlot = 0; iSegmentSlot < Count; ++iSegmentSlot)
				{
					new(&pData->m_pKeys[iMyFirst + iSegmentSlot]) t_CKey(pOtherData->m_pKeys[iFirst + iSegmentSlot]);
					new(&pData->m_pValues[iMyFirst + iSegmentSlot]) t_CValue(pOtherData->m_pValues[iFirst + iSegmentSlot]);
				}
			}
			else
			{
				iAssignCleanupSeg = iSeg;
				nAssignCleanupPlacedInSeg = 0;

				for (mint iSegmentSlot = 0; iSegmentSlot < Count; ++iSegmentSlot)
				{
					new(&pData->m_pKeys[iMyFirst + iSegmentSlot]) t_CKey(pOtherData->m_pKeys[iFirst + iSegmentSlot]);
					auto KeyGuard = g_OnScopeExit / [&]
						{
							pData->m_pKeys[iMyFirst + iSegmentSlot].~t_CKey();
						}
					;
					new(&pData->m_pValues[iMyFirst + iSegmentSlot]) t_CValue(pOtherData->m_pValues[iFirst + iSegmentSlot]);
					KeyGuard.f_Clear();
					++nAssignCleanupPlacedInSeg;
				}

			}
			pData->m_pSegmentMeta[iSeg].m_Count = (uint16)Count;
			pData->m_nElements += Count;
		}

		AssignCleanup.f_Clear();

		fp_RebuildStaticIndex(pData);
		fp_InitializeCalibratorCounts(pData);

		return *this;
	}

	// Move assignment
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::operator = (TCPackedMap &&_Other) -> TCPackedMap &
	{
		if (this == &_Other)
			return *this;

		f_Clear();

		mp_Stats = fg_Move(_Other.mp_Stats);
		mp_Allocator = fg_Move(_Other.mp_Allocator);
		mp_Compare = fg_Move(_Other.mp_Compare);
		mp_pData = fg_Exchange(_Other.mp_pData, nullptr);

		return *this;
	}

	// Copy assignment from different template parameters
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::operator = (TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> const &_Other)
		-> TCPackedMap &
	{
		if constexpr (NTraits::cIsSame<t_CCompare, tf_CCompare>)
			mp_Compare = _Other.mp_Compare;

		if constexpr (NTraits::cIsSame<t_CKey, tf_CKey> && NTraits::cIsSame<t_CCompare, tf_CCompare>)
		{
			// f_BulkLoad calls f_Clear() internally
			f_BulkLoad
				(
					_Other.f_GetLen()
					, [&](auto &&_fInsert) noexcept(noexcept(_fInsert(fg_GetType<tf_CKey const &>(), fg_GetType<tf_CValue const &>())))
					{
						for (auto Iter = _Other.f_Entries().f_GetIterator(); Iter; ++Iter)
						{
							auto Ref = *Iter;
							_fInsert(Ref.f_Key(), Ref.f_Value());
						}
					}
				)
			;
		}
		else
		{
			f_Clear(false);
			fp_ReserveForElementCount(_Other.f_GetLen());

			for (auto Iter = _Other.f_Entries().f_GetIterator(); Iter; ++Iter)
			{
				auto Ref = *Iter;
				if constexpr (NTraits::cIsSame<t_CKey, tf_CKey>)
					f_Insert(Ref.f_Key(), Ref.f_Value());
				else
					f_Insert(t_CKey(Ref.f_Key()), Ref.f_Value());
			}
		}

		return *this;
	}

	// Move assignment from different template parameters
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator, CPackedMapOptions tf_Options>
	constexpr auto TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::operator = (TCPackedMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator, tf_Options> &&_Other)
		-> TCPackedMap &
	{
		if constexpr (NTraits::cIsSame<t_CCompare, tf_CCompare>)
			mp_Compare = fg_Move(_Other.mp_Compare);

		// At this point comparator state has already been moved from _Other.
		// From here on the source must be treated as invalid, so clear it even if transfer throws.
		// This because we move the keys from the source, so it will be left in an indeterminate state otherwise
		auto ClearOther = g_OnScopeExit / [&]
			{
				_Other.f_Clear();
			}
		;

		if constexpr (NTraits::cIsSame<t_CKey, tf_CKey> && NTraits::cIsSame<t_CCompare, tf_CCompare>)
		{
			// f_BulkLoad calls f_Clear() internally
			f_BulkLoad
				(
					_Other.f_GetLen()
					, [&](auto &&_fInsert) noexcept(noexcept(_fInsert(fg_GetType<tf_CKey &&>(), fg_GetType<tf_CValue &&>())))
					{
						for (auto Iter = _Other.f_Entries().f_GetIterator(); Iter; ++Iter)
						{
							auto Ref = *Iter;
							_fInsert(fg_Move(const_cast<tf_CKey &>(Ref.f_Key())), fg_Move(Ref.f_Value()));
						}
					}
				)
			;
		}
		else
		{
			f_Clear(false);
			fp_ReserveForElementCount(_Other.f_GetLen());

			for (auto Iter = _Other.f_Entries().f_GetIterator(); Iter; ++Iter)
			{
				auto Ref = *Iter;
				if constexpr (NTraits::cIsSame<t_CKey, tf_CKey>)
					f_Insert(fg_Move(const_cast<tf_CKey &>(Ref.f_Key())), fg_Move(Ref.f_Value()));
				else
					f_Insert(t_CKey(fg_Move(const_cast<tf_CKey &>(Ref.f_Key()))), fg_Move(Ref.f_Value()));
			}
		}

		return *this;
	}
}
