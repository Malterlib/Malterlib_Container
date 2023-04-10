// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <set>

namespace
{
	using namespace NMib;
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
					auto &ToRemove = Testing[1];
					Testing.f_Remove(&ToRemove);
				};
				DMibTestSuite("Map")
				{
					TCMap<int, int> Testing;
					auto &ToRemove = Testing[1];
					Testing.f_Remove(&ToRemove);
				};
			};
			DMibTestCategory("Extract")
			{
				DMibTestSuite("Map")
				{
					TCMap<CStr, CStr> Testing;

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

				bool bCreated;
				for (int i = 0; i < 10; ++i)
					Testing.f_Map(i, bCreated, Values[i]);

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
				static_assert(NMib::NTraits::TCIsSame<decltype(Map0), TCMap<int, int>>::mc_Value);
				DMibExpect(Map0, ==, (TCMap<int, int>({{1, 2}, {3 ,4}, {5, 6}})));
			};
			DMibTestSuite("CTAD Set")
			{
				TCSet Set0 = {1, 2, 3};
				static_assert(NMib::NTraits::TCIsSame<decltype(Set0), TCSet<int>>::mc_Value);
				DMibExpect(Set0, ==, TCSet<int>({1, 2, 3}));

				TCSet Set1{1, 2, 3};
				static_assert(NMib::NTraits::TCIsSame<decltype(Set1), TCSet<int>>::mc_Value);
				DMibExpect(Set1, ==, TCSet<int>({1, 2, 3}));
			};
		}
	};

	DMibTestRegister(CMap_Tests, Malterlib::Container);
}

