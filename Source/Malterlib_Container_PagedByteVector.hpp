// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NContainer
{
	CPagedByteVector::CPagedByteVector(umint _PageSize)
		: mp_PageSize(_PageSize)
		, mp_nBytes(0)
		, mp_iFirstPageStart(0)
		, mp_iLastPageEnd(0)
	{
	}

	bool CPagedByteVector::f_IsEmpty() const
	{
		if (mp_Pages.f_IsEmpty())
			return true;
		else
			return false;
	}

	umint CPagedByteVector::f_GetLen() const
	{
		return mp_nBytes;
	}

	umint CPagedByteVector::f_GetPageSize() const
	{
		return mp_PageSize;
	}

	umint CPagedByteVector::f_GetFirstPageSpace() const
	{
		if (f_IsEmpty())
			return mp_PageSize;
		else
			return mp_PageSize - mp_iFirstPageStart;
	}

	// tf_FReader is of the format: bool (umint _iStart, uint8 const *_pPtr, umint _nBytes)
	template <typename tf_FReader>
	bool CPagedByteVector::f_Read(umint _iStart, umint _nBytes, tf_FReader &&_fReader) const
	{
		if ((_nBytes + _iStart) > mp_nBytes)
			return false;

		umint iLastPage = mp_Pages.f_GetLen() - 1;

		umint iCurPage = (_iStart + mp_iFirstPageStart) / mp_PageSize;
		umint iCurPagePos = (_iStart + mp_iFirstPageStart) - (iCurPage * mp_PageSize);

		while (_nBytes)
		{
			umint nPageBytes = mp_PageSize - iCurPagePos;
			if (iCurPage == iLastPage)
				nPageBytes -= (mp_PageSize - mp_iLastPageEnd);

			umint nBlockBytes = fg_Min(_nBytes, nPageBytes);

			if (!_fReader(_iStart, (uint8 const *)mp_Pages[iCurPage].f_Get() + iCurPagePos, nBlockBytes))
				break;

			_iStart += nBlockBytes;
			_nBytes -= nBlockBytes;
			++iCurPage;
			iCurPagePos = 0;
		}

		return true;
	}

	// tf_FReader is of the format: bool (umint _iStart, uint8 const *_pPtr, umint _nBytes)
	template <typename tf_FMutate>
	bool CPagedByteVector::f_Mutate(umint _iStart, umint _nBytes, tf_FMutate &&_fMutate) const
	{
		if ((_nBytes + _iStart) > mp_nBytes)
			return false;

		umint iLastPage = mp_Pages.f_GetLen() - 1;

		umint iCurPage = (_iStart + mp_iFirstPageStart) / mp_PageSize;
		umint iCurPagePos = (_iStart + mp_iFirstPageStart) - (iCurPage * mp_PageSize);

		while (_nBytes)
		{
			umint nPageBytes = mp_PageSize - iCurPagePos;
			if (iCurPage == iLastPage)
				nPageBytes -= (mp_PageSize - mp_iLastPageEnd);

			umint nBlockBytes = fg_Min(_nBytes, nPageBytes);

			if (!_fMutate(_iStart, mp_Pages[iCurPage].f_Get() + iCurPagePos, nBlockBytes))
				break;

			_iStart += nBlockBytes;
			_nBytes -= nBlockBytes;
			++iCurPage;
			iCurPagePos = 0;
		}

		return true;
	}

	// tf_FReader is of the format: bool (umint _iStart, uint8 const *_pPtr, umint _nBytes)
	template <typename tf_FReader>
	bool CPagedByteVector::f_ReadFront(umint _nBytes, tf_FReader &&_fReader) const
	{
		return f_Read(0, _nBytes, fg_Forward<tf_FReader>(_fReader));
	}

	// tf_FReader is of the format: bool (umint _iStart, uint8 const *_pPtr, umint _nBytes)
	template <typename tf_FReader>
	bool CPagedByteVector::f_ReadFront(tf_FReader &&_fReader) const
	{
		return f_Read(0, mp_nBytes, fg_Forward<tf_FReader>(_fReader));
	}

	// tf_FReader is of the format: bool (umint _iStart, uint8 const *_pPtr, umint _nBytes)
	template <typename tf_FReader>
	bool CPagedByteVector::f_ReadBack(umint _nBytes, tf_FReader &&_fReader) const
	{
		return f_Read(mp_nBytes - _nBytes, _nBytes, fg_Forward<tf_FReader>(_fReader));
	}

	// tf_FReader is of the format: void (umint _iStart, uint8 const *_pPtr, umint _nBytes, umint _nTotalBytes)
	template <typename tf_FReader>
	bool CPagedByteVector::f_ReadFrontUntil(uint8 const *_pMatch, umint _nMatchBytes, umint &o_iPos, tf_FReader &&_fReader) const
	{
		umint iEnd;

		if (f_FindFront(_pMatch, _nMatchBytes, iEnd))
		{
			f_ReadFront
				(
					iEnd
					, [&](umint _iStart, uint8 const *_pPtr, umint _nBytes) -> bool
					{
						return fg_Forward<tf_FReader>(_fReader)(_iStart, _pPtr, _nBytes, iEnd);
					}
				)
			;

			o_iPos = iEnd;
			return true;
		}
		else
			return false;
	}

	void CPagedByteVector::fp_GetPageInfo(umint _iPage, umint &o_PageSize, umint &o_iPageStart)
	{
		o_PageSize = mp_PageSize;
		o_iPageStart = 0;

		if (_iPage == 0)
		{
			o_PageSize -= mp_iFirstPageStart;
			o_iPageStart = mp_iFirstPageStart;
		}

		if (_iPage == mp_Pages.f_GetLen())
			o_PageSize -= (mp_PageSize - mp_iLastPageEnd);
	}

	// tf_FReader is of the format: void (umint _iStart, uint8 const *_pPtr, umint _nBytes, umint _nTotalBytes)
	template <typename tf_FReader>
	CPagedByteVector::EMatchResult CPagedByteVector::f_ReadFrontUntilEx(uint8 const *_pMatch, umint _nMatchBytes, umint &o_iPos, tf_FReader &&_fReader) const
	{
		umint iEnd = f_GetLen();
		EMatchResult Result = f_FindFrontEx(_pMatch, _nMatchBytes, iEnd);

		// The return value does not matter here. Either iEnd is set to the start of a (potential)
		// match or it is left as the end of the buffer.

		f_ReadFront
			(
				iEnd
				, [&](umint _iStart, uint8 const *_pPtr, umint _nBytes) -> bool
				{
					return fg_Forward<tf_FReader>(_fReader)(_iStart, _pPtr, _nBytes, iEnd);
				}
			)
		;

		o_iPos = iEnd;
		return Result;
	}

	///
	/// Stream
	///	======

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::fp_SetPositionInternal(NStream::CFilePos _Pos)
	{
		if ((_Pos < 0) || fg_SafeLargerThan(_Pos, TCLimitsInt<umint>::mc_Max))
			DMibError("Memory stream positions are limited to 0 -> TCLimitsInt<umint>::mc_Max");

		mp_Position = _Pos;
	}

	template <typename t_CInherit>
	TCBinaryStreamPagedByteVector<t_CInherit>::TCBinaryStreamPagedByteVector(CPagedByteVector &_Buffer)
		: mp_Buffer(_Buffer)
	{
		mp_Position = 0;
		mp_Length = _Buffer.f_GetLen();
	}

	template <typename t_CInherit>
	TCBinaryStreamPagedByteVector<t_CInherit>::~TCBinaryStreamPagedByteVector()
	{
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_ResetStream()
	{
		mp_Position = 0;
		mp_Length = 0;
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_Clear()
	{
		mp_Position = 0;
		mp_Length = 0;
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_FeedBytes(void const *_pMem, umint _nBytes)
	{
		mp_Buffer.f_InsertBack((uint8 const *)_pMem, _nBytes);
		mp_Position += _nBytes;
		if (mp_Position > mp_Length)
			mp_Length = mp_Position;
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_ConsumeBytes(void *_pMem, umint _nBytes)
	{
		DMibPDebugBreak; // Not supported
	}

	template <typename t_CInherit>
	bool TCBinaryStreamPagedByteVector<t_CInherit>::f_IsValid() const
	{
		return true;
	}

	template <typename t_CInherit>
	bool TCBinaryStreamPagedByteVector<t_CInherit>::f_IsAtEndOfStream() const
	{
		DMibPDebugBreak; // Not supported
		return mp_Position == mp_Length;
	}

	template <typename t_CInherit>
	NStream::CFilePos TCBinaryStreamPagedByteVector<t_CInherit>::f_GetPosition() const
	{
		return mp_Position;
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_SetPosition(NStream::CFilePos _Pos)
	{
		fp_SetPositionInternal(_Pos);
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_SetPositionFromEnd(NStream::CFilePos _Pos)
	{
		fp_SetPositionInternal(mp_Length + _Pos);
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_AddPosition(NStream::CFilePos _Pos)
	{
		fp_SetPositionInternal(mp_Position + _Pos);
	}

	template <typename t_CInherit>
	bool TCBinaryStreamPagedByteVector<t_CInherit>::f_IsValidReadPosition(NStream::CFilePos _Pos) const
	{
		return _Pos >= 0 && _Pos < NStream::CFilePos(mp_Length);
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_Flush(bool _bLocalCacheOnly)
	{
	}

	template <typename t_CInherit>
	NStream::CFilePos TCBinaryStreamPagedByteVector<t_CInherit>::f_GetLength() const
	{
		return mp_Length;
	}

	template <typename t_CInherit>
	umint TCBinaryStreamPagedByteVector<t_CInherit>::f_ContainerLengthLimit() const
	{
		return NStream::fg_CapLengthLimit(f_GetLength() - f_GetPosition());
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_SetLength(NStream::CFilePos _Length)
	{
		DMibPDebugBreak; // Not supported
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_SetCacheSize(umint _CacheSize)
	{
	}

	template <typename t_CInherit>
	TCBinaryStreamPagedByteVectorPtr<t_CInherit>::TCBinaryStreamPagedByteVectorPtr()
	{
	}

	template <typename t_CInherit>
	TCBinaryStreamPagedByteVectorPtr<t_CInherit>::~TCBinaryStreamPagedByteVectorPtr()
	{
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_OpenRead(CPagedByteVector const &_Buffer)
	{
		f_OpenRead(_Buffer, 0, _Buffer.f_GetLen());
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_OpenRead(CPagedByteVector const &_Buffer, umint _Offset, umint _Length)
	{
		if (_Offset > _Buffer.f_GetLen() || _Length > _Buffer.f_GetLen() - _Offset)
			DMibError("Paged byte vector stream range exceeds buffer length");

		mp_pBuffer = &_Buffer;
		mp_BaseOffset = _Offset;
		mp_Length = _Length;
		mp_Position = 0;
		fp_RebuildPageCache();
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::fp_RebuildPageCache()
	{
		if (mp_pBuffer == nullptr || mp_Position >= mp_Length)
		{
			mp_pCurrentPage = nullptr;
			mp_nCurrentPageRemaining = 0;
			return;
		}

		CPagedByteVector const &Buffer = *mp_pBuffer;
		umint AbsolutePos = mp_BaseOffset + mp_Position + Buffer.mp_iFirstPageStart;
		umint iCurPage = AbsolutePos / Buffer.mp_PageSize;
		umint iCurPagePos = AbsolutePos - iCurPage * Buffer.mp_PageSize;
		umint iLastPage = Buffer.mp_Pages.f_GetLen() - 1;

		umint nPageBytes = Buffer.mp_PageSize - iCurPagePos;
		if (iCurPage == iLastPage)
			nPageBytes -= (Buffer.mp_PageSize - Buffer.mp_iLastPageEnd);

		// Cap by remaining bytes in our window.
		umint nWindowRemaining = mp_Length - mp_Position;
		if (nPageBytes > nWindowRemaining)
			nPageBytes = nWindowRemaining;

		mp_iCurrentPage = iCurPage;
		mp_pCurrentPage = static_cast<uint8 const *>(Buffer.mp_Pages[iCurPage].f_Get()) + iCurPagePos;
		mp_nCurrentPageRemaining = nPageBytes;
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::fp_AdvanceToNextPage()
	{
		// Caller guarantees mp_pBuffer != nullptr, mp_iCurrentPage is the page just exhausted,
		// and mp_Position < mp_Length (more data ahead). The next page always begins at offset 0,
		// so no division or modulo is needed.
		CPagedByteVector const &Buffer = *mp_pBuffer;
		++mp_iCurrentPage;

		umint nPageBytes;
		if (mp_iCurrentPage == Buffer.mp_Pages.f_GetLen() - 1)
			nPageBytes = Buffer.mp_iLastPageEnd;
		else
			nPageBytes = Buffer.mp_PageSize;

		umint nWindowRemaining = mp_Length - mp_Position;
		if (nPageBytes > nWindowRemaining)
			nPageBytes = nWindowRemaining;

		mp_pCurrentPage = static_cast<uint8 const *>(Buffer.mp_Pages[mp_iCurrentPage].f_Get());
		mp_nCurrentPageRemaining = nPageBytes;
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_FeedBytes(void const *_pMem, umint _nBytes)
	{
		DMibError("Paged byte vector read stream cannot be written to");
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_ConsumeBytes(void *_pMem, umint _nBytes)
	{
		if (mp_Length - mp_Position < _nBytes) [[unlikely]]
			this->fp_ThrowEndOfStreamException();

		if (_nBytes <= mp_nCurrentPageRemaining) [[likely]]
		{
			NMemory::fg_MemCopy(_pMem, mp_pCurrentPage, _nBytes);
			mp_pCurrentPage += _nBytes;
			mp_nCurrentPageRemaining -= _nBytes;
			mp_Position += _nBytes;
			return;
		}

		uint8 *pDest = static_cast<uint8 *>(_pMem);
		while (_nBytes != 0)
		{
			if (mp_nCurrentPageRemaining == 0)
				fp_AdvanceToNextPage();

			umint nCopy = _nBytes < mp_nCurrentPageRemaining ? _nBytes : mp_nCurrentPageRemaining;
			NMemory::fg_MemCopy(pDest, mp_pCurrentPage, nCopy);
			mp_pCurrentPage += nCopy;
			mp_nCurrentPageRemaining -= nCopy;
			mp_Position += nCopy;
			pDest += nCopy;
			_nBytes -= nCopy;
		}
	}

	template <typename t_CInherit>
	bool TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_IsValid() const
	{
		return mp_pBuffer != nullptr;
	}

	template <typename t_CInherit>
	bool TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_IsAtEndOfStream() const
	{
		return mp_Position == mp_Length;
	}

	template <typename t_CInherit>
	NStream::CFilePos TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_GetPosition() const
	{
		return mp_Position;
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::fp_SetPositionInternal(NStream::CFilePos _Pos)
	{
		if (_Pos < 0 || fg_SafeLargerThan(_Pos, TCLimitsInt<umint>::mc_Max))
			DMibError("Paged byte vector stream positions are limited to 0 -> TCLimitsInt<umint>::mc_Max");

		if (umint(_Pos) > mp_Length)
			DMibError("Position is past end of stream");

		mp_Position = umint(_Pos);
		fp_RebuildPageCache();
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_SetPosition(NStream::CFilePos _Pos)
	{
		fp_SetPositionInternal(_Pos);
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_SetPositionFromEnd(NStream::CFilePos _Pos)
	{
		fp_SetPositionInternal(NStream::CFilePos(mp_Length) + _Pos);
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_AddPosition(NStream::CFilePos _Pos)
	{
		fp_SetPositionInternal(NStream::CFilePos(mp_Position) + _Pos);
	}

	template <typename t_CInherit>
	bool TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_IsValidReadPosition(NStream::CFilePos _Pos) const
	{
		return _Pos >= 0 && _Pos < NStream::CFilePos(mp_Length);
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_Flush(bool _bLocalCacheOnly)
	{
	}

	template <typename t_CInherit>
	NStream::CFilePos TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_GetLength() const
	{
		return mp_Length;
	}

	template <typename t_CInherit>
	umint TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_ContainerLengthLimit() const
	{
		return NStream::fg_CapLengthLimit(f_GetLength() - f_GetPosition());
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_SetLength(NStream::CFilePos _Length)
	{
		DMibError("Paged byte vector read stream cannot change length");
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVectorPtr<t_CInherit>::f_SetCacheSize(umint _CacheSize)
	{
	}
}
