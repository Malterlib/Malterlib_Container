// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Test/Exception>

/*************************************************************************************************\
|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
| CTestSort
|__________________________________________________________________________________________________
\*************************************************************************************************/

namespace
{
	class CVector_Tests : public NMib::NTest::CTest
	{
	public:

		static NMib::NContainer::TCVector<int32> fs_Vector(int32 _0, int32 _1, int32 _2, int32 _3, int32 _4, int32 _5, int32 _6, int32 _7, int32 _8, int32 _9)
		{
			NMib::NContainer::TCVector<int32> Ret;
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

		NMib::NContainer::TCVector<int32> fs_DefaultList()
		{
			NMib::NContainer::TCVector<int32> Ret;

			for (int32 i = 0; i < 10; ++i)
				Ret.f_Insert(i);

			return Ret;
		}
		void f_Dynamic()
		{
			NMib::NContainer::TCVector<int32> List;
			DMibTestSuite("Basic")
			{
			// Test const insert
			int32 Test(0);
			List.f_Insert(Test);
			// Test non-const insert
			for (int32 i = 1; i < 10; ++i)
				List.f_Insert(i);

			int32 nItems = 10;

			DMibTest(DMibExpr(List.f_GetLen() == 10));
			{
				bint bCorrectOrder = true;
				NMib::NContainer::TCVector<int32>::CIterator Iter = List.f_GetIterator();
				for (int32 i = 0; i < nItems; ++i, ++Iter)
					bCorrectOrder = bCorrectOrder && *Iter == i;
				DMibTest(DMibExpr(bCorrectOrder));
			}
			{
				bint bCorrectOrder2 = true;
				NMib::NContainer::TCVector<int32>::CIteratorConst Iter = NMib::fg_Const(List).f_GetIterator();
				for (int32 i = 0; i < nItems; ++i, ++Iter)
					bCorrectOrder2 = bCorrectOrder2 && *Iter == i;
				DMibTest(DMibExpr(bCorrectOrder2));
			}

			List.f_InsertFirst(11);
			DMibTest(DMibExpr(List.f_GetFirst()) == DMibExpr(11));

			int32 ToInsertFirst = 55;
			int32 &Inserted = List.f_InsertFirst(ToInsertFirst);
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
			int32 &InsertedLast = List.f_InsertLast(ToInsertLast);
			DMibTest(DMibExpr(List.f_GetLast()) == DMibExpr(66));

			List.f_Remove(List.f_GetLen() - 1);
			DMibTest(DMibExpr(List.f_GetLast()) == DMibExpr(13) && DMibExpr(2));

			NMib::NContainer::TCVector<int32> List2 = List;
			DMibTest(DMibExpr(List2) == DMibExpr(List));

			NMib::NContainer::TCVector<int32> List3;
			List3 = List;
			DMibTest(DMibExpr(List3) == DMibExpr(List));

			{
				NMib::NContainer::TCVector<int32> const &ListConst = List;

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

				DMibTest(DMibExpr(TCThrowsException<NMib::NException::CException>()) == DMibLExpr(List[-1]));
				DMibTest(DMibExpr(TCThrowsException<NMib::NException::CException>()) == DMibLExpr(List[10]));

				DMibTest(DMibExpr(TCThrowsException<NMib::NException::CException>()) == DMibLExpr(List.f_InsertAfter(10)));
				DMibTest(DMibExpr(TCThrowsException<NMib::NException::CException>()) == DMibLExpr(List.f_InsertBefore(11)));


				// Out of bounds move
				DMibTest(DMibExpr(TCThrowsException<NMib::NException::CException>()) == DMibLExpr(List.f_Move(9, 0, 2)));
				DMibTest(DMibExpr(TCThrowsException<NMib::NException::CException>()) == DMibLExpr(List.f_Move(0, 11, 2)));

				// Overlapping ranges
				DMibTest(DMibExpr(TCThrowsException<NMib::NException::CException>()) == DMibLExpr(List.f_Move(0, 2, 3)));
				DMibTest(DMibExpr(TCThrowsException<NMib::NException::CException>()) == DMibLExpr(List.f_Move(5, 2, 4)));
			};


		}

		void f_DoTests()
		{

			DMibTestCategory("Dynamic")
			{
				this->f_Dynamic();
			};
		}	
	};
	
	DMibTestRegister(CVector_Tests, Malterlib::Container);
}

