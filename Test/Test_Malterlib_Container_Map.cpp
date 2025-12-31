// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Memory/MemoryManager>

namespace
{
	using namespace NMib;
	using namespace NMib::NTraits;
	using namespace NMib::NStr;
	using namespace NMib::NContainer;

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
				TCMap Map0{std::tuple{1, 2}, {3 ,4}, {5, 6}};
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

				auto fInitSet = []<mint tf_ExpectedSize = sizeof(void *) * 2>(auto &_Set, TCCompileTimeConstant<mint, tf_ExpectedSize> _x = TCCompileTimeConstant<mint, sizeof(void *) * 2>())
					{
//						static_assert(sizeof(_Set) == tf_ExpectedSize);

						for (int i = 0; i < 10; ++i)
							_Set[i];
					}
				;
				auto fInitMap = []<mint tf_ExpectedSize = sizeof(void *) * 2>(auto &_Map, TCCompileTimeConstant<mint, tf_ExpectedSize> _x = TCCompileTimeConstant<mint, sizeof(void *) * 2>())
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
							fInitSet(Testing, TCCompileTimeConstant<mint, sizeof(void *) * 3>());
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
							fInitMap(Testing, TCCompileTimeConstant<mint, sizeof(void *) * 3>());
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
								fInitSet(Testing, TCCompileTimeConstant<mint, sizeof(void *) * 3>());
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
								fInitMap(Testing, TCCompileTimeConstant<mint, sizeof(void *) * 3>());
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
								fInitSet(Testing, TCCompileTimeConstant<mint, sizeof(void *) * 3>());
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
								fInitMap(Testing, TCCompileTimeConstant<mint, sizeof(void *) * 3>());
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
								fInitSet(Testing, TCCompileTimeConstant<mint, sizeof(void *) * 3>());
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
								fInitMap(Testing, TCCompileTimeConstant<mint, sizeof(void *) * 3>());
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
		}
	};

	DMibTestRegister(CMap_Tests, Malterlib::Container);
}

