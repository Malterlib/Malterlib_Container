// Copyright © 2025 Unborken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Memory/MemoryManager>

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
			};
		}
	};

	DMibTestRegister(CSet_Tests, Malterlib::Container);
}
