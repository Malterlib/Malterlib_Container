// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_FCompare>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Sort(tf_FCompare &&_fCompare)
	{
		fp_QuickSort(f_GetArray(), 0, f_GetLen() - 1, _fCompare);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Sort()
	{
		f_Sort(CSort_Default());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_FCompare>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsSorted(tf_FCompare &&_fCompare) const
	{
		aint Length = f_GetLen();
		if (Length < 2)
			return true;

		t_CData const *pArray = f_GetArray();
		for (umint iPrevious = 0, iCurrent = 1; iCurrent != Length; ++iPrevious, ++iCurrent)
		{
			if (fg_CheckOrdering(_fCompare(pArray[iCurrent], pArray[iPrevious])) < 0)
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
	template <typename tf_FCompare>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_IsSortedNoDuplicates(tf_FCompare &&_fCompare) const
	{
		aint Length = f_GetLen();
		if (Length < 2)
			return true;

		t_CData const *pArray = f_GetArray();

		for (umint iPrevious = 0, iCurrent = 1; iCurrent != Length; ++iPrevious, ++iCurrent)
		{
			if (fg_CheckOrdering(_fCompare(pArray[iPrevious], pArray[iCurrent])) >= 0)
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
	template <typename tf_FCompare>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_UniqueIfSorted(tf_FCompare &&_fCompare)
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

		umint iPos = iUniqueFinish ? iUniqueFinish - f_GetIterator() : f_GetLen();
		f_SetLen(iPos);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_UniqueIfSorted()
	{
		f_UniqueIfSorted(CSort_Default());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_FCompare>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fp_InsertSort(t_CData *_pArray, aint _Low, aint _High, tf_FCompare &&_fCompare)
	{
		t_CData Temp;
		aint iUnsorted, iSorted;

		for (iUnsorted = _Low + 1; iUnsorted <= _High; iUnsorted++)
		{
			Temp = fg_Move(_pArray[iUnsorted]);

			for (iSorted = iUnsorted - 1; iSorted >= _Low && fg_CheckOrdering(_fCompare(Temp, _pArray[iSorted])) < 0; iSorted--)
				_pArray[iSorted + 1] = fg_Move(_pArray[iSorted]);

			_pArray[iSorted + 1] = fg_Move(Temp);
		}
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_FCompare>
	aint TCVector<t_CData, t_CAllocator, t_COptions>::fp_Partition(t_CData *_pArray, aint _Low, aint _High, tf_FCompare &&_fCompare)
	{
		t_CData Temp, Pivot;
		aint iLeft, iRight, iPivot;

		iPivot = _Low + ((_High - _Low) >> 1);
		Pivot = fg_Move(_pArray[iPivot]);
		_pArray[iPivot] = fg_Move(_pArray[_Low]);

		iLeft = _Low + 1;
		iRight = _High;
		while (1)
		{
			while (iLeft < iRight && fg_CheckOrdering(_fCompare(_pArray[iLeft], Pivot)) < 0)
				iLeft++;
			while (iRight >= iLeft && fg_CheckOrdering(_fCompare(Pivot, _pArray[iRight])) < 0)
				iRight--;
			if (iLeft >= iRight)
				break;
			Temp = fg_Move(_pArray[iLeft]);
			_pArray[iLeft] = fg_Move(_pArray[iRight]);
			_pArray[iRight] = fg_Move(Temp);
			iRight--;
			iLeft++;
		}

		if (_Low != iRight)
			_pArray[_Low] = fg_Move(_pArray[iRight]);
		_pArray[iRight] = fg_Move(Pivot);

		return iRight;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_FCompare>
	void TCVector<t_CData, t_CAllocator, t_COptions>::fp_QuickSort(t_CData *_pArray, aint _Low, aint _High, tf_FCompare &&_fCompare)
	{
		aint iPivot;

		while (_Low < _High)
		{
			if (_High - _Low <= 12)
			{
				fp_InsertSort(_pArray, _Low, _High, _fCompare);
				return;
			}

			iPivot = fp_Partition(_pArray, _Low, _High, _fCompare);

			if (iPivot - _Low <= _High - iPivot)
			{
				fp_QuickSort(_pArray, _Low, iPivot - 1, _fCompare);
				_Low = iPivot + 1;
			}
			else
			{
				fp_QuickSort(_pArray, iPivot + 1, _High, _fCompare);
				_High = iPivot - 1;
			}
		}
	}
}
