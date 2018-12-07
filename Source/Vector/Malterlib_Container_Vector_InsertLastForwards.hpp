// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::operator << (tf_CData &&_Data) -> TCVector &
	{
		f_Insert(fg_Forward<tf_CData>(_Data));
		return *this;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertLast(TCVector<tf_CData, tf_CAllocator, tf_COptions> const &_Vector)
	{
		return f_Insert(_Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertLast(TCVector<tf_CData, tf_CAllocator, tf_COptions> &_Vector)
	{
		return f_Insert(_Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CData, typename tf_CAllocator, typename tf_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertLast(TCVector<tf_CData, tf_CAllocator, tf_COptions> &&_Vector)
	{
		return f_Insert(_Vector);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertLast(const t_CData *_pData, mint _Len)
	{
		return f_Insert(_pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData *TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertLast(t_CData *_pData, mint _Len)
	{
		return f_Insert(_pData, _Len);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CType, typename... tfp_CParams>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertLast(TCConstruct<tf_CType, tfp_CParams...> &&_CreateParams)
	{
		return f_Insert(fg_Move(_CreateParams));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertLast(const t_CData &_Data)
	{
		return f_Insert(_Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertLast(t_CData &_Data)
	{
		return f_Insert(_Data);
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertLast(t_CData &&_Data)
	{
		return f_Insert(fg_Move(_Data));
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	t_CData &TCVector<t_CData, t_CAllocator, t_COptions>::f_InsertLast()
	{
		return f_Insert();
	}
}
