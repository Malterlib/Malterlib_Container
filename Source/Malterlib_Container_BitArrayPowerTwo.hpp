// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib
{
	namespace NContainer
	{
		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::TCBitArrayPowerTwo(bool _bInitValues)
			: m_Bits(_bInitValues)
		{
		}
		
		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_Clear()
		{
			m_Bits.f_Clear();
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename tf_CStream>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_Feed(tf_CStream &_Stream) const
		{
			m_Bits.f_Feed(_Stream);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename tf_CStream>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_Consume(tf_CStream &_Stream)
		{
			f_Clear();
			m_Bits.f_Consume(_Stream);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <bool tf_bValue>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_SetBit(mint _Level, mint _Bit)
		{
			m_Bits.template f_SetBit<tf_bValue>(_Level, _Bit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_SetBit(mint _Level, mint _Bit, bool _bValue)
		{
			m_Bits.f_SetBit(_Level, _Bit, _bValue);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_GetBit(mint _Level, mint _Bit) const
		{
			return m_Bits.f_GetBit(_Level, _Bit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBit(mint _Level) const
		{
			return m_Bits.f_FindFreeBit(_Level);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitAndSet(mint _Level)
		{
			return m_Bits.f_FindFreeBitAndSet(_Level);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitReverse(mint _Level) const
		{
			return m_Bits.f_FindFreeBitReverse(_Level);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitReverseAndSet(mint _Level)
		{
			return m_Bits.f_FindFreeBitReverseAndSet(_Level);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindUpperBoundAtLevel(mint _Level, mint _StartBit) const
		{
			return m_Bits.f_FindUpperBoundAtLevel(_Level, _StartBit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_IsFullySet(mint _Level) const
		{
			return m_Bits.f_IsFullySet(_Level);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_IsFullyFree(mint _Level) const
		{
			return m_Bits.f_IsFullyFree(_Level);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename t_CFunctor>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumFreeBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit) const
		{
			m_Bits.f_EnumFreeBits(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, t_nBits >> _Level);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename t_CFunctor>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumFreeBitRanges(mint _Level, t_CFunctor &&_Functor, mint _StartBit) const
		{
			m_Bits.f_EnumFreeBitRanges(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, t_nBits >> _Level);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename t_CFunctor>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumSetBitRanges(mint _Level, t_CFunctor &&_Functor, mint _StartBit) const
		{
			m_Bits.f_EnumSetBitRanges(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, t_nBits >> _Level);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename t_CFunctor>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumFreeBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
		{
			m_Bits.f_EnumFreeBits(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename t_CFunctor>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumSetBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit) const
		{
			m_Bits.f_EnumSetBits(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, t_nBits >> _Level);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename t_CFunctor>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumSetBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
		{
			m_Bits.f_EnumSetBits(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <bool tf_bValue>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_SetBit(mint _Bit)
		{
			m_Bits.template f_SetBit<tf_bValue>(_Bit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_SetBit(mint _Bit, bool _bValue)
		{
			m_Bits.f_SetBit(_Bit, _bValue);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_GetBit(mint _Bit) const
		{
			return m_Bits.m_Bits.f_GetBit(_Bit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBit() const
		{
			return m_Bits.f_FindFreeBit();
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitAndSet()
		{
			return m_Bits.f_FindFreeBitAndSet();
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitReverse() const
		{
			return m_Bits.f_FindFreeBitReverse();
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindFreeBitReverseAndSet()
		{
			return m_Bits.f_FindFreeBitReverseAndSet();
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		aint TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_FindUpperBound(mint _StartBit) const
		{
			return m_Bits.f_FindUpperBound(_StartBit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_IsFullySet() const
		{
			return m_Bits.f_IsFullySet();
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		bool TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_IsFullyFree() const
		{
			return m_Bits.f_IsFullyFree();
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename t_CFunctor>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumFreeBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
		{
			m_Bits.f_EnumFreeBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename t_CFunctor>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumFreeBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
		{
			m_Bits.f_EnumFreeBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename t_CFunctor>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumSetBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
		{
			m_Bits.f_EnumSetBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
		}

		template <mint t_nBits, mint t_nMaxLevels, template <mint t_nBits2> class t_TCBitArray>
		template <typename t_CFunctor>
		void TCBitArrayPowerTwo<t_nBits, t_nMaxLevels, t_TCBitArray>::f_EnumSetBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
		{
			m_Bits.f_EnumSetBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
		}
		
	}
}

