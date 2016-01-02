// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Container/Registry>
#include <Mib/String/Mixed>

namespace NMib
{
	namespace NRegistry
	{
		typedef TCRegistry<NStr::CMStrDeprecated, NStr::CMStrDeprecated, TCRegistryKeyStrMulti<NStr::CMStrDeprecated>> CRegistry_CMStrDeprecated;
		typedef TCRegistry<NStr::CMStrDeprecated, NStr::CMStrDeprecated, TCRegistryKeyStrMulti<NStr::CMStrDeprecated>, true> CRegistryPreserveOrder_CMStrDeprecated;
		typedef TCRegistry<NStr::CMStrDeprecated, NStr::CMStrDeprecated, TCRegistryKeyStrPreserve<NStr::CMStrDeprecated>> CRegistryPreserve_CMStrDeprecated;
		typedef TCRegistry<NStr::CMStrDeprecated, NStr::CMStrDeprecated, TCRegistryKeyStrPreserve<NStr::CMStrDeprecated>, true> CRegistryPreserveAndOrder_CMStrDeprecated;
	}
}

