/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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

#include "../../../Characters/String.h"
#include "../../../Debug/CompileTimeFlagChecker.h"
#include "../DOM.h"

#include "IProvider.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 * 
 *      \note - Generally most code should NOT include this file - its implemeantion specific data for each provider and just included in implemtatniosn of DOM etc code
 */

namespace Stroika::Foundation::DataExchange::XML::Providers::Xerces {

    XERCES_CPP_NAMESPACE_USE;

    /**
     */
    struct IXercesSchemaRep {
        virtual xercesc_3_2::XMLGrammarPool* GetCachedGrammarPool () = 0;
    };

    /**
     */
    struct Map2StroikaExceptionsErrorReporter : public XMLErrorReporter, public ErrorHandler {
    public:
        virtual void error ([[maybe_unused]] const unsigned int errCode, [[maybe_unused]] const XMLCh* const errDomain,
                            [[maybe_unused]] const ErrTypes type, const XMLCh* const errorText, [[maybe_unused]] const XMLCh* const systemId,
                            [[maybe_unused]] const XMLCh* const publicId, const XMLFileLoc lineNum, const XMLFileLoc colNum) override;
        virtual void resetErrors () override;
        virtual void warning ([[maybe_unused]] const SAXParseException& exc) override;
        virtual void error (const SAXParseException& exc) override;
        virtual void fatalError (const SAXParseException& exc) override;
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

        virtual shared_ptr<Schema::IRep>        SchemaFactory (const optional<URI>& targetNamespace, const BLOB& targetNamespaceData,
                                                               const Sequence<Schema::SourceComponent>& sourceComponents,
                                                               const NamespaceDefinitionsList&          namespaceDefinitions) const override;
        virtual shared_ptr<DOM::Document::IRep> DocumentFactory (const String& documentElementName, const optional<URI>& ns) const override;
        virtual shared_ptr<DOM::Document::IRep> DocumentFactory (const Streams::InputStream::Ptr<byte>& in,
                                                                 const Schema::Ptr& schemaToValidateAgainstWhileReading) const override;
        virtual void SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer& callback,
                               const Schema::Ptr& schema) const override;

    private:
        struct MyXercesMemMgr_;
        MyXercesMemMgr_* fUseXercesMemoryManager{nullptr};
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
