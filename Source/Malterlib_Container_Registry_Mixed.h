// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Container/Registry>
#include <Mib/String/Mixed>

namespace NMib::NContainer
{
	using CRegistry_CMStrDeprecated = TCRegistry<NStr::CMStrDeprecated, NStr::CMStrDeprecated, ERegistryFlag_DuplicateKeys>;
	using CRegistryPreserveOrder_CMStrDeprecated = TCRegistry<NStr::CMStrDeprecated, NStr::CMStrDeprecated, ERegistryFlag_DuplicateKeys | ERegistryFlag_PreserveOrder>;
	using CRegistryPreserve_CMStrDeprecated = TCRegistry<NStr::CMStrDeprecated, NStr::CMStrDeprecated, ERegistryFlag_DuplicateKeys | ERegistryFlag_PreserveWhitspace>;
	using CRegistryPreserveAndOrder_CMStrDeprecated =
		TCRegistry<NStr::CMStrDeprecated, NStr::CMStrDeprecated, ERegistryFlag_DuplicateKeys | ERegistryFlag_PreserveWhitspace | ERegistryFlag_PreserveOrder>
	;
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NContainer;
#endif
