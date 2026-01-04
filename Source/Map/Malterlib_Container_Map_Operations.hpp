// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Xor(this t_CThis &&_This, TCMap const &_Right) -> TCMap
	{
		if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
		{
			if (_Right.f_IsEmpty())
				return fg_Move(_This);
			else if (_This.f_IsEmpty())
				return _Right;

			TCMap Return;

			auto iLeft = fg_Move(_This).f_Entries().f_GetIteratorBidirectionalDestructive();
			auto iRight = _Right.f_Entries().f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
				if (Cmp < 0)
					Return.f_Insert(iLeft.f_ExtractNode());
				else if (Cmp > 0)
				{
					Return[iRight->f_Key(), iRight->f_Value()];
					++iRight;
				}
				else
				{
					++iLeft;
					++iRight;
				}
			}

			for (; iLeft;)
				Return.f_Insert(iLeft.f_ExtractNode());

			for (; iRight; ++iRight)
				Return[iRight->f_Key(), iRight->f_Value()];

			return Return;
		}
		else
		{
			if (_Right.f_IsEmpty())
				return _This;
			else if (_This.f_IsEmpty())
				return _Right;

			TCMap Return;

			auto iLeft = _This.f_Entries().f_GetIterator();
			auto iRight = _Right.f_Entries().f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
				if (Cmp < 0)
				{
					Return[iLeft->f_Key(), iLeft->f_Value()];
					++iLeft;
				}
				else if (Cmp > 0)
				{
					Return[iRight->f_Key(), iRight->f_Value()];
					++iRight;
				}
				else
				{
					++iLeft;
					++iRight;
				}
			}

			for (; iLeft; ++iLeft)
				Return[iLeft->f_Key(), iLeft->f_Value()];

			for (; iRight; ++iRight)
				Return[iRight->f_Key(), iRight->f_Value()];

			return Return;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Xor(this t_CThis &&_This, TCMap &&_Right) -> TCMap
	{
		if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
		{
			if (_Right.f_IsEmpty())
				return fg_Move(_This);
			else if (_This.f_IsEmpty())
				return fg_Move(_Right);

			TCMap Return(fg_Move(_This));

			_Right.f_ExtractAll
				(
					[&](CNodeHandle &&_Handle)
					{
						if (Return.f_Remove(_Handle.f_Key()))
							return;

						Return.f_Insert(fg_Move(_Handle));
					}
				)
			;

			return Return;
		}
		else
		{
			if (_Right.f_IsEmpty())
				return _This;
			else if (_This.f_IsEmpty())
				return fg_Move(_Right);

			TCMap Return;

			auto iLeft = _This.f_Entries().f_GetIterator();
			auto iRight = fg_Move(_Right).f_Entries().f_GetIteratorBidirectionalDestructive();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
				if (Cmp < 0)
				{
					Return[iLeft->f_Key(), iLeft->f_Value()];
					++iLeft;
				}
				else if (Cmp > 0)
					Return.f_Insert(iRight.f_ExtractNode());
				else
				{
					++iLeft;
					++iRight;
				}
			}

			for (; iLeft; ++iLeft)
				Return[iLeft->f_Key(), iLeft->f_Value()];

			for (; iRight;)
				Return.f_Insert(iRight.f_ExtractNode());

			return Return;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator ^ (this t_CThis &&_This, TCMap const &_Right) -> TCMap
	{
		return fg_Forward<t_CThis>(_This).f_Xor(_Right);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator ^ (this t_CThis &&_This, TCMap &&_Right) -> TCMap
	{
		return fg_Forward<t_CThis>(_This).f_Xor(fg_Move(_Right));
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Difference(this t_CThis &&_This, TCMap const &_Right) -> TCMap
	{
		if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
		{
			TCMap Return = fg_Move(_This);
			Return -= _Right;
			return Return;
		}
		else
		{
			TCMap Return;
			auto iLeft = _This.f_Entries().f_GetIterator();
			auto iRight = _Right.f_Entries().f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
				if (Cmp < 0)
				{
					Return[iLeft->f_Key(), iLeft->f_Value()];
					++iLeft;
				}
				else if (Cmp > 0)
					++iRight;
				else
				{
					++iLeft;
					++iRight;
				}
			}

			for (; iLeft; ++iLeft)
				Return[iLeft->f_Key(), iLeft->f_Value()];

			return Return;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator - (this t_CThis &&_This, TCMap const &_Right) -> TCMap
	{
		return fg_Forward<t_CThis>(_This).f_Difference(_Right);
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <EMapOperationPolicy t_Policy, typename t_CThis>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Or(this t_CThis &&_This, TCMap const &_Right) -> TCMap
	{
		if constexpr (t_Policy == EMapOperationPolicy::mc_Fastest)
		{
			if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
			{
				TCMap Return(_Right);
				Return += fg_Move(_This);
				return Return;
			}
			else
			{
				TCMap Return(_This);
				Return += _Right;
				return Return;
			}
		}
		else if constexpr (t_Policy == EMapOperationPolicy::mc_PreferLeft)
		{
			if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
			{
				TCMap Return(fg_Move(_This));
				Return += _Right;
				return Return;
			}
			else
			{
				TCMap Return(_This);
				Return += _Right;
				return Return;
			}
		}
		else
		{
			TCMap Return(_Right);
			if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
				Return += fg_Move(_This);
			else
				Return += _This;
			return Return;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <EMapOperationPolicy t_Policy, typename t_CThis>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Or(this t_CThis &&_This, TCMap &&_Right) -> TCMap
	{
		if constexpr (t_Policy == EMapOperationPolicy::mc_Fastest)
		{
			if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
			{
				TCMap Return(fg_Move(_Right));
				Return += fg_Move(_This);
				return Return;
			}
			else
			{
				TCMap Return(_This);
				Return += fg_Move(_Right);
				return Return;
			}
		}
		else if constexpr (t_Policy == EMapOperationPolicy::mc_PreferLeft)
		{
			if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
			{
				TCMap Return(fg_Move(_This));
				Return += fg_Move(_Right);
				return Return;
			}
			else
			{
				TCMap Return(_This);
				Return += fg_Move(_Right);
				return Return;
			}
		}
		else
		{
			TCMap Return(fg_Move(_Right));
			if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
				Return += fg_Move(_This);
			else
				Return += _This;
			return Return;
		}
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <EMapOperationPolicy t_Policy, typename t_CThis>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_And(this t_CThis &&_This, TCMap const &_Right) -> TCMap
	{
		TCMap Return;

		constexpr static bool c_bPreferLeft = (t_Policy == EMapOperationPolicy::mc_PreferLeft) || (t_Policy == EMapOperationPolicy::mc_Fastest);

		if constexpr (c_bPreferLeft && NTraits::cIsRValueReference<t_CThis &&>)
		{
			auto iLeft = fg_Move(_This).f_Entries().f_GetIteratorBidirectionalDestructive();
			auto iRight = _Right.f_Entries().f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
				if (Cmp < 0)
					++iLeft;
				else if (Cmp > 0)
					++iRight;
				else
				{
					Return.f_Insert(iLeft.f_ExtractNode());
					++iRight;
				}
			}
		}
		else if constexpr (c_bPreferLeft)
		{
			auto iLeft = _This.f_Entries().f_GetIterator();
			auto iRight = _Right.f_Entries().f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
				if (Cmp < 0)
					++iLeft;
				else if (Cmp > 0)
					++iRight;
				else
				{
					Return[iLeft->f_Key(), iLeft->f_Value()];
					++iLeft;
					++iRight;
				}
			}
		}
		else
		{
			auto iLeft = _This.f_Entries().f_GetIterator();
			auto iRight = _Right.f_Entries().f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
				if (Cmp < 0)
					++iLeft;
				else if (Cmp > 0)
					++iRight;
				else
				{
					Return[iRight->f_Key(), iRight->f_Value()];
					++iLeft;
					++iRight;
				}
			}
		}

		return Return;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <EMapOperationPolicy t_Policy, typename t_CThis>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_And(this t_CThis &&_This, TCMap &&_Right) -> TCMap
	{
		TCMap Return;

		if constexpr (t_Policy == EMapOperationPolicy::mc_PreferLeft)
		{
			if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
			{
				auto iLeft = fg_Move(_This).f_Entries().f_GetIteratorBidirectionalDestructive();
				auto iRight = _Right.f_Entries().f_GetIterator();

				while (iLeft && iRight)
				{
					auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
					if (Cmp < 0)
						++iLeft;
					else if (Cmp > 0)
						++iRight;
					else
					{
						Return.f_Insert(iLeft.f_ExtractNode());
						++iRight;
					}
				}
			}
			else
			{
				auto iLeft = _This.f_Entries().f_GetIterator();
				auto iRight = _Right.f_Entries().f_GetIterator();

				while (iLeft && iRight)
				{
					auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
					if (Cmp < 0)
						++iLeft;
					else if (Cmp > 0)
						++iRight;
					else
					{
						Return[iLeft->f_Key(), iLeft->f_Value()];
						++iLeft;
						++iRight;
					}
				}
			}
		}
		else if constexpr (t_Policy == EMapOperationPolicy::mc_PreferRight)
		{
			auto iLeft = _This.f_Entries().f_GetIterator();
			auto iRight = fg_Move(_Right).f_Entries().f_GetIteratorBidirectionalDestructive();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
				if (Cmp < 0)
					++iLeft;
				else if (Cmp > 0)
					++iRight;
				else
				{
					Return.f_Insert(iRight.f_ExtractNode());
					++iLeft;
				}
			}
		}
		else
		{
			auto iLeft = _This.f_Entries().f_GetIterator();
			auto iRight = fg_Move(_Right).f_Entries().f_GetIteratorBidirectionalDestructive();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(iLeft->f_Key(), iRight->f_Key());
				if (Cmp < 0)
					++iLeft;
				else if (Cmp > 0)
					++iRight;
				else
				{
					Return.f_Insert(iRight.f_ExtractNode());
					++iLeft;
				}
			}
		}

		return Return;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator += (TCMap const &_Other) -> TCMap &
	{
		DMibSafeCheck(this != &_Other, "Must not add same object to itself");
		fp_CopyAddAll(_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator += (TCMap &&_Other) -> TCMap &
	{
		DMibSafeCheck(this != &_Other, "Must not add same object to itself");
		fp_MoveAddAll(_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator -= (TCMap const &_Other) -> TCMap &
	{
		DMibSafeCheck(this != &_Other, "Must not remove same object from itself");
		CIteratorConst Iter = _Other.f_GetIterator();
		while (Iter)
		{
			f_Remove(Iter.f_GetKey());
			++Iter;
		}

		return *this;
	}
}
