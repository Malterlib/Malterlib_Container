// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	constexpr TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::TCBitArrayPowerTwo(bool _bInitValues)
		: m_Bits(_bInitValues)
	{
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_Clear()
	{
		m_Bits.f_Clear();
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename tf_CStream>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_Feed(tf_CStream &_Stream) const
	{
		m_Bits.f_Feed(_Stream);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename tf_CStream>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_Consume(tf_CStream &_Stream)
	{
		f_Clear();
		m_Bits.f_Consume(_Stream);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <bool tf_bValue>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_SetBit(umint _Level, umint _Bit)
	{
		m_Bits.template f_SetBit<tf_bValue>(_Level, _Bit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_SetBit(umint _Level, umint _Bit, bool _bValue)
	{
		m_Bits.f_SetBit(_Level, _Bit, _bValue);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_GetBit(umint _Level, umint _Bit) const
	{
		return m_Bits.f_GetBit(_Level, _Bit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBit(umint _Level) const
	{
		return m_Bits.f_FindFreeBit(_Level);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitAndSet(umint _Level)
	{
		return m_Bits.f_FindFreeBitAndSet(_Level);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitReverse(umint _Level) const
	{
		return m_Bits.f_FindFreeBitReverse(_Level);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitReverseAndSet(umint _Level)
	{
		return m_Bits.f_FindFreeBitReverseAndSet(_Level);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindUpperBoundAtLevel(umint _Level, umint _StartBit) const
	{
		return m_Bits.f_FindUpperBoundAtLevel(_Level, _StartBit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_IsFullySet(umint _Level) const
	{
		return m_Bits.f_IsFullySet(_Level);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_IsFullyFree(umint _Level) const
	{
		return m_Bits.f_IsFullyFree(_Level);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename t_CFunctor>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumFreeBits(umint _Level, t_CFunctor &&_Functor, umint _StartBit) const
	{
		m_Bits.f_EnumFreeBits(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, t_nBits >> _Level);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename t_CFunctor>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumFreeBitRanges(umint _Level, t_CFunctor &&_Functor, umint _StartBit) const
	{
		m_Bits.f_EnumFreeBitRanges(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, t_nBits >> _Level);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename t_CFunctor>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumSetBitRanges(umint _Level, t_CFunctor &&_Functor, umint _StartBit) const
	{
		m_Bits.f_EnumSetBitRanges(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, t_nBits >> _Level);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename t_CFunctor>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumFreeBits(umint _Level, t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		m_Bits.f_EnumFreeBits(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename t_CFunctor>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumSetBits(umint _Level, t_CFunctor &&_Functor, umint _StartBit) const
	{
		m_Bits.f_EnumSetBits(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, t_nBits >> _Level);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename t_CFunctor>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumSetBits(umint _Level, t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		m_Bits.f_EnumSetBits(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <bool tf_bValue>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_SetBit(umint _Bit)
	{
		m_Bits.template f_SetBit<tf_bValue>(_Bit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_SetBit(umint _Bit, bool _bValue)
	{
		m_Bits.f_SetBit(_Bit, _bValue);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_GetBit(umint _Bit) const
	{
		return m_Bits.m_Bits.f_GetBit(_Bit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBit() const
	{
		return m_Bits.f_FindFreeBit();
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitAndSet()
	{
		return m_Bits.f_FindFreeBitAndSet();
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitReverse() const
	{
		return m_Bits.f_FindFreeBitReverse();
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitReverseAndSet()
	{
		return m_Bits.f_FindFreeBitReverseAndSet();
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindUpperBound(umint _StartBit) const
	{
		return m_Bits.f_FindUpperBound(_StartBit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_IsFullySet() const
	{
		return m_Bits.f_IsFullySet();
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_IsFullyFree() const
	{
		return m_Bits.f_IsFullyFree();
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename t_CFunctor>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumFreeBits(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		m_Bits.f_EnumFreeBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename t_CFunctor>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumFreeBitRanges(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		m_Bits.f_EnumFreeBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename t_CFunctor>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumSetBitRanges(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		m_Bits.f_EnumSetBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <umint t_nBits, umint t_nMaxLevels, template <umint t_nBits2> class t_TCBitArray>
	template <typename t_CFunctor>
	void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumSetBits(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		m_Bits.f_EnumSetBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}
}
