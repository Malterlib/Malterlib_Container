// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib
{
	namespace NContainer
	{
		namespace NPrivate
		{
			template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal, bool t_bLast = t_nBitsInternal <= 1>
			struct TCBitArrayHierarchicalInternalArray : public TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, (t_nBitsInternal + t_nBitsPerEntry - 1) / t_nBitsPerEntry>
			{
				enum
				{
					mc_bIsRoot = t_nBitsInternal <= t_nBitsPerEntry
					, mc_bIsFirstLevel = t_nBitsInternal == t_nBits
					, mc_nSuperBits = (t_nBitsInternal + t_nBitsPerEntry - 1) / t_nBitsPerEntry
				};
				typedef TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, (t_nBitsInternal + t_nBitsPerEntry - 1) / t_nBitsPerEntry> CSuper;
				TCBitArray<t_nBitsInternal> m_Bits;
				
				TCBitArrayHierarchicalInternalArray(bool _bInitValues);
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

				template <bool tf_bValue>
				void f_SetBitRange(mint _StartBit, mint _nBits);
				void f_SetBitRange(mint _StartBit, mint _nBits, bool _bValue);

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
				void f_EnumSetBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;

				template <typename t_CFunctor>
				void f_EnumSetBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const;
			};
			
			template <mint t_nBitsPerEntry, mint t_nBits, mint t_nBitsInternal>
			struct TCBitArrayHierarchicalInternalArray<t_nBitsPerEntry, t_nBits, t_nBitsInternal, true>
			{
				TCBitArrayHierarchicalInternalArray(bool _bInitValues);
				void f_Clear();
				
				template <typename tf_CStream>
				void f_Feed(tf_CStream &_Stream) const;
				template <typename tf_CStream>
				void f_Consume(tf_CStream &_Stream);
				
				template <bool tf_bValue>
				void f_SetBit(mint _Bit);
				void f_SetBit(mint _Bit, bool _bValue);

				template <bool tf_bValue>
				void f_SetBitRange(mint _StartBit, mint _nBits);
				void f_SetBitRange(mint _StartBit, mint _nBits, bool _bValue);
				
				bool f_IsFullySet() const;
				bool f_IsFullyFree() const;
				
				bool f_GetBit(mint _Bit) const;
				
				aint f_FindFreeBit() const;
				aint f_FindFreeBitAndSet();
				
				aint f_FindFreeBitReverse() const;
				aint f_FindFreeBitReverseAndSet();
				
				aint f_FindUpperBound(mint _StartBit) const;
			};
			
		}
	}
}

#include "Malterlib_Container_BitArrayHierarchical_Internal.hpp"
