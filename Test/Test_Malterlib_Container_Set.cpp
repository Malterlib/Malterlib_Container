// Copyright © 2025 Unborken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Memory/MemoryManager>
#include <Mib/Test/Performance>

namespace
{
	using namespace NMib;
	using namespace NMib::NTraits;
	using namespace NMib::NStr;
	using namespace NMib::NContainer;

	class CSet_Tests : public NMib::NTest::CTest
	{
	public:
		void f_DoTests()
		{
			DMibTestSuite("SetOperations")
			{
				DMibTestCategory("Union")
				{
					DMibTestCategory("BothEmpty")
					{
						TCSet<int> Left;
						TCSet<int> Right;
						DMibTest((DMibExpr(Left) | DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("LeftEmpty")
					{
						TCSet<int> Left;
						TCSet<int> Right{1, 2, 3};
						DMibTest((DMibExpr(Left) | DMibExpr(Right)) == DMibExpr(Right));
					};
					DMibTestCategory("RightEmpty")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right;
						DMibTest((DMibExpr(Left) | DMibExpr(Right)) == DMibExpr(Left));
					};
					DMibTestCategory("Disjoint")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{4, 5, 6};
						TCSet<int> Expected{1, 2, 3, 4, 5, 6};
						DMibTest((DMibExpr(Left) | DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("Overlapping")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{2, 3, 4};
						TCSet<int> Expected{1, 2, 3, 4};
						DMibTest((DMibExpr(Left) | DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("Identical")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{1, 2, 3};
						TCSet<int> Expected{1, 2, 3};
						DMibTest((DMibExpr(Left) | DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("LeftSubset")
					{
						TCSet<int> Left{2, 3};
						TCSet<int> Right{1, 2, 3, 4};
						DMibTest((DMibExpr(Left) | DMibExpr(Right)) == DMibExpr(Right));
					};
					DMibTestCategory("RightSubset")
					{
						TCSet<int> Left{1, 2, 3, 4};
						TCSet<int> Right{2, 3};
						DMibTest((DMibExpr(Left) | DMibExpr(Right)) == DMibExpr(Left));
					};
				};

				DMibTestCategory("Intersection")
				{
					DMibTestCategory("BothEmpty")
					{
						TCSet<int> Left;
						TCSet<int> Right;
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("LeftEmpty")
					{
						TCSet<int> Left;
						TCSet<int> Right{1, 2, 3};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("RightEmpty")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right;
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("Disjoint")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{4, 5, 6};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("Overlapping")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{2, 3, 4};
						TCSet<int> Expected{2, 3};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("Identical")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{1, 2, 3};
						TCSet<int> Expected{1, 2, 3};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("LeftSubset")
					{
						TCSet<int> Left{2, 3};
						TCSet<int> Right{1, 2, 3, 4};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(Left));
					};
					DMibTestCategory("RightSubset")
					{
						TCSet<int> Left{1, 2, 3, 4};
						TCSet<int> Right{2, 3};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(Right));
					};
					// Edge case: left exhausted before right (original bug trigger)
					DMibTestCategory("LeftExhaustedFirst")
					{
						TCSet<int> Left{1, 2};
						TCSet<int> Right{3, 4, 5};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					// Edge case: right exhausted before left (original bug trigger)
					DMibTestCategory("RightExhaustedFirst")
					{
						TCSet<int> Left{3, 4, 5};
						TCSet<int> Right{1, 2};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					// Edge case that triggered the original crash
					DMibTestCategory("RightExhaustedAfterMatch")
					{
						TCSet<int> Left{1, 3, 5};
						TCSet<int> Right{2, 3};
						TCSet<int> Expected{3};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(Expected));
					};
				};

				DMibTestCategory("SymmetricDifference")
				{
					DMibTestCategory("BothEmpty")
					{
						TCSet<int> Left;
						TCSet<int> Right;
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("LeftEmpty")
					{
						TCSet<int> Left;
						TCSet<int> Right{1, 2, 3};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Right));
					};
					DMibTestCategory("RightEmpty")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right;
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Left));
					};
					DMibTestCategory("Disjoint")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{4, 5, 6};
						TCSet<int> Expected{1, 2, 3, 4, 5, 6};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("Overlapping")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{2, 3, 4};
						TCSet<int> Expected{1, 4};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("Identical")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{1, 2, 3};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("LeftSubset")
					{
						TCSet<int> Left{2, 3};
						TCSet<int> Right{1, 2, 3, 4};
						TCSet<int> Expected{1, 4};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("RightSubset")
					{
						TCSet<int> Left{1, 2, 3, 4};
						TCSet<int> Right{2, 3};
						TCSet<int> Expected{1, 4};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					// Edge case: left exhausted before right
					DMibTestCategory("LeftExhaustedFirst")
					{
						TCSet<int> Left{1, 2};
						TCSet<int> Right{3, 4, 5};
						TCSet<int> Expected{1, 2, 3, 4, 5};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					// Edge case: right exhausted before left
					DMibTestCategory("RightExhaustedFirst")
					{
						TCSet<int> Left{3, 4, 5};
						TCSet<int> Right{1, 2};
						TCSet<int> Expected{1, 2, 3, 4, 5};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					// Edge case that would trigger crash with original bug
					DMibTestCategory("RightExhaustedAfterMatch")
					{
						TCSet<int> Left{1, 3, 5};
						TCSet<int> Right{2, 3};
						TCSet<int> Expected{1, 2, 5};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
				};

				DMibTestCategory("Difference")
				{
					DMibTestCategory("BothEmpty")
					{
						TCSet<int> Left;
						TCSet<int> Right;
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("LeftEmpty")
					{
						TCSet<int> Left;
						TCSet<int> Right{1, 2, 3};
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("RightEmpty")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right;
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(Left));
					};
					DMibTestCategory("Disjoint")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{4, 5, 6};
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(Left));
					};
					DMibTestCategory("Overlapping")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{2, 3, 4};
						TCSet<int> Expected{1};
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("Identical")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{1, 2, 3};
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("LeftSubset")
					{
						TCSet<int> Left{2, 3};
						TCSet<int> Right{1, 2, 3, 4};
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(TCSet<int>()));
					};
					DMibTestCategory("RightSubset")
					{
						TCSet<int> Left{1, 2, 3, 4};
						TCSet<int> Right{2, 3};
						TCSet<int> Expected{1, 4};
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(Expected));
					};
				};

				DMibTestCategory("CompoundAssignment")
				{
					DMibTestCategory("UnionAssign")
					{
						TCSet<int> Left{1, 2, 3};
						TCSet<int> Right{3, 4, 5};
						Left += Right;
						TCSet<int> Expected{1, 2, 3, 4, 5};
						DMibTest(DMibExpr(Left) == DMibExpr(Expected));
					};
					DMibTestCategory("DifferenceAssign")
					{
						TCSet<int> Left{1, 2, 3, 4};
						TCSet<int> Right{2, 3};
						Left -= Right;
						TCSet<int> Expected{1, 4};
						DMibTest(DMibExpr(Left) == DMibExpr(Expected));
					};
				};

				DMibTestCategory("WithStrings")
				{
					DMibTestCategory("Intersection")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"b", "c", "d"};
						TCSet<CStr> Expected{"b", "c"};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("IntersectionRightExhausted")
					{
						// This is the pattern that triggered the original crash
						TCSet<CStr> Left{"a-tag", "b-tag", "c-tag"};
						TCSet<CStr> Right{"b-tag", "c-tag"};
						TCSet<CStr> Expected{"b-tag", "c-tag"};
						DMibTest((DMibExpr(Left) & DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("SymmetricDifference")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"b", "c", "d"};
						TCSet<CStr> Expected{"a", "d"};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
				};

				DMibTestCategory("MoveSemantics")
				{
					DMibTestCategory("UnionMoveLeft")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"c", "d", "e"};
						TCSet<CStr> Expected{"a", "b", "c", "d", "e"};
						auto Result = fg_Move(Left) | Right;
						DMibExpect(Result, ==, Expected);
						DMibExpectTrue(Left.f_IsEmpty());
					};
					DMibTestCategory("UnionMoveRight")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"c", "d", "e"};
						TCSet<CStr> Expected{"a", "b", "c", "d", "e"};
						auto Result = Left | fg_Move(Right);
						DMibExpect(Result, ==, Expected);
						DMibExpectTrue(Right.f_IsEmpty());
					};
					DMibTestCategory("UnionMoveBoth")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"c", "d", "e"};
						TCSet<CStr> Expected{"a", "b", "c", "d", "e"};
						auto Result = fg_Move(Left) | fg_Move(Right);
						DMibExpect(Result, ==, Expected);
						DMibExpectTrue(Left.f_IsEmpty());
						DMibExpectTrue(Right.f_IsEmpty());
					};
					DMibTestCategory("IntersectionMoveLeft")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"b", "c", "d"};
						TCSet<CStr> Expected{"b", "c"};
						auto Result = fg_Move(Left) & Right;
						DMibExpect(Result, ==, Expected);
						DMibExpectTrue(Left.f_IsEmpty());
					};
					DMibTestCategory("IntersectionMoveRight")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"b", "c", "d"};
						TCSet<CStr> Expected{"b", "c"};
						auto Result = Left & fg_Move(Right);
						DMibExpect(Result, ==, Expected);
						DMibExpectTrue(Right.f_IsEmpty());
					};
					DMibTestCategory("IntersectionMoveBoth")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"b", "c", "d"};
						TCSet<CStr> Expected{"b", "c"};
						auto Result = fg_Move(Left) & fg_Move(Right);
						DMibExpect(Result, ==, Expected);
						DMibExpectFalse(Left.f_IsEmpty()); // We always use the right side when intersecting
						DMibExpectTrue(Right.f_IsEmpty());
					};
					DMibTestCategory("XorMoveLeft")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"b", "c", "d"};
						TCSet<CStr> Expected{"a", "d"};
						auto Result = fg_Move(Left) ^ Right;
						DMibExpect(Result, ==, Expected);
						DMibExpectTrue(Left.f_IsEmpty());
					};
					DMibTestCategory("XorMoveRight")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"b", "c", "d"};
						TCSet<CStr> Expected{"a", "d"};
						auto Result = Left ^ fg_Move(Right);
						DMibExpect(Result, ==, Expected);
						DMibExpectTrue(Right.f_IsEmpty());
					};
					DMibTestCategory("XorMoveBoth")
					{
						TCSet<CStr> Left{"a", "b", "c"};
						TCSet<CStr> Right{"b", "c", "d"};
						TCSet<CStr> Expected{"a", "d"};
						auto Result = fg_Move(Left) ^ fg_Move(Right);
						DMibExpect(Result, ==, Expected);
						DMibExpectTrue(Left.f_IsEmpty());
						DMibExpectTrue(Right.f_IsEmpty());
					};
					DMibTestCategory("DifferenceMoveLeft")
					{
						TCSet<CStr> Left{"a", "b", "c", "d"};
						TCSet<CStr> Right{"b", "c"};
						TCSet<CStr> Expected{"a", "d"};
						auto Result = fg_Move(Left) - Right;
						DMibExpect(Result, ==, Expected);
						DMibExpectTrue(Left.f_IsEmpty());
					};
					DMibTestCategory("DifferenceLvalueLeft")
					{
						// Difference with lvalue left should not modify left
						TCSet<CStr> Left{"a", "b", "c", "d"};
						TCSet<CStr> Right{"b", "c"};
						TCSet<CStr> Expected{"a", "d"};
						auto Result = Left - Right;
						DMibExpect(Result, ==, Expected);
						DMibExpect(Left.f_GetLen(), ==, 4); // Left unchanged
					};
					DMibTestCategory("DestructiveIterator")
					{
						TCSet<CStr> Source{"a", "b", "c"};
						TCVector<CStr> Collected;
						for (auto iIter = fg_Move(Source).f_GetIteratorDestructive(); iIter; ++iIter)
							Collected.f_InsertLast(fg_Move(*iIter));
						DMibExpectTrue(Source.f_IsEmpty());
						DMibExpect(Collected.f_GetLen(), ==, 3);
					};
				};
				DMibTestCategory("NormalCompare")
				{
					DMibTestCategory("Xor")
					{
						TCSet<int> Left = {1, 2, 3};
						TCSet<int> Right = {2, 4};

						TCSet<int> Expected = {1, 3, 4};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("XorMove")
					{
						TCSet<int> Left = {1, 2, 3};
						TCSet<int> Right = {2, 4};

						TCSet<int> Expected = {1, 3, 4};
						DMibTest((DMibExpr(fg_Move(Left)) ^ DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("XorMoveLeft")
					{
						TCSet<int> Left = {1, 2, 3};
						TCSet<int> Right = {2, 4};

						TCSet<int> Expected = {1, 3, 4};
						DMibTest((DMibExpr(fg_Move(Left)) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("XorMoveRight")
					{
						TCSet<int> Left = {1, 2, 3};
						TCSet<int> Right = {2, 4};

						TCSet<int> Expected = {1, 3, 4};
						DMibTest((DMibExpr(Left) ^ DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("Difference")
					{
						TCSet<int> Left = {1, 2, 3};
						TCSet<int> Right = {2, 4};

						TCSet<int> Expected = {1, 3};
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("DifferenceMove")
					{
						TCSet<int> Left = {1, 2, 3};
						TCSet<int> Right = {2, 4};

						TCSet<int> Expected = {1, 3};
						DMibTest((DMibExpr(fg_Move(Left)) - DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("DifferenceMoveLeft")
					{
						TCSet<int> Left = {1, 2, 3};
						TCSet<int> Right = {2, 4};

						TCSet<int> Expected = {1, 3};
						DMibTest((DMibExpr(fg_Move(Left)) - DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("DifferenceMoveRight")
					{
						TCSet<int> Left = {1, 2, 3};
						TCSet<int> Right = {2, 4};

						TCSet<int> Expected = {1, 3};
						DMibTest((DMibExpr(Left) - DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
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
						TCSet<int, CCompareReverse> Left = {1, 2, 3};
						TCSet<int, CCompareReverse> Right = {2, 4};

						TCSet<int, CCompareReverse> Expected = {1, 3, 4};
						DMibTest((DMibExpr(Left) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("XorMove")
					{
						TCSet<int, CCompareReverse> Left = {1, 2, 3};
						TCSet<int, CCompareReverse> Right = {2, 4};

						TCSet<int, CCompareReverse> Expected = {1, 3, 4};
						DMibTest((DMibExpr(fg_Move(Left)) ^ DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("XorMoveLeft")
					{
						TCSet<int, CCompareReverse> Left = {1, 2, 3};
						TCSet<int, CCompareReverse> Right = {2, 4};

						TCSet<int, CCompareReverse> Expected = {1, 3, 4};
						DMibTest((DMibExpr(fg_Move(Left)) ^ DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("XorMoveRight")
					{
						TCSet<int, CCompareReverse> Left = {1, 2, 3};
						TCSet<int, CCompareReverse> Right = {2, 4};

						TCSet<int, CCompareReverse> Expected = {1, 3, 4};
						DMibTest((DMibExpr(Left) ^ DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("Difference")
					{
						TCSet<int, CCompareReverse> Left = {1, 2, 3};
						TCSet<int, CCompareReverse> Right = {2, 4};

						TCSet<int, CCompareReverse> Expected = {1, 3};
						DMibTest((DMibExpr(Left) - DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("DifferenceMove")
					{
						TCSet<int, CCompareReverse> Left = {1, 2, 3};
						TCSet<int, CCompareReverse> Right = {2, 4};

						TCSet<int, CCompareReverse> Expected = {1, 3};
						DMibTest((DMibExpr(fg_Move(Left)) - DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
					DMibTestCategory("DifferenceMoveLeft")
					{
						TCSet<int, CCompareReverse> Left = {1, 2, 3};
						TCSet<int, CCompareReverse> Right = {2, 4};

						TCSet<int, CCompareReverse> Expected = {1, 3};
						DMibTest((DMibExpr(fg_Move(Left)) - DMibExpr(Right)) == DMibExpr(Expected));
					};
					DMibTestCategory("DifferenceMoveRight")
					{
						TCSet<int, CCompareReverse> Left = {1, 2, 3};
						TCSet<int, CCompareReverse> Right = {2, 4};

						TCSet<int, CCompareReverse> Expected = {1, 3};
						DMibTest((DMibExpr(Left) - DMibExpr(fg_Move(Right))) == DMibExpr(Expected));
					};
				};
			};
		}
	};

	DMibTestRegister(CSet_Tests, Malterlib::Container);

	class CSetPerformance_Tests : public NMib::NTest::CTest
	{
	public:
		using CTestVector = TCVector<mint>;

		static CTestVector fs_CreateVector(mint _Value)
		{
			CTestVector Result;
			Result.f_SetLen(1); // Force allocation
			Result[0] = _Value;
			return Result;
		}

		template <typename tf_CSet = TCSet<CTestVector>>
		tf_CSet f_CreateSet(mint _Start, mint _Count)
		{
			tf_CSet Result;
			for (mint i = 0; i < _Count; ++i)
				Result[fs_CreateVector(_Start + i)];
			return Result;
		}

		void fp_GarbageCollect()
		{
			auto Checkout = fg_GetSys()->f_MemoryManager_Checkout();
			Checkout.f_CheckMessages();
			Checkout.f_GarbageCollectLocalArena(false); // Garbage collect memory
		}

		void f_DoTests()
		{
			DMibTestSuite(NTest::CTestCategory("MoveSemantics") << NTest::CTestGroup("Performance"))
			{
#if defined(DMibDebug) || defined(DMibSanitizerEnabled)
				constexpr static mint c_nNumElements = 10000;
				constexpr static mint c_nNumIterations = 5;
#else
				constexpr static mint c_nNumElements = 1000000;
				constexpr static mint c_nNumIterations = 5;
#endif
				constexpr static pfp64 AllowedPerformance = 1.001;

				DMibTestCategory("Union")
				{
					NTest::CTestPerformance PerfTest(AllowedPerformance);
					[&] inline_never
					{
						DMibTestPath("Move");
						NTest::CTestPerformanceMeasure MoveMeasure("Move");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<>(0, c_nNumElements);
							auto Right = f_CreateSet<>(c_nNumElements, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) | fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements * 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MoveLeft");
						NTest::CTestPerformanceMeasure MoveMeasure("MoveLeft");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<>(0, c_nNumElements);
							auto Right = f_CreateSet<>(c_nNumElements, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) | Right;
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements * 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MoveRight");
						NTest::CTestPerformanceMeasure MoveMeasure("MoveRight");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<>(0, c_nNumElements);
							auto Right = f_CreateSet<>(c_nNumElements, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = Left | fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements * 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("Copy");
						NTest::CTestPerformanceMeasure CopyMeasure("Copy");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto LeftCopy = f_CreateSet<>(0, c_nNumElements);
							auto RightCopy = f_CreateSet<>(c_nNumElements, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(CopyMeasure, c_nNumElements * 2);
									Result = LeftCopy | RightCopy;
									LeftCopy.f_Clear();
									RightCopy.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements * 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_AddReference(CopyMeasure);
					}
					();

					auto fRunMapTests = [&]<EMapOperationPolicy t_Policy>
						{
							CStr PolicySuffix;
							switch (t_Policy)
							{
							case EMapOperationPolicy::mc_Fastest: PolicySuffix = "Fastest"; break;
							case EMapOperationPolicy::mc_PreferLeft: PolicySuffix = "PLeft"; break;
							case EMapOperationPolicy::mc_PreferRight: PolicySuffix = "PRight"; break;
							}

							[&] inline_never
							{
								DMibTestPath("MapMove" + PolicySuffix);
								NTest::CTestPerformanceMeasure MoveMeasure("MapMove" + PolicySuffix);
								for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
								{
									fp_GarbageCollect();
									auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
									auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements, c_nNumElements);
									TCMap<CTestVector, CMapSet> Result;
									[&]() inline_never
										{
											DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
											Result = fg_Move(Left).f_Or<t_Policy>(fg_Move(Right));
											Left.f_Clear();
											Right.f_Clear();
										}
										()
									;
									DMibExpect(Result.f_GetLen(), ==, c_nNumElements * 2)(ETestFlag_Aggregated);
								}
								PerfTest.f_Add(MoveMeasure);
							}
							();

							[&] inline_never
							{
								DMibTestPath("MapMoveLeft" + PolicySuffix);
								NTest::CTestPerformanceMeasure MoveMeasure("MapMoveLeft" + PolicySuffix);
								for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
								{
									fp_GarbageCollect();
									auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
									auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements, c_nNumElements);
									TCMap<CTestVector, CMapSet> Result;
									[&]() inline_never
										{
											DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
											Result = fg_Move(Left).f_Or<t_Policy>(Right);
											Left.f_Clear();
											Right.f_Clear();
										}
										()
									;
									DMibExpect(Result.f_GetLen(), ==, c_nNumElements * 2)(ETestFlag_Aggregated);
								}
								PerfTest.f_Add(MoveMeasure);
							}
							();

							[&] inline_never
							{
								DMibTestPath("MapMoveRight" + PolicySuffix);
								NTest::CTestPerformanceMeasure MoveMeasure("MapMoveRight" + PolicySuffix);
								for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
								{
									fp_GarbageCollect();
									auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
									auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements, c_nNumElements);
									TCMap<CTestVector, CMapSet> Result;
									[&]() inline_never
										{
											DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
											Result = Left.f_Or<t_Policy>(fg_Move(Right));
											Left.f_Clear();
											Right.f_Clear();
										}
										()
									;
									DMibExpect(Result.f_GetLen(), ==, c_nNumElements * 2)(ETestFlag_Aggregated);
								}
								PerfTest.f_Add(MoveMeasure);
							}
							();

							[&] inline_never
							{
								DMibTestPath("MapCopy" + PolicySuffix);
								NTest::CTestPerformanceMeasure CopyMeasure("MapCopy" + PolicySuffix);
								for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
								{
									fp_GarbageCollect();
									auto LeftCopy = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
									auto RightCopy = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements, c_nNumElements);
									TCMap<CTestVector, CMapSet> Result;
									[&]() inline_never
										{
											DMibTestScopeMeasure(CopyMeasure, c_nNumElements * 2);
											Result = LeftCopy.f_Or<t_Policy>(RightCopy);
											LeftCopy.f_Clear();
											RightCopy.f_Clear();
										}
										()
									;
									DMibExpect(Result.f_GetLen(), ==, c_nNumElements * 2)(ETestFlag_Aggregated);
								}
								PerfTest.f_AddReference(CopyMeasure);
							}
							();
						}
					;

					fRunMapTests.operator() <EMapOperationPolicy::mc_Fastest>();
					fRunMapTests.operator() <EMapOperationPolicy::mc_PreferLeft>();
					fRunMapTests.operator() <EMapOperationPolicy::mc_PreferRight>();

					DMibExpectTrue(PerfTest);
				};

				DMibTestCategory("Intersection")
				{
					NTest::CTestPerformance PerfTest(AllowedPerformance);

					[&] inline_never
					{
						DMibTestPath("Move");
						NTest::CTestPerformanceMeasure MoveMeasure("Move");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<>(0, c_nNumElements);
							auto Right = f_CreateSet<>(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) & fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MoveLeft");
						NTest::CTestPerformanceMeasure MoveMeasure("MoveLeft");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<>(0, c_nNumElements);
							auto Right = f_CreateSet<>(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) & Right;
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MoveRight");
						NTest::CTestPerformanceMeasure MoveMeasure("MoveRight");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<>(0, c_nNumElements);
							auto Right = f_CreateSet<>(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = Left & fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("Copy");
						NTest::CTestPerformanceMeasure CopyMeasure("Copy");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto LeftCopy = f_CreateSet<>(0, c_nNumElements);
							auto RightCopy = f_CreateSet<>(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(CopyMeasure, c_nNumElements * 2);
									Result = LeftCopy & RightCopy;
									LeftCopy.f_Clear();
									RightCopy.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_AddReference(CopyMeasure);
					}
					();

					auto fRunMapTests = [&]<EMapOperationPolicy t_Policy>
						{
							CStr PolicySuffix;
							switch (t_Policy)
							{
							case EMapOperationPolicy::mc_Fastest: PolicySuffix = "Fastest"; break;
							case EMapOperationPolicy::mc_PreferLeft: PolicySuffix = "PLeft"; break;
							case EMapOperationPolicy::mc_PreferRight: PolicySuffix = "PRight"; break;
							}

							[&] inline_never
							{
								DMibTestPath("MapMove" + PolicySuffix);
								NTest::CTestPerformanceMeasure MoveMeasure("MapMove" + PolicySuffix);
								for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
								{
									fp_GarbageCollect();
									auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
									auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
									TCMap<CTestVector, CMapSet> Result;
									[&]() inline_never
										{
											DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
											Result = fg_Move(Left).f_And<t_Policy>(fg_Move(Right));
											Left.f_Clear();
											Right.f_Clear();
										}
										()
									;
									DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
								}
								PerfTest.f_Add(MoveMeasure);
							}
							();

							[&] inline_never
							{
								DMibTestPath("MapMoveLeft" + PolicySuffix);
								NTest::CTestPerformanceMeasure MoveMeasure("MapMoveLeft" + PolicySuffix);
								for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
								{
									fp_GarbageCollect();
									auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
									auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
									TCMap<CTestVector, CMapSet> Result;
									[&]() inline_never
										{
											DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
											Result = fg_Move(Left).f_And<t_Policy>(Right);
											Left.f_Clear();
											Right.f_Clear();
										}
										()
									;
									DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
								}
								PerfTest.f_Add(MoveMeasure);
							}
							();

							[&] inline_never
							{
								DMibTestPath("MapMoveRight" + PolicySuffix);
								NTest::CTestPerformanceMeasure MoveMeasure("MapMoveRight" + PolicySuffix);
								for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
								{
									fp_GarbageCollect();
									auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
									auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
									TCMap<CTestVector, CMapSet> Result;
									[&]() inline_never
										{
											DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
											Result = Left.f_And<t_Policy>(fg_Move(Right));
											Left.f_Clear();
											Right.f_Clear();
										}
										()
									;
									DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
								}
								PerfTest.f_Add(MoveMeasure);
							}
							();

							[&] inline_never
							{
								DMibTestPath("MapCopy" + PolicySuffix);
								NTest::CTestPerformanceMeasure CopyMeasure("MapCopy" + PolicySuffix);
								for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
								{
									fp_GarbageCollect();
									auto LeftCopy = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
									auto RightCopy = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
									TCMap<CTestVector, CMapSet> Result;
									[&]() inline_never
										{
											DMibTestScopeMeasure(CopyMeasure, c_nNumElements * 2);
											Result = LeftCopy.f_And<t_Policy>(RightCopy);
											LeftCopy.f_Clear();
											RightCopy.f_Clear();
										}
										()
									;
									DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
								}
								PerfTest.f_AddReference(CopyMeasure);
							}
							();
						}
					;

					fRunMapTests.operator() <EMapOperationPolicy::mc_Fastest>();
					fRunMapTests.operator() <EMapOperationPolicy::mc_PreferLeft>();
					fRunMapTests.operator() <EMapOperationPolicy::mc_PreferRight>();

					DMibExpectTrue(PerfTest);
				};

				DMibTestCategory("Xor")
				{
					NTest::CTestPerformance PerfTest(AllowedPerformance);

					[&] inline_never
					{
						DMibTestPath("Move");
						NTest::CTestPerformanceMeasure MoveMeasure("Move");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<>(0, c_nNumElements);
							auto Right = f_CreateSet<>(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) ^ fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MoveLeft");
						NTest::CTestPerformanceMeasure MoveMeasure("MoveLeft");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<>(0, c_nNumElements);
							auto Right = f_CreateSet<>(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) ^ Right;
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MoveRight");
						NTest::CTestPerformanceMeasure MoveMeasure("MoveRight");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<>(0, c_nNumElements);
							auto Right = f_CreateSet<>(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = Left ^ fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("Copy");
						NTest::CTestPerformanceMeasure CopyMeasure("Copy");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto LeftCopy = f_CreateSet<>(0, c_nNumElements);
							auto RightCopy = f_CreateSet<>(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(CopyMeasure, c_nNumElements * 2);
									Result = LeftCopy ^ RightCopy;
									LeftCopy.f_Clear();
									RightCopy.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements)(ETestFlag_Aggregated);
						}
						PerfTest.f_AddReference(CopyMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MapMove");
						NTest::CTestPerformanceMeasure MoveMeasure("MapMove");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
							auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
							TCMap<CTestVector, CMapSet> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) ^ fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MapMoveLeft");
						NTest::CTestPerformanceMeasure MoveMeasure("MapMoveLeft");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
							auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
							TCMap<CTestVector, CMapSet> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) ^ Right;
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MapMoveRight");
						NTest::CTestPerformanceMeasure MoveMeasure("MapMoveRight");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
							auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
							TCMap<CTestVector, CMapSet> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = Left ^ fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MapCopy");
						NTest::CTestPerformanceMeasure CopyMeasure("MapCopy");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto LeftCopy = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
							auto RightCopy = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
							TCMap<CTestVector, CMapSet> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(CopyMeasure, c_nNumElements * 2);
									Result = LeftCopy ^ RightCopy;
									LeftCopy.f_Clear();
									RightCopy.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements)(ETestFlag_Aggregated);
						}
						PerfTest.f_AddReference(CopyMeasure);
					}
					();

					DMibExpectTrue(PerfTest);
				};

				DMibTestCategory("Difference")
				{
					NTest::CTestPerformance PerfTest(AllowedPerformance);

					[&] inline_never
					{
						DMibTestPath("Move");
						NTest::CTestPerformanceMeasure MoveMeasure("Move");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet(0, c_nNumElements);
							auto Right = f_CreateSet(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) - fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MoveLeft");
						NTest::CTestPerformanceMeasure MoveMeasure("MoveLeft");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet(0, c_nNumElements);
							auto Right = f_CreateSet(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) - Right;
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MoveRight");
						NTest::CTestPerformanceMeasure MoveMeasure("MoveRight");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet(0, c_nNumElements);
							auto Right = f_CreateSet(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = Left - fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("Copy");
						NTest::CTestPerformanceMeasure CopyMeasure("Copy");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto LeftCopy = f_CreateSet(0, c_nNumElements);
							auto RightCopy = f_CreateSet(c_nNumElements / 2, c_nNumElements);
							TCSet<CTestVector> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(CopyMeasure, c_nNumElements * 2);
									Result = LeftCopy - RightCopy;
									LeftCopy.f_Clear();
									RightCopy.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_AddReference(CopyMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MapMove");
						NTest::CTestPerformanceMeasure MoveMeasure("MapMove");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
							auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
							TCMap<CTestVector, CMapSet> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) - fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MapMoveLeft");
						NTest::CTestPerformanceMeasure MoveMeasure("MapMoveLeft");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
							auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
							TCMap<CTestVector, CMapSet> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = fg_Move(Left) - Right;
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MapMoveRight");
						NTest::CTestPerformanceMeasure MoveMeasure("MapMoveRight");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto Left = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
							auto Right = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
							TCMap<CTestVector, CMapSet> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(MoveMeasure, c_nNumElements * 2);
									Result = Left - fg_Move(Right);
									Left.f_Clear();
									Right.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_Add(MoveMeasure);
					}
					();

					[&] inline_never
					{
						DMibTestPath("MapCopy");
						NTest::CTestPerformanceMeasure CopyMeasure("MapCopy");
						for (mint iIter = 0; iIter < c_nNumIterations; ++iIter)
						{
							fp_GarbageCollect();
							auto LeftCopy = f_CreateSet<TCMap<CTestVector, CMapSet>>(0, c_nNumElements);
							auto RightCopy = f_CreateSet<TCMap<CTestVector, CMapSet>>(c_nNumElements / 2, c_nNumElements);
							TCMap<CTestVector, CMapSet> Result;
							[&]() inline_never
								{
									DMibTestScopeMeasure(CopyMeasure, c_nNumElements * 2);
									Result = LeftCopy - RightCopy;
									LeftCopy.f_Clear();
									RightCopy.f_Clear();
								}
								()
							;
							DMibExpect(Result.f_GetLen(), ==, c_nNumElements / 2)(ETestFlag_Aggregated);
						}
						PerfTest.f_AddReference(CopyMeasure);
					}
					();

					DMibExpectTrue(PerfTest);
				};
			};

		}
	};

	DMibTestRegister(CSetPerformance_Tests, Malterlib::Container);
}
