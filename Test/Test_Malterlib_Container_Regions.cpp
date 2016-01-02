// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Container/Regions>

namespace
{
	class CRegion_Tests : public NMib::NTest::CTest
	{
	public:

		void f_DoTests()
		{
			DMibTestCategory("Basic")
			{
				DMibTestSuite("Whole")
				{
					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Separate")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(150));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Merge overlapping")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(50, 175);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Merge overlapping left")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(50, 120);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(120));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(150));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Merge overlapping right")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(120, 175);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(120));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Merge equal left")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(100, 120);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(120));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(150));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Merge equal right")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(120, 150);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(120));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Merge overlapping left equal right")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(50, 150);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Merge overlapping right equal left")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(100, 175);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Merge equal")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(100, 150);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Before")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(-100, -10);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(-100));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(-10));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("3"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(150));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Before equal")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(-100, 0);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(-100));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(150));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Before overlapping")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(-100, 10);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(-100));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(150));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("After")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(210, 300);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(150));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(200));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("3"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(210));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(300));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("After equal")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(200, 300);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(150));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(300));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("After overlapping")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(190, 300);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(150));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(300));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Over")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(210, 300);
					Regions.f_MakeRegion(-10, 310);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(-10));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(310));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Over equal left")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(210, 300);
					Regions.f_MakeRegion(0, 310);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(310));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Over equal right")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(210, 300);
					Regions.f_MakeRegion(-10, 300);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(-10));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(300));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Over equal")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(210, 300);
					Regions.f_MakeRegion(0, 300);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(300));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Over middle")
				{

					NMib::NContainer::TCRegions<int32, NMib::CVoidTag> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion(150, 200);
					Regions.f_MakeRegion(210, 300);
					Regions.f_MakeRegion(10, 290);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(300));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Split left ovelapping")
				{

					NMib::NContainer::TCRegions<int32, NMib::TCAutoClearInt<int32>> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion
						(
							-10
							, 10
							, [&](NMib::TCAutoClearInt<int32> &_Data)
							{
								_Data = 1;
							}
						)
					;
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(-10));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(10));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(10));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(0));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Split left equal")
				{

					NMib::NContainer::TCRegions<int32, NMib::TCAutoClearInt<int32>> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion
						(
							0
							, 10
							, [&](NMib::TCAutoClearInt<int32> &_Data)
							{
								_Data = 1;
							}
						)
					;
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(10));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(10));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(0));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Split right ovelapping")
				{

					NMib::NContainer::TCRegions<int32, NMib::TCAutoClearInt<int32>> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion
						(
							90
							, 110
							, [&](NMib::TCAutoClearInt<int32> &_Data)
							{
								_Data = 1;
							}
						)
					;
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(90));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(0));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(90));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(110));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Split right equal")
				{

					NMib::NContainer::TCRegions<int32, NMib::TCAutoClearInt<int32>> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion
						(
							90
							, 100
							, [&](NMib::TCAutoClearInt<int32> &_Data)
							{
								_Data = 1;
							}
						)
					;
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(90));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(0));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(90));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
				DMibTestSuite("Split middle")
				{

					NMib::NContainer::TCRegions<int32, NMib::TCAutoClearInt<int32>> Regions;
					Regions.f_MakeRegion(0, 100);
					Regions.f_MakeRegion
						(
							40
							, 60
							, [&](NMib::TCAutoClearInt<int32> &_Data)
							{
								_Data = 1;
							}
						)
					;
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(40));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(0));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(40));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(60));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("3"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(60));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(100));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(0) && DMibExpr("4"));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
			};
			DMibTestCategory("Bugs")
			{
				DMibTestSuite("Change state")
				{
					NMib::NContainer::TCRegions<int32, NMib::TCAutoClearInt<int32>> Regions;
					auto fl_Set1
						= [](NMib::TCAutoClearInt<int32> &_Data)
						{
							_Data = 1;
						}
					;
					Regions.f_MakeRegion(0, 8);
					Regions.f_MakeRegion(0, 2, fl_Set1);
					Regions.f_MakeRegion(2, 4, fl_Set1);
					Regions.f_MakeRegion(4, 6, fl_Set1);
					auto Iter = Regions.f_GetIterator();
					DMibTest(DMibExpr(Iter))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(0));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(6));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(1));
					++Iter;
					DMibTest(DMibExpr(Iter) && DMibExpr("2"))(ETest_FailAndStop);
					DMibTest(DMibExpr(Iter->f_Start()) == DMibExpr(6));
					DMibTest(DMibExpr(Iter->f_End()) == DMibExpr(8));
					DMibTest(DMibExpr(Iter->f_Data()) == DMibExpr(0));
					++Iter;
					DMibTest(DMibExpr(!Iter))(ETest_FailAndStop);
				};
			};
		}
	};

	DMibTestRegister(CRegion_Tests, Malterlib::Container);
}

