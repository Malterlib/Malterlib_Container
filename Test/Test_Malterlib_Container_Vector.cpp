// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Test/Exception>
#include <Mib/Test/Memory>
#include <Mib/Memory/MemoryManager>

/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| CTestSort
|__________________________________________________________________________________________________
\*************************************************************************************************/

namespace
{
	using namespace NMib;
	using namespace NMib::NStr;
	using namespace NMib::NContainer;
	using namespace NMib::NException;

	constinit NAtomic::TCAtomic<umint> g_nObjects = 0;

	class CVector_Tests : public NMib::NTest::CTest
	{
	public:
		struct CCountedObject
		{
			CCountedObject(CStr const &_Value)
				: m_Value(_Value)
			{
				++g_nObjects;
			}

			CCountedObject()
			{
				++g_nObjects;
			}

			~CCountedObject()
			{
				--g_nObjects;
			}

			CCountedObject(CCountedObject const &_Other)
				: m_Value(_Other.m_Value)
			{
				if (_Other.m_bThrowCopy)
					DMibError("Throw copy");

				_Other.m_bCopiedFrom = true;

				++g_nObjects;
			}

			CCountedObject(CCountedObject &&_Other)
				: m_Value(fg_Move(_Other.m_Value))
			{
				_Other.m_bMovedFrom = true;

				++g_nObjects;
			}

			bool operator == (CCountedObject const &_Right) const noexcept
			{
				return m_Value == _Right.m_Value;
			}

			COrdering_Weak operator <=> (CCountedObject const &_Right) const noexcept
			{
				return m_Value <=> _Right.m_Value;
			}

			template <typename tf_CStr>
			void f_Format(tf_CStr &o_Str) const
			{
				o_Str += m_Value;
			}

			CStr m_Value;

			bool m_bThrowCopy = false;
			bool m_bMovedFrom = false;
			mutable bool m_bCopiedFrom = false;
		};

		static TCVector<int32> fs_Vector(int32 _0, int32 _1, int32 _2, int32 _3, int32 _4, int32 _5, int32 _6, int32 _7, int32 _8, int32 _9)
		{
			TCVector<int32> Ret;
			Ret.f_Insert(_0);
			Ret.f_Insert(_1);
			Ret.f_Insert(_2);
			Ret.f_Insert(_3);
			Ret.f_Insert(_4);
			Ret.f_Insert(_5);
			Ret.f_Insert(_6);
			Ret.f_Insert(_7);
			Ret.f_Insert(_8);
			Ret.f_Insert(_9);
			return Ret;
		}

		TCVector<int32> fs_DefaultList()
		{
			TCVector<int32> Ret;

			for (int32 i = 0; i < 10; ++i)
				Ret.f_Insert(i);

			return Ret;
		}

		void f_General()
		{
			DMibTestSuite("Basic")
			{
				TCVector<int32> List;
				// Test const insert
				int32 Test(0);
				List.f_Insert(Test);
				// Test non-const insert
				for (int32 i = 1; i < 10; ++i)
					List.f_Insert(i);

				int32 nItems = 10;

				DMibTest(DMibExpr(List.f_GetLen() == 10));
				{
					bool bCorrectOrder = true;
					TCVector<int32>::CIterator Iter = List.f_GetIterator();
					for (int32 i = 0; i < nItems; ++i, ++Iter)
						bCorrectOrder = bCorrectOrder && *Iter == i;
					DMibTest(DMibExpr(bCorrectOrder));
				}
				{
					bool bCorrectOrder2 = true;
					TCVector<int32>::CIteratorConst Iter = NMib::fg_Const(List).f_GetIterator();
					for (int32 i = 0; i < nItems; ++i, ++Iter)
						bCorrectOrder2 = bCorrectOrder2 && *Iter == i;
					DMibTest(DMibExpr(bCorrectOrder2));
				}

				List.f_InsertFirst(11);
				DMibTest(DMibExpr(List.f_GetFirst()) == DMibExpr(11));

				int32 ToInsertFirst = 55;
				[[maybe_unused]] int32 &Inserted = List.f_InsertFirst(ToInsertFirst);
				DMibTest(DMibExpr(List.f_GetFirst()) == DMibExpr(55));

				List.f_Remove(0);
				DMibTest(DMibExpr(List.f_GetFirst()) == DMibExpr(11) && DMibExpr(2));

				List.f_Push(12);
				DMibTest(DMibExpr(List.f_Pop()) == DMibExpr(12));

				int32 ToPush = 77;
				List.f_Push(ToPush);
				DMibTest(DMibExpr(List.f_Pop()) == DMibExpr(77));

				List.f_InsertLast(13);
				DMibTest(DMibExpr(List.f_GetLast()) == DMibExpr(13));

				int32 ToInsertLast = 66;
				[[maybe_unused]] int32 &InsertedLast = List.f_InsertLast(ToInsertLast);
				DMibTest(DMibExpr(List.f_GetLast()) == DMibExpr(66));

				List.f_Remove(List.f_GetLen() - 1);
				DMibTest(DMibExpr(List.f_GetLast()) == DMibExpr(13) && DMibExpr(2));

				TCVector<int32> List2 = List;
				DMibTest(DMibExpr(List2) == DMibExpr(List));

				TCVector<int32> List3;
				List3 = List;
				DMibTest(DMibExpr(List3) == DMibExpr(List));

				{
					TCVector<int32> const &ListConst = List;

					DMibTest(DMibExpr(ListConst.f_GetFirst()) == DMibExpr(11));
					DMibTest(DMibExpr(ListConst.f_GetLast()) == DMibExpr(13));
				}

				List2.f_Remove(List3.f_GetLen()-1);
				List3.f_Remove(0);

				DMibTest(DMibExpr(List2) < DMibExpr(List));
				DMibTest(DMibExpr(List3) < DMibExpr(List));
				DMibTest(DMibExpr(List3) < DMibExpr(List2));

				DMibTest(!(DMibExpr(List) < DMibExpr(List2)));
				DMibTest(!(DMibExpr(List) < DMibExpr(List3)));
				DMibTest(!(DMibExpr(List2) < DMibExpr(List3)));

				DMibTest(!(DMibExpr(List) < DMibExpr(List)));

				DMibTest(DMibExpr(List3) != DMibExpr(List));
				DMibTest(DMibExpr(List) != DMibExpr(List3));
				DMibTest(DMibExpr(List2) != DMibExpr(List));
				DMibTest(DMibExpr(List) != DMibExpr(List2));
				DMibTest(DMibExpr(List3) != DMibExpr(List2));
				DMibTest(DMibExpr(List2) != DMibExpr(List3));

				// Insert After Tests
				{
					List.f_InsertAfter(0) = 77;
					auto Iter = List.f_GetIterator();
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(77));
				}
				{
					auto Iter = List.f_GetIterator();
					List.f_InsertAfter(Iter) = 88;
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(88));
				}
				{
					auto Iter = List.f_GetIterator();
					List.f_InsertAfter(Iter, 99);
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(99));
				}
				{
					List.f_InsertAfter(0, 111);
					auto Iter = List.f_GetIterator();
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(111));
				}
				{
					List.f_InsertAfter(10, 444);
					DMibTest(DMibExpr(List[11]) == DMibExpr(444));
				}
				{
					List.f_InsertAfter(List.f_GetLen()-1, 33);
					DMibTest(DMibExpr(List.f_GetLast()) == DMibExpr(33));
				}

				// InsertBefore Tests
				{
					List.f_InsertBefore(0) = 112;
					auto Iter = List.f_GetIterator();
					DMibTest(DMibExpr(*Iter) == DMibExpr(112));
				}
				{
					auto Iter = List.f_GetIterator();
					List.f_InsertBefore(Iter) = 113;
					Iter = List.f_GetIterator();
					DMibTest(DMibExpr(*Iter) == DMibExpr(113));
				}
				{
					auto Iter = List.f_GetIterator();
					List.f_InsertBefore(Iter, 114);
					Iter = List.f_GetIterator();
					DMibTest(DMibExpr(*Iter) == DMibExpr(114));
				}
				{
					List.f_InsertBefore(0, 115);
					auto Iter = List.f_GetIterator();
					DMibTest(DMibExpr(*Iter) == DMibExpr(115));
				}
				{
					List.f_InsertBefore(10, 555);
					DMibTest(DMibExpr(List[10]) == DMibExpr(555));
				}
				{
					List.f_InsertBefore(List.f_GetLen()-1, 366);
					DMibTest(DMibExpr(List[List.f_GetLen()-2]) == DMibExpr(366));
				}
				{
					List.f_InsertBefore(List.f_GetLen(), 367);
					DMibTest(DMibExpr(List[List.f_GetLen()-1]) == DMibExpr(367));
				}

				List.f_Clear();
				DMibTest(DMibExpr(List.f_IsEmpty()));

				List = fs_DefaultList();
				List.f_Move(0, 5, 2);
				DMibTest(DMibExpr(List) == DMibExpr(fs_Vector(2,3,4,0,1,5,6,7,8,9)));

				List = fs_DefaultList();
				List.f_Move(1, 5, 2);
				DMibTest(DMibExpr(List) == DMibExpr(fs_Vector(0,3,4,1,2,5,6,7,8,9)));

				List = fs_DefaultList();
				List.f_Move(0, 10, 2);
				DMibTest(DMibExpr(List) == DMibExpr(fs_Vector(2,3,4,5,6,7,8,9,0,1)));

				List = fs_DefaultList();
				List.f_Move(1, 10, 2);
				DMibTest(DMibExpr(List) == DMibExpr(fs_Vector(0,3,4,5,6,7,8,9,1,2)));


				List = fs_DefaultList();
				List.f_Move(8, 5, 2);
				DMibTest(DMibExpr(List) == DMibExpr(fs_Vector(0,1,2,3,4,8,9,5,6,7)));

				List = fs_DefaultList();
				List.f_Move(7, 5, 2);
				DMibTest(DMibExpr(List) == DMibExpr(fs_Vector(0,1,2,3,4,7,8,5,6,9)));

				List = fs_DefaultList();
				List.f_Move(8, 0, 2);
				DMibTest(DMibExpr(List) == DMibExpr(fs_Vector(8,9,0,1,2,3,4,5,6,7)));

				List = fs_DefaultList();
				List.f_Move(7, 0, 2);
				DMibTest(DMibExpr(List) == DMibExpr(fs_Vector(7,8,0,1,2,3,4,5,6,9)));

				// Bounds

				List = fs_DefaultList();
				List.f_Move(0, 2, 2);
				DMibTest(DMibExpr(List) == DMibExpr(fs_Vector(0,1,2,3,4,5,6,7,8,9)));

				List = fs_DefaultList();
				List.f_Move(2, 2, 2);
				DMibTest(DMibExpr(List) == DMibExpr(fs_Vector(0,1,2,3,4,5,6,7,8,9)) && DMibExpr(2));
			};

			DMibTestSuite("Exceptions")
			{
				TCVector<int32> List = fs_DefaultList();

				DMibTest(DMibExpr(TCThrowsException<CException>()) == DMibLExpr(List[-1]));
				DMibTest(DMibExpr(TCThrowsException<CException>()) == DMibLExpr(List[10]));

				DMibTest(DMibExpr(TCThrowsException<CException>()) == DMibLExpr(List.f_InsertAfter(10)));
				DMibTest(DMibExpr(TCThrowsException<CException>()) == DMibLExpr(List.f_InsertBefore(11)));


				// Out of bounds move
				DMibTest(DMibExpr(TCThrowsException<CException>()) == DMibLExpr(List.f_Move(9, 0, 2)));
				DMibTest(DMibExpr(TCThrowsException<CException>()) == DMibLExpr(List.f_Move(0, 11, 2)));

				// Overlapping ranges
				DMibTest(DMibExpr(TCThrowsException<CException>()) == DMibLExpr(List.f_Move(0, 2, 3)));
				DMibTest(DMibExpr(TCThrowsException<CException>()) == DMibLExpr(List.f_Move(5, 2, 4)));

				// Overflow
				DMibExpectException(List.f_AddArrayAtEnd(NMib::TCLimitsInt<umint>::mc_Max), DMibErrorInstanceListBoundCheck("Vector length would have overflowed"));
			};

			DMibTestSuite("CTAD")
			{
				TCVector Vector0 = {1, 2, 3};
				static_assert(NMib::NTraits::cIsSame<decltype(Vector0), TCVector<int>>);
				DMibExpect(Vector0, ==, TCVector<int>({1, 2, 3}));

				TCVector Vector1{1, 2, 3};
				static_assert(NMib::NTraits::cIsSame<decltype(Vector1), TCVector<int>>);
				DMibExpect(Vector1, ==, TCVector<int>({1, 2, 3}));

				uint32 Array[3] = {1, 2, 3};
				TCVector Vector2(Array, 3);
				static_assert(NMib::NTraits::cIsSame<decltype(Vector2), TCVector<uint32>>);
				DMibExpect(Vector2, ==, TCVector<uint32>({1, 2, 3}));
			};

#if DMibConfig_Memory_Shims_EnableLocal
			DMibTestSuite("Reserve")
			{
				{
					// Destroy empty reserved
					TCVector<int32> TestVector;
					TestVector.f_Reserve(500);
				}

				TCVector<int32> TestVector;

				TestVector.f_Reserve(500);


				{
					CTestMemoryMeasure MeasureMemory("Alloc");
					MeasureMemory.f_Start();
					for (int32 i = 0; i < 500; ++i)
						TestVector.f_Insert(i);

					MeasureMemory.f_Stop(1);
					NMib::NTest::CTestMemoryResult Results;
					MeasureMemory.f_GetResults(Results);
					DMibExpect(Results.m_AllAllocations.m_nAllocations.m_Average, ==, 0.0);
				}

				{
					CTestMemoryMeasure MeasureMemory("Alloc");
					MeasureMemory.f_Start();
					for (int32 i = 0; i < 500; ++i)
						TestVector.f_Insert(i);
					MeasureMemory.f_Stop(1);
					NMib::NTest::CTestMemoryResult Results;
					MeasureMemory.f_GetResults(Results);
					DMibExpect(Results.m_AllAllocations.m_nAllocations.m_Average, >, 0.0);
				}
			};
			DMibTestSuite("ClearNoTrim")
			{
				TCVector<int32> TestVector;

				{
					CTestMemoryMeasure MeasureMemory("Alloc");
					MeasureMemory.f_Start();
					for (int32 i = 0; i < 500; ++i)
						TestVector.f_Insert(i);

					MeasureMemory.f_Stop(1);
					NMib::NTest::CTestMemoryResult Results;
					MeasureMemory.f_GetResults(Results);
					DMibExpect(Results.m_AllAllocations.m_nAllocations.m_Average, >, 0.0);
				}

				TestVector.f_ClearNoTrim();

				{
					CTestMemoryMeasure MeasureMemory("Alloc");
					MeasureMemory.f_Start();
					for (int32 i = 0; i < 500; ++i)
						TestVector.f_Insert(i);
					MeasureMemory.f_Stop(1);
					NMib::NTest::CTestMemoryResult Results;
					MeasureMemory.f_GetResults(Results);
					DMibExpect(Results.m_AllAllocations.m_nAllocations.m_Average, ==, 0.0);
				}

				TestVector.f_ClearNoTrim();
			};
#endif
			auto fRunReplaceTests = [](umint _DestLen, umint _SourceLen, umint _SourceCopyLen, umint _SourceStart, umint _MinLen, umint _bThrowCopy, bool _bDoMove)
				{
					auto fDestVector = [&] -> TCVector<CCountedObject>
						{
							TCVector<CCountedObject> Vector;

							for (umint i = 0; i < _DestLen; ++i)
								Vector.f_Insert(CStr("Dst {}"_f << i));

							return Vector;
						}
					;
					auto fSourceVector = [&] -> TCVector<CCountedObject>
						{
							TCVector<CCountedObject> Vector;

							for (umint i = 0; i < _SourceLen; ++i)
								Vector.f_Insert(CStr("Src {}"_f << i));

							Vector[fg_Min(_SourceStart + _SourceCopyLen / 2, Vector.f_GetLen() - 1)].m_bThrowCopy = _bThrowCopy;
							return Vector;
						}
					;
					auto fDoTest = [&](auto &&_fTest)
						{
							g_nObjects = 0;
							if (_bDoMove || !_bThrowCopy)
							{
								_fTest();
								return;
							}

							try
							{
								_fTest();
							}
							catch (...)
							{
							}
						}
					;

					struct CExpected
					{
						CStr m_Type;
						umint m_Len = 0;
					};

					auto fExpected = [](TCInitializerList<CExpected> _Expected) -> TCVector<CCountedObject>
						{
							umint iDest = 0;
							umint iSource = 0;

							TCVector<CCountedObject> Return;

							for (auto &Expected : _Expected)
							{
								if (Expected.m_Type == "D")
								{
									for (umint i = 0; i < Expected.m_Len; ++i)
									{
										Return.f_Insert().m_Value = "Dst {}"_f << iDest;
										++iDest;
									}
								}
								else if (Expected.m_Type == "S")
								{
									for (umint i = 0; i < Expected.m_Len; ++i)
									{
										Return.f_Insert().m_Value = "Src {}"_f << iSource;
										++iSource;
										++iDest;
									}
								}
								else if (Expected.m_Type == "SI")
									iSource += Expected.m_Len;
								else
								{
									for (umint i = 0; i < Expected.m_Len; ++i)
									{
										Return.f_Insert();
										++iDest;
									}
								}
							}

							return Return;
						}
					;

					umint CopiedSourceLen = fg_Min(_SourceLen - _SourceStart, _SourceCopyLen);

					DMibTestCategory("At start")
					{
						fDoTest
							(
								[&]
								{
									auto TestVector0 = fDestVector();
									DMibExpect(g_nObjects.f_Load(), ==, _DestLen);
									auto TestVector1 = fSourceVector();
									DMibExpect(g_nObjects.f_Load(), ==, (_DestLen + _SourceLen));
									{
										DMibTestPath("After Replace");
										if (_bDoMove)
											TestVector0.f_Replace(fg_Move(TestVector1), _SourceStart, _SourceCopyLen, 0, _MinLen);
										else
											TestVector0.f_Replace(TestVector1, _SourceStart, _SourceCopyLen, 0, _MinLen);
										DMibExpect(g_nObjects.f_Load(), ==,  fg_Max((_DestLen + _SourceLen), _MinLen + _SourceLen));
									}
									DMibExpect(TestVector1[_SourceStart].m_bMovedFrom, == , _bDoMove);
									DMibExpect(TestVector1[_SourceStart].m_bCopiedFrom, == , !_bDoMove);

									umint EndLen = fg_Max(_MinLen, _DestLen) - _DestLen;
									DMibExpect(TestVector0, == , fExpected({{"SI", _SourceStart}, {"S", CopiedSourceLen}, {"D", _DestLen - CopiedSourceLen}, {"", EndLen}}));
								}
							)
						;

						DMibExpect(g_nObjects.f_Load(), ==, 0);
					};
					DMibTestCategory("At end")
					{
						fDoTest
							(
								[&]
								{
									auto TestVector0 = fDestVector();
									DMibExpect(g_nObjects.f_Load(), ==, _DestLen);
									auto TestVector1 = fSourceVector();
									DMibExpect(g_nObjects.f_Load(), ==, (_DestLen + _SourceLen));
									{
										DMibTestPath("After Replace");
										if (_bDoMove)
											TestVector0.f_Replace(fg_Move(TestVector1), _SourceStart, _SourceCopyLen, _DestLen - _SourceLen, _MinLen);
										else
											TestVector0.f_Replace(TestVector1, _SourceStart, _SourceCopyLen, _DestLen - _SourceLen, _MinLen);
										DMibExpect(g_nObjects.f_Load(), ==, fg_Max((_DestLen + _SourceLen), _MinLen + _SourceLen));
									}
									DMibExpect(TestVector1[_SourceStart].m_bMovedFrom, == , _bDoMove);
									DMibExpect(TestVector1[_SourceStart].m_bCopiedFrom, == , !_bDoMove);

									umint EndLen = fg_Max(_MinLen, _DestLen) - _DestLen;
									DMibExpect
										(
											TestVector0
											, ==
											, fExpected({{"SI", _SourceStart}, {"D", _DestLen - _SourceLen}, {"S", CopiedSourceLen}, {"D", _SourceLen - CopiedSourceLen}, {"", EndLen}})
										)
									;
								}
							)
						;
						DMibExpect(g_nObjects.f_Load(), ==, 0);
					};
					DMibTestCategory("Straddling end")
					{
						fDoTest
							(
								[&]
								{
									umint CopyEnd = _DestLen - _SourceLen/2 + CopiedSourceLen;
									umint NewEnd = fg_Max(_DestLen, CopyEnd);
									umint EndLen = fg_Max(_MinLen, NewEnd) - NewEnd;

									auto TestVector0 = fDestVector();
									DMibExpect(g_nObjects.f_Load(), ==, _DestLen);
									auto TestVector1 = fSourceVector();
									DMibExpect(g_nObjects.f_Load(), ==, (_DestLen + _SourceLen));
									{
										DMibTestPath("After Replace");
										if (_bDoMove)
											TestVector0.f_Replace(fg_Move(TestVector1), _SourceStart, _SourceCopyLen, _DestLen - _SourceLen/2, _MinLen);
										else
											TestVector0.f_Replace(TestVector1, _SourceStart, _SourceCopyLen, _DestLen - _SourceLen/2, _MinLen);
										DMibExpect(g_nObjects.f_Load(), ==, fg_Max(_MinLen, NewEnd) + _SourceLen);
									}
									DMibExpect(TestVector1[_SourceStart].m_bMovedFrom, == , _bDoMove);
									DMibExpect(TestVector1[_SourceStart].m_bCopiedFrom, == , !_bDoMove);

									DMibExpect
										(
											TestVector0
											, ==
											, fExpected({{"SI", _SourceStart}, {"D", _DestLen - _SourceLen/2}, {"S", CopiedSourceLen}, {"", EndLen}})
										)
									;
								}
							)
						;
						DMibExpect(g_nObjects.f_Load(), ==, 0);
					};
					DMibTestCategory("Past end")
					{
						fDoTest
							(
								[&]
								{
									umint CopyEnd = _DestLen + CopiedSourceLen;
									umint NewEnd = fg_Max(_DestLen, CopyEnd);
									umint EndLen = fg_Max(_MinLen, NewEnd) - NewEnd;

									auto TestVector0 = fDestVector();
									DMibExpect(g_nObjects.f_Load(), ==, _DestLen);
									auto TestVector1 = fSourceVector();
									DMibExpect(g_nObjects.f_Load(), ==, (_DestLen + _SourceLen));
									{
										DMibTestPath("After Replace");
										if (_bDoMove)
											TestVector0.f_Replace(fg_Move(TestVector1), _SourceStart, _SourceCopyLen, _DestLen, _MinLen);
										else
											TestVector0.f_Replace(TestVector1, _SourceStart, _SourceCopyLen, _DestLen, _MinLen);
										DMibExpect(g_nObjects.f_Load(), ==, fg_Max(_MinLen, NewEnd) + _SourceLen);
									}
									DMibExpect(TestVector1[_SourceStart].m_bMovedFrom, == , _bDoMove);
									DMibExpect(TestVector1[_SourceStart].m_bCopiedFrom, == , !_bDoMove);

									DMibExpect
										(
											TestVector0
											, ==
											, fExpected({{"SI", _SourceStart}, {"D", _DestLen}, {"S", CopiedSourceLen}, {"", EndLen}})
										)
									;
								}
							)
						;
						DMibExpect(g_nObjects.f_Load(), ==, 0);
					};
					DMibTestCategory("Past end 2")
					{
						fDoTest
							(
								[&]
								{
									umint CopyEnd = _DestLen + _SourceLen + CopiedSourceLen;
									umint NewEnd = fg_Max(_DestLen, CopyEnd);
									umint EndLen = fg_Max(_MinLen, NewEnd) - NewEnd;

									auto TestVector0 = fDestVector();
									DMibExpect(g_nObjects.f_Load(), ==, _DestLen);
									auto TestVector1 = fSourceVector();
									DMibExpect(g_nObjects.f_Load(), ==, (_DestLen + _SourceLen));
									{
										DMibTestPath("After Replace");
										if (_bDoMove)
											TestVector0.f_Replace(fg_Move(TestVector1), _SourceStart, _SourceCopyLen, _DestLen + _SourceLen, _MinLen);
										else
											TestVector0.f_Replace(TestVector1, _SourceStart, _SourceCopyLen, (_DestLen + _SourceLen), _MinLen);
										DMibExpect(g_nObjects.f_Load(), ==, fg_Max(_MinLen, NewEnd) + _SourceLen);
									}
									DMibExpect(TestVector1[_SourceStart].m_bMovedFrom, == , _bDoMove);
									DMibExpect(TestVector1[_SourceStart].m_bCopiedFrom, == , !_bDoMove);

									DMibExpect
										(
											TestVector0
											, ==
											, fExpected({{"SI", _SourceStart}, {"D", _DestLen}, {"", _SourceLen}, {"S", CopiedSourceLen}, {"", EndLen}})
										)
									;
								}
							)
						;
						DMibExpect(g_nObjects.f_Load(), ==, 0);
					};
				}
			;

			DMibTestSuite("Replace")
			{
				for (auto MoveCategory : {gc_Str<"Move">.m_Str, gc_Str<"Copy">.m_Str})
				{
					DMibTestCategory(MoveCategory)
					{
						for (auto ExceptionCategory : {gc_Str<"No Exception">.m_Str, gc_Str<"ThrowCopy">.m_Str})
						{
							DMibTestCategory(ExceptionCategory)
							{
								for (auto LenCategory : {gc_Str<"No MinLen">.m_Str, gc_Str<"MinLen">.m_Str})
								{
									DMibTestCategory(LenCategory)
									{
										for (auto OffsetSource : {gc_Str<"No SourceOffset">.m_Str, gc_Str<"SourceOffset">.m_Str})
										{
											DMibTestCategory(OffsetSource)
											{
												for (auto SourceCopyLenCategory : {gc_Str<"SourceCopyLenUnrestricted">.m_Str, gc_Str<"SourceCopyLenRestricted">.m_Str})
												{
													DMibTestCategory(SourceCopyLenCategory)
													{
														umint DestLen = 100;
														umint SourceLen = 20;
														umint SourceCopyLen = SourceCopyLenCategory == "SourceCopyLenUnrestricted" ? SourceLen : (SourceLen * 2) / 3;
														umint SourceStart = OffsetSource == "SourceOffset" ? 3 : 0;

														umint MinLen = LenCategory == "No MinLen" ? 0 : 200;
														umint bThrowCopy = ExceptionCategory == "ThrowCopy";
														bool bDoMove = MoveCategory == "Move";

														fRunReplaceTests(DestLen, SourceLen, SourceCopyLen, SourceStart, MinLen, bThrowCopy, bDoMove);
													};
												}
											};
										}
									};
								}
							};
						}
					};
				}
			};
		}

		struct CExceptionObject
		{
			bool m_bThrowException = false;
			umint m_Dummy[64];

			CExceptionObject() = default;

			CExceptionObject(CExceptionObject const &_Other)
			{
				if (_Other.m_bThrowException)
					DMibError("Test");
			}
			CExceptionObject(CExceptionObject &&_Other)
			{
				if (_Other.m_bThrowException)
					DMibError("Test");
			}
		};

		void f_ExceptionSafety()
		{
			DMibTestSuite("ExceptionSafety")
			{
				TCVector<CExceptionObject> TestVector0;
				TCVector<CExceptionObject> TestVector1;

				DMibExpectExceptionType(TestVector0.f_SetLen(NMib::TCLimitsInt<umint>::mc_Max, false), CExceptionMemory);
				DMibAssert(TestVector0.f_GetLen(), ==, 0);

				{
					DMibTestPath("Complexity");
					umint LastAllocSize = 0;
					TCVector<umint> AllocSizes{0};
					TCVector<umint> ShrinkSizes;
					// Test grow
					for (umint i = 0; i < 10000; ++i)
					{
						TestVector0.f_Insert();
						umint AllocSize = TestVector0.f_GetArrayAllocSize();
						if (AllocSize != LastAllocSize)
						{
							LastAllocSize = AllocSize;
							AllocSizes.f_Insert(AllocSize);
						}
					}
					ShrinkSizes.f_InsertFirst(TestVector0.f_GetArrayAllocSize());
					// Test shrink
					for (umint i = 0; i < 10000; ++i)
					{
						TestVector0.f_PopBack();
						umint AllocSize = TestVector0.f_GetArrayAllocSize();
						if (AllocSize != LastAllocSize)
						{
							LastAllocSize = AllocSize;
							ShrinkSizes.f_InsertFirst(AllocSize);
						}
					}
					if (NMib::NMemory::fg_AllocHasDeterministicSize())
					{
						DMibExpect(AllocSizes, ==, ShrinkSizes);
						DMibExpect(AllocSizes.f_GetLen(), ==, 12);
					}
				}

				{
					DMibTestPath("Insert last");

					TestVector0.f_SetLen(10);
					TestVector1.f_SetLen(10);

					TestVector1[5].m_bThrowException = true;

					DMibExpectExceptionType(TestVector0.f_Insert(TestVector1), CException);
					DMibAssert(TestVector0.f_GetLen(), ==, 10);
				}

				{
					DMibTestPath("Grow");

					TestVector0.f_Grow(20);
					DMibAssert(TestVector0.f_GetLen(), >=, 20u);
					TestVector0.f_SetLen(20);
				}

				{
					DMibTestPath("Remove");

					TestVector0.f_Remove(10, 10);
					DMibAssert(TestVector0.f_GetLen(), ==, 10);
				}

				{
					DMibTestPath("Remove Beginning");
					TestVector0.f_SetLen(20);
					DMibAssert(TestVector0.f_GetLen(), ==, 20);

					TestVector0.f_Remove(0, 10);
					DMibAssert(TestVector0.f_GetLen(), ==, 10);
				}
			};
		}

		void f_Bugs()
		{
			DMibTestSuite("Addition of unsigned offset overflow")
			{
				TCVector<CStr> TestVector;

				TestVector.f_Reserve(2);
				TestVector.f_Clear();

				DMibExpectTrue(TestVector.f_IsEmpty());
			};
		}

		void f_Allocators()
		{
			DMibTestSuite("DefaultAllocator")
			{
				// Default allocator is stateless - can always steal
				DMibTestCategory("MoveConstruct")
				{
					TCVector<CStr> VectorA;
					VectorA.f_Insert("Value1");
					VectorA.f_Insert("Value2");

					CStr *pOriginalData = VectorA.f_GetArray();

					TCVector<CStr> VectorB(fg_Move(VectorA));

					DMibExpect(VectorB.f_GetLen(), ==, 2);
					DMibExpect(VectorA.f_GetLen(), ==, 0);
					DMibExpect(VectorB.f_GetArray(), ==, pOriginalData);
				};
				DMibTestCategory("MoveAssign")
				{
					TCVector<CStr> VectorA;
					VectorA.f_Insert("Value1");
					VectorA.f_Insert("Value2");

					CStr *pOriginalData = VectorA.f_GetArray();

					TCVector<CStr> VectorB;
					VectorB.f_Insert("Existing");
					VectorB = fg_Move(VectorA);

					DMibExpect(VectorB.f_GetLen(), ==, 2);
					DMibExpect(VectorA.f_GetLen(), ==, 0);
					DMibExpect(VectorB.f_GetArray(), ==, pOriginalData);
				};
			};
			DMibTestSuite("StatefulAllocator")
			{
				using CMemoryManagerParams = NMemory::TCMemoryManagerParams<>;
				using CMemoryManager = NMemory::TCMemoryManager<CMemoryManagerParams>;
				using CStatefulAllocator = NMemory::TCAllocator_MemoryManager<CMemoryManagerParams>;
				using CVectorStateful = TCVector<CStr, CStatefulAllocator>;

				CMemoryManager MemoryManagerA{NMemory::CMemoryManagerConfig()};
				CMemoryManager MemoryManagerB{NMemory::CMemoryManagerConfig()};

				DMibTestCategory("MoveConstruct")
				{
					// Move construction copies the allocator from source, so can always steal
					CVectorStateful VectorA(CAllocatorConstructTag(), &MemoryManagerA);
					VectorA.f_Insert("Value1");
					VectorA.f_Insert("Value2");
					VectorA.f_Insert("Value3");

					CStr *pOriginalData = VectorA.f_GetArray();

					// Move construct - should steal because allocator is copied from source
					CVectorStateful VectorB(fg_Move(VectorA));

					DMibExpect(VectorB.f_GetLen(), ==, 3);
					DMibExpect(VectorA.f_GetLen(), ==, 0);
					// Verify pointer was stolen (same address)
					DMibExpect(VectorB.f_GetArray(), ==, pOriginalData);
				};
				DMibTestCategory("MoveAssign")
				{
					DMibTestCategory("SameAllocator")
					{
						CVectorStateful VectorA(CAllocatorConstructTag(), &MemoryManagerA);
						VectorA.f_Insert("Value1");
						VectorA.f_Insert("Value2");

						CStr *pOriginalData = VectorA.f_GetArray();

						CVectorStateful VectorB(CAllocatorConstructTag(), &MemoryManagerA);
						VectorB.f_Insert("Existing");
						VectorB = fg_Move(VectorA);

						DMibExpect(VectorB.f_GetLen(), ==, 2);
						DMibExpect(VectorA.f_GetLen(), ==, 0);
						DMibExpect(VectorB.f_GetArray(), ==, pOriginalData);
					};
					DMibTestCategory("DifferentAllocator")
					{
						CVectorStateful VectorA(CAllocatorConstructTag(), &MemoryManagerA);
						VectorA.f_Insert("Value1");
						VectorA.f_Insert("Value2");

						CStr *pOriginalData = VectorA.f_GetArray();

						CVectorStateful VectorB(CAllocatorConstructTag(), &MemoryManagerB);
						VectorB.f_Insert("Existing");
						VectorB = fg_Move(VectorA);

						DMibExpect(VectorB.f_GetLen(), ==, 2);
						// Source should still have data (was copied, not stolen)
						DMibExpect(VectorA.f_GetLen(), ==, 2);
						DMibExpect(VectorB.f_GetArray(), !=, pOriginalData);
						DMibExpect(VectorB[0], ==, CStr("Value1"));
						DMibExpect(VectorB[1], ==, CStr("Value2"));
					};
				};
			};
			DMibTestSuite("StaticAllocator")
			{
				// Use small static buffer so we can test both static and overflow cases
				using CStaticAllocator = NMemory::TCAllocator_Static<128>;
				using CVectorStatic = TCVector<int32, CStaticAllocator>;

				DMibTestCategory("StaticAllocation")
				{
					// Small data fits in static buffer - should NOT be stolen
					DMibTestCategory("MoveConstruct")
					{
						CVectorStatic VectorA;
						// Use f_SetLen with trim to avoid default 16-element minimum allocation
						VectorA.f_SetLen(2, true);
						VectorA[0] = 1;
						VectorA[1] = 2;

						int32 *pOriginalData = VectorA.f_GetArray();

						// Static allocator - move should copy since static allocations can't be transferred
						CVectorStatic VectorB(fg_Move(VectorA));

						DMibExpect(VectorB.f_GetLen(), ==, 2);
						// Source should still have data (was copied, not stolen)
						DMibExpect(VectorA.f_GetLen(), ==, 2);
						// Verify pointer was NOT stolen
						DMibExpect(VectorB.f_GetArray(), !=, pOriginalData);
						DMibExpect(VectorB[0], ==, 1);
						DMibExpect(VectorB[1], ==, 2);
					};
					DMibTestCategory("MoveAssign")
					{
						CVectorStatic VectorA;
						VectorA.f_SetLen(2, true);
						VectorA[0] = 1;
						VectorA[1] = 2;

						int32 *pOriginalData = VectorA.f_GetArray();

						CVectorStatic VectorB;
						VectorB.f_SetLen(1, true);
						VectorB[0] = 99;
						VectorB = fg_Move(VectorA);

						DMibExpect(VectorB.f_GetLen(), ==, 2);
						// Source should still have data
						DMibExpect(VectorA.f_GetLen(), ==, 2);
						DMibExpect(VectorB.f_GetArray(), !=, pOriginalData);
						DMibExpect(VectorB[0], ==, 1);
						DMibExpect(VectorB[1], ==, 2);
					};
				};
				DMibTestCategory("HeapOverflow")
				{
					// Large data overflows static buffer to heap - CAN be stolen
					DMibTestCategory("MoveConstruct")
					{
						CVectorStatic VectorA;
						// Add enough data to overflow the 128-byte static buffer (128 bytes / 4 bytes per int32 = 32, but need header too)
						for (umint i = 0; i < 50; ++i)
							VectorA.f_Insert((int32)i);

						int32 *pOriginalData = VectorA.f_GetArray();

						// Heap allocation can be stolen since it's not in the static buffer
						CVectorStatic VectorB(fg_Move(VectorA));

						DMibExpect(VectorB.f_GetLen(), ==, 50);
						DMibExpect(VectorA.f_GetLen(), ==, 0);
						// Verify pointer WAS stolen (heap allocation)
						DMibExpect(VectorB.f_GetArray(), ==, pOriginalData);
					};
					DMibTestCategory("MoveAssign")
					{
						CVectorStatic VectorA;
						for (umint i = 0; i < 50; ++i)
							VectorA.f_Insert((int32)i);

						int32 *pOriginalData = VectorA.f_GetArray();

						CVectorStatic VectorB;
						VectorB.f_Insert(99);
						VectorB = fg_Move(VectorA);

						DMibExpect(VectorB.f_GetLen(), ==, 50);
						DMibExpect(VectorA.f_GetLen(), ==, 0);
						// Verify pointer WAS stolen (heap allocation)
						DMibExpect(VectorB.f_GetArray(), ==, pOriginalData);
					};
				};
			};
		}

		void f_DoTests()
		{
			DMibTestCategory("General")
			{
				this->f_General();
			};
			DMibTestCategory("ExceptionSafety")
			{
				this->f_ExceptionSafety();
			};
			DMibTestCategory("Bugs")
			{
				this->f_Bugs();
			};
			DMibTestCategory("Allocators")
			{
				this->f_Allocators();
			};
		}
	};

	DMibTestRegister(CVector_Tests, Malterlib::Container);
}

