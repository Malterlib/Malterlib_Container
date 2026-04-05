// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_GetIterator() -> CIterator
	{
		return CIterator(f_GetArray(),f_GetArray()+f_GetLen());
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	auto TCVector<t_CData, t_CAllocator, t_COptions>::f_GetIterator() const -> CIteratorConst
	{
		return CIteratorConst(f_GetArray(),f_GetArray()+f_GetLen());
	}

	template <typename t_CIteratorData>
	TCVectorIterator<t_CIteratorData>::TCVectorIterator()
		: m_pBegin(nullptr)
		, m_pEnd(nullptr)
	{
	}

	template <typename t_CIteratorData>
	TCVectorIterator<t_CIteratorData>::TCVectorIterator(t_CIteratorData *_Begin,t_CIteratorData *_End)
		: m_pBegin(_Begin)
		, m_pEnd(_End)
	{
	}

	template <typename t_CIteratorData>
	TCVectorIterator<t_CIteratorData>::TCVectorIterator(TCVectorIterator const& _ToCopy)
		: m_pBegin(_ToCopy.m_pBegin)
		, m_pEnd(_ToCopy.m_pEnd)
	{
	}

	template <typename t_CIteratorData>
	inline_small auto TCVectorIterator<t_CIteratorData>::f_GetIterator() const -> TCVectorIterator
	{
		return *this;
	}

	template <typename t_CIteratorData>
	inline_small auto TCVectorIterator<t_CIteratorData>::operator = (TCVectorIterator const& _ToCopy) -> TCVectorIterator &
	{
		m_pBegin = _ToCopy.m_pBegin;
		m_pEnd = _ToCopy.m_pEnd;
		return *this;
	}

	template <typename t_CIteratorData>
	inline_small bool TCVectorIterator<t_CIteratorData>::operator == (TCVectorIterator const& _Other) const noexcept
	{
		return (m_pBegin == _Other.m_pBegin) && (m_pEnd == _Other.m_pEnd);
	}

	template <typename t_CIteratorData>
	inline_small TCVectorIterator<t_CIteratorData>::operator t_CIteratorData *() const
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
		m_pBegin = m_pBegin + 1;
		DMibFastCheck(m_pBegin <= m_pEnd);

		return *this;
	}

	template <typename t_CIteratorData>
	inline_small auto TCVectorIterator<t_CIteratorData>::operator += (smint _Offset) -> TCVectorIterator &
	{
		m_pBegin = m_pBegin + _Offset;
		DMibFastCheck(m_pBegin <= m_pEnd);

		return *this;
	}

	template <typename t_CIteratorData>
	inline_small umint TCVectorIterator<t_CIteratorData>::f_GetLen() const
	{
		return m_pEnd - m_pBegin;
	}

	template <typename t_CIteratorData>
	inline_small t_CIteratorData *TCVectorIterator<t_CIteratorData>::fp_GetCurrent() const
	{
		if (m_pBegin != m_pEnd)
			return m_pBegin;
		else
			return nullptr;
	}
}
