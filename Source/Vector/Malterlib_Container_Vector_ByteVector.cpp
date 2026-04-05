// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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

	CByteVector const &CByteVector::f_ToInsecure() const &
	{
		return *this;
	}

	CByteVector &&CByteVector::f_ToInsecure() &&
	{
		return fg_Move(*this);
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

	CByteVector &&CByteVector::fs_AllowInsecureConversion(CByteVector &&_Other)
	{
		return fg_Move(_Other);
	}

	CByteVector CByteVector::fs_AllowInsecureConversion(CByteVector const &_Other)
	{
		return _Other;
	}

	CByteVector CByteVector::fs_AllowInsecureConversion(CSecureByteVector const &_Other)
	{
		return _Other.f_ToInsecure();
	}

	CByteVector CSecureByteVector::f_ToInsecure() const // Accept the risk of this
	{
		CByteVector Vector;
		static_cast<TCVector<uint8> &>(Vector) = *this;
		return Vector;
	}

	CSecureByteVector const &CSecureByteVector::f_ToSecure() const &
	{
		return *this;
	}

	CSecureByteVector &&CSecureByteVector::f_ToSecure() &&
	{
		return fg_Move(*this);
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

	CSecureByteVector &&CSecureByteVector::fs_AllowInsecureConversion(CSecureByteVector &&_Other)
	{
		return fg_Move(_Other);
	}

	CSecureByteVector CSecureByteVector::fs_AllowInsecureConversion(CSecureByteVector const &_Other)
	{
		return _Other;
	}

	CSecureByteVector CSecureByteVector::fs_AllowInsecureConversion(CByteVector const &_Other)
	{
		return _Other.f_ToSecure();
	}
}
