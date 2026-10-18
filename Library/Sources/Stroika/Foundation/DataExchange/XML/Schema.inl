/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Streams/BinaryToText.h"

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
            return Streams::BinaryToText::Reader::New (fRep_->GetData ()).ReadAll ();
        }
        else if constexpr (same_as<AS_T, Memory::BLOB>) {
            return fRep_->GetData ();
        }
    }
    // defined in Schema.cpp, where ISchemaProvider is complete: its call to New, not dependent on AS_T, is resolved where it
    // is defined - and here, before IProvider.h defines ISchemaProvider, considering New (stream, ...) would test whether
    // an incomplete ISchemaProvider converts to an InputStream::Ptr (g++'s -Wsfinae-incomplete)
    template <>
    XML::Schema::Ptr Ptr::As (const Providers::ISchemaProvider& p);
    template <>
    XML::DOM::Document::Ptr Ptr::As (const Providers::IDOMProvider& p);

}
