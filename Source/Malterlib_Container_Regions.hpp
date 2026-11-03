// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CRegionData>
	TCRegionData<t_CKey, t_CRegionData>::TCRegionData()
	{
	}

	template <typename t_CKey, typename t_CRegionData>
	TCRegionData<t_CKey, t_CRegionData>::TCRegionData(TCRegionData const &_Other)
		: t_CRegionData(_Other)
		, mp_End(_Other.mp_End)
	{
	}

	template <typename t_CKey, typename t_CRegionData>
	TCRegionData<t_CKey, t_CRegionData>::TCRegionData(TCRegionData &&_Other)
		: t_CRegionData(fg_Move(_Other))
		, mp_End(fg_Move(_Other.mp_End))
	{
	}

	template <typename t_CKey, typename t_CRegionData>
	t_CKey const &TCRegionData<t_CKey, t_CRegionData>::f_End() const
	{
		return mp_End;
	}

	template <typename t_CKey, typename t_CRegionData>
	t_CKey &TCRegionData<t_CKey, t_CRegionData>::f_End()
	{
		return mp_End;
	}

	template <typename t_CKey, typename t_CRegionData>
	t_CRegionData &TCRegionData<t_CKey, t_CRegionData>::f_Data()
	{
		return *this;
	}

	template <typename t_CKey, typename t_CRegionData>
	t_CRegionData const &TCRegionData<t_CKey, t_CRegionData>::f_Data() const
	{
		return *this;
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	TCRegions<t_CKey, t_CRegionData, t_CAllocator>::CRegionData::CRegionData()
	{
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	TCRegions<t_CKey, t_CRegionData, t_CAllocator>::CRegionData::CRegionData(CRegionData const &_Other)
		: TCRegionData<t_CKey, t_CRegionData>(_Other)
	{
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	TCRegions<t_CKey, t_CRegionData, t_CAllocator>::CRegionData::CRegionData(CRegionData &&_Other)
		: TCRegionData<t_CKey, t_CRegionData>(fg_Move(_Other))
	{
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	t_CKey const &TCRegions<t_CKey, t_CRegionData, t_CAllocator>::CRegionData::f_Start() const
	{
		return CMap::fs_GetKey(this);
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	void TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_Clear()
	{
		mp_Regions.f_Clear();
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	bool TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_IsEmpty() const
	{
		return mp_Regions.f_IsEmpty();
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	auto TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_GetIterator() -> CIterator
	{
		return mp_Regions.f_GetIterator();
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	auto TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_GetIterator() const -> CIteratorConst
	{
		return mp_Regions.f_GetIterator();
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	auto TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_GetIterator(t_CKey const &_Start) -> CIterator
	{
		auto Iter = mp_Regions.f_GetIterator_SmallestGreaterThanEqual(_Start);
		if (Iter == nullptr)
			Iter = mp_Regions.f_GetIterator();
		return Iter;
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	auto TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_GetIterator(t_CKey const &_Start) const -> CIteratorConst
	{
		auto Iter = mp_Regions.f_GetIterator_SmallestGreaterThanEqual(_Start);
		if (Iter == nullptr)
			Iter = mp_Regions.f_GetIterator();
		return Iter;
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	auto TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_GetIteratorLower(t_CKey const &_Start) -> CIterator
	{
		auto Iter = mp_Regions.f_GetIterator_LargestLessThanEqual(_Start);
		if (Iter == nullptr)
			Iter = mp_Regions.f_GetIterator();
		return Iter;
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	auto TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_GetIteratorLower(t_CKey const &_Start) const -> CIteratorConst
	{
		auto Iter = mp_Regions.f_GetIterator_LargestLessThanEqual(_Start);
		if (Iter == nullptr)
			Iter = mp_Regions.f_GetIterator();
		return Iter;
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	auto TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_GetData(t_CKey const &_Key) const -> CRegionData const *
	{
		auto *pValue = mp_Regions.f_FindLargestLessThanEqual(_Key);
		if (pValue && _Key < pValue->f_End())
			return pValue;
		return nullptr;
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	void TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_MakeRegion(t_CKey const &_Start, t_CKey const &_End)
	{
		f_MakeRegion(_Start, _End, CVoidFunctor());
	}

	template <typename t_CKey, typename t_CRegionData, typename t_CAllocator>
	template <class tf_CTransform>
	void TCRegions<t_CKey, t_CRegionData, t_CAllocator>::f_MakeRegion(t_CKey const &_Start, t_CKey const &_End, tf_CTransform const &_Transform)
	{
		t_CKey Search = _Start;
		--Search;
		if (Search > _Start)
			Search = _Start;

		DMibFastCheck(_Start < _End);
		DMibListLinkD_List_FromTemplate(CRegionData, m_Link) ToProcess;
		auto Iter = mp_Regions.f_GetIterator_LargestLessThanEqual(Search);
		if (Iter == nullptr)
			Iter = mp_Regions.f_GetIterator();
		if (Iter != nullptr)
		{
			auto *pSlice = Iter.f_GetCurrent();
			while (pSlice && pSlice->f_Start() <= _End)
			{
				ToProcess.f_Insert(pSlice);
				++Iter;
				pSlice = Iter.f_GetCurrent();
			}

			// Start by creating new regions
			{
				auto ProcessIter = ToProcess.f_GetIterator();

				t_CKey CurrentRegion = _Start;
				pSlice = ProcessIter;
				// DMibSafeCheck(pSlice && pSlice->f_End() >= CurrentRegion && pSlice->f_Start() <= CurrentRegion , "We should have a slice from start of time to end of time");
				while (ProcessIter && CurrentRegion < _End)
				{
					pSlice = ProcessIter;
					++ProcessIter;
					if (CurrentRegion >= pSlice->f_End())
						continue;

					if (CurrentRegion < pSlice->f_Start())
					{
						auto &New = mp_Regions[CurrentRegion];
						New.f_End() = fg_Min(pSlice->f_Start(), _End);
						_Transform(New.f_Data());
						CurrentRegion = New.f_End();
						ToProcess.f_InsertBefore(&New, pSlice);
						if (CurrentRegion == _End)
							break;
					}

					if (pSlice->f_Start() >= CurrentRegion && pSlice->f_End() <= _End)
					{
						// Slice is fully inclosed in current block, just transform
						_Transform(pSlice->f_Data());
						//DDTrace("0x{nfh,sj16,sf0} 0x{nfh,sj16,sf0} {}: Transform" DMibNewLine, pSlice->f_Start(), pSlice->f_End(), iOper++);
						CurrentRegion = pSlice->f_End();
					}
					else
					{
						if (CurrentRegion > pSlice->f_Start() && _End < pSlice->f_End())
						{
							//DDTrace("0x{nfh,sj16,sf0} 0x{nfh,sj16,sf0} {}: Split" DMibNewLine, pSlice->f_Start(), pSlice->f_End(), iOper++);
							// We are inside and need to split in three
							auto &New0 = mp_Regions[CurrentRegion];
							New0.f_End() = _End;
							New0.f_Data() = pSlice->f_Data();
							_Transform(New0.f_Data());
							ToProcess.f_InsertAfter(&New0, pSlice);

							auto &New1 = mp_Regions[_End];
							New1.f_End() = pSlice->f_End();
							pSlice->f_End() = CurrentRegion;
							New1.f_Data() = pSlice->f_Data();
							ToProcess.f_InsertAfter(&New1, &New0);

							CurrentRegion = _End;
						}
						else
						{
							if (CurrentRegion == pSlice->f_Start())
							{
								//DDTrace("0x{nfh,sj16,sf0} 0x{nfh,sj16,sf0} {}: Same start" DMibNewLine, pSlice->m_TimeStart, pSlice->f_End(), iOper++);
								DMibSafeCheck(_End != pSlice->f_End(), "");

								auto &New0 = mp_Regions[_End];
								New0.f_End() = pSlice->f_End();
								pSlice->f_End() = _End;
								New0.f_Data() = pSlice->f_Data();
								ToProcess.f_InsertAfter(&New0, pSlice);
								_Transform(pSlice->f_Data());
								CurrentRegion = pSlice->f_End();
							}
							else
							{
								//DDTrace("0x{nfh,sj16,sf0} 0x{nfh,sj16,sf0} {}: Same end" DMibNewLine, pSlice->m_TimeStart, pSlice->f_End(), iOper++);
								DMibSafeCheck(CurrentRegion != pSlice->f_Start(), "");

								auto &New0 = mp_Regions[CurrentRegion];
								New0.f_End() = pSlice->f_End();
								New0.f_Data() = pSlice->f_Data();
								_Transform(New0.f_Data());
								ToProcess.f_InsertAfter(&New0, pSlice);
								pSlice->f_End() = CurrentRegion;
								CurrentRegion = New0.f_End();
							}
						}
					}
				}

				if (CurrentRegion < _End)
				{
					// Just add off of end
					auto &New = mp_Regions[CurrentRegion];
					New.f_End() = _End;
					CurrentRegion = _End;
					_Transform(New.f_Data());
					ToProcess.f_Insert(&New);
				}
			}
		}
		else
		{
			//DMibSafeCheck(_Start == 0, "Must be whole block");
			// Just create the new region
			auto pSmallest = mp_Regions.f_FindSmallest();
			auto &New = mp_Regions[_Start];
			New.f_End() = _End;
			_Transform(New.f_Data());
			ToProcess.f_Insert(New);

			if (pSmallest)
				ToProcess.f_Insert(pSmallest);

			// DDTrace("New Region {}" DMibNewLine, iOper++);
		}

		// Merge regions
		{
			auto ProcessIter = ToProcess.f_GetIterator();

			auto pSlice = ProcessIter.f_GetCurrent();
			if (pSlice)
			{
				++ProcessIter;
				while (ProcessIter)
				{
					auto *pPrevSlice = pSlice;
					pSlice = ProcessIter;
					++ProcessIter;

					if (pPrevSlice->f_Data() == pSlice->f_Data() && pPrevSlice->f_End() == pSlice->f_Start())
					{
						// 	DDTrace("0x{nfh,sj16,sf0} 0x{nfh,sj16,sf0} {}: Merge" DMibNewLine, pPrevSlice->m_TimeStart, pSlice->f_End(), iOper++);
						// Merge
						pPrevSlice->f_End() = pSlice->f_End();
						mp_Regions.f_Remove(pSlice);
						pSlice = pPrevSlice;
					}
				}
			}
		}
	}
}
