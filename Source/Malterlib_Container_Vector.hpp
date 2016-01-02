// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib
{
	namespace NContainer
	{
		template <typename t_CData, typename t_CAllocator, typename t_CBoundsChecker, typename t_CInternalData, typename t_CStaticData>
		template <typename tf_COption>
		bool TCVector<t_CData, t_CAllocator, t_CBoundsChecker, t_CInternalData, t_CStaticData>::f_FormatParseOption(CFormatOptions &_Options, tf_COption &_Option) const
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
					}
				}
				break;
			}
			return false;
		}

		template <typename t_CData, typename t_CAllocator, typename t_CBoundsChecker, typename t_CInternalData, typename t_CStaticData>
		template <typename tf_CFormatInto, typename tf_CFormatOptions>
		void TCVector<t_CData, t_CAllocator, t_CBoundsChecker, t_CInternalData, t_CStaticData>::f_Format(tf_CFormatInto &o_FormatInto, tf_CFormatOptions const &_Options) const
		{
			if (_Options.m_LocalOptions.m_bSingleLine)
			{
				o_FormatInto += "[";
				auto iValue = f_GetIterator();
				if (iValue)
					o_FormatInto += NStr::CStr::CFormat("{}") << *iValue;
				for (; iValue; ++iValue)
				{
					o_FormatInto += ", ";
					o_FormatInto += NStr::CStr::CFormat("{}") << *iValue;
				}
				o_FormatInto += "]";
			}
			else
			{
				o_FormatInto += "[\n";
				auto iValue = f_GetIterator();
				if (iValue)
				{
					o_FormatInto += NStr::CStr::CFormat("\t{}") << *iValue;
					++iValue;
					for (; iValue; ++iValue)
					{
						o_FormatInto += ",\n";
						o_FormatInto += NStr::CStr::CFormat("\t{}") << *iValue;
					}
					o_FormatInto += "\n]\n";
				}
				else
					o_FormatInto += "]\n";
			}
		}
	}

};
