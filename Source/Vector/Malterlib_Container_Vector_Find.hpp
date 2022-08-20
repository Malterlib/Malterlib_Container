// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename t_CType2>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::f_Contains(const t_CType2 &_Data) const
	{
		aint Len = f_GetLen();
		const t_CData *pArray = f_GetArray();

		for (int i = 0; i < Len; ++i)
		{
			if (pArray[i] == _Data)
				return i;
		}
		return -1;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_FUnaryPredicate>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::f_ContainsIf(tf_FUnaryPredicate &&_fPredicate) const
	{
		aint Len = f_GetLen();
		const t_CData *pArray = f_GetArray();

		for (int iData = 0; iData < Len; ++iData)
		{
			if (_fPredicate(pArray[iData]))
				return iData;
		}
		return -1;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CCompare, typename t_CFind>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::f_BinarySearch(tf_CCompare &&_fCompare, const t_CFind &_ToFind, aint _nMax) const
	{
		mint Len = f_GetLen();
		if (_nMax >= 0)
			Len = fg_Min(mint(_nMax), Len);
		mint Low = 0;
		mint High = Len;
		t_CData const*pArray = f_GetArray();

		while(Low < High)
		{
			mint Mid = (Low + High) >> 1;
			if (COrdering_Partial(_fCompare(pArray[Mid], _ToFind)) < 0)
				Low = Mid + 1;
			else
				High = Mid;
		}
		if(Low < Len && COrdering_Partial(_fCompare(pArray[Low], _ToFind)) == 0)
			return Low;
		else
			return -1;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename t_CFind>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::f_BinarySearch(const t_CFind &_ToFind) const
	{
		return f_BinarySearch(CSort_Default(), _ToFind, -1);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CCompare, typename t_CFind>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::f_BinarySearchLowerBound(tf_CCompare &&_fCompare, const t_CFind &_ToFind) const
	{
		mint Low = 0;
		mint High = f_GetLen();

		if (High == 0)
			return -1;

		while(Low < High)
		{
			mint Mid = (Low + High) >> 1;
			if(COrdering_Partial(_fCompare(operator[](Mid) , _ToFind)) < 0)
				Low = Mid + 1;
			else
				High = Mid;
		}

		return Low;
	}
}
