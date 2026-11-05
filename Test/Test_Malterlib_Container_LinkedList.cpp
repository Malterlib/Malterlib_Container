// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| CTestSort
|__________________________________________________________________________________________________
\*************************************************************************************************/

// TODO: Figure out why compiler is complaining about C4789
#ifdef DCompiler_MSVC
#	pragma warning(disable:4789)
#endif
namespace
{
	struct CLinkedList_Tests : public NMib::NTest::CTest
	{
		struct CTestClass
		{
			DMibListLinkD_Link(CTestClass, m_Link);
			umint m_Value;
		};

		struct CTestS
		{
			DMibListLinkDS_Link(CTestS, m_Link);
			umint m_Value;
		};

		void f_InvasiveNormal();

		void f_InvasiveSmall();

		void f_Dynamic();

		void f_DoTests();
	};

	void CLinkedList_Tests::f_InvasiveNormal()
	{
		DMibListLinkD_List(CTestClass, m_Link) TestList;
		DMibListLinkD_List(CTestClass, m_Link) OtherList;
		CTestClass Tests[10];
		for (aint iItem = 0; iItem < 10; ++iItem)
		{
			Tests[iItem].m_Value = iItem;
			TestList.f_Insert(Tests[iItem]);
		}

		// Whole list
		OtherList.f_SplitInto(TestList.f_GetFirst(), TestList.f_GetLast());
		DMibTest(DMibExpr(OtherList.f_CheckList(false)));
		DMibTest(DMibExpr(TestList.f_CheckList(false)));
		DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(0u));
		DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(10u));
		TestList.f_Insert(OtherList);
		DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(2));
		DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(2));
		DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(10u));
		DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(0u));

		for (umint iCase = 0; iCase < 4; ++iCase)
		{
			DMibTestPath(NMib::NStr::CStr::CFormat("Split {}") << iCase);
			umint nItems = 10;
			umint nToSplit = 5;
			if (iCase == 1)
				nToSplit = 1;
			else if (iCase == 2)
				nToSplit = 9;
			else if (iCase == 3)
			{
				nToSplit = 1;
				TestList.f_Clear();
				TestList.f_Insert(Tests[0]);
				TestList.f_Insert(Tests[1]);
				nItems = 2;
			}
			// Test splitting in the middle

			OtherList.f_SplitInto(&Tests[1], &Tests[nToSplit]);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)));
			DMibTest(DMibExpr(TestList.f_CheckList(false)));
			DMibTest(DMibExpr(OtherList.f_GetFirst()->m_Value) == DMibExpr(1u));
			DMibTest(DMibExpr(OtherList.f_GetLast()->m_Value) == DMibExpr(nToSplit));
			DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(nToSplit));
			DMibTest(DMibExpr(TestList.f_GetLen()) == (DMibExpr(nItems) - DMibExpr(nToSplit)));

			TestList.f_InsertAfter(OtherList, &Tests[0]);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(2u));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(2u));
			DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems));
			DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(0u));

			{
				bool bCorrectOrder = true;
				for (umint iItem = 0; iItem < nItems; ++iItem)
					bCorrectOrder = bCorrectOrder && DMibListLinkD_Iter(CTestClass, m_Link)(TestList)[aint(iItem)]->m_Value == iItem;
				DMibTest(DMibExpr(bCorrectOrder));
			}

			// Test splitting at beginning
			OtherList.f_SplitInto(&Tests[0], &Tests[nToSplit - 1]);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(3u));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(3u));
			DMibTest(DMibExpr(OtherList.f_GetFirst()->m_Value) == DMibExpr(0u));
			DMibTest(DMibExpr(OtherList.f_GetLast()->m_Value) == DMibExpr(nToSplit) - DMibExpr(1u));
			DMibTest(DMibExpr(TestList.f_GetFirst()->m_Value) == DMibExpr(nToSplit));
			DMibTest(DMibExpr(TestList.f_GetLast()->m_Value) == DMibExpr(nItems) - DMibExpr(1u));
			DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(nToSplit) && DMibExpr(2u));
			DMibTest((DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems) - DMibExpr(nToSplit)) && DMibExpr(2u));

			{
				DMibTestPath("Recombine");
				TestList.f_InsertFirst(OtherList);
				DMibTest(DMibExpr(OtherList.f_CheckList(false)));
				DMibTest(DMibExpr(TestList.f_CheckList(false)));
				DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems));
				DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(0u));
				{
					bool bCorrectOrder = true;
					for (umint iItem = 0; iItem < nItems; ++iItem)
						bCorrectOrder = bCorrectOrder && DMibListLinkD_Iter(CTestClass, m_Link)(TestList)[aint(iItem)]->m_Value == iItem;
					DMibTest(DMibExpr(bCorrectOrder));
				}

				// Test splitting at end
				OtherList.f_SplitInto(&Tests[nItems - nToSplit], &Tests[nItems - 1]);
				DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(2));
				DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(2));
				DMibTest(DMibExpr(OtherList.f_GetFirst()->m_Value) == DMibExpr(nItems) - DMibExpr(nToSplit));
				DMibTest(DMibExpr(OtherList.f_GetLast()->m_Value) == DMibExpr(nItems) - DMibExpr(1u));
				DMibTest(DMibExpr(TestList.f_GetFirst()->m_Value) == DMibExpr(0u));
				DMibTest(DMibExpr(TestList.f_GetLast()->m_Value) == DMibExpr(nItems) - DMibExpr(nToSplit) - DMibExpr(1u));
				DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(nToSplit));
				DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems) - DMibExpr(nToSplit));

				TestList.f_InsertLast(OtherList);
				DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(3));
				DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(3));
				DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems) && DMibExpr(2));
				DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(0u) && DMibExpr(2));
				{
					bool bCorrectOrder = true;
					for (umint iItem = 0; iItem < nItems; ++iItem)
						bCorrectOrder = bCorrectOrder && DMibListLinkD_Iter(CTestClass, m_Link)(TestList)[aint(iItem)]->m_Value == iItem;
					DMibTest(DMibExpr(bCorrectOrder) && DMibExpr(2));
				}
			}
		}

		// Whole list with one member
		{
			DMibTestPath("One member");
			TestList.f_Clear();
			TestList.f_Insert(Tests[0]);
			OtherList.f_SplitInto(TestList.f_GetFirst(), TestList.f_GetLast());
			DMibTest(DMibExpr(OtherList.f_CheckList(false)));
			DMibTest(DMibExpr(TestList.f_CheckList(false)));
			DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(0u));
			DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(1u));
			TestList.f_Insert(OtherList);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(2));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(2));
			DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(1u));
			DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(0u));

			// Insert empty list into non empty list
			TestList.f_Insert(OtherList);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(3));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(3));

			// Insert non-empty list into empty list
			OtherList.f_Insert(TestList);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(4));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(4));

			// Insert empty list into empty list
			OtherList.f_Clear();
			TestList.f_Clear();
			OtherList.f_Insert(TestList);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(5));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(5));
		}
	}

	void CLinkedList_Tests::f_InvasiveSmall()
	{
		DMibListLinkDS_List(CTestS, m_Link) TestList;
		DMibListLinkDS_List(CTestS, m_Link) OtherList;
		CTestS Tests[10];
		for (aint iItem = 0; iItem < 10; ++iItem)
		{
			Tests[iItem].m_Value = iItem;
			TestList.f_Insert(Tests[iItem]);
		}

		// Whole list
		OtherList.f_SplitInto(TestList.f_GetFirst(), TestList.f_GetLast());
		DMibTest(DMibExpr(OtherList.f_CheckList(false)));
		DMibTest(DMibExpr(TestList.f_CheckList(false)));
		DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(0u));
		DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(10u));
		TestList.f_Insert(OtherList);
		DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(2));
		DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(2));
		DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(10u));
		DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(0u));

		for (umint iCase = 0; iCase < 4; ++iCase)
		{
			DMibTestPath(NMib::NStr::CStr::CFormat("Split {}") << iCase);
			umint nItems = 10;
			umint nToSplit = 5;
			if (iCase == 1)
				nToSplit = 1;
			else if (iCase == 2)
				nToSplit = 9;
			else if (iCase == 3)
			{
				nToSplit = 1;
				TestList.f_Clear();
				TestList.f_Insert(Tests[0]);
				TestList.f_Insert(Tests[1]);
				nItems = 2;
			}
			// Test splitting in the middle

			{
				DMibTestPath("Middle");

				OtherList.f_SplitInto(&Tests[1], &Tests[nToSplit]);
				DMibTest(DMibExpr(OtherList.f_CheckList(false)));
				DMibTest(DMibExpr(TestList.f_CheckList(false)));
				DMibTest(DMibExpr(OtherList.f_GetFirst()->m_Value) == DMibExpr(1u));
				DMibTest(DMibExpr(OtherList.f_GetLast()->m_Value) == DMibExpr(nToSplit));
				DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(nToSplit));
				DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems) - DMibExpr(nToSplit));

				TestList.f_InsertAfter(OtherList, &Tests[0]);
				DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(2));
				DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(2));
				DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems));
				DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(0u));
				{
					bool bCorrectOrder = true;
					for (umint iItem = 0; iItem < nItems; ++iItem)
						bCorrectOrder = bCorrectOrder && DMibListLinkDS_Iter(CTestS, m_Link)(TestList)[aint(iItem)]->m_Value == iItem;
					DMibTest(DMibExpr(bCorrectOrder));
				}
			}

			// Test splitting at beginning
			{
				DMibTestPath("Beginning");

				OtherList.f_SplitInto(&Tests[0], &Tests[nToSplit - 1]);
				DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(3));
				DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(3));
				DMibTest(DMibExpr(OtherList.f_GetFirst()->m_Value) == DMibExpr(0u));
				DMibTest(DMibExpr(OtherList.f_GetLast()->m_Value) == DMibExpr(nToSplit) - DMibExpr(1u));
				DMibTest(DMibExpr(TestList.f_GetFirst()->m_Value) == DMibExpr(nToSplit));
				DMibTest(DMibExpr(TestList.f_GetLast()->m_Value) == DMibExpr(nItems) - DMibExpr(1u));
				DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(nToSplit));
				DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems) - DMibExpr(nToSplit));

				TestList.f_InsertFirst(OtherList);
				DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(4));
				DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(4));
				DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems));
				DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(0u));
				{
					bool bCorrectOrder = true;
					for (umint iItem = 0; iItem < nItems; ++iItem)
						bCorrectOrder = bCorrectOrder && DMibListLinkDS_Iter(CTestS, m_Link)(TestList)[aint(iItem)]->m_Value == iItem;
					DMibTest(DMibExpr(bCorrectOrder));
				}
			}

			// Test splitting at end
			{
				DMibTestPath("End");

				OtherList.f_SplitInto(&Tests[nItems - nToSplit], &Tests[nItems - 1]);
				DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(5));
				DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(5));
				DMibTest(DMibExpr(OtherList.f_GetFirst()->m_Value) == DMibExpr(nItems) - DMibExpr(nToSplit));
				DMibTest(DMibExpr(OtherList.f_GetLast()->m_Value) == DMibExpr(nItems) - DMibExpr(1u));
				DMibTest(DMibExpr(TestList.f_GetFirst()->m_Value) == DMibExpr(0u));
				DMibTest(DMibExpr(TestList.f_GetLast()->m_Value) == DMibExpr(nItems) - DMibExpr(nToSplit) - DMibExpr(1u));
				DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(nToSplit));
				DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems) - DMibExpr(nToSplit));

				TestList.f_InsertLast(OtherList);
				DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(6));
				DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(6));
				DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(nItems));
				DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(0u));
				{
					bool bCorrectOrder = true;
					for (umint iItem = 0; iItem < nItems; ++iItem)
						bCorrectOrder = bCorrectOrder && DMibListLinkDS_Iter(CTestS, m_Link)(TestList)[aint(iItem)]->m_Value == iItem;
					DMibTest(DMibExpr(bCorrectOrder));
				}
			}
		}

		// Whole list with one member
		{
			DMibTestPath("One member");

			TestList.f_Clear();
			TestList.f_Insert(Tests[0]);
			OtherList.f_SplitInto(TestList.f_GetFirst(), TestList.f_GetLast());
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(3));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(3));
			DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(0u));
			DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(1u));
			TestList.f_Insert(OtherList);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(4));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(4));
			DMibTest(DMibExpr(TestList.f_GetLen()) == DMibExpr(1u));
			DMibTest(DMibExpr(OtherList.f_GetLen()) == DMibExpr(0u));

			// Insert empty list into non empty list
			TestList.f_Insert(OtherList);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(5));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(5));

			// Insert non-empty list into empty list
			OtherList.f_Insert(TestList);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(6));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(6));

			// Insert empty list into empty list
			OtherList.f_Clear();
			TestList.f_Clear();
			OtherList.f_Insert(TestList);
			DMibTest(DMibExpr(OtherList.f_CheckList(false)) && DMibExpr(7));
			DMibTest(DMibExpr(TestList.f_CheckList(false)) && DMibExpr(7));
		}
	}

	void CLinkedList_Tests::f_Dynamic()
	{
		NMib::NContainer::TCLinkedList<int32> List;
		// Test const insert
		List.f_Insert(0);
		// Test non-const insert
		for (int32 iItem = 1; iItem < 10; ++iItem)
			List.f_Insert(iItem);

		int32 nItems = 10;

		DMibTest(DMibExpr(List.f_GetLen()) == DMibExpr(10u));
		{
			bool bCorrectOrder = true;
			NMib::NContainer::TCLinkedList<int32>::CIterator iIter = List;
			for (int32 iItem = 0; iItem < nItems; ++iItem, ++iIter)
				bCorrectOrder = bCorrectOrder && *iIter == iItem;
			DMibTest(DMibExpr(bCorrectOrder));
		}
		{
			bool bCorrectOrder = true;
			NMib::NContainer::TCLinkedList<int32>::CIteratorConst iIter = List;
			for (int32 iItem = 0; iItem < nItems; ++iItem, ++iIter)
				bCorrectOrder = bCorrectOrder && *iIter == iItem;
			DMibTest(DMibExpr(bCorrectOrder) && DMibExpr(2));
		}

		List.f_InsertFirst(11);
		DMibTest(DMibExpr(List.f_GetFirst()) == DMibExpr(11));

		int32 ToInsertFirst = 55;
		int32 &Inserted = List.f_InsertFirst(ToInsertFirst);
		DMibTest(DMibExpr(List.f_GetFirst()) == DMibExpr(55));

		List.f_Remove(Inserted);
		DMibTest(DMibExpr(List.f_GetFirst()) == DMibExpr(11) && DMibExpr(2));

		List.f_Push(12);
		DMibTest(DMibExpr(List.f_Pop()) == DMibExpr(12));

		int32 ToPush = 77;
		List.f_Push(ToPush);
		DMibTest(DMibExpr(List.f_Pop()) == DMibExpr(77));

		List.f_InsertLast(13);
		DMibTest(DMibExpr(List.f_GetLast()) == DMibExpr(13));

		int32 ToInsertLast = 66;
		int32 &InsertedLast = List.f_InsertLast(ToInsertLast);
		DMibTest(DMibExpr(List.f_GetLast()) == DMibExpr(66));

		List.f_Remove(InsertedLast);
		DMibTest(DMibExpr(List.f_GetLast()) == DMibExpr(13) && DMibExpr(2));

		NMib::NContainer::TCLinkedList<int32> List2 = List;
		DMibTest(DMibExpr(List2 == List));

		NMib::NContainer::TCLinkedList<int32> List3;
		List3 = List;
		DMibTest(DMibExpr(List3 == List));

		{
			NMib::NContainer::TCLinkedList<int32> const &ListConst = List;

			DMibTest(DMibExpr(ListConst.f_GetFirst()) == DMibExpr(11) && DMibExpr(3));
			DMibTest(DMibExpr(ListConst.f_GetLast()) == DMibExpr(13) && DMibExpr(3));
		}

		List2.f_Remove(List2.f_GetLast());
		List3.f_Remove(List3.f_GetFirst());

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
			List.f_InsertAfter(List.f_GetFirst()) = 77;
			auto iIter = List.f_GetIterator();
			++iIter;
			DMibTest(DMibExpr(*iIter) == DMibExpr(77));
		}
		{
			auto iIter = List.f_GetIterator();
			List.f_InsertAfter(iIter) = 88;
			++iIter;
			DMibTest(DMibExpr(*iIter) == DMibExpr(88));
		}
		{
			auto iIter = List.f_GetIterator();
			List.f_InsertAfter(99, iIter);
			++iIter;
			DMibTest(DMibExpr(*iIter) == DMibExpr(99));
		}
		{
			List.f_InsertAfter(111, List.f_GetFirst());
			auto iIter = List.f_GetIterator();
			++iIter;
			DMibTest(DMibExpr(*iIter) == DMibExpr(111));
		}

		// InsertBefore Tests
		{
			List.f_InsertBefore(List.f_GetFirst()) = 112;
			auto iIter = List.f_GetIterator();
			DMibTest(DMibExpr(*iIter) == DMibExpr(112));
		}
		{
			auto iIter = List.f_GetIterator();
			List.f_InsertBefore(iIter) = 113;
			iIter = List.f_GetIterator();
			DMibTest(DMibExpr(*iIter) == DMibExpr(113));
		}
		{
			auto iIter = List.f_GetIterator();
			List.f_InsertBefore(114, iIter);
			iIter = List.f_GetIterator();
			DMibTest(DMibExpr(*iIter) == DMibExpr(114));
		}
		{
			List.f_InsertBefore(115, List.f_GetFirst());
			auto iIter = List.f_GetIterator();
			DMibTest(DMibExpr(*iIter) == DMibExpr(115));
		}

		List.f_Clear();
		DMibTest(DMibExpr(List.f_IsEmpty()));
	}

	void CLinkedList_Tests::f_DoTests()
	{
		DMibTestSuite("Invasive Normal")
		{
			this->f_InvasiveNormal();
		};

		DMibTestSuite("Invasive Small")
		{
			this->f_InvasiveSmall();
		};

		DMibTestSuite("Dynamic")
		{
			this->f_Dynamic();
		};

		DMibTestSuite("CTAD")
		{
			NMib::NContainer::TCLinkedList List0 = {1, 2, 3};
			static_assert(NMib::NTraits::cIsSame<decltype(List0), NMib::NContainer::TCLinkedList<int>>);
			DMibExpect(List0, ==, (NMib::NContainer::TCLinkedList<int>({1, 2, 3})));

			NMib::NContainer::TCLinkedList List1{1, 2, 3};
			static_assert(NMib::NTraits::cIsSame<decltype(List1), NMib::NContainer::TCLinkedList<int>>);
			DMibExpect(List1, ==, (NMib::NContainer::TCLinkedList<int>({1, 2, 3})));
		};

		DMibTestSuite("Node Extraction")
		{
			using namespace NMib;
			using namespace NMib::NContainer;
			using namespace NMib::NStr;

			// Test f_Extract with reference
			DMibTestCategory("Extract via reference")
			{
				TCLinkedList<int32> List = {1, 2, 3, 4, 5};
				auto iIter = List.f_GetIterator();
				++iIter; // Point to 2
				int32 &Value = *iIter;

				auto Handle = List.f_Extract(Value);
				DMibExpect(*Handle, ==, 2);
				DMibExpect(List.f_GetLen(), ==, 4u);
			};

			// Test iterator f_ExtractNode
			DMibTestCategory("Iterator ExtractNode")
			{
				TCLinkedList<int32> List = {1, 2, 3, 4, 5};
				auto iIter = List.f_GetIterator();
				++iIter; // Point to 2

				auto Handle = iIter.f_ExtractNode();
				DMibExpect(*Handle, ==, 2);
				DMibExpect(*iIter, ==, 3); // Iterator advanced
				DMibExpect(List.f_GetLen(), ==, 4u);
			};

			// Test f_Insert with handle
			DMibTestCategory("Insert extracted node")
			{
				TCLinkedList<int32> Source = {1, 2, 3};
				TCLinkedList<int32> Dest;

				auto iIter = Source.f_GetIterator();
				++iIter; // Point to 2
				auto Handle = iIter.f_ExtractNode();

				Dest.f_Insert(fg_Move(Handle));
				DMibExpect(Source.f_GetLen(), ==, 2u);
				DMibExpect(Dest.f_GetLen(), ==, 1u);
				DMibExpect(Dest.f_GetFirst(), ==, 2);
			};

			// Test f_InsertFirst with handle
			DMibTestCategory("InsertFirst extracted node")
			{
				TCLinkedList<int32> Source = {1, 2, 3};
				TCLinkedList<int32> Dest = {10, 20};

				auto iIter = Source.f_GetIterator();
				auto Handle = iIter.f_ExtractNode();

				Dest.f_InsertFirst(fg_Move(Handle));
				DMibExpect(Dest.f_GetFirst(), ==, 1);
				DMibExpect(Dest.f_GetLen(), ==, 3u);
			};

			// Test f_InsertAfter with handle
			DMibTestCategory("InsertAfter extracted node")
			{
				TCLinkedList<int32> Source = {1, 2, 3};
				TCLinkedList<int32> Dest = {10, 20, 30};

				auto iSource = Source.f_GetIterator();
				auto Handle = iSource.f_ExtractNode();

				auto iDest = Dest.f_GetIterator();
				++iDest; // Point to 20

				Dest.f_InsertAfter(fg_Move(Handle), iDest);
				// Order should be: 10, 20, 1, 30
				auto iCheck = Dest.f_GetIterator();
				DMibExpect(*iCheck, ==, 10);
				++iCheck;
				DMibExpect(*iCheck, ==, 20);
				++iCheck;
				DMibExpect(*iCheck, ==, 1);
				++iCheck;
				DMibExpect(*iCheck, ==, 30);
			};

			// Test transfer all elements between lists
			DMibTestCategory("Transfer all elements")
			{
				TCLinkedList<int32> Source = {1, 2, 3, 4, 5};
				TCLinkedList<int32> Dest;

				for (auto iIter = Source.f_GetIterator(); iIter;)
				{
					auto Handle = iIter.f_ExtractNode();
					Dest.f_Insert(fg_Move(Handle));
				}

				DMibExpect(Source.f_IsEmpty(), ==, true);
				DMibExpect(Dest.f_GetLen(), ==, 5u);

				// Verify order is preserved
				auto iCheck = Dest.f_GetIterator();
				for (int32 iItem = 1; iItem <= 5; ++iItem, ++iCheck)
					DMibExpect(*iCheck, ==, iItem)(ETestFlag_Aggregated);
			};

			// Test handle destructor properly cleans up
			DMibTestCategory("Handle destructor")
			{
				TCLinkedList<int32> List = {1, 2, 3};

				{
					DMibTestPath("Inner");
					auto iIter = List.f_GetIterator();
					auto Handle = iIter.f_ExtractNode();
					DMibExpect(List.f_GetLen(), ==, 2u);
					// Handle goes out of scope and should clean up
				}

				DMibExpect(List.f_GetLen(), ==, 2u);
			};

			// Test handle move semantics
			DMibTestCategory("Handle move semantics")
			{
				TCLinkedList<int32> List = {1, 2, 3};
				auto iIter = List.f_GetIterator();
				auto Handle1 = iIter.f_ExtractNode();

				// Move construct
				TCLinkedList<int32>::CNodeHandle Handle2(fg_Move(Handle1));
				DMibExpect(Handle1.f_IsEmpty(), ==, true);
				DMibExpect(Handle2.f_IsEmpty(), ==, false);
				DMibExpect(*Handle2, ==, 1);

				// Move assign
				TCLinkedList<int32>::CNodeHandle Handle3;
				Handle3 = fg_Move(Handle2);
				DMibExpect(Handle2.f_IsEmpty(), ==, true);
				DMibExpect(Handle3.f_IsEmpty(), ==, false);
				DMibExpect(*Handle3, ==, 1);
			};

			// Test with complex type (CStr)
			DMibTestCategory("Complex type extraction")
			{
				TCLinkedList<CStr> List;
				List.f_Insert("Hello");
				List.f_Insert("World");
				List.f_Insert("Test");

				auto iIter = List.f_GetIterator();
				++iIter; // Point to "World"

				auto Handle = iIter.f_ExtractNode();
				DMibExpect(*Handle, ==, CStr("World"));

				TCLinkedList<CStr> Dest;
				Dest.f_Insert(fg_Move(Handle));
				DMibExpect(Dest.f_GetFirst(), ==, CStr("World"));
			};
		};
	}

	DMibTestRegister(CLinkedList_Tests, Malterlib::Container);
}
