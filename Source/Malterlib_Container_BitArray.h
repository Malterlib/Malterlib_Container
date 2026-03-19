// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <umint t_nBits>
	class TCBitArray
	{
	public:

		constexpr TCBitArray(bool _bInitValues = true);

		void f_Clear();

		template <typename tf_CStream>
		void f_Feed(tf_CStream &_Stream) const;
		template <typename tf_CStream>
		void f_Consume(tf_CStream &_Stream);

		void f_SetBit(umint _Bit, bool _bValue);
		template <bool tf_bValue>
		void f_SetBit(umint _Bit);

		template <bool tf_bValue>
		void f_SetBitRange(umint _StartBit, umint _nBits);
		void f_SetBitRange(umint _StartBit, umint _nBits, bool _bValue);

		template <bool tf_bValue>
		bool f_SetBitRangeWholeEntrySet(umint _StartBit, umint _nBits);

		template <bool tf_bValue>
		bool f_SetBitWholeEntrySet(umint _Bit);
		bool f_SetBitWholeEntrySet(umint _Bit, bool _bValue);

		bool f_GetBit(umint _Bit) const;
		bool f_GetWholeEntrySet(umint _Bit) const;

		aint f_FindFreeBit() const;
		aint f_FindFreeBit(umint _StartBitApproximately) const;
		aint f_FindFreeBitAndSet(umint _StartBitApproximately);

		aint f_FindFreeBitReverse() const;
		aint f_FindFreeBitReverse(umint _StartBitApproximately) const;
		aint f_FindFreeBitReverseAndSet(umint _StartBitApproximately);

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

		aint f_FindFreeBitAndSet();
		aint f_FindFreeBitReverseAndSet();
		aint f_FindUpperBound(umint _StartBit) const;

	private:
		template <bool t_bIsSmaller, typename t_CDummy = void>
		class TCSizeHelper
		{
		public:
			using CType = uaint;
		};

		template <typename t_CDummy>
		class TCSizeHelper<true, t_CDummy>
		{
		public:
			using CType = NTraits::TCUnsigned<NTraits::TCIntFromSizeLarger<(t_nBits + 7) / 8, true>>;
		};

	public:
		using CEntryType = typename TCSizeHelper<(t_nBits < sizeof(uaint)*8)>::CType;

	private:

		aint fp_FindFreeBit(umint &_iEntry) const;
		aint fp_FindFreeBit(umint _StartBitApproximately, umint &_iEntry) const;

		aint fp_FindFreeBitReverse(umint &_iEntry) const;
		aint fp_FindFreeBitReverse(umint _StartBitApproximately, umint &_iEntry) const;

		template <bool tf_bValue>
		CEntryType fp_SetBit(CEntryType _Entry, umint _Bit);
		CEntryType fp_SetBit(CEntryType _Entry, umint _Bit, bool _bValue);
		template <bool tf_bValue>
		CEntryType fp_SetBitRange(CEntryType _Entry, umint _Bit, umint _nBits);

		enum
		{
			ENumBitsPerEntry = sizeof(CEntryType) * 8,
			ENumEntries = (t_nBits + ENumBitsPerEntry - 1) / ENumBitsPerEntry,
		};

	private:
		CEntryType mp_Entries[ENumEntries];


	private:

		static constexpr CEntryType mcp_FullySet = TCLimitsInt<CEntryType>::mc_Max;
		static constexpr CEntryType mcp_FullySetLast = ((CEntryType(1) << (t_nBits - (t_nBits / (ENumBitsPerEntry)) * (ENumBitsPerEntry))) - 1);
		static constexpr CEntryType mcp_FullyUnset = 0;

	};
}

#include "Malterlib_Container_BitArray.hpp"

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
