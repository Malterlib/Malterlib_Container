// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Container/Vector>
#include <Mib/String/String>

namespace NMib::NContainer
{
	CSecureByteVector CByteVector::f_ToSecure() const // Accept the risk of this
	{
		CSecureByteVector Vector;
		static_cast<TCVector<uint8, NMemory::CAllocator_HeapSecure> &>(Vector) = *this;
		return Vector;
	}

	NStr::CStr CByteVector::f_ToString() const
	{
		DMibRequire(this->f_Contains(uint8(0)) < 0)("String will be cut off");
		return NStr::CStr(f_GetArray(), f_GetLen());
	}

	CByteVector CByteVector::fs_FromString(NStr::CStr const &_String)
	{
		CByteVector Return;
		Return.f_Insert((uint8 const *)_String.f_GetStr(), _String.f_GetLen());
		return Return;
	}

	CByteVector CSecureByteVector::f_ToInsecure() const // Accept the risk of this
	{
		CByteVector Vector;
		static_cast<TCVector<uint8> &>(Vector) = *this;
		return Vector;
	}

	NStr::CStrSecure CSecureByteVector::f_ToString() const
	{
		DMibRequire(this->f_Contains(uint8(0)) < 0)("String will be cut off");
		return NStr::CStrSecure(f_GetArray(), f_GetLen());
	}

	CSecureByteVector CSecureByteVector::fs_FromString(NStr::CStrSecure const &_String)
	{
		CSecureByteVector Return;
		Return.f_Insert((uint8 const *)_String.f_GetStr(), _String.f_GetLen());
		return Return;
	}
}
