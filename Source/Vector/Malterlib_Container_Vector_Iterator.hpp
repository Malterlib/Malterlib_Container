// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_GetIterator() -> CIterator
	{
		return CIterator(f_GetArray(), f_GetArray() + f_GetLen());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_GetIterator() const -> CIteratorConst
	{
		return CIteratorConst(f_GetArray(), f_GetArray() + f_GetLen());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CIteratorData>
	umint TCVector<t_CData, t_CAllocator, t_COptions>::fp_GetIteratorPos(TCVectorIterator<tf_CIteratorData> const &_iIter) const
	{
		return _iIter - f_GetArray();
	}

	template <typename t_CIteratorData>
	TCVectorIterator<t_CIteratorData>::TCVectorIterator()
		: mp_pBegin(nullptr)
		, mp_pEnd(nullptr)
	{
	}

	template <typename t_CIteratorData>
	TCVectorIterator<t_CIteratorData>::TCVectorIterator(t_CIteratorData *_pBegin, t_CIteratorData *_pEnd)
		: mp_pBegin(_pBegin)
		, mp_pEnd(_pEnd)
	{
	}

	template <typename t_CIteratorData>
	TCVectorIterator<t_CIteratorData>::TCVectorIterator(TCVectorIterator const &_iToCopy)
		: mp_pBegin(_iToCopy.mp_pBegin)
		, mp_pEnd(_iToCopy.mp_pEnd)
	{
	}

	template <typename t_CIteratorData>
	inline_small auto TCVectorIterator<t_CIteratorData>::f_GetIterator() const -> TCVectorIterator
	{
		return *this;
	}

	template <typename t_CIteratorData>
	inline_small auto TCVectorIterator<t_CIteratorData>::operator = (TCVectorIterator const &_iToCopy) -> TCVectorIterator &
	{
		mp_pBegin = _iToCopy.mp_pBegin;
		mp_pEnd = _iToCopy.mp_pEnd;
		return *this;
	}

	template <typename t_CIteratorData>
	inline_small bool TCVectorIterator<t_CIteratorData>::operator == (TCVectorIterator const &_iOther) const noexcept
	{
		return (mp_pBegin == _iOther.mp_pBegin) && (mp_pEnd == _iOther.mp_pEnd);
	}

	template <typename t_CIteratorData>
	inline_small TCVectorIterator<t_CIteratorData>::operator t_CIteratorData * () const
	{
		return fp_GetCurrent();
	}

	template <typename t_CIteratorData>
	inline_small t_CIteratorData *TCVectorIterator<t_CIteratorData>::operator -> () const
	{
		return fp_GetCurrent();
	}

	template <typename t_CIteratorData>
	inline_small auto TCVectorIterator<t_CIteratorData>::operator ++ () -> TCVectorIterator &
	{
		mp_pBegin = mp_pBegin + 1;
		DMibFastCheck(mp_pBegin <= mp_pEnd);

		return *this;
	}

	template <typename t_CIteratorData>
	inline_small auto TCVectorIterator<t_CIteratorData>::operator += (smint _Offset) -> TCVectorIterator &
	{
		mp_pBegin = mp_pBegin + _Offset;
		DMibFastCheck(mp_pBegin <= mp_pEnd);

		return *this;
	}

	template <typename t_CIteratorData>
	inline_small umint TCVectorIterator<t_CIteratorData>::f_GetLen() const
	{
		return mp_pEnd - mp_pBegin;
	}

	template <typename t_CIteratorData>
	inline_small t_CIteratorData *TCVectorIterator<t_CIteratorData>::fp_GetCurrent() const
	{
		if (mp_pBegin != mp_pEnd)
			return mp_pBegin;
		else
			return nullptr;
	}
}
