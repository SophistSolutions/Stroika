/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "Namespace.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;

/*
 ********************************************************************************
 ************************** NamespaceDefinitionsList ****************************
 ********************************************************************************
 */
void NamespaceDefinitionsList::Add (const URI& uri, const optional<String>& prefix)
{
#if defined(_DEBUG)
    for (auto i = fNamespaces.begin (); i != fNamespaces.end (); ++i) {
        Assert (uri != i->fURI);
        Assert (prefix == nullopt or prefix != i->fPrefix);
    }
#endif
    fNamespaces.push_back (NamespaceDefinition{uri, prefix});
}
