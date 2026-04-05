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
	static constexpr umint mc_nItemsToTest = 100'000;
#else
	static constexpr umint mc_nItemsToTest = 5'000'000;
#endif
	static constexpr double mc_AllowedPerformance = 0.95;

	namespace
	{
		using namespace NMib::NTime;
		using namespace NMib::NContainer;

		struct CMapTester
		{
		private: // Measure
			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeysRandomSorted(CTestPerformanceMeasure &_PerformanceMeasure, tf_FOperation const &_fOperation, tf_FInit const &_fInit, tf_FDestruct const &_fDestruct, umint _nTries) const
			{
				auto pDataStart = m_Key_DataSorted.f_GetArray();
				auto pDataEnd = pDataStart + mc_nItemsToTest;
				auto fMeasure = [_fOperation, pDataStart, pDataEnd]() inline_never
					{
						for (auto *pData = pDataStart; pData != pDataEnd; ++pData)
							_fOperation(pData->m_Key);
					}
				;
				for (umint i = 0; i < _nTries; ++i)
				{
					_fInit();
					{
						DMibTestScopeMeasure(_PerformanceMeasure, mc_nItemsToTest);
						fMeasure();
					}
					_fDestruct();
				}
			}

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeysSorted(CTestPerformanceMeasure &_PerformanceMeasure, tf_FOperation const &_fOperation, tf_FInit const &_fInit, tf_FDestruct const &_fDestruct, umint _nTries = 5) const
			{
				if (m_bRandom)
					f_MeasureForAllKeysRandomSorted(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
				else
					f_MeasureForAllKeysLinear(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
			}

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeysRandom(CTestPerformanceMeasure &_PerformanceMeasure, tf_FOperation const &_fOperation, tf_FInit const &_fInit, tf_FDestruct const &_fDestruct, umint _nTries) const
			{
				auto pDataStart = m_Key_Data.f_GetArray();
				auto pDataEnd = pDataStart + mc_nItemsToTest;
				auto fMeasure = [_fOperation, pDataStart, pDataEnd]() inline_never
					{
						for (auto *pData = pDataStart; pData != pDataEnd; ++pData)
							_fOperation(pData->m_Key);
					}
				;
				for (umint i = 0; i < _nTries; ++i)
				{
					_fInit();
					{
						DMibTestScopeMeasure(_PerformanceMeasure, mc_nItemsToTest);
						fMeasure();
					}
					_fDestruct();
				}
			}

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeysLinear(CTestPerformanceMeasure &_PerformanceMeasure, tf_FOperation const &_fOperation, tf_FInit const &_fInit, tf_FDestruct const &_fDestruct, umint _nTries) const
			{
				auto fMeasure = [_fOperation]() inline_never
					{
						for (umint i = 0; i < mc_nItemsToTest; ++i)
							_fOperation(i);
					}
				;
				for (umint i = 0; i < _nTries; ++i)
				{
					_fInit();
					{
						DMibTestScopeMeasure(_PerformanceMeasure, mc_nItemsToTest);
						fMeasure();
					}
					_fDestruct();
				}
			}

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeys(CTestPerformanceMeasure &_PerformanceMeasure, tf_FOperation const &_fOperation, tf_FInit const &_fInit, tf_FDestruct const &_fDestruct, umint _nTries = 5) const
			{
				if (m_bRandom)
					f_MeasureForAllKeysRandom(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
				else
					f_MeasureForAllKeysLinear(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
			}

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeyValuesRandom(CTestPerformanceMeasure &_PerformanceMeasure, tf_FOperation const &_fOperation, tf_FInit const &_fInit, tf_FDestruct const &_fDestruct, umint _nTries) const
			{
				auto pDataStart = m_Key_Data.f_GetArray();
				auto pDataEnd = pDataStart + mc_nItemsToTest;
				auto fMeasure = [_fOperation, pDataStart, pDataEnd]() inline_never
					{
						for (auto *pData = pDataStart; pData != pDataEnd; ++pData)
							_fOperation(pData->m_Key, pData->m_Value);
					}
				;
				for (umint i = 0; i < _nTries; ++i)
				{
					_fInit();
					{
						DMibTestScopeMeasure(_PerformanceMeasure, mc_nItemsToTest);
						fMeasure();
					}
					_fDestruct();
				}
			}

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeyValuesLinear(CTestPerformanceMeasure &_PerformanceMeasure, tf_FOperation const &_fOperation, tf_FInit const &_fInit, tf_FDestruct const &_fDestruct, umint _nTries) const
			{
				auto fMeasure = [_fOperation]() inline_never
					{
						for (umint i = 0; i < mc_nItemsToTest; ++i)
							_fOperation(i, i);
					}
				;
				for (umint i = 0; i < _nTries; ++i)
				{
					_fInit();
					{
						DMibTestScopeMeasure(_PerformanceMeasure, mc_nItemsToTest);
						fMeasure();
					}
					_fDestruct();
				}
			}

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeyValues(CTestPerformanceMeasure &_PerformanceMeasure, tf_FOperation const &_fOperation, tf_FInit const &_fInit, tf_FDestruct const &_fDestruct, umint _nTries = 5) const
			{
				if (m_bRandom)
					f_MeasureForAllKeyValuesRandom(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
				else
					f_MeasureForAllKeyValuesLinear(_PerformanceMeasure, _fOperation, _fInit, _fDestruct, _nTries);
			}

		private: // Generate test data
			template <typename tf_CMap>
			tf_CMap f_GetFilledMap() const
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
					for (umint i = 0; i < mc_nItemsToTest; ++i)
						Map[i] = i;
					return Map;
				}
			}

			template <typename tf_CSet>
			tf_CSet f_GetFilledSet() const
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
					for (umint i = 0; i < mc_nItemsToTest; ++i)
						Set.insert(i);
					return Set;
				}
			}

			template <>
			TCMap<umint, umint> f_GetFilledMap<TCMap<umint, umint>>() const
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
					for (umint i = 0; i < mc_nItemsToTest; ++i)
						Map[i] = i;
					return Map;
				}
			}

			template <>
			TCSet<umint> f_GetFilledSet<TCSet<umint>>() const
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
					for (umint i = 0; i < mc_nItemsToTest; ++i)
						Set[i];
					return Set;
				}
			}

		private: // Operations
			void fp_CheckInit()
			{
				if (!m_bRandom)
					return;

				if (m_Key_Data.f_IsEmpty())
				{
					m_Key_Data.f_Reserve(mc_nItemsToTest);
					NMisc::CRandomShiftRNG RandomRng;
					for (umint i = 0; i < mc_nItemsToTest; ++i)
						m_Key_Data.f_Insert({RandomRng.f_GetValue<umint>(), RandomRng.f_GetValue<umint>()});

					m_Key_DataSorted = m_Key_Data;
					m_Key_DataSorted.f_Sort();
				}
			}

		public: // Test cases
			CMapTester(bool random)
				: m_bRandom(random)
			{
			}

			void f_InsertSet()
			{
				DMibTestSuite("Insert")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");

					auto PrecachSet = f_GetFilledSet<TCSet<umint>>();

					TCSet<umint> Set;
					f_MeasureForAllKeys
						(
							MalterlibMeasure
							, [&](umint _Key) inline_always_lambda
							{
								Set[_Key];
							}
							, []{}
							, [&]
							{
								Set.f_Clear();
							}
						)
					;
					PerfTest.f_Add(MalterlibMeasure);

					CTestPerformanceMeasure StdMeasure("std");
					std::set<umint> Set2;
					f_MeasureForAllKeys
						(
							StdMeasure
							, [&](umint _Key) inline_always_lambda
							{
								Set2.insert(_Key);
							}
							, []{}
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

			void f_InsertMap()
			{
				DMibTestSuite("Insert")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");

					TCMap<umint, umint> Map;
					f_MeasureForAllKeyValues
						(
							MalterlibMeasure
							, [&](umint _Key, umint _Value) inline_always_lambda
							{
								Map[_Key] = _Value;
							}
							, []{}
							, [&]
							{
								Map.f_Clear();
							}
						)
					;
					PerfTest.f_Add(MalterlibMeasure);

					CTestPerformanceMeasure StdMeasure("std");
					std::map<umint, umint> Map2;
					f_MeasureForAllKeyValues
						(
							StdMeasure
							, [&](umint _Key, umint _Value) inline_always_lambda
							{
								Map2[_Key] = _Value;
							}
							, []{}
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

			struct CIntrusiveSetNode
			{
				umint m_Value;
				NIntrusive::TCAVLLink<> m_Link;

				struct CCompare
				{
					auto &operator ()(CIntrusiveSetNode &_Node)
					{
						return _Node.m_Value;
					}
				};
			};

			void f_AccessSet()
			{
				DMibTestSuite("Access")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");
					auto Set = f_GetFilledSet<TCSet<umint>>();
					umint nFoundMalterlib = 0;
					f_MeasureForAllKeys
						(
							MalterlibMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Set.f_FindEqual(_Key))
									++nFoundMalterlib;
							}
							, []{}
							, []{}
							, 5
						)
					;
					PerfTest.f_Add(MalterlibMeasure);

					CTestPerformanceMeasure StdMeasure("std");
					auto Set2 = f_GetFilledSet<std::set<umint>>();
					umint nFound = 0;
					f_MeasureForAllKeys
						(
							StdMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Set2.find(_Key) != Set2.end())
									++nFound;
							}
							, []{}
							, []{}
							, 5
						)
					;
					PerfTest.f_AddReference(StdMeasure);

					CTestPerformanceMeasure IntrusiveMeasure("IntrusiveVector");
					umint nFoundIntrusive = 0;
					{
						NContainer::TCVector<CIntrusiveSetNode> SetVector3(mc_nItemsToTest * 2);
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
						f_MeasureForAllKeys
							(
								IntrusiveMeasure
								, [&](umint _Key) inline_always_lambda
								{
									if (Set3.f_FindEqual(_Key))
										++nFoundIntrusive;
								}
								, []{}
								, []{}
								, 5
							)
						;
					}

					PerfTest.f_AddBaseline(IntrusiveMeasure);
					DMibExpect(nFoundMalterlib, ==, mc_nItemsToTest * 5);
					DMibExpect(nFoundIntrusive, ==, mc_nItemsToTest * 5);
					DMibExpect(nFound, ==, mc_nItemsToTest * 5);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_CopySet()
			{
				DMibTestSuite("Copy")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");

					auto Set = f_GetFilledSet<TCSet<umint>>();
					TCSet<umint> SetCopy;
					for (umint i = 0; i < 5; ++i)
					{
						[&]() inline_never
							{
								DMibTestScopeMeasure(MalterlibMeasure, mc_nItemsToTest);
								SetCopy = Set;
							}
							()
						;
						SetCopy.f_Clear();
					}

					PerfTest.f_Add(MalterlibMeasure);
					CTestPerformanceMeasure StdMeasure("std");
					auto Set2 = f_GetFilledSet<std::set<umint>>();

					std::set<umint> Set2Copy;
					for (umint i = 0; i < 5; ++i)
					{
						[&]() inline_never
							{
								DMibTestScopeMeasure(StdMeasure, mc_nItemsToTest);
								Set2Copy = Set2;
							}
							()
						;
						Set2Copy.clear();
					}

					PerfTest.f_AddReference(StdMeasure);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_ClearSet()
			{
				DMibTestSuite("Clear")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");

					for (umint i = 0; i < 5; ++i)
					{
						auto Set = f_GetFilledSet<TCSet<umint>>();
						[&]() inline_never
							{
								DMibTestScopeMeasure(MalterlibMeasure, mc_nItemsToTest);
								Set.f_Clear();
							}
							()
						;
					}

					PerfTest.f_Add(MalterlibMeasure);

					CTestPerformanceMeasure StdMeasure("std");


					std::set<umint> Set2Copy;
					for (umint i = 0; i < 5; ++i)
					{
						auto Set2 = f_GetFilledSet<std::set<umint>>();
						[&]() inline_never
							{
								DMibTestScopeMeasure(StdMeasure, mc_nItemsToTest);
								Set2.clear();
							}
							()
						;
					}

					PerfTest.f_AddReference(StdMeasure);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_AccessAfterCopySet()
			{
				DMibTestSuite("AccessAfterCopy")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");
					auto SetSource = f_GetFilledSet<TCSet<umint>>();
					auto Set = SetSource;
					umint nFoundMalterlib = 0;
					f_MeasureForAllKeys
						(
							MalterlibMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Set.f_FindEqual(_Key))
									++nFoundMalterlib;
							}
							, []{}
							, []{}
							, 5
						)
					;
					PerfTest.f_Add(MalterlibMeasure);

					CTestPerformanceMeasure StdMeasure("std");
					auto SetSource2 = f_GetFilledSet<std::set<umint>>();
					auto Set2 = SetSource2;
					umint nFound = 0;
					f_MeasureForAllKeys
						(
							StdMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Set2.find(_Key) != Set2.end())
									++nFound;
							}
							, []{}
							, []{}
							, 5
						)
					;
					PerfTest.f_AddReference(StdMeasure);

					DMibExpect(nFoundMalterlib, ==, mc_nItemsToTest * 5);
					DMibExpect(nFound, ==, mc_nItemsToTest * 5);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_AccessAfterCopySortedSet()
			{
				DMibTestSuite("AccessAfterCopySorted")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");
					auto SetSource = f_GetFilledSet<TCSet<umint>>();
					auto Set = SetSource;
					umint nFoundMalterlib = 0;
					f_MeasureForAllKeysSorted
						(
							MalterlibMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Set.f_FindEqual(_Key))
									++nFoundMalterlib;
							}
							, []{}
							, []{}
							, 5
						)
					;
					PerfTest.f_Add(MalterlibMeasure);

					CTestPerformanceMeasure StdMeasure("std");
					auto SetSource2 = f_GetFilledSet<std::set<umint>>();
					auto Set2 = SetSource2;
					umint nFound = 0;
					f_MeasureForAllKeysSorted
						(
							StdMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Set2.find(_Key) != Set2.end())
									++nFound;
							}
							, []{}
							, []{}
							, 5
						)
					;
					PerfTest.f_AddReference(StdMeasure);

					DMibExpect(nFoundMalterlib, ==, mc_nItemsToTest * 5);
					DMibExpect(nFound, ==, mc_nItemsToTest * 5);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_AccessSortedSet()
			{
				DMibTestSuite("AccessSorted")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");
					auto Set = f_GetFilledSet<TCSet<umint>>();
					umint nFoundMalterlib = 0;
					f_MeasureForAllKeysSorted
						(
							MalterlibMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Set.f_FindEqual(_Key))
									++nFoundMalterlib;
							}
							, []{}
							, []{}
							, 5
						)
					;
					PerfTest.f_Add(MalterlibMeasure);

					CTestPerformanceMeasure StdMeasure("std");
					auto Set2 = f_GetFilledSet<std::set<umint>>();
					umint nFound = 0;
					f_MeasureForAllKeysSorted
						(
							StdMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Set2.find(_Key) != Set2.end())
									++nFound;
							}
							, []{}
							, []{}
							, 5
						)
					;
					PerfTest.f_AddReference(StdMeasure);

					DMibExpect(nFoundMalterlib, ==, mc_nItemsToTest * 5);
					DMibExpect(nFound, ==, mc_nItemsToTest * 5);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_AccessMap()
			{
				DMibTestSuite("Access")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");

					auto Map = f_GetFilledMap<TCMap<umint, umint>>();
					umint Found = 0;
					f_MeasureForAllKeys
						(
							MalterlibMeasure
							, [&](umint _Key) inline_always_lambda
							{
								Found += fg_Const(Map)[_Key];
							}
							, []{}
							, []{}
							, 5
						)
					;
					PerfTest.f_Add(MalterlibMeasure);

					CTestPerformanceMeasure StdMeasure("std");
					auto Map2 = f_GetFilledMap<std::map<umint, umint>>();
					f_MeasureForAllKeys
						(
							StdMeasure
							, [&](umint _Key) inline_always_lambda
							{
								Found += fg_Const(Map2).find(_Key)->second;
							}
							, []{}
							, []{}
							, 5
						)
					;
					PerfTest.f_AddReference(StdMeasure);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_EraseSet()
			{
				DMibTestSuite("Erase")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");

					TCSet<umint> Set;
					f_MeasureForAllKeys
						(
							MalterlibMeasure
							, [&](umint _Key) inline_always_lambda
							{
								Set.f_Remove(_Key);
							}
							, [&]
							{
								Set = f_GetFilledSet<TCSet<umint>>();
							}
							, []{}
						)
					;
					PerfTest.f_Add(MalterlibMeasure);

					CTestPerformanceMeasure StdMeasure("std");
					std::set<umint> Set2;
					f_MeasureForAllKeys
						(
							StdMeasure
							, [&](umint _Key) inline_always_lambda
							{
								Set2.erase(_Key);
							}
							, [&]
							{
								Set2 = f_GetFilledSet<std::set<umint>>();
							}
							, []{}
						)
					;
					PerfTest.f_AddReference(StdMeasure);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_EraseMap()
			{
				DMibTestSuite("Erase")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure MalterlibMeasure("Malterlib");

					TCMap<umint, umint> Map;
					f_MeasureForAllKeys
						(
							MalterlibMeasure
							, [&](umint _Key) inline_always_lambda
							{
								Map.f_Remove(_Key);
							}
							, [&]
							{
								Map = f_GetFilledMap<TCMap<umint, umint>>();
							}
							, []{}
						)
					;
					PerfTest.f_Add(MalterlibMeasure);

					CTestPerformanceMeasure StdMeasure("std");
					std::map<umint, umint> Map2;
					f_MeasureForAllKeys
						(
							StdMeasure
							, [&](umint _Key) inline_always_lambda
							{
								Map2.erase(_Key);
							}
							, [&]
							{
								Map2 = f_GetFilledMap<std::map<umint, umint>>();
							}
							, []{}
						)
					;
					PerfTest.f_AddReference(StdMeasure);
					DMibExpectTrue(PerfTest);
				};
			}

			struct CKeyValue
			{
				umint m_Key = 0;
				umint m_Value = 0;

				auto operator <=> (CKeyValue const &_Right) const noexcept = default;
			};

			bool m_bRandom;
			NContainer::TCVector<CKeyValue> m_Key_Data;
			NContainer::TCVector<CKeyValue> m_Key_DataSorted;
		};

		struct CMapPerformance_Tests : public CTest
		{
			void f_Suite(CMapTester &&_Tester)
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

			void f_DoTests()
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
		};

		DMibTestRegister(CMapPerformance_Tests, Malterlib::Container);
	}
}
