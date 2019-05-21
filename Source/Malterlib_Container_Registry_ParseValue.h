// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CData>
	struct TCRegistry_ParseValue
	{
		TCRegistry_ParseValue(t_CStr const &_Value, bool _bEscaped);
		operator t_CData ();

		t_CStr const &m_Value;
		bool m_bEscaped;
	};

	template <typename t_CStr>
	struct TCRegistry_ParseValue<t_CStr, t_CStr>
	{
		TCRegistry_ParseValue(t_CStr const &_Value, bool _bEscaped);
		operator t_CStr const &();

		t_CStr const &m_Value;
		bool m_bEscaped;
	};

	template <typename t_CValue>
	struct TCRegistry_FormatValue
	{
		template <typename tf_CString>
		static void fs_Generate(tf_CString &o_Output, t_CValue const &_Value, bool &o_bForceEscape);
	};
}

#include "Malterlib_Container_Registry_ParseValue.hpp"
