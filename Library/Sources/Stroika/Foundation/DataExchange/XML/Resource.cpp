/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "Resource.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::XML::Resource;

/*
 ********************************************************************************
 ********************************* Resource::Name *******************************
 ********************************************************************************
 */
String Name::ToString () const
{
    StringBuilder sb;
    sb << "{";
    if (fNamespace) {
        sb << "namespace: " << Characters::ToString (*fNamespace) << ", ";
    }
    if (fPublicID) {
        sb << "fPublicID: " << Characters::ToString (*fPublicID) << ", ";
    }
    if (fSystemID) {
        sb << "systemID: " << Characters::ToString (*fSystemID) << ", ";
    }
    if (sb.size () > 3) {             // so a comma there
        sb.ShrinkTo (sb.size () - 2); // lose comma
    }
    sb << "}";
    return sb.str ();
}

/*
 ********************************************************************************
 ************************** Resource::MemoryResolver ****************************
 ********************************************************************************
 */
ResolverPtr MemoryResolver::New (const Iterable<Definition>& defs)
{
    struct R : IResolverRep {
        R (const Iterable<Definition>& defs)
            : fDefs_{defs}
        {
        }
        virtual optional<Definition> Lookup (const Name& n) const override
        {
            // Treat namespaces and publicids as higher-priority matchers (because old code did in HealthFrame - but not sure of reason)
            for (auto i : fDefs_) {
                if (n.fNamespace != nullopt and n.fNamespace == i.fName.fNamespace) {
                    return i;
                }
                if (n.fPublicID != nullopt and n.fPublicID == i.fName.fPublicID) {
                    return i;
                }
            }
            for (auto i : fDefs_) {
                if (n.fSystemID != nullopt and n.fSystemID == i.fName.fSystemID) {
                    return i;
                }
            }
            return nullopt;
        }
        Iterable<Definition> fDefs_;
    };
    return ResolverPtr{make_shared<R> (defs)};
}
