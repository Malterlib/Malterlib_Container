// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NContainer
{
	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::TCRegistry()
		: mp_Key((TCRegistry *)nullptr) // No parent
		, mp_pParent (nullptr)
	{

	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::~TCRegistry()
	{
		if (mp_pParent && mp_ChildLink.f_IsInTree())
			mp_pParent->mp_Children.f_Remove(this);

		mp_Children.m_Tree.f_DeleteAllDefiniteType();

#ifdef DMibDebug
		static_assert(TCInstantiateValue<&TCRegistry::fp_Debug_GetUTF16>::mc_Value);
		static_assert(TCInstantiateValue<&TCRegistry::f_DebugTraceTree>::mc_Value);
#endif
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::TCRegistry(TCRegistry *_pParent)
		: mp_Key(_pParent)
	{
		mp_pParent = _pParent;
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_Clear()
	{
		mp_Children.m_Tree.f_DeleteAllDefiniteType();
		mp_Data = t_CData();
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	t_CKey const &TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_GetName() const
	{
		return mp_Key.f_GetName();
	}

	template <typename t_CKey, typename t_CData, ERegistryFlag t_Flags, typename t_CStr>
	void TCRegistry<t_CKey, t_CData, t_Flags, t_CStr>::f_SetName(t_CKey const &_Name)
	{
		if (mp_pParent && mp_ChildLink.f_IsInTree())
			mp_pParent->mp_Children.f_Remove(this);
		mp_Key.f_Set(_Name);
		if (mp_pParent)
			mp_pParent->mp_Children.f_Insert(this);
	}
}

#include "Malterlib_Container_Registry_Children.hpp"
#include "Malterlib_Container_Registry_ChildHelpers.hpp"
#include "Malterlib_Container_Registry_Compare.hpp"
#include "Malterlib_Container_Registry_Copy.hpp"
#include "Malterlib_Container_Registry_Debug.hpp"
#include "Malterlib_Container_Registry_Diff.hpp"
#include "Malterlib_Container_Registry_Generate.hpp"
#include "Malterlib_Container_Registry_Iterate.hpp"
#include "Malterlib_Container_Registry_Move.hpp"
#include "Malterlib_Container_Registry_Parse.hpp"
#include "Malterlib_Container_Registry_ParseContext.hpp"
#include "Malterlib_Container_Registry_Path.hpp"
#include "Malterlib_Container_Registry_PreserveWhitespace.hpp"
#include "Malterlib_Container_Registry_SetOperations.hpp"
#include "Malterlib_Container_Registry_Stream.hpp"
#include "Malterlib_Container_Registry_Value.hpp"
