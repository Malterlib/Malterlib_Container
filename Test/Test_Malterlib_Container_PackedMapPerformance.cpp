// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Test/Test>
#include <Mib/Test/Performance>
#include <Mib/Container/PackedMap>
#include <Mib/Cryptography/SecureRandom>

#include "../../Core/Source/Malterlib_Core_Misc.h"

namespace NMib::NMisc
{
#if defined(DMibDebug) || defined(DMibSanitizerEnabled)
	constexpr static umint mc_nItemsToTest = 250'000;
#else
	constexpr static umint mc_nItemsToTest = 1'000'000;
#endif
	constexpr static pfp64 mc_AllowedPerformance = 1.0;

	constexpr static umint gc_nTries = 5;

	namespace
	{
		using namespace NMib::NTest;
		using namespace NMib::NTime;
		using namespace NMib::NContainer;

		enum EPopulateMode
		{
			EPopulateMode_Random,
			EPopulateMode_Linear,
			EPopulateMode_LinearBulkLoad
		};

		struct CPackedMapTester
		{
		private: // Measure
			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeysRandomSorted
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
				) const
			{
				auto pDataStart = m_Key_DataSorted.f_GetArray();
				auto pDataEnd = pDataStart + mc_nItemsToTest;
				auto fMeasure = [_fOperation, pDataStart, pDataEnd]() inline_never
					{
						for (auto *pData = pDataStart; pData != pDataEnd; ++pData)
							_fOperation(pData->m_Key);
					}
				;
				for (umint iTry = 0; iTry < gc_nTries; ++iTry)
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
			inline_never void f_MeasureForAllKeysSorted
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
				) const
			{
				if (fp_IsRandom())
					f_MeasureForAllKeysRandomSorted(_PerformanceMeasure, _fOperation, _fInit, _fDestruct);
				else
					f_MeasureForAllKeysLinear(_PerformanceMeasure, _fOperation, _fInit, _fDestruct);
			}

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeysRandom
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
				) const
			{
				auto pDataStart = m_Key_Data.f_GetArray();
				auto pDataEnd = pDataStart + mc_nItemsToTest;
				auto fMeasure = [_fOperation, pDataStart, pDataEnd]() inline_never
					{
						for (auto *pData = pDataStart; pData != pDataEnd; ++pData)
							_fOperation(pData->m_Key);
					}
				;
				for (umint iTry = 0; iTry < gc_nTries; ++iTry)
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
			inline_never void f_MeasureForAllKeysLinear
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
				) const
			{
				auto fMeasure = [_fOperation]() inline_never
					{
						for (umint iTest = 0; iTest < mc_nItemsToTest; ++iTest)
							_fOperation(iTest);
					}
				;
				for (umint iTry = 0; iTry < gc_nTries; ++iTry)
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
			inline_never void f_MeasureForAllKeys
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
				) const
			{
				if (fp_IsRandom())
					f_MeasureForAllKeysRandom(_PerformanceMeasure, _fOperation, _fInit, _fDestruct);
				else
					f_MeasureForAllKeysLinear(_PerformanceMeasure, _fOperation, _fInit, _fDestruct);
			}

			template <typename tf_FOperation, typename tf_FInit, typename tf_FDestruct>
			inline_never void f_MeasureForAllKeyValuesRandom
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
				) const
			{
				auto pDataStart = m_Key_Data.f_GetArray();
				auto pDataEnd = pDataStart + mc_nItemsToTest;
				auto fMeasure = [_fOperation, pDataStart, pDataEnd]() inline_never
					{
						for (auto *pData = pDataStart; pData != pDataEnd; ++pData)
							_fOperation(pData->m_Key, pData->m_Value);
					}
				;
				for (umint iTry = 0; iTry < gc_nTries; ++iTry)
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
			inline_never void f_MeasureForAllKeyValuesLinear
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
				) const
			{
				auto fMeasure = [_fOperation]() inline_never
					{
						for (umint iTest = 0; iTest < mc_nItemsToTest; ++iTest)
							_fOperation(iTest, iTest);
					}
				;
				for (umint iTry = 0; iTry < gc_nTries; ++iTry)
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
			inline_never void f_MeasureForAllKeyValues
				(
					CTestPerformanceMeasure &_PerformanceMeasure
					, tf_FOperation const &_fOperation
					, tf_FInit const &_fInit
					, tf_FDestruct const &_fDestruct
				) const
			{
				if (fp_IsRandom())
					f_MeasureForAllKeyValuesRandom(_PerformanceMeasure, _fOperation, _fInit, _fDestruct);
				else
					f_MeasureForAllKeyValuesLinear(_PerformanceMeasure, _fOperation, _fInit, _fDestruct);
			}

		private: // Generate test data
			template <typename tf_CMap>
			tf_CMap f_GetFilledMap() const
			{
				if (fp_IsRandom())
				{
					tf_CMap Map;
					for (auto &Source : m_Key_Data)
						Map[Source.m_Key] = Source.m_Value;
					return Map;
				}
				else if (fp_IsBulkLoad())
				{
					tf_CMap Map;
					Map.f_BulkLoad(mc_nItemsToTest, [](auto &&_fInsert)
						{
							for (umint iTest = 0; iTest < mc_nItemsToTest; ++iTest)
								_fInsert(iTest, iTest);
						});
					return Map;
				}
				else
				{
					tf_CMap Map;
					for (umint iTest = 0; iTest < mc_nItemsToTest; ++iTest)
						Map[iTest] = iTest;
					return Map;
				}
			}

			template <>
			TCMap<umint, umint> f_GetFilledMap<TCMap<umint, umint>>() const
			{
				if (fp_IsRandom())
				{
					TCMap<umint, umint> Map;
					for (auto &Source : m_Key_Data)
						Map[Source.m_Key] = Source.m_Value;
					return Map;
				}
				else
				{
					TCMap<umint, umint> Map;
					for (umint iTest = 0; iTest < mc_nItemsToTest; ++iTest)
						Map[iTest] = iTest;
					return Map;
				}
			}

		private: // Operations
			void fp_CheckInit()
			{
				if (!fp_IsRandom())
					return;

				if (m_Key_Data.f_IsEmpty())
				{
					m_Key_Data.f_Reserve(mc_nItemsToTest);
					NCryptography::CSecureRandom RandomRng;
					RandomRng.f_InsecureDeterministicReseed(constant_uint64(0xDEADBEEFDEADBEEF));

					for (umint iTest = 0; iTest < mc_nItemsToTest; ++iTest)
						m_Key_Data.f_Insert({RandomRng.f_GetValue<umint>(), RandomRng.f_GetValue<umint>()});

					m_Key_DataSorted = m_Key_Data;
					m_Key_DataSorted.f_Sort();
				}
			}

		public: // Test cases
			CPackedMapTester(EPopulateMode _PopulateMode)
				: m_PopulateMode(_PopulateMode)
			{
			}

			bool fp_IsRandom() const
			{
				return m_PopulateMode == EPopulateMode_Random;
			}

			bool fp_IsBulkLoad() const
			{
				return m_PopulateMode == EPopulateMode_LinearBulkLoad;
			}

			void f_Insert()
			{
				DMibTestSuite("Insert")
				{
					auto Checkout = fg_GetSys()->f_MemoryManager_Checkout();
					bool bDecommit = false;
					fp_CheckInit();

					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure PackedMapMeasure("PackedMap");

					if (fp_IsBulkLoad())
					{
						for (umint iTry = 0; iTry < gc_nTries; ++iTry)
						{
							{
								TCPackedMap<umint, umint> PackedMap;
								[&]() inline_never
									{
										DMibTestScopeMeasure(PackedMapMeasure, mc_nItemsToTest);
										PackedMap.f_BulkLoad
											(
												mc_nItemsToTest
												, [](auto &&_fInsert)
												{
													for (umint iTest = 0; iTest < mc_nItemsToTest; ++iTest)
														_fInsert(iTest, iTest);
												}
											)
										;
									}
									()
								;
							}
							Checkout.f_GarbageCollectLocalArena(bDecommit);
						}
						Checkout.f_GarbageCollectLocalArena(bDecommit);
					}
					else
					{
						TCPackedMap<umint, umint> PackedMap;
						f_MeasureForAllKeyValues
							(
								PackedMapMeasure
								, [&](umint _Key, umint _Value) inline_always_lambda
								{
									PackedMap[_Key] = _Value;
								}
								, []
								{
								}
								, [&]
								{
									PackedMap.f_Clear();
									Checkout.f_GarbageCollectLocalArena(bDecommit);
								}
							)
						;
						Checkout.f_GarbageCollectLocalArena(bDecommit);
					}
					PerfTest.f_Add(PackedMapMeasure);

					CTestPerformanceMeasure MapMeasure("Map");
					TCMap<umint, umint> Map;
					f_MeasureForAllKeyValues
						(
							MapMeasure
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
								Checkout.f_GarbageCollectLocalArena(bDecommit);
							}
						)
					;
					Checkout.f_GarbageCollectLocalArena(bDecommit);
					PerfTest.f_AddReference(MapMeasure);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_Access()
			{
				DMibTestSuite("Access")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure PackedMapMeasure("PackedMap");

					auto PackedMap = f_GetFilledMap<TCPackedMap<umint, umint>>();
					umint FoundPackedMap = 0;
					f_MeasureForAllKeys
						(
							PackedMapMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (PackedMap.f_FindEqual(_Key))
									++FoundPackedMap;
							}
							, []{}
							, []{}
						)
					;
					PerfTest.f_Add(PackedMapMeasure);

					CTestPerformanceMeasure MapMeasure("Map");
					auto Map = f_GetFilledMap<TCMap<umint, umint>>();
					umint FoundMap = 0;
					f_MeasureForAllKeys
						(
							MapMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Map.f_FindEqual(_Key))
									++FoundMap;
							}
							, []{}
							, []{}
						)
					;
					PerfTest.f_AddReference(MapMeasure);
					DMibExpect(FoundPackedMap, ==, mc_nItemsToTest * gc_nTries);
					DMibExpect(FoundMap, ==, mc_nItemsToTest * gc_nTries);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_AccessSorted()
			{
				DMibTestSuite("AccessSorted")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure PackedMapMeasure("PackedMap");

					auto PackedMap = f_GetFilledMap<TCPackedMap<umint, umint>>();
					umint FoundPackedMap = 0;
					f_MeasureForAllKeysSorted
						(
							PackedMapMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (PackedMap.f_FindEqual(_Key))
									++FoundPackedMap;
							}
							, []{}
							, []{}
						)
					;
					PerfTest.f_Add(PackedMapMeasure);

					CTestPerformanceMeasure MapMeasure("Map");
					auto Map = f_GetFilledMap<TCMap<umint, umint>>();
					umint FoundMap = 0;
					f_MeasureForAllKeysSorted
						(
							MapMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Map.f_FindEqual(_Key))
									++FoundMap;
							}
							, []{}
							, []{}
						)
					;
					PerfTest.f_AddReference(MapMeasure);
					DMibExpect(FoundPackedMap, ==, mc_nItemsToTest * gc_nTries);
					DMibExpect(FoundMap, ==, mc_nItemsToTest * gc_nTries);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_Erase()
			{
				DMibTestSuite("Erase")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure PackedMapMeasure("PackedMap");

					TCPackedMap<umint, umint> PackedMap;
					f_MeasureForAllKeys
						(
							PackedMapMeasure
							, [&](umint _Key) inline_always_lambda
							{
								PackedMap.f_Remove(_Key);
							}
							, [&]
							{
								PackedMap = f_GetFilledMap<TCPackedMap<umint, umint>>();
							}
							, [&]
							{
								PackedMap.f_Clear();
							}
						)
					;
					PerfTest.f_Add(PackedMapMeasure);

					CTestPerformanceMeasure MapMeasure("Map");
					TCMap<umint, umint> Map;
					f_MeasureForAllKeys
						(
							MapMeasure
							, [&](umint _Key) inline_always_lambda
							{
								Map.f_Remove(_Key);
							}
							, [&]
							{
								Map = f_GetFilledMap<TCMap<umint, umint>>();
							}
							, [&]
							{
								Map.f_Clear();
							}
						)
					;
					PerfTest.f_AddReference(MapMeasure);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_Clear()
			{
				DMibTestSuite("Clear")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure PackedMapMeasure("PackedMap");

					for (umint iTry = 0; iTry < gc_nTries; ++iTry)
					{
						auto PackedMap = f_GetFilledMap<TCPackedMap<umint, umint>>();
						[&]() inline_never
							{
								DMibTestScopeMeasure(PackedMapMeasure, mc_nItemsToTest);
								PackedMap.f_Clear();
							}
							()
						;
					}

					PerfTest.f_Add(PackedMapMeasure);

					CTestPerformanceMeasure MapMeasure("Map");

					for (umint iTry = 0; iTry < gc_nTries; ++iTry)
					{
						auto Map = f_GetFilledMap<TCMap<umint, umint>>();
						[&]() inline_never
							{
								DMibTestScopeMeasure(MapMeasure, mc_nItemsToTest);
								Map.f_Clear();
							}
							()
						;
					}

					PerfTest.f_AddReference(MapMeasure);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_Copy()
			{
				DMibTestSuite("Copy")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure PackedMapMeasure("PackedMap");

					auto PackedMapSource = f_GetFilledMap<TCPackedMap<umint, umint>>();
					TCPackedMap<umint, umint> PackedMapCopy;
					for (umint iTry = 0; iTry < gc_nTries; ++iTry)
					{
						[&]() inline_never
							{
								DMibTestScopeMeasure(PackedMapMeasure, mc_nItemsToTest);
								PackedMapCopy = PackedMapSource;
							}
							()
						;
						PackedMapCopy.f_Clear();
					}

					PerfTest.f_Add(PackedMapMeasure);

					CTestPerformanceMeasure MapMeasure("Map");
					auto MapSource = f_GetFilledMap<TCMap<umint, umint>>();
					TCMap<umint, umint> MapCopy;
					for (umint iTry = 0; iTry < gc_nTries; ++iTry)
					{
						[&]() inline_never
							{
								DMibTestScopeMeasure(MapMeasure, mc_nItemsToTest);
								MapCopy = MapSource;
							}
							()
						;
						MapCopy.f_Clear();
					}

					PerfTest.f_AddReference(MapMeasure);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_AccessAfterCopy()
			{
				DMibTestSuite("AccessAfterCopy")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure PackedMapMeasure("PackedMap");

					auto PackedMapSource = f_GetFilledMap<TCPackedMap<umint, umint>>();
					auto PackedMap = PackedMapSource;
					umint FoundPackedMap = 0;
					f_MeasureForAllKeys
						(
							PackedMapMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (PackedMap.f_FindEqual(_Key))
									++FoundPackedMap;
							}
							, []{}
							, []{}
						)
					;
					PerfTest.f_Add(PackedMapMeasure);

					CTestPerformanceMeasure MapMeasure("Map");
					auto MapSource = f_GetFilledMap<TCMap<umint, umint>>();
					auto Map = MapSource;
					umint FoundMap = 0;
					f_MeasureForAllKeys
						(
							MapMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Map.f_FindEqual(_Key))
									++FoundMap;
							}
							, []{}
							, []{}
						)
					;
					PerfTest.f_AddReference(MapMeasure);

					DMibExpect(FoundPackedMap, ==, mc_nItemsToTest * gc_nTries);
					DMibExpect(FoundMap, ==, mc_nItemsToTest * gc_nTries);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_AccessAfterCopySorted()
			{
				DMibTestSuite("AccessAfterCopySorted")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					CTestPerformanceMeasure PackedMapMeasure("PackedMap");

					auto PackedMapSource = f_GetFilledMap<TCPackedMap<umint, umint>>();
					auto PackedMap = PackedMapSource;
					umint FoundPackedMap = 0;
					f_MeasureForAllKeysSorted
						(
							PackedMapMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (PackedMap.f_FindEqual(_Key))
									++FoundPackedMap;
							}
							, []{}
							, []{}
						)
					;
					PerfTest.f_Add(PackedMapMeasure);

					CTestPerformanceMeasure MapMeasure("Map");
					auto MapSource = f_GetFilledMap<TCMap<umint, umint>>();
					auto Map = MapSource;
					umint FoundMap = 0;
					f_MeasureForAllKeysSorted
						(
							MapMeasure
							, [&](umint _Key) inline_always_lambda
							{
								if (Map.f_FindEqual(_Key))
									++FoundMap;
							}
							, []{}
							, []{}
						)
					;
					PerfTest.f_AddReference(MapMeasure);

					DMibExpect(FoundPackedMap, ==, mc_nItemsToTest * gc_nTries);
					DMibExpect(FoundMap, ==, mc_nItemsToTest * gc_nTries);
					DMibExpectTrue(PerfTest);
				};
			}

			void f_Iterate()
			{
				DMibTestSuite("Iterate")
				{
					fp_CheckInit();
					CTestPerformance PerfTest(mc_AllowedPerformance);
					{
						CTestPerformanceMeasure PackedMapMeasure("PackedMap");

						auto PackedMap = f_GetFilledMap<TCPackedMap<umint, umint>>();
						umint SumPackedMap = 0;
						for (umint iTry = 0; iTry < gc_nTries; ++iTry)
						{
							[&]() inline_never
								{
									DMibTestScopeMeasure(PackedMapMeasure, mc_nItemsToTest);
									for (auto Iter = PackedMap.f_GetIterator(); Iter; ++Iter)
										SumPackedMap += *Iter;
								}
								()
							;
						}

						PerfTest.f_Add(PackedMapMeasure);
					}

					{
						CTestPerformanceMeasure PackedMapScanMeasure("PackedMapScan");

						auto PackedMap = f_GetFilledMap<TCPackedMap<umint, umint>>();
						umint SumPackedMap = 0;
						for (umint iTry = 0; iTry < gc_nTries; ++iTry)
						{
							[&]() inline_never
								{
									DMibTestScopeMeasure(PackedMapScanMeasure, mc_nItemsToTest);
									PackedMap.f_Scan
										(
											[&](auto &_Key, auto &_Value)
											{
												SumPackedMap += _Value;
											}
										)
									;
								}
								()
							;
						}

						PerfTest.f_Add(PackedMapScanMeasure);
					}

					{
						// This tests the overhead of having the early exit branch in the scan algorithm
						CTestPerformanceMeasure PackedMapScanMeasure("PackedMapScanEarly");

						auto PackedMap = f_GetFilledMap<TCPackedMap<umint, umint>>();
						umint SumPackedMap = 0;
						umint CheckValue = 0;
						for (umint iTry = 0; iTry < gc_nTries; ++iTry)
						{
							[&]() inline_never
								{
									DMibTestScopeMeasure(PackedMapScanMeasure, mc_nItemsToTest);
									PackedMap.f_Scan
										(
											[&](auto &_Key, auto &_Value)
											{
												SumPackedMap += _Value;

												return CheckValue < 0xfffffff0u;
											}
										)
									;
								}
								()
							;
						}

						PerfTest.f_Add(PackedMapScanMeasure);
					}

					{
						CTestPerformanceMeasure MapMeasure("Map");
						auto Map = f_GetFilledMap<TCMap<umint, umint>>();
						umint SumMap = 0;
						for (umint iTry = 0; iTry < gc_nTries; ++iTry)
						{
							[&]() inline_never
								{
									DMibTestScopeMeasure(MapMeasure, mc_nItemsToTest);
									for (auto Iter = Map.f_GetIterator(); Iter; ++Iter)
										SumMap += *Iter;
								}
								()
							;
						}

						PerfTest.f_AddReference(MapMeasure);
					}
					DMibExpectTrue(PerfTest);
				};
			}

			struct CKeyValue
			{
				umint m_Key = 0;
				umint m_Value = 0;

				auto operator <=> (CKeyValue const &_Right) const noexcept = default;
			};

			EPopulateMode m_PopulateMode;
			NContainer::TCVector<CKeyValue> m_Key_Data;
			NContainer::TCVector<CKeyValue> m_Key_DataSorted;
		};

		struct CPackedMapPerformance_Tests : public CTest
		{
			void f_Suite(CPackedMapTester &&_Tester)
			{
				_Tester.f_Insert();
				_Tester.f_Access();
				_Tester.f_AccessSorted();
				_Tester.f_Erase();
				_Tester.f_Clear();
				_Tester.f_Copy();
				_Tester.f_AccessAfterCopy();
				_Tester.f_AccessAfterCopySorted();
				_Tester.f_Iterate();
			}

			void f_DoTests()
			{
				DMibTestCategory(CTestCategory("Performance") << CTestGroup("Performance"))
				{
					DMibTestCategory("Random Input")
					{
						f_Suite(CPackedMapTester(EPopulateMode_Random));
					};
					DMibTestCategory("Linear Input")
					{
						f_Suite(CPackedMapTester(EPopulateMode_Linear));
					};
					DMibTestCategory("Linear Input (BulkLoad)")
					{
						f_Suite(CPackedMapTester(EPopulateMode_LinearBulkLoad));
					};
				};
			}
		};

		DMibTestRegister(CPackedMapPerformance_Tests, Malterlib::Container);
	}
}
