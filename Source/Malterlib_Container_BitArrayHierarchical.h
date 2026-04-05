// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "Malterlib_Container_BitArray.h"

#include "Malterlib_Container_BitArrayHierarchical_Internal.h"

namespace NMib::NContainer
{
	template <umint t_nBits>
	class TCBitArrayHierarchical
	{
	public:
		using CEntryType = typename TCBitArray<t_nBits>::CEntryType;
	public:

		constexpr TCBitArrayHierarchical(bool _bInitValues = true);
		void f_Clear();

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		template <bool tf_bValue>
		void f_SetBit(umint _Bit);
		void f_SetBit(umint _Bit, bool _bValue);

		template <bool tf_bValue>
		void f_SetBitRange(umint _StartBit, umint _nBits);
		void f_SetBitRange(umint _StartBit, umint _nBits, bool _bValue);

		bool f_GetBit(umint _Bit) const;

		aint f_FindFreeBit() const;
		aint f_FindFreeBitAndSet();
		aint f_FindUpperBound(umint _StartBit) const;

		aint f_FindFreeBitReverse() const;
		aint f_FindFreeBitReverseAndSet();

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
		enum
		{
			EBitsPerEntry = sizeof(CEntryType) * 8
		};

		NPrivate::TCBitArrayHierarchicalInternalArray<EBitsPerEntry, t_nBits, t_nBits> m_Bits;
	};
}

#include "Malterlib_Container_BitArrayHierarchical.hpp"

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
