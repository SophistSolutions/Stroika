/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Providers_Xerces_h_
#define _Stroika_Foundation_DataExchange_XML_Providers_Xerces_h_ 1

#include "../../../StroikaPreComp.h"

/**
 *  \def qHasFeature_Xerces
 *      Stroika currently depends on Xerces to provide most XML services/functions./p>
 */
#ifndef qHasFeature_Xerces
#error "qHasFeature_Xerces should normally be defined indirectly by StroikaConfig.h"
#endif

static_assert (qHasFeature_Xerces, "Don't include this file if qHasFeature_Xerces not set");

#include "../../../Debug/CompileTimeFlagChecker.h"

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

//#include "../../../Characters/String.h"
#include "IProvider.h"

// @todo understand why cannot #include on windoze!!!
//#include "../DOM.h"
//#include "../Schema.h"
namespace Stroika::Foundation::Characters {
    class String;
}

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 * 
 *      \note - Generally most code should NOT include this file - its implemeantion specific data for each provider and just included in implemtatniosn of DOM etc code
 */

namespace Stroika::Foundation::DataExchange::XML::Providers::Xerces {

    XERCES_CPP_NAMESPACE_USE;

/*
 */
#define START_LIB_EXCEPTION_MAPPER try {
#define END_LIB_EXCEPTION_MAPPER                                                                                                           \
    }                                                                                                                                      \
    catch (const xercesc_3_2::OutOfMemoryException&)                                                                                       \
    {                                                                                                                                      \
        Execution::Throw (bad_alloc{}, "xerces OutOfMemoryException - throwing bad_alloc");                                                \
    }                                                                                                                                      \
    catch (...)                                                                                                                            \
    {                                                                                                                                      \
        Execution::ReThrow ();                                                                                                             \
    }

    /**
     */
    struct IXercesSchemaRep {
        virtual xercesc_3_2::XMLGrammarPool* GetCachedGrammarPool () = 0;
    };

    /**
     */
    void SetupCommonParserFeatures (SAX2XMLReader& reader);
    void SetupCommonParserFeatures (SAX2XMLReader& reader, bool validatingWithSchema);

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
    * &&&todo REDOC/OPTIONS....
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

    private:
        struct MyXercesMemMgr_;
        MyXercesMemMgr_* fUseXercesMemoryManager{nullptr};
    };
    inline const Provider kDefault;

}

///IMPL DETAILS SECTION
CompileTimeFlagChecker_HEADER (Stroika::Foundation::DataExchange::XML, qHasFeature_Xerces, qHasFeature_Xerces);

#endif /*_Stroika_Foundation_DataExchange_XML_Providers_Xerces_h_*/
