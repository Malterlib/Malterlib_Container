// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "../Malterlib_Container_Shared.h"

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CFormatInto, typename tf_CFormatOptions>
	void TCSet<t_CKey, t_CCompare, t_CAllocator>::f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const
	{
		if (_Options.m_LocalOptions.m_bSingleLine)
		{
			if (_Options.m_LocalOptions.m_bBrackets)
				o_FormatInto += "{";
			auto iValue = this->f_GetIterator();
			if (iValue)
			{
				auto pFormat = "{}";
				if constexpr (TCIsContainer<t_CKey>::mc_Value)
					pFormat = "{vs}";
				o_FormatInto += typename tf_CFormatInto::CFormat(pFormat) << *iValue;
				++iValue;
				for (; iValue; ++iValue)
				{
					o_FormatInto += ", ";
					o_FormatInto += typename tf_CFormatInto::CFormat(pFormat) << *iValue;
				}
			}
			if (_Options.m_LocalOptions.m_bBrackets)
				o_FormatInto += "}";
		}
		else
		{
			if (_Options.m_LocalOptions.m_bBrackets)
				o_FormatInto += "{\n";
			auto iValue = this->f_GetIterator();
			if (iValue)
			{
				o_FormatInto += typename tf_CFormatInto::CFormat("\t{}") << *iValue;
				++iValue;
				for (; iValue; ++iValue)
				{
					o_FormatInto += ",\n";
					o_FormatInto += typename tf_CFormatInto::CFormat("\t{}") << *iValue;
				}
				if (_Options.m_LocalOptions.m_bBrackets)
					o_FormatInto += "\n}\n";
				else
					o_FormatInto += "\n";
			}
			else if (_Options.m_LocalOptions.m_bBrackets)
				o_FormatInto += "}\n";
		}
	}
}
