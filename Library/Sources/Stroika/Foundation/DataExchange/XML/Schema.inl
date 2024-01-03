/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Schema_inl_
#define _Stroika_Foundation_DataExchange_XML_Schema_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Providers/IProvider.h"

namespace Stroika::Foundation::DataExchange::XML::Schema {

    /*
     ********************************************************************************
     ************************************* Schema::Ptr ******************************
     ********************************************************************************
     */
    inline Ptr::Ptr (nullptr_t)
    {
    }
    inline Ptr::Ptr (shared_ptr<IRep> s)
        : fRep_{s}
    {
    }
    inline Sequence<SourceComponent> Ptr::GetSourceComponents () const
    {
        return fRep_->GetSourceComponents ();
    }
    inline optional<URI> Ptr::GetTargetNamespace () const
    {
        return fRep_->GetTargetNamespace ();
    }
    inline NamespaceDefinitionsList Ptr::GetNamespaceDefinitions () const
    {
        return fRep_->GetNamespaceDefinitions ();
    }
    inline shared_ptr<IRep> Ptr::GetRep () const
    {
        return fRep_;
    }

}

#endif