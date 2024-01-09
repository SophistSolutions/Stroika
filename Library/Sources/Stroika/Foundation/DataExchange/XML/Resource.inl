/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Resource_inl_
#define _Stroika_Foundation_DataExchange_XML_Resource_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::DataExchange::XML::Resource {

    /*
     ********************************************************************************
     ******************************** NamespaceDefinition ***************************
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

#endif /*_Stroika_Foundation_DataExchange_XML_Resource_inl_*/
