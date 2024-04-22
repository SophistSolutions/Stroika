/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Schema_inl_
#define _Stroika_Foundation_DataExchange_XML_Schema_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Stroika/Foundation/Streams/TextReader.h"

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
    inline optional<URI> Ptr::GetTargetNamespace () const
    {
        return fRep_->GetTargetNamespace ();
    }
    inline shared_ptr<IRep> Ptr::GetRep () const
    {
        return fRep_;
    }
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    template <>
    XML::DOM::Document::Ptr Ptr::As ();
#endif
    template <typename AS_T>
    AS_T Ptr::As ()
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        requires (same_as<AS_T, String>
#if qStroika_Foundation_DataExchange_XML_SupportDOM
                  or same_as<AS_T, XML::DOM::Document::Ptr>
#endif
                  or same_as<AS_T, Memory::BLOB>)
#endif
    {
        if constexpr (same_as<AS_T, String>) {
            return Streams::TextReader::New (fRep_->GetData ()).ReadAll ();
        }
        else if constexpr (same_as<AS_T, Memory::BLOB>) {
            return fRep_->GetData ();
        }
    }
    template <typename AS_T>
    AS_T Ptr::As (const Providers::ISchemaProvider& p)
        requires (same_as<AS_T, XML::Schema::Ptr>)
    {
        if constexpr (same_as<AS_T, XML::Schema::Ptr>) {
            return New (p, fRep_->GetData (), fRep_->GetResolver ());
        }
    }
    template <>
    XML::DOM::Document::Ptr Ptr::As (const Providers::IDOMProvider& p);

}

#endif