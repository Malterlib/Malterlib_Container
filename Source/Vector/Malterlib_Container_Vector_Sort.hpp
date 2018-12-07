// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CSorter>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Sort(tf_CSorter &&_Sorter)
	{
		fpr_QuickSort(f_GetArray(), 0, f_GetLen()-1, fg_Forward<tf_CSorter>(_Sorter));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Sort()
	{
		f_Sort(CSort_Default());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CSorter>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsSorted(tf_CSorter &&_Sorter) const
	{
		aint len = f_GetLen();
		if(len<2) return 1;
		t_CData const*pArray = f_GetArray();
		for(aint i=0,j=1;j!=len;++i,++j)
			if(fg_Forward<tf_CSorter>(_Sorter)(pArray[j], pArray[i]))
				return 0;
		return 1;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsAdjacentUnique() const
	{
		return f_IsSorted(CSort_Default());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CSorter>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsSortedNoDuplicates(tf_CSorter &&_Sorter) const
	{
		aint len = f_GetLen();
		if(len<2) return 1;
		t_CData const*pArray = f_GetArray();
		for(aint i=0,j=1;j!=len;++i,++j)
			if(!fg_Forward<tf_CSorter>(_Sorter)(pArray[i],pArray[j]))
				return 0;
		return 1;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsSortedNoDuplicates() const
	{
		return f_IsSortedNoDuplicates(CSort_Default());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CSorter>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_UniqueIfSorted(tf_CSorter &&_Sorter)
	{
		if (f_GetLen() <= 1)
			return;

		DMibSafeCheck(f_IsSorted(fg_Forward<tf_CSorter>(_Sorter)), "Vector must be sorted");

		auto iUniqueFinish = f_GetIterator();
		auto iData = iUniqueFinish;

		while (++iData)
		{
			if (fg_Forward<tf_CSorter>(_Sorter)(*iUniqueFinish, *iData) && (++iUniqueFinish != iData))
				*iUniqueFinish = fg_Move(*iData);
		}
		++iUniqueFinish;

		mint iPos = iUniqueFinish ? iUniqueFinish - f_GetIterator() : f_GetLen();
		f_SetLen(iPos);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_UniqueIfSorted()
	{
		f_UniqueIfSorted(CSort_Default());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CSorter>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fp_InsertSort(t_CData *_pArray, aint _Low, aint _High, tf_CSorter &&_Sorter)
	{
		t_CData Temp;
		aint i, j;

		for (i = _Low + 1; i <= _High; i++)
		{
			Temp = fg_Move(_pArray[i]);

			for (j = i-1; j >= _Low && fg_Forward<tf_CSorter>(_Sorter)(Temp, _pArray[j]); j--)
				_pArray[j+1] = fg_Move(_pArray[j]);

			_pArray[j+1] = fg_Move(Temp);
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CSorter>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::fp_Partition(t_CData *_pArray, aint _Low, aint _High, tf_CSorter &&_Sorter)
	{
		t_CData Temp, Pivot;
		aint i, j, p;

		p = _Low + ((_High - _Low)>>1);
		Pivot = fg_Move(_pArray[p]);
		_pArray[p] = fg_Move(_pArray[_Low]);

		i = _Low+1;
		j = _High;
		while (1)
		{
			while (i < j && fg_Forward<tf_CSorter>(_Sorter)(_pArray[i], Pivot))
				i++;
			while (j >= i && fg_Forward<tf_CSorter>(_Sorter)(Pivot, _pArray[j]))
				j--;
			if (i >= j)
				break;
			Temp = fg_Move(_pArray[i]);
			_pArray[i] = fg_Move(_pArray[j]);
			_pArray[j] = fg_Move(Temp);
			j--; i++;
		}

		if (_Low != j)
			_pArray[_Low] = fg_Move(_pArray[j]);
		_pArray[j] = fg_Move(Pivot);

		return j;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CSorter>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fpr_QuickSort(t_CData *_pArray, aint _Low, aint _High, tf_CSorter &&_Sorter)
	{
		aint m;

		while (_Low < _High)
		{

			if (_High - _Low <= 12)
			{
				fp_InsertSort(_pArray, _Low, _High, _Sorter);
				return;
			}

			m = fp_Partition(_pArray, _Low, _High, _Sorter);

			if (m - _Low <= _High - m)
			{
				fpr_QuickSort(_pArray, _Low, m - 1, _Sorter);
				_Low = m + 1;
			}
			else
			{
				fpr_QuickSort(_pArray, m + 1, _High, _Sorter);
				_High = m - 1;
			}
		}
	}
}
