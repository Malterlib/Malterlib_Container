// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "Malterlib_Container_SharedByteVector.h"

namespace NMib::NContainer
{
	CSharedByteVector::CSharedByteVector(CIOByteVector &&_Vector)
		: mp_pVector(fg_Construct(fg_Move(_Vector)))
	{
		mp_nBytes = mp_pVector->f_GetLen();
	}

	CSharedByteVector::CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> &&_pVector)
		: mp_pVector(fg_Move(_pVector))
	{
		if (mp_pVector)
			mp_nBytes = mp_pVector->f_GetLen();
	}

	CSharedByteVector::CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> const &_pVector)
		: mp_pVector(_pVector)
	{
		if (mp_pVector)
			mp_nBytes = mp_pVector->f_GetLen();
	}

	CSharedByteVector::CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> const &_pVector, umint _Offset, umint _nBytes)
		: mp_pVector(_pVector)
		, mp_Offset(_Offset)
		, mp_nBytes(_nBytes)
	{
		DMibFastCheck(mp_pVector && _Offset <= mp_pVector->f_GetLen() && _nBytes <= mp_pVector->f_GetLen() - _Offset);
	}

	CSharedByteVector::CSharedByteVector(CSharedByteVector const &_Other, umint _Offset, umint _nBytes)
		: mp_pVector(_Other.mp_pVector)
		, mp_Offset(_Other.mp_Offset + _Offset)
		, mp_nBytes(_nBytes)
	{
		DMibFastCheck(_Offset <= _Other.mp_nBytes && _nBytes <= _Other.mp_nBytes - _Offset);
	}

	CSharedByteVector::CSharedByteVector(CSharedByteVector &&_Other)
		: mp_pVector(fg_Move(_Other.mp_pVector))
		, mp_Offset(_Other.mp_Offset)
		, mp_nBytes(_Other.mp_nBytes)
	{
		_Other.mp_Offset = 0;
		_Other.mp_nBytes = 0;
	}

	CSharedByteVector &CSharedByteVector::operator = (CSharedByteVector const &_Other)
	{
		DMibFastCheck(this != &_Other);

		mp_pVector = _Other.mp_pVector;
		mp_Offset = _Other.mp_Offset;
		mp_nBytes = _Other.mp_nBytes;

		return *this;
	}

	CSharedByteVector &CSharedByteVector::operator = (CSharedByteVector &&_Other)
	{
		mp_pVector = fg_Move(_Other.mp_pVector);
		mp_Offset = _Other.mp_Offset;
		mp_nBytes = _Other.mp_nBytes;
		_Other.mp_Offset = 0;
		_Other.mp_nBytes = 0;

		return *this;
	}

	uint8 const *CSharedByteVector::f_GetArray() const
	{
		if (!mp_pVector)
			return nullptr;

		// An empty vector has no array; offsetting it would be undefined even at zero
		uint8 const *pArray = mp_pVector->f_GetArray();
		if (!pArray)
			return nullptr;

		return pArray + mp_Offset;
	}

	umint CSharedByteVector::f_GetLen() const
	{
		return mp_nBytes;
	}

	bool CSharedByteVector::f_IsEmpty() const
	{
		return mp_nBytes == 0;
	}

	NStorage::TCSharedPointer<CIOByteVector const> const &CSharedByteVector::f_GetSharedPointer() const
	{
		return mp_pVector;
	}

	void CSharedByteVector::f_Clear()
	{
		mp_pVector.f_Clear();
		mp_Offset = 0;
		mp_nBytes = 0;
	}

	bool CSharedByteVector::operator == (CSharedByteVector const &_Other) const
	{
		if (mp_pVector == _Other.mp_pVector && mp_Offset == _Other.mp_Offset && mp_nBytes == _Other.mp_nBytes)
			return true;

		if (mp_nBytes != _Other.mp_nBytes)
			return false;

		uint8 const *pThis = f_GetArray();
		uint8 const *pOther = _Other.f_GetArray();
		for (umint i = 0; i < mp_nBytes; ++i)
		{
			if (pThis[i] != pOther[i])
				return false;
		}

		return true;
	}
}
