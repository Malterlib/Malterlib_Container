// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer::NPrivate
{
	template
	<
		umint t_nBits
		, umint t_nBitsInternal
		, umint t_nLevel
		, template <umint t_nBits2> class t_TCBitArray
		, umint t_nMaxLevels
		, bool t_bLast = (t_nBitsInternal == 0 || t_nLevel == t_nMaxLevels)
	>
	struct TCBitArrayPowerTwoInternalArray : public TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal / 2, t_nLevel + 1, t_TCBitArray, t_nMaxLevels>
	{
		using CSuper = TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal / 2, t_nLevel + 1, t_TCBitArray, t_nMaxLevels>;

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

		template <typename tf_CFunctor>
		void f_EnumFreeBits(tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename tf_CFunctor>
		void f_EnumFreeBitRanges(tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename tf_CFunctor>
		void f_EnumSetBits(tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename tf_CFunctor>
		void f_EnumSetBitRanges(tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;

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
		bool f_IsFullyFree(umint _Level) const;

		template <typename tf_CFunctor>
		void f_EnumFreeBits(umint _Level, tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename tf_CFunctor>
		void f_EnumFreeBitRanges(umint _Level, tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;

		template <typename tf_CFunctor>
		void f_EnumSetBits(umint _Level, tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename tf_CFunctor>
		void f_EnumSetBitRanges(umint _Level, tf_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;

		static constexpr bool mc_bIsFirstLevel = t_nBitsInternal == t_nBits;
		static constexpr bool mc_bIsLastLevelCalc = false;
		static constexpr bool mc_bIsLastLevel = TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal / 2, t_nLevel + 1, t_TCBitArray, t_nMaxLevels>::mc_bIsLastLevelCalc;
		static constexpr aint mc_nSuperBits = t_nBitsInternal / 2;

		t_TCBitArray<t_nBitsInternal> m_Bits;

	protected:
		bool fp_IsFullyFree() const;
	};

	template <umint t_nBits, umint t_nBitsInternal, umint t_nLevel, template <umint t_nBits2> class t_TCBitArray, umint t_nMaxLevels>
	struct TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>
	{
		TCBitArrayPowerTwoInternalArray(bool _bInitValues);
		void f_Clear();
		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);
		template <bool tf_bValue>
		void f_SetBit(umint _Bit);
		void f_SetBit(umint _Bit, bool _bValue);
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

		static constexpr umint mc_nLevels = t_nLevel;
		static constexpr bool mc_bIsLastLevelCalc = true;

	protected:
		bool fp_IsFullyFree() const;
	};
}

#include "Malterlib_Container_BitArrayPowerTwo_Internal.hpp"
