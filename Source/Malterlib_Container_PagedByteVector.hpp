// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

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
	template<typename tf_FReader>
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
	template<typename tf_FMutate>
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
	bool CPagedByteVector::f_ReadFrontUntil(uint8 const *_pMatch, umint _nMatchBytes, umint &_oPos, tf_FReader &&_fReader) const
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

			_oPos = iEnd;
			return true;
		}
		else
			return false;
	}

	void CPagedByteVector::fp_GetPageInfo(umint _iPage, umint &_oPageSize, umint &_oPageStart)
	{
		_oPageSize = mp_PageSize;
		_oPageStart = 0;

		if (_iPage == 0)
		{
			_oPageSize -= mp_iFirstPageStart;
			_oPageStart = mp_iFirstPageStart;
		}

		if (_iPage == mp_Pages.f_GetLen())
		{
			_oPageSize -= (mp_PageSize - mp_iLastPageEnd);
		}
	}

	// tf_FReader is of the format: void (umint _iStart, uint8 const *_pPtr, umint _nBytes, umint _nTotalBytes)
	template <typename tf_FReader>
	CPagedByteVector::EMatchResult CPagedByteVector::f_ReadFrontUntilEx(uint8 const *_pMatch, umint _nMatchBytes, umint &_oPos, tf_FReader &&_fReader) const
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

		_oPos = iEnd;
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

		m_Position = _Pos;
	}

	template <typename t_CInherit>
	TCBinaryStreamPagedByteVector<t_CInherit>::TCBinaryStreamPagedByteVector(CPagedByteVector &_Buffer)
		: m_Buffer(_Buffer)
	{
		m_Position = 0;
		m_Length = _Buffer.f_GetLen();
	}

	template <typename t_CInherit>
	TCBinaryStreamPagedByteVector<t_CInherit>::~TCBinaryStreamPagedByteVector()
	{
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_ResetStream()
	{
		m_Position = 0;
		m_Length = 0;
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_Clear()
	{
		m_Position = 0;
		m_Length = 0;
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_FeedBytes(const void *_pMem, umint _nBytes)
	{
		m_Buffer.f_InsertBack((uint8 const *)_pMem, _nBytes);
		m_Position += _nBytes;
		if (m_Position > m_Length)
			m_Length = m_Position;

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
		return m_Position == m_Length;
	}

	template <typename t_CInherit>
	NStream::CFilePos TCBinaryStreamPagedByteVector<t_CInherit>::f_GetPosition() const
	{
		return m_Position;
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_SetPosition(NStream::CFilePos _Pos)
	{
		fp_SetPositionInternal(_Pos);
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_SetPositionFromEnd(NStream::CFilePos _Pos)
	{
		fp_SetPositionInternal(m_Length + _Pos);
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_AddPosition(NStream::CFilePos _Pos)
	{
		fp_SetPositionInternal(m_Position + _Pos);
	}

	template <typename t_CInherit>
	bool TCBinaryStreamPagedByteVector<t_CInherit>::f_IsValidReadPosition(NStream::CFilePos _Pos) const
	{
		return _Pos >= 0 && _Pos < NStream::CFilePos(m_Length);
	}

	template <typename t_CInherit>
	void TCBinaryStreamPagedByteVector<t_CInherit>::f_Flush(bool _bLocalCacheOnly)
	{
	}

	template <typename t_CInherit>
	NStream::CFilePos TCBinaryStreamPagedByteVector<t_CInherit>::f_GetLength() const
	{
		return m_Length;
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
}
