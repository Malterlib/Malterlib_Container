// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_COption>
	bool TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_FormatParseOption(CFormatOptions &_Options, tf_COption &_Option) const
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

	template <typename t_CKey, typename t_CValue, typename t_CCompare, typename t_CAllocator>
	template <typename tf_CFormatInto, typename tf_CFormatOptions>
	void TCMap<t_CKey, t_CValue, t_CCompare, t_CAllocator>::f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const
	{
		if (_Options.m_LocalOptions.m_bSingleLine)
		{

			if (_Options.m_LocalOptions.m_bBrackets)
				o_FormatInto += "{";
			auto iValue = this->f_GetIterator();
			if (iValue)
			{
				auto pFormat = "{} = {}";
				if constexpr (TCIsContainer<t_CKey>::mc_Value)
				{
					if constexpr (TCIsContainer<t_CValue>::mc_Value)
						pFormat = "{vs} = {vs}";
					else
						pFormat = "{vs} = {}";
				}
				else if constexpr (TCIsContainer<t_CValue>::mc_Value)
					pFormat = "{} = {vs}";

				o_FormatInto += typename tf_CFormatInto::CFormat(pFormat) << iValue.f_GetKey() << *iValue;
				++iValue;
				for (; iValue; ++iValue)
				{
					o_FormatInto += ", ";
					o_FormatInto += typename tf_CFormatInto::CFormat(pFormat) << iValue.f_GetKey() << *iValue;
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
				o_FormatInto += typename tf_CFormatInto::CFormat("\t{} = {}") << iValue.f_GetKey() << *iValue;
				++iValue;
				for (; iValue; ++iValue)
				{
					o_FormatInto += ",\n";
					o_FormatInto += typename tf_CFormatInto::CFormat("\t{} = {}") << iValue.f_GetKey() << *iValue;
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
