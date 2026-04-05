// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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
