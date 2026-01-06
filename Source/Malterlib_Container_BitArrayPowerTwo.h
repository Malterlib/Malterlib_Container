// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Container_BitArrayHierarchical.h"
#include "Malterlib_Container_BitArrayPowerTwo_Internal.h"

namespace NMib::NContainer
{
	template <mint t_nBits, mint t_nMaxLevels = 64, template <mint t_nBits2> class t_TCBitArray = TCBitArrayHierarchical>
	class TCBitArrayPowerTwo
	{
	public:
		constexpr TCBitArrayPowerTwo(bool _bInitValues = true);
		void f_Clear();
		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);
		template <bool tf_bValue>
		void f_SetBit(mint _Level, mint _Bit);
		void f_SetBit(mint _Level, mint _Bit, bool _bValue);
		bool f_GetBit(mint _Level, mint _Bit) const;

		aint f_FindFreeBit(mint _Level) const;
		aint f_FindFreeBitAndSet(mint _Level);
		aint f_FindFreeBitReverse(mint _Level) const;
		aint f_FindFreeBitReverseAndSet(mint _Level);
		aint f_FindUpperBoundAtLevel(mint _Level, mint _StartBit = 0) const;

		bool f_IsFullySet(mint _Level) const;
		bool f_IsFullyFree(mint _Level) const;

		template <typename t_CFunctor>
		void f_EnumFreeBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit = 0) const;
		template <typename t_CFunctor>
		void f_EnumFreeBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumFreeBitRanges(mint _Level, t_CFunctor &&_Functor, mint _StartBit = 0) const;

		template <typename t_CFunctor>
		void f_EnumSetBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit = 0) const;
		template <typename t_CFunctor>
		void f_EnumSetBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumSetBitRanges(mint _Level, t_CFunctor &&_Functor, mint _StartBit = 0) const;

		template <bool tf_bValue>
		void f_SetBit(mint _Bit);
		void f_SetBit(mint _Bit, bool _bValue);

		bool f_GetBit(mint _Bit) const;

		aint f_FindFreeBit() const;
		aint f_FindFreeBitAndSet();
		aint f_FindFreeBitReverse() const;
		aint f_FindFreeBitReverseAndSet();
		aint f_FindUpperBound(mint _StartBit = 0) const;

		bool f_IsFullySet() const;
		bool f_IsFullyFree() const;

		template <typename t_CFunctor>
		void f_EnumFreeBits(t_CFunctor &&_Functor, mint _StartBit = 0, mint _EndBit = t_nBits) const;
		template <typename t_CFunctor>
		void f_EnumFreeBitRanges(t_CFunctor &&_Functor, mint _StartBit = 0, mint _EndBit = t_nBits) const;

		template <typename t_CFunctor>
		void f_EnumSetBits(t_CFunctor &&_Functor, mint _StartBit = 0, mint _EndBit = t_nBits) const;
		template <typename t_CFunctor>
		void f_EnumSetBitRanges(t_CFunctor &&_Functor, mint _StartBit = 0, mint _EndBit = t_nBits) const;

	private:
		using CInternal = NPrivate::TCBitArrayPowerTwoInternalArray<t_nBits, t_nBits, 0, t_TCBitArray, t_nMaxLevels>;

		CInternal m_Bits;
		static constexpr mint mc_nLevels = CInternal::mc_nLevels;
	};
}

#include "Malterlib_Container_BitArrayPowerTwo.hpp"

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
