// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/Test/Test>
#include <Mib/Test/Performance>

#include "../../Core/Source/Malterlib_Core_Misc.h"
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace NMib::NMisc
{
#if defined(DMibDebug) || defined(DMibSanitizerEnabled)
	static constexpr umint gc_nItemsToTest = 100'000;
#else
	static constexpr umint gc_nItemsToTest = 5'000'000;
#endif
	static constexpr double gc_AllowedPerformance = 0.95;

	namespace
	{
		using namespace NMib::NTime;
		using namespace NMib::NContainer;

		struct CMapTester
		{
			struct CIntrusiveSetNode
			{
				struct CCompare
				{
					auto &operator () (CIntrusiveSetNode &_Node);
				};

				umint m_Value;
				NIntrusive::TCAVLLink<> m_Link;
			};

			struct CKeyValue
			{
				auto operator <=> (CKeyValue const &_Right) const noexcept = default;

				umint m_Key = 0;
				umint m_Value = 0;
			};

			CMapTester(bool _bRandom);

			void f_InsertSet();

			void f_InsertMap();

			void f_AccessSet();

			void f_CopySet();

			void f_ClearSet();

			void f_AccessAfterCopySet();

			void f_AccessAfterCopySortedSet();

			void f_AccessSortedSet();

			void f_AccessMap();

			void f_EraseSet();

			void f_EraseMap();

			bool m_bRandom;
			NContainer::TCVector<CKeyValue> m_Key_Data;
			NContainer::TCVector<CKeyValue> m_Key_DataSorted;

		private: // Measure
			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void fp_MeasureForAllKeysRandomSorted
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
					, umint _nTries
				)
				const
			;

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void fp_MeasureForAllKeysSorted
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
					, umint _nTries = 5
				)
				const
			;

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void fp_MeasureForAllKeysRandom
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
					, umint _nTries
				)
				const
			;

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void fp_MeasureForAllKeysLinear
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
					, umint _nTries
				)
				const
			;

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void fp_MeasureForAllKeys
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
					, umint _nTries = 5
				)
				const
			;

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void fp_MeasureForAllKeyValuesRandom
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
					, umint _nTries
				)
				const
			;

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void fp_MeasureForAllKeyValuesLinear
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
					, umint _nTries
				)
				const
			;

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void fp_MeasureForAllKeyValues
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
					, umint _nTries = 5
				)
				const
			;

		private: // Generate test data
			template <typename tf_CMap>
			tf_CMap fp_GetFilledMap() const;

			template <typename tf_CSet>
			tf_CSet fp_GetFilledSet() const;

			template <>
			TCMap<umint, umint> fp_GetFilledMap<TCMap<umint, umint>>() const;

			template <>
			TCSet<umint> fp_GetFilledSet<TCSet<umint>>() const;

			void fp_CheckInit();
		};

		auto &CMapTester::CIntrusiveSetNode::CCompare::operator () (CIntrusiveSetNode &_Node)
		{
			return _Node.m_Value;
		}

		CMapTester::CMapTester(bool _bRandom)
			: m_bRandom(_bRandom)
		{
		}

		void CMapTester::f_InsertSet()
		{
			DMibTestSuite("Insert")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");

				auto PrecachSet = fp_GetFilledSet<TCSet<umint>>();

				TCSet<umint> Set;
				fp_MeasureForAllKeys
					(
						MalterlibMeasure
						, [&](umint _Key) inline_always_lambda
						{
							Set[_Key];
						}
						, []
						{
						}
						, [&]
						{
							Set.f_Clear();
						}
					)
				;
				PerfTest.f_Add(MalterlibMeasure);

				CTestPerformanceMeasure StdMeasure("std");
				std::set<umint> Set2;
				fp_MeasureForAllKeys
					(
						StdMeasure
						, [&](umint _Key) inline_always_lambda
						{
							Set2.insert(_Key);
						}
						, []
						{
						}
						, [&]
						{
							Set2.clear();
						}
					)
				;
				PerfTest.f_AddReference(StdMeasure);
				DMibExpectTrue(PerfTest);
			};
		}

		void CMapTester::f_InsertMap()
		{
			DMibTestSuite("Insert")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");

				TCMap<umint, umint> Map;
				fp_MeasureForAllKeyValues
					(
						MalterlibMeasure
						, [&](umint _Key, umint _Value) inline_always_lambda
						{
							Map[_Key] = _Value;
						}
						, []
						{
						}
						, [&]
						{
							Map.f_Clear();
						}
					)
				;
				PerfTest.f_Add(MalterlibMeasure);

				CTestPerformanceMeasure StdMeasure("std");
				std::map<umint, umint> Map2;
				fp_MeasureForAllKeyValues
					(
						StdMeasure
						, [&](umint _Key, umint _Value) inline_always_lambda
						{
							Map2[_Key] = _Value;
						}
						, []
						{
						}
						, [&]
						{
							Map2.clear();
						}
					)
				;
				PerfTest.f_AddReference(StdMeasure);
				DMibExpectTrue(PerfTest);
			};
		}

		void CMapTester::f_AccessSet()
		{
			DMibTestSuite("Access")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");
				auto Set = fp_GetFilledSet<TCSet<umint>>();
				umint nFoundMalterlib = 0;
				fp_MeasureForAllKeys
					(
						MalterlibMeasure
						, [&](umint _Key) inline_always_lambda
						{
							if (Set.f_FindEqual(_Key))
								++nFoundMalterlib;
						}
						, []
						{
						}
						, []
						{
						}
						, 5
					)
				;
				PerfTest.f_Add(MalterlibMeasure);

				CTestPerformanceMeasure StdMeasure("std");
				auto Set2 = fp_GetFilledSet<std::set<umint>>();
				umint nFound = 0;
				fp_MeasureForAllKeys
					(
						StdMeasure
						, [&](umint _Key) inline_always_lambda
						{
							if (Set2.find(_Key) != Set2.end())
								++nFound;
						}
						, []
						{
						}
						, []
						{
						}
						, 5
					)
				;
				PerfTest.f_AddReference(StdMeasure);

				CTestPerformanceMeasure IntrusiveMeasure("IntrusiveVector");
				umint nFoundIntrusive = 0;
				{
					NContainer::TCVector<CIntrusiveSetNode> SetVector3(gc_nItemsToTest * 2);
					CIntrusiveSetNode *pNode = SetVector3.f_GetArray();
					CIntrusiveSetNode *pStartArray = pNode;
					umint CacheConflictPreventionSize = 16 * 1024;
					NIntrusive::TCAVLTree<&CIntrusiveSetNode::m_Link, CIntrusiveSetNode::CCompare> Set3;

					for (auto &Source : Set)
					{
						if ((((uint8 *)pNode - (uint8 *)pStartArray) % CacheConflictPreventionSize) == 0)
							++pNode;

						pNode->m_Value = Source;
						Set3.f_FindEqualOrInsert
							(
								Source
								, [&]() -> CIntrusiveSetNode *
								{
									return pNode;
								}
							)
						;
						++pNode;
					}
					fp_MeasureForAllKeys
						(
							IntrusiveMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Set3.f_FindEqual(_Key))
									++nFoundIntrusive;
							}
							, []
							{
							}
							, []
							{
							}
							, 5
						)
					;
				}

				PerfTest.f_AddBaseline(IntrusiveMeasure);
				DMibExpect(nFoundMalterlib, ==, gc_nItemsToTest * 5);
				DMibExpect(nFoundIntrusive, ==, gc_nItemsToTest * 5);
				DMibExpect(nFound, ==, gc_nItemsToTest * 5);
				DMibExpectTrue(PerfTest);
			};
		}

		void CMapTester::f_CopySet()
		{
			DMibTestSuite("Copy")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");

				auto Set = fp_GetFilledSet<TCSet<umint>>();
				TCSet<umint> SetCopy;
				for (umint iRep = 0; iRep < 5; ++iRep)
				{
					[&]() inline_never
					{
						DMibTestScopeMeasure(MalterlibMeasure, gc_nItemsToTest);
						SetCopy = Set;
					}
					();
					SetCopy.f_Clear();
				}

				PerfTest.f_Add(MalterlibMeasure);
				CTestPerformanceMeasure StdMeasure("std");
				auto Set2 = fp_GetFilledSet<std::set<umint>>();

				std::set<umint> Set2Copy;
				for (umint iRep = 0; iRep < 5; ++iRep)
				{
					[&]() inline_never
					{
						DMibTestScopeMeasure(StdMeasure, gc_nItemsToTest);
						Set2Copy = Set2;
					}
					();
					Set2Copy.clear();
				}

				PerfTest.f_AddReference(StdMeasure);
				DMibExpectTrue(PerfTest);
			};
		}

		void CMapTester::f_ClearSet()
		{
			DMibTestSuite("Clear")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");

				for (umint iRep = 0; iRep < 5; ++iRep)
				{
					auto Set = fp_GetFilledSet<TCSet<umint>>();
					[&]() inline_never
					{
						DMibTestScopeMeasure(MalterlibMeasure, gc_nItemsToTest);
						Set.f_Clear();
					}
					();
				}

				PerfTest.f_Add(MalterlibMeasure);

				CTestPerformanceMeasure StdMeasure("std");

				std::set<umint> Set2Copy;
				for (umint iRep = 0; iRep < 5; ++iRep)
				{
					auto Set2 = fp_GetFilledSet<std::set<umint>>();
					[&]() inline_never
					{
						DMibTestScopeMeasure(StdMeasure, gc_nItemsToTest);
						Set2.clear();
					}
					();
				}

				PerfTest.f_AddReference(StdMeasure);
				DMibExpectTrue(PerfTest);
			};
		}

		void CMapTester::f_AccessAfterCopySet()
		{
			DMibTestSuite("AccessAfterCopy")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");
				auto SetSource = fp_GetFilledSet<TCSet<umint>>();
				auto Set = SetSource;
				umint nFoundMalterlib = 0;
				fp_MeasureForAllKeys
					(
						MalterlibMeasure
						, [&](umint _Key) inline_always_lambda
						{
							if (Set.f_FindEqual(_Key))
								++nFoundMalterlib;
						}
						, []
						{
						}
						, []
						{
						}
						, 5
					)
				;
				PerfTest.f_Add(MalterlibMeasure);

				CTestPerformanceMeasure StdMeasure("std");
				auto SetSource2 = fp_GetFilledSet<std::set<umint>>();
				auto Set2 = SetSource2;
				umint nFound = 0;
				fp_MeasureForAllKeys
					(
						StdMeasure
						, [&](umint _Key) inline_always_lambda
						{
							if (Set2.find(_Key) != Set2.end())
								++nFound;
						}
						, []
						{
						}
						, []
						{
						}
						, 5
					)
				;
				PerfTest.f_AddReference(StdMeasure);

				DMibExpect(nFoundMalterlib, ==, gc_nItemsToTest * 5);
				DMibExpect(nFound, ==, gc_nItemsToTest * 5);
				DMibExpectTrue(PerfTest);
			};
		}

		void CMapTester::f_AccessAfterCopySortedSet()
		{
			DMibTestSuite("AccessAfterCopySorted")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");
				auto SetSource = fp_GetFilledSet<TCSet<umint>>();
				auto Set = SetSource;
				umint nFoundMalterlib = 0;
				fp_MeasureForAllKeysSorted
					(
						MalterlibMeasure
						, [&](umint _Key) inline_always_lambda
						{
							if (Set.f_FindEqual(_Key))
								++nFoundMalterlib;
						}
						, []
						{
						}
						, []
						{
						}
						, 5
					)
				;
				PerfTest.f_Add(MalterlibMeasure);

				CTestPerformanceMeasure StdMeasure("std");
				auto SetSource2 = fp_GetFilledSet<std::set<umint>>();
				auto Set2 = SetSource2;
				umint nFound = 0;
				fp_MeasureForAllKeysSorted
					(
						StdMeasure
						, [&](umint _Key) inline_always_lambda
						{
							if (Set2.find(_Key) != Set2.end())
								++nFound;
						}
						, []
						{
						}
						, []
						{
						}
						, 5
					)
				;
				PerfTest.f_AddReference(StdMeasure);

				DMibExpect(nFoundMalterlib, ==, gc_nItemsToTest * 5);
				DMibExpect(nFound, ==, gc_nItemsToTest * 5);
				DMibExpectTrue(PerfTest);
			};
		}

		void CMapTester::f_AccessSortedSet()
		{
			DMibTestSuite("AccessSorted")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");
				auto Set = fp_GetFilledSet<TCSet<umint>>();
				umint nFoundMalterlib = 0;
				fp_MeasureForAllKeysSorted
					(
						MalterlibMeasure
						, [&](umint _Key) inline_always_lambda
						{
							if (Set.f_FindEqual(_Key))
								++nFoundMalterlib;
						}
						, []
						{
						}
						, []
						{
						}
						, 5
					)
				;
				PerfTest.f_Add(MalterlibMeasure);

				CTestPerformanceMeasure StdMeasure("std");
				auto Set2 = fp_GetFilledSet<std::set<umint>>();
				umint nFound = 0;
				fp_MeasureForAllKeysSorted
					(
						StdMeasure
						, [&](umint _Key) inline_always_lambda
						{
							if (Set2.find(_Key) != Set2.end())
								++nFound;
						}
						, []
						{
						}
						, []
						{
						}
						, 5
					)
				;
				PerfTest.f_AddReference(StdMeasure);

				DMibExpect(nFoundMalterlib, ==, gc_nItemsToTest * 5);
				DMibExpect(nFound, ==, gc_nItemsToTest * 5);
				DMibExpectTrue(PerfTest);
			};
		}

		void CMapTester::f_AccessMap()
		{
			DMibTestSuite("Access")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");

				auto Map = fp_GetFilledMap<TCMap<umint, umint>>();
				umint Found = 0;
				fp_MeasureForAllKeys
					(
						MalterlibMeasure
						, [&](umint _Key) inline_always_lambda
						{
							Found += fg_Const(Map)[_Key];
						}
						, []
						{
						}
						, []
						{
						}
						, 5
					)
				;
				PerfTest.f_Add(MalterlibMeasure);

				CTestPerformanceMeasure StdMeasure("std");
				auto Map2 = fp_GetFilledMap<std::map<umint, umint>>();
				fp_MeasureForAllKeys
					(
						StdMeasure
						, [&](umint _Key) inline_always_lambda
						{
							Found += fg_Const(Map2).find(_Key)->second;
						}
						, []
						{
						}
						, []
						{
						}
						, 5
					)
				;
				PerfTest.f_AddReference(StdMeasure);
				DMibExpectTrue(PerfTest);
			};
		}

		void CMapTester::f_EraseSet()
		{
			DMibTestSuite("Erase")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");

				TCSet<umint> Set;
				fp_MeasureForAllKeys
					(
						MalterlibMeasure
						, [&](umint _Key) inline_always_lambda
						{
							Set.f_Remove(_Key);
						}
						, [&]
						{
							Set = fp_GetFilledSet<TCSet<umint>>();
						}
						, []
						{
						}
					)
				;
				PerfTest.f_Add(MalterlibMeasure);

				CTestPerformanceMeasure StdMeasure("std");
				std::set<umint> Set2;
				fp_MeasureForAllKeys
					(
						StdMeasure
						, [&](umint _Key) inline_always_lambda
						{
							Set2.erase(_Key);
						}
						, [&]
						{
							Set2 = fp_GetFilledSet<std::set<umint>>();
						}
						, []
						{
						}
					)
				;
				PerfTest.f_AddReference(StdMeasure);
				DMibExpectTrue(PerfTest);
			};
		}

		void CMapTester::f_EraseMap()
		{
			DMibTestSuite("Erase")
			{
				fp_CheckInit();
				CTestPerformance PerfTest(gc_AllowedPerformance);
				CTestPerformanceMeasure MalterlibMeasure("Malterlib");

				TCMap<umint, umint> Map;
				fp_MeasureForAllKeys
					(
						MalterlibMeasure
						, [&](umint _Key) inline_always_lambda
						{
							Map.f_Remove(_Key);
						}
						, [&]
						{
							Map = fp_GetFilledMap<TCMap<umint, umint>>();
						}
						, []
						{
						}
					)
				;
				PerfTest.f_Add(MalterlibMeasure);

				CTestPerformanceMeasure StdMeasure("std");
				std::map<umint, umint> Map2;
				fp_MeasureForAllKeys
					(
						StdMeasure
						, [&](umint _Key) inline_always_lambda
						{
							Map2.erase(_Key);
						}
						, [&]
						{
							Map2 = fp_GetFilledMap<std::map<umint, umint>>();
						}
						, []
						{
						}
					)
				;
				PerfTest.f_AddReference(StdMeasure);
				DMibExpectTrue(PerfTest);
			};
		}

		template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
		void CMapTester::fp_MeasureForAllKeysRandomSorted
			(
				CTestPerformanceMeasure &_PerformanceMeasure
				, tf_FOperation const &_fOperation
				, tf_FInit const &_fInit
				, tf_FDestruct const &_fDestruct
				, umint _nTries
			)
			const
		{
			auto pDataStart = m_Key_DataSorted.f_GetArray();
			auto pDataEnd = pDataStart + gc_nItemsToTest;
			auto fMeasure = [_fOperation, pDataStart, pDataEnd]() inline_never
				{
					for (auto *pData = pDataStart; pData != pDataEnd; ++pData)
						_fOperation(pData->m_Key);
				}
			;
			for (umint iTry = 0; iTry < _nTries; ++iTry)
			{
				_fInit();
				{
					DMibTestScopeMeasure(_PerformanceMeasure, gc_nItemsToTest);
					fMeasure();
				}
				_fDestruct();
			}
		}

		template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
		void CMapTester::fp_MeasureForAllKeysSorted
			(
				CTestPerformanceMeasure &_PerformanceMeasure
				, tf_FOperation const &_fOperation
				, tf_FInit const &_fInit
				, tf_FDestruct const &_fDestruct
				, umint _nTries
			)
			const
		{
			if (m_bRandom)
				fp_MeasureForAllKeysRandomSorted(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
			else
				fp_MeasureForAllKeysLinear(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
		}

		template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
		void CMapTester::fp_MeasureForAllKeysRandom
			(
				CTestPerformanceMeasure &_PerformanceMeasure
				, tf_FOperation const &_fOperation
				, tf_FInit const &_fInit
				, tf_FDestruct const &_fDestruct
				, umint _nTries
			)
			const
		{
			auto pDataStart = m_Key_Data.f_GetArray();
			auto pDataEnd = pDataStart + gc_nItemsToTest;
			auto fMeasure = [_fOperation, pDataStart, pDataEnd]() inline_never
				{
					for (auto *pData = pDataStart; pData != pDataEnd; ++pData)
						_fOperation(pData->m_Key);
				}
			;
			for (umint iTry = 0; iTry < _nTries; ++iTry)
			{
				_fInit();
				{
					DMibTestScopeMeasure(_PerformanceMeasure, gc_nItemsToTest);
					fMeasure();
				}
				_fDestruct();
			}
		}

		template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
		void CMapTester::fp_MeasureForAllKeysLinear
			(
				CTestPerformanceMeasure &_PerformanceMeasure
				, tf_FOperation const &_fOperation
				, tf_FInit const &_fInit
				, tf_FDestruct const &_fDestruct
				, umint _nTries
			)
			const
		{
			auto fMeasure = [_fOperation]() inline_never
				{
					for (umint iItem = 0; iItem < gc_nItemsToTest; ++iItem)
						_fOperation(iItem);
				}
			;
			for (umint iTry = 0; iTry < _nTries; ++iTry)
			{
				_fInit();
				{
					DMibTestScopeMeasure(_PerformanceMeasure, gc_nItemsToTest);
					fMeasure();
				}
				_fDestruct();
			}
		}

		template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
		void CMapTester::fp_MeasureForAllKeys
			(
				CTestPerformanceMeasure &_PerformanceMeasure
				, tf_FOperation const &_fOperation
				, tf_FInit const &_fInit
				, tf_FDestruct const &_fDestruct
				, umint _nTries
			)
			const
		{
			if (m_bRandom)
				fp_MeasureForAllKeysRandom(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
			else
				fp_MeasureForAllKeysLinear(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
		}

		template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
		void CMapTester::fp_MeasureForAllKeyValuesRandom
			(
				CTestPerformanceMeasure &_PerformanceMeasure
				, tf_FOperation const &_fOperation
				, tf_FInit const &_fInit
				, tf_FDestruct const &_fDestruct
				, umint _nTries
			)
			const
		{
			auto pDataStart = m_Key_Data.f_GetArray();
			auto pDataEnd = pDataStart + gc_nItemsToTest;
			auto fMeasure = [_fOperation, pDataStart, pDataEnd]() inline_never
				{
					for (auto *pData = pDataStart; pData != pDataEnd; ++pData)
						_fOperation(pData->m_Key, pData->m_Value);
				}
			;
			for (umint iTry = 0; iTry < _nTries; ++iTry)
			{
				_fInit();
				{
					DMibTestScopeMeasure(_PerformanceMeasure, gc_nItemsToTest);
					fMeasure();
				}
				_fDestruct();
			}
		}

		template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
		void CMapTester::fp_MeasureForAllKeyValuesLinear
			(
				CTestPerformanceMeasure &_PerformanceMeasure
				, tf_FOperation const &_fOperation
				, tf_FInit const &_fInit
				, tf_FDestruct const &_fDestruct
				, umint _nTries
			)
			const
		{
			auto fMeasure = [_fOperation]() inline_never
				{
					for (umint iItem = 0; iItem < gc_nItemsToTest; ++iItem)
						_fOperation(iItem, iItem);
				}
			;
			for (umint iTry = 0; iTry < _nTries; ++iTry)
			{
				_fInit();
				{
					DMibTestScopeMeasure(_PerformanceMeasure, gc_nItemsToTest);
					fMeasure();
				}
				_fDestruct();
			}
		}

		template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
		void CMapTester::fp_MeasureForAllKeyValues
			(
				CTestPerformanceMeasure &_PerformanceMeasure
				, tf_FOperation const &_fOperation
				, tf_FInit const &_fInit
				, tf_FDestruct const &_fDestruct
				, umint _nTries
			)
			const
		{
			if (m_bRandom)
				fp_MeasureForAllKeyValuesRandom(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
			else
				fp_MeasureForAllKeyValuesLinear(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
		}

		template <typename tf_CMap>
		tf_CMap CMapTester::fp_GetFilledMap() const
		{
			if (m_bRandom)
			{
				tf_CMap Map;
				for (auto &Source : m_Key_Data)
					Map[Source.m_Key] = Source.m_Value;
				return Map;
			}
			else
			{
				tf_CMap Map;
				for (umint iItem = 0; iItem < gc_nItemsToTest; ++iItem)
					Map[iItem] = iItem;
				return Map;
			}
		}

		template <typename tf_CSet>
		tf_CSet CMapTester::fp_GetFilledSet() const
		{
			if (m_bRandom)
			{
				tf_CSet Set;
				for (auto &Source : m_Key_Data)
					Set.insert(Source.m_Key);
				return Set;
			}
			else
			{
				tf_CSet Set;
				for (umint iItem = 0; iItem < gc_nItemsToTest; ++iItem)
					Set.insert(iItem);
				return Set;
			}
		}

		template <>
		TCMap<umint, umint> CMapTester::fp_GetFilledMap<TCMap<umint, umint>>() const
		{
			if (m_bRandom)
			{
				TCMap<umint, umint> Map;
				for (auto &Source : m_Key_Data)
					Map[Source.m_Key] = Source.m_Value;
				return Map;
			}
			else
			{
				TCMap<umint, umint> Map;
				for (umint iItem = 0; iItem < gc_nItemsToTest; ++iItem)
					Map[iItem] = iItem;
				return Map;
			}
		}

		template <>
		TCSet<umint> CMapTester::fp_GetFilledSet<TCSet<umint>>() const
		{
			if (m_bRandom)
			{
				TCSet<umint> Set;
				for (auto &Source : m_Key_Data)
					Set[Source.m_Key];
				return Set;
			}
			else
			{
				TCSet<umint> Set;
				for (umint iItem = 0; iItem < gc_nItemsToTest; ++iItem)
					Set[iItem];
				return Set;
			}
		}

		void CMapTester::fp_CheckInit()
		{
			if (!m_bRandom)
				return;

			if (m_Key_Data.f_IsEmpty())
			{
				m_Key_Data.f_Reserve(gc_nItemsToTest);
				NMisc::CRandomShiftRNG RandomRng;
				for (umint iItem = 0; iItem < gc_nItemsToTest; ++iItem)
					m_Key_Data.f_Insert({RandomRng.f_GetValue<umint>(), RandomRng.f_GetValue<umint>()});

				m_Key_DataSorted = m_Key_Data;
				m_Key_DataSorted.f_Sort();
			}
		}

		struct CMapPerformance_Tests : public CTest
		{
			void f_Suite(CMapTester &&_Tester);

			void f_DoTests();
		};

		void CMapPerformance_Tests::f_Suite(CMapTester &&_Tester)
		{
			DMibTestCategory("Set")
			{
				_Tester.f_InsertSet();
				_Tester.f_AccessSet();
				_Tester.f_AccessSortedSet();
				_Tester.f_EraseSet();
				_Tester.f_ClearSet();
				_Tester.f_CopySet();
				_Tester.f_AccessAfterCopySet();
				_Tester.f_AccessAfterCopySortedSet();
			};
			DMibTestCategory("Map")
			{
				_Tester.f_InsertMap();
				_Tester.f_AccessMap();
				_Tester.f_EraseMap();
			};
		}

		void CMapPerformance_Tests::f_DoTests()
		{
			DMibTestCategory(CTestCategory("Performace") << CTestGroup("Performance"))
			{
				DMibTestCategory("Random Input")
				{
					f_Suite(CMapTester(true));
				};
				DMibTestCategory("Linear Input")
				{
					f_Suite(CMapTester(false));
				};
			};
		}

		DMibTestRegister(CMapPerformance_Tests, Malterlib::Container);
	}
}
