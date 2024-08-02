// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast = (t_nBitsInternal == 0 || t_nLevel == t_nMaxLevels)>
	struct TCBitArrayPowerTwoInternalArray : public TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal / 2, t_nLevel + 1, t_TCBitArray, t_nMaxLevels>
	{
		enum
		{
			mc_bIsFirstLevel = t_nBitsInternal == t_nBits
			, mc_bIsLastLevelCalc = false
			, mc_bIsLastLevel = TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal / 2, t_nLevel + 1, t_TCBitArray, t_nMaxLevels>::mc_bIsLastLevelCalc
			, mc_nSuperBits = t_nBitsInternal / 2
		};
		typedef TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal / 2, t_nLevel + 1, t_TCBitArray, t_nMaxLevels> CSuper;
		t_TCBitArray<t_nBitsInternal> m_Bits;

		constexpr TCBitArrayPowerTwoInternalArray(bool _bInitValues);

		void f_Clear();

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		bool f_IsFullySet() const;
		bool f_IsFullyFree() const;


		template <bool tf_bValue>
		void f_SetBit(mint _Bit);
		void f_SetBit(mint _Bit, bool _bValue);

		bool f_GetBit(mint _Bit) const;

		aint f_FindFreeBit() const;
		aint f_FindFreeBitAndSet();

		aint f_FindFreeBitReverse() const;
		aint f_FindFreeBitReverseAndSet();

		aint f_FindUpperBound(mint _StartBit) const;

		template <typename t_CFunctor>
		void f_EnumFreeBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumFreeBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumSetBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumSetBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;

		template <bool tf_bValue>
		void f_SetBit(mint _Level, mint _Bit);
		void f_SetBit(mint _Level, mint _Bit, bool _bValue);

		bool f_GetBit(mint _Level, mint _Bit) const;

		aint f_FindFreeBit(mint _Level) const;
		aint f_FindFreeBitAndSet(mint _Level);
		aint f_FindFreeBitReverse(mint _Level) const;
		aint f_FindFreeBitReverseAndSet(mint _Level);
		aint f_FindUpperBoundAtLevel(mint _Level, mint _StartBit) const;

		bool f_IsFullySet(mint _Level) const;
		bool fp_IsFullyFree() const;
		bool f_IsFullyFree(mint _Level) const;

		template <typename t_CFunctor>
		void f_EnumFreeBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumFreeBitRanges(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;

		template <typename t_CFunctor>
		void f_EnumSetBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumSetBitRanges(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;

	};

	template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
	struct TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>
	{
		static constexpr mint mc_nLevels = t_nLevel;
		static constexpr bool mc_bIsLastLevelCalc = true;

		TCBitArrayPowerTwoInternalArray(bool _bInitValues);
		void f_Clear();
		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);
		template <bool tf_bValue>
		void f_SetBit(mint _Bit);
		void f_SetBit(mint _Bit, bool _bValue);
		bool fp_IsFullyFree() const;
		bool f_IsFullySet() const;
		bool f_IsFullyFree() const;
		bool f_GetBit(mint _Bit) const;
		aint f_FindFreeBit() const;
		aint f_FindFreeBitAndSet();
		aint f_FindFreeBitReverse() const;
		aint f_FindFreeBitReverseAndSet();
		aint f_FindUpperBound(mint _StartBit) const;
		void f_SetBit(mint _Level, mint _Bit, bool _bValue);
		template <bool tf_bValue>
		void f_SetBit(mint _Level, mint _Bit);
		bool f_IsFullySet(mint _Level) const;
		bool f_IsFullyFree(mint _Level) const;
		bool f_GetBit(mint _Level, mint _Bit) const;
		aint f_FindFreeBit(mint _Level) const;
		aint f_FindFreeBitAndSet(mint _Level);
		aint f_FindFreeBitReverse(mint _Level) const;
		aint f_FindFreeBitReverseAndSet(mint _Level);
		aint f_FindUpperBoundAtLevel(mint _Level, mint _StartBit) const;
	};
}

#include "Malterlib_Container_BitArrayPowerTwo_Internal.hpp"
