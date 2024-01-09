/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Providers_Xerces_h_
#define _Stroika_Foundation_DataExchange_XML_Providers_Xerces_h_ 1

#include "../../../StroikaPreComp.h"

static_assert (qHasFeature_Xerces, "Don't include this file if qHasFeature_Xerces not set");

// avoid namespace conflict with some Xerces code
#undef Assert
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/parsers/SAX2XMLReaderImpl.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#define Assert(c) AssertExpression (c);

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h"
#include "Stroika/Foundation/Debug/CompileTimeFlagChecker.h"

#include "IProvider.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 * 
 *  \note - Generally most code should NOT include this file - its implementation specific data for each provider
 *          Mostly use to control which implementation you are using for SAX, Schema etc, and/or to access implementation-specific objects
 *          from schema, DOM nodes, etc.
 */

namespace Stroika::Foundation::DataExchange::XML::Providers::Xerces {

    XERCES_CPP_NAMESPACE_USE;

    /**
     */
    struct IXercesSchemaRep : Schema::IRep {
        virtual xercesc_3_2::XMLGrammarPool* GetCachedGrammarPool () = 0;
    };

    /**
     */
    struct IXercesNodeRep : DOM::Node::IRep {
        virtual xercesc_3_2::DOMNode* GetInternalTRep () = 0;
    };

    /**
     */
    Characters::String xercesString2String (const XMLCh* s, const XMLCh* e);
    Characters::String xercesString2String (const XMLCh* t);

    /**
     *  Can only be created ONCE (because Xerces library can only be initialized once). Use the default impl.
     */
    struct Provider : Providers::IXMLProvider {
        Provider ();
        Provider (const Provider&) = delete;
        ~Provider ();

        virtual shared_ptr<Schema::IRep> SchemaFactory (const BLOB& targetNamespaceData, const Sequence<Schema::SourceComponent>& sourceComponents,
                                                        const NamespaceDefinitionsList& namespaceDefinitions) const override;
        virtual shared_ptr<DOM::Document::IRep> DocumentFactory (const NameWithNamespace& documentElementName) const override;
        virtual shared_ptr<DOM::Document::IRep> DocumentFactory (const Streams::InputStream::Ptr<byte>& in,
                                                                 const Schema::Ptr& schemaToValidateAgainstWhileReading) const override;
        virtual void SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback,
                               const Schema::Ptr& schema) const override;

    private:
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
        struct MyXercesMemMgr_;
        MyXercesMemMgr_* fUseXercesMemoryManager_{nullptr};
#endif
    };
    inline const Provider kDefaultProvider;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
CompileTimeFlagChecker_HEADER (Stroika::Foundation::DataExchange::XML, qHasFeature_Xerces, qHasFeature_Xerces);

#endif /*_Stroika_Foundation_DataExchange_XML_Providers_Xerces_h_*/
