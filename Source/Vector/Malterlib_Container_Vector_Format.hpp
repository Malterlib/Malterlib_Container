// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_COption>
	bool TCVector<t_CData, t_CAllocator, t_COptions>::f_FormatParseOption(CFormatOptions &_Options, tf_COption &_Option) const
	{
		switch (_Option.m_FormatTypes.m_Format1)
		{
		case 'V':
			{
				switch (_Option.m_FormatTypes.m_Format2)
				{
				case 'S':
					_Options.m_bSingleLine = true;
					return true;
				case 'B':
					_Options.m_bBrackets = false;
					return true;
				}
			}
			break;
		}
		return false;
	}

	template <typename t_CData, typename t_CAllocator, typename t_COptions>
	template <typename tf_CFormatInto, typename tf_CFormatOptions>
	void TCVector<t_CData, t_CAllocator, t_COptions>::f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const
	{
		if (_Options.m_LocalOptions.m_bSingleLine)
		{
			if (_Options.m_LocalOptions.m_bBrackets)
				o_FormatInto += "[";
			auto iValue = f_GetIterator();
			if (iValue)
			{
				auto pFormat = "{}";
				if constexpr (TCIsContainer<t_CData>::mc_Value)
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
				o_FormatInto += "]";
		}
		else
		{
			if (_Options.m_LocalOptions.m_bBrackets)
				o_FormatInto += "[\n";
			auto iValue = f_GetIterator();
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
					o_FormatInto += "\n]\n";
				else
					o_FormatInto += "\n";
			}
			else if (_Options.m_LocalOptions.m_bBrackets)
				o_FormatInto += "]\n";
		}
	}
}
