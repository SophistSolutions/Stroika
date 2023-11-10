/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "Stroika/Foundation/IO/FileSystem/PathName.h"

#include "Schema.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qStroika_Foundation_DataExchange_XML_SupportSchema
using std::byte;

using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::XML::DOM;

// avoid namespace conflcit with some Xerces code
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
#if qDebug
#define Assert(c)                                                                                                                          \
    (!!(c) || (Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Assert", #c, __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_), false))
#else
#define Assert(c) ((void)0)
#endif
XERCES_CPP_NAMESPACE_USE
#endif

/*
 *  UnderlyingXMLLibExcptionMapping layer
 */
#if qHasFeature_Xerces
#define START_LIB_EXCEPTION_MAPPER try {
#define END_LIB_EXCEPTION_MAPPER                                                                                                           \
    }                                                                                                                                      \
    catch (const OutOfMemoryException&)                                                                                                    \
    {                                                                                                                                      \
        Execution::Throw (bad_alloc{}, "xerces OutOfMemoryException - throwing bad_alloc");                                                \
    }                                                                                                                                      \
    catch (...)                                                                                                                            \
    {                                                                                                                                      \
        Execution::ReThrow ();                                                                                                             \
    }
#else
#define START_LIB_EXCEPTION_MAPPER
#define END_LIB_EXCEPTION_MAPPER
#endif

#if qHasFeature_Xerces
namespace {
    class MySchemaResolver : public XMLEntityResolver {
    private:
        vector<Schema::SourceComponent> fSourceComponents;

    public:
        MySchemaResolver (const vector<Schema::SourceComponent>& sourceComponents)
            : fSourceComponents{sourceComponents}
        {
        }

    public:
        virtual InputSource* resolveEntity (XMLResourceIdentifier* resourceIdentifier) override
        {
            TraceContextBumper ctx{"XMLDB::{}::MySchemaResolver::resolveEntity"};
            RequireNotNull (resourceIdentifier);
            // Treat namespaces and publicids as higher-priority matchers
            for (vector<Schema::SourceComponent>::const_iterator i = fSourceComponents.begin (); i != fSourceComponents.end (); ++i) {

                if (resourceIdentifier->getNameSpace () != nullptr and i->fNamespace and resourceIdentifier->getNameSpace () == i->fNamespace) {
                    return mkMemInputSrc_ (i->fBLOB);
                }
                if (resourceIdentifier->getPublicId () != nullptr and i->fPublicID and resourceIdentifier->getPublicId () == i->fPublicID) {
                    return mkMemInputSrc_ (i->fBLOB);
                }
            }
            for (vector<Schema::SourceComponent>::const_iterator i = fSourceComponents.begin (); i != fSourceComponents.end (); ++i) {
                if (resourceIdentifier->getSystemId () != nullptr and i->fSystemID and resourceIdentifier->getSystemId () == i->fSystemID) {
                    return mkMemInputSrc_ (i->fBLOB);
                }
            }
            for (vector<Schema::SourceComponent>::const_iterator i = fSourceComponents.begin (); i != fSourceComponents.end (); ++i) {
                String itemSysID;
                if (const XMLCh* s = resourceIdentifier->getSystemId (); itemSysID != nullptr) {
                    itemSysID = String{s};
                    if (optional<size_t> rr = itemSysID.RFind ('/')) {
                        itemSysID = itemSysID.SubString (*rr + 1);
                    }
                }
                if (i->fSystemID and i->fSystemID == itemSysID) {
                    return mkMemInputSrc_ (i->fBLOB);
                }
            }
            WeakAsserteNotReached (); // Untested if Xerces handles null here...
            return nullptr;
        }

    private:
        static InputSource* mkMemInputSrc_ (const Memory::BLOB& schemaData)
        {
            // copy RAM to C++ array - freed by MemBufInputSource - adopt flag....
            XMLByte* useBuf = new XMLByte[schemaData.GetSize ()];
            memcpy (useBuf, schemaData.begin (), schemaData.GetSize ());
            return new MemBufInputSource (useBuf, schemaData.GetSize (), "", true);
        }
    };
    inline void SetupCommonParserFeatures_ (SAX2XMLReader& reader, bool validatingWithSchema)
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
}

namespace {
#if qHasFeature_Xerces
    class MyErrorReproter : public XMLErrorReporter, public ErrorHandler {
        // XMLErrorReporter
    public:
        virtual void error ([[maybe_unused]] const unsigned int errCode, [[maybe_unused]] const XMLCh* const errDomain,
                            [[maybe_unused]] const ErrTypes type, const XMLCh* const errorText, [[maybe_unused]] const XMLCh* const systemId,
                            [[maybe_unused]] const XMLCh* const publicId, const XMLFileLoc lineNum, const XMLFileLoc colNum) override
        {
            Execution::Throw (BadFormatException{errorText, static_cast<unsigned int> (lineNum), static_cast<unsigned int> (colNum), 0});
        }
        virtual void resetErrors () override
        {
        }

        // ErrorHandler
    public:
        virtual void warning ([[maybe_unused]] const SAXParseException& exc) override
        {
            // ignore
        }
        virtual void error (const SAXParseException& exc) override
        {
            Execution::Throw (BadFormatException (exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()),
                                                  static_cast<unsigned int> (exc.getColumnNumber ()), 0));
        }
        virtual void fatalError (const SAXParseException& exc) override
        {
            Execution::Throw (BadFormatException (exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()),
                                                  static_cast<unsigned int> (exc.getColumnNumber ()), 0));
        }
    };
    MyErrorReproter sMyErrorReproter;
#endif
}

/*
 ********************************************************************************
 *********************************** Schema *************************************
 ********************************************************************************
 */
Schema::AccessCompiledXSD::AccessCompiledXSD (const Schema& schema2Access)
    : fSchema2Access{schema2Access}
{
}

Schema::T_CompiledXSDRep Schema::AccessCompiledXSD::GetCachedTRep () const
{
    return fSchema2Access.GetCachedTRep_ ();
}

/*
 ********************************************************************************
 *********************************** Schema *************************************
 ********************************************************************************
 */

struct Schema::Rep_ {
    Rep_ (const String& targetNamespace, const Memory::BLOB& targetNamespaceData, const vector<SourceComponent>& sourceComponents,
          const NamespaceDefinitionsList& namespaceDefinitions)
        : fTargetNamespace{targetNamespace}
        , fSourceComponents{sourceComponents}
        , fNamespaceDefinitions{namespaceDefinitions}
    {
        if (not targetNamespace.empty ()) {
            fSourceComponents.push_back (SourceComponent{.fBLOB = targetNamespaceData, .fNamespace = targetNamespace});
        }
        Memory::BLOB schemaData = targetNamespaceData;
        AssertNotNull (XMLPlatformUtils::fgMemoryManager);
        XMLGrammarPoolImpl* grammarPool = new (XMLPlatformUtils::fgMemoryManager) XMLGrammarPoolImpl (XMLPlatformUtils::fgMemoryManager);
        try {
            Require (not schemaData.empty ()); // checked above
            MemBufInputSource mis{reinterpret_cast<const XMLByte*> (schemaData.begin ()), schemaData.GetSize (),
                                  targetNamespace.As<u16string> ().c_str ()};

            MySchemaResolver mySchemaResolver{sourceComponents};
            // Directly construct SAX2XMLReaderImpl so we can use XMLEntityResolver - which passes along namespace (regular
            // EntityResolve just passes systemID
            //      shared_ptr<SAX2XMLReader>   reader = shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, grammarPool));
            //
            shared_ptr<SAX2XMLReaderImpl> reader = shared_ptr<SAX2XMLReaderImpl> (
                new (XMLPlatformUtils::fgMemoryManager) SAX2XMLReaderImpl (XMLPlatformUtils::fgMemoryManager, grammarPool));
            reader->setXMLEntityResolver (&mySchemaResolver);

            SetupCommonParserFeatures_ (*reader, true);

            // Reset fgXercesCacheGrammarFromParse to TRUE so we actually load the XSD here
            reader->setFeature (XMLUni::fgXercesCacheGrammarFromParse, true);

            reader->setErrorHandler (&sMyErrorReproter);

            reader->loadGrammar (mis, Grammar::SchemaGrammarType, true);
        }
        catch (...) {
            delete grammarPool;
            Execution::ReThrow ();
        }
        fCachedGrammarPool = grammarPool;
    }
    Rep_ (const Rep_& from) = delete;
    ~Rep_ ()
    {
        delete fCachedGrammarPool;
    }
    String                   fTargetNamespace;
    vector<SourceComponent>  fSourceComponents;
    NamespaceDefinitionsList fNamespaceDefinitions;
    T_CompiledXSDRep         fCachedGrammarPool{nullptr};
};

Schema::Schema (const String& targetNamespace, const Memory::BLOB& targetNamespaceData, const vector<SourceComponent>& otherSources,
                const NamespaceDefinitionsList& namespaceDefs)
    : fRep{make_shared<Rep_> (targetNamespace, targetNamespaceData, otherSources, namespaceDefs)}
{
}

vector<Schema::SourceComponent> Schema::GetSourceComponents () const
{
    return fRep->fSourceComponents;
}

String Schema::GetTargetNamespace () const
{
    return fRep->fTargetNamespace;
}

NamespaceDefinitionsList Schema::GetNamespaceDefinitions () const
{
    return fRep->fNamespaceDefinitions;
}

Schema::T_CompiledXSDRep Schema::GetCachedTRep_ () const
{
    return fRep->fCachedGrammarPool;
}

Memory::BLOB Schema::GetSchemaData_ () const
{
    auto targetNS = GetTargetNamespace ();
    for (vector<Schema::SourceComponent>::const_iterator i = fRep->fSourceComponents.begin (); i != fRep->fSourceComponents.end (); ++i) {
        if (targetNS == i->fNamespace) {
            return i->fBLOB;
        }
    }
    Require (false); // must have provided data by now or we cannot construct the actual implementaiton object for the schema
    return Memory::BLOB{};
}

/*
 ********************************************************************************
 ********************************* TempSchemaSetter *****************************
 ********************************************************************************
 */
TempSchemaSetter::TempSchemaSetter (Document& doc, const Schema* s)
    : fDoc (&doc)
    , fOldSchema (doc.GetSchema ())
{
    if (fOldSchema == s) {
        fDoc = nullptr; // don't bother reseting here or our DTOR
    }
    else {
        doc.SetSchema (s);
    }
}

TempSchemaSetter::~TempSchemaSetter ()
{
    if (fDoc != nullptr) {
        try {
            fDoc->SetSchema (fOldSchema);
        }
        catch (...) {
            DbgTrace ("serious error - ~TempSchemaSetter exception caught - ignoring");
        }
    }
}

/*
 ********************************************************************************
 ****************************** ValidateExternalFile ****************************
 ********************************************************************************
 */
void DataExchange::XML::ValidateExternalFile (const filesystem::path& externalFileName, const Schema& schema)
{
    START_LIB_EXCEPTION_MAPPER
    {
        Schema::AccessCompiledXSD accessSchema (schema); // REALLY need READLOCK - cuz this just prevents UPDATE of Scehma (never happens anyhow) -- LGP 2009-05-19
        shared_ptr<SAX2XMLReader> parser =
            shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema.GetCachedTRep ()));
        SetupCommonParserFeatures_ (*parser, true);
        parser->setErrorHandler (&sMyErrorReproter);
        parser->parse (IO::FileSystem::FromPath (externalFileName).As<u16string> ().c_str ());
    }
    END_LIB_EXCEPTION_MAPPER
}

#endif