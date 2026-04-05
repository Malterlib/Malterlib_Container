// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Test/Exception>

namespace
{
	using namespace NMib;
	using namespace NMib::NStr;
	using namespace NMib::NContainer;
	using namespace NMib::NException;

	class CVectorFunctional_Tests : public NMib::NTest::CTest
	{
	public:
		struct CTrackedObject
		{
			CStr m_Value;
			bool m_bMovedFrom = false;
			mutable bool m_bCopiedFrom = false;

			CTrackedObject() = default;

			CTrackedObject(CStr const &_Value)
				: m_Value(_Value)
			{
			}

			CTrackedObject(CTrackedObject const &_Other)
				: m_Value(_Other.m_Value)
			{
				_Other.m_bCopiedFrom = true;
			}

			CTrackedObject(CTrackedObject &&_Other)
				: m_Value(fg_Move(_Other.m_Value))
			{
				_Other.m_bMovedFrom = true;
			}

			CTrackedObject &operator = (CTrackedObject const &_Other)
			{
				m_Value = _Other.m_Value;
				_Other.m_bCopiedFrom = true;
				return *this;
			}

			CTrackedObject &operator = (CTrackedObject &&_Other)
			{
				m_Value = fg_Move(_Other.m_Value);
				_Other.m_bMovedFrom = true;
				return *this;
			}

			bool operator == (CTrackedObject const &_Other) const noexcept
			{
				return m_Value == _Other.m_Value;
			}

			template <typename tf_CStr>
			void f_Format(tf_CStr &o_Str) const
			{
				o_Str += m_Value;
			}
		};

		void f_Map()
		{
			DMibTestSuite("f_Map")
			{
				DMibTestCategory("Basic")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					auto Doubled = Vector.f_Map([](int32 _Value) { return _Value * 2; });

					DMibExpect(Doubled.f_GetLen(), ==, 5);
					DMibExpect(Doubled, ==, TCVector<int32>({2, 4, 6, 8, 10}));
				};

				DMibTestCategory("Empty vector")
				{
					TCVector<int32> Vector;

					auto Mapped = Vector.f_Map([](int32 _Value) { return _Value * 2; });

					DMibExpectTrue(Mapped.f_IsEmpty());
				};

				DMibTestCategory("Type transformation")
				{
					TCVector<int32> Vector = {1, 2, 3};

					auto Strings = Vector.f_Map([](int32 _Value) -> CStr { return "{}"_f << _Value; });

					static_assert(NTraits::cIsSame<decltype(Strings)::CData, CStr>);
					DMibExpect(Strings, ==, TCVector<CStr>({"1", "2", "3"}));
				};

				DMibTestCategory("With index")
				{
					TCVector<CStr> Vector = {"a", "b", "c"};

					auto WithIndex = Vector.f_Map([](CStr const &_Value, umint _Index) -> CStr { return "{}:{}"_f << _Index << _Value; });

					DMibExpect(WithIndex, ==, TCVector<CStr>({"0:a", "1:b", "2:c"}));
				};

				DMibTestCategory("With index and array")
				{
					TCVector<int32> Vector = {10, 20, 30};

					auto Result = Vector.f_Map
						(
							[](int32 _Value, umint _Index, TCVector<int32> const &_Array) -> int32
							{
								return _Value + (int32)_Array.f_GetLen() + (int32)_Index;
							}
						)
					;

					// 10+3+0=13, 20+3+1=24, 30+3+2=35
					DMibExpect(Result, ==, TCVector<int32>({13, 24, 35}));
				};

				DMibTestCategory("Move semantics")
				{
					TCVector<CTrackedObject> Vector;
					Vector.f_Insert(CTrackedObject("a"));
					Vector.f_Insert(CTrackedObject("b"));
					Vector.f_Insert(CTrackedObject("c"));

					auto Mapped = fg_Move(Vector).f_Map([](CTrackedObject &&_Value) { return fg_Move(_Value); });

					DMibExpect(Mapped.f_GetLen(), ==, 3);
					DMibExpectTrue(Vector[0].m_bMovedFrom);
					DMibExpectTrue(Vector[1].m_bMovedFrom);
					DMibExpectTrue(Vector[2].m_bMovedFrom);
				};

				DMibTestCategory("Const vector")
				{
					TCVector<int32> const Vector = {1, 2, 3};

					auto Doubled = Vector.f_Map([](int32 _Value) { return _Value * 2; });

					DMibExpect(Doubled, ==, TCVector<int32>({2, 4, 6}));
				};
			};
		}

		void f_Filter()
		{
			DMibTestSuite("f_Filter")
			{
				DMibTestCategory("Basic")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

					auto Evens = Vector.f_Filter([](int32 _Value) { return _Value % 2 == 0; });

					DMibExpect(Evens, ==, TCVector<int32>({2, 4, 6, 8, 10}));
				};

				DMibTestCategory("Empty vector")
				{
					TCVector<int32> Vector;

					auto Filtered = Vector.f_Filter([](int32 _Value) { return _Value > 0; });

					DMibExpectTrue(Filtered.f_IsEmpty());
				};

				DMibTestCategory("No matches")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					auto Filtered = Vector.f_Filter([](int32 _Value) { return _Value > 10; });

					DMibExpectTrue(Filtered.f_IsEmpty());
				};

				DMibTestCategory("All match")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					auto Filtered = Vector.f_Filter([](int32 _Value) { return _Value > 0; });

					DMibExpect(Filtered, ==, Vector);
				};

				DMibTestCategory("With index")
				{
					TCVector<CStr> Vector = {"a", "b", "c", "d", "e"};

					auto EvenIndices = Vector.f_Filter([](CStr const &, umint _Index) { return _Index % 2 == 0; });

					DMibExpect(EvenIndices, ==, TCVector<CStr>({"a", "c", "e"}));
				};

				DMibTestCategory("With index and array")
				{
					TCVector<int32> Vector = {10, 20, 30, 40, 50};

					auto Result = Vector.f_Filter
						(
							[](int32 _Value, umint _Index, TCVector<int32> const &_Array)
							{
								return _Value > (int32)_Array.f_GetLen() * 5 && _Index > 1;
							}
						)
					;

					// Length is 5, threshold is 25. Values > 25 with index > 1: 30@2, 40@3, 50@4
					DMibExpect(Result, ==, TCVector<int32>({30, 40, 50}));
				};

				DMibTestCategory("Move semantics")
				{
					TCVector<CTrackedObject> Vector;
					Vector.f_Insert(CTrackedObject("a"));
					Vector.f_Insert(CTrackedObject("b"));
					Vector.f_Insert(CTrackedObject("c"));

					auto Filtered = fg_Move(Vector).f_Filter([](CTrackedObject const &_Value) { return _Value.m_Value != "b"; });

					DMibExpect(Filtered.f_GetLen(), ==, 2);
					DMibExpectTrue(Vector[0].m_bMovedFrom);
					DMibExpectFalse(Vector[1].m_bMovedFrom); // "b" was not moved since it didn't match
					DMibExpectTrue(Vector[2].m_bMovedFrom);
				};

				DMibTestCategory("Const vector")
				{
					TCVector<int32> const Vector = {1, 2, 3, 4, 5};

					auto Odds = Vector.f_Filter([](int32 _Value) { return _Value % 2 == 1; });

					DMibExpect(Odds, ==, TCVector<int32>({1, 3, 5}));
				};
			};
		}

		void f_Every()
		{
			DMibTestSuite("f_Every")
			{
				DMibTestCategory("All match")
				{
					TCVector<int32> Vector = {2, 4, 6, 8, 10};

					bool bAllEven = Vector.f_Every([](int32 _Value) { return _Value % 2 == 0; });

					DMibExpectTrue(bAllEven);
				};

				DMibTestCategory("Some don't match")
				{
					TCVector<int32> Vector = {2, 4, 5, 8, 10};

					bool bAllEven = Vector.f_Every([](int32 _Value) { return _Value % 2 == 0; });

					DMibExpectFalse(bAllEven);
				};

				DMibTestCategory("Empty vector")
				{
					TCVector<int32> Vector;

					bool bResult = Vector.f_Every([](int32 _Value) { return _Value > 0; });

					// Empty vector should return true (vacuous truth)
					DMibExpectTrue(bResult);
				};

				DMibTestCategory("With index")
				{
					TCVector<int32> Vector = {0, 1, 2, 3, 4};

					bool bIndexMatches = Vector.f_Every([](int32 _Value, umint _Index) { return _Value == (int32)_Index; });

					DMibExpectTrue(bIndexMatches);
				};

				DMibTestCategory("With index and array")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					bool bResult = Vector.f_Every
						(
							[](int32 _Value, umint _Index, TCVector<int32> const &_Array)
							{
								return _Value <= (int32)_Array.f_GetLen();
							}
						)
					;

					DMibExpectTrue(bResult);
				};

				DMibTestCategory("Short circuit")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};
					umint nCallCount = 0;

					bool bResult = Vector.f_Every
						(
							[&nCallCount](int32 _Value)
							{
								++nCallCount;
								return _Value < 3;
							}
						)
					;

					DMibExpectFalse(bResult);
					DMibExpect(nCallCount, ==, 3); // Should stop after finding 3
				};
			};
		}

		void f_Some()
		{
			DMibTestSuite("f_Some")
			{
				DMibTestCategory("Some match")
				{
					TCVector<int32> Vector = {1, 3, 5, 6, 7};

					bool bHasEven = Vector.f_Some([](int32 _Value) { return _Value % 2 == 0; });

					DMibExpectTrue(bHasEven);
				};

				DMibTestCategory("None match")
				{
					TCVector<int32> Vector = {1, 3, 5, 7, 9};

					bool bHasEven = Vector.f_Some([](int32 _Value) { return _Value % 2 == 0; });

					DMibExpectFalse(bHasEven);
				};

				DMibTestCategory("Empty vector")
				{
					TCVector<int32> Vector;

					bool bResult = Vector.f_Some([](int32 _Value) { return _Value > 0; });

					// Empty vector returns false (no element can satisfy the predicate)
					DMibExpectFalse(bResult);
				};

				DMibTestCategory("With index")
				{
					TCVector<int32> Vector = {0, 10, 20, 30, 40};

					bool bHasMatchingIndex = Vector.f_Some([](int32 _Value, umint _Index) { return _Value == (int32)_Index * 10; });

					DMibExpectTrue(bHasMatchingIndex);
				};

				DMibTestCategory("With index and array")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					bool bResult = Vector.f_Some
						(
							[](int32 _Value, umint _Index, TCVector<int32> const &_Array)
							{
								return _Value == (int32)_Array.f_GetLen();
							}
						)
					;

					DMibExpectTrue(bResult); // 5 == Length(5)
				};

				DMibTestCategory("Short circuit")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};
					umint nCallCount = 0;

					bool bResult = Vector.f_Some
						(
							[&nCallCount](int32 _Value)
							{
								++nCallCount;
								return _Value == 2;
							}
						)
					;

					DMibExpectTrue(bResult);
					DMibExpect(nCallCount, ==, 2); // Should stop after finding 2
				};
			};
		}

		void f_Reduce()
		{
			DMibTestSuite("f_Reduce")
			{
				DMibTestCategory("Sum with initial value")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					int32 Sum = Vector.f_Reduce([](int32 &&_Acc, int32 _Value) { return _Acc + _Value; }, 0);

					DMibExpect(Sum, ==, 15);
				};

				DMibTestCategory("Sum with non-zero initial value")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					int32 Sum = Vector.f_Reduce([](int32 &&_Acc, int32 _Value) { return _Acc + _Value; }, 100);

					DMibExpect(Sum, ==, 115);
				};

				DMibTestCategory("Product")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					int32 Product = Vector.f_Reduce([](int32 &&_Acc, int32 _Value) { return _Acc * _Value; }, 1);

					DMibExpect(Product, ==, 120);
				};

				DMibTestCategory("Type transformation")
				{
					TCVector<int32> Vector = {1, 2, 3};

					CStr Joined = Vector.f_Reduce
						(
							[](CStr &&_Acc, int32 _Value) -> CStr
							{
								if (_Acc.f_IsEmpty())
									return "{}"_f << _Value;
								return "{},{}"_f << _Acc << _Value;
							}
							, CStr()
						)
					;

					DMibExpect(Joined, ==, CStr("1,2,3"));
				};

				DMibTestCategory("Empty vector with initial value")
				{
					TCVector<int32> Vector;

					int32 Result = Vector.f_Reduce([](int32 &&_Acc, int32 _Value) { return _Acc + _Value; }, 42);

					DMibExpect(Result, ==, 42);
				};

				DMibTestCategory("Without initial value")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					int32 Sum = Vector.f_Reduce([](int32 &&_Acc, int32 _Value) { return _Acc + _Value; });

					DMibExpect(Sum, ==, 15);
				};

				DMibTestCategory("With index")
				{
					TCVector<int32> Vector = {10, 20, 30};

					int32 Sum = Vector.f_Reduce
						(
							[](int32 &&_Acc, int32 _Value, umint _Index)
							{
								return _Acc + _Value + (int32)_Index;
							}
							, 0
						)
					;

					// 0 + (10+0) + (20+1) + (30+2) = 63
					DMibExpect(Sum, ==, 63);
				};

				DMibTestCategory("With index and array")
				{
					TCVector<int32> Vector = {1, 2, 3};

					int32 Sum = Vector.f_Reduce
						(
							[](int32 &&_Acc, int32 _Value, umint _Index, TCVector<int32> const &_Array)
							{
								return _Acc + _Value * (int32)_Array.f_GetLen();
							}
							, 0
						)
					;

					// 0 + 1*3 + 2*3 + 3*3 = 18
					DMibExpect(Sum, ==, 18);
				};

				DMibTestCategory("Reference mode (void return)")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};
					int32 Accumulator = 0;

					Vector.f_Reduce
						(
							[](int32 &_Acc, int32 _Value)
							{
								_Acc += _Value;
							}
							, Accumulator
						)
					;

					DMibExpect(Accumulator, ==, 15);
				};

				DMibTestCategory("Rvalue reference does not move")
				{
					// When taking by rvalue reference, the original elements should NOT be moved
					// because we're just binding a reference, not invoking the move constructor
					TCVector<CTrackedObject> Vector;
					Vector.f_Insert(CTrackedObject("a"));
					Vector.f_Insert(CTrackedObject("b"));
					Vector.f_Insert(CTrackedObject("c"));

					CStr Result = fg_Move(Vector).f_Reduce
						(
							[](CStr &&_Acc, CTrackedObject &&_Value) -> CStr
							{
								if (_Acc.f_IsEmpty())
									return _Value.m_Value;
								return "{},{}"_f << _Acc << _Value.m_Value;
							}
							, CStr()
						)
					;

					DMibExpect(Result, ==, CStr("a,b,c"));
					DMibExpectFalse(Vector[0].m_bMovedFrom);
					DMibExpectFalse(Vector[1].m_bMovedFrom);
					DMibExpectFalse(Vector[2].m_bMovedFrom);
				};

				DMibTestCategory("Move into accumulated vector")
				{
					// When accumulating into a vector, elements should be moved
					TCVector<CTrackedObject> Vector;
					Vector.f_Insert(CTrackedObject("a"));
					Vector.f_Insert(CTrackedObject("b"));
					Vector.f_Insert(CTrackedObject("c"));

					TCVector<CTrackedObject> Result = fg_Move(Vector).f_Reduce
						(
							[](TCVector<CTrackedObject> &&_Acc, CTrackedObject &&_Value) -> TCVector<CTrackedObject>
							{
								_Acc.f_Insert(fg_Move(_Value));
								return fg_Move(_Acc);
							}
							, TCVector<CTrackedObject>()
						)
					;

					DMibExpect(Result.f_GetLen(), ==, 3);
					DMibExpect(Result[0].m_Value, ==, CStr("a"));
					DMibExpect(Result[1].m_Value, ==, CStr("b"));
					DMibExpect(Result[2].m_Value, ==, CStr("c"));
					DMibExpectTrue(Vector[0].m_bMovedFrom);
					DMibExpectTrue(Vector[1].m_bMovedFrom);
					DMibExpectTrue(Vector[2].m_bMovedFrom);
				};
			};
		}

		void f_ReduceRight()
		{
			DMibTestSuite("f_ReduceRight")
			{
				DMibTestCategory("Basic - reverse order")
				{
					TCVector<int32> Vector = {1, 2, 3};

					CStr Result = Vector.f_ReduceRight
						(
							[](CStr &&_Acc, int32 _Value) -> CStr
							{
								if (_Acc.f_IsEmpty())
									return "{}"_f << _Value;
								return "{},{}"_f << _Acc << _Value;
							}
							, CStr()
						)
					;

					DMibExpect(Result, ==, CStr("3,2,1"));
				};

				DMibTestCategory("Subtraction order matters")
				{
					TCVector<int32> Vector = {1, 2, 3, 4};

					// Reduce: ((((0-1)-2)-3)-4) = -10
					int32 ReduceResult = Vector.f_Reduce([](int32 &&_Acc, int32 _Value) { return _Acc - _Value; }, 0);
					DMibExpect(ReduceResult, ==, -10);

					// ReduceRight: ((((0-4)-3)-2)-1) = -10 (same for subtraction with 0 initial)
					int32 ReduceRightResult = Vector.f_ReduceRight([](int32 &&_Acc, int32 _Value) { return _Acc - _Value; }, 0);
					DMibExpect(ReduceRightResult, ==, -10);
				};

				DMibTestCategory("Empty vector with initial value")
				{
					TCVector<int32> Vector;

					int32 Result = Vector.f_ReduceRight([](int32 &&_Acc, int32 _Value) { return _Acc + _Value; }, 42);

					DMibExpect(Result, ==, 42);
				};

				DMibTestCategory("Without initial value")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					int32 Sum = Vector.f_ReduceRight([](int32 &&_Acc, int32 _Value) { return _Acc + _Value; });

					DMibExpect(Sum, ==, 15);
				};

				DMibTestCategory("With index")
				{
					TCVector<CStr> Vector = {"a", "b", "c"};

					CStr Result = Vector.f_ReduceRight
						(
							[](CStr &&_Acc, CStr const &_Value, umint _Index) -> CStr
							{
								if (_Acc.f_IsEmpty())
									return "{}:{}"_f << _Index << _Value;
								return "{},{}:{}"_f << _Acc << _Index << _Value;
							}
							, CStr()
						)
					;

					// Indices: c@2, b@1, a@0
					DMibExpect(Result, ==, CStr("2:c,1:b,0:a"));
				};

				DMibTestCategory("With index and array")
				{
					TCVector<int32> Vector = {1, 2, 3};

					int32 Sum = Vector.f_ReduceRight
						(
							[](int32 &&_Acc, int32 _Value, umint _Index, TCVector<int32> const &_Array)
							{
								return _Acc + _Value * (int32)_Array.f_GetLen() + (int32)_Index;
							}
							, 0
						)
					;

					// (0 + 3*3 + 2) + (2*3 + 1) + (1*3 + 0) = 11 + 7 + 3 = 21
					DMibExpect(Sum, ==, 21);
				};

				DMibTestCategory("Reference mode (void return)")
				{
					TCVector<CStr> Vector = {"a", "b", "c"};
					CStr Accumulator;

					Vector.f_ReduceRight
						(
							[](CStr &_Acc, CStr const &_Value)
							{
								_Acc += _Value;
							}
							, Accumulator
						)
					;

					DMibExpect(Accumulator, ==, CStr("cba"));
				};

				DMibTestCategory("Rvalue reference does not move")
				{
					// When taking by rvalue reference, the original elements should NOT be moved
					TCVector<CTrackedObject> Vector;
					Vector.f_Insert(CTrackedObject("a"));
					Vector.f_Insert(CTrackedObject("b"));
					Vector.f_Insert(CTrackedObject("c"));

					CStr Result = fg_Move(Vector).f_ReduceRight
						(
							[](CStr &&_Acc, CTrackedObject &&_Value) -> CStr
							{
								if (_Acc.f_IsEmpty())
									return _Value.m_Value;
								return "{},{}"_f << _Acc << _Value.m_Value;
							}
							, CStr()
						)
					;

					DMibExpect(Result, ==, CStr("c,b,a"));
					DMibExpectFalse(Vector[0].m_bMovedFrom);
					DMibExpectFalse(Vector[1].m_bMovedFrom);
					DMibExpectFalse(Vector[2].m_bMovedFrom);
				};

				DMibTestCategory("Move into accumulated vector")
				{
					// When accumulating into a vector, elements should be moved
					TCVector<CTrackedObject> Vector;
					Vector.f_Insert(CTrackedObject("a"));
					Vector.f_Insert(CTrackedObject("b"));
					Vector.f_Insert(CTrackedObject("c"));

					TCVector<CTrackedObject> Result = fg_Move(Vector).f_ReduceRight
						(
							[](TCVector<CTrackedObject> &&_Acc, CTrackedObject &&_Value) -> TCVector<CTrackedObject>
							{
								_Acc.f_Insert(fg_Move(_Value));
								return fg_Move(_Acc);
							}
							, TCVector<CTrackedObject>()
						)
					;

					DMibExpect(Result.f_GetLen(), ==, 3);
					// ReduceRight processes c, b, a in that order
					DMibExpect(Result[0].m_Value, ==, CStr("c"));
					DMibExpect(Result[1].m_Value, ==, CStr("b"));
					DMibExpect(Result[2].m_Value, ==, CStr("a"));
					DMibExpectTrue(Vector[0].m_bMovedFrom);
					DMibExpectTrue(Vector[1].m_bMovedFrom);
					DMibExpectTrue(Vector[2].m_bMovedFrom);
				};
			};
		}

		void f_Chaining()
		{
			DMibTestSuite("Chaining operations")
			{
				DMibTestCategory("Filter then Map")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

					auto Result = Vector
						.f_Filter([](int32 _Value) { return _Value % 2 == 0; })
						.f_Map([](int32 _Value) { return _Value * _Value; })
					;

					DMibExpect(Result, ==, TCVector<int32>({4, 16, 36, 64, 100}));
				};

				DMibTestCategory("Map then Filter then Reduce")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5};

					int32 Result = Vector
						.f_Map([](int32 _Value) { return _Value * 2; })
						.f_Filter([](int32 _Value) { return _Value > 4; })
						.f_Reduce([](int32 &&_Acc, int32 _Value) { return _Acc + _Value; }, 0)
					;

					// Map: {2, 4, 6, 8, 10}
					// Filter: {6, 8, 10}
					// Reduce: 24
					DMibExpect(Result, ==, 24);
				};

				DMibTestCategory("Filter then Every")
				{
					TCVector<int32> Vector = {1, 2, 3, 4, 5, 6};

					bool bResult = Vector
						.f_Filter([](int32 _Value) { return _Value > 3; })
						.f_Every([](int32 _Value) { return _Value > 3; })
					;

					DMibExpectTrue(bResult);
				};

				DMibTestCategory("Map then Some")
				{
					TCVector<int32> Vector = {1, 2, 3};

					bool bResult = Vector
						.f_Map([](int32 _Value) { return _Value * 10; })
						.f_Some([](int32 _Value) { return _Value == 20; })
					;

					DMibExpectTrue(bResult);
				};
			};
		}

		void f_ConstVectors()
		{
			DMibTestSuite("Const vectors")
			{
				DMibTestCategory("Map on const vector")
				{
					TCVector<int32> const Vector = {1, 2, 3, 4, 5};

					auto Result = Vector.f_Map([](int32 _Value) { return _Value * 2; });

					DMibExpect(Result, ==, TCVector<int32>({2, 4, 6, 8, 10}));
				};

				DMibTestCategory("Filter on const vector")
				{
					TCVector<int32> const Vector = {1, 2, 3, 4, 5, 6};

					auto Result = Vector.f_Filter([](int32 _Value) { return _Value % 2 == 0; });

					DMibExpect(Result, ==, TCVector<int32>({2, 4, 6}));
				};

				DMibTestCategory("Every on const vector")
				{
					TCVector<int32> const Vector = {2, 4, 6, 8};

					bool bResult = Vector.f_Every([](int32 _Value) { return _Value % 2 == 0; });

					DMibExpectTrue(bResult);
				};

				DMibTestCategory("Some on const vector")
				{
					TCVector<int32> const Vector = {1, 3, 5, 6, 7};

					bool bResult = Vector.f_Some([](int32 _Value) { return _Value % 2 == 0; });

					DMibExpectTrue(bResult);
				};

				DMibTestCategory("Reduce on const vector")
				{
					TCVector<int32> const Vector = {1, 2, 3, 4, 5};

					int32 Sum = Vector.f_Reduce([](int32 &&_Acc, int32 _Value) { return _Acc + _Value; }, 0);

					DMibExpect(Sum, ==, 15);
				};

				DMibTestCategory("ReduceRight on const vector")
				{
					TCVector<CStr> const Vector = {"a", "b", "c"};

					CStr Result = Vector.f_ReduceRight
						(
							[](CStr &&_Acc, CStr const &_Value) -> CStr
							{
								if (_Acc.f_IsEmpty())
									return _Value;
								return "{},{}"_f << _Acc << _Value;
							}
							, CStr()
						)
					;

					DMibExpect(Result, ==, CStr("c,b,a"));
				};

				DMibTestCategory("Chaining on const vector")
				{
					TCVector<int32> const Vector = {1, 2, 3, 4, 5};

					int32 Result = Vector
						.f_Filter([](int32 _Value) { return _Value > 2; })
						.f_Map([](int32 _Value) { return _Value * 10; })
						.f_Reduce([](int32 &&_Acc, int32 _Value) { return _Acc + _Value; }, 0)
					;

					// Filter: {3, 4, 5}
					// Map: {30, 40, 50}
					// Reduce: 120
					DMibExpect(Result, ==, 120);
				};

				DMibTestCategory("Const vector does not move elements")
				{
					TCVector<CTrackedObject> Vector;
					Vector.f_Insert(CTrackedObject("a"));
					Vector.f_Insert(CTrackedObject("b"));
					Vector.f_Insert(CTrackedObject("c"));

					TCVector<CTrackedObject> const &ConstVector = Vector;

					// Use copy constructor to verify const ref is passed
					auto Result = ConstVector.f_Map([](CTrackedObject const &_Value) { return CTrackedObject(_Value); });

					DMibExpect(Result.f_GetLen(), ==, 3);
					// Elements should be copied, not moved
					DMibExpectFalse(Vector[0].m_bMovedFrom);
					DMibExpectFalse(Vector[1].m_bMovedFrom);
					DMibExpectFalse(Vector[2].m_bMovedFrom);
					DMibExpectTrue(Vector[0].m_bCopiedFrom);
					DMibExpectTrue(Vector[1].m_bCopiedFrom);
					DMibExpectTrue(Vector[2].m_bCopiedFrom);
				};
			};
		}

		void f_DoTests()
		{
			DMibTestCategory("Map")
			{
				this->f_Map();
			};
			DMibTestCategory("Filter")
			{
				this->f_Filter();
			};
			DMibTestCategory("Every")
			{
				this->f_Every();
			};
			DMibTestCategory("Some")
			{
				this->f_Some();
			};
			DMibTestCategory("Reduce")
			{
				this->f_Reduce();
			};
			DMibTestCategory("ReduceRight")
			{
				this->f_ReduceRight();
			};
			DMibTestCategory("Chaining")
			{
				this->f_Chaining();
			};
			DMibTestCategory("ConstVectors")
			{
				this->f_ConstVectors();
			};
		}
	};

	DMibTestRegister(CVectorFunctional_Tests, Malterlib::Container::Vector::Functional);
}
