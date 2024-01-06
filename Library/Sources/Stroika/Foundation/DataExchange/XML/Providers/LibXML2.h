/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Providers_LibXML2_h_
#define _Stroika_Foundation_DataExchange_XML_Providers_LibXML2_h_ 1

#include "../../../StroikaPreComp.h"

static_assert (qHasFeature_libxml2, "Don't include this file if qHasFeature_libxml2 not set");

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include "Stroika/Foundation//DataExchange/XML/Common.h"
#include "Stroika/Foundation/Debug/CompileTimeFlagChecker.h"

#include "IProvider.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 * 
 *  \note - Generally most code should NOT include this file - its implementation specific data for each provider
 *          Mostly use to control which implementation you are using for SAX, Schema etc, and/or to access implementation-specific objects
 *          from schema, DOM nodes, etc.
 */

namespace Stroika::Foundation::DataExchange::XML::Providers::LibXML2 {

    using Characters::String;

    /**
     *  Note - it is documented in the headers that xmlChar is always UTF8
     */
    String libXMLString2String (const xmlChar* s, int len);
    String libXMLString2String (const xmlChar* t);

    /**
     */
    struct ILibXML2SchemaRep : Schema::IRep {
        virtual xmlSchema* GetSchemaLibRep () = 0;
    };

    struct ILibXML2DocRep : DOM::Document::IRep {
        virtual xmlDoc* GetLibXMLDocRep () = 0;
    };

    /**
     */
    struct ILibXML2NodeRep : DOM::Node::IRep {
        virtual xmlNode* GetInternalTRep () = 0;
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
        virtual void SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback,
                               const Schema::Ptr& schema) const override;
    };
    inline const Provider kDefaultProvider;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
CompileTimeFlagChecker_HEADER (Stroika::Foundation::DataExchange::XML, qHasFeature_libxml2, qHasFeature_libxml2);

#endif /*_Stroika_Foundation_DataExchange_XML_Providers_LibXML2_h_*/
