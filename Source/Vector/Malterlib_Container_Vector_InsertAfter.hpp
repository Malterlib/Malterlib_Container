// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(umint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector)
	{
		return f_InsertBefore(_Position + 1, _Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(umint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector)
	{
		return f_InsertBefore(_Position + 1, _Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(umint _Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector)
	{
		return f_InsertBefore(_Position + 1, fg_Move(_Vector));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(umint _Position, const t_CData *_pData, umint _Len)
	{
		return f_InsertBefore(_Position + 1, _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(umint _Position, t_CData *_pData, umint _Len)
	{
		return f_InsertBefore(_Position + 1, _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfterMove(umint _Position, t_CData *_pData, umint _Len)
	{
		return f_InsertBeforeMove(_Position + 1, _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(umint _Position, const t_CData &_Data)
	{
		return f_InsertBefore(_Position + 1, _Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(umint _Position, t_CData &_Data)
	{
		return f_InsertBefore(_Position + 1, _Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(umint _Position, t_CData &&_Data)
	{
		return f_InsertBefore(_Position + 1, fg_Move(_Data));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(umint _Position)
	{
		return f_InsertBefore(_Position + 1);
	}


	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector)
	{
		return f_InsertAfter(fp_GetIteratorPos(_Position), _Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector)
	{
		return f_InsertAfter(fp_GetIteratorPos(_Position), _Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector)
	{
		return f_InsertAfter(fp_GetIteratorPos(_Position), fg_Move(_Vector));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, const t_CData *_pData, umint _Len)
	{
		return f_InsertAfter(fp_GetIteratorPos(_Position), _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, t_CData *_pData, umint _Len)
	{
		return f_InsertAfter(fp_GetIteratorPos(_Position), _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfterMove(TCVectorIterator<tf_CIterator> const &_Position, t_CData *_pData, umint _Len)
	{
		return f_InsertAfterMove(fp_GetIteratorPos(_Position), _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, const t_CData &_Data)
	{
		return f_InsertAfter(fp_GetIteratorPos(_Position), _Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, t_CData &_Data)
	{
		return f_InsertAfter(fp_GetIteratorPos(_Position), _Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position, t_CData &&_Data)
	{
		return f_InsertAfter(fp_GetIteratorPos(_Position), fg_Move(_Data));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertAfter(TCVectorIterator<tf_CIterator> const &_Position)
	{
		return f_InsertAfter(fp_GetIteratorPos(_Position));
	}
}
