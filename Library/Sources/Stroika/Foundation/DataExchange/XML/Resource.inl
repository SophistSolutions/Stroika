/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::XML::Resource {

    /*
     ********************************************************************************
     ********************************** ResolverPtr *********************************
     ********************************************************************************
     */
    inline ResolverPtr::ResolverPtr (nullptr_t)
        : fRep_{nullptr}
    {
    }
    inline ResolverPtr::ResolverPtr (const shared_ptr<IResolverRep>& rep)
        : fRep_{rep}
    {
    }
    inline optional<Definition> ResolverPtr::Lookup (const Name& n) const
    {
        RequireNotNull (fRep_);
        return fRep_->Lookup (n);
    }

}
