// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	bool TCRegistry<t_CStr, t_CData, t_Flags>::fpr_DebugIsValid() const
	{
		auto Iter = mp_Children.f_GetIterator();
		while (Iter)
		{
			const TCRegistry *pReg = Iter;
			++Iter;
			if (pReg->mp_pParent != this)
				return false;
			if (!pReg->fpr_DebugIsValid())
				return false;
		}
		return true;
	}

#ifdef DMibDebug
	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	inline_never ch16 const *TCRegistry<t_CStr, t_CData, t_Flags>::fp_Debug_GetUTF16() const
	{
		NMib::NStr::g_DebugOutputTemp16 = f_GenerateStr();
		return NMib::NStr::g_DebugOutputTemp16;
	}

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	void TCRegistry<t_CStr, t_CData, t_Flags>::f_DebugTraceTree() const
	{
		NStr::CStr Temp = f_GenerateStr<NStr::CStr, mc_bSupportFileLine, 1>();
		DMibDTrace("{}" DMibNewLine, Temp);
	}
#endif

	template <typename t_CStr, typename t_CData, ERegistryFlag t_Flags>
	bool TCRegistry<t_CStr, t_CData, t_Flags>::f_DebugIsValid(bool _bIsRoot) const
	{
		if (_bIsRoot && mp_pParent != nullptr)
			return false;
		return fpr_DebugIsValid();
	}
}
