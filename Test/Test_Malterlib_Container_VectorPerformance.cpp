// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/Test/Test>
#include <Mib/Time/PerfTimeMeasure>

#include <vector>
#include <algorithm>

using namespace NMib::NTime;
using namespace NMib::NContainer;
using namespace NMib::NTest;

namespace NMib
{
	namespace NAlgorithm
	{

		template <typename tf_CIterator, typename tf_CCompare>
		bool fg_IsReverseSorted(tf_CIterator _iFirst, tf_CIterator _iLast, tf_CCompare &&_fCompare)
		{
			if (_iFirst != _iLast)
			{
				for (tf_CIterator iNext = _iFirst; ++iNext != _iLast; ++_iFirst)
				{
					if (COrdering_Partial(_fCompare(*_iFirst, *iNext)) < 0)
						return false;
				}
			}
			return true;
		}

		template <typename tf_CIterator>
		inline_small bool fg_IsReverseSorted(tf_CIterator _iFirst, tf_CIterator _iLast)
		{
			return fg_IsReverseSorted(_iFirst, _iLast, CSort_Default());
		}

		template <typename tf_CIterator, typename tf_CCompare>
		bool fg_IsSorted(tf_CIterator _iFirst, tf_CIterator _iLast, tf_CCompare &&_fCompare)
		{
			if (_iFirst != _iLast)
			{
				for (tf_CIterator iNext = _iFirst; ++iNext != _iLast; ++_iFirst)
				{
					if (COrdering_Partial(_fCompare(*iNext, *_iFirst)) < 0)
						return false;
				}
			}
			return true;
		}

		template <typename tf_CIterator>
		inline_small bool fg_IsSorted(tf_CIterator _iFirst, tf_CIterator _iLast)
		{
			return fg_IsSorted(_iFirst, _iLast, CSort_Default());
		}

		template <typename tf_CIterator>
		inline_small void fg_Reverse(tf_CIterator _iFirst, tf_CIterator _iLast)
		{
			for (; _iFirst != _iLast && _iFirst != --_iLast; ++_iFirst)
				fg_Swap(*_iFirst, *_iLast);
		}

		template <typename tf_CData, typename tf_CCompare>
		void fg_InplaceMergeLists(tf_CData *_pBegin, tf_CData *_pMiddle, tf_CData *_pEnd, tf_CCompare &&_fCompare)
		{
			std::inplace_merge(_pBegin, _pMiddle, _pEnd, _fCompare);
/*			if (COrdering_Partial(_fCompare(*_pBegin, *_pMiddle)) < 0)
			{
				while (COrdering_Partial(_fCompare(*_pBegin, *_pMiddle)) < 0 && _pBegin < _pEnd)
					++_pBegin;

				if (_pBegin == _pEnd)
					return; // Already sorted
			}

			while (1)
			{
				while (COrdering_Partial(_fCompare(*_pBegin, *_pBegin2)) < 0)
				{

				}
			}*/
		}

		enum
		{
			EInsertSortTryMaxSwaps = 16
		};
		template <typename t_CData, typename tf_CType>
		inline_always t_CData *fg_InsertSortTryReverse(t_CData *_pLow, t_CData *_pHigh, tf_CType &&_fCompare)
		{
			//return _pHigh;
			umint Swaps = 0;
			t_CData *pI = _pHigh - 2;
			for (; pI >= _pLow; pI--)
			{
				t_CData *pJ = pI + 1;

				if (COrdering_Partial(_fCompare(*pI, *pJ)) < 0)
				{
					if (Swaps >= umint(_pHigh - pI))
						break; // Brake if we are above n*2
					t_CData Temp(fg_Move(*pI));
					pJ[-1] = fg_Move(*pJ);
					pJ++;
					umint SwapsBefore = Swaps;
					++Swaps;
					for (; pJ < _pHigh && COrdering_Partial(_fCompare(Temp, *pJ)) < 0; pJ++)
					{
						++Swaps;
						if (Swaps - SwapsBefore >= EInsertSortTryMaxSwaps)
						{
							for (;pJ > pI; --pJ)
								*pJ = fg_Move(pJ[-1]);
							*pJ = fg_Move(Temp);
							return pI + 1;
						}
						pJ[-1] = fg_Move(*pJ);
					}

					pJ[-1] = fg_Move(Temp);
				}
			}

			return pI + 1;
		}

		template <typename tf_CType, typename t_CData>
		inline_always t_CData *fg_InsertSortTry(t_CData *_pLow, t_CData *_pHigh, tf_CType &&_fCompare)
		{
			//return _pLow;
			umint Swaps = 0;
			t_CData *pL = _pLow;
			t_CData *pEnd = _pHigh - 1;

			auto fl_Swap
				= [&](t_CData *pL) -> bool
				{
					if (Swaps >= umint(pL - _pLow) + EInsertSortTryMaxSwaps)
						return true;
					t_CData *pJ = pL;

					t_CData Temp(fg_Move(pL[1]));
					pJ[1] = fg_Move(*pJ);
					pJ--;

					umint SwapsBefore = Swaps;

					++Swaps;

					for (; pJ >= _pLow && COrdering_Partial(_fCompare(Temp, *pJ)) < 0; pJ--)
					{
						++Swaps;
						if (Swaps - SwapsBefore >= EInsertSortTryMaxSwaps)
						{
							for (;pJ <= pL; ++pJ)
								*pJ = fg_Move(pJ[1]);
							*pJ = fg_Move(Temp);
							return true;
						}
						pJ[1] = fg_Move(*pJ);
					}

					pJ[1] = fg_Move(Temp);
					return false;
				}
			;

			for (; pL < pEnd; ++pL)
			{
				if (COrdering_Partial(_fCompare(pL[1], *pL)) < 0)
				{
					if (fl_Swap(pL))
						break;
				}
			}

			return pL + 1;
		}

		template <typename tf_CType, typename t_CData>
		inline_always void fg_InsertSort(t_CData *_pLow, t_CData *_pHigh, tf_CType &&_fCompare)
		{
			t_CData *pI = _pLow + 1;
			for (; pI < _pHigh; pI++)
			{
				t_CData *pJ = pI - 1;

				if (COrdering_Partial(_fCompare(*pI, *pJ)) < 0)
				{
					t_CData Temp(fg_Move(*pI));
					pJ[1] = fg_Move(*pJ);
					pJ--;

					for (; pJ >= _pLow && COrdering_Partial(_fCompare(Temp, *pJ)) < 0; pJ--)
						pJ[1] = fg_Move(*pJ);

					pJ[1] = fg_Move(Temp);
				}
			}
		}

		template <typename tf_CParam0, typename tf_CParam1, typename tf_CParam2, typename tf_CCompare>
		inline_always void fg_Sort3(tf_CParam0 &_0, tf_CParam1 &_1, tf_CParam2 &_2, tf_CCompare &&_fCompare)
		{
			if (COrdering_Partial(_fCompare(_1, _0)) < 0)
			{
				auto Temp = fg_Move(_0);
				if (COrdering_Partial(_fCompare(_1, _2)) < 0)
				{
					_0 = fg_Move(_1);
					if (COrdering_Partial(_fCompare(_2, Temp)) < 0)
					{
						_1 = fg_Move(_2);
						_2 = fg_Move(Temp);
					}
					else
						_1 = fg_Move(Temp);
				}
				else
				{
					_0 = fg_Move(_2);
					_2 = fg_Move(Temp);
				}
			}
			else
			{
				if (COrdering_Partial(_fCompare(_2, _1)) < 0)
				{
					if (COrdering_Partial(_fCompare(_2, _0)) < 0)
					{
						auto Temp = fg_Move(_0);
						_0 = fg_Move(_2);
						_2 = fg_Move(_1);
						_1 = fg_Move(Temp);
					}
					else
						NMib::fg_Swap(_1, _2);
				}
			}
		}

		namespace NPrivate
		{
			enum
			{
				ETI7_RandomSeed = 123456789,
				ETI7_InsertionSortLimit = 20,
				EIntrospective_InsertionSortLimit = 20,
			};

			inline_always static umint fg_TI7Rand(umint _Old)
			{
				return (_Old * 69069 + 362437);
			}

			template <typename tf_CIterator, typename tf_CCompare>
			inline_always static void fg_TI7InsertionSort(tf_CIterator _Iterator, umint _Length, tf_CCompare &&_Comp)
			{
				// umint counter;

				if (_Length > 1)
				{
					tf_CIterator iMiddle = _Iterator + 1;
					tf_CIterator iFirst = iMiddle;
					tf_CIterator iLast = _Iterator + (_Length-1);
					while (iFirst != iLast)
					{
						++iFirst;
						if (COrdering_Partial(_Comp(*iFirst, *iMiddle)) < 0)
						{
							iMiddle = iFirst;
						}
					}
					if (COrdering_Partial(_Comp(*iMiddle, *_Iterator)) < 0)
					{
						NMib::fg_Swap(*_Iterator, *iMiddle);
					}
					++_Iterator;
					while (_Iterator != iLast)
					{
						iFirst = _Iterator++;
						if (COrdering_Partial(_Comp(*_Iterator, *iFirst)) < 0)
						{
							iMiddle = _Iterator;
							auto temp = *iMiddle;
							do
							{
								*(iMiddle--) = *(iFirst--);
							}
							while (COrdering_Partial(_Comp(temp, *iFirst)) < 0);
							*iMiddle = temp;
						}
					}
				}
			}

			template <typename tf_CIterator, typename tf_CCompare>
			inline_always static void fg_TI7SiftDownMiddle(tf_CIterator _Iterator, umint _i, umint _n, tf_CCompare &&_Comp)
			{
				umint p = _i;
				umint c = p * 2;
				auto temp = _Iterator[p];

				while(_n - p > p)
				{
					if (COrdering_Partial(_Comp(*(_Iterator + c), *(_Iterator + c + 1))) < 0)
					{
						++c;
					}
					if (COrdering_Partial(_Comp(temp, *(_Iterator + c))) < 0)
					{
						_Iterator[p] = _Iterator[c];
						p = c;
						c *= 2;
					}
					else
						break;
				}
				if ((_n == c) && COrdering_Partial(_Comp(temp, *(_Iterator + c))) < 0)
				{
					_Iterator[p] = _Iterator[c];
					p = c;
				}
				_Iterator[p] = temp;
			}

			template <typename tf_CIterator, typename tf_CCompare>
			inline_never static void fg_TI7HeapSort(tf_CIterator _Iterator, umint _Length, tf_CCompare &&_Comp)
			{
				if (_Length > 1)
				{
					umint i = --_Length / 2;
					do
					{
						fg_TI7SiftDownMiddle(_Iterator, i, _Length, _Comp);
					}
					while (i-- != 0);

					NMib::fg_Swap(*_Iterator, *(_Iterator + _Length));
					while (--_Length != 0)
					{
						fg_TI7SiftDownMiddle(_Iterator, 0, _Length, _Comp);
						NMib::fg_Swap(*_Iterator, *(_Iterator + _Length));
					}
				}
			}

			template <typename tf_CIterator, typename tf_CCompare>
			static umint fg_TI7Loop(tf_CIterator _Iterator, umint _Length, umint _DepthLeft, umint _Random, tf_CCompare &&_Comp)
			{
				static_assert(ETI7_InsertionSortLimit >= 4, "what");

				while (_Length > ETI7_InsertionSortLimit)
				{
					if (fg_IsSorted(_Iterator, _Iterator + _Length, _Comp))
					{
						return _Random;
					}
					if (!_DepthLeft--)
					{
						fg_TI7HeapSort(_Iterator, _Length, _Comp);
						return _Random;
					}
					_Random = fg_TI7Rand(_Random);
					NMib::fg_Swap(*_Iterator, *(_Iterator + umint(_Random % umint(--_Length))));
					tf_CIterator iFirst = _Iterator + 1;
					tf_CIterator iLast = _Iterator + _Length;
					NMib::NAlgorithm::fg_Sort3(*iFirst, *_Iterator, *iLast, _Comp);
					do
					{
						++iFirst;
					}
					while (COrdering_Partial(_Comp(*iFirst, *_Iterator)) < 0);

					do
					{
						--iLast;
					}
					while (COrdering_Partial(_Comp(*_Iterator, *iLast)) < 0);

					while (iLast > iFirst)
					{
						NMib::fg_Swap(*iLast, *iFirst);
						do
						{
							++iFirst;
						}
						while (COrdering_Partial(_Comp(*iFirst, *_Iterator)) < 0);

						do
						{
							--iLast;
						}
						while (COrdering_Partial(_Comp(*_Iterator, *iLast)) < 0);
					}
					NMib::fg_Swap(*_Iterator, *iLast);
					_Random = fg_TI7Loop(iLast + 1, _Length + (_Iterator - iLast), _DepthLeft, _Random, _Comp);
					_Length = iLast - _Iterator;
				}
				fg_TI7InsertionSort(_Iterator, _Length, _Comp);
				return _Random;
			}
			template <typename tf_CIterator, typename tf_CCompare>
			inline_never static umint fg_IntrospectiveLoop(tf_CIterator _Iterator, umint _Length, umint _DepthLeft, umint _Random, tf_CCompare &&_Comp)
			{
				static_assert(EIntrospective_InsertionSortLimit >= 4, "what");
				if (_Length > EIntrospective_InsertionSortLimit)
				{

#if 0
					auto pLow = _Iterator;
					auto pHigh = _Iterator + _Length;
					auto *pOldLow = pLow;
					auto *pOldHigh = pHigh;

					auto *pInsertResultLow = fg_InsertSortTry(pLow, pHigh, _Comp);
//					if (!fg_IsSorted(pLow, pInsertResultLow, _Comp))
//						int x = 0;
					if (pInsertResultLow == pHigh)
						return _Random;

					if (pInsertResultLow - pLow > EInsertSortTryMaxSwaps)
						pLow = pInsertResultLow;

					auto *pInsertResultHigh = fg_InsertSortTryReverse(pLow, pHigh, _Comp);
//					if (!fg_IsReverseSorted(pInsertResultHigh, pHigh, _Comp))
//						int x = 0;

					if (pInsertResultHigh != pOldHigh)
						fg_Reverse(pInsertResultHigh, pOldHigh);

					if (pInsertResultHigh <= pInsertResultLow)
					{
						if (pInsertResultHigh != pOldLow)
							fg_InplaceMergeLists(pOldLow, pInsertResultHigh, pOldHigh, _Comp);

//						if (!fg_IsSorted(pOldLow, pOldHigh, _Comp))
//							int x = 0;

						return _Random;
					}
#endif

					while (_Length > 0)
					{
						[[maybe_unused]] auto pLow = _Iterator;
						[[maybe_unused]] auto pHigh = _Iterator + _Length;

/*						auto *pInsertResultLow = fg_InsertSortTry(pLow, pHigh, _Comp);
						if (pInsertResultLow == pHigh)
							return _Random;*/

						if (!_DepthLeft--)
						{
							fg_TI7HeapSort(_Iterator, _Length, _Comp);
							return _Random;
						}
						//_Random = fg_TI7Rand(_Random);
						//NMib::fg_Swap(*_Iterator, *(_Iterator + umint(_Random % umint(--_Length))));
						--_Length;
						NMib::fg_Swap(*_Iterator, *(_Iterator + _Length / 2));
						tf_CIterator iFirst = _Iterator + 1;
						tf_CIterator iLast = _Iterator + _Length;
						NMib::NAlgorithm::fg_Sort3(*iFirst, *_Iterator, *iLast, _Comp);
						do
						{
							++iFirst;
						}
						while (COrdering_Partial(_Comp(*iFirst, *_Iterator)) < 0);

						do
						{
							--iLast;
						}
						while (COrdering_Partial(_Comp(*_Iterator, *iLast)) < 0);

						while (iLast > iFirst)
						{
							NMib::fg_Swap(*iLast, *iFirst);
							do
							{
								++iFirst;
							}
							while(COrdering_Partial(_Comp(*iFirst, *_Iterator)) < 0);

							do
							{
								--iLast;
							}
							while (COrdering_Partial(_Comp(*_Iterator, *iLast)) < 0);
						}
						NMib::fg_Swap(*_Iterator, *iLast);
						_Random = fg_IntrospectiveLoop(iLast + 1, _Length + (_Iterator - iLast), _DepthLeft, _Random, _Comp);
						_Length = iLast - _Iterator;
					}
				}
				//fg_TI7InsertionSort(_Iterator, _Length, _Comp);
				fg_InsertSort(_Iterator, _Iterator + _Length, _Comp);
				return _Random;
			}
		}

		template <typename tf_CIterator, typename tf_CCompare>
		inline void fg_TI7Sort(tf_CIterator _Iterator, umint _Count, tf_CCompare &&_Comp)
		{
			if (_Count > 1 && !fg_IsSorted(_Iterator, _Iterator+_Count, _Comp))
			{
				if (!fg_IsReverseSorted(_Iterator, _Iterator + _Count, _Comp))
				{
					umint MaxDepth = 2;
					for (umint n = _Count / 4; n; n /= 2)
						++MaxDepth;
					MaxDepth *= 2;
					NPrivate::fg_TI7Loop(_Iterator, _Count, MaxDepth, NPrivate::ETI7_RandomSeed, _Comp);
				}
				else
				{
					fg_Reverse(_Iterator, _Iterator + _Count);
				}
			}
		}

		template <typename tf_CIterator, typename tf_CCompare>
		inline_small void fg_SortIntrospective(tf_CIterator _Iterator, umint _Count, tf_CCompare &&_Comp)
		{
			if (_Count < 16)
			{
				fg_InsertSort(_Iterator, _Iterator + _Count, _Comp);
				return;
			}

			umint MaxDepth = (2 + fg_GetHighestBitSetNoZero(_Count / 4)) * 2;
			NPrivate::fg_IntrospectiveLoop(_Iterator, _Count, MaxDepth, NPrivate::ETI7_RandomSeed, _Comp);
		}

		template <typename tf_CIterator>
		inline_small void fg_SortIntrospective(tf_CIterator _Iterator, umint _Count)
		{
			fg_SortIntrospective(_Iterator, _Count, CSort_Default());
		}

	}
}

namespace NMib
{
	namespace NContainer
	{

		namespace Tests {

			struct Big { char dummy[1000]; };
			class CVectorPerformance_Tests : public CTest
			{
				template <int32 tf_nItems>
				void fp_Insert_Big()
				{
#ifdef DMibDebug
					int32 nRep = 1;
#else
					int32 nRep = 100000 / tf_nItems;
					if (nRep < 10)
						nRep = 10;
#endif

					Big Item;
					for (int32 i = 0; i < 1000; ++i)
						Item.dummy[i] = i;
					CPerfTimeMeasureMin MalterlibTime;
					CPerfTimeMeasureMin StdTime;

					for (int32 i=0;i<nRep;++i)
					{
						TCVector<Big> Vec1;
						MalterlibTime.f_Start();
						for (int32 i=0;i<tf_nItems;++i)
							Vec1.f_Insert(Item);
						MalterlibTime.f_Stop();
					}
					for (int32 i=0;i<nRep;++i)
					{
						std::vector<Big> Vec2;
						StdTime.f_Start();
						for (int32 i=0;i<tf_nItems;++i)
							Vec2.push_back(Item);
						StdTime.f_Stop();
					}
					DMibTest(DMibExpr(StdTime) / DMibExpr(MalterlibTime) > DMibExpr(1.0));
				}
				template <int32 tf_nItems>
				void fp_Insert_Simple()
				{
#ifdef DMibDebug
					int32 nRep = 1;
#else
					int32 nRep = 1000000 / tf_nItems;
					if (nRep < 10)
						nRep = 10;
#endif

					int32 Item = 1;
					CPerfTimeMeasureMin MalterlibTime;
					CPerfTimeMeasureMin StdTime;
					for (int32 i=0;i<nRep;++i)
					{
						TCVector<int32> Vec1;
						MalterlibTime.f_Start();
						for (int32 i=0;i<tf_nItems;++i)
							Vec1.f_Insert(Item);
						MalterlibTime.f_Stop();
					}
					for (int32 i=0;i<nRep;++i)
					{
						std::vector<int32> Vec2;
						StdTime.f_Start();
						for (int32 i=0;i<tf_nItems;++i)
							Vec2.push_back(Item);
						StdTime.f_Stop();
					}
					DMibTest(DMibExpr(StdTime) / DMibExpr(MalterlibTime) > DMibExpr(1.0));
				}
				template <int32 tf_nItems>
				void fp_Insert_Vector()
				{
#ifdef DMibDebug
					int32 nRep = 1;
#else
					int32 nRep = 100000 / tf_nItems;
					if (nRep < 3)
						nRep = 3;
#endif

					TCVector<int32> Vec1;
					std::vector<int32> Vec2;
					for (int32 i=0;i<100;++i)
					{
						Vec1.f_Insert(1);
					}

					for (int32 i=0;i<100;++i)
					{
						Vec2.push_back(1);
					}

					TCVector<TCVector<int32>> vecvec1;
					std::vector<std::vector<int32>> vecvec2;

					CPerfTimeMeasureMin MalterlibTime;
					CPerfTimeMeasureMin StdTime;
					for (int32 i=0;i<nRep;++i)
					{
						MalterlibTime.f_Start();
						for (int32 i=0;i<tf_nItems;++i)
						{
							vecvec1.f_Insert(Vec1);
						}
						MalterlibTime.f_Stop();
					}
					for (int32 i=0;i<nRep;++i)
					{
						StdTime.f_Start();
						for (int32 i=0;i<tf_nItems;++i)
						{
							vecvec2.push_back(Vec2);
						}
						StdTime.f_Stop();
					}

					DMibTest(DMibExpr(StdTime) / DMibExpr(MalterlibTime) > DMibExpr(1.0));
				}

				enum ESortType
				{
					ESortType_Random
					, ESortType_Sorted
					, ESortType_Reversed
					, ESortType_AlmostSorted
					, ESortType_AlmostReversed
					, ESortType_ManySame
					, ESortType_AllSame
					, ESortType_Uneven
					, ESortType_OneUnsorted
					, ESortType_HalfSortedHalfReversed
					, ESortType_TwoSortedRanges
				};

				template <int32 tf_nItems, ESortType tf_SortType>
				void fp_Sort_Simple()
				{
					NMisc::CRandomShiftRNG Random;

#ifdef DMibDebug
					int32 nRep = 1;
#else
					int32 nRep = 1000000 / tf_nItems;
					if (nRep < 10)
						nRep = 10;
					if (tf_nItems > 10000000)
						nRep = 1;
#endif

					TCVector<int32> ToSortMalterlib;

					for (int i = 0; i < tf_nItems; ++i)
					{
						int32 Value;
						switch (tf_SortType)
						{
						case ESortType_Random:
							Value = Random.f_GetValue<uint32>() % tf_nItems;
							//Value = Random.f_GetValue<uint32>() % TCLimitsInt<int32>::mc_Max;
							break;
						case ESortType_Sorted:
							Value = i;
							break;
						case ESortType_OneUnsorted:
							if (i == tf_nItems / 2)
								Value = tf_nItems * 2;
							else
								Value = i;
							break;
						case ESortType_Reversed:
							Value = (tf_nItems - 1) - i;
							break;
						case ESortType_HalfSortedHalfReversed:
							if (i < tf_nItems / 2)
								Value = i;
							else
								Value = (tf_nItems - 1) - i;
							break;
						case ESortType_TwoSortedRanges:
							if (i < tf_nItems / 2)
								Value = i;
							else
								Value = i - tf_nItems / 2;
							break;
						case ESortType_AlmostSorted:
							Value = i + Random.f_GetValue<uint32>() % 5;
							break;
						case ESortType_AlmostReversed:
							Value = ((tf_nItems - 1) - i) + Random.f_GetValue<uint32>() % 5;
							break;
						case ESortType_ManySame:
							Value = Random.f_GetValue<uint32>() % 100;
							break;
						case ESortType_AllSame:
							Value = 0;
							break;
						case ESortType_Uneven:
							if ((i & 1) == 0)
								Value = 0;
							else
								Value = i;
							break;
						}

						ToSortMalterlib.f_Insert(Value);
					}

					CPerfTimeMeasureMin MalterlibTime;
					CPerfTimeMeasureMin StdTime;

					for (int32 i=0;i<nRep;++i)
					{
						TCVector<int32> Vec1 = ToSortMalterlib;
						MalterlibTime.f_Start();
						NMib::NAlgorithm::fg_SortIntrospective(Vec1.f_GetArray(), Vec1.f_GetLen());
						//Vec1.f_Sort2();
						//fg_IntroSort(Vec1.f_GetArray(), Vec1.f_GetArray() + Vec1.f_GetLen(), CSort_Default());
						MalterlibTime.f_Stop();
						if (i == 0)
						{
							if (!Vec1.f_IsAdjacentUnique())
							{
								DMibTest(DMibExpr(Vec1.f_IsAdjacentUnique()))(ETestFlag_Aggregated);
							}
						}
					}
					for (int32 i=0;i<nRep;++i)
					{
						TCVector<int32> Vec2 = ToSortMalterlib;
						StdTime.f_Start();
						//fg_IntroSort(Vec2.begin(), Vec2.end(), CSort_Default());
						NMib::NAlgorithm::fg_TI7Sort(Vec2.f_GetArray(), Vec2.f_GetLen(), NMib::CSort_Default());
//						std::sort(Vec2.begin(), Vec2.end());
						StdTime.f_Stop();
		//				if (i == 0)
			//				DMibTest(DMibExpr(Vec2.f_IsAdjacentUnique()))(ETestFlag_Aggregated);
					}
					DMibTest(DMibExpr(StdTime) / DMibExpr(MalterlibTime) > DMibExpr(0.90));
				}

				struct CTestClass
				{
					int32 m_Value;
					int32 m_Sequence;

					auto operator <=> (CTestClass const &_Right) const noexcept
					{
						return m_Value <=> _Right.m_Value;
					}
				};

				template <int32 tf_nItems, ESortType tf_SortType>
				void fp_Sort_Stability()
				{
					NMisc::CRandomShiftRNG Random;

#ifdef DMibDebug
					int32 nRep = 1;
#else
					int32 nRep = 1000000 / tf_nItems;
					if (nRep < 10)
						nRep = 10;
#endif

					TCVector<CTestClass> ToSortMalterlib;
					std::vector<CTestClass> ToSortStl;

					int32 Sequence = 0;
					for (int i = 0; i < tf_nItems; ++i)
					{
						int32 Value;
						switch (tf_SortType)
						{
						case ESortType_Random:
							//Value = Random.f_GetValue<uint32>() % tf_nItems;
							Value = Random.f_GetValue<uint32>() % TCLimitsInt<int32>::mc_Max;
							break;
						case ESortType_Sorted:
							Value = i;
							break;
						case ESortType_Reversed:
							Value = (tf_nItems - 1) - i;
							break;
						case ESortType_AlmostSorted:
							Value = i + Random.f_GetValue<int32>() % 10;
							break;
						case ESortType_ManySame:
							Value = i / 100;
							break;
						case ESortType_AllSame:
							Value = 0;
							break;
						case ESortType_Uneven:
							if ((i & 1) == 0)
								Value = 0;
							else
								Value = i;
							break;
						}

						auto &Item = ToSortMalterlib.f_Insert();
						Item.m_Value = Value;
						Item.m_Sequence = Sequence++;
						ToSortStl.push_back(Item);
					}
					auto fl_IsStable =
						[](TCVector<CTestClass> const &_Vector) -> bool
						{
							CTestClass const *pPrev = nullptr;
							for (auto iItem = _Vector.f_GetIterator(); iItem; ++iItem)
							{
								if (pPrev)
								{
									if (pPrev->m_Value == iItem->m_Value)
									{
										if (iItem->m_Sequence < pPrev->m_Sequence)
											return false;
									}
								}
								pPrev = &(*iItem);
							}
							return true;
						}
					;
					CPerfTimeMeasureMin MalterlibTime;
					CPerfTimeMeasureMin StdTime;
					for (int32 i=0;i<nRep;++i)
					{
						TCVector<CTestClass> Vec1 = ToSortMalterlib;
						MalterlibTime.f_Start();
						Vec1.f_Sort();
						MalterlibTime.f_Stop();
						if (i == 0)
						{
							DMibTest(DMibExpr(Vec1.f_IsAdjacentUnique()))(ETestFlag_Aggregated);
							DMibTest(DMibExpr(fl_IsStable(Vec1)));
						}
					}

					for (int32 i=0;i<nRep;++i)
					{
						std::vector<CTestClass> Vec2 = ToSortStl;
						StdTime.f_Start();
						std::stable_sort(Vec2.begin(), Vec2.end());
						StdTime.f_Stop();
					}
					DMibTest(DMibExpr(StdTime) / DMibExpr(MalterlibTime) > DMibExpr(1.0));
				}

			public:

				struct CThing
				{
					int32 m_Val;
					CThing() : m_Val(-1)
					{
					}
					CThing(int32 _V) : m_Val(_V)
					{
					}
				private:
					CThing(const CThing& _CopyThis) : m_Val(_CopyThis.m_Val)
					{
					}
				public:
					CThing(CThing && _MoveThis)
					{
						*this = fg_Move(_MoveThis);
					}

					CThing& operator =(CThing &&_MoveThis)
					{
						DMibTest(DMibExpr(this != &_MoveThis )) (ETestFlag_Aggregated);
						DMibTest(DMibExpr(_MoveThis.m_Val != -1)) (ETestFlag_Aggregated);
						m_Val = _MoveThis.m_Val;
						_MoveThis.m_Val = -1;
						return *this;
					}
				};

				template <int32 tf_nItems>
				void f_TestItems()
				{
					DMibTestCategory(NMib::NStr::CStr(NMib::NStr::CStr::CFormat("{} items") << tf_nItems))
					{
						DMibTestSuite("Insert simple Items")
						{
							fp_Insert_Simple<tf_nItems>();
						};
						DMibTestSuite("Insert big Items")
						{
							fp_Insert_Big<tf_nItems>();
						};
						DMibTestSuite("Insert vectors")
						{
							fp_Insert_Vector<tf_nItems>();
						};
#if 1
						DMibTestSuite("Sort random")
						{
							fp_Sort_Simple<tf_nItems, ESortType_Random>();
						};
						DMibTestSuite("Sort sorted")
						{
							fp_Sort_Simple<tf_nItems, ESortType_Sorted>();
						};
						DMibTestSuite("Sort reversed")
						{
							fp_Sort_Simple<tf_nItems, ESortType_Reversed>();
						};
						DMibTestSuite("Sort half sorted half reversed")
						{
							fp_Sort_Simple<tf_nItems, ESortType_HalfSortedHalfReversed>();
						};
						DMibTestSuite("Sort two sorted ranges")
						{
							fp_Sort_Simple<tf_nItems, ESortType_TwoSortedRanges>();
						};
						DMibTestSuite("Sort almost sorted")
						{
							fp_Sort_Simple<tf_nItems, ESortType_AlmostSorted>();
						};
						DMibTestSuite("Sort almost reversed")
						{
							fp_Sort_Simple<tf_nItems, ESortType_AlmostReversed>();
						};
						DMibTestSuite("Sort many same")
						{
							fp_Sort_Simple<tf_nItems, ESortType_ManySame>();
						};
						DMibTestSuite("Sort same")
						{
							fp_Sort_Simple<tf_nItems, ESortType_AllSame>();
						};
						DMibTestSuite("Sort uneven")
						{
							fp_Sort_Simple<tf_nItems, ESortType_Uneven>();
						};
						DMibTestSuite("Sort one unsorted")
						{
							fp_Sort_Simple<tf_nItems, ESortType_OneUnsorted>();
						};
#else
						DMibTestSuite("Sort same")
						{
							fp_Sort_Simple<tf_nItems, ESortType_AllSame>();
						};
#endif
#if 0
						DMibTestSuite("Sort stable random")
						{
							fp_Sort_Stability<tf_nItems, ESortType_Random>();
						};
						DMibTestSuite("Sort stable sorted")
						{
							fp_Sort_Stability<tf_nItems, ESortType_Sorted>();
						};
						DMibTestSuite("Sort stable reversed")
						{
							fp_Sort_Stability<tf_nItems, ESortType_Reversed>();
						};
						DMibTestSuite("Sort stable almost sorted")
						{
							fp_Sort_Stability<tf_nItems, ESortType_AlmostSorted>();
						};
						DMibTestSuite("Sort stable many same")
						{
							fp_Sort_Stability<tf_nItems, ESortType_ManySame>();
						};
						DMibTestSuite("Sort stable all same")
						{
							fp_Sort_Stability<tf_nItems, ESortType_AllSame>();
						};
						DMibTestSuite("Sort stable uneven")
						{
							fp_Sort_Stability<tf_nItems, ESortType_Uneven>();
						};
#endif
					};
				}

				void f_DoTests()
				{
					DMibTestSuite("Regressions")
					{
						using CThing = CThing;
						{
							NMib::NContainer::TCVector<CThing> lVec;

							for (int32 i = 0; i< 19; ++i)
								lVec.f_Insert(CThing(i + 1));

							lVec[9] = CThing(0);

							lVec.f_Sort([](CThing const& _A, CThing const& _B)
							{
								return _A.m_Val <=> _B.m_Val;
							} );

							for (int32 i = 1; i< 19; ++i)
							{
								DMibTest(DMibExpr(lVec[i].m_Val) != DMibExpr(-1)) (ETestFlag_Aggregated);
								DMibTest(DMibExpr(lVec[i-1].m_Val) < DMibExpr(lVec[i].m_Val )) (ETestFlag_Aggregated);
							}

						}
					};
					DMibTestCategory(CTestCategory("Performace") << CTestGroup("Unfinished"))
					{
//						f_TestItems<2>();
						//f_TestItems<100000>();
						DMibTestSuite("Debug")
						{
							fp_Sort_Simple<21, ESortType_Reversed>();
						};

#if 0
						f_TestItems<10>();
						f_TestItems<100>();
						f_TestItems<1000>();
						f_TestItems<10000>();
						f_TestItems<100000>();
						f_TestItems<1000000>();
						f_TestItems<10000000>();
#endif
						f_TestItems<100000000>();
					};
				}
			};

			DMibTestRegister(CVectorPerformance_Tests, Malterlib::Container);
		}
	}
}
