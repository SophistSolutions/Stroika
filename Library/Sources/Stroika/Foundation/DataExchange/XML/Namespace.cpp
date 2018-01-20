/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
    fNamespaces.push_back (NamespaceDefinition (uri, prefix));
}

bool XML::operator< (const NamespaceDefinitionsList& lhs, const NamespaceDefinitionsList& rhs)
{
    vector<NamespaceDefinition>::const_iterator li = lhs.fNamespaces.begin ();
    vector<NamespaceDefinition>::const_iterator le = lhs.fNamespaces.end ();
    vector<NamespaceDefinition>::const_iterator ri = rhs.fNamespaces.begin ();
    vector<NamespaceDefinition>::const_iterator re = rhs.fNamespaces.end ();
    for (; li < le or ri < re;) {
        if (li < le and ri < re) {
            if (*li < *ri) {
                return true;
            }
            else if (not(*li == *ri)) {
                // cuz implies >
                return false;
            }
            // keep looking
        }
        else if (li < le) {
            // rhs ran out, so LHS is longer
            return false;
        }
        else if (ri < re) {
            // lhs ran out, so RHS is longer
            return true;
        }
        // keep looking
        Assert (li < le);
        Assert (ri < re);
        li++;
        ri++;
    }
    // both ran out at the same time
    return false;
}

bool XML::operator== (const NamespaceDefinitionsList& lhs, const NamespaceDefinitionsList& rhs)
{
    vector<NamespaceDefinition>::const_iterator li = lhs.fNamespaces.begin ();
    vector<NamespaceDefinition>::const_iterator le = lhs.fNamespaces.end ();
    vector<NamespaceDefinition>::const_iterator ri = rhs.fNamespaces.begin ();
    vector<NamespaceDefinition>::const_iterator re = rhs.fNamespaces.end ();
    for (; li < le or ri < re;) {
        if (li < le and ri < re) {
            if (not(*li == *ri)) {
                return false;
            }
            // keep looking
        }
        else if (li < le) {
            // rhs ran out, so LHS is longer
            return false;
        }
        else if (ri < re) {
            // lhs ran out, so RHS is longer
            return false;
        }
        // keep looking
        Assert (li < le);
        Assert (ri < re);
        li++;
        ri++;
    }
    // both ran out at the same time
    return true;
}
