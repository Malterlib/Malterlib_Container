// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_iPosition, TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector)
	{
		return f_InsertBefore(fp_GetIteratorPos(_iPosition), _Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_iPosition, TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector)
	{
		return f_InsertBefore(fp_GetIteratorPos(_iPosition), _Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_iPosition, TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector)
	{
		return f_InsertBefore(fp_GetIteratorPos(_iPosition), fg_Move(_Vector));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_iPosition, t_CData const *_pData, umint _Len)
	{
		return f_InsertBefore(fp_GetIteratorPos(_iPosition), _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_iPosition, t_CData *_pData, umint _Len)
	{
		return f_InsertBefore(fp_GetIteratorPos(_iPosition), _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBeforeMove(TCVectorIterator<tf_CIterator> const &_iPosition, t_CData *_pData, umint _Len)
	{
		return f_InsertBeforeMove(fp_GetIteratorPos(_iPosition), _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_iPosition, t_CData const &_Data)
	{
		return f_InsertBefore(fp_GetIteratorPos(_iPosition), _Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_iPosition, t_CData &_Data)
	{
		return f_InsertBefore(fp_GetIteratorPos(_iPosition), _Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_iPosition, t_CData &&_Data)
	{
		return f_InsertBefore(fp_GetIteratorPos(_iPosition), fg_Move(_Data));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_iPosition)
	{
		return f_InsertBefore(fp_GetIteratorPos(_iPosition));
	}
}
