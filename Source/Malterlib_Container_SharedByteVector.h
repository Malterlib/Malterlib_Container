// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Core/Core>
#include <Mib/Stream/Binary>

namespace NMib::NContainer
{
	// Shared immutable bytes; copies and subviews retain a type-erased owner.
	// The owner is destroyed on the thread that releases its last reference.
	// Producers must stop modifying the bytes before sharing them with concurrent readers.
	struct CSharedByteVector
	{
		CSharedByteVector() = default;

		explicit CSharedByteVector(CIOByteVector &&_Vector);
		explicit CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> &&_pVector);
		explicit CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> const &_pVector);

		CSharedByteVector(void const *_pData, umint _nBytes, NStorage::TCSharedPointer<CVirtualDestroyBase const> &&_pOwner);
		CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> const &_pVector, umint _Offset, umint _nBytes);
		CSharedByteVector(CSharedByteVector const &_Other, umint _Offset, umint _nBytes);

		CSharedByteVector(CSharedByteVector const &) = default;
		CSharedByteVector &operator = (CSharedByteVector const &_Other);

		CSharedByteVector(CSharedByteVector &&_Other);
		CSharedByteVector &operator = (CSharedByteVector &&_Other);

		uint8 const *f_GetArray() const;
		umint f_GetLen() const;
		bool f_IsEmpty() const;
		bool f_HasBuffer() const;
		void f_Clear();

		bool operator == (CSharedByteVector const &_Other) const;

		static CSharedByteVector fs_AllocateExact(umint _nBytes, uint8 *&o_pWritable);

		bool f_TryAppendContiguous(CSharedByteVector const &_Next);

	private:
		uint8 const *mp_pData = nullptr;
		umint mp_nBytes = 0;
		NStorage::TCSharedPointer<CVirtualDestroyBase const> mp_pOwner;
	};
}

#include "Malterlib_Container_SharedByteVector_Stream.hpp"
