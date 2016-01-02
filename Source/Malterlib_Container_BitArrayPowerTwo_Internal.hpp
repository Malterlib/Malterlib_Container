// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib
{
	namespace NContainer
	{
		namespace NPrivate
		{
			
			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::TCBitArrayPowerTwoInternalArray(bool _bInitValues)
				: CSuper(_bInitValues)
				, m_Bits(_bInitValues)
			{
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>:: f_Clear()
			{
				CSuper::f_Clear();
				m_Bits.f_Clear();
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <typename tf_CStream>
			void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_Feed(tf_CStream &_Stream) const
			{
				if (mc_bIsFirstLevel)
					_Stream << m_Bits;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_IsFullySet() const
			{
				return m_Bits.f_IsFullySet();
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_IsFullyFree() const
			{
				return fp_IsFullyFree();
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <typename tf_CStream>
			void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_Consume(tf_CStream &_Stream)
			{
				if (mc_bIsFirstLevel)
				{
					CSuper::f_Clear();
					_Stream >> m_Bits;
					for (mint i = 0; i < mc_nSuperBits; ++i)
					{
						if (m_Bits.f_GetWholeEntrySet(i * t_TCBitArray<t_nBits>::EBitsPerEntry))
							CSuper::template f_SetBit<true>(i);
					}
				}
			}
			
			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <bool tf_bValue>
			void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_SetBit(mint _Bit)
			{
				m_Bits.template f_SetBit<tf_bValue>(_Bit);
				if (tf_bValue)
				{
					if (t_nBitsInternal & mint(1))
					{
						if (_Bit != t_nBitsInternal - 1)
						{
							if (!CSuper::f_GetBit(_Bit / 2))
								CSuper::template f_SetBit<true>(_Bit / 2);
						}
					}
					else
					{
						if (!CSuper::f_GetBit(_Bit / 2))
							CSuper::template f_SetBit<true>(_Bit / 2);
					}
				}
				else
				{
					if (t_nBitsInternal & mint(1))
					{
						if (_Bit != t_nBitsInternal - 1 && !m_Bits.f_GetBit(_Bit ^ mint(1)))
						{
							if (CSuper::f_GetBit(_Bit / 2))
								CSuper::template f_SetBit<false>(_Bit / 2);
						}
					}
					else
					{
						if (!m_Bits.f_GetBit(_Bit ^ mint(1)))
						{
							if (CSuper::f_GetBit(_Bit / 2))
								CSuper::template f_SetBit<false>(_Bit / 2);
						}
					}
				}
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_SetBit(mint _Bit, bool _bValue)
			{
				if (_bValue)
				{
					m_Bits.template f_SetBit<true>(_Bit);
					if (t_nBitsInternal & mint(1))
					{
						if (_Bit != t_nBitsInternal - 1)
						{
							if (!CSuper::f_GetBit(_Bit / 2))
								CSuper::template f_SetBit<true>(_Bit / 2);
						}
					}
					else
					{
						if (!CSuper::f_GetBit(_Bit / 2))
							CSuper::template f_SetBit<true>(_Bit / 2);
					}
				}
				else
				{
					m_Bits.template f_SetBit<false>(_Bit);
					if (t_nBitsInternal & mint(1))
					{
						if (_Bit != t_nBitsInternal - 1 && !m_Bits.f_GetBit(_Bit ^ mint(1)))
						{
							if (CSuper::f_GetBit(_Bit / 2))
								CSuper::template f_SetBit<false>(_Bit / 2);
						}
					}
					else
					{
						if (!m_Bits.f_GetBit(_Bit ^ mint(1)))
						{
							if (CSuper::f_GetBit(_Bit / 2))
								CSuper::template f_SetBit<false>(_Bit / 2);
						}
					}
				}
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_GetBit(mint _Bit) const
			{
				return m_Bits.f_GetBit(_Bit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_FindFreeBit() const
			{
				return m_Bits.f_FindFreeBit();
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_FindFreeBitAndSet()
			{
				aint Bit = m_Bits.f_FindFreeBitAndSet();
				if (Bit >= 0)
				{
					if (t_nBitsInternal & mint(1))
					{
						if (Bit != t_nBitsInternal - 1)
						{
							if (!CSuper::f_GetBit(Bit / 2))
								CSuper::template f_SetBit<true>(Bit / 2);
						}
					}
					else
					{
						if (!CSuper::f_GetBit(Bit / 2))
							CSuper::template f_SetBit<true>(Bit / 2);
					}
				}
				return Bit;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_FindFreeBitReverse() const
			{
				return m_Bits.f_FindFreeBitReverse();
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_FindFreeBitReverseAndSet()
			{
				aint Bit = m_Bits.f_FindFreeBitReverseAndSet();
				if (Bit >= 0)
				{
					if (t_nBitsInternal & mint(1))
					{
						if (Bit != t_nBitsInternal - 1)
						{
							if (!CSuper::f_GetBit(Bit / 2))
								CSuper::template f_SetBit<true>(Bit / 2);
						}
					}
					else
					{
						if (!CSuper::f_GetBit(Bit / 2))
							CSuper::template f_SetBit<true>(Bit / 2);
					}
				}
				return Bit;
			}
			
			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_FindUpperBound(mint _StartBit) const
			{
				return m_Bits.f_FindUpperBound(_StartBit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <typename t_CFunctor>
			void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_EnumFreeBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
			{
				return m_Bits.f_EnumFreeBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <typename t_CFunctor>
			void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_EnumFreeBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
			{
				return m_Bits.f_EnumFreeBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <typename t_CFunctor>
			void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_EnumSetBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
			{
				return m_Bits.f_EnumSetBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <typename t_CFunctor>
			void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_EnumSetBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
			{
				return m_Bits.f_EnumSetBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <bool tf_bValue>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_SetBit(mint _Level, mint _Bit)
			{
				if (_Level == t_nLevel)
					f_SetBit<tf_bValue>(_Bit);
				else 
				{
					CSuper::template f_SetBit<tf_bValue>(_Level, _Bit);

					mint nLevels = _Level - t_nLevel;
					if (nLevels)
						m_Bits.template f_SetBitRange<tf_bValue>(_Bit << nLevels, mint(1) << nLevels);
					else
						m_Bits.template f_SetBit<tf_bValue>(_Bit);
				}
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_SetBit(mint _Level, mint _Bit, bool _bValue)
			{
				if (_Level == t_nLevel)
					f_SetBit(_Bit, _bValue);
				else 
				{
					CSuper::f_SetBit(_Level, _Bit, _bValue);
					mint nLevels = _Level - t_nLevel;

					if (nLevels)
						m_Bits.f_SetBitRange(_Bit << nLevels, mint(1) << nLevels, _bValue);
					else
						m_Bits.f_SetBit(_Bit, _bValue);
				}
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_GetBit(mint _Level, mint _Bit) const
			{
				if (_Level == t_nLevel)
					return m_Bits.f_GetBit(_Bit);
				else 
					return CSuper::f_GetBit(_Level, _Bit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_FindFreeBit(mint _Level) const
			{
				if (_Level == t_nLevel)
					return m_Bits.f_FindFreeBit();
				else 
					return CSuper::f_FindFreeBit(_Level);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_FindFreeBitAndSet(mint _Level)
			{
				if (_Level == t_nLevel)
					return f_FindFreeBitAndSet();
				else
				{
					aint Bit = CSuper::f_FindFreeBitAndSet(_Level);
					if (Bit >= 0)
					{
						mint nLevels = _Level - t_nLevel;
						mint Start = Bit << nLevels;
						if (nLevels)
							m_Bits.template f_SetBitRange<true>(Start, mint(1) << nLevels);
						else
							m_Bits.template f_SetBit<true>(Start);
					}
					return Bit;
				}
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_FindFreeBitReverse(mint _Level) const
			{
				if (_Level == t_nLevel)
					return m_Bits.f_FindFreeBitReverse();
				else 
					return CSuper::f_FindFreeBitReverse(_Level);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_FindFreeBitReverseAndSet(mint _Level)
			{
				if (_Level == t_nLevel)
					return f_FindFreeBitReverseAndSet();
				else
				{
					aint Bit = CSuper::f_FindFreeBitReverseAndSet(_Level);
					if (Bit >= 0)
					{
						mint nLevels = _Level - t_nLevel;
						mint Start = Bit << nLevels;
						if (nLevels)
							m_Bits.template f_SetBitRange<true>(Start, mint(1) << nLevels);
						else
							m_Bits.template f_SetBit<true>(Start);
					}
					return Bit;
				}
			}
			
			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_FindUpperBoundAtLevel(mint _Level, mint _StartBit) const
			{
				if (_Level == t_nLevel)
					return m_Bits.f_FindUpperBound(_StartBit);
				else 
					return CSuper::f_FindUpperBoundAtLevel(_Level, _StartBit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_IsFullySet(mint _Level) const
			{
				if (_Level == t_nLevel)
					return m_Bits.f_IsFullySet();
				else
					return CSuper::f_IsFullySet(_Level);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::fp_IsFullyFree() const
			{
				if (mc_bIsLastLevel)
					return m_Bits.f_IsFullyFree();

				if (t_nBitsInternal & mint(1))
					return CSuper::fp_IsFullyFree() && !m_Bits.f_GetBit(t_nBitsInternal-1);
				else
					return CSuper::fp_IsFullyFree();
			}
			
			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_IsFullyFree(mint _Level) const
			{
				if (_Level == t_nLevel)
					return fp_IsFullyFree();
				else
					return CSuper::f_IsFullyFree(_Level);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <typename t_CFunctor>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_EnumFreeBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
			{
				if (_Level == t_nLevel)
					m_Bits.f_EnumFreeBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
				else
					CSuper::f_EnumFreeBits(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <typename t_CFunctor>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_EnumFreeBitRanges(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
			{
				if (_Level == t_nLevel)
					m_Bits.f_EnumFreeBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
				else
					CSuper::f_EnumFreeBitRanges(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
			template <typename t_CFunctor>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_EnumSetBitRanges(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
			{
				if (_Level == t_nLevel)
					m_Bits.f_EnumSetBitRanges(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
				else
					CSuper::f_EnumSetBitRanges(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels, bool t_bLast>
 			template <typename t_CFunctor>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, t_bLast>::f_EnumSetBits(mint _Level, t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
			{
				if (_Level == t_nLevel)
					m_Bits.f_EnumSetBits(fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
				else
					CSuper::f_EnumSetBits(_Level, fg_Forward<t_CFunctor>(_Functor), _StartBit, _EndBit);
			}
			
			///
			/// Root
			/// ====
			
			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::TCBitArrayPowerTwoInternalArray(bool _bInitValues)
			{
			}
			
			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_Clear()
			{
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			template <typename tf_CStream>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_Feed(tf_CStream &_Stream) const
			{
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			template <typename tf_CStream>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_Consume(tf_CStream &_Stream)
			{
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			template <bool tf_bValue>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_SetBit(mint _Bit)
			{
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_SetBit(mint _Bit, bool _bValue)
			{
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::fp_IsFullyFree() const
			{
				return false;
			}
			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_IsFullySet() const
			{
				return false;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_IsFullyFree() const
			{
				return true;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_GetBit(mint _Bit) const
			{
				return false;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_FindFreeBit() const
			{
				return 0;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_FindFreeBitAndSet()
			{
				return 0;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_FindFreeBitReverse() const
			{
				return 0;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_FindFreeBitReverseAndSet()
			{
				return 0;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_FindUpperBound(mint _StartBit) const
			{
				return 0;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_SetBit(mint _Level, mint _Bit, bool _bValue)
			{
				DMibFastCheck(false);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			template <bool tf_bValue>
			inline_always void TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_SetBit(mint _Level, mint _Bit)
			{
				DMibFastCheck(false);
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_IsFullySet(mint _Level) const
			{
				DMibFastCheck(false);
				return false;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_IsFullyFree(mint _Level) const
			{
				DMibFastCheck(false);
				return true;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always bool TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_GetBit(mint _Level, mint _Bit) const
			{
				DMibFastCheck(false);
				return false;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_FindFreeBit(mint _Level) const
			{
				DMibFastCheck(false);
				return 0;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_FindFreeBitAndSet(mint _Level)
			{
				DMibFastCheck(false);
				return 0;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_FindFreeBitReverse(mint _Level) const
			{
				DMibFastCheck(false);
				return 0;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_FindFreeBitReverseAndSet(mint _Level)
			{
				DMibFastCheck(false);
				return 0;
			}

			template <mint t_nBits, mint t_nBitsInternal, mint t_nLevel, template <mint t_nBits2> class t_TCBitArray, mint t_nMaxLevels>
			inline_always aint TCBitArrayPowerTwoInternalArray<t_nBits, t_nBitsInternal, t_nLevel, t_TCBitArray, t_nMaxLevels, true>::f_FindUpperBoundAtLevel(mint _Level, mint _StartBit) const
			{
				DMibFastCheck(false);
				return 0;
			}
		}
		
	}
}

