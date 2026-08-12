// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Core/Core>
#include <Mib/Stream/Binary>

namespace NMib::NContainer
{
	// Shared immutable byte buffer with value semantics: copies only adjust the reference
	// count, so buffers can be forwarded through streams, async generators and send queues
	// without copying the bytes. An instance can also be a view of a range of the shared
	// buffer, which lets receive paths hand out payload slices of a packet buffer without
	// copying them. The wrapped vector is held by const pointer because consumers (segmented
	// binary storage, send queues, resend logic) may read it concurrently: a producer fills
	// its buffer through a mutable pointer and hands it over, after which it is frozen
	struct CSharedByteVector
	{
		CSharedByteVector() = default;

		explicit CSharedByteVector(CIOByteVector &&_Vector);
		explicit CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> &&_pVector);
		explicit CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> const &_pVector);

		// View of [_Offset, _Offset + _nBytes) of the shared buffer. Bounds are checked in
		// subtraction form so an offset plus length that wraps cannot pass
		CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> const &_pVector, umint _Offset, umint _nBytes);

		// View of [_Offset, _Offset + _nBytes) of the buffer another view shares
		CSharedByteVector(CSharedByteVector const &_Other, umint _Offset, umint _nBytes);

		CSharedByteVector(CSharedByteVector const &) = default;

		// The shared pointer releases its reference before reading the source, so assigning a
		// view to itself would drop the buffer and leave a null array behind a nonzero length
		CSharedByteVector &operator = (CSharedByteVector const &_Other);

		// Moves reset the source's view metadata: the generated moves would null the shared
		// pointer but keep the length, leaving a moved-from view that reports bytes it cannot
		// deliver
		CSharedByteVector(CSharedByteVector &&_Other);
		CSharedByteVector &operator = (CSharedByteVector &&_Other);

		uint8 const *f_GetArray() const;
		umint f_GetLen() const;
		bool f_IsEmpty() const;
		NStorage::TCSharedPointer<CIOByteVector const> const &f_GetSharedPointer() const;
		void f_Clear();

		bool operator == (CSharedByteVector const &_Other) const;

	private:
		NStorage::TCSharedPointer<CIOByteVector const> mp_pVector;
		umint mp_Offset = 0;
		umint mp_nBytes = 0;
	};
}

#include "Malterlib_Container_SharedByteVector_Stream.hpp"
