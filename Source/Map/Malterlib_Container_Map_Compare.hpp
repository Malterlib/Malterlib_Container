// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator == (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> const &_Other) const
		noexcept(noexcept(fg_GetType<t_CKey const &>() == fg_GetType<tf_CKey const &>()) && noexcept(fg_GetType<t_CValue const &>() == fg_GetType<tf_CValue const &>()))
		-> bool
	{
		CIteratorConst iThis = *this;
		auto iOther = _Other.f_GetIterator();
		while (iThis && iOther)
		{
			if (!(iThis.f_GetKey() == iOther.f_GetKey()))
				return false;
			if (!(*iThis == *iOther))
				return false;

			++iThis;
			++iOther;
		}

		if (!!iThis != !!iOther)
			return false;

		return true;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator == (TCMap const &_Other) const
		noexcept(noexcept(fg_GetType<t_CKey const &>() == fg_GetType<t_CKey const &>()) && noexcept(fg_GetType<t_CValue const &>() == fg_GetType<t_CValue const &>()))
		-> bool
	{
		CIteratorConst iThis = *this;
		CIteratorConst iOther = _Other;
		while (iThis && iOther)
		{
			if (!(iThis.f_GetKey() == iOther.f_GetKey()))
				return false;
			if (!(*iThis == *iOther))
				return false;

			++iThis;
			++iOther;
		}

		if (!!iThis != !!iOther)
			return false;

		return true;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator <=> (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> const &_Other) const
		noexcept(noexcept(fg_GetType<t_CKey const &>() <=> fg_GetType<tf_CKey const &>()) && noexcept(fg_GetType<t_CValue const &>() <=> fg_GetType<tf_CValue const &>()))
	{
		using COrdering = TCCommonOrderingType
			<
				decltype(fg_GetType<t_CKey const &>() <=> fg_GetType<tf_CKey const &>())
				, decltype(fg_GetType<t_CValue const &>() <=> fg_GetType<tf_CValue const &>())
			>
		;

		CIteratorConst iThis = *this;
		auto iOther = _Other.f_GetIterator();
		while (iThis && iOther)
		{
			if (auto Result = iThis.f_GetKey() <=> iOther.f_GetKey(); Result != 0)
				return COrdering(Result);

			if (auto Result = *iThis <=> *iOther; Result != 0)
				return COrdering(Result);

			++iThis;
			++iOther;
		}

		if (!iThis && iOther)
			return COrdering::less;
		else if (iThis && !iOther)
			return COrdering::greater;

		return COrdering::equivalent;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator <=> (TCMap const &_Other) const
		noexcept(noexcept(fg_GetType<t_CKey const &>() <=> fg_GetType<t_CKey const &>()) && noexcept(fg_GetType<t_CValue const &>() <=> fg_GetType<t_CValue const &>()))
	{
		using COrdering = TCCommonOrderingType
			<
				decltype(fg_GetType<t_CKey const &>() <=> fg_GetType<t_CKey const &>())
				, decltype(fg_GetType<t_CValue const &>() <=> fg_GetType<t_CValue const &>())
			>
		;

		CIteratorConst iThis = *this;
		CIteratorConst iOther = _Other;
		while (iThis && iOther)
		{
			if (auto Result = iThis.f_GetKey() <=> iOther.f_GetKey(); Result != 0)
				return COrdering(Result);

			if (auto Result = *iThis <=> *iOther; Result != 0)
				return COrdering(Result);

			++iThis;
			++iOther;
		}

		if (!iThis && iOther)
			return COrdering::less;
		else if (iThis && !iOther)
			return COrdering::greater;

		return COrdering::equivalent;
	}
}
