// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CData>
	struct TCRegistry_CustomValue
	{
		static constexpr bool mc_bDefault = true;
		static constexpr bool mc_bDefaultKey = true;
		static constexpr bool mc_bRequireStartScopeOnSeparateLine = false;

//		template <typename tf_CString>
//		static void fs_Generate(tf_CString &o_Output, t_CData const &_Value, bool &o_bForceEscape, tf_CString const &_PreData);
//		template <typename tf_CChar, typename tf_CParseContext>
//		static t_CData fs_Parse(tf_CChar const * &o_pParse, tf_CParseContext &o_ParseContext, bool &o_bWasEscaped);
//		static bool fs_ValueIsEmpty(t_CData const &_Value, bool _bForceEscape);
	};
}
