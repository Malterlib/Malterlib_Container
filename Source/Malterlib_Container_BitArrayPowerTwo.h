// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Container_BitArrayHierarchical.h"
#include "Malterlib_Container_BitArrayPowerTwo_Internal.h"

namespace NMib::NContainer
{
	template <umint t_nBits, umint t_nMaxLevels = 64, template <umint t_nBits2> class t_TCBitArray = TCBitArrayHierarchical>
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
		void f_SetBit(umint _Level, umint _Bit);
		void f_SetBit(umint _Level, umint _Bit, bool _bValue);
		bool f_GetBit(umint _Level, umint _Bit) const;

		aint f_FindFreeBit(umint _Level) const;
		aint f_FindFreeBitAndSet(umint _Level);
		aint f_FindFreeBitReverse(umint _Level) const;
		aint f_FindFreeBitReverseAndSet(umint _Level);
		aint f_FindUpperBoundAtLevel(umint _Level, umint _StartBit = 0) const;

		bool f_IsFullySet(umint _Level) const;
		bool f_IsFullyFree(umint _Level) const;

		template <typename t_CFunctor>
		void f_EnumFreeBits(umint _Level, t_CFunctor &&_Functor, umint _StartBit = 0) const;
		template <typename t_CFunctor>
		void f_EnumFreeBits(umint _Level, t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumFreeBitRanges(umint _Level, t_CFunctor &&_Functor, umint _StartBit = 0) const;

		template <typename t_CFunctor>
		void f_EnumSetBits(umint _Level, t_CFunctor &&_Functor, umint _StartBit = 0) const;
		template <typename t_CFunctor>
		void f_EnumSetBits(umint _Level, t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
		template <typename t_CFunctor>
		void f_EnumSetBitRanges(umint _Level, t_CFunctor &&_Functor, umint _StartBit = 0) const;

		template <bool tf_bValue>
		void f_SetBit(umint _Bit);
		void f_SetBit(umint _Bit, bool _bValue);

		bool f_GetBit(umint _Bit) const;

		aint f_FindFreeBit() const;
		aint f_FindFreeBitAndSet();
		aint f_FindFreeBitReverse() const;
		aint f_FindFreeBitReverseAndSet();
		aint f_FindUpperBound(umint _StartBit = 0) const;

		bool f_IsFullySet() const;
		bool f_IsFullyFree() const;

		template <typename t_CFunctor>
		void f_EnumFreeBits(t_CFunctor &&_Functor, umint _StartBit = 0, umint _EndBit = t_nBits) const;
		template <typename t_CFunctor>
		void f_EnumFreeBitRanges(t_CFunctor &&_Functor, umint _StartBit = 0, umint _EndBit = t_nBits) const;

		template <typename t_CFunctor>
		void f_EnumSetBits(t_CFunctor &&_Functor, umint _StartBit = 0, umint _EndBit = t_nBits) const;
		template <typename t_CFunctor>
		void f_EnumSetBitRanges(t_CFunctor &&_Functor, umint _StartBit = 0, umint _EndBit = t_nBits) const;

	private:
		using CInternal = NPrivate::TCBitArrayPowerTwoInternalArray<t_nBits, t_nBits, 0, t_TCBitArray, t_nMaxLevels>;

		CInternal m_Bits;
		static constexpr umint mc_nLevels = CInternal::mc_nLevels;
	};
}

#include "Malterlib_Container_BitArrayPowerTwo.hpp"

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
