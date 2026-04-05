// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Memory/MemoryManager>

namespace
{
	using namespace NMib;
	using namespace NMib::NTraits;
	using namespace NMib::NStr;
	using namespace NMib::NContainer;
	using namespace NMib::NStorage;

	class CMap_Tests : public NMib::NTest::CTest
	{
	public:
		void f_Dynamic()
		{
		}

		void f_DoTests()
		{
			DMibTestCategory("Bugs")
			{
				DMibTestSuite("1")
				{
					TCMap<CStr, TCVector<CStr>> Testing;
					CStr Name;

					Name = "subjectAltName";
					Testing[Name].f_Insert("Test 2");

					Name = "MalterlibHostID";
					Testing[Name].f_Insert("Test 1");

					auto *pTest = Testing.f_FindEqual("MalterlibHostID");
					DMibAssertTrue(pTest);

					DMibExpect(*pTest, ==, TCVector<CStr>{"Test 1"});
				};
			};
			DMibTestCategory("Remove")
			{
				DMibTestSuite("Set")
				{
					TCSet<int> Testing;
					static_assert(sizeof(Testing) == sizeof(void *));

					auto &ToRemove = Testing[1];
					Testing.f_Remove(&ToRemove);
				};
				DMibTestSuite("Map")
				{
					TCMap<int, int> Testing;
					static_assert(sizeof(Testing) == sizeof(void *));

					auto &ToRemove = Testing[1];
					Testing.f_Remove(&ToRemove);
				};
			};
			DMibTestCategory("Extract")
			{
				DMibTestSuite("Map")
				{
					TCMap<CStr, CStr> Testing;
					static_assert(sizeof(Testing) == sizeof(void *));

					for (int i = 0; i < 10; ++i)
						Testing[CStr("Key {}"_f << i)] = "Value {}"_f << i;

					TCMap<CStr, CStr> Testing2;
					{
						DMibTestPath("Non-existing");
						auto Handle = Testing.f_Extract("Key 20");
						DMibExpectFalse(Handle);
					}
					{
						DMibTestPath("Remove");
						TCMap<CStr, CStr>::CNodeHandle Handle;
						DMibExpectFalse(Handle);

						Handle = Testing.f_Extract("Key 9");
						DMibExpectTrue(Handle);
						DMibExpect(Testing.f_GetLen(), ==, 9);

						Handle.f_Clear();
					}
					{
						DMibTestPath("By key");
						auto Handle = Testing.f_Extract("Key 5");
						DMibExpectTrue(Handle);
						DMibExpect(Testing.f_GetLen(), ==, 8);

						DMibExpect(Handle.f_Key(), ==, CStr("Key 5"));
						DMibExpect(Handle.f_Value(), ==, CStr("Value 5"));

						Testing2.f_Insert(fg_Move(Handle));
						DMibExpectFalse(Handle);
						DMibExpect(Testing2.f_GetLen(), ==, 1);
					}
					{
						DMibTestPath("By object");
						auto *pValue = Testing.f_FindEqual("Key 7");
						DMibAssertTrue(pValue);

						auto Handle = Testing.f_Extract(pValue);
						DMibExpectTrue(Handle);
						DMibExpect(Testing.f_GetLen(), ==, 7);

						Testing2.f_Insert(fg_Move(Handle));
						DMibExpectFalse(Handle);
						DMibExpect(Testing2.f_GetLen(), ==, 2);
					}
					{
						DMibTestPath("Duplicate");
						auto Handle = Testing.f_Extract("Key 4");
						DMibExpectTrue(Handle);
						DMibExpect(Testing.f_GetLen(), ==, 6);

						auto &ExistingValue = (Testing2["Key 4"] = "Value 4");
						auto &InsertedValue = Testing2.f_Insert(fg_Move(Handle));
						DMibExpect(&ExistingValue, ==, &InsertedValue);
						{
							DMibTestPath("After insert");
							DMibExpectTrue(Handle);
						}
					}
					{
						DMibTestPath("InsertOrAssign existing");
						// Test that InsertOrAssign replaces value when key exists
						TCMap<CStr, CStr> MapA;
						TCMap<CStr, CStr> MapB;

						MapA["Key"] = "Original Value";
						MapB["Key"] = "New Value";

						auto Handle = MapB.f_Extract("Key");
						DMibExpectTrue(Handle);
						DMibExpect(Handle.f_Value(), ==, CStr("New Value"));

						auto &ResultValue = MapA.f_InsertOrAssign(fg_Move(Handle));
						DMibExpect(MapA.f_GetLen(), ==, 1);
						DMibExpect(ResultValue, ==, CStr("New Value"));
						DMibExpect(*MapA.f_FindEqual("Key"), ==, CStr("New Value"));
						{
							DMibTestPath("After assign");
							// Handle stays valid when key exists (node not consumed, only value moved)
							DMibExpectTrue(Handle);
						}
					}
					{
						DMibTestPath("InsertOrAssign non-existing");
						// Test that InsertOrAssign works for new keys too
						TCMap<CStr, CStr> MapA;
						TCMap<CStr, CStr> MapB;

						MapB["NewKey"] = "New Value";

						auto Handle = MapB.f_Extract("NewKey");
						DMibExpectTrue(Handle);

						auto &ResultValue = MapA.f_InsertOrAssign(fg_Move(Handle));
						DMibExpect(MapA.f_GetLen(), ==, 1);
						DMibExpect(ResultValue, ==, CStr("New Value"));
						{
							DMibTestPath("After insert");
							// Handle is empty when key didn't exist (node was consumed)
							DMibExpectFalse(Handle);
						}
					}
					{
						DMibTestPath("ExtractAll");
						Testing.f_ExtractAll
							(
								[&](auto &&_Handle)
								{
									Testing2.f_Insert(fg_Move(_Handle));
								}
							)
						;
						DMibExpect(Testing.f_GetLen(), ==, 0);
						DMibExpect(Testing2.f_GetLen(), ==, 9);
					}

					{
						DMibTestPath("InsertHandle Diff MemoryManager");
						using CMemoryManagerParams = NMemory::TCMemoryManagerParams<>;
						using CMemoryManager = NMemory::TCMemoryManager<CMemoryManagerParams>;
						using CDiffAllocator = NMemory::TCAllocator_MemoryManager<CMemoryManagerParams>;

						CMemoryManager MemoryManagerA{NMemory::CMemoryManagerConfig()};
						CMemoryManager MemoryManagerB{NMemory::CMemoryManagerConfig()};

						TCMap<CStr, CStr, CSort_Default, CDiffAllocator> MapA(CAllocatorConstructTag(), &MemoryManagerA);
						MapA["Key"] = "Value";

						auto Handle = MapA.f_Extract("Key");
						DMibExpectTrue(Handle);
						DMibExpect(MapA.f_GetLen(), ==, 0);

						TCMap<CStr, CStr, CSort_Default, CDiffAllocator> MapB(CAllocatorConstructTag(), &MemoryManagerB);
						MapB.f_Insert(fg_Move(Handle));

						DMibExpectFalse(Handle);
						DMibExpect(MapB.f_GetLen(), ==, 1);
						DMibExpect(MapB.f_FindEqual("Key"), !=, nullptr);
						DMibExpect(*MapB.f_FindEqual("Key"), ==, CStr("Value"));
					}

					{
						DMibTestPath("InsertHandle StaticAllocator");
						using CStaticAllocator = NMemory::TCAllocator_Static<4096>;

						TCMap<CStr, CStr, CSort_Default, CStaticAllocator> MapA;
						MapA["Key"] = "Value";

						auto Handle = MapA.f_Extract("Key");
						DMibExpectTrue(Handle);
						DMibExpect(MapA.f_GetLen(), ==, 0);

						TCMap<CStr, CStr, CSort_Default, CStaticAllocator> MapB;
						MapB.f_Insert(fg_Move(Handle));

						DMibExpectFalse(Handle);
						DMibExpect(MapB.f_GetLen(), ==, 1);
						DMibExpect(MapB.f_FindEqual("Key"), !=, nullptr);
						DMibExpect(*MapB.f_FindEqual("Key"), ==, CStr("Value"));
					}
				};
				DMibTestSuite("Set")
				{
					TCSet<CStr> Testing;
					static_assert(sizeof(Testing) == sizeof(void *));

					for (int i = 0; i < 10; ++i)
						Testing[CStr("Key {}"_f << i)];

					TCSet<CStr> Testing2;
					{
						DMibTestPath("Non-existing");
						auto Handle = Testing.f_Extract("Key 20");
						DMibExpectFalse(Handle);
					}
					{
						DMibTestPath("Remove");
						TCSet<CStr>::CNodeHandle Handle;
						DMibExpectFalse(Handle);

						Handle = Testing.f_Extract("Key 9");
						DMibExpectTrue(Handle);
						DMibExpect(Testing.f_GetLen(), ==, 9);

						Handle.f_Clear();
					}
					{
						DMibTestPath("By key");
						auto Handle = Testing.f_Extract("Key 5");
						DMibExpectTrue(Handle);
						DMibExpect(Testing.f_GetLen(), ==, 8);

						DMibExpect(Handle.f_Key(), ==, CStr("Key 5"));
						DMibExpect(Handle.f_Value(), ==, CStr("Key 5"));

						Testing2.f_Insert(fg_Move(Handle));
						DMibExpectFalse(Handle);
						DMibExpect(Testing2.f_GetLen(), ==, 1);
					}
					{
						DMibTestPath("By object");
						auto *pValue = Testing.f_FindEqual("Key 7");
						DMibAssertTrue(pValue);

						auto Handle = Testing.f_Extract(pValue);
						DMibExpectTrue(Handle);
						DMibExpect(Testing.f_GetLen(), ==, 7);

						Testing2.f_Insert(fg_Move(Handle));
						DMibExpectFalse(Handle);
						DMibExpect(Testing2.f_GetLen(), ==, 2);
					}
					{
						DMibTestPath("Duplicate");

						auto Handle = Testing.f_Extract("Key 4");
						DMibExpectTrue(Handle);
						DMibExpect(Testing.f_GetLen(), ==, 6);

						auto &ExistingValue = Testing2["Key 4"];
						auto &InsertedValue = Testing2.f_Insert(fg_Move(Handle));
						DMibExpect(&ExistingValue, ==, &InsertedValue);
						{
							DMibTestPath("After insert");
							DMibExpectTrue(Handle);
						}
					}
					{
						DMibTestPath("ExtractAll");
						Testing.f_ExtractAll
							(
								[&](auto &&_Handle)
								{
									Testing2.f_Insert(fg_Move(_Handle));
								}
							)
						;
						DMibExpect(Testing.f_GetLen(), ==, 0);
						DMibExpect(Testing2.f_GetLen(), ==, 9);
					}
				};
			};
			DMibTestSuite("MapSetOperations")
			{
				DMibTestCategory("Difference")
				{
					DMibTestCategory("Overlapping")
					{
						TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{3, 30}};
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("MovableLeft")
					{
						TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{3, 30}};
						DMibTest((DMibExpr(fg_Move(Left)) - DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("Disjoint")
					{
						TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}};
						TCMap<int, int> Right = {TCTuple{3, 30}, TCTuple{4, 40}};

						TCMap<int, int> Expected = Left;
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(Expected));
					};
				};

				DMibTestCategory("Xor")
				{
					DMibTestCategory("ConstRight")
					{
						TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("MovableBoth")
					{
						TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest((DMibExpr(fg_Move(Left)) ^ DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("MovableLeft")
					{
						TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest((DMibExpr(fg_Move(Left)) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("MovableRight")
					{
						TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest((DMibExpr(Left) ^ DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("Identical")
					{
						TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}};
						TCMap<int, int> Right = {TCTuple{1, 100}, TCTuple{2, 200}};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr((TCMap<int, int>())));
					};
				};

				DMibTestCategory("Or")
				{
					DMibTestCategory("PreferLeft")
					{
						DMibTestCategory("Overlapping")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}, TCTuple{4, 400}};
							DMibTest(DMibExpr(Left.f_Or<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableLeft")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}, TCTuple{4, 400}};
							DMibTest(DMibExpr(fg_Move(Left).f_Or<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableRight")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}, TCTuple{4, 400}};
							DMibTest(DMibExpr(Left.f_Or<EMapOperationPolicy::mc_PreferLeft>(fg_Move(Right))) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableBoth")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}, TCTuple{4, 400}};
							DMibTest(DMibExpr(fg_Move(Left).f_Or<EMapOperationPolicy::mc_PreferLeft>(fg_Move(Right))) == DMibExpr(Expected));
						};
						DMibTestCategory("Disjoint")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}};
							TCMap<int, int> Right = {TCTuple{3, 30}, TCTuple{4, 40}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}, TCTuple{4, 40}};
							DMibTest(DMibExpr(Left.f_Or<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("Identical")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}};
							TCMap<int, int> Right = {TCTuple{1, 100}, TCTuple{2, 200}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 20}};
							DMibTest(DMibExpr(Left.f_Or<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
						};
					};
					DMibTestCategory("PreferRight")
					{
						DMibTestCategory("Overlapping")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 200}, TCTuple{3, 30}, TCTuple{4, 400}};
							DMibTest(DMibExpr(Left.f_Or<EMapOperationPolicy::mc_PreferRight>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableLeft")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 200}, TCTuple{3, 30}, TCTuple{4, 400}};
							DMibTest(DMibExpr(fg_Move(Left).f_Or<EMapOperationPolicy::mc_PreferRight>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableRight")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 200}, TCTuple{3, 30}, TCTuple{4, 400}};
							DMibTest(DMibExpr(Left.f_Or<EMapOperationPolicy::mc_PreferRight>(fg_Move(Right))) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableBoth")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 200}, TCTuple{3, 30}, TCTuple{4, 400}};
							DMibTest(DMibExpr(fg_Move(Left).f_Or<EMapOperationPolicy::mc_PreferRight>(fg_Move(Right))) == DMibExpr(Expected));
						};
						DMibTestCategory("Identical")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}};
							TCMap<int, int> Right = {TCTuple{1, 100}, TCTuple{2, 200}};

							TCMap<int, int> Expected = {TCTuple{1, 100}, TCTuple{2, 200}};
							DMibTest(DMibExpr(Left.f_Or<EMapOperationPolicy::mc_PreferRight>(Right)) == DMibExpr(Expected));
						};
					};
					DMibTestCategory("Fastest")
					{
						DMibTestCategory("Overlapping")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							// Fastest produces a valid union (all keys present)
							auto Result = Left.f_Or<EMapOperationPolicy::mc_Fastest>(Right);
							DMibExpect(Result.f_GetLen(), ==, 4);
							DMibExpectTrue(Result.f_FindEqual(1) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(2) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(3) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(4) != nullptr);
						};
						DMibTestCategory("MovableLeft")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							auto Result = fg_Move(Left).f_Or<EMapOperationPolicy::mc_Fastest>(Right);
							DMibExpect(Result.f_GetLen(), ==, 4);
							DMibExpectTrue(Result.f_FindEqual(1) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(2) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(3) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(4) != nullptr);
						};
						DMibTestCategory("MovableRight")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							auto Result = Left.f_Or<EMapOperationPolicy::mc_Fastest>(fg_Move(Right));
							DMibExpect(Result.f_GetLen(), ==, 4);
							DMibExpectTrue(Result.f_FindEqual(1) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(2) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(3) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(4) != nullptr);
						};
						DMibTestCategory("MovableBoth")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

							auto Result = fg_Move(Left).f_Or<EMapOperationPolicy::mc_Fastest>(fg_Move(Right));
							DMibExpect(Result.f_GetLen(), ==, 4);
						};
					};
				};

				DMibTestCategory("And")
				{
					DMibTestCategory("PreferLeft")
					{
						DMibTestCategory("Overlapping")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{2, 20}, TCTuple{3, 30}};  // Left values kept
							DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableLeft")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{2, 20}, TCTuple{3, 30}};
							DMibTest(DMibExpr(fg_Move(Left).f_And<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableRight")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{2, 20}, TCTuple{3, 30}};
							DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferLeft>(fg_Move(Right))) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableBoth")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{2, 20}, TCTuple{3, 30}};
							DMibTest(DMibExpr(fg_Move(Left).f_And<EMapOperationPolicy::mc_PreferLeft>(fg_Move(Right))) == DMibExpr(Expected));
						};
						DMibTestCategory("Disjoint")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}};
							TCMap<int, int> Right = {TCTuple{3, 30}, TCTuple{4, 40}};

							TCMap<int, int> Expected;
							DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("Identical")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}};
							TCMap<int, int> Right = {TCTuple{1, 100}, TCTuple{2, 200}};

							TCMap<int, int> Expected = {TCTuple{1, 10}, TCTuple{2, 20}};
							DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("LeftExhaustedFirst")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{2, 20}};
							DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("RightExhaustedFirst")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{1, 100}};

							TCMap<int, int> Expected = {TCTuple{1, 10}};
							DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
						};
					};
					DMibTestCategory("PreferRight")
					{
						DMibTestCategory("Overlapping")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{2, 200}, TCTuple{3, 300}};  // Right values kept
							DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferRight>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableLeft")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{2, 200}, TCTuple{3, 300}};
							DMibTest(DMibExpr(fg_Move(Left).f_And<EMapOperationPolicy::mc_PreferRight>(Right)) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableRight")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{2, 200}, TCTuple{3, 300}};
							DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferRight>(fg_Move(Right))) == DMibExpr(Expected));
						};
						DMibTestCategory("MovableBoth")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							TCMap<int, int> Expected = {TCTuple{2, 200}, TCTuple{3, 300}};
							DMibTest(DMibExpr(fg_Move(Left).f_And<EMapOperationPolicy::mc_PreferRight>(fg_Move(Right))) == DMibExpr(Expected));
						};
						DMibTestCategory("Identical")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}};
							TCMap<int, int> Right = {TCTuple{1, 100}, TCTuple{2, 200}};

							TCMap<int, int> Expected = {TCTuple{1, 100}, TCTuple{2, 200}};
							DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferRight>(Right)) == DMibExpr(Expected));
						};
					};
					DMibTestCategory("Fastest")
					{
						DMibTestCategory("Overlapping")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							// Fastest produces a valid intersection (correct keys)
							auto Result = Left.f_And<EMapOperationPolicy::mc_Fastest>(Right);
							DMibExpect(Result.f_GetLen(), ==, 2);
							DMibExpectTrue(Result.f_FindEqual(2) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(3) != nullptr);
						};
						DMibTestCategory("MovableLeft")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							auto Result = fg_Move(Left).f_And<EMapOperationPolicy::mc_Fastest>(Right);
							DMibExpect(Result.f_GetLen(), ==, 2);
							DMibExpectTrue(Result.f_FindEqual(2) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(3) != nullptr);
						};
						DMibTestCategory("MovableRight")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							auto Result = Left.f_And<EMapOperationPolicy::mc_Fastest>(fg_Move(Right));
							DMibExpect(Result.f_GetLen(), ==, 2);
							DMibExpectTrue(Result.f_FindEqual(2) != nullptr);
							DMibExpectTrue(Result.f_FindEqual(3) != nullptr);
						};
						DMibTestCategory("MovableBoth")
						{
							TCMap<int, int> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
							TCMap<int, int> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

							auto Result = fg_Move(Left).f_And<EMapOperationPolicy::mc_Fastest>(fg_Move(Right));
							DMibExpect(Result.f_GetLen(), ==, 2);
						};
					};
				};

				DMibTestCategory("CustomCompare")
				{
					struct CCompareReverse
					{
						auto operator () (int const &_Left, int const &_Right) const
						{
							return _Right <=> _Left;
						}
					};

					DMibTestCategory("Xor")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("XorMove")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest((DMibExpr(fg_Move(Left)) ^ DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("XorMoveLeft")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest((DMibExpr(fg_Move(Left)) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("XorMoveRight")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest((DMibExpr(Left) ^ DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("Difference")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{3, 30}};
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("DifferenceMove")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{3, 30}};
						DMibTest((DMibExpr(fg_Move(Left)) - DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("DifferenceMoveLeft")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{3, 30}};
						DMibTest((DMibExpr(fg_Move(Left)) - DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("DifferenceMoveRight")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{3, 30}};
						DMibTest((DMibExpr(Left) - DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("OrPreferLeft")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest(DMibExpr(Left.f_Or<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("OrPreferLeftMove")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest(DMibExpr(fg_Move(Left).f_Or<EMapOperationPolicy::mc_PreferLeft>(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("OrPreferRight")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{2, 200}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest(DMibExpr(Left.f_Or<EMapOperationPolicy::mc_PreferRight>(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("OrPreferRightMove")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{1, 10}, TCTuple{2, 200}, TCTuple{3, 30}, TCTuple{4, 400}};
						DMibTest(DMibExpr(fg_Move(Left).f_Or<EMapOperationPolicy::mc_PreferRight>(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("AndPreferLeft")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{2, 20}, TCTuple{3, 30}};
						DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferLeft>(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("AndPreferLeftMove")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{2, 20}, TCTuple{3, 30}};
						DMibTest(DMibExpr(fg_Move(Left).f_And<EMapOperationPolicy::mc_PreferLeft>(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("AndPreferRight")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{2, 200}, TCTuple{3, 300}};
						DMibTest(DMibExpr(Left.f_And<EMapOperationPolicy::mc_PreferRight>(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("AndPreferRightMove")
					{
						TCMap<int, int, CCompareReverse> Left = {TCTuple{1, 10}, TCTuple{2, 20}, TCTuple{3, 30}};
						TCMap<int, int, CCompareReverse> Right = {TCTuple{2, 200}, TCTuple{3, 300}, TCTuple{4, 400}};

						TCMap<int, int, CCompareReverse> Expected = {TCTuple{2, 200}, TCTuple{3, 300}};
						DMibTest(DMibExpr(fg_Move(Left).f_And<EMapOperationPolicy::mc_PreferRight>(fg_Move(Right))) == DMibExpr(Expected));
					};
				};
			};
			DMibTestSuite("Misc")
			{
				TCSet<int> Testing;
				static_assert(sizeof(Testing) == sizeof(void *));

				Testing[2];
				Testing[3];

				TCSet<int> Testing2;
				Testing2[2];
				Testing2[3];

				Testing2 = fg_Move(Testing);
			};
			DMibTestSuite("Copy Set")
			{
				TCSet<int32> Source;

				for (int32 i = 0; i < 50; ++i)
					Source[i];

				TCSet<int32> Copied = Source;
				DMibExpect(Copied, ==, Source);

				TCSet<int32> Moved = fg_Move(Copied);
				DMibExpect(Moved, ==, Source);

				TCSet<int64> CopiedOtherType = Source;
				DMibExpect(CopiedOtherType, ==, Source);
			};
			DMibTestSuite("Copy Map")
			{
				TCMap<int32, int32> Source;

				for (int32 i = 0; i < 50; ++i)
					Source[i] = i;

				TCMap<int32, int32> Copied = Source;
				DMibExpect(Copied, ==, Source);

				TCMap<int32, int32> Moved = fg_Move(Copied);
				DMibExpect(Moved, ==, Source);

				TCMap<int32, int64> CopiedOtherType = Source;
				DMibExpect(CopiedOtherType, ==, Source);

				for (int32 i = 50; i < 100; ++i)
				{
					Source[i] = i;
					CopiedOtherType[i] = i;
				}
				{
					DMibTestPath("After addition");
					DMibExpect(CopiedOtherType, ==, Source);
				}
			};
			DMibTestSuite("Iterator")
			{
				TCSet<int> Testing;
				static_assert(sizeof(Testing) == sizeof(void *));

				for (int i = 0; i < 10; ++i)
					Testing[i];

				auto Iter = Testing.f_GetIterator();
				{
					DMibTestPath("Forward");
					DMibTest(DMibExpr(*Iter) == DMibExpr(0));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(2));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(3));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(4));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(5));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(6));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(7));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(8));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(9));
					auto IterCopy = Iter;
					++IterCopy;
					DMibTest(DMibExpr(!IterCopy));
				}
			};
			DMibTestSuite("Bidirectional Iterator")
			{

				TCSet<int> Testing;

				static_assert(sizeof(Testing) == sizeof(void *));
				for (int i = 0; i < 10; ++i)
					Testing[i];

				auto Iter = Testing.f_GetIteratorBidirectional();
				{
					DMibTestPath("Forward");
					DMibTest(DMibExpr(*Iter) == DMibExpr(0));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(2));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(3));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(4));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(5));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(6));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(7));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(8));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(9));
					auto IterCopy = Iter;
					++IterCopy;
					DMibTest(DMibExpr(!IterCopy));
				}
				{
					DMibTestPath("Backward");
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(8));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(7));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(6));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(5));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(4));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(3));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(2));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(1));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(0));
					--Iter;
					DMibTest(DMibExpr(!Iter));
				}
			};
			DMibTestSuite("Reverse Iterator")
			{

				TCSet<int> Testing;
				static_assert(sizeof(Testing) == sizeof(void *));

				for (int i = 0; i < 10; ++i)
					Testing[i];

				auto Iter = Testing.f_GetIteratorReverse();
				{
					DMibTestPath("Forward");
					DMibTest(DMibExpr(*Iter) == DMibExpr(9));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(8));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(7));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(6));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(5));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(4));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(3));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(2));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(0));
					auto IterCopy = Iter;
					++IterCopy;
					DMibTest(DMibExpr(!IterCopy));
				}
			};
			DMibTestSuite("Bidirectional Reverse Iterator")
			{

				TCSet<int> Testing;
				static_assert(sizeof(Testing) == sizeof(void *));

				for (int i = 0; i < 10; ++i)
					Testing[i];

				auto Iter = Testing.f_GetIteratorBidirectionalReverse();
				{
					DMibTestPath("Forward");
					DMibTest(DMibExpr(*Iter) == DMibExpr(9));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(8));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(7));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(6));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(5));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(4));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(3));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(2));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(0));
					auto IterCopy = Iter;
					++IterCopy;
					DMibTest(DMibExpr(!IterCopy));
				}
				{
					DMibTestPath("Backward");
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(1));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(2));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(3));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(4));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(5));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(6));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(7));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(8));
					--Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(9));
					--Iter;
					DMibTest(DMibExpr(!Iter));
				}
			};
			DMibTestSuite("Reference Iterator")
			{

				int Values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

				TCMap<int, int const &> Testing;
				static_assert(sizeof(Testing) == sizeof(void *));

				for (int i = 0; i < 10; ++i)
					Testing[i, Values[i]];

				auto Iter = Testing.f_GetIterator();
				{
					DMibTestPath("Forward");
					DMibTest(DMibExpr(*Iter) == DMibExpr(0));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(2));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(3));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(4));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(5));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(6));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(7));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(8));
					++Iter;
					DMibTest(DMibExpr(*Iter) == DMibExpr(9));
					auto IterCopy = Iter;
					++IterCopy;
					DMibTest(DMibExpr(!IterCopy));
				}
			};
			DMibTestSuite("CTAD Map")
			{
				TCMap Map0{TCTuple{1, 2}, {3 ,4}, {5, 6}};
				static_assert(cIsSame<decltype(Map0), TCMap<int, int>>);
				DMibExpect(Map0, ==, (TCMap<int, int>({{1, 2}, {3 ,4}, {5, 6}})));
			};
			DMibTestSuite("CTAD Set")
			{
				TCSet Set0 = {1, 2, 3};
				static_assert(cIsSame<decltype(Set0), TCSet<int>>);
				DMibExpect(Set0, ==, TCSet<int>({1, 2, 3}));

				TCSet Set1{1, 2, 3};
				static_assert(cIsSame<decltype(Set1), TCSet<int>>);
				DMibExpect(Set1, ==, TCSet<int>({1, 2, 3}));
			};
			DMibTestSuite("Key Iterator")
			{
				{
					DMibTestPath("Set");
					TCSet<int> Testing;
					static_assert(sizeof(Testing) == sizeof(void *));

					for (int i = 0; i < 10; ++i)
						Testing[i];

					int iTest = 0;
					for (auto &Key : Testing.f_Keys())
					{
						DMibExpect(Key, ==, iTest)(ETestFlag_Aggregated);
						++iTest;
					}
				}
				{
					DMibTestPath("Map");
					TCMap<int, CStr> Testing;
					static_assert(sizeof(Testing) == sizeof(void *));

					for (int i = 0; i < 10; ++i)
						Testing(i, CStr::fs_ToStr(i));

					int iTest = 0;
					for (auto &Key : Testing.f_Keys())
					{
						DMibExpect(Key, ==, iTest)(ETestFlag_Aggregated);
						++iTest;
					}
				}
			};
			DMibTestSuite("Entries Iterator")
			{
				{
					DMibTestPath("Set");
					TCSet<int> Testing;
					static_assert(sizeof(Testing) == sizeof(void *));

					for (int i = 0; i < 10; ++i)
						Testing[i];

					int iTest = 0;
					for (auto &Key : Testing.f_Entries())
					{
						DMibExpect(Key.f_Key(), ==, iTest)(ETestFlag_Aggregated);
						DMibExpect(Key.f_Value(), ==, iTest)(ETestFlag_Aggregated);
						++iTest;
					}
				}
				{
					DMibTestPath("Map");
					TCMap<int, CStr> Testing;
					static_assert(sizeof(Testing) == sizeof(void *));

					for (int i = 0; i < 10; ++i)
						Testing(i, CStr::fs_ToStr(i));

					int iTest = 0;
					for (auto &Key : Testing.f_Entries())
					{
						DMibExpect(Key.f_Key(), ==, iTest)(ETestFlag_Aggregated);
						DMibExpect(Key.f_Value(), ==, CStr::fs_ToStr(iTest))(ETestFlag_Aggregated);
						++iTest;
					}
				}
			};
			DMibTestSuite("Custom Compare")
			{
				{
					DMibTestPath("Static");
					struct CCompare
					{
						auto operator ()(int const &_Left, int const &_Right) const
						{
							return _Right <=> _Left;
						}
					};

					{
						DMibTestPath("Set");
						TCSet<int, CCompare> Testing;
						static_assert(sizeof(Testing) == sizeof(void *));
						for (int i = 0; i < 10; ++i)
							Testing[i];

						int iTest = 9;
						for (auto &Key : Testing.f_Entries())
						{
							DMibExpect(Key.f_Key(), ==, iTest)(ETestFlag_Aggregated);
							DMibExpect(Key.f_Value(), ==, iTest)(ETestFlag_Aggregated);
							--iTest;
						}
					}
					{
						DMibTestPath("Map");
						TCMap<int, CStr, CCompare> Testing;
						static_assert(sizeof(Testing) == sizeof(void *));
						for (int i = 0; i < 10; ++i)
							Testing(i, CStr::fs_ToStr(i));

						int iTest = 9;
						for (auto &Key : Testing.f_Entries())
						{
							DMibExpect(Key.f_Key(), ==, iTest)(ETestFlag_Aggregated);
							DMibExpect(Key.f_Value(), ==, CStr::fs_ToStr(iTest))(ETestFlag_Aggregated);
							--iTest;
						}
					}
				}

				auto fInitSet = []<umint tf_ExpectedSize = sizeof(void *) * 2>(auto &_Set, TCCompileTimeConstant<umint, tf_ExpectedSize> _x = TCCompileTimeConstant<umint, sizeof(void *) * 2>())
					{
//						static_assert(sizeof(_Set) == tf_ExpectedSize);

						for (int i = 0; i < 10; ++i)
							_Set[i];
					}
				;
				auto fInitMap = []<umint tf_ExpectedSize = sizeof(void *) * 2>(auto &_Map, TCCompileTimeConstant<umint, tf_ExpectedSize> _x = TCCompileTimeConstant<umint, sizeof(void *) * 2>())
					{
						static_assert(sizeof(_Map) == tf_ExpectedSize);

						for (int i = 0; i < 10; ++i)
							_Map(i, CStr::fs_ToStr(i));
					}
				;

				auto fCheckSet = [](auto &_Set)
					{
						int iTest = 0;
						for (auto &Key : _Set.f_Entries())
						{
							DMibExpect(Key.f_Key(), ==, iTest)(ETestFlag_Aggregated);
							DMibExpect(Key.f_Value(), ==, iTest)(ETestFlag_Aggregated);
							++iTest;
						}
					}
				;
				auto fCheckMap = [](auto &_Map)
					{
						int iTest = 0;
						for (auto &Key : _Map.f_Entries())
						{
							DMibExpect(Key.f_Key(), ==, iTest)(ETestFlag_Aggregated);
							DMibExpect(Key.f_Value(), ==, CStr::fs_ToStr(iTest))(ETestFlag_Aggregated);
							++iTest;
						}
					}
				;

				auto fCheckSetReverse = [](auto &_Set)
					{
						int iTest = 9;
						for (auto &Key : _Set.f_Entries())
						{
							DMibExpect(Key.f_Key(), ==, iTest)(ETestFlag_Aggregated);
							DMibExpect(Key.f_Value(), ==, iTest)(ETestFlag_Aggregated);
							--iTest;
						}
					}
				;
				auto fCheckMapReverse = [](auto &_Map)
					{
						int iTest = 9;
						for (auto &Key : _Map.f_Entries())
						{
							DMibExpect(Key.f_Key(), ==, iTest)(ETestFlag_Aggregated);
							DMibExpect(Key.f_Value(), ==, CStr::fs_ToStr(iTest))(ETestFlag_Aggregated);
							--iTest;
						}
					}
				;
				{
					DMibTestPath("Stateful");
					struct CCompare
					{
						auto operator ()(int const &_Left, int const &_Right) const
						{
							if (m_bReverse)
								return _Right <=> _Left;
							else
								return _Left <=> _Right;
						}

						bool m_bReverse = false;
					};

					struct CCompareParam
					{
						CCompareParam(bool _bReverse)
							: m_bReverse(_bReverse)
						{
						}

						auto operator ()(int const &_Left, int const &_Right) const
						{
							if (m_bReverse)
								return _Right <=> _Left;
							else
								return _Left <=> _Right;
						}

						bool m_bReverse = false;
					};

					{
						DMibTestPath("Default");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> Testing;
							fInitSet(Testing);
							fCheckSet(Testing);
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> Testing;
							fInitMap(Testing);
							fCheckMap(Testing);
						}
					}
					{
						DMibTestPath("Explicit");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> Testing(CCompareConstructTag(), CCompare{.m_bReverse = false});
							fInitSet(Testing);
							fCheckSet(Testing);
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> Testing{CCompareConstructTag(), CCompare{.m_bReverse = false}};
							fInitMap(Testing);

							int iTest = 0;
							for (auto &Key : Testing.f_Entries())
							{
								DMibExpect(Key.f_Key(), ==, iTest)(ETestFlag_Aggregated);
								DMibExpect(Key.f_Value(), ==, CStr::fs_ToStr(iTest))(ETestFlag_Aggregated);
								++iTest;
							}
						}
					}
					{
						DMibTestPath("Reversed");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> Testing(CCompareConstructTag(), CCompare{.m_bReverse = true});
							fInitSet(Testing);
							fCheckSetReverse(Testing);
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> Testing(CCompareConstructTag(), CCompare{.m_bReverse = true});
							fInitMap(Testing);
							fCheckMapReverse(Testing);
						}
					}
					{
						DMibTestPath("ReversedParam");
						{
							DMibTestPath("Set");
							TCSet<int, CCompareParam> Testing(CCompareConstructTag(), true);
							fInitSet(Testing);
							fCheckSetReverse(Testing);
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompareParam> Testing(CCompareConstructTag(), true);
							fInitMap(Testing);
							fCheckMapReverse(Testing);
						}
					}
					{
						DMibTestPath("ReversedCopy");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCSet<int, CCompare> Testing(TestingSource);
							fInitSet(Testing);
							fCheckSetReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCSet<int, CCompare> TestingPost(TestingSource);
								fCheckSetReverse(TestingPost);
							}
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCMap<int, CStr, CCompare> Testing(TestingSource);
							fInitMap(Testing);
							fCheckMapReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCMap<int, CStr, CCompare> TestingPost(TestingSource);
								fCheckMapReverse(TestingPost);
							}
						}
					}
					{
						DMibTestPath("ReversedMove");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCSet<int, CCompare> Testing(fg_Move(TestingSource));
							fInitSet(Testing);
							fCheckSetReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCSet<int, CCompare> TestingPost(fg_Move(TestingSource));
								fCheckSetReverse(TestingPost);
							}
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCMap<int, CStr, CCompare> Testing(fg_Move(TestingSource));
							fInitMap(Testing);
							fCheckMapReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCMap<int, CStr, CCompare> TestingPost(fg_Move(TestingSource));
								fCheckMapReverse(TestingPost);
							}
						}
					}
					{
						DMibTestPath("ReversedAssign");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCSet<int, CCompare> Testing;
							Testing = TestingSource;
							fInitSet(Testing);
							fCheckSetReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCSet<int, CCompare> TestingPost;
								TestingPost = TestingSource;
								fCheckSetReverse(TestingPost);
							}
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCMap<int, CStr, CCompare> Testing;
							Testing = TestingSource;
							fInitMap(Testing);
							fCheckMapReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCMap<int, CStr, CCompare> TestingPost;
								TestingPost = TestingSource;
								fCheckMapReverse(TestingPost);
							}
						}
					}
					{
						DMibTestPath("ReversedMoveAssign");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCSet<int, CCompare> Testing;
							Testing = fg_Move(TestingSource);
							fInitSet(Testing);
							fCheckSetReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCSet<int, CCompare> TestingPost;
								TestingPost = fg_Move(TestingSource);
								fCheckSetReverse(TestingPost);
							}
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCMap<int, CStr, CCompare> Testing;
							Testing = fg_Move(TestingSource);
							fInitMap(Testing);
							fCheckMapReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCMap<int, CStr, CCompare> TestingPost;
								TestingPost = fg_Move(TestingSource);
								fCheckMapReverse(TestingPost);
							}
						}
					}

					using CDiffAllocator = NMemory::CAllocator_Virtual;
					{
						DMibTestPath("DiffAllocReversedCopy");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCSet<int, CCompare, CDiffAllocator> Testing(TestingSource);
							fInitSet(Testing);
							fCheckSetReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCSet<int, CCompare, CDiffAllocator> TestingPost(TestingSource);
								fCheckSetReverse(TestingPost);
							}
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCMap<int, CStr, CCompare, CDiffAllocator> Testing(TestingSource);
							fInitMap(Testing);
							fCheckMapReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCMap<int, CStr, CCompare, CDiffAllocator> TestingPost(TestingSource);
								fCheckMapReverse(TestingPost);
							}
						}
					}
					{
						DMibTestPath("DiffAllocReversedMove");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCSet<int, CCompare, CDiffAllocator> Testing(fg_Move(TestingSource));
							fInitSet(Testing);
							fCheckSetReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCSet<int, CCompare, CDiffAllocator> TestingPost(fg_Move(TestingSource));
								fCheckSetReverse(TestingPost);
							}
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCMap<int, CStr, CCompare, CDiffAllocator> Testing(fg_Move(TestingSource));
							fInitMap(Testing);
							fCheckMapReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCMap<int, CStr, CCompare, CDiffAllocator> TestingPost(fg_Move(TestingSource));
								fCheckMapReverse(TestingPost);
							}
						}
					}
					{
						DMibTestPath("DiffAllocReversedAssign");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCSet<int, CCompare, CDiffAllocator> Testing;
							Testing = TestingSource;
							fInitSet(Testing);
							fCheckSetReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCSet<int, CCompare, CDiffAllocator> TestingPost;
								TestingPost = TestingSource;
								fCheckSetReverse(TestingPost);
							}
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCMap<int, CStr, CCompare, CDiffAllocator> Testing;
							Testing = TestingSource;
							fInitMap(Testing);
							fCheckMapReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCMap<int, CStr, CCompare, CDiffAllocator> TestingPost;
								TestingPost = TestingSource;
								fCheckMapReverse(TestingPost);
							}
						}
					}
					{
						DMibTestPath("DiffAllocReversedMoveAssign");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCSet<int, CCompare, CDiffAllocator> Testing;
							Testing = fg_Move(TestingSource);
							fInitSet(Testing);
							fCheckSetReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCSet<int, CCompare, CDiffAllocator> TestingPost;
								TestingPost = fg_Move(TestingSource);
								fCheckSetReverse(TestingPost);
							}
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
							TCMap<int, CStr, CCompare, CDiffAllocator> Testing;
							Testing = fg_Move(TestingSource);
							fInitMap(Testing);
							fCheckMapReverse(Testing);
							{
								DMibTestPath("Post");
								TestingSource = Testing;
								TCMap<int, CStr, CCompare, CDiffAllocator> TestingPost;
								TestingPost = fg_Move(TestingSource);
								fCheckMapReverse(TestingPost);
							}
						}
					}
					{
						using CMemoryManagerParams = NMemory::TCMemoryManagerParams<>;
						using CMemoryManager = NMemory::TCMemoryManager<CMemoryManagerParams>;
						CMemoryManager MemoryManager{NMemory::CMemoryManagerConfig()};
						using CDiffAllocator = NMemory::TCAllocator_MemoryManager<CMemoryManagerParams>;
						DMibTestPath("StatefulAllocator");
						{
							DMibTestPath("Set");
							TCSet<int, CCompare, CDiffAllocator> Testing
								(
									CAllocatorConstructTag()
									, CCompareConstructTag()
									, fg_Construct(&MemoryManager)
									, fg_Construct(CCompare{.m_bReverse = true})
								)
							;
							fInitSet(Testing, TCCompileTimeConstant<umint, sizeof(void *) * 3>());
							fCheckSetReverse(Testing);
						}
						{
							DMibTestPath("Map");
							TCMap<int, CStr, CCompare, CDiffAllocator> Testing
								(
									CAllocatorConstructTag()
									, CCompareConstructTag()
									, fg_Construct(&MemoryManager)
									, fg_Construct(CCompare{.m_bReverse = true})
								)
							;
							fInitMap(Testing, TCCompileTimeConstant<umint, sizeof(void *) * 3>());
							fCheckMapReverse(Testing);
						}
						{
							DMibTestPath("DiffAllocReversedMove");
							{
								DMibTestPath("Set");
								TCSet<int, CCompare, CDiffAllocator> TestingSource
									(
										CAllocatorConstructTag()
										, CCompareConstructTag()
										, fg_Construct(&MemoryManager)
										, fg_Construct(CCompare{.m_bReverse = true})
									)
								;
								TCSet<int, CCompare, CDiffAllocator> Testing(fg_Move(TestingSource));
								fInitSet(Testing, TCCompileTimeConstant<umint, sizeof(void *) * 3>());
								fCheckSetReverse(Testing);
								{
									DMibTestPath("Post");
									TestingSource = Testing;
									TCSet<int, CCompare, CDiffAllocator> TestingPost(fg_Move(TestingSource));
									fCheckSetReverse(TestingPost);
								}
							}
							{
								DMibTestPath("Map");
								TCMap<int, CStr, CCompare, CDiffAllocator> TestingSource
									(
										CAllocatorConstructTag()
										, CCompareConstructTag()
										, fg_Construct(&MemoryManager)
										, fg_Construct(CCompare{.m_bReverse = true})
									)
								;
								TCMap<int, CStr, CCompare, CDiffAllocator> Testing(fg_Move(TestingSource));
								fInitMap(Testing, TCCompileTimeConstant<umint, sizeof(void *) * 3>());
								fCheckMapReverse(Testing);
								{
									DMibTestPath("Post");
									TestingSource = Testing;
									TCMap<int, CStr, CCompare, CDiffAllocator> TestingPost(fg_Move(TestingSource));
									fCheckMapReverse(TestingPost);
								}
							}
						}
						{
							DMibTestPath("DiffAllocReversedAssign");
							{
								DMibTestPath("Set");
								TCSet<int, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
								TCSet<int, CCompare, CDiffAllocator> Testing(CAllocatorConstructTag(), &MemoryManager);
								Testing = TestingSource;
								fInitSet(Testing, TCCompileTimeConstant<umint, sizeof(void *) * 3>());
								fCheckSetReverse(Testing);
								{
									DMibTestPath("Post");
									TestingSource = Testing;
									TCSet<int, CCompare, CDiffAllocator> TestingPost(CAllocatorConstructTag(), &MemoryManager);
									TestingPost = TestingSource;
									fCheckSetReverse(TestingPost);
								}
							}
							{
								DMibTestPath("Map");
								TCMap<int, CStr, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
								TCMap<int, CStr, CCompare, CDiffAllocator> Testing(CAllocatorConstructTag(), &MemoryManager);
								Testing = TestingSource;
								fInitMap(Testing, TCCompileTimeConstant<umint, sizeof(void *) * 3>());
								fCheckMapReverse(Testing);
								{
									DMibTestPath("Post");
									TestingSource = Testing;
									TCMap<int, CStr, CCompare, CDiffAllocator> TestingPost(CAllocatorConstructTag(), &MemoryManager);
									TestingPost = TestingSource;
									fCheckMapReverse(TestingPost);
								}
							}
						}
						{
							DMibTestPath("DiffAllocReversedMoveAssign");
							{
								DMibTestPath("Set");
								TCSet<int, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
								TCSet<int, CCompare, CDiffAllocator> Testing(CAllocatorConstructTag(), &MemoryManager);
								Testing = fg_Move(TestingSource);
								fInitSet(Testing, TCCompileTimeConstant<umint, sizeof(void *) * 3>());
								fCheckSetReverse(Testing);
								{
									DMibTestPath("Post");
									TestingSource = Testing;
									TCSet<int, CCompare, CDiffAllocator> TestingPost(CAllocatorConstructTag(), &MemoryManager);
									TestingPost = fg_Move(TestingSource);
									fCheckSetReverse(TestingPost);
								}
							}
							{
								DMibTestPath("Map");
								TCMap<int, CStr, CCompare> TestingSource(CCompareConstructTag(), CCompare{.m_bReverse = true});
								TCMap<int, CStr, CCompare, CDiffAllocator> Testing(CAllocatorConstructTag(), &MemoryManager);
								Testing = fg_Move(TestingSource);
								fInitMap(Testing, TCCompileTimeConstant<umint, sizeof(void *) * 3>());
								fCheckMapReverse(Testing);
								{
									DMibTestPath("Post");
									TestingSource = Testing;
									TCMap<int, CStr, CCompare, CDiffAllocator> TestingPost(CAllocatorConstructTag(), &MemoryManager);
									TestingPost = fg_Move(TestingSource);
									fCheckMapReverse(TestingPost);
								}
							}
						}
					}
				}
			};
			DMibTestSuite("Destructive Iterator")
			{
				DMibTestCategory("Iterator Move Semantics")
				{
					{
						DMibTestPath("MoveCtor Transfers Ownership");
						TCMap<int, int> Source;
						for (int i = 0; i < 10; ++i)
							Source[i] = i;

						{
							auto iIter0 = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalDestructive();
							{
								DMibTestPath("BeforeMove");
								DMibExpect(Source.f_GetLen(), ==, 10);
							}

							{
								auto iIter1 = fg_Move(iIter0);
								{
									DMibTestPath("AfterMove");
									DMibExpect(Source.f_GetLen(), ==, 10);
								}
								DMibExpectTrue(iIter1);
							}

							// iIter0 was moved-from, should not clear the map here
							{
								DMibTestPath("AfterMovedToDestroyed");
								DMibExpect(Source.f_GetLen(), ==, 0);
							}
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
					}

					{
						DMibTestPath("MoveAssign Clears Previous Map");
						TCMap<int, int> SourceA;
						for (int i = 0; i < 10; ++i)
							SourceA[i] = i;

						TCMap<int, int> SourceB;
						for (int i = 0; i < 7; ++i)
							SourceB[i] = i;

						{
							auto iIterA = fg_Move(SourceA).f_Entries().f_GetIteratorBidirectionalDestructive();
							auto iIterB = fg_Move(SourceB).f_Entries().f_GetIteratorBidirectionalDestructive();

							{
								DMibTestPath("BeforeAssign");
								DMibExpect(SourceA.f_GetLen(), ==, 10);
								DMibExpect(SourceB.f_GetLen(), ==, 7);
							}

							iIterB = fg_Move(iIterA);

							// Move-assign should clear the previously-owned map (SourceB)
							{
								DMibTestPath("AfterAssign");
								DMibExpect(SourceB.f_GetLen(), ==, 0);
								DMibExpect(SourceA.f_GetLen(), ==, 10);
							}
						}

						// Remaining ownership should clear SourceA at end of scope
						DMibExpect(SourceA.f_GetLen(), ==, 0);
						DMibExpect(SourceB.f_GetLen(), ==, 0);
					}
				};

				DMibTestCategory("Keys Destructive")
				{
					{
						DMibTestPath("Forward");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						TCVector<CStr> Collected;
						for (auto iIter = fg_Move(Source).f_Keys().f_GetIteratorDestructive(); iIter; ++iIter)
						{
							CStr Key = fg_Move(*iIter);
							Collected.f_InsertLast(fg_Move(Key));
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Collected.f_GetLen(), ==, 10);
						for (int i = 0; i < 10; ++i)
							DMibExpect(Collected[i], ==, CStr("Key {}"_f << i))(ETestFlag_Aggregated);
					}

					{
						DMibTestPath("Reverse");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						TCVector<CStr> Collected;
						for (auto iIter = fg_Move(Source).f_Keys().f_GetIteratorReverseDestructive(); iIter; ++iIter)
						{
							CStr Key = fg_Move(*iIter);
							Collected.f_InsertLast(fg_Move(Key));
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Collected.f_GetLen(), ==, 10);
						for (int i = 0; i < 10; ++i)
							DMibExpect(Collected[i], ==, CStr("Key {}"_f << (9 - i)))(ETestFlag_Aggregated);
					}

					{
						DMibTestPath("Bidirectional");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						auto iIter = fg_Move(Source).f_Keys().f_GetIteratorBidirectionalDestructive();

						DMibExpectTrue(iIter);
						DMibExpect(*iIter, ==, CStr("Key 0"));
						++iIter;
						++iIter;
						DMibExpect(*iIter, ==, CStr("Key 2"));

						--iIter;
						DMibExpect(*iIter, ==, CStr("Key 1"));
					}

					{
						DMibTestPath("BidirectionalReverse");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						{
							auto iIter = fg_Move(Source).f_Keys().f_GetIteratorBidirectionalReverseDestructive();

							DMibExpectTrue(iIter);
							DMibExpect(*iIter, ==, CStr("Key 9"));
							++iIter;
							DMibExpect(*iIter, ==, CStr("Key 8"));
							--iIter;
							{
								DMibTestPath("After decrease");
								DMibExpect(*iIter, ==, CStr("Key 9"));
							}
						}

						// Iterator destructor should clear the moved-from map
						DMibExpect(Source.f_GetLen(), ==, 0);
					}
				};

				DMibTestCategory("Forward Non-Extracting")
				{
					// Test that destructive iteration moves keys/values and clears map
					{
						DMibTestPath("Map");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						TCVector<TCTuple<CStr, CStr>> Collected;
						for (auto iIter = fg_Move(Source).f_Entries().f_GetIteratorDestructive(); iIter; ++iIter)
						{
							// Keys should be movable from destructive iterator
							CStr Key = fg_Move(iIter->f_Key());
							CStr Value = fg_Move(iIter->f_Value());
							Collected.f_InsertLast(TCTuple{fg_Move(Key), fg_Move(Value)});
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Collected.f_GetLen(), ==, 10);

						// Verify ordering (should be sorted by key)
						for (int i = 0; i < 10; ++i)
						{
							DMibExpect(fg_Get<0>(Collected[i]), ==, CStr("Key {}"_f << i))(ETestFlag_Aggregated);
							DMibExpect(fg_Get<1>(Collected[i]), ==, CStr("Value {}"_f << i))(ETestFlag_Aggregated);
						}
					}
					{
						DMibTestPath("Set");
						TCSet<CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)];

						TCVector<CStr> Collected;
						for (auto iIter = fg_Move(Source).f_GetIteratorDestructive(); iIter; ++iIter)
						{
							CStr Key = fg_Move(*iIter);
							Collected.f_InsertLast(fg_Move(Key));
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Collected.f_GetLen(), ==, 10);

						for (int i = 0; i < 10; ++i)
							DMibExpect(Collected[i], ==, CStr("Key {}"_f << i))(ETestFlag_Aggregated);
					}
				};
				DMibTestCategory("Reverse Non-Extracting")
				{
					{
						DMibTestPath("Map");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						TCVector<TCTuple<CStr, CStr>> Collected;
						for (auto iIter = fg_Move(Source).f_Entries().f_GetIteratorReverseDestructive(); iIter; ++iIter)
						{
							CStr Key = fg_Move(iIter->f_Key());
							CStr Value = fg_Move(iIter->f_Value());
							Collected.f_InsertLast(TCTuple{fg_Move(Key), fg_Move(Value)});
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Collected.f_GetLen(), ==, 10);

						// Reverse order - should be from 9 down to 0
						for (int i = 0; i < 10; ++i)
						{
							DMibExpect(fg_Get<0>(Collected[i]), ==, CStr("Key {}"_f << (9 - i)))(ETestFlag_Aggregated);
							DMibExpect(fg_Get<1>(Collected[i]), ==, CStr("Value {}"_f << (9 - i)))(ETestFlag_Aggregated);
						}
					}
					{
						DMibTestPath("Set");
						TCSet<CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)];

						TCVector<CStr> Collected;
						for (auto iIter = fg_Move(Source).f_GetIteratorReverseDestructive(); iIter; ++iIter)
						{
							CStr Key = fg_Move(*iIter);
							Collected.f_InsertLast(fg_Move(Key));
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Collected.f_GetLen(), ==, 10);

						for (int i = 0; i < 10; ++i)
							DMibExpect(Collected[i], ==, CStr("Key {}"_f << (9 - i)))(ETestFlag_Aggregated);
					}
				};
				DMibTestCategory("Bidirectional Non-Extracting")
				{
					{
						DMibTestPath("Map");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						auto iIter = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalDestructive();

						// Move forward 5 steps
						TCVector<CStr> ForwardKeys;
						for (int i = 0; i < 5 && iIter; ++i, ++iIter)
							ForwardKeys.f_InsertLast(iIter->f_Key());

						DMibExpect(ForwardKeys.f_GetLen(), ==, 5);
						for (int i = 0; i < 5; ++i)
							DMibExpect(ForwardKeys[i], ==, CStr("Key {}"_f << i))(ETestFlag_Aggregated);

						// Move backward 2 steps
						TCVector<CStr> BackwardKeys;
						--iIter;
						BackwardKeys.f_InsertLast(iIter->f_Key());
						--iIter;
						BackwardKeys.f_InsertLast(iIter->f_Key());

						DMibExpect(BackwardKeys.f_GetLen(), ==, 2);
						DMibExpect(BackwardKeys[0], ==, CStr("Key 4"));
						DMibExpect(BackwardKeys[1], ==, CStr("Key 3"));

						// Let iterator destructor clean up remaining elements
					}
					{
						DMibTestPath("Set");
						TCSet<CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)];

						auto iIter = fg_Move(Source).f_GetIteratorBidirectionalDestructive();

						// Move forward 5 steps
						TCVector<CStr> ForwardKeys;
						for (int i = 0; i < 5 && iIter; ++i, ++iIter)
							ForwardKeys.f_InsertLast(*iIter);

						DMibExpect(ForwardKeys.f_GetLen(), ==, 5);

						// Move backward 2 steps
						--iIter;
						--iIter;
						DMibExpect(*iIter, ==, CStr("Key 3"));
					}
				};
				DMibTestCategory("Bidirectional Reverse Non-Extracting")
				{
					{
						DMibTestPath("Map");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						auto iIter = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalReverseDestructive();

						// First element should be Key 9 (highest)
						DMibExpect(iIter->f_Key(), ==, CStr("Key 9"));

						// Move forward (which goes toward lower keys in reverse mode)
						++iIter;
						DMibExpect(iIter->f_Key(), ==, CStr("Key 8"));
						++iIter;
						DMibExpect(iIter->f_Key(), ==, CStr("Key 7"));

						// Move backward (toward higher keys in reverse mode)
						--iIter;
						{
							DMibTestPath("AfterBackward");
							DMibExpect(iIter->f_Key(), ==, CStr("Key 8"));
						}
					}
					{
						DMibTestPath("Set");
						TCSet<CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)];

						auto iIter = fg_Move(Source).f_GetIteratorBidirectionalReverseDestructive();

						DMibExpect(*iIter, ==, CStr("Key 9"));
						++iIter;
						DMibExpect(*iIter, ==, CStr("Key 8"));
						{
							DMibTestPath("AfterBackward");
							--iIter;
							DMibExpect(*iIter, ==, CStr("Key 9"));
						}
					}
				};
				DMibTestCategory("Forward Extract Node")
				{
					{
						DMibTestPath("Map ExtractAll");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						TCMap<CStr, CStr> Target;
						// f_ExtractNode() requires bidirectional iterator (maintains full ancestor path for parent lookup)
						for (auto iIter = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalDestructive(); iIter; )
						{
							// f_ExtractNode() advances the iterator
							auto Handle = iIter.f_ExtractNode();
							DMibExpectTrue(Handle)(ETestFlag_Aggregated);
							Target.f_Insert(fg_Move(Handle));
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Target.f_GetLen(), ==, 10);

						// Verify all keys/values transferred correctly
						for (int i = 0; i < 10; ++i)
						{
							CStr Key = "Key {}"_f << i;
							auto *pValue = Target.f_FindEqual(Key);
							DMibAssertTrue(pValue)(ETestFlag_Aggregated);
							DMibExpect(*pValue, ==, CStr("Value {}"_f << i))(ETestFlag_Aggregated);
						}
					}
					{
						DMibTestPath("Map ExtractConditional");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						TCMap<CStr, CStr> Target;
						// f_ExtractNode() requires bidirectional iterator
						for (auto iIter = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalDestructive(); iIter; )
						{
							// Extract only even-numbered keys
							CStr Key = iIter->f_Key();
							int Num = 0;
							aint nParsed = 0;
							(CStr::CParse("Key {}") >> Num).f_Parse(Key, nParsed);

							if (Num % 2 == 0)
							{
								auto Handle = iIter.f_ExtractNode();
								Target.f_Insert(fg_Move(Handle));
							}
							else
							{
								++iIter;
							}
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Target.f_GetLen(), ==, 5);

						// Verify only even keys transferred
						for (int i = 0; i < 10; i += 2)
						{
							CStr Key = "Key {}"_f << i;
							auto *pValue = Target.f_FindEqual(Key);
							DMibAssertTrue(pValue)(ETestFlag_Aggregated);
						}
					}
					{
						DMibTestPath("Set ExtractAll");
						TCSet<CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)];

						TCSet<CStr> Target;
						// f_ExtractNode() requires bidirectional iterator
						for (auto iIter = fg_Move(Source).f_GetIteratorBidirectionalDestructive(); iIter;)
						{
							auto Handle = iIter.f_ExtractNode();
							Target.f_Insert(fg_Move(Handle));
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Target.f_GetLen(), ==, 10);
					}
				};
				DMibTestCategory("Reverse Extract Node")
				{
					{
						DMibTestPath("Map ExtractAll");
						TCMap<CStr, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)] = "Value {}"_f << i;

						TCMap<CStr, CStr> Target;
						TCVector<CStr> ExtractOrder;
						// f_ExtractNode() requires bidirectional iterator
						for (auto iIter = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalReverseDestructive(); iIter; )
						{
							ExtractOrder.f_InsertLast(iIter->f_Key());
							auto Handle = iIter.f_ExtractNode();
							Target.f_Insert(fg_Move(Handle));
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Target.f_GetLen(), ==, 10);
						DMibExpect(ExtractOrder.f_GetLen(), ==, 10);

						// Verify extraction was in reverse order
						for (int i = 0; i < 10; ++i)
							DMibExpect(ExtractOrder[i], ==, CStr("Key {}"_f << (9 - i)))(ETestFlag_Aggregated);
					}
					{
						DMibTestPath("Set ExtractConditional");
						TCSet<CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[CStr("Key {}"_f << i)];

						TCSet<CStr> Target;
						// f_ExtractNode() requires bidirectional iterator
						for (auto iIter = fg_Move(Source).f_GetIteratorBidirectionalReverseDestructive(); iIter; )
						{
							CStr Key = *iIter;
							int Num = 0;
							aint nParsed = 0;
							(CStr::CParse("Key {}") >> Num).f_Parse(Key, nParsed);

							if (Num % 2 == 1) // Extract odd keys
							{
								auto Handle = iIter.f_ExtractNode();
								Target.f_Insert(fg_Move(Handle));
							}
							else
							{
								++iIter;
							}
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Target.f_GetLen(), ==, 5);
					}
				};
				DMibTestCategory("Bidirectional Extract Node")
				{
					{
						DMibTestPath("Map Forward Then Back");
						TCMap<int, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[i] = "Value {}"_f << i;

						TCMap<int, CStr> Target;

						auto iIter = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalDestructive();

						// Extract first 3 elements going forward
						for (int i = 0; i < 3; ++i)
						{
							Target.f_Insert(iIter.f_ExtractNode());
						}

						// Now at element 3, move forward to 5
						++iIter;
						++iIter;
						DMibExpect(iIter->f_Key(), ==, 5);

						// Extract element 5
						Target.f_Insert(iIter.f_ExtractNode());

						// Should now be at element 6, go back to 4
						--iIter;
						DMibExpect(iIter->f_Key(), ==, 4);

						// Extract element 4
						Target.f_Insert(iIter.f_ExtractNode());

						// Verify extracted elements
						DMibExpect(Target.f_GetLen(), ==, 5);
						DMibExpectTrue(Target.f_FindEqual(0));
						DMibExpectTrue(Target.f_FindEqual(1));
						DMibExpectTrue(Target.f_FindEqual(2));
						DMibExpectTrue(Target.f_FindEqual(4));
						DMibExpectTrue(Target.f_FindEqual(5));
					}
					{
						DMibTestPath("Set Zigzag");
						TCSet<int> Source;
						for (int i = 0; i < 20; ++i)
							Source[i];

						TCSet<int> Target;
						{
							auto iIter = fg_Move(Source).f_GetIteratorBidirectionalDestructive();

							// Extract 0
							Target.f_Insert(iIter.f_ExtractNode());
							// Now at 1, skip to 5
							for (int i = 0; i < 4; ++i)
								++iIter;
							DMibExpect(*iIter, ==, 5);
							Target.f_Insert(iIter.f_ExtractNode());

							// Now at 6, go back to 3
							--iIter;
							--iIter;
							DMibExpect(*iIter, ==, 3);
							Target.f_Insert(iIter.f_ExtractNode());

							DMibExpect(Target.f_GetLen(), ==, 3);
							DMibExpect(Source.f_GetLen(), ==, 17);
							DMibExpectTrue(Target.f_FindEqual(0));
							DMibExpectTrue(Target.f_FindEqual(3));
							DMibExpectTrue(Target.f_FindEqual(5));
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
					}
				};
				DMibTestCategory("Bidirectional Reverse Extract Node")
				{
					{
						DMibTestPath("Map");
						TCMap<int, CStr> Source;
						for (int i = 0; i < 10; ++i)
							Source[i] = "Value {}"_f << i;

						TCMap<int, CStr> Target;

						auto iIter = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalReverseDestructive();

						// Start at 9 (reverse iterator starts at highest)
						DMibExpect(iIter->f_Key(), ==, 9);
						Target.f_Insert(iIter.f_ExtractNode());

						// Now at 8
						DMibExpect(iIter->f_Key(), ==, 8);
						++iIter; // Move to 7 in reverse direction
						DMibExpect(iIter->f_Key(), ==, 7);

						// Go back towards 8
						{
							DMibTestPath("AfterBackward");
							--iIter;
							DMibExpect(iIter->f_Key(), ==, 8);
							Target.f_Insert(iIter.f_ExtractNode());

							DMibExpect(Target.f_GetLen(), ==, 2);
							DMibExpectTrue(Target.f_FindEqual(8));
							DMibExpectTrue(Target.f_FindEqual(9));
						}
					}
				};
				DMibTestCategory("Extract Edge Cases")
				{
					{
						DMibTestPath("ExtractLastElementForward");
						TCSet<int> Source;
						for (int i = 0; i < 10; ++i)
							Source[i];

						TCSet<int> Target;
						{
							auto iIter = fg_Move(Source).f_GetIteratorBidirectionalDestructive();

							// Advance to last element (9)
							for (int i = 0; i < 9; ++i)
								++iIter;
							DMibExpectTrue(iIter);
							DMibExpect(*iIter, ==, 9);

							Target.f_Insert(iIter.f_ExtractNode());
							DMibExpectFalse(iIter);

							DMibExpect(Target.f_GetLen(), ==, 1);
							DMibExpectTrue(Target.f_FindEqual(9));
							DMibExpect(Source.f_GetLen(), ==, 9);
						}
						DMibExpect(Source.f_GetLen(), ==, 0);
					}

					{
						DMibTestPath("ExtractAndDropHandle");
						TCMap<int, int> Source;
						for (int i = 0; i < 10; ++i)
							Source[i] = i * 10;

						{
							auto iIter = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalDestructive();
							DMibExpect(Source.f_GetLen(), ==, 10);

							// Extract node and immediately drop it (handle destructor deletes the node)
							{
								auto Handle = iIter.f_ExtractNode();
								DMibExpectTrue(Handle);
							}

							DMibExpect(Source.f_GetLen(), ==, 9);
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
					}
				};
				DMibTestCategory("Extract Preserves Tree Integrity")
				{
					// Test with larger dataset to stress the no-rebalance removal
					{
						DMibTestPath("Large Map");
						TCMap<int, int> Source;
						for (int i = 0; i < 100; ++i)
							Source[i] = i * 10;

						TCMap<int, int> Target;
						TCVector<int> ExtractedKeys;

						// f_ExtractNode() requires bidirectional iterator
						for (auto iIter = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalDestructive(); iIter; )
						{
							// Extract every third element
							if (iIter->f_Key() % 3 == 0)
							{
								ExtractedKeys.f_InsertLast(iIter->f_Key());
								Target.f_Insert(iIter.f_ExtractNode());
							}
							else
							{
								++iIter;
							}
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Target.f_GetLen(), ==, 34); // 0, 3, 6, ..., 99 = 34 elements

						// Verify extracted keys maintain sorted order in target
						int PrevKey = -1;
						for (auto iIter = Target.f_GetIterator(); iIter; ++iIter)
						{
							DMibExpectTrue(iIter.f_GetKey() > PrevKey)(ETestFlag_Aggregated);
							DMibExpect(*iIter, ==, iIter.f_GetKey() * 10)(ETestFlag_Aggregated);
							PrevKey = iIter.f_GetKey();
						}
					}
					{
						DMibTestPath("Large Set Reverse");
						TCSet<int> Source;
						for (int i = 0; i < 100; ++i)
							Source[i];

						TCSet<int> Target;
						int nExtracted = 0;

						// f_ExtractNode() requires bidirectional iterator
						for (auto iIter = fg_Move(Source).f_GetIteratorBidirectionalReverseDestructive(); iIter; )
						{
							// Extract every other element
							if (*iIter % 2 == 0)
							{
								Target.f_Insert(iIter.f_ExtractNode());
								++nExtracted;
							}
							else
							{
								++iIter;
							}
						}

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Target.f_GetLen(), ==, 50);
						DMibExpect(nExtracted, ==, 50);
					}
				};
				DMibTestCategory("Empty Map Destructive Iterator")
				{
					{
						DMibTestPath("Map Forward");
						TCMap<int, int> Source;
						int nCount = 0;
						for (auto iIter = fg_Move(Source).f_Entries().f_GetIteratorDestructive(); iIter; ++iIter)
							++nCount;
						DMibExpect(nCount, ==, 0);
					}
					{
						DMibTestPath("Map Reverse");
						TCMap<int, int> Source;
						int nCount = 0;
						for (auto iIter = fg_Move(Source).f_Entries().f_GetIteratorReverseDestructive(); iIter; ++iIter)
							++nCount;
						DMibExpect(nCount, ==, 0);
					}
					{
						DMibTestPath("Set Bidirectional");
						TCSet<int> Source;
						auto iIter = fg_Move(Source).f_GetIteratorBidirectionalDestructive();
						DMibExpectFalse(iIter);
					}
				};
				DMibTestCategory("Single Element Destructive Iterator")
				{
					{
						DMibTestPath("Map Extract");
						TCMap<CStr, int> Source;
						Source["OnlyKey"] = 42;

						TCMap<CStr, int> Target;
						// f_ExtractNode() requires bidirectional iterator
						auto iIter = fg_Move(Source).f_Entries().f_GetIteratorBidirectionalDestructive();
						DMibExpectTrue(iIter);
						Target.f_Insert(iIter.f_ExtractNode());
						DMibExpectFalse(iIter);

						DMibExpect(Source.f_GetLen(), ==, 0);
						DMibExpect(Target.f_GetLen(), ==, 1);
						DMibExpect(*Target.f_FindEqual("OnlyKey"), ==, 42);
					}
					{
						DMibTestPath("Set Reverse Extract");
						TCSet<int> Source;
						Source[999];

						TCSet<int> Target;
						// f_ExtractNode() requires bidirectional iterator
						auto iIter = fg_Move(Source).f_GetIteratorBidirectionalReverseDestructive();
						DMibExpectTrue(iIter);
						DMibExpect(*iIter, ==, 999);
						Target.f_Insert(iIter.f_ExtractNode());
						DMibExpectFalse(iIter);

						DMibExpect(Target.f_GetLen(), ==, 1);
					}
				};
			};
		}
	};

	DMibTestRegister(CMap_Tests, Malterlib::Container);
}
