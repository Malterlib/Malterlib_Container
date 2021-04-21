// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fpr_DebugIsValid() const
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
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	inline_never ch16 const *TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::fp_Debug_GetUTF16() const
	{
		NMib::NStr::g_DebugOutputTemp16 = f_GenerateStr();
		return NMib::NStr::g_DebugOutputTemp16;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_DebugTraceTree() const
	{
		NStr::CStr Temp = f_GenerateStr<NStr::CStr, mc_bSupportLocation, 1>();
		DMibDTrace("{}" DMibNewLine, Temp);
	}
#endif

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	bool TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_DebugIsValid(bool _bIsRoot) const
	{
		if (_bIsRoot && mp_pParent != nullptr)
			return false;
		return fpr_DebugIsValid();
	}
}
