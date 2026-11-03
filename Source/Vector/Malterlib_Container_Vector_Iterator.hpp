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

	template <typename t_CIteratorData>
	TCVectorIterator<t_CIteratorData>::TCVectorIterator()
		: mp_pBegin(nullptr)
		, mp_pEnd(nullptr)
	{
	}

	template <typename t_CIteratorData>
	TCVectorIterator<t_CIteratorData>::TCVectorIterator(t_CIteratorData *_Begin, t_CIteratorData *_End)
		: mp_pBegin(_Begin)
		, mp_pEnd(_End)
	{
	}

	template <typename t_CIteratorData>
	TCVectorIterator<t_CIteratorData>::TCVectorIterator(TCVectorIterator const &_ToCopy)
		: mp_pBegin(_ToCopy.mp_pBegin)
		, mp_pEnd(_ToCopy.mp_pEnd)
	{
	}

	template <typename t_CIteratorData>
	inline_small auto TCVectorIterator<t_CIteratorData>::f_GetIterator() const -> TCVectorIterator
	{
		return *this;
	}

	template <typename t_CIteratorData>
	inline_small auto TCVectorIterator<t_CIteratorData>::operator = (TCVectorIterator const &_ToCopy) -> TCVectorIterator &
	{
		mp_pBegin = _ToCopy.mp_pBegin;
		mp_pEnd = _ToCopy.mp_pEnd;
		return *this;
	}

	template <typename t_CIteratorData>
	inline_small bool TCVectorIterator<t_CIteratorData>::operator == (TCVectorIterator const &_Other) const noexcept
	{
		return (mp_pBegin == _Other.mp_pBegin) && (mp_pEnd == _Other.mp_pEnd);
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
