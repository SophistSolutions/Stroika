/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Providers_LibXML2_h_
#define _Stroika_Foundation_DataExchange_XML_Providers_LibXML2_h_ 1

#include "../../../StroikaPreComp.h"

static_assert (qHasFeature_libxml2, "Don't include this file if qHasFeature_libxml2 not set");

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include "../../../Debug/CompileTimeFlagChecker.h"
#include "../Common.h"

#include "IProvider.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 * 
 *      \note - Generally most code should NOT include this file - its implemeantion specific data for each provider and just included in implemtatniosn of DOM etc code
 */

namespace Stroika::Foundation::DataExchange::XML::Providers::LibXML2 {

    using Characters::String;

    String libXMLString2String (const xmlChar* s, int len);
    String libXMLString2String (const xmlChar* t);

    /**
     */
    struct ILibXML2SchemaRep {
        virtual xmlSchema* GetSchemaLibRep () = 0;
    };

    /**
     *  Can only be created ONCE (because libxml2 library can only be constructed once). Use the default impl.
     */
    struct Provider : Providers::IXMLProvider {
        Provider ();
        Provider (const Provider&) = delete;
        ~Provider ();

        virtual shared_ptr<Schema::IRep>        SchemaFactory (const optional<URI>& targetNamespace, const BLOB& targetNamespaceData,
                                                               const Sequence<Schema::SourceComponent>& sourceComponents,
                                                               const NamespaceDefinitionsList&          namespaceDefinitions) const override;
        virtual shared_ptr<DOM::Document::IRep> DocumentFactory (const String& documentElementName, const optional<URI>& ns) const override;
        virtual shared_ptr<DOM::Document::IRep> DocumentFactory (const Streams::InputStream::Ptr<byte>& in,
                                                                 const Schema::Ptr& schemaToValidateAgainstWhileReading) const override;
        virtual void SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer& callback,
                               const Schema::Ptr& schema) const override;
    };
    inline const Provider kDefaultProvider;

}

///IMPL DETAILS SECTION
CompileTimeFlagChecker_HEADER (Stroika::Foundation::DataExchange::XML, qHasFeature_libxml2, qHasFeature_libxml2);

#endif /*_Stroika_Foundation_DataExchange_XML_Providers_LibXML2_h_*/
