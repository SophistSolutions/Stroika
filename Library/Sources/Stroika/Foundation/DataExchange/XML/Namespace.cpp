/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <sstream>

#include "../BadFormatException.h"

#include "Namespace.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;

/*
 ********************************************************************************
 ************************** NamespaceDefinitionsList ****************************
 ********************************************************************************
 */
void NamespaceDefinitionsList::Add (const wstring& uri, const wstring& prefix)
{
#if defined(_DEBUG)
    for (auto i = fNamespaces.begin (); i != fNamespaces.end (); ++i) {
        Assert (uri != i->fURI);
        Assert (prefix.empty () or prefix != i->fPrefix);
    }
#endif
    fNamespaces.push_back (NamespaceDefinition{uri, prefix});
}
