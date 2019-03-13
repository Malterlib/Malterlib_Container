// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::operator == (TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Other) const
	{
		mint Len = f_GetLen();
		if (Len != _Other.f_GetLen())
			return false;

		auto pThisArray = f_GetArray();
		auto pOtherArray = _Other.f_GetArray();

		for (mint i = 0; i < Len; ++i)
		{
			if (pThisArray[i] != pOtherArray[i])
				return false;
		}

		return true;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::operator < (TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Other) const
	{
		mint Len = f_GetLen();
		mint OtherLen = _Other.f_GetLen();

		if (Len < OtherLen)
			return true;
		else if (Len > OtherLen)
			return false;

		auto pThisArray = f_GetArray();
		auto pOtherArray = _Other.f_GetArray();

		for (mint i = 0; i < Len; ++i)
		{
			auto &Left = pThisArray[i];
			auto &Right = pOtherArray[i];

			if (Left < Right)
				return true;
			else if (Left > Right)
				return false;
		}

		return false;
	}
	
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions, typename t_CFunctor>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::f_Compare(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Other, t_CFunctor &&_Functor) const
	{
		mint Len0 = f_GetLen();
		mint Len1 = _Other.f_GetLen();
		if (Len0 > Len1)
			return 1;
		else if (Len0 < Len1)
			return -1;

		const t_CData *pData0 = f_GetArray();
		const tf_CData *pData1 = _Other.f_GetArray();

		for (mint i = 0; i < Len0; ++i)
		{
			aint iCompare = _Functor(pData0[i], pData1[i]);
			if (iCompare)
				return iCompare;
		}
		return 0;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::f_Compare(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Other) const
	{
		mint Len0 = f_GetLen();
		mint Len1 = _Other.f_GetLen();
		if (Len0 > Len1)
			return 1;
		else if (Len0 < Len1)
			return -1;

		const t_CData *pData0 = f_GetArray();
		const tf_CData *pData1 = _Other.f_GetArray();

		for (mint i = 0; i < Len0; ++i)
		{
			if (pData0[i] > pData1[i])
				return 1;
			else if (pData0[i] < pData1[i])
				return -1;
		}
		return 0;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::f_CompareLexicographical(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Other) const
	{
		mint Len0 = f_GetLen();
		mint Len1 = _Other.f_GetLen();
		mint Len = fg_Min(Len0, Len1);

		const t_CData *pData0 = f_GetArray();
		const tf_CData *pData1 = _Other.f_GetArray();

		for (mint iElement = 0; iElement < Len; ++iElement)
		{
			if (pData0[iElement] > pData1[iElement])
				return 1;
			else if (pData0[iElement] < pData1[iElement])
				return -1;
		}

		if (Len1 < Len0)
			return 1;

		if (Len0 < Len1)
			return -1;

		return 0;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template
	<
		typename tf_CData0
		, typename tf_CAllocator0
		, typename tf_COptions0
		, typename tf_CData1
		, typename tf_CAllocator1
		, typename tf_COptions1
		, typename tf_CData2
		, typename tf_CAllocator2
		, typename tf_COptions2
	>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_FindDifferences
	(
		const TCVector<tf_CData0, tf_CAllocator0, tf_COptions0> &_CompareTo
		, TCVector<tf_CData1, tf_CAllocator1, tf_COptions1> *_pAdded
		, TCVector<tf_CData2, tf_CAllocator2, tf_COptions2> *_pRemoved) const
	{
		// Find added
		if (_pAdded)
		{
			TCVector Sorted = *this;
			Sorted.f_Sort();
			mint nItems = _CompareTo.f_GetLen();
			for (mint i = 0; i < nItems; ++i)
			{
				const tf_CData0 &CompareTo = _CompareTo[i];
				if (Sorted.f_BinarySearch(CompareTo) < 0)
					_pAdded->f_Insert(CompareTo);
			}
		}

		// Find removed
		if (_pRemoved)
		{
			TCVector<tf_CData0> Sorted = _CompareTo;
			Sorted.f_Sort();
			mint nItems = f_GetLen();
			for (mint i = 0; i < nItems; ++i)
			{
				const t_CData &CompareTo = (*this)[i];
				if (Sorted.f_BinarySearch(CompareTo) < 0)
					_pRemoved->f_Insert(CompareTo);
			}
		}
	}
}
