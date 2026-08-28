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
	// copying them.
	//
	// The bytes are anchored by a type-erased owner rather than a vector: whatever object keeps
	// them alive derives from CVirtualDestroyBase, so a kernel receive buffer, a wrapped vector
	// and a foreign allocation all travel the same way. The owner's destructor is the release
	// event — it runs exactly once, on whichever thread drops the last reference — which is what
	// lets outstanding-buffer accounting hang off it reliably. The bytes are const because
	// consumers (segmented binary storage, send queues, resend logic) may read them
	// concurrently: a producer fills its buffer, hands it over, and it is frozen
	struct CSharedByteVector
	{
		CSharedByteVector() = default;

		explicit CSharedByteVector(CIOByteVector &&_Vector);
		explicit CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> &&_pVector);
		explicit CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> const &_pVector);

		// A span of bytes and whatever keeps them alive
		CSharedByteVector(void const *_pData, umint _nBytes, NStorage::TCSharedPointer<CVirtualDestroyBase const> &&_pOwner);

		// View of [_Offset, _Offset + _nBytes) of the shared buffer. Bounds are checked in
		// subtraction form so an offset plus length that wraps cannot pass
		CSharedByteVector(NStorage::TCSharedPointer<CIOByteVector const> const &_pVector, umint _Offset, umint _nBytes);

		// View of [_Offset, _Offset + _nBytes) of the buffer another view shares
		CSharedByteVector(CSharedByteVector const &_Other, umint _Offset, umint _nBytes);

		CSharedByteVector(CSharedByteVector const &) = default;

		// Takes hold of the source's owner before the members are overwritten, so a view assigned
		// to itself keeps its buffer
		CSharedByteVector &operator = (CSharedByteVector const &_Other);

		// Moves reset the source's view metadata: the generated moves would null the owner but
		// keep the pointer and length, leaving a moved-from view that reports bytes it cannot
		// deliver
		CSharedByteVector(CSharedByteVector &&_Other);
		CSharedByteVector &operator = (CSharedByteVector &&_Other);

		uint8 const *f_GetArray() const;
		umint f_GetLen() const;
		bool f_IsEmpty() const;
		// Whether any buffer is anchored at all, empty view or not
		bool f_HasBuffer() const;
		void f_Clear();

		bool operator == (CSharedByteVector const &_Other) const;

		// Allocates exactly _nBytes with no vector header sharing the block, so a power of
		// two request wastes nothing in the memory manager. o_pWritable is the caller's
		// window to fill the bytes before the vector is handed on and frozen
		static CSharedByteVector fs_AllocateExact(umint _nBytes, uint8 *&o_pWritable);

		// Extends this view over _Next when both share the same owner and _Next's bytes start
		// exactly where this view ends, so receive paths can stitch adjacent slices of one
		// buffer back into a single view. Returns false without touching either view otherwise
		bool f_TryAppendContiguous(CSharedByteVector const &_Next);

	private:
		uint8 const *mp_pData = nullptr;
		umint mp_nBytes = 0;
		NStorage::TCSharedPointer<CVirtualDestroyBase const> mp_pOwner;
	};
}

#include "Malterlib_Container_SharedByteVector_Stream.hpp"
