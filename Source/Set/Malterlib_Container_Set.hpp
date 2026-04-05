// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	TCSet<t_CKey, t_CCompare, t_CAllocator>::TCSet()
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	TCSet<t_CKey, t_CCompare, t_CAllocator>::TCSet(TCSet &&_Other)
		: CMap((CMap &&)fg_Move(_Other))
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	TCSet<t_CKey, t_CCompare, t_CAllocator>::TCSet(TCSet const &_Other)
		: CMap((CMap const &)_Other)
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	TCSet<t_CKey, t_CCompare, t_CAllocator>::TCSet(std::initializer_list<t_CKey> const &_Values)
	{
		for (auto &Value : _Values)
			(*this)[Value];
	}


	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename... tfp_CParams>
	TCSet<t_CKey, t_CCompare, t_CAllocator>::TCSet(CAllocatorConstructTag const &_Tag, tfp_CParams && ...p_Params)
		: CMap(_Tag, fg_Forward<tfp_CParams>(p_Params)...)
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename... tfp_CParams>
	TCSet<t_CKey, t_CCompare, t_CAllocator>::TCSet(CCompareConstructTag const &_Tag, tfp_CParams && ...p_Params)
		: CMap(_Tag, fg_Forward<tfp_CParams>(p_Params)...)
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename... tfp_CAllocatorParams, typename... tfp_CCompareParams>
	TCSet<t_CKey, t_CCompare, t_CAllocator>::TCSet
		(
			CAllocatorConstructTag &&_AllocatorTag
			, CCompareConstructTag &&_CompareTag
			, TCConstruct<void, tfp_CAllocatorParams...> &&_ConstructAllocator
			, TCConstruct<void, tfp_CCompareParams...> &&_ConstructCompare
		)
		: CMap(fg_Move(_AllocatorTag), fg_Move(_CompareTag), fg_Move(_ConstructAllocator), fg_Move(_ConstructCompare))
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename tf_COther>
	TCSet<t_CKey, t_CCompare, t_CAllocator>::TCSet(tf_COther &&_Other)
		: CMap(fg_Forward<tf_COther>(_Other))
	{
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator = (TCSet &&_Other) -> TCSet &
	{
		*((CMap *)this) = ((CMap &&)fg_Move(_Other));
		return *this;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator = (TCSet const &_Other) -> TCSet &
	{
		*((CMap *)this) = ((CMap const &)_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename tf_COther>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator = (tf_COther &&_Other) -> TCSet &
	{
		*((CMap *)this) = fg_Forward<tf_COther>(_Other);
		return *this;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	TCSet<t_CKey, t_CCompare, t_CAllocator> TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_KeySet() const
	{
		TCSet<t_CKey, t_CCompare, t_CAllocator> Return;

		for (auto &Value : *this)
			Return[fs_GetKey(Value)];

		return Return;
	}

#ifndef DCompiler_MSVC_Workaround
	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CContainer>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_AddContainer(tf_CContainer &&_Container) -> TCSet &
		requires (!NTraits::cIsVoid<decltype(begin(fg_GetType<tf_CContainer &&>()))>)
	{
		for (auto &Value : _Container)
			(*this)[Value];
		return *this;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CContainer>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::fs_FromContainer(tf_CContainer &&_Container) -> TCSet
		requires (!NTraits::cIsVoid<decltype(begin(fg_GetType<tf_CContainer &&>()))>)
	{
		TCSet Return;
		Return.f_AddContainer(fg_Forward<tf_CContainer>(_Container));
		return Return;
	}
#endif

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_Or(this t_CThis &&_This, TCSet const &_Right) -> TCSet
	{
		if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
		{
			TCSet Return(_Right);
			Return += fg_Move(_This);
			return Return;
		}
		else
		{
			TCSet Return(_This);
			Return += _Right;
			return Return;
		}
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_Or(this t_CThis &&_This, TCSet &&_Right) -> TCSet
	{

		if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
		{
			TCSet Return(fg_Move(_Right));
			Return += fg_Move(_This);
			return Return;
		}
		else
		{
			TCSet Return(_This);
			Return += fg_Move(_Right);
			return Return;
		}
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator | (this t_CThis &&_This, TCSet const &_Right) -> TCSet
	{
		return fg_Forward<t_CThis>(_This).f_Or(_Right);
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator | (this t_CThis &&_This, TCSet &&_Right) -> TCSet
	{
		return fg_Forward<t_CThis>(_This).f_Or(fg_Move(_Right));
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_And(this t_CThis &&_This, TCSet const &_Right) -> TCSet
	{
		TCSet Return;

		if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
		{
			auto iLeft = fg_Move(_This).f_GetIteratorBidirectionalDestructive();
			auto iRight = _Right.f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(*iLeft, *iRight);
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
			auto iLeft = _This.f_GetIterator();
			auto iRight = _Right.f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(*iLeft, *iRight);
				if (Cmp < 0)
					++iLeft;
				else if (Cmp > 0)
					++iRight;
				else
				{
					Return[*iLeft];
					++iLeft;
					++iRight;
				}
			}
		}

		return Return;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_And(this t_CThis &&_This, TCSet &&_Right) -> TCSet
	{
		TCSet Return;
		auto iLeft = _This.f_GetIterator();
		auto iRight = fg_Move(_Right).f_GetIteratorBidirectionalDestructive();

		while (iLeft && iRight)
		{
			auto Cmp = _This.mp_Compare.f_CompareKeys(*iLeft, *iRight);
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

		return Return;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator & (this t_CThis &&_This, TCSet const &_Right) -> TCSet
	{
		return fg_Forward<t_CThis>(_This).f_And(_Right);
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator & (this t_CThis &&_This, TCSet &&_Right) -> TCSet
	{
		return fg_Forward<t_CThis>(_This).f_And(fg_Move(_Right));
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_Xor(this t_CThis &&_This, TCSet const &_Right) -> TCSet
	{
		TCSet Return;

		if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
		{
			if (_Right.f_IsEmpty())
				return fg_Move(_This);
			else if (_This.f_IsEmpty())
				return _Right;

			auto iLeft = fg_Move(_This).f_GetIteratorBidirectionalDestructive();
			auto iRight = _Right.f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(*iLeft, *iRight);
				if (Cmp < 0)
					Return.f_Insert(iLeft.f_ExtractNode());
				else if (Cmp > 0)
				{
					Return[*iRight];
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
				Return[*iRight];
		}
		else
		{
			if (_Right.f_IsEmpty())
				return _This;
			else if (_This.f_IsEmpty())
				return _Right;

			auto iLeft = _This.f_GetIterator();
			auto iRight = _Right.f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(*iLeft, *iRight);
				if (Cmp < 0)
				{
					Return[*iLeft];
					++iLeft;
				}
				else if (Cmp > 0)
				{
					Return[*iRight];
					++iRight;
				}
				else
				{
					++iLeft;
					++iRight;
				}
			}

			for (; iLeft; ++iLeft)
				Return[*iLeft];

			for (; iRight; ++iRight)
				Return[*iRight];
		}

		return Return;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_Xor(this t_CThis &&_This, TCSet &&_Right) -> TCSet
	{
		if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
		{
			if (_Right.f_IsEmpty())
				return fg_Move(_This);
			else if (_This.f_IsEmpty())
				return fg_Move(_Right);

			TCSet Return(fg_Move(_This));

			_Right.f_ExtractAll
				(
					[&](typename CMap::CNodeHandle &&_Handle)
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
			TCSet Return;

			if (_Right.f_IsEmpty())
				return _This;
			else if (_This.f_IsEmpty())
				return fg_Move(_Right);

			auto iLeft = _This.f_GetIterator();
			auto iRight = fg_Move(_Right).f_GetIteratorBidirectionalDestructive();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(*iLeft, *iRight);
				if (Cmp < 0)
				{
					Return[*iLeft];
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
				Return[*iLeft];

			for (; iRight;)
				Return.f_Insert(iRight.f_ExtractNode());

			return Return;
		}
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator ^ (this t_CThis &&_This, TCSet const &_Right) -> TCSet
	{
		return fg_Forward<t_CThis>(_This).f_Xor(_Right);
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator ^ (this t_CThis &&_This, TCSet &&_Right) -> TCSet
	{
		return fg_Forward<t_CThis>(_This).f_Xor(fg_Move(_Right));
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_Difference(this t_CThis &&_This, TCSet const &_Right) -> TCSet
	{
		if constexpr (NTraits::cIsRValueReference<t_CThis &&>)
		{
			TCSet Return = fg_Move(_This);
			Return -= _Right;
			return Return;
		}
		else
		{
			TCSet Return;
			auto iLeft = _This.f_GetIterator();
			auto iRight = _Right.f_GetIterator();

			while (iLeft && iRight)
			{
				auto Cmp = _This.mp_Compare.f_CompareKeys(*iLeft, *iRight);
				if (Cmp < 0)
				{
					Return[*iLeft];
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
				Return[*iLeft];

			return Return;
		}
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename t_CThis>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator - (this t_CThis &&_This, TCSet const &_Right) -> TCSet
	{
		return fg_Forward<t_CThis>(_This).f_Difference(_Right);
	}

	template <typename tf_CReturn, typename... tf_CParams>
	void fg_CreateSetHelper(TCSet<tf_CReturn> &_Return)
	{
	}

	template <typename tf_CReturn, typename tf_CFirst, typename... tf_CParams>
	void fg_CreateSetHelper(TCSet<tf_CReturn> &_Return, tf_CFirst &&_First, tf_CParams && ...p_Params)
	{
		_Return[fg_Forward<tf_CFirst>(_First)];
		fg_CreateSetHelper<tf_CReturn>(_Return, fg_Forward<tf_CParams>(p_Params)...);
	}

	template <typename tf_CFirst, typename... tf_CParams>
	TCSet<NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>> fg_CreateSet(tf_CFirst &&_First, tf_CParams && ...p_Params)
	{
		TCSet<NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>> Return;
		fg_CreateSetHelper<NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>>(Return, fg_Forward<tf_CFirst>(_First), fg_Forward<tf_CParams>(p_Params)...);
		return Return;
	}

	template <typename tf_CReturn, typename... tf_CParams>
	TCSet<tf_CReturn> fg_CreateSet(tf_CParams && ...p_Params)
	{
		TCSet<tf_CReturn> Return;
		fg_CreateSetHelper<tf_CReturn>(Return, fg_Forward<tf_CParams>(p_Params)...);
		return Return;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_GetIteratorDestructive() && -> typename TCSet<t_CKey, t_CCompare, t_CAllocator>::CIteratorDestructive
	{
		return {fg_Move(*this)};
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_GetIteratorReverseDestructive() && -> typename TCSet<t_CKey, t_CCompare, t_CAllocator>::CIteratorReverseDestructive
	{
		return {fg_Move(*this)};
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_GetIteratorBidirectionalDestructive() && -> typename TCSet<t_CKey, t_CCompare, t_CAllocator>::CIteratorBidirectionalDestructive
	{
		return {fg_Move(*this)};
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_GetIteratorBidirectionalReverseDestructive() && -> typename TCSet<t_CKey, t_CCompare, t_CAllocator>::CIteratorBidirectionalReverseDestructive
	{
		return {fg_Move(*this)};
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator[] (tf_CKey &&_Key) -> typename CMap::CUserData &
	{
		return this->mp_Tree.f_FindEqualOrInsert
			(
				_Key
				, [&]() -> typename CMap::CNodeDestructive *
				{
					auto Memory = this->mp_Allocator.f_AllocSafe(sizeof(typename CMap::CNodeDestructive), alignof(typename CMap::CNodeDestructive));
					auto pData = (typename CMap::CNodeDestructive *)Memory.m_pMemory;
					new ((void *)pData) typename CMap::CNodeDestructive(fg_Forward<tf_CKey>(_Key));
					Memory.f_Claim();
					return pData;
				}
				, this->mp_Compare
			)->f_Value()
		;
	}

}
