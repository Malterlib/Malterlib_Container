// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Container_BitArray.h"

namespace NMib::NContainer::NPrivate
{
	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	constexpr TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::TCBitArrayHierarchicalInternalArray(bool _bInitValues)
		: CSuper(_bInitValues)
		, m_Bits(_bInitValues)
	{
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_Clear()
	{
		CSuper::f_Clear();
		m_Bits.f_Clear();
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	template <typename tf_CStream>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_Feed(tf_CStream &_Stream) const
	{
		if (mc_bIsFirstLevel)
			_Stream << m_Bits;
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small bool TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_IsFullySet() const
	{
		if (mc_bIsRoot)
		{
			return m_Bits.f_IsFullySet();
		}
		return CSuper::f_IsFullySet();
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small bool TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_IsFullyFree() const
	{
		if (mc_bIsRoot)
		{
			return m_Bits.f_IsFullyFree();
		}
		return CSuper::f_IsFullyFree();
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	template <typename tf_CStream>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_Consume(tf_CStream &_Stream)
	{
		if (mc_bIsFirstLevel)
		{
			CSuper::f_Clear();
			_Stream >> m_Bits;
			for (mint i = 0; i < mc_nSuperBits; ++i)
			{
				if (m_Bits.f_GetWholeEntrySet(i * t_nBitsPerEntry))
					CSuper::f_SetBit<true>(i);
			}
		}
	}


	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	template <bool tf_bValue>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_SetBit(mint _Bit)
	{
		if (m_Bits.template f_SetBitWholeEntrySet<tf_bValue>(_Bit) == tf_bValue)
			CSuper::template f_SetBit<tf_bValue>(_Bit / t_nBitsPerEntry);
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	template <bool tf_bValue>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_SetBitRange(mint _StartBit, mint _nBits)
	{
		mint StartEntry = _StartBit / t_nBitsPerEntry;
		mint StartEntryBit = StartEntry * t_nBitsPerEntry;
		mint nFirst = fg_Min(t_nBitsPerEntry - (_StartBit - StartEntryBit), _nBits);
		DMibFastCheck(nFirst);

		if (m_Bits.template f_SetBitRangeWholeEntrySet<tf_bValue>(_StartBit, nFirst) == tf_bValue)
			CSuper::template f_SetBit<tf_bValue>(StartEntry);

		_nBits -= nFirst;
		StartEntryBit += t_nBitsPerEntry;
		++StartEntry;

		while (_nBits)
		{
			mint nBitsToSet = fg_Min(_nBits, mint(t_nBitsPerEntry));
			if (m_Bits.template f_SetBitRangeWholeEntrySet<tf_bValue>(StartEntryBit, nBitsToSet) == tf_bValue)
				CSuper::template f_SetBit<tf_bValue>(StartEntry);
			StartEntryBit += nBitsToSet;
			_nBits -= nBitsToSet;
			++StartEntry;
		}
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_SetBitRange(mint _StartBit, mint _nBits, bool _bValue)
	{
		if (_bValue)
			f_SetBitRange<true>(_StartBit, _nBits);
		else
			f_SetBitRange<false>(_StartBit, _nBits);
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_SetBit(mint _Bit, bool _bValue)
	{
		if (_bValue)
		{
			if (m_Bits.f_SetBitWholeEntrySet(_Bit, true))
				CSuper::template f_SetBit<true>(_Bit / t_nBitsPerEntry);
		}
		else
		{
			if (!m_Bits.f_SetBitWholeEntrySet(_Bit, false))
				CSuper::template f_SetBit<false>(_Bit / t_nBitsPerEntry);
		}
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small bool TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_GetBit(mint _Bit) const
	{
		return m_Bits.f_GetBit(_Bit);
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small aint TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_FindFreeBit() const
	{
		aint Free = CSuper::f_FindFreeBit();
		if (Free < 0)
			return -1;
		aint Bit = m_Bits.f_FindFreeBit(Free * t_nBitsPerEntry);
		return Bit;
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small aint TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_FindFreeBitAndSet()
	{
		aint Free = CSuper::f_FindFreeBit();
		if (Free < 0)
			return -1;
		aint Bit = m_Bits.f_FindFreeBitAndSet(Free * t_nBitsPerEntry);
		if (mc_bIsFirstLevel && Bit >= 0)
		{
			if (m_Bits.f_GetWholeEntrySet(Bit))
				CSuper::template f_SetBit<true>(Bit / t_nBitsPerEntry);
		}
		return Bit;
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small aint TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_FindFreeBitReverse() const
	{
		aint Free = CSuper::f_FindFreeBitReverse();
		if (Free < 0)
			return -1;
		aint Bit = m_Bits.f_FindFreeBitReverse(Free * t_nBitsPerEntry);
		return Bit;
	}


	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small aint TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_FindFreeBitReverseAndSet()
	{
		aint Free = CSuper::f_FindFreeBitReverse();
		if (Free < 0)
			return -1;
		aint Bit = m_Bits.f_FindFreeBitReverseAndSet(Free * t_nBitsPerEntry);
		if (mc_bIsFirstLevel && Bit >= 0)
		{
			if (m_Bits.f_GetWholeEntrySet(Bit))
				CSuper::template f_SetBit<true>(Bit / t_nBitsPerEntry);
		}
		return Bit;
	}


	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	inline_small aint TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_FindUpperBound(mint _StartBit) const
	{
		return m_Bits.f_FindUpperBound(_StartBit);
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	template <typename t_CFunctor>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_EnumFreeBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
	{
		return m_Bits.f_EnumFreeBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	template <typename t_CFunctor>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_EnumFreeBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
	{
		return m_Bits.f_EnumFreeBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	template <typename t_CFunctor>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_EnumSetBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
	{
		return m_Bits.f_EnumSetBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast>
	template <typename t_CFunctor>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, t_bLast>::f_EnumSetBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
	{
		return m_Bits.f_EnumSetBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
	}

	///
	/// Root
	/// ====

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	constexpr inline_small TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::TCBitArrayHierarchicalInternalArray(bool _bInitValues)
	{
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_Clear()
	{
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	template <typename tf_CStream>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_Feed(tf_CStream &_Stream) const
	{
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	template <typename tf_CStream>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_Consume(tf_CStream &_Stream)
	{
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	template <bool tf_bValue>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_SetBit(mint _Bit)
	{
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	template <bool tf_bValue>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_SetBitRange(mint _StartBit, mint _nBits)
	{
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_SetBitRange(mint _StartBit, mint _nBits, bool _bValue)
	{
	}


	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small void TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_SetBit(mint _Bit, bool _bValue)
	{
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small bool TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_IsFullySet() const
	{
		return false;
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small bool TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_IsFullyFree() const
	{
		return true;
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small bool TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_GetBit(mint _Bit) const
	{
		return false;
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small aint TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_FindFreeBit() const
	{
		return 0;
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small aint TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_FindFreeBitAndSet()
	{
		return 0;
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small aint TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_FindFreeBitReverse() const
	{
		return 0;
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small aint TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_FindFreeBitReverseAndSet()
	{
		return 0;
	}

	template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
	inline_small aint TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>::f_FindUpperBound(mint _StartBit) const
	{
		return 0;
	}
}
