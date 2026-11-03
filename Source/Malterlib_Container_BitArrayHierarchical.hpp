// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <umint t_nBits>
	constexpr TCBitArrayHierarchical<t_nBits>::TCBitArrayHierarchical(bool _bInitValues)
		: mp_Bits(_bInitValues)
	{
	}

	template <umint t_nBits>
	void TCBitArrayHierarchical<t_nBits>::f_Clear()
	{
		mp_Bits.f_Clear();
	}

	template <umint t_nBits>
	template <typename tf_CStream>
	void TCBitArrayHierarchical<t_nBits>::f_Feed(tf_CStream &_Stream) const
	{
		mp_Bits.f_Feed(_Stream);
	}

	template <umint t_nBits>
	template <typename tf_CStream>
	void TCBitArrayHierarchical<t_nBits>::f_Consume(tf_CStream &_Stream)
	{
		f_Clear();
		mp_Bits.f_Consume(_Stream);
	}

	template <umint t_nBits>
	template <bool tf_bValue>
	inline_small void TCBitArrayHierarchical<t_nBits>::f_SetBit(umint _Bit)
	{
		mp_Bits.template f_SetBit<tf_bValue>(_Bit);
	}

	template <umint t_nBits>
	template <bool tf_bValue>
	inline_small void TCBitArrayHierarchical<t_nBits>::f_SetBitRange(umint _StartBit, umint _nBits)
	{
		mp_Bits.template f_SetBitRange<tf_bValue>(_StartBit, _nBits);
	}

	template <umint t_nBits>
	inline_small void TCBitArrayHierarchical<t_nBits>::f_SetBitRange(umint _StartBit, umint _nBits, bool _bValue)
	{
		if (_bValue)
			f_SetBitRange<true>(_StartBit, _nBits);
		else
			f_SetBitRange<false>(_StartBit, _nBits);
	}

	template <umint t_nBits>
	inline_small void TCBitArrayHierarchical<t_nBits>::f_SetBit(umint _Bit, bool _bValue)
	{
		mp_Bits.f_SetBit(_Bit, _bValue);
	}

	template <umint t_nBits>
	inline_small bool TCBitArrayHierarchical<t_nBits>::f_GetBit(umint _Bit) const
	{
		return mp_Bits.m_Bits.f_GetBit(_Bit);
	}

	template <umint t_nBits>
	inline_small aint TCBitArrayHierarchical<t_nBits>::f_FindFreeBit() const
	{
		return mp_Bits.f_FindFreeBit();
	}

	template <umint t_nBits>
	inline_small aint TCBitArrayHierarchical<t_nBits>::f_FindFreeBitAndSet()
	{
		return mp_Bits.f_FindFreeBitAndSet();
	}

	template <umint t_nBits>
	inline_small aint TCBitArrayHierarchical<t_nBits>::f_FindFreeBitReverse() const
	{
		return mp_Bits.f_FindFreeBitReverse();
	}

	template <umint t_nBits>
	inline_small aint TCBitArrayHierarchical<t_nBits>::f_FindFreeBitReverseAndSet()
	{
		return mp_Bits.f_FindFreeBitReverseAndSet();
	}

	template <umint t_nBits>
	aint TCBitArrayHierarchical<t_nBits>::f_FindUpperBound(umint _StartBit) const
	{
		return mp_Bits.f_FindUpperBound(_StartBit);
	}

	template <umint t_nBits>
	bool TCBitArrayHierarchical<t_nBits>::f_IsFullySet() const
	{
		return mp_Bits.f_IsFullySet();
	}

	template <umint t_nBits>
	bool TCBitArrayHierarchical<t_nBits>::f_IsFullyFree() const
	{
		return mp_Bits.m_Bits.f_IsFullyFree();
	}

	template <umint t_nBits>
	template <typename tf_CFunctor>
	void TCBitArrayHierarchical<t_nBits>::f_EnumFreeBits(tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		mp_Bits.f_EnumFreeBits(fg_Forward<tf_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <umint t_nBits>
	template <typename tf_CFunctor>
	void TCBitArrayHierarchical<t_nBits>::f_EnumFreeBitRanges(tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		mp_Bits.f_EnumFreeBitRanges(fg_Forward<tf_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <umint t_nBits>
	template <typename tf_CFunctor>
	void TCBitArrayHierarchical<t_nBits>::f_EnumSetBitRanges(tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		mp_Bits.f_EnumSetBitRanges(fg_Forward<tf_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <umint t_nBits>
	template <typename tf_CFunctor>
	void TCBitArrayHierarchical<t_nBits>::f_EnumSetBits(tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		mp_Bits.f_EnumSetBits(fg_Forward<tf_CFunctor>(_Functor), _StartBit, _EndBit);
	}
}
