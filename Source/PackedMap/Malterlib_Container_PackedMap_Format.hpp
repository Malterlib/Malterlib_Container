// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	// Format support for output
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator, CPackedMapOptions t_Options>
	template <typename tf_COutput>
	constexpr void TCPackedMap<t_CKey, t_CValue, t_CCompare, t_CAllocator, t_Options>::f_Format(tf_COutput &_Output, CFormatOptions const &_Options) const
	{
		if (_Options.m_bBrackets)
			_Output += "{";

		bool bFirst = true;
		for (auto Iter = f_Entries().f_GetIterator(); Iter; ++Iter)
		{
			if (!bFirst)
			{
				if (_Options.m_bSingleLine)
					_Output += ", ";
				else
					_Output += ",\n";
			}
			bFirst = false;

			auto Ref = *Iter;
			_Output += Ref.f_Key();
			_Output += ": ";
			_Output += Ref.f_Value();
		}

		if (_Options.m_bBrackets)
			_Output += "}";
	}
}
