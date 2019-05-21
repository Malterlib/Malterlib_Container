// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CData>
	TCRegistry_ParseValue<t_CStr, t_CData>::TCRegistry_ParseValue(t_CStr const &_Value, bool _bEscaped)
		: m_Value(_Value)
		, m_bEscaped(_bEscaped)
	{
	}

	template <typename t_CStr, typename t_CData>
	TCRegistry_ParseValue<t_CStr, t_CData>::operator t_CData ()
	{
		return m_Value;
	}

	template <typename t_CStr>
	TCRegistry_ParseValue<t_CStr, t_CStr>::TCRegistry_ParseValue(t_CStr const &_Value, bool _bEscaped)
		: m_Value(_Value)
		, m_bEscaped(_bEscaped)
	{
	}

	template <typename t_CStr>
	TCRegistry_ParseValue<t_CStr, t_CStr>::operator t_CStr const &()
	{
		return m_Value;
	}

	template <typename t_CValue>
	template <typename tf_CString>
	void TCRegistry_FormatValue<t_CValue>::fs_Generate(tf_CString &o_Output, t_CValue const &_Value, bool &o_bForceEscape)
	{
		o_Output = tf_CString::fs_ToStr(_Value);
	}
}
