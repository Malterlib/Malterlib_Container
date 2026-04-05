// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer::NPrivate
{
	template <umint t_nBitsPerEntry, umint t_nBits, umint t_nBitsInternal, bool t_bLast = (t_nBitsInternal == 0)>
	struct TCBitArrayHierarchicalInternalArray : public TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal == 1 ? 0 : ((t_nBitsInternal + t_nBitsPerEntry - 1) / t_nBitsPerEntry)>
	{
		enum
		{
			mc_bIsRoot = (t_nBitsInternal <= t_nBitsPerEntry)
			, mc_bIsFirstLevel = t_nBitsInternal == t_nBits
			, mc_nSuperBits = t_nBitsInternal == 1 ? 0 : ((t_nBitsInternal + t_nBitsPerEntry - 1) / t_nBitsPerEntry)
		};

		using CSuper = TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal == 1 ? 0 : ((t_nBitsInternal + t_nBitsPerEntry - 1) / t_nBitsPerEntry)>;

		TCBitArray<t_nBitsInternal> m_Bits;

		constexpr TCBitArrayHierarchicalInternalArray(bool _bInitValues);
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

		template <bool tf_bValue>
		void f_SetBitRange(umint _StartBit, umint _nBits);
		void f_SetBitRange(umint _StartBit, umint _nBits, bool _bValue);

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
		void f_EnumSetBitRanges(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;

		template <typename t_CFunctor>
		void f_EnumSetBits(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const;
	};

	template <umint t_nBitsPerEntry, umint t_nBits, umint t_nBitsInternal>
	struct TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>
	{
		constexpr TCBitArrayHierarchicalInternalArray(bool _bInitValues);
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

		bool f_IsFullySet() const;
		bool f_IsFullyFree() const;

		bool f_GetBit(umint _Bit) const;

		aint f_FindFreeBit() const;
		aint f_FindFreeBitAndSet();

		aint f_FindFreeBitReverse() const;
		aint f_FindFreeBitReverseAndSet();

		aint f_FindUpperBound(umint _StartBit) const;
	};
}

#include "Malterlib_Container_BitArrayHierarchical_Internal.hpp"
