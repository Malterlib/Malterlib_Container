// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

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
		requires (!NTraits::TCIsVoid<decltype(begin(fg_GetType<tf_CContainer &&>()))>::mc_Value)
	{
		for (auto &Value : _Container)
			(*this)[Value];
		return *this;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CContainer>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::fs_FromContainer(tf_CContainer &&_Container) -> TCSet
		requires (!NTraits::TCIsVoid<decltype(begin(fg_GetType<tf_CContainer &&>()))>::mc_Value)
	{
		TCSet Return;
		Return.f_AddContainer(fg_Forward<tf_CContainer>(_Container));
		return Return;
	}
#endif

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_Or(TCSet const &_Other) const -> TCSet 
	{
		TCSet Return = *this;
		Return += _Other;
		return Return;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator | (TCSet const &_Right) const -> TCSet 
	{
		return f_Or(_Right);
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_And(TCSet const &_Other) const -> TCSet 
	{
		TCSet Return;
		auto iLeft = this->f_GetIterator();
		auto iRight = _Other.f_GetIterator();

		if (!iRight)
			return {};

		while (true)
		{
			while (iLeft && *iLeft < *iRight)
				++iLeft;
			if (!iLeft)
				break;
			while (iRight && *iRight < *iLeft)
				++iRight;
			if (!iRight)
				break;
			Return[*iLeft];
			++iLeft;
			++iRight;
		}

		return Return;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator & (TCSet const &_Right) const -> TCSet 
	{
		return f_And(_Right);
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_Xor(TCSet const &_Right) const -> TCSet 
	{
		TCSet Return;
		auto iLeft = this->f_GetIterator();
		auto iRight = _Right.f_GetIterator();
		if (!iRight)
		{
			for (; iLeft; ++iLeft)
				Return[*iLeft];
			return Return;
		}

		while (true)
		{
			for (; iLeft && *iLeft < *iRight; ++iLeft)
				Return[*iLeft];

			if (!iLeft)
			{
				for (; iRight; ++iRight)
					Return[*iRight];
				break;
			}
			for (; iRight && *iRight < *iLeft; ++iRight)
				Return[*iRight];
			if (!iRight)
			{
				for (; iLeft; ++iLeft)
					Return[*iLeft];
				break;
			}
			Return[*iLeft];
			++iLeft;
			++iRight;
		}

		return Return;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator ^ (TCSet const &_Right) -> TCSet 
	{
		return f_Xor(_Right);
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::f_Difference(TCSet const &_Right) const -> TCSet 
	{
		TCSet Return = *this;

		DMibFastCheck(&Return != this);

		Return -= _Right;

		return Return;
	}

	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	auto TCSet<t_CKey, t_CCompare, t_CAllocator>::operator - (TCSet const &_Right) -> TCSet 
	{
		return f_Difference(_Right);
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
	TCSet<typename NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>::CType> fg_CreateSet(tf_CFirst &&_First, tf_CParams && ...p_Params)
	{
		TCSet<typename NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>::CType> Return;
		fg_CreateSetHelper<typename NTraits::TCRemoveReferenceAndQualifiers<tf_CFirst>::CType>(Return, fg_Forward<tf_CFirst>(_First), fg_Forward<tf_CParams>(p_Params)...);
		return Return;
	}

	template <typename tf_CReturn, typename... tf_CParams>
	TCSet<tf_CReturn> fg_CreateSet(tf_CParams && ...p_Params)
	{
		TCSet<tf_CReturn> Return;
		fg_CreateSetHelper<tf_CReturn>(Return, fg_Forward<tf_CParams>(p_Params)...);
		return Return;
	}
}
