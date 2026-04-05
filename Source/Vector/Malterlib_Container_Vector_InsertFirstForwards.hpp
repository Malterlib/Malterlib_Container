// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Push(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector)
	{
		return f_InsertFirst(_Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Push(TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector)
	{
		return f_InsertFirst(_Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Push(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector)
	{
		return f_InsertFirst(_Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Push(const t_CData *_pData, umint _Len)
	{
		return f_InsertFirst(_pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_Push(t_CData *_pData, umint _Len)
	{
		return f_InsertFirst(_pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_PushMove(t_CData *_pData, umint _Len)
	{
		return f_InsertFirstMove(_pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_Push(const t_CData &_Data)
	{
		return f_InsertFirst(_Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_Push(t_CData &_Data)
	{
		return f_InsertFirst(_Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_Push(t_CData &&_Data)
	{
		return f_InsertFirst(_Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_Push()
	{
		return f_InsertFirst();
	}
}
