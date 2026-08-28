// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "Malterlib_Container_SharedByteVector.h"

namespace NMib::NContainer
{
	namespace
	{
		// The anchors for the vector-backed constructors: the bytes live in (or are shared
		// through) a vector, and the owner is what carries its lifetime once the view has
		// forgotten the concrete type
		struct CVectorOwner final : CVirtualDestroyBase
		{
			CVectorOwner(CIOByteVector &&_Vector)
				: m_Vector(fg_Move(_Vector))
			{
			}

			CIOByteVector m_Vector;
		};

		struct CSharedVectorOwner final : CVirtualDestroyBase
		{
			CSharedVectorOwner(NStorage::TCSharedPointer<CIOByteVector const> &&_pVector)
				: m_pVector(fg_Move(_pVector))
			{
			}

			NStorage::TCSharedPointer<CIOByteVector const> m_pVector;
		};

		// The anchor for exact sized raw allocations: no vector header shares the block, so
		// a power of two request costs exactly its size class in the memory manager
		struct CRawBytesOwner final : CVirtualDestroyBase
		{
			~CRawBytesOwner() override
			{
				if (m_pData)
					NMemory::CDefaultAllocator::f_Free(m_pData, m_nBytes);
			}

			uint8 *m_pData = nullptr;
			umint m_nBytes = 0;
		};
	}

	CSharedByteVector CSharedByteVector::fs_AllocateExact(umint _nBytes, uint8 *&o_pWritable)
	{
		NStorage::TCSharedPointer<CRawBytesOwner> pOwner = fg_Construct();
		pOwner->m_pData = (uint8 *)NMemory::CDefaultAllocator::f_Alloc(_nBytes);
		pOwner->m_nBytes = _nBytes;

		o_pWritable = pOwner->m_pData;

		CSharedByteVector Result;
		Result.mp_pData = pOwner->m_pData;
		Result.mp_nBytes = _nBytes;
		Result.mp_pOwner = pOwner.f_ShareAsConst();
		return Result;
	}

	CSharedByteVector::CSharedByteVector(CIOByteVector &&_Vector)
	{
		NStorage::TCSharedPointer<CVectorOwner> pOwner = fg_Construct(fg_Move(_Vector));

		mp_pData = pOwner->m_Vector.f_GetArray();
		mp_nBytes = pOwner->m_Vector.f_GetLen();
		mp_pOwner = pOwner.f_ShareAsConst();
	}

	CSharedByteVector::CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> &&_pVector)
	{
		if (!_pVector)
			return;

		mp_pData = _pVector->f_GetArray();
		mp_nBytes = _pVector->f_GetLen();

		NStorage::TCSharedPointer<CSharedVectorOwner> pOwner = fg_Construct(fg_Move(_pVector));
		mp_pOwner = pOwner.f_ShareAsConst();
	}

	CSharedByteVector::CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> const &_pVector)
		: CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const>(_pVector))
	{
	}

	CSharedByteVector::CSharedByteVector(void const *_pData, umint _nBytes, NStorage::TCSharedPointer<CVirtualDestroyBase const> &&_pOwner)
		: mp_pData((uint8 const *)_pData)
		, mp_nBytes(_nBytes)
		, mp_pOwner(fg_Move(_pOwner))
	{
	}

	CSharedByteVector::CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> const &_pVector, umint _Offset, umint _nBytes)
	{
		DMibFastCheck(_pVector && _Offset <= _pVector->f_GetLen() && _nBytes <= _pVector->f_GetLen() - _Offset);

		// An empty vector has no array; offsetting it would be undefined even at zero
		uint8 const *pArray = _pVector->f_GetArray();
		mp_pData = pArray ? pArray + _Offset : nullptr;
		mp_nBytes = _nBytes;

		NStorage::TCSharedPointer<CSharedVectorOwner> pOwner = fg_Construct(NStorage::TCSharedPointer<CIOByteVector const>(_pVector));
		mp_pOwner = pOwner.f_ShareAsConst();
	}

	CSharedByteVector::CSharedByteVector(CSharedByteVector const &_Other, umint _Offset, umint _nBytes)
		: mp_pData(_Other.mp_pData ? _Other.mp_pData + _Offset : nullptr)
		, mp_nBytes(_nBytes)
		, mp_pOwner(_Other.mp_pOwner)
	{
		DMibFastCheck(_Offset <= _Other.mp_nBytes && _nBytes <= _Other.mp_nBytes - _Offset);
	}

	CSharedByteVector::CSharedByteVector(CSharedByteVector &&_Other)
		: mp_pData(_Other.mp_pData)
		, mp_nBytes(_Other.mp_nBytes)
		, mp_pOwner(fg_Move(_Other.mp_pOwner))
	{
		_Other.mp_pData = nullptr;
		_Other.mp_nBytes = 0;
	}

	CSharedByteVector &CSharedByteVector::operator = (CSharedByteVector const &_Other)
	{
		DMibFastCheck(this != &_Other);

		mp_pData = _Other.mp_pData;
		mp_nBytes = _Other.mp_nBytes;
		mp_pOwner = _Other.mp_pOwner;

		return *this;
	}

	CSharedByteVector &CSharedByteVector::operator = (CSharedByteVector &&_Other)
	{
		mp_pData = _Other.mp_pData;
		mp_nBytes = _Other.mp_nBytes;
		mp_pOwner = fg_Move(_Other.mp_pOwner);
		_Other.mp_pData = nullptr;
		_Other.mp_nBytes = 0;

		return *this;
	}

	uint8 const *CSharedByteVector::f_GetArray() const
	{
		return mp_pData;
	}

	umint CSharedByteVector::f_GetLen() const
	{
		return mp_nBytes;
	}

	bool CSharedByteVector::f_IsEmpty() const
	{
		return mp_nBytes == 0;
	}

	bool CSharedByteVector::f_HasBuffer() const
	{
		return (bool)mp_pOwner;
	}

	void CSharedByteVector::f_Clear()
	{
		mp_pOwner.f_Clear();
		mp_pData = nullptr;
		mp_nBytes = 0;
	}

	bool CSharedByteVector::operator == (CSharedByteVector const &_Other) const
	{
		if (mp_pData == _Other.mp_pData && mp_nBytes == _Other.mp_nBytes)
			return true;

		if (mp_nBytes != _Other.mp_nBytes)
			return false;

		uint8 const *pThis = mp_pData;
		uint8 const *pOther = _Other.mp_pData;
		for (umint i = 0; i < mp_nBytes; ++i)
		{
			if (pThis[i] != pOther[i])
				return false;
		}

		return true;
	}

	bool CSharedByteVector::f_TryAppendContiguous(CSharedByteVector const &_Next)
	{
		if (!_Next.mp_nBytes)
			return true;

		if (mp_pOwner.f_Get() != _Next.mp_pOwner.f_Get() || !mp_pOwner)
			return false;

		if (mp_pData + mp_nBytes != _Next.mp_pData)
			return false;

		mp_nBytes += _Next.mp_nBytes;
		return true;
	}
}
