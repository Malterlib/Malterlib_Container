// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib
{
	namespace NContainer
	{
		namespace NPrivate
		{
			template <typename t_CType, bool t_bValue>
			struct TCSetBitHelper
			{
			};

			template <typename t_CType>
			struct TCSetBitHelper<t_CType, true>
			{
				static inline_small t_CType fs_SetBit(t_CType _Variable, mint _Bit)
				{
					return _Variable | (t_CType(1) << _Bit);
				}

				static inline_small t_CType fs_SetBitRange(t_CType _Variable, mint _Bit, mint _nBits)
				{
					mint EndBit = _Bit + _nBits - 1;
					return _Variable | DMibBitRangeTyped(_Bit, EndBit, t_CType);
				}
			};

			template <typename t_CType>
			struct TCSetBitHelper<t_CType, false>
			{
				static inline_small t_CType fs_SetBit(t_CType _Variable, mint _Bit)
				{
					return _Variable & t_CType(~(t_CType(1) << _Bit));
				}
				
				static inline_small t_CType fs_SetBitRange(t_CType _Variable, mint _Bit, mint _nBits)
				{
					mint EndBit = _Bit + _nBits - 1;
					return _Variable & t_CType(~(DMibBitRangeTyped(_Bit, EndBit, t_CType)));
				}
			};
		}
		
		///
		/// Privates
		/// ========
		
		template <mint t_nBits>
		template <bool tf_bValue>
		inline_small typename TCBitArray<t_nBits>::CEntryType TCBitArray<t_nBits>::fp_SetBit(CEntryType _Entry, mint _Bit)
		{
			return NPrivate::TCSetBitHelper<CEntryType, tf_bValue>::fs_SetBit(_Entry, _Bit);
		}

		template <mint t_nBits>
		template <bool tf_bValue>
		inline_small typename TCBitArray<t_nBits>::CEntryType TCBitArray<t_nBits>::fp_SetBitRange(CEntryType _Entry, mint _Bit, mint _nBits)
		{
			return NPrivate::TCSetBitHelper<CEntryType, tf_bValue>::fs_SetBitRange(_Entry, _Bit, _nBits);
		}

		template <mint t_nBits>
		inline_small typename TCBitArray<t_nBits>::CEntryType TCBitArray<t_nBits>::fp_SetBit(CEntryType _Entry, mint _Bit, bool _bValue)
		{
			return (_Entry & CEntryType(~(CEntryType(1) << _Bit))) | (CEntryType(_bValue) << _Bit);
		}
		

		///
		/// Publics
		/// =======
		
		template <mint t_nBits>
		constexpr TCBitArray<t_nBits>::TCBitArray(bool _bInitValues)
			: mp_Entries{0}
		{
		}

		template <mint t_nBits>
		void TCBitArray<t_nBits>::f_Clear()
		{
			for (mint i = 0; i < ENumEntries; ++i)
				mp_Entries[i] = 0;
		}

		template <mint t_nBits>
		template <typename tf_CStream>
		void TCBitArray<t_nBits>::f_Feed(tf_CStream &_Stream) const
		{
			uint32 nBytes = (t_nBits + 7) / 8;
			for (mint i = 0; i < nBytes; ++i)
			{
				mint iEntry = (i*8) / ENumBitsPerEntry;
				mint iSubBit = (i*8) % ENumBitsPerEntry;		
				uint8 Byte = (mp_Entries[iEntry] >> iSubBit) & CEntryType(0xff);
				_Stream << Byte;
			}
		}

		template <mint t_nBits>
		template <typename tf_CStream>
		void TCBitArray<t_nBits>::f_Consume(tf_CStream &_Stream)
		{
			f_Clear();
			uint32 nBytes = (t_nBits + 7) / 8;
			for (mint i = 0; i < nBytes; ++i)
			{
				mint iEntry = (i*8) / ENumBitsPerEntry;
				mint iSubBit = (i*8) % ENumBitsPerEntry;		
				uint8 Byte;
				_Stream >> Byte;
				mp_Entries[iEntry] |= CEntryType(Byte) << iSubBit;
			}
		}

		template <mint t_nBits>
		inline_small void TCBitArray<t_nBits>::f_SetBit(mint _Bit, bool _bValue)
		{
			DMibFastCheck(_Bit < t_nBits);
			mint iEntry = _Bit / ENumBitsPerEntry;
			mint iSubBit = _Bit % ENumBitsPerEntry;
			mp_Entries[iEntry] = fp_SetBit(mp_Entries[iEntry], iSubBit, _bValue);
		}

		template <mint t_nBits>
		template <bool tf_bValue>
		inline_small void TCBitArray<t_nBits>::f_SetBit(mint _Bit)
		{
			DMibFastCheck(_Bit < t_nBits);
			mint iEntry = _Bit / ENumBitsPerEntry;
			mint iSubBit = _Bit % ENumBitsPerEntry;
			mp_Entries[iEntry] = fp_SetBit<tf_bValue>(mp_Entries[iEntry], iSubBit);
		}

		template <mint t_nBits>
		template <bool tf_bValue>
		inline_small void TCBitArray<t_nBits>::f_SetBitRange(mint _StartBit, mint _nBits)
		{
			DMibFastCheck(_StartBit + _nBits <= t_nBits);
			mint iEntry = _StartBit / ENumBitsPerEntry;
			mint iSubBit = _StartBit % ENumBitsPerEntry;

			mint nBitsToSet = fg_Min(_nBits, ENumBitsPerEntry - iSubBit);

			mp_Entries[iEntry] = fp_SetBitRange<tf_bValue>(mp_Entries[iEntry], iSubBit, nBitsToSet);
			++iEntry;
			_nBits -= nBitsToSet;

			while (_nBits)
			{
				nBitsToSet = fg_Min(_nBits, ENumBitsPerEntry);
				mp_Entries[iEntry] = fp_SetBitRange<tf_bValue>(mp_Entries[iEntry], 0, nBitsToSet);
				++iEntry;
				_nBits -= nBitsToSet;
			}
		}

		template <mint t_nBits>
		inline_small void TCBitArray<t_nBits>::f_SetBitRange(mint _StartBit, mint _nBits, bool _bValue)
		{
			if (_bValue)
				f_SetBitRange<true>(_StartBit, _nBits);
			else
				f_SetBitRange<false>(_StartBit, _nBits);
		}

		template <mint t_nBits>
		inline_small bool TCBitArray<t_nBits>::f_SetBitWholeEntrySet(mint _Bit, bool _bValue)
		{
			DMibFastCheck(_Bit < t_nBits);
			mint iEntry = _Bit / ENumBitsPerEntry;
			mint iSubBit = _Bit % ENumBitsPerEntry;
			auto NewEntry = fp_SetBit(mp_Entries[iEntry], iSubBit, _bValue);
			mp_Entries[iEntry] = NewEntry;
			if (t_nBits != ENumEntries * ENumBitsPerEntry)
			{
				mint NumEntries = ENumEntries - 1;
				if (iEntry < NumEntries)
					return NewEntry == mcp_FullySet;
				else
					return NewEntry == mcp_FullySetLast;
			}
			else
			{
				return NewEntry == mcp_FullySet;
			}
		}

		template <mint t_nBits>
		template <bool tf_bValue>
		inline_small bool TCBitArray<t_nBits>::f_SetBitRangeWholeEntrySet(mint _StartBit, mint _nBits)
		{
			DMibFastCheck(_StartBit + _nBits <= t_nBits);
			mint iEntry = _StartBit / ENumBitsPerEntry;
			mint iSubBit = _StartBit % ENumBitsPerEntry;

			mint nBitsToSet = fg_Min(_nBits, ENumBitsPerEntry - iSubBit);
			DMibFastCheck(nBitsToSet == _nBits); // Only one entry supported

			auto NewEntry = fp_SetBitRange<tf_bValue>(mp_Entries[iEntry], iSubBit, nBitsToSet);
			mp_Entries[iEntry] = NewEntry;

			if (t_nBits != ENumEntries * ENumBitsPerEntry)
			{
				mint NumEntries = ENumEntries - 1;
				if (iEntry < NumEntries)
					return NewEntry == mcp_FullySet;
				else
					return NewEntry == mcp_FullySetLast;
			}
			else
			{
				return NewEntry == mcp_FullySet;
			}
		}

		template <mint t_nBits>
		template <bool tf_bValue>
		inline_small bool TCBitArray<t_nBits>::f_SetBitWholeEntrySet(mint _Bit)
		{
			DMibFastCheck(_Bit < t_nBits);
			mint iEntry = _Bit / ENumBitsPerEntry;
			mint iSubBit = _Bit % ENumBitsPerEntry;
			auto NewEntry = fp_SetBit<tf_bValue>(mp_Entries[iEntry], iSubBit);
			mp_Entries[iEntry] = NewEntry;
			if (t_nBits != ENumEntries * ENumBitsPerEntry)
			{
				mint NumEntries = ENumEntries - 1;
				if (iEntry < NumEntries)
					return NewEntry == mcp_FullySet;
				else
					return NewEntry == mcp_FullySetLast;
			}
			else
			{
				return NewEntry == mcp_FullySet;
			}
		}

		template <mint t_nBits>
		inline_small bool TCBitArray<t_nBits>::f_GetBit(mint _Bit) const
		{
			DMibFastCheck(_Bit < t_nBits);
			mint iEntry = _Bit / ENumBitsPerEntry;
			mint iSubBit = _Bit % ENumBitsPerEntry;		

			return ((mp_Entries[iEntry] >> iSubBit) & CEntryType(1)) != 0;
		}

		template <mint t_nBits>
		inline_small bool TCBitArray<t_nBits>::f_GetWholeEntrySet(mint _Bit) const
		{
			DMibFastCheck(_Bit < t_nBits);
			mint iEntry = _Bit / ENumBitsPerEntry;

			if (t_nBits != ENumEntries * ENumBitsPerEntry)
			{
				mint NumEntries = ENumEntries - 1;			
				if (iEntry < NumEntries)
					return mp_Entries[iEntry] == mcp_FullySet;
				else
					return mp_Entries[iEntry] == mcp_FullySetLast;
			}
			else
				return mp_Entries[iEntry] == mcp_FullySet;
		}

		template <mint t_nBits>
		inline_always aint TCBitArray<t_nBits>::fp_FindFreeBit(mint _StartBitApproximately, mint &_iEntry) const
		{
			mint iEntry = _StartBitApproximately / ENumBitsPerEntry;
			DMibFastCheck(iEntry < ENumEntries);
			if (t_nBits != ENumEntries * ENumBitsPerEntry)
			{
				mint NumEntries = ENumEntries - 1;
				if (iEntry < NumEntries)
				{
					CEntryType const &Entry = mp_Entries[iEntry];
					if (Entry != mcp_FullySet)
					{
						mint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
						_iEntry = iEntry;
						return FreeEntry;
					}
				}
				else
				{
					CEntryType Entry = mp_Entries[iEntry] | CEntryType(~mcp_FullySetLast);
					if (Entry != mcp_FullySet)
					{
						mint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
						_iEntry = iEntry;
						return FreeEntry;
					}
				}
			}
			else
			{
				CEntryType const &Entry = mp_Entries[iEntry];
				if (Entry != mcp_FullySet)
				{
					mint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
					_iEntry = iEntry;
					return FreeEntry;
				}
			}
			return -1;
		}

		template <mint t_nBits>
		inline_always aint TCBitArray<t_nBits>::fp_FindFreeBitReverse(mint _StartBitApproximately, mint &_iEntry) const
		{
			aint iEntry = _StartBitApproximately / ENumBitsPerEntry;
			DMibFastCheck(iEntry < ENumEntries);
			if (t_nBits != ENumEntries * ENumBitsPerEntry)
			{
				aint NumEntries = ENumEntries - 1;
				if (iEntry < NumEntries)
				{
					CEntryType const &Entry = mp_Entries[iEntry];
					if (Entry != mcp_FullySet)
					{
						mint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
						_iEntry = iEntry;
						return FreeEntry;
					}
				}
				else
				{
					CEntryType Entry = mp_Entries[iEntry] | CEntryType(~mcp_FullySetLast);
					
					if (Entry != mcp_FullySet)
					{
						mint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
						_iEntry = iEntry;
						return FreeEntry;
					}
				}
			}
			else
			{
				CEntryType const &Entry = mp_Entries[iEntry];
				if (Entry != mcp_FullySet)
				{
					mint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
					_iEntry = iEntry;
					return FreeEntry;
				}
			}
			return -1;
		}


		template <mint t_nBits>
		inline_small aint TCBitArray<t_nBits>::f_FindFreeBit(mint _StartBitApproximately) const
		{
			mint iEntry;
			aint iFreeEntry = fp_FindFreeBit(_StartBitApproximately, iEntry);
			if (iFreeEntry < 0)
				return -1;
			return iEntry * ENumBitsPerEntry + iFreeEntry;
		}

		template <mint t_nBits>
		inline_small aint TCBitArray<t_nBits>::f_FindFreeBitReverse(mint _StartBitApproximately) const
		{
			mint iEntry;
			aint iFreeEntry = fp_FindFreeBitReverse(_StartBitApproximately, iEntry);
			if (iFreeEntry < 0)
				return -1;
			return iEntry * ENumBitsPerEntry + iFreeEntry;
		}

		template <mint t_nBits>
		inline_small aint TCBitArray<t_nBits>::f_FindFreeBitAndSet(mint _StartBitApproximately)
		{
			mint iEntry;
			aint iFreeEntry = fp_FindFreeBit(_StartBitApproximately, iEntry);
			if (iFreeEntry < 0)
				return -1;

			mp_Entries[iEntry] = fp_SetBit(mp_Entries[iEntry], iFreeEntry, 1);

			return iEntry * ENumBitsPerEntry + iFreeEntry;
		}

		template <mint t_nBits>
		inline_small aint TCBitArray<t_nBits>::f_FindFreeBitReverseAndSet(mint _StartBitApproximately)
		{
			mint iEntry;
			aint iFreeEntry = fp_FindFreeBitReverse(_StartBitApproximately, iEntry);
			if (iFreeEntry < 0)
				return -1;

			mp_Entries[iEntry] = fp_SetBit(mp_Entries[iEntry], iFreeEntry, 1);

			return iEntry * ENumBitsPerEntry + iFreeEntry;
		}

		template <mint t_nBits>
		inline_small bool TCBitArray<t_nBits>::f_IsFullySet() const
		{
			if (t_nBits != ENumEntries * ENumBitsPerEntry)
			{
				mint nEntries = ENumEntries - 1;
				for (mint i = 0; i < nEntries; ++i)
				{
					CEntryType const &Entry = mp_Entries[i];
					if (Entry != mcp_FullySet)
						return false;
				}
//				if (ENumEntries > 1)
				{
					CEntryType const &Entry = mp_Entries[ENumEntries - 1];
					if (Entry != mcp_FullySetLast)
						return false;
				}
			}
			else
			{
				for (mint i = 0; i < ENumEntries; ++i)
				{
					CEntryType const &Entry = mp_Entries[i];
					if (Entry != mcp_FullySet)
						return false;
				}
			}
			return true;
		}

		template <mint t_nBits>
		inline_small bool TCBitArray<t_nBits>::f_IsFullyFree() const
		{
			if (t_nBits != ENumEntries * ENumBitsPerEntry)
			{
				mint nEntries = ENumEntries - 1;
				for (mint i = 0; i < nEntries; ++i)
				{
					CEntryType const &Entry = mp_Entries[i];
					if (Entry != 0)
						return false;
				}
//				if (ENumEntries > 1)
				{
					CEntryType const &Entry = mp_Entries[ENumEntries - 1];
					if (Entry != 0)
						return false;
				}
			}
			else
			{
				for (mint i = 0; i < ENumEntries; ++i)
				{
					CEntryType const &Entry = mp_Entries[i];
					if (Entry != 0)
						return false;
				}
			}
			return true;
		}

		template <mint t_nBits>
		inline_always aint TCBitArray<t_nBits>::fp_FindFreeBit(mint &_iEntry) const
		{
			if (t_nBits != ENumEntries * ENumBitsPerEntry)
			{
				for (mint i = 0; i < ENumEntries - 1; ++i)
				{
					CEntryType const &Entry = mp_Entries[i];
					if (Entry != mcp_FullySet)
					{
						mint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
						_iEntry = i;
						return FreeEntry;
					}
				}
//				if (ENumEntries > 1)
				{
					CEntryType Entry = mp_Entries[ENumEntries - 1] | CEntryType(~mcp_FullySetLast);
					if (Entry != mcp_FullySet)
					{
						mint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
						_iEntry = ENumEntries - 1;
						return FreeEntry;
					}
				}
			}
			else
			{
				for (mint i = 0; i < ENumEntries; ++i)
				{
					CEntryType const &Entry = mp_Entries[i];
					if (Entry != mcp_FullySet)
					{
						mint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
						_iEntry = i;
						return FreeEntry;
					}
				}
			}
			return -1;
		}

		template <mint t_nBits>
		inline_always aint TCBitArray<t_nBits>::fp_FindFreeBitReverse(mint &_iEntry) const
		{
			if (t_nBits != ENumEntries * ENumBitsPerEntry)
			{
				{
					CEntryType Entry = mp_Entries[ENumEntries - 1] | (CEntryType(~mcp_FullySetLast));
					if (Entry != mcp_FullySet)
					{
						mint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
						_iEntry = ENumEntries - 1;
						return FreeEntry;
					}
				}
				for (aint i = ENumEntries - 2; i >= 0; --i)
				{
					CEntryType const &Entry = mp_Entries[i];
					if (Entry != mcp_FullySet)
					{
						mint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
						_iEntry = i;
						return FreeEntry;
					}
				}
			}
			else
			{
				for (aint i = ENumEntries - 1; i >= 0; --i)
				{
					CEntryType const &Entry = mp_Entries[i];
					if (Entry != mcp_FullySet)
					{
						mint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
						_iEntry = i;
						return FreeEntry;
					}
				}
			}
			return -1;
		}
		
		template <mint t_nBits>
		inline_small aint TCBitArray<t_nBits>::f_FindFreeBit() const
		{
			mint iEntry;
			aint iFreeEntry = fp_FindFreeBit(iEntry);
			if (iFreeEntry < 0)
				return -1;
			return iEntry * ENumBitsPerEntry + iFreeEntry;
		}

		template <mint t_nBits>
		inline_small aint TCBitArray<t_nBits>::f_FindFreeBitReverse() const
		{
			mint iEntry;
			aint iFreeEntry = fp_FindFreeBitReverse(iEntry);
			if (iFreeEntry < 0)
				return -1;
			return iEntry * ENumBitsPerEntry + iFreeEntry;
		}

		template <mint t_nBits>
		template <typename t_CFunctor>
		void TCBitArray<t_nBits>::f_EnumFreeBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
		{
			DMibFastCheck(_StartBit < _EndBit);
			DMibFastCheck(_EndBit <= t_nBits);
			
			mint iEntry = _StartBit / ENumBitsPerEntry;
			mint iSubBit = _StartBit % ENumBitsPerEntry;	
			
			mint iEntryForward = iEntry;
			mint iSubBitForward = iSubBit;

			mint EntryStartBit = iEntryForward * ENumBitsPerEntry;
			
			while (EntryStartBit < _EndBit)
			{
				CEntryType Entry = CEntryType(~(mp_Entries[iEntryForward]));
				Entry >>= iSubBitForward;
				
				while (Entry)
				{
					mint iFreeEntry = fg_GetLowestBitSetNoZero(Entry);
					mint iFinalEntry = EntryStartBit + iFreeEntry + iSubBitForward;
					if (iFinalEntry >= _EndBit || !_Functor(iFinalEntry))
						return;
					++iFreeEntry;
					// We need to do this in two stages because we might need to shift all bits which is undefined
					Entry >>= iFreeEntry / 2;
					Entry >>= iFreeEntry - iFreeEntry / 2;
					iSubBitForward += iFreeEntry;
				}
				iSubBitForward = 0;
				EntryStartBit += ENumBitsPerEntry;
				++iEntryForward;
			}
		}

		template <mint t_nBits>
		template <typename t_CFunctor>
		void TCBitArray<t_nBits>::f_EnumSetBits(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
		{
			DMibFastCheck(_StartBit < _EndBit);
			DMibFastCheck(_EndBit <= t_nBits);
			
			mint iEntry = _StartBit / ENumBitsPerEntry;
			mint iSubBit = _StartBit % ENumBitsPerEntry;	
			
			mint iEntryForward = iEntry;
			mint iSubBitForward = iSubBit;

			mint EntryStartBit = iEntryForward * ENumBitsPerEntry;
			
			while (EntryStartBit < _EndBit)
			{
				CEntryType Entry = mp_Entries[iEntryForward];
				Entry >>= iSubBitForward;
				
				while (Entry)
				{
					mint iFreeEntry = fg_GetLowestBitSetNoZero(Entry);
					mint iFinalEntry = EntryStartBit + iFreeEntry + iSubBitForward;
					if (iFinalEntry >= _EndBit || !_Functor(iFinalEntry))
						return;
					
					++iFreeEntry;
					// We need to do this in two stages because we might need to shift all bits which is undefined
					Entry >>= iFreeEntry/2;
					Entry >>= iFreeEntry - iFreeEntry/2;
					
					iSubBitForward += iFreeEntry;
				}
				iSubBitForward = 0;
				EntryStartBit += ENumBitsPerEntry;
				++iEntryForward;
			}
		}

		template <mint t_nBits>
		template <typename t_CFunctor>
		void TCBitArray<t_nBits>::f_EnumFreeBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
		{
			DMibFastCheck(_StartBit < _EndBit);
			DMibFastCheck(_EndBit <= t_nBits);
			
			mint iEntry = _StartBit / ENumBitsPerEntry;
			mint iSubBit = _StartBit % ENumBitsPerEntry;	
			
			mint iEntryForward = iEntry;
			mint iSubBitForward = iSubBit;

			mint EntryStartBit = iEntryForward * ENumBitsPerEntry;
			
			while (EntryStartBit < _EndBit)
			{
				CEntryType Entry = CEntryType(~(mp_Entries[iEntryForward]));
				Entry >>= iSubBitForward;
				
				while (Entry)
				{
					mint iFreeEntry = fg_GetLowestBitSetNoZero(Entry);
					mint iStartFinalEntry = EntryStartBit + iFreeEntry + iSubBitForward;
					if (iStartFinalEntry >= _EndBit)
						return;
					
					CEntryType EndEntry;
					
					if (iFreeEntry)
						EndEntry = CEntryType(~(Entry | DMibBitRangeTyped(0, iFreeEntry - 1, CEntryType)));
					else
						EndEntry = CEntryType(~Entry);
					
					if (iSubBitForward)
						EndEntry &= CEntryType(~DMibBitRangeTyped((sizeof(CEntryType) * 8 - iSubBitForward), (sizeof(CEntryType) * 8 - 1), CEntryType));

					if (!EndEntry)
					{
						++iEntryForward;
						EntryStartBit += ENumBitsPerEntry;
						
						mint iEndFinalEntry = EntryStartBit;
						
						while (iEndFinalEntry < _EndBit)
						{
							auto Entry = mp_Entries[iEntryForward];
							if (Entry)
							{
								mint iSetEntry = fg_GetLowestBitSetNoZero(Entry);
								DMibFastCheck(iSetEntry >= 0);
								iEndFinalEntry = EntryStartBit + iSetEntry;
								iSubBitForward = iSetEntry + 1;
								break;								
							}
							else
							{
								++iEntryForward;
								EntryStartBit += ENumBitsPerEntry;
								iEndFinalEntry = EntryStartBit;
							}
						}
						
						if (!_Functor(iStartFinalEntry, fg_Min(iEndFinalEntry, _EndBit) - iStartFinalEntry))
							return;
						
						if (iEndFinalEntry >= _EndBit)
							return;
						
						Entry = CEntryType(~(mp_Entries[iEntryForward]));
						// We need to do this in two stages because we might need to shift all bits which is undefined
						Entry >>= iSubBitForward/2;
						Entry >>= iSubBitForward - iSubBitForward/2;
						continue;
					}
					else
					{
						mint iSetEntry = fg_GetLowestBitSetNoZero(EndEntry);
					
						mint iEndFinalEntry = EntryStartBit + iSetEntry + iSubBitForward;
						if (!_Functor(iStartFinalEntry, fg_Min(iEndFinalEntry, _EndBit) - iStartFinalEntry))
							return;
						++iSetEntry;
						// We need to do this in two stages because we might need to shift all bits which is undefined
						Entry >>= iSetEntry/2;
						Entry >>= iSetEntry - iSetEntry/2;
						iSubBitForward += iSetEntry;
					}
				}
				iSubBitForward = 0;
				EntryStartBit += ENumBitsPerEntry;
				++iEntryForward;
			}
		}

		template <mint t_nBits>
		template <typename t_CFunctor>
		void TCBitArray<t_nBits>::f_EnumSetBitRanges(t_CFunctor &&_Functor, mint _StartBit, mint _EndBit) const
		{
			DMibFastCheck(_StartBit < _EndBit);
			DMibFastCheck(_EndBit <= t_nBits);
			
			mint iEntry = _StartBit / ENumBitsPerEntry;
			mint iSubBit = _StartBit % ENumBitsPerEntry;	
			
			mint iEntryForward = iEntry;
			mint iSubBitForward = iSubBit;

			mint EntryStartBit = iEntryForward * ENumBitsPerEntry;
			
			while (EntryStartBit < _EndBit)
			{
				CEntryType Entry = mp_Entries[iEntryForward];
				Entry >>= iSubBitForward;
				
				while (Entry)
				{
					mint iFreeEntry = fg_GetLowestBitSetNoZero(Entry);
					mint iStartFinalEntry = EntryStartBit + iFreeEntry + iSubBitForward;
					if (iStartFinalEntry >= _EndBit)
						return;
					
					CEntryType EndEntry;
					
					if (iFreeEntry)
						EndEntry = CEntryType(~(Entry | DMibBitRangeTyped(0, iFreeEntry - 1, CEntryType)));
					else
						EndEntry = CEntryType(~Entry);
					
					if (iSubBitForward)
						EndEntry &= CEntryType(~DMibBitRangeTyped((sizeof(CEntryType) * 8 - iSubBitForward), (sizeof(CEntryType) * 8 - 1), CEntryType));

					if (!EndEntry)
					{
						++iEntryForward;
						EntryStartBit += ENumBitsPerEntry;
						
						mint iEndFinalEntry = EntryStartBit;
						
						while (iEndFinalEntry < _EndBit)
						{
							auto Entry = CEntryType(~(mp_Entries[iEntryForward]));
							if (Entry)
							{
								mint iSetEntry = fg_GetLowestBitSetNoZero(Entry);
								iEndFinalEntry = EntryStartBit + iSetEntry;
								iSubBitForward = iSetEntry + 1;
								break;								
							}
							else
							{
								++iEntryForward;
								EntryStartBit += ENumBitsPerEntry;
								iEndFinalEntry = EntryStartBit;
							}
						}
						
						if (!_Functor(iStartFinalEntry, fg_Min(iEndFinalEntry, _EndBit) - iStartFinalEntry))
							return;
						
						if (iEndFinalEntry >= _EndBit)
							return;
						
						Entry = mp_Entries[iEntryForward];
						// We need to do this in two stages because we might need to shift all bits which is undefined
						Entry >>= iSubBitForward / 2;
						Entry >>= iSubBitForward - iSubBitForward / 2;
						continue;
					}
					else
					{
						mint iSetEntry = fg_GetLowestBitSetNoZero(EndEntry);
					
						mint iEndFinalEntry = EntryStartBit + iSetEntry + iSubBitForward;
						if (!_Functor(iStartFinalEntry, fg_Min(iEndFinalEntry, _EndBit) - iStartFinalEntry))
							return;
						++iSetEntry;
						// We need to do this in two stages because we might need to shift all bits which is undefined
						Entry >>= iSetEntry/2;
						Entry >>= iSetEntry - iSetEntry/2;
						iSubBitForward += iSetEntry;
					}
				}
				iSubBitForward = 0;
				EntryStartBit += ENumBitsPerEntry;
				++iEntryForward;
			}
		}

		template <mint t_nBits>
		inline_small aint TCBitArray<t_nBits>::f_FindFreeBitAndSet()
		{
			mint iEntry;
			aint iFreeEntry = fp_FindFreeBit(iEntry);
			if (iFreeEntry < 0)
				return -1;

			mp_Entries[iEntry] = fp_SetBit(mp_Entries[iEntry], iFreeEntry, 1);

			return iEntry * ENumBitsPerEntry + iFreeEntry;
		}

		template <mint t_nBits>
		inline_small aint TCBitArray<t_nBits>::f_FindFreeBitReverseAndSet()
		{
			mint iEntry;
			aint iFreeEntry = fp_FindFreeBitReverse(iEntry);
			if (iFreeEntry < 0)
				return -1;

			mp_Entries[iEntry] = fp_SetBit(mp_Entries[iEntry], iFreeEntry, 1);

			return iEntry * ENumBitsPerEntry + iFreeEntry;
		}

		template <mint t_nBits>
		inline_small aint TCBitArray<t_nBits>::f_FindUpperBound(mint _StartBit) const
		{
			DMibFastCheck(_StartBit < t_nBits);

			mint StartEntry = _StartBit / ENumBitsPerEntry;
			mint StartBit = _StartBit % ENumBitsPerEntry;

			CEntryType const &Entry = mp_Entries[StartEntry];
			if (Entry)
			{
				aint FoundBit = fg_GetLowestBitSet(Entry >> StartBit);
				if (FoundBit >= 0)
					return StartEntry * ENumBitsPerEntry + StartBit + FoundBit;
			}

			++StartEntry;

			for (mint i = StartEntry; i < ENumEntries; ++i)
			{
				CEntryType const &Entry = mp_Entries[i];
				if (Entry)
				{
					aint FoundBit = fg_GetLowestBitSet(Entry);
					if (FoundBit >= 0)
						return i * ENumBitsPerEntry + FoundBit;
				}
			}

			return -1;
		}
		
	}
}

