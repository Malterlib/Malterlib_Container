// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Container/Map>

namespace NMib
{
	namespace NContainer
	{

		template <typename t_CKey, typename t_CRegionData = NMib::CVoidTag, typename t_CAllocator = NMem::CAllocator_Heap>
		class TCRegions
		{
			class CRegionData;
			typedef TCMap<t_CKey, CRegionData, CSort_Default, t_CAllocator> CMap;
			class CRegionData : public t_CRegionData
			{
				t_CKey m_End;
			public:
				CRegionData()
				{
				}
				CRegionData(CRegionData const &_Other)
					: t_CRegionData(_Other)
					, m_End(_Other.m_End)
				{
				}
				CRegionData(CRegionData &&_Other)
					: t_CRegionData(fg_Move(_Other))
					, m_End(fg_Move(_Other.m_End))
				{
				}
				DMibListLinkD_Link(CRegionData, m_Link);
				t_CKey const &f_End() const
				{
					return m_End;
				}
				t_CKey &f_End()
				{
					return m_End;
				}

				t_CKey const &f_Start() const
				{
					return CMap::fs_GetKey(this);
				}
				t_CRegionData &f_Data()
				{
					return *this;
				}
				t_CRegionData const &f_Data() const
				{
					return *this;
				}
			};
			CMap m_Regions;
		public:

			void f_Clear()
			{
				m_Regions.f_Clear();
			}
			bool f_IsEmpty() const
			{
				return m_Regions.f_IsEmpty();
			}
			typename CMap::CIterator f_GetIterator()
			{
				return m_Regions.f_GetIterator();
			}
			typename CMap::CIteratorConst f_GetIterator() const
			{
				return m_Regions.f_GetIterator();
			}

			typename CMap::CIterator f_GetIterator(const t_CKey &_Start)
			{
				auto Iter = m_Regions.f_GetIterator_SmallestGreaterThanEqual(_Start);
				if (Iter == nullptr)
					Iter = m_Regions.f_GetIterator();
				return Iter;
			}
			typename CMap::CIteratorConst f_GetIterator(const t_CKey &_Start) const
			{
				auto Iter = m_Regions.f_GetIterator_SmallestGreaterThanEqual(_Start);
				if (Iter == nullptr)
					Iter = m_Regions.f_GetIterator();
				return Iter;
			}

			typename CMap::CIterator f_GetIteratorLower(const t_CKey &_Start)
			{
				auto Iter = m_Regions.f_GetIterator_LargestLessThanEqual(_Start);
				if (Iter == nullptr)
					Iter = m_Regions.f_GetIterator();
				return Iter;
			}
			typename CMap::CIteratorConst f_GetIteratorLower(const t_CKey &_Start) const
			{
				auto Iter = m_Regions.f_GetIterator_LargestLessThanEqual(_Start);
				if (Iter == nullptr)
					Iter = m_Regions.f_GetIterator();
				return Iter;
			}

			void f_MakeRegion(const t_CKey &_Start, const t_CKey &_End)
			{
				f_MakeRegion(_Start, _End, CVoidFunctor());
			}
			template <class t_CTransform>
			void f_MakeRegion(const t_CKey &_Start, const t_CKey &_End, t_CTransform const &_Transform)
			{
				t_CKey Search = _Start;
				--Search;
				if (Search > _Start)
					Search = _Start;
				
				DMibFastCheck(_Start < _End);
				DMibListLinkD_List_FromTemplate(CRegionData, m_Link) ToProcess;
				auto Iter = m_Regions.f_GetIterator_LargestLessThanEqual(Search);
				if (Iter == nullptr)
					Iter = m_Regions.f_GetIterator();
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
//						DMibSafeCheck(pSlice && pSlice->f_End() >= CurrentRegion && pSlice->f_Start() <= CurrentRegion , "We should have a slice from start of time to end of time");
						while (ProcessIter && CurrentRegion < _End)
						{
							pSlice = ProcessIter;
							++ProcessIter;
							if (CurrentRegion >= pSlice->f_End())
								continue;

							if (CurrentRegion < pSlice->f_Start())
							{
								auto &New = m_Regions[CurrentRegion];
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
								//DDTrace("0x{nfh,sj16,sf0} 0x{nfh,sj16,sf0} {}: Transform" DMibNewLine, pSlice->f_Start() << pSlice->f_End() << iOper++);
								CurrentRegion = pSlice->f_End();
							}
							else
							{
								if (CurrentRegion > pSlice->f_Start() && _End < pSlice->f_End())
								{
									//DDTrace("0x{nfh,sj16,sf0} 0x{nfh,sj16,sf0} {}: Split" DMibNewLine, pSlice->f_Start() << pSlice->f_End() << iOper++);
									// We are inside and need to split in three
									auto &New0 = m_Regions[CurrentRegion];
									New0.f_End() = _End;
									New0.f_Data() = pSlice->f_Data();
									_Transform(New0.f_Data());
									ToProcess.f_InsertAfter(&New0, pSlice);

									auto &New1 = m_Regions[_End];
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
										//DDTrace("0x{nfh,sj16,sf0} 0x{nfh,sj16,sf0} {}: Same start" DMibNewLine, pSlice->m_TimeStart << pSlice->f_End() << iOper++);
										DMibSafeCheck(_End != pSlice->f_End(), "");

										auto &New0 = m_Regions[_End];
										New0.f_End() = pSlice->f_End();
										pSlice->f_End() = _End;
										New0.f_Data() = pSlice->f_Data();
										ToProcess.f_InsertAfter(&New0, pSlice);
										_Transform(pSlice->f_Data());
										CurrentRegion = pSlice->f_End();
									}
									else
									{
										//DDTrace("0x{nfh,sj16,sf0} 0x{nfh,sj16,sf0} {}: Same end" DMibNewLine, pSlice->m_TimeStart << pSlice->f_End() << iOper++);
										DMibSafeCheck(CurrentRegion != pSlice->f_Start(), "");

										auto &New0 = m_Regions[CurrentRegion];
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
							auto &New = m_Regions[CurrentRegion];
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
					auto pSmallest = m_Regions.f_FindSmallest();
					auto &New = m_Regions[_Start];
					New.f_End() = _End;
					_Transform(New.f_Data());
					ToProcess.f_Insert(New);

					if (pSmallest)
						ToProcess.f_Insert(pSmallest);

	//				DDTrace("New Region {}" DMibNewLine, iOper++);
				}

				// Merge regions
				{
					auto ProcessIter = ToProcess.f_GetIterator();

					auto pSlice = ProcessIter.f_GetCurrent();
					if (pSlice)
						++ProcessIter;

					while (ProcessIter)
					{
						auto *pPrevSlice = pSlice;
						pSlice = ProcessIter;
						++ProcessIter;

						if (pPrevSlice->f_Data() == pSlice->f_Data() && pPrevSlice->f_End() == pSlice->f_Start())
						{
//							DDTrace("0x{nfh,sj16,sf0} 0x{nfh,sj16,sf0} {}: Merge" DMibNewLine, pPrevSlice->m_TimeStart << pSlice->f_End() << iOper++);
							// Merge
							pPrevSlice->f_End() = pSlice->f_End();
							m_Regions.f_Remove(pSlice);
							pSlice = pPrevSlice;
						}
					}
				}

			}

		};

	}
}

