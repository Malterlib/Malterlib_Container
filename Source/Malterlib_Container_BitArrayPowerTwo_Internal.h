// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <umint t_nBits, umint t_nBitsInternal, umint t_nLevel, template <umint t_nBits2> class t_TCBitArray, umint t_nMaxLevels, bool t_bLast = (t_nBitsInternal == 0 || t_nLevel == t_nMaxLevels)>
	struct TCBitArrayPowerTwoInternalArray : public TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal / 2, t_nLevel + 1, t_TCBitArray, t_nMaxLevels>
	{
		enum
		{
			mc_bIsFirstLevel = t_nBitsInternal == t_nBits
			, mc_bIsLastLevelCalc = false
			, mc_bIsLastLevel = TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal / 2, t_nLevel + 1, t_TCBitArray, t_nMaxLevels>::mc_bIsLastLevelCalc
			, mc_nSuperBits = t_nBitsInternal / 2
		};

		using CSuper = TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal / 2, t_nLevel + 1, t_TCBitArray, t_nMaxLevels>;

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
		void f_SetBit(umint _Bit);
		void f_SetBit(umint _Bit, bool _bValue);

		bool f_GetBit(umint _Bit) const;

		aint f_FindFreeBit() const;
		aint f_FindFreeBitAndSet();

		aint f_FindFreeBitReverse() const;
		aint f_FindFreeBitReverseAndSet();

		aint f_FindUpperBound(umint _StartBit) const;

		template <typename t_CFunctor>
		void f_EnumFreeBits(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumFreeBitRanges(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumSetBits(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumSetBitRanges(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;

		template <bool tf_bValue>
		void f_SetBit(umint _Level, umint _Bit);
		void f_SetBit(umint _Level, umint _Bit, bool _bValue);

		bool f_GetBit(umint _Level, umint _Bit) const;

		aint f_FindFreeBit(umint _Level) const;
		aint f_FindFreeBitAndSet(umint _Level);
		aint f_FindFreeBitReverse(umint _Level) const;
		aint f_FindFreeBitReverseAndSet(umint _Level);
		aint f_FindUpperBoundAtLevel(umint _Level, umint _StartBit) const;

		bool f_IsFullySet(umint _Level) const;
		bool fp_IsFullyFree() const;
		bool f_IsFullyFree(umint _Level) const;

		template <typename t_CFunctor>
		void f_EnumFreeBits(umint _Level, t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumFreeBitRanges(umint _Level, t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;

		template <typename t_CFunctor>
		void f_EnumSetBits(umint _Level, t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumSetBitRanges(umint _Level, t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;

	};

	template <umint t_nBits, umint t_nBitsInternal, umint t_nLevel, template <umint t_nBits2> class t_TCBitArray, umint t_nMaxLevels>
	struct TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>
	{
		static constexpr umint mc_nLevels = t_nLevel;
		static constexpr bool mc_bIsLastLevelCalc = true;

		TCBitArrayPowerTwoInternalArray(bool _bInitValues);
		void f_Clear();
		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);
		template <bool tf_bValue>
		void f_SetBit(umint _Bit);
		void f_SetBit(umint _Bit, bool _bValue);
		bool fp_IsFullyFree() const;
		bool f_IsFullySet() const;
		bool f_IsFullyFree() const;
		bool f_GetBit(umint _Bit) const;
		aint f_FindFreeBit() const;
		aint f_FindFreeBitAndSet();
		aint f_FindFreeBitReverse() const;
		aint f_FindFreeBitReverseAndSet();
		aint f_FindUpperBound(umint _StartBit) const;
		void f_SetBit(umint _Level, umint _Bit, bool _bValue);
		template <bool tf_bValue>
		void f_SetBit(umint _Level, umint _Bit);
		bool f_IsFullySet(umint _Level) const;
		bool f_IsFullyFree(umint _Level) const;
		bool f_GetBit(umint _Level, umint _Bit) const;
		aint f_FindFreeBit(umint _Level) const;
		aint f_FindFreeBitAndSet(umint _Level);
		aint f_FindFreeBitReverse(umint _Level) const;
		aint f_FindFreeBitReverseAndSet(umint _Level);
		aint f_FindUpperBoundAtLevel(umint _Level, umint _StartBit) const;
	};
}

#include "Malterlib_Container_BitArrayPowerTwo_Internal.hpp"
