// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector)
	{
		return f_InsertBefore(fp_GetIteratorPos(_Position), _Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector)
	{
		return f_InsertBefore(fp_GetIteratorPos(_Position), _Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator, typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_Position, TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector)
	{
		return f_InsertBefore(fp_GetIteratorPos(_Position), fg_Move(_Vector));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_Position, const t_CData *_pData, umint _Len)
	{
		return f_InsertBefore(fp_GetIteratorPos(_Position), _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_Position, t_CData *_pData, umint _Len)
	{
		return f_InsertBefore(fp_GetIteratorPos(_Position), _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBeforeMove(TCVectorIterator<tf_CIterator> const &_Position, t_CData *_pData, umint _Len)
	{
		return f_InsertBeforeMove(fp_GetIteratorPos(_Position), _pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_Position, const t_CData &_Data)
	{
		return f_InsertBefore(fp_GetIteratorPos(_Position), _Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_Position, t_CData &_Data)
	{
		return f_InsertBefore(fp_GetIteratorPos(_Position), _Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_Position, t_CData &&_Data)
	{
		return f_InsertBefore(fp_GetIteratorPos(_Position), fg_Move(_Data));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIterator>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertBefore(TCVectorIterator<tf_CIterator> const &_Position)
	{
		return f_InsertBefore(fp_GetIteratorPos(_Position));
	}
}
