/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../DataExchange/BadFormatException.h"
#include "../../../Debug/Trace.h"
#include "../../../Execution/Throw.h"
#include "../../../Memory/Common.h"
#include "../../../Memory/MemoryAllocator.h"

#include "Xerces.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qHasFeature_Xerces && defined(_MSC_VER)

using namespace Providers::Xerces;

#if 0
// now handled through pkg-config/configure
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#if qDebug
#pragma comment(lib, "xerces-c_3d.lib")
#else
#pragma comment(lib, "xerces-c_3.lib")
#endif
#endif
#endif

#if qHasFeature_Xerces

XERCES_CPP_NAMESPACE_USE
void Map2StroikaExceptionsErrorReporter::error ([[maybe_unused]] const unsigned int errCode, [[maybe_unused]] const XMLCh* const errDomain,
                                                [[maybe_unused]] const ErrTypes type, const XMLCh* const errorText,
                                                [[maybe_unused]] const XMLCh* const systemId, [[maybe_unused]] const XMLCh* const publicId,
                                                const XMLFileLoc lineNum, const XMLFileLoc colNum)
{
    Execution::Throw (BadFormatException{errorText, static_cast<unsigned int> (lineNum), static_cast<unsigned int> (colNum), 0});
}
void Map2StroikaExceptionsErrorReporter::resetErrors ()
{
}
void Map2StroikaExceptionsErrorReporter::warning ([[maybe_unused]] const SAXParseException& exc)
{
    // ignore
}
void Map2StroikaExceptionsErrorReporter::error (const SAXParseException& exc)
{
    Execution::Throw (BadFormatException{exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()),
                                         static_cast<unsigned int> (exc.getColumnNumber ()), 0});
}
void Map2StroikaExceptionsErrorReporter::fatalError (const SAXParseException& exc)
{
    Execution::Throw (BadFormatException{exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()),
                                         static_cast<unsigned int> (exc.getColumnNumber ()), 0});
}

void Providers::Xerces::SetupCommonParserFeatures (SAX2XMLReader& reader)
{
    reader.setFeature (XMLUni::fgSAX2CoreNameSpaces, true);
    reader.setFeature (XMLUni::fgXercesDynamic, false);
    reader.setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, false); // false:  * *Do not report attributes used for Namespace declarations, and optionally do not report original prefixed names
}
void Providers::Xerces::SetupCommonParserFeatures (SAX2XMLReader& reader, bool validatingWithSchema)
{
    reader.setFeature (XMLUni::fgXercesSchema, validatingWithSchema);
    reader.setFeature (XMLUni::fgSAX2CoreValidation, validatingWithSchema);

    // The purpose of this maybe to find errors with the schema itself, in which case,
    // we shouldn't bother (esp for release builds)
    //  (leave for now til we performance test)
    //      -- LGP 2007-06-21
    reader.setFeature (XMLUni::fgXercesSchemaFullChecking, validatingWithSchema);
    reader.setFeature (XMLUni::fgXercesUseCachedGrammarInParse, validatingWithSchema);
    reader.setFeature (XMLUni::fgXercesIdentityConstraintChecking, validatingWithSchema);

    // we only want to use loaded schemas - don't save any more into the grammar cache, since that
    // is global/shared.
    reader.setFeature (XMLUni::fgXercesCacheGrammarFromParse, false);
}

String Providers::Xerces::xercesString2String (const XMLCh* s, const XMLCh* e)
{
    if constexpr (same_as<XMLCh, char16_t>) {
        return String{span{s, e}};
    }
    // nb: casts required cuz Xerces doesn't (currently) use wchar_t/char16_t/char32_t but something the sizeof char16_t
    // --LGP 2016-07-29
    if constexpr (sizeof (XMLCh) == sizeof (char16_t)) {
        return String{span{reinterpret_cast<const char16_t*> (s), reinterpret_cast<const char16_t*> (e)}};
    }
    else if constexpr (sizeof (XMLCh) == sizeof (char32_t)) {
        return String{span{reinterpret_cast<const char32_t*> (s), reinterpret_cast<const char32_t*> (e)}};
    }
    else {
        AssertNotReached ();
        return String{};
    }
}
String Providers::Xerces::xercesString2String (const XMLCh* t)
{
    if constexpr (same_as<XMLCh, char16_t>) {
        return String{t};
    }
    // nb: casts required cuz Xerces doesn't (currently) use wchar_t/char16_t/char32_t but something the sizeof char16_t
    // --LGP 2016-07-29
    if constexpr (sizeof (XMLCh) == sizeof (char16_t)) {
        return String{reinterpret_cast<const char16_t*> (t)};
    }
    else if constexpr (sizeof (XMLCh) == sizeof (char32_t)) {
        return String{reinterpret_cast<const char32_t*> (t)};
    }
    else {
        AssertNotReached ();
        return String{};
    }
}

#endif
