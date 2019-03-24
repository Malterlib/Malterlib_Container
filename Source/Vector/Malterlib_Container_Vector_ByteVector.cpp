// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Container/Vector>

namespace NMib::NContainer
{
	CSecureByteVector CByteVector::f_ToSecure() const // Accept the risk of this
	{
		CSecureByteVector Vector;
		static_cast<TCVector<uint8, NMemory::CAllocator_HeapSecure> &>(Vector) = *this;
		return Vector;
	}

	CByteVector CSecureByteVector::f_ToInsecure() const // Accept the risk of this
	{
		CByteVector Vector;
		static_cast<TCVector<uint8> &>(Vector) = *this;
		return Vector;
	}
}
