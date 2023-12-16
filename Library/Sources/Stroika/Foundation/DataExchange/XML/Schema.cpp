/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"

#include "Schema.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using std::byte;

using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::XML::Schema;

#if qHasFeature_Xerces
#include "Providers/Xerces.h"

using namespace Stroika::Foundation::DataExchange::XML::Providers::Xerces;

namespace {
    struct MySchemaResolver_ : public XMLEntityResolver {
    private:
        Sequence<Schema::SourceComponent> fSourceComponents;

    public:
        MySchemaResolver_ (const Sequence<Schema::SourceComponent>& sourceComponents)
            : fSourceComponents{sourceComponents}
        {
        }
        virtual InputSource* resolveEntity (XMLResourceIdentifier* resourceIdentifier) override
        {
            TraceContextBumper ctx{"XMLDB::{}::MySchemaResolver_::resolveEntity"};
            RequireNotNull (resourceIdentifier);
            // Treat namespaces and publicids as higher-priority matchers
            for (auto i = fSourceComponents.begin (); i != fSourceComponents.end (); ++i) {

                if (resourceIdentifier->getNameSpace () != nullptr and i->fNamespace and resourceIdentifier->getNameSpace () == i->fNamespace) {
                    return mkMemInputSrc_ (i->fBLOB);
                }
                if (resourceIdentifier->getPublicId () != nullptr and i->fPublicID and resourceIdentifier->getPublicId () == i->fPublicID) {
                    return mkMemInputSrc_ (i->fBLOB);
                }
            }
            for (auto i = fSourceComponents.begin (); i != fSourceComponents.end (); ++i) {
                if (resourceIdentifier->getSystemId () != nullptr and i->fSystemID and resourceIdentifier->getSystemId () == i->fSystemID) {
                    return mkMemInputSrc_ (i->fBLOB);
                }
            }
            for (auto i = fSourceComponents.begin (); i != fSourceComponents.end (); ++i) {
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

    struct XercesSchemaRep_ : Schema::IRep, IXercesSchemaRep {
        XercesSchemaRep_ (const optional<URI>& targetNamespace, const Memory::BLOB& targetNamespaceData,
                          const Sequence<SourceComponent>& sourceComponents, const NamespaceDefinitionsList& namespaceDefinitions)
            : fTargetNamespace{targetNamespace}
            , fSourceComponents{sourceComponents}
            , fNamespaceDefinitions{namespaceDefinitions}
        {
            if (targetNamespace) {
                fSourceComponents.push_back (SourceComponent{.fBLOB = targetNamespaceData, .fNamespace = *targetNamespace});
            }
            Memory::BLOB schemaData = targetNamespaceData;
            AssertNotNull (XMLPlatformUtils::fgMemoryManager);
            XMLGrammarPoolImpl* grammarPool = new (XMLPlatformUtils::fgMemoryManager) XMLGrammarPoolImpl (XMLPlatformUtils::fgMemoryManager);
            try {
                Require (not schemaData.empty ()); // checked above
                MemBufInputSource mis{reinterpret_cast<const XMLByte*> (schemaData.begin ()), schemaData.GetSize (),
                                      (targetNamespace == nullopt ? u16string{} : targetNamespace->As<String> ().As<u16string> ()).c_str ()};

                MySchemaResolver_ mySchemaResolver{sourceComponents};
                // Directly construct SAX2XMLReaderImpl so we can use XMLEntityResolver - which passes along namespace (regular
                // EntityResolve just passes systemID
                //      shared_ptr<SAX2XMLReader>   reader = shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, grammarPool));
                //
                shared_ptr<SAX2XMLReaderImpl> reader = shared_ptr<SAX2XMLReaderImpl> (
                    new (XMLPlatformUtils::fgMemoryManager) SAX2XMLReaderImpl (XMLPlatformUtils::fgMemoryManager, grammarPool));
                reader->setXMLEntityResolver (&mySchemaResolver);

                SetupCommonParserFeatures (*reader, true);

                // Reset fgXercesCacheGrammarFromParse to TRUE so we actually load the XSD here
                reader->setFeature (XMLUni::fgXercesCacheGrammarFromParse, true);

                reader->setErrorHandler (&fErrorReporter_);

                reader->loadGrammar (mis, Grammar::SchemaGrammarType, true);
            }
            catch (...) {
                delete grammarPool;
                Execution::ReThrow ();
            }
            fCachedGrammarPool = grammarPool;
        }
        XercesSchemaRep_ (const XercesSchemaRep_&) = delete;
        virtual ~XercesSchemaRep_ ()
        {
            delete fCachedGrammarPool;
        }
        optional<URI>                      fTargetNamespace;
        Sequence<SourceComponent>          fSourceComponents;
        NamespaceDefinitionsList           fNamespaceDefinitions;
        xercesc_3_2::XMLGrammarPool*       fCachedGrammarPool{nullptr};
        Map2StroikaExceptionsErrorReporter fErrorReporter_;

        virtual optional<URI> GetTargetNamespace () const override
        {
            return fTargetNamespace;
        }
        virtual NamespaceDefinitionsList GetNamespaceDefinitions () const override
        {
            return fNamespaceDefinitions;
        }
        virtual Sequence<SourceComponent> GetSourceComponents () override
        {
            return fSourceComponents;
        }

        virtual xercesc_3_2::XMLGrammarPool* GetCachedGrammarPool () override
        {
            return fCachedGrammarPool;
        }
    };

}
#endif

Schema::Ptr Schema::New ([[maybe_unused]] Provider p, const optional<URI>& targetNamespace, const Memory::BLOB& targetNamespaceData,
                         const Sequence<SourceComponent>& sourceComponents, const NamespaceDefinitionsList& namespaceDefinitions)
{
#if qHasFeature_Xerces
    return Ptr{make_shared<XercesSchemaRep_> (targetNamespace, targetNamespaceData, sourceComponents, namespaceDefinitions)};
#endif
}

#if qStroika_Foundation_DataExchange_XML_SupportSchema

/*
 ********************************************************************************
 ****************************** ValidateExternalFile ****************************
 ********************************************************************************
 */
void DataExchange::XML::ValidateExternalFile (const filesystem::path& externalFileName, const Schema::Ptr& schema)
{
#if qHasFeature_Xerces
    START_LIB_EXCEPTION_MAPPER
    {
        shared_ptr<IXercesSchemaRep> accessSchema = dynamic_pointer_cast<IXercesSchemaRep> (schema.GetRep ());
        shared_ptr<SAX2XMLReader>    parser       = shared_ptr<SAX2XMLReader> (
            XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema->GetCachedGrammarPool ()));
        SetupCommonParserFeatures (*parser, true);
        Map2StroikaExceptionsErrorReporter errReporter;
        parser->setErrorHandler (&errReporter);
        parser->parse (IO::FileSystem::FromPath (externalFileName).As<u16string> ().c_str ());
    }
    END_LIB_EXCEPTION_MAPPER
#endif
}

#endif