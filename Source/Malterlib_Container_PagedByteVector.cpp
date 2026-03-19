// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include "Malterlib_Container_PagedByteVector.h"

namespace NMib::NContainer
{
	namespace
	{
		struct CThreadLocal
		{
			umint m_PageSize = 0;
		};

		constinit NStorage::TCAggregate<NThread::TCThreadLocal<CThreadLocal>> g_ThreadLocal = {DAggregateInit};
	}

	CPagedByteVector::CPageSizeScope::CPageSizeScope(umint _PageSize)
	{
		(**g_ThreadLocal).m_PageSize = _PageSize;
	}

	CPagedByteVector::CPageSizeScope::~CPageSizeScope()
	{
		(**g_ThreadLocal).m_PageSize = 0;
	}

	only_parameters_aliased void CPagedByteVector::CPageAllocator::f_Free(void *_pBlock, umint _Size)
	{
		umint PageSize = (**g_ThreadLocal).m_PageSize;
		DMibFastCheck(PageSize != 0);
		NMemory::CAllocator_Heap::f_Free(_pBlock, PageSize);
	}

	CPagedByteVector::~CPagedByteVector()
	{
		CPageSizeScope PageScope(mp_PageSize);
		mp_Pages.f_Clear();
	}

	void CPagedByteVector::fp_InsertWhenEmpty(uint8 const *_pPtr, umint _nBytes)
	{
		umint nBytesLeft = _nBytes;
		umint nBlockBytes = 0;

		while (nBytesLeft)
		{
			uint8 *pNewPage = fp_InsertPage(false);

			nBlockBytes = fg_Min(nBytesLeft, mp_PageSize);

			NMemory::fg_MemCopy(pNewPage, _pPtr, nBlockBytes);

			nBytesLeft -= nBlockBytes;
			_pPtr += nBlockBytes;
		}

		mp_iFirstPageStart = 0;
		mp_iLastPageEnd = nBlockBytes;
		mp_nBytes = _nBytes;
	}

	void CPagedByteVector::f_InsertFront(uint8 const *_pPtr, umint _nBytes)
	{
		if (f_IsEmpty())
		{
			fp_InsertWhenEmpty(_pPtr, _nBytes);
			return;
		}

		umint nBytesLeft = _nBytes;

		uint8 const *pEndPtr = _pPtr + _nBytes;

		if (mp_iFirstPageStart > 0)
		{
			// There is space in the first page.
			umint nBytesFreeInFirstPage = mp_iFirstPageStart;

			umint nBlockBytes = fg_Min(nBytesLeft, nBytesFreeInFirstPage);

			mp_iFirstPageStart -= nBlockBytes;
			pEndPtr -= nBlockBytes;

			NMemory::fg_MemCopy(mp_Pages[0].f_Get() + mp_iFirstPageStart, pEndPtr, nBlockBytes);

			nBytesLeft -= nBlockBytes;
		}

		while(nBytesLeft)
		{
			umint nBlockBytes = fg_Min(nBytesLeft, mp_PageSize);

			uint8 *pNewPage = fp_InsertPage(true);

			mp_iFirstPageStart = mp_PageSize - nBytesLeft;
			pEndPtr -= nBlockBytes;

			NMemory::fg_MemCopy(pNewPage + mp_iFirstPageStart, pEndPtr, nBlockBytes);

			nBytesLeft -= nBlockBytes;
		}

		mp_nBytes += _nBytes;
	}

	void CPagedByteVector::f_InsertBack(uint8 const *_pPtr, umint _nBytes)
	{
		if (f_IsEmpty())
		{
			fp_InsertWhenEmpty(_pPtr, _nBytes);
			return;
		}

		umint nBytesLeft = _nBytes;

		if (mp_iLastPageEnd < mp_PageSize)
		{ // There is space in the last block
			umint nBlockBytes = fg_Min(nBytesLeft, mp_PageSize - mp_iLastPageEnd);

			NMemory::fg_MemCopy(mp_Pages.f_GetLast().f_Get() + mp_iLastPageEnd, _pPtr, nBlockBytes);

			_pPtr += nBlockBytes;
			nBytesLeft -= nBlockBytes;
			mp_iLastPageEnd += nBlockBytes;
		}

		while(nBytesLeft)
		{
			umint nBlockBytes = fg_Min(nBytesLeft, mp_PageSize);

			uint8 *pNewPage = fp_InsertPage(false);

			NMemory::fg_MemCopy(pNewPage, _pPtr, nBlockBytes);

			_pPtr += nBlockBytes;
			nBytesLeft -= nBlockBytes;

			mp_iLastPageEnd = nBlockBytes;
		}

		mp_nBytes += _nBytes;
	}

	void CPagedByteVector::f_RemoveFront(umint _nBytes)
	{
		umint iCurPage = 0;

		umint iLastPage = mp_Pages.f_GetLen() - 1;
		umint nBytes = _nBytes;

		while (nBytes)
		{
			umint nPageBytes = mp_PageSize;
			if (iCurPage == 0)
				nPageBytes -= mp_iFirstPageStart;
			if (iCurPage == iLastPage)
				nPageBytes -= (mp_PageSize - mp_iLastPageEnd);

			umint nBlockBytes = fg_Min(nPageBytes, nBytes);

			if (nBlockBytes == nPageBytes)
			{
				++iCurPage;
				mp_iFirstPageStart = 0;
			}
			else
				mp_iFirstPageStart += nBlockBytes;

			nBytes -= nBlockBytes;
		}

		mp_nBytes -= _nBytes;
		CPageSizeScope PageScope(mp_PageSize);
		mp_Pages.f_Remove(0, iCurPage);
	}

	void CPagedByteVector::f_RemoveBack(umint _nBytes)
	{
		umint iLastPage = mp_Pages.f_GetLen() - 1;
		umint iCurPage = iLastPage;

		umint nBytes = _nBytes;

		while (nBytes)
		{
			umint nPageBytes = mp_PageSize;
			if (iCurPage == 0)
				nPageBytes -= mp_iFirstPageStart;
			if (iCurPage == iLastPage)
				nPageBytes -= (mp_PageSize - mp_iLastPageEnd);

			umint nBlockBytes = fg_Min(nPageBytes, nBytes);

			if (nBlockBytes == nPageBytes)
			{
				--iCurPage;
				mp_iLastPageEnd = mp_PageSize;
			}
			else
				mp_iLastPageEnd -= nBlockBytes;

			nBytes -= nBlockBytes;
		}

		mp_nBytes -= _nBytes;
		CPageSizeScope PageScope(mp_PageSize);
		mp_Pages.f_Remove(iCurPage + 1, iLastPage - iCurPage);
	}

	// If _pMatch is found at the front it is removed and this returns true.
	// else this returns false and the data is untouched.
	bool CPagedByteVector::f_ExpectAndRemoveFront(uint8 const *_pMatch, umint _nMatchBytes)
	{
		return f_ExpectAndRemoveFrontEx(_pMatch, _nMatchBytes) == EMatchResult_Full;
	}

	CPagedByteVector::EMatchResult CPagedByteVector::f_ExpectAndRemoveFrontEx(uint8 const *_pMatch, umint _nMatchBytes)
	{
		EMatchResult Result = f_StartsWithEx(_pMatch, _nMatchBytes);

		if (Result == EMatchResult_Full)
			f_RemoveFront(_nMatchBytes);

		return Result;
	}

	uint8 *CPagedByteVector::fp_InsertPage(bool _bFront)
	{
		NStorage::TCUniquePointer<uint8, CPageAllocator> pNewPage = fg_Explicit((uint8 *)NMemory::fg_Alloc(mp_PageSize));

		if (_bFront)
			return mp_Pages.f_InsertFirst(fg_Move(pNewPage)).f_Get();
		else
			return mp_Pages.f_Insert(fg_Move(pNewPage)).f_Get();
	}

	bool CPagedByteVector::f_FindFront(uint8 const *_pMatch, umint _nMatchBytes, umint &_oPos) const
	{
		umint Pos = 0;

		if (f_FindFrontEx(_pMatch, _nMatchBytes, Pos) == EMatchResult_Full)
		{
			_oPos = Pos;
			return true;
		}
		else
			return false;
	}

	CPagedByteVector::EMatchResult CPagedByteVector::f_FindFrontEx(uint8 const *_pMatch, umint _nMatchBytes, umint &_oPos) const
	{
		bool bFound = false;
		bool bDoesNotMatch = false;
		umint iFoundPos = 0;

		f_ReadFront
			(
				mp_nBytes
				, [&](umint _iStart, uint8 const *_pPtr, umint _nBytes) -> bool
				{
					uint8 const *pEnd = _pPtr + _nBytes;
					for (uint8 const *pCurPos = _pPtr; pCurPos != pEnd; ++pCurPos)
					{
						if (*pCurPos == *_pMatch)
						{
							umint iMatchPos = 0;
							umint iCurPos = _iStart + (pCurPos - _pPtr);

							f_Read
								(
									iCurPos
									, _nBytes - iCurPos
									, [&](umint _iStart, uint8 const *_pPtr, umint _nBytes) -> bool
									{
										umint CmpLen = fg_Min(_nBytes, _nMatchBytes - iMatchPos);
										if (NMemory::fg_MemCmp(_pPtr, _pMatch + iMatchPos, CmpLen) == 0)
										{
											iMatchPos += CmpLen;

											if (iMatchPos == _nMatchBytes)
											{
												bFound = true;
												return false;
											}
											else
												return true;
										}
										else
										{
											bFound = false;
											bDoesNotMatch = true;
											return false;
										}
									}
								)
							;

							if (bFound)
							{
								iFoundPos = iCurPos;
								break;
							}
						}
					}

					return !bFound;
				}
			)
		;

		if (bFound)
		{
			_oPos = iFoundPos;
			return EMatchResult_Full;
		}
		else if (!bDoesNotMatch)
		{
			_oPos = iFoundPos;
			return EMatchResult_Partial;
		}
		else
			return EMatchResult_None;
	}

	CPagedByteVector::EMatchResult CPagedByteVector::f_StartsWithEx(uint8 const *_pMatch, umint _nMatchBytes)
	{
		bool bFound = false;
		bool bDoesNotMatch = false;
		umint iMatchPos = 0;

		f_ReadFront
			(
				_nMatchBytes
				, [&](umint _iStart, uint8 const *_pPtr, umint _nBytes) -> bool
				{
					if (NMemory::fg_MemCmp(_pMatch + iMatchPos, _pPtr, _nBytes) == 0)
					{
						iMatchPos += _nBytes;
						if (iMatchPos == _nMatchBytes)
						{
							bFound = true;
							return false;
						}
						else
							return true;
					}
					else
					{
						bFound = false;
						bDoesNotMatch = true;
						return false;
					}
				}
			)
		;

		if (bFound)
			return EMatchResult_Full;
		else if (!bDoesNotMatch)
			return EMatchResult_Partial;
		else
			return EMatchResult_None;
	}
}
