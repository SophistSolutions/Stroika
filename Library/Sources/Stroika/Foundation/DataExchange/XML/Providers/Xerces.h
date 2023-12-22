/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Providers_Xerces_h_
#define _Stroika_Foundation_DataExchange_XML_Providers_Xerces_h_ 1

#include "../../../StroikaPreComp.h"

static_assert (qHasFeature_Xerces, "Don't include this file if qHasFeature_Xerces not set");

// // Not sure if we want this defined HERE or in the MAKEFILE/PROJECT FILE
// #define XML_LIBRARY 1
// #define XERCES_STATIC_LIBRARY 1

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

}

#endif /*_Stroika_Foundation_DataExchange_XML_Providers_Xerces_h_*/
