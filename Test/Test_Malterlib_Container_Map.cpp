// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <set>
namespace
{
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

					NMib::NContainer::TCMap<NMib::NStr::CStr, NMib::NContainer::TCVector<NMib::NStr::CStr>> Testing;
					NMib::NStr::CStr Name;

					Name = "subjectAltName";
					Testing[Name].f_Insert("Test 2");

					Name = "MalterlibHostID";
					Testing[Name].f_Insert("Test 1");

					auto *pTest = Testing.f_FindEqual("MalterlibHostID");
					DMibAssertTrue(pTest);

					DMibExpect(*pTest, ==, NMib::NContainer::TCVector<NMib::NStr::CStr>{"Test 1"});
				};
			};
			DMibTestCategory("Remove")
			{
				DMibTestSuite("Set")
				{
					NMib::NContainer::TCSet<int> Testing;
					auto &ToRemove = Testing[1];
					Testing.f_Remove(&ToRemove);
				};
				DMibTestSuite("Map")
				{
					NMib::NContainer::TCMap<int, int> Testing;
					auto &ToRemove = Testing[1];
					Testing.f_Remove(&ToRemove);
				};
			};

			DMibTestSuite("Misc")
			{
				NMib::NContainer::TCSet<int> Testing;

				Testing[2];
				Testing[3];

				NMib::NContainer::TCSet<int> Testing2;
				Testing2[2];
				Testing2[3];

				Testing2 = fg_Move(Testing);
			};

			DMibTestSuite("Copy Set")
			{
				NMib::NContainer::TCSet<int32> Source;

				for (int32 i = 0; i < 50; ++i)
					Source[i];

				NMib::NContainer::TCSet<int32> Copied = Source;
				DMibExpect(Copied, ==, Source);

				NMib::NContainer::TCSet<int32> Moved = fg_Move(Copied);
				DMibExpect(Moved, ==, Source);

				NMib::NContainer::TCSet<int64> CopiedOtherType = Source;
				DMibExpect(CopiedOtherType, ==, Source);
			};
			DMibTestSuite("Copy Map")
			{
				NMib::NContainer::TCMap<int32, int32> Source;

				for (int32 i = 0; i < 50; ++i)
					Source[i] = i;

				NMib::NContainer::TCMap<int32, int32> Copied = Source;
				DMibExpect(Copied, ==, Source);

				NMib::NContainer::TCMap<int32, int32> Moved = fg_Move(Copied);
				DMibExpect(Moved, ==, Source);

				NMib::NContainer::TCMap<int32, int64> CopiedOtherType = Source;
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

				NMib::NContainer::TCSet<int> Testing;
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

				NMib::NContainer::TCSet<int> Testing;
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

				NMib::NContainer::TCSet<int> Testing;
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

				NMib::NContainer::TCSet<int> Testing;
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

				NMib::NContainer::TCMap<int, int const &> Testing;

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


		}
	};

	DMibTestRegister(CMap_Tests, Malterlib::Container);
}

