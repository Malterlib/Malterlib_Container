// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
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
			static inline_small t_CType fs_SetBit(t_CType _Variable, umint _Bit)
			{
				return _Variable | (t_CType(1) << _Bit);
			}

			static inline_small t_CType fs_SetBitRange(t_CType _Variable, umint _Bit, umint _nBits)
			{
				umint EndBit = _Bit + _nBits - 1;
				return _Variable | DMibBitRangeTyped(_Bit, EndBit, t_CType);
			}
		};

		template <typename t_CType>
		struct TCSetBitHelper<t_CType, false>
		{
			static inline_small t_CType fs_SetBit(t_CType _Variable, umint _Bit)
			{
				return _Variable & t_CType(~(t_CType(1) << _Bit));
			}

			static inline_small t_CType fs_SetBitRange(t_CType _Variable, umint _Bit, umint _nBits)
			{
				umint EndBit = _Bit + _nBits - 1;
				return _Variable & t_CType(~(DMibBitRangeTyped(_Bit, EndBit, t_CType)));
			}
		};
	}

	///
	/// Privates
	/// ========

	template <umint t_nBits>
	template <bool tf_bValue>
	inline_small typename TCBitArray<t_nBits>::CEntryType TCBitArray<t_nBits>::fp_SetBit(CEntryType _Entry, umint _Bit)
	{
		return NPrivate::TCSetBitHelper<CEntryType, tf_bValue>::fs_SetBit(_Entry, _Bit);
	}

	template <umint t_nBits>
	template <bool tf_bValue>
	inline_small typename TCBitArray<t_nBits>::CEntryType TCBitArray<t_nBits>::fp_SetBitRange(CEntryType _Entry, umint _Bit, umint _nBits)
	{
		return NPrivate::TCSetBitHelper<CEntryType, tf_bValue>::fs_SetBitRange(_Entry, _Bit, _nBits);
	}

	template <umint t_nBits>
	inline_small typename TCBitArray<t_nBits>::CEntryType TCBitArray<t_nBits>::fp_SetBit(CEntryType _Entry, umint _Bit, bool _bValue)
	{
		return (_Entry & CEntryType(~(CEntryType(1) << _Bit))) | (CEntryType(_bValue) << _Bit);
	}


	///
	/// Publics
	/// =======

	template <umint t_nBits>
	constexpr TCBitArray<t_nBits>::TCBitArray(bool _bInitValues)
		: mp_Entries{0}
	{
	}

	template <umint t_nBits>
	void TCBitArray<t_nBits>::f_Clear()
	{
		for (umint i = 0; i < ENumEntries; ++i)
			mp_Entries[i] = 0;
	}

	template <umint t_nBits>
	template <typename tf_CStream>
	void TCBitArray<t_nBits>::f_Feed(tf_CStream &_Stream) const
	{
		uint32 nBytes = (t_nBits + 7) / 8;
		for (umint i = 0; i < nBytes; ++i)
		{
			umint iEntry = (i*8) / ENumBitsPerEntry;
			umint iSubBit = (i*8) % ENumBitsPerEntry;
			uint8 Byte = (mp_Entries[iEntry] >> iSubBit) & CEntryType(0xff);
			_Stream << Byte;
		}
	}

	template <umint t_nBits>
	template <typename tf_CStream>
	void TCBitArray<t_nBits>::f_Consume(tf_CStream &_Stream)
	{
		f_Clear();
		uint32 nBytes = (t_nBits + 7) / 8;
		for (umint i = 0; i < nBytes; ++i)
		{
			umint iEntry = (i*8) / ENumBitsPerEntry;
			umint iSubBit = (i*8) % ENumBitsPerEntry;
			uint8 Byte;
			_Stream >> Byte;
			mp_Entries[iEntry] |= CEntryType(Byte) << iSubBit;
		}
	}

	template <umint t_nBits>
	inline_small void TCBitArray<t_nBits>::f_SetBit(umint _Bit, bool _bValue)
	{
		DMibFastCheck(_Bit < t_nBits);
		umint iEntry = _Bit / ENumBitsPerEntry;
		umint iSubBit = _Bit % ENumBitsPerEntry;
		mp_Entries[iEntry] = fp_SetBit(mp_Entries[iEntry], iSubBit, _bValue);
	}

	template <umint t_nBits>
	template <bool tf_bValue>
	inline_small void TCBitArray<t_nBits>::f_SetBit(umint _Bit)
	{
		DMibFastCheck(_Bit < t_nBits);
		umint iEntry = _Bit / ENumBitsPerEntry;
		umint iSubBit = _Bit % ENumBitsPerEntry;
		mp_Entries[iEntry] = fp_SetBit<tf_bValue>(mp_Entries[iEntry], iSubBit);
	}

	template <umint t_nBits>
	template <bool tf_bValue>
	inline_small void TCBitArray<t_nBits>::f_SetBitRange(umint _StartBit, umint _nBits)
	{
		DMibFastCheck(_StartBit + _nBits <= t_nBits);
		umint iEntry = _StartBit / ENumBitsPerEntry;
		umint iSubBit = _StartBit % ENumBitsPerEntry;

		umint nBitsToSet = fg_Min(_nBits, ENumBitsPerEntry - iSubBit);

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

	template <umint t_nBits>
	inline_small void TCBitArray<t_nBits>::f_SetBitRange(umint _StartBit, umint _nBits, bool _bValue)
	{
		if (_bValue)
			f_SetBitRange<true>(_StartBit, _nBits);
		else
			f_SetBitRange<false>(_StartBit, _nBits);
	}

	template <umint t_nBits>
	inline_small bool TCBitArray<t_nBits>::f_SetBitWholeEntrySet(umint _Bit, bool _bValue)
	{
		DMibFastCheck(_Bit < t_nBits);
		umint iEntry = _Bit / ENumBitsPerEntry;
		umint iSubBit = _Bit % ENumBitsPerEntry;
		auto NewEntry = fp_SetBit(mp_Entries[iEntry], iSubBit, _bValue);
		mp_Entries[iEntry] = NewEntry;
		if constexpr (t_nBits != ENumEntries * ENumBitsPerEntry)
		{
			umint NumEntries = ENumEntries - 1;
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

	template <umint t_nBits>
	template <bool tf_bValue>
	inline_small bool TCBitArray<t_nBits>::f_SetBitRangeWholeEntrySet(umint _StartBit, umint _nBits)
	{
		DMibFastCheck(_StartBit + _nBits <= t_nBits);
		umint iEntry = _StartBit / ENumBitsPerEntry;
		umint iSubBit = _StartBit % ENumBitsPerEntry;

		umint nBitsToSet = fg_Min(_nBits, ENumBitsPerEntry - iSubBit);
		DMibFastCheck(nBitsToSet == _nBits); // Only one entry supported

		auto NewEntry = fp_SetBitRange<tf_bValue>(mp_Entries[iEntry], iSubBit, nBitsToSet);
		mp_Entries[iEntry] = NewEntry;

		if constexpr (t_nBits != ENumEntries * ENumBitsPerEntry)
		{
			umint NumEntries = ENumEntries - 1;
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

	template <umint t_nBits>
	template <bool tf_bValue>
	inline_small bool TCBitArray<t_nBits>::f_SetBitWholeEntrySet(umint _Bit)
	{
		DMibFastCheck(_Bit < t_nBits);
		umint iEntry = _Bit / ENumBitsPerEntry;
		umint iSubBit = _Bit % ENumBitsPerEntry;
		auto NewEntry = fp_SetBit<tf_bValue>(mp_Entries[iEntry], iSubBit);
		mp_Entries[iEntry] = NewEntry;
		if constexpr (t_nBits != ENumEntries * ENumBitsPerEntry)
		{
			umint NumEntries = ENumEntries - 1;
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

	template <umint t_nBits>
	inline_small bool TCBitArray<t_nBits>::f_GetBit(umint _Bit) const
	{
		DMibFastCheck(_Bit < t_nBits);
		umint iEntry = _Bit / ENumBitsPerEntry;
		umint iSubBit = _Bit % ENumBitsPerEntry;

		return ((mp_Entries[iEntry] >> iSubBit) & CEntryType(1)) != 0;
	}

	template <umint t_nBits>
	inline_small bool TCBitArray<t_nBits>::f_GetWholeEntrySet(umint _Bit) const
	{
		DMibFastCheck(_Bit < t_nBits);
		umint iEntry = _Bit / ENumBitsPerEntry;

		if constexpr (t_nBits != ENumEntries * ENumBitsPerEntry)
		{
			umint NumEntries = ENumEntries - 1;
			if (iEntry < NumEntries)
				return mp_Entries[iEntry] == mcp_FullySet;
			else
				return mp_Entries[iEntry] == mcp_FullySetLast;
		}
		else
			return mp_Entries[iEntry] == mcp_FullySet;
	}

	template <umint t_nBits>
	inline_always aint TCBitArray<t_nBits>::fp_FindFreeBit(umint _StartBitApproximately, umint &_iEntry) const
	{
		umint iEntry = _StartBitApproximately / ENumBitsPerEntry;
		DMibFastCheck(iEntry < ENumEntries);
		if constexpr (t_nBits != ENumEntries * ENumBitsPerEntry)
		{
			umint NumEntries = ENumEntries - 1;
			if (iEntry < NumEntries)
			{
				CEntryType const &Entry = mp_Entries[iEntry];
				if (Entry != mcp_FullySet)
				{
					umint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
					_iEntry = iEntry;
					return FreeEntry;
				}
			}
			else
			{
				CEntryType Entry = mp_Entries[iEntry] | CEntryType(~mcp_FullySetLast);
				if (Entry != mcp_FullySet)
				{
					umint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
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
				umint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
				_iEntry = iEntry;
				return FreeEntry;
			}
		}
		return -1;
	}

	template <umint t_nBits>
	inline_always aint TCBitArray<t_nBits>::fp_FindFreeBitReverse(umint _StartBitApproximately, umint &_iEntry) const
	{
		aint iEntry = _StartBitApproximately / ENumBitsPerEntry;
		DMibFastCheck(iEntry < ENumEntries);
		if constexpr (t_nBits != ENumEntries * ENumBitsPerEntry)
		{
			aint NumEntries = ENumEntries - 1;
			if (iEntry < NumEntries)
			{
				CEntryType const &Entry = mp_Entries[iEntry];
				if (Entry != mcp_FullySet)
				{
					umint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
					_iEntry = iEntry;
					return FreeEntry;
				}
			}
			else
			{
				CEntryType Entry = mp_Entries[iEntry] | CEntryType(~mcp_FullySetLast);

				if (Entry != mcp_FullySet)
				{
					umint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
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
				umint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
				_iEntry = iEntry;
				return FreeEntry;
			}
		}
		return -1;
	}


	template <umint t_nBits>
	inline_small aint TCBitArray<t_nBits>::f_FindFreeBit(umint _StartBitApproximately) const
	{
		umint iEntry;
		aint iFreeEntry = fp_FindFreeBit(_StartBitApproximately, iEntry);
		if (iFreeEntry < 0)
			return -1;
		return iEntry * ENumBitsPerEntry + iFreeEntry;
	}

	template <umint t_nBits>
	inline_small aint TCBitArray<t_nBits>::f_FindFreeBitReverse(umint _StartBitApproximately) const
	{
		umint iEntry;
		aint iFreeEntry = fp_FindFreeBitReverse(_StartBitApproximately, iEntry);
		if (iFreeEntry < 0)
			return -1;
		return iEntry * ENumBitsPerEntry + iFreeEntry;
	}

	template <umint t_nBits>
	inline_small aint TCBitArray<t_nBits>::f_FindFreeBitAndSet(umint _StartBitApproximately)
	{
		umint iEntry;
		aint iFreeEntry = fp_FindFreeBit(_StartBitApproximately, iEntry);
		if (iFreeEntry < 0)
			return -1;

		mp_Entries[iEntry] = fp_SetBit(mp_Entries[iEntry], iFreeEntry, 1);

		return iEntry * ENumBitsPerEntry + iFreeEntry;
	}

	template <umint t_nBits>
	inline_small aint TCBitArray<t_nBits>::f_FindFreeBitReverseAndSet(umint _StartBitApproximately)
	{
		umint iEntry;
		aint iFreeEntry = fp_FindFreeBitReverse(_StartBitApproximately, iEntry);
		if (iFreeEntry < 0)
			return -1;

		mp_Entries[iEntry] = fp_SetBit(mp_Entries[iEntry], iFreeEntry, 1);

		return iEntry * ENumBitsPerEntry + iFreeEntry;
	}

	template <umint t_nBits>
	inline_small bool TCBitArray<t_nBits>::f_IsFullySet() const
	{
		if constexpr (t_nBits != ENumEntries * ENumBitsPerEntry)
		{
			umint nEntries = ENumEntries - 1;
			for (umint i = 0; i < nEntries; ++i)
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
			for (umint i = 0; i < ENumEntries; ++i)
			{
				CEntryType const &Entry = mp_Entries[i];
				if (Entry != mcp_FullySet)
					return false;
			}
		}
		return true;
	}

	template <umint t_nBits>
	inline_small bool TCBitArray<t_nBits>::f_IsFullyFree() const
	{
		if constexpr (t_nBits != ENumEntries * ENumBitsPerEntry)
		{
			umint nEntries = ENumEntries - 1;
			for (umint i = 0; i < nEntries; ++i)
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
			for (umint i = 0; i < ENumEntries; ++i)
			{
				CEntryType const &Entry = mp_Entries[i];
				if (Entry != 0)
					return false;
			}
		}
		return true;
	}

	template <umint t_nBits>
	inline_always aint TCBitArray<t_nBits>::fp_FindFreeBit(umint &_iEntry) const
	{
		if constexpr (t_nBits != ENumEntries * ENumBitsPerEntry)
		{
			for (umint i = 0; i < ENumEntries - 1; ++i)
			{
				CEntryType const &Entry = mp_Entries[i];
				if (Entry != mcp_FullySet)
				{
					umint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
					_iEntry = i;
					return FreeEntry;
				}
			}
//				if (ENumEntries > 1)
			{
				CEntryType Entry = mp_Entries[ENumEntries - 1] | CEntryType(~mcp_FullySetLast);
				if (Entry != mcp_FullySet)
				{
					umint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
					_iEntry = ENumEntries - 1;
					return FreeEntry;
				}
			}
		}
		else
		{
			for (umint i = 0; i < ENumEntries; ++i)
			{
				CEntryType const &Entry = mp_Entries[i];
				if (Entry != mcp_FullySet)
				{
					umint FreeEntry = fg_GetLowestBitSetNoZero(CEntryType(~Entry));
					_iEntry = i;
					return FreeEntry;
				}
			}
		}
		return -1;
	}

	template <umint t_nBits>
	inline_always aint TCBitArray<t_nBits>::fp_FindFreeBitReverse(umint &_iEntry) const
	{
		if constexpr (t_nBits != ENumEntries * ENumBitsPerEntry)
		{
			{
				CEntryType Entry = mp_Entries[ENumEntries - 1] | (CEntryType(~mcp_FullySetLast));
				if (Entry != mcp_FullySet)
				{
					umint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
					_iEntry = ENumEntries - 1;
					return FreeEntry;
				}
			}
			for (aint i = ENumEntries - 2; i >= 0; --i)
			{
				CEntryType const &Entry = mp_Entries[i];
				if (Entry != mcp_FullySet)
				{
					umint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
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
					umint FreeEntry = fg_GetHighestBitSetNoZero(CEntryType(~Entry));
					_iEntry = i;
					return FreeEntry;
				}
			}
		}
		return -1;
	}

	template <umint t_nBits>
	inline_small aint TCBitArray<t_nBits>::f_FindFreeBit() const
	{
		umint iEntry;
		aint iFreeEntry = fp_FindFreeBit(iEntry);
		if (iFreeEntry < 0)
			return -1;
		return iEntry * ENumBitsPerEntry + iFreeEntry;
	}

	template <umint t_nBits>
	inline_small aint TCBitArray<t_nBits>::f_FindFreeBitReverse() const
	{
		umint iEntry;
		aint iFreeEntry = fp_FindFreeBitReverse(iEntry);
		if (iFreeEntry < 0)
			return -1;
		return iEntry * ENumBitsPerEntry + iFreeEntry;
	}

	template <umint t_nBits>
	template <typename t_CFunctor>
	void TCBitArray<t_nBits>::f_EnumFreeBits(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		DMibFastCheck(_StartBit < _EndBit);
		DMibFastCheck(_EndBit <= t_nBits);

		umint iEntry = _StartBit / ENumBitsPerEntry;
		umint iSubBit = _StartBit % ENumBitsPerEntry;

		umint iEntryForward = iEntry;
		umint iSubBitForward = iSubBit;

		umint EntryStartBit = iEntryForward * ENumBitsPerEntry;

		while (EntryStartBit < _EndBit)
		{
			CEntryType Entry = CEntryType(~(mp_Entries[iEntryForward]));
			Entry >>= iSubBitForward;

			while (Entry)
			{
				umint iFreeEntry = fg_GetLowestBitSetNoZero(Entry);
				umint iFinalEntry = EntryStartBit + iFreeEntry + iSubBitForward;
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

	template <umint t_nBits>
	template <typename t_CFunctor>
	void TCBitArray<t_nBits>::f_EnumSetBits(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		DMibFastCheck(_StartBit < _EndBit);
		DMibFastCheck(_EndBit <= t_nBits);

		umint iEntry = _StartBit / ENumBitsPerEntry;
		umint iSubBit = _StartBit % ENumBitsPerEntry;

		umint iEntryForward = iEntry;
		umint iSubBitForward = iSubBit;

		umint EntryStartBit = iEntryForward * ENumBitsPerEntry;

		while (EntryStartBit < _EndBit)
		{
			CEntryType Entry = mp_Entries[iEntryForward];
			Entry >>= iSubBitForward;

			while (Entry)
			{
				umint iFreeEntry = fg_GetLowestBitSetNoZero(Entry);
				umint iFinalEntry = EntryStartBit + iFreeEntry + iSubBitForward;
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

	template <umint t_nBits>
	template <typename t_CFunctor>
	void TCBitArray<t_nBits>::f_EnumFreeBitRanges(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		DMibFastCheck(_StartBit < _EndBit);
		DMibFastCheck(_EndBit <= t_nBits);

		umint iEntry = _StartBit / ENumBitsPerEntry;
		umint iSubBit = _StartBit % ENumBitsPerEntry;

		umint iEntryForward = iEntry;
		umint iSubBitForward = iSubBit;

		umint EntryStartBit = iEntryForward * ENumBitsPerEntry;

		while (EntryStartBit < _EndBit)
		{
			CEntryType Entry = CEntryType(~(mp_Entries[iEntryForward]));
			Entry >>= iSubBitForward;

			while (Entry)
			{
				umint iFreeEntry = fg_GetLowestBitSetNoZero(Entry);
				umint iStartFinalEntry = EntryStartBit + iFreeEntry + iSubBitForward;
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

					umint iEndFinalEntry = EntryStartBit;

					while (iEndFinalEntry < _EndBit)
					{
						auto Entry = mp_Entries[iEntryForward];
						if (Entry)
						{
							umint iSetEntry = fg_GetLowestBitSetNoZero(Entry);
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
					umint iSetEntry = fg_GetLowestBitSetNoZero(EndEntry);

					umint iEndFinalEntry = EntryStartBit + iSetEntry + iSubBitForward;
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

	template <umint t_nBits>
	template <typename t_CFunctor>
	void TCBitArray<t_nBits>::f_EnumSetBitRanges(t_CFunctor &&_Functor, umint _StartBit, umint _EndBit) const
	{
		DMibFastCheck(_StartBit < _EndBit);
		DMibFastCheck(_EndBit <= t_nBits);

		umint iEntry = _StartBit / ENumBitsPerEntry;
		umint iSubBit = _StartBit % ENumBitsPerEntry;

		umint iEntryForward = iEntry;
		umint iSubBitForward = iSubBit;

		umint EntryStartBit = iEntryForward * ENumBitsPerEntry;

		while (EntryStartBit < _EndBit)
		{
			CEntryType Entry = mp_Entries[iEntryForward];
			Entry >>= iSubBitForward;

			while (Entry)
			{
				umint iFreeEntry = fg_GetLowestBitSetNoZero(Entry);
				umint iStartFinalEntry = EntryStartBit + iFreeEntry + iSubBitForward;
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

					umint iEndFinalEntry = EntryStartBit;

					while (iEndFinalEntry < _EndBit)
					{
						auto Entry = CEntryType(~(mp_Entries[iEntryForward]));
						if (Entry)
						{
							umint iSetEntry = fg_GetLowestBitSetNoZero(Entry);
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
					umint iSetEntry = fg_GetLowestBitSetNoZero(EndEntry);

					umint iEndFinalEntry = EntryStartBit + iSetEntry + iSubBitForward;
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

	template <umint t_nBits>
	inline_small aint TCBitArray<t_nBits>::f_FindFreeBitAndSet()
	{
		umint iEntry;
		aint iFreeEntry = fp_FindFreeBit(iEntry);
		if (iFreeEntry < 0)
			return -1;

		mp_Entries[iEntry] = fp_SetBit(mp_Entries[iEntry], iFreeEntry, 1);

		return iEntry * ENumBitsPerEntry + iFreeEntry;
	}

	template <umint t_nBits>
	inline_small aint TCBitArray<t_nBits>::f_FindFreeBitReverseAndSet()
	{
		umint iEntry;
		aint iFreeEntry = fp_FindFreeBitReverse(iEntry);
		if (iFreeEntry < 0)
			return -1;

		mp_Entries[iEntry] = fp_SetBit(mp_Entries[iEntry], iFreeEntry, 1);

		return iEntry * ENumBitsPerEntry + iFreeEntry;
	}

	template <umint t_nBits>
	inline_small aint TCBitArray<t_nBits>::f_FindUpperBound(umint _StartBit) const
	{
		DMibFastCheck(_StartBit < t_nBits);

		umint StartEntry = _StartBit / ENumBitsPerEntry;
		umint StartBit = _StartBit % ENumBitsPerEntry;

		CEntryType const &Entry = mp_Entries[StartEntry];
		if (Entry)
		{
			aint FoundBit = fg_GetLowestBitSet(Entry >> StartBit);
			if (FoundBit >= 0)
				return StartEntry * ENumBitsPerEntry + StartBit + FoundBit;
		}

		++StartEntry;

		for (umint i = StartEntry; i < ENumEntries; ++i)
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

