// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <mint t_nBits>
	constexpr TCBitArrayHierarchical<t_nBits>::TCBitArrayHierarchical(bool _bInitValues)
		: m_Bits(_bInitValues)
	{
	}

	template <mint t_nBits>
	void TCBitArrayHierarchical<t_nBits>::f_Clear()
	{
		m_Bits.f_Clear();
	}

	template <mint t_nBits>
	template <typename tf_CStream>
	void TCBitArrayHierarchical<t_nBits>::f_Feed(tf_CStream &_Stream) const
	{
		m_Bits.f_Feed(_Stream);
	}

	template <mint t_nBits>
	template <typename tf_CStream>
	void TCBitArrayHierarchical<t_nBits>::f_Consume(tf_CStream &_Stream)
	{
		f_Clear();
		m_Bits.f_Consume(_Stream);
	}

	template <mint t_nBits>
	template <bool tf_bValue>
	inline_small void TCBitArrayHierarchical<t_nBits>::f_SetBit(mint _Bit)
	{
		m_Bits.template f_SetBit<tf_bValue>(_Bit);
	}

	template <mint t_nBits>
	template <bool tf_bValue>
	inline_small void TCBitArrayHierarchical<t_nBits>::f_SetBitRange(mint _StartBit, mint _nBits)
	{
		m_Bits.template f_SetBitRange<tf_bValue>(_StartBit, _nBits);
	}

	template <mint t_nBits>
	inline_small void TCBitArrayHierarchical<t_nBits>::f_SetBitRange(mint _StartBit, mint _nBits, bool _bValue)
	{
		if (_bValue)
			f_SetBitRange<true>(_StartBit, _nBits);
		else
			f_SetBitRange<false>(_StartBit, _nBits);
	}

	template <mint t_nBits>
	inline_small void TCBitArrayHierarchical<t_nBits>::f_SetBit(mint _Bit, bool _bValue)
	{
		m_Bits.f_SetBit(_Bit, _bValue);
	}

	template <mint t_nBits>
	inline_small bool TCBitArrayHierarchical<t_nBits>::f_GetBit(mint _Bit) const
	{
		return m_Bits.m_Bits.f_GetBit(_Bit);
	}

	template <mint t_nBits>
	inline_small aint TCBitArrayHierarchical<t_nBits>::f_FindFreeBit() const
	{
		return m_Bits.f_FindFreeBit();
	}

	template <mint t_nBits>
	inline_small aint TCBitArrayHierarchical<t_nBits>::f_FindFreeBitAndSet()
	{
		return m_Bits.f_FindFreeBitAndSet();
	}

	template <mint t_nBits>
	inline_small aint TCBitArrayHierarchical<t_nBits>::f_FindFreeBitReverse() const
	{
		return m_Bits.f_FindFreeBitReverse();
	}

	template <mint t_nBits>
	inline_small aint TCBitArrayHierarchical<t_nBits>::f_FindFreeBitReverseAndSet()
	{
		return m_Bits.f_FindFreeBitReverseAndSet();
	}

	template <mint t_nBits>
	aint TCBitArrayHierarchical<t_nBits>::f_FindUpperBound(mint _StartBit) const
	{
		return m_Bits.f_FindUpperBound(_StartBit);
	}

	template <mint t_nBits>
	bool TCBitArrayHierarchical<t_nBits>::f_IsFullySet() const
	{
		return m_Bits.f_IsFullySet();
	}

	template <mint t_nBits>
	bool TCBitArrayHierarchical<t_nBits>::f_IsFullyFree() const
	{
		return m_Bits.m_Bits.f_IsFullyFree();
	}

	template <mint t_nBits>
	template <typename t_CFunctor>
	void TCBitArrayHierarchical<t_nBits>::f_EnumFreeBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
	{
		m_Bits.f_EnumFreeBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <mint t_nBits>
	template <typename t_CFunctor>
	void TCBitArrayHierarchical<t_nBits>::f_EnumFreeBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
	{
		m_Bits.f_EnumFreeBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <mint t_nBits>
	template <typename t_CFunctor>
	void TCBitArrayHierarchical<t_nBits>::f_EnumSetBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
	{
		m_Bits.f_EnumSetBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <mint t_nBits>
	template <typename t_CFunctor>
	void TCBitArrayHierarchical<t_nBits>::f_EnumSetBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
	{
		m_Bits.f_EnumSetBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}
}
