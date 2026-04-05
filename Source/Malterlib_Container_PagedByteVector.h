// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <Mib/Core/Core>

namespace NMib::NContainer
{
	class CPagedByteVector
	{
	public:
		enum EMatchResult
		{
			EMatchResult_None
			, EMatchResult_Partial
			, EMatchResult_Full
		};

		inline CPagedByteVector(umint _PageSize = 1024);
		~CPagedByteVector();

		CPagedByteVector(CPagedByteVector &&) = delete;
		CPagedByteVector(CPagedByteVector const &) = delete;
		CPagedByteVector &operator = (CPagedByteVector &&) = delete;
		CPagedByteVector &operator = (CPagedByteVector const &) = delete;

		inline bool f_IsEmpty() const;
		inline umint f_GetLen() const;

		inline umint f_GetPageSize() const;

		inline umint f_GetFirstPageSpace() const;

		void f_InsertFront(uint8 const *_pPtr, umint _nBytes);
		void f_InsertBack(uint8 const *_pPtr, umint _nBytes);

		void f_RemoveFront(umint _nBytes);
		void f_RemoveBack(umint _nBytes);

		// If _pMatch is found at the front it is removed and this returns true.
		// else this returns false and the data is untouched.
		bool f_ExpectAndRemoveFront(uint8 const *_pMatch, umint _nMatchBytes);

		// If _pMatch is found at the front it is removed and this returns EMatchResult_Full.
		// If the buffer ends before a whole _pMatch is found (but all existing buffer bytes match
		// _pMatch) then EMatchResult_Partial is returned and the buffer is untouched.
		// If the buffer cannot match _pMatch then EMatchResult_None is returned and the buffer
		// is untouched.
		EMatchResult f_ExpectAndRemoveFrontEx(uint8 const *_pMatch, umint _nMatchBytes);

		// tf_FReader is of the format: void (umint _iStart, uint8 const *_pPtr, umint _nBytes)
		template <typename tf_FReader>
		inline bool f_Read(umint _iStart, umint _nBytes, tf_FReader &&_fReader) const;

		// tf_FReader is of the format: void (umint _iStart, uint8  *_pPtr, umint _nBytes)
		template <typename tf_FReader>
		inline bool f_Mutate(umint _iStart, umint _nBytes, tf_FReader &&_fReader) const;

		// tf_FReader is of the format: void (umint _iStart, uint8 const *_pPtr, umint _nBytes)
		template <typename tf_FReader>
		inline bool f_ReadFront(umint _nBytes, tf_FReader &&_fReader) const;

		// tf_FReader is of the format: void (umint _iStart, uint8 const *_pPtr, umint _nBytes, umint _nTotalBytes)
		template <typename tf_FReader>
		inline bool f_ReadFront(tf_FReader &&_fReader) const;

		// tf_FReader is of the format: void (umint _iStart, uint8 const *_pPtr, umint _nBytes)
		template <typename tf_FReader>
		inline bool f_ReadBack(umint _nBytes, tf_FReader &&_fReader) const;

		bool f_FindFront(uint8 const *_pMatch, umint _nMatchBytes, umint &_oPos) const;

		// tf_FReader is of the format: void (umint _iStart, uint8 const *_pPtr, umint _nBytes, umint _nTotalBytes)
		template <typename tf_FReader>
		inline bool f_ReadFrontUntil(uint8 const *_pMatch, umint _nMatchBytes, umint &_oPos, tf_FReader &&_fReader) const;

		// Searches from the front for _pMatch.
		// If a full match is found EMatchResult_Full is returned and _oPos is set to the offset of
		// the start of _pMatch in the buffer.
		// If the buffer ends with a prefix of _pMatch EMatchResult_Partial is returned and _oPos is set
		// to the offset of the start of the potential match.
			// NOTE: This doesn't mean that a match will definitely occur at this pos in the future!
			// just that it could.
		// If the buffer does not contain either a full match of _pMatch or end with a partial match
		// then EMatchResult_None is returned and _oPos is unchanged.
		EMatchResult f_FindFrontEx(uint8 const *_pMatch, umint _nMatchBytes, umint &_oPos) const;

		// tf_FReader is of the format: void (umint _iStart, uint8 const *_pPtr, umint _nBytes, umint _nTotalBytes)
		// Reads bytes (fed to _fReader) until either a full match of _pMatch is found or the buffer ends
		// with a partial match for _pMatch, or (differing from f_ReadFrontUntil above) the buffer ends.
		// Return value works the same as for f_FindFrontEx
		template <typename tf_FReader>
		inline EMatchResult f_ReadFrontUntilEx(uint8 const *_pMatch, umint _nMatchBytes, umint &_oPos, tf_FReader &&_fReader) const;

		EMatchResult f_StartsWithEx(uint8 const *_pMatch, umint _nMatchBytes);

	private:
		struct CPageSizeScope
		{
			CPageSizeScope(umint _PageSize);
			~CPageSizeScope();

			DMibThreadLocalScopeDebugMember;
		};

		struct CPageAllocator : public NMemory::CAllocator_Heap
		{
			enum
			{
				mc_bIsDefault = false
			};
			only_parameters_aliased static void f_Free(void *_pBlock, umint _Size);
		};

		uint8 *fp_InsertPage(bool _bFront);
		void fp_InsertWhenEmpty(uint8 const *_pPtr, umint _nBytes);
		inline void fp_GetPageInfo(umint _iPage, umint &_oPageSize, umint &_oPageStart);

		NContainer::TCVector<NStorage::TCUniquePointer<uint8, CPageAllocator>> mp_Pages;
		umint mp_PageSize;
		umint mp_nBytes;
		umint mp_iFirstPageStart;
		umint mp_iLastPageEnd;
	};

	template <typename t_CInherit = NStream::CBinaryStreamLittleEndian>
	class TCBinaryStreamPagedByteVector : public t_CInherit
	{
	public:
		DMibStreamImplementOperators(TCBinaryStreamPagedByteVector);

		TCBinaryStreamPagedByteVector(CPagedByteVector &_Buffer);
		~TCBinaryStreamPagedByteVector();
		void f_ResetStream();
		void f_Clear();
		void f_FeedBytes(const void *_pMem, umint _nBytes);
		void f_ConsumeBytes(void *_pMem, umint _nBytes);
		bool f_IsValid() const;
		bool f_IsAtEndOfStream() const;
		NStream::CFilePos f_GetPosition() const;
		void f_SetPosition(NStream::CFilePos _Pos);
		void f_SetPositionFromEnd(NStream::CFilePos _Pos);
		void f_AddPosition(NStream::CFilePos _Pos);
		bool f_IsValidReadPosition(NStream::CFilePos _Pos) const;
		void f_Flush(bool _bLocalCacheOnly);
		NStream::CFilePos f_GetLength() const;
		umint f_ContainerLengthLimit() const;
		void f_SetLength(NStream::CFilePos _Length);
		void f_SetCacheSize(umint _CacheSize);

	protected:
		void fp_SetPositionInternal(NStream::CFilePos _Pos);
		DMibStreamImplementProtected(TCBinaryStreamPagedByteVector);

		umint m_Position;
		umint m_Length;
		CPagedByteVector &m_Buffer;

	private:
		TCBinaryStreamPagedByteVector(TCBinaryStreamPagedByteVector const &) = delete;
		TCBinaryStreamPagedByteVector &operator = (TCBinaryStreamPagedByteVector const &) = delete;
	};
}

#include "Malterlib_Container_PagedByteVector.hpp"
