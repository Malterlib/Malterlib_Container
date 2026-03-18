// Copyright © 2023 Favro Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	bool TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator == (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> const &_Other) const noexcept
		(
			noexcept(fg_GetType<t_CKey const &>() == fg_GetType<tf_CKey const &>())
			&& noexcept(fg_GetType<t_CValue const &>() == fg_GetType<tf_CValue const &>())
		)
	{
		CIteratorConst Iter0 = *this;
		auto Iter1 = _Other.f_GetIterator();
		while (Iter0 && Iter1)
		{
			if (!(Iter0.f_GetKey() == Iter1.f_GetKey()))
				return false;
			if (!(*Iter0 == *Iter1))
				return false;

			++Iter0;
			++Iter1;
		}

		if (!!Iter0 != !!Iter1)
			return false;

		return true;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	bool TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator == (TCMap const &_Other) const noexcept
		(
			noexcept(fg_GetType<t_CKey const &>() == fg_GetType<t_CKey const &>())
			&& noexcept(fg_GetType<t_CValue const &>() == fg_GetType<t_CValue const &>())
		)
	{
		CIteratorConst Iter0 = *this;
		CIteratorConst Iter1 = _Other;
		while (Iter0 && Iter1)
		{
			if (!(Iter0.f_GetKey() == Iter1.f_GetKey()))
				return false;
			if (!(*Iter0 == *Iter1))
				return false;

			++Iter0;
			++Iter1;
		}

		if (!!Iter0 != !!Iter1)
			return false;

		return true;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CKey, typename tf_CValue, typename tf_CCompare, typename tf_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator <=> (TCMap<tf_CKey, tf_CValue, tf_CCompare, tf_CAllocator> const &_Other) const noexcept
		(
			noexcept(fg_GetType<t_CKey const &>() <=> fg_GetType<tf_CKey const &>())
			&& noexcept(fg_GetType<t_CValue const &>() <=> fg_GetType<tf_CValue const &>())
		)
	{
		using COrdering = TCCommonOrderingType
			<
				decltype(fg_GetType<t_CKey const &>() <=> fg_GetType<tf_CKey const &>())
				, decltype(fg_GetType<t_CValue const &>() <=> fg_GetType<tf_CValue const &>())
			>
		;

		CIteratorConst Iter0 = *this;
		auto Iter1 = _Other.f_GetIterator();
		while (Iter0 && Iter1)
		{
			if (auto Result = Iter0.f_GetKey() <=> Iter1.f_GetKey(); Result != 0)
				return COrdering(Result);

			if (auto Result = *Iter0 <=> *Iter1; Result != 0)
				return COrdering(Result);

			++Iter0;
			++Iter1;
		}

		if (!Iter0 && Iter1)
			return COrdering::less;
		else if (Iter0 && !Iter1)
			return COrdering::greater;

		return COrdering::equivalent;
	}

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	auto TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::operator <=> (TCMap const &_Other) const noexcept
		(
			noexcept(fg_GetType<t_CKey const &>() <=> fg_GetType<t_CKey const &>())
			&& noexcept(fg_GetType<t_CValue const &>() <=> fg_GetType<t_CValue const &>())
		)
	{
		using COrdering = TCCommonOrderingType
			<
				decltype(fg_GetType<t_CKey const &>() <=> fg_GetType<t_CKey const &>())
				, decltype(fg_GetType<t_CValue const &>() <=> fg_GetType<t_CValue const &>())
			>
		;

		CIteratorConst Iter0 = *this;
		CIteratorConst Iter1 = _Other;
		while (Iter0 && Iter1)
		{
			if (auto Result = Iter0.f_GetKey() <=> Iter1.f_GetKey(); Result != 0)
				return COrdering(Result);

			if (auto Result = *Iter0 <=> *Iter1; Result != 0)
				return COrdering(Result);

			++Iter0;
			++Iter1;
		}

		if (!Iter0 && Iter1)
			return COrdering::less;
		else if (Iter0 && !Iter1)
			return COrdering::greater;

		return COrdering::equivalent;
	}
}
