// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CCompare>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Sort(tf_CCompare &&_fCompare)
	{
		fpr_QuickSort(f_GetArray(), 0, f_GetLen() - 1, _fCompare);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Sort()
	{
		f_Sort(CSort_Default());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CCompare>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsSorted(tf_CCompare &&_fCompare) const
	{
		aint Length = f_GetLen();
		if (Length < 2)
			return true;

		t_CData const *pArray = f_GetArray();
		for (mint i = 0, j = 1; j != Length; ++i, ++j)
		{
			if (fg_CheckOrdering(_fCompare(pArray[j], pArray[i])) < 0)
				return false;
		}

		return true;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsAdjacentUnique() const
	{
		return f_IsSorted(CSort_Default());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CCompare>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsSortedNoDuplicates(tf_CCompare &&_fCompare) const
	{
		aint Length = f_GetLen();
		if (Length < 2)
			return true;

		t_CData const *pArray = f_GetArray();

		for (mint i = 0, j = 1; j != Length; ++i, ++j)
		{
			if (fg_CheckOrdering(_fCompare(pArray[i], pArray[j])) >= 0)
				return false;
		}

		return true;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsSortedNoDuplicates() const
	{
		return f_IsSortedNoDuplicates(CSort_Default());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CCompare>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_UniqueIfSorted(tf_CCompare &&_fCompare)
	{
		if (f_GetLen() <= 1)
			return;

		DMibSafeCheck(f_IsSorted(_fCompare), "Vector must be sorted");

		auto iUniqueFinish = f_GetIterator();
		auto iData = iUniqueFinish;

		while (++iData)
		{
			if (fg_CheckOrdering(_fCompare(*iUniqueFinish, *iData)) < 0 && (++iUniqueFinish != iData))
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
	template <typename tf_CCompare>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fp_InsertSort(t_CData *_pArray, aint _Low, aint _High, tf_CCompare &&_fCompare)
	{
		t_CData Temp;
		aint i, j;

		for (i = _Low + 1; i <= _High; i++)
		{
			Temp = fg_Move(_pArray[i]);

			for (j = i-1; j >= _Low && fg_CheckOrdering(_fCompare(Temp, _pArray[j])) < 0; j--)
				_pArray[j+1] = fg_Move(_pArray[j]);

			_pArray[j+1] = fg_Move(Temp);
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CCompare>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::fp_Partition(t_CData *_pArray, aint _Low, aint _High, tf_CCompare &&_fCompare)
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
			while (i < j && fg_CheckOrdering(_fCompare(_pArray[i], Pivot)) < 0)
				i++;
			while (j >= i && fg_CheckOrdering(_fCompare(Pivot, _pArray[j])) < 0)
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
	template <typename tf_CCompare>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fpr_QuickSort(t_CData *_pArray, aint _Low, aint _High, tf_CCompare &&_fCompare)
	{
		aint m;

		while (_Low < _High)
		{

			if (_High - _Low <= 12)
			{
				fp_InsertSort(_pArray, _Low, _High, _fCompare);
				return;
			}

			m = fp_Partition(_pArray, _Low, _High, _fCompare);

			if (m - _Low <= _High - m)
			{
				fpr_QuickSort(_pArray, _Low, m - 1, _fCompare);
				_Low = m + 1;
			}
			else
			{
				fpr_QuickSort(_pArray, m + 1, _High, _fCompare);
				_High = m - 1;
			}
		}
	}
}
