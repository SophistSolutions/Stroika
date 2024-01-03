/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"

#include "SAXReader.h"
#include "Schema.h"

#include "Providers/IProvider.h"

#if qHasFeature_Xerces
#include "Providers/Xerces.h"
#endif

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qCompilerAndStdLib_clangWithLibStdCPPStringConstexpr_Buggy
namespace {
    inline std::u16string clang_string_BWA_ (const char16_t* a, const char16_t* b)
    {
        return {a, b};
    }
}
#endif

#if qStroika_Foundation_DataExchange_XML_SupportSchema
using namespace Stroika::Foundation::DataExchange::XML::Schema;

#if qHasFeature_Xerces
namespace {
    namespace XercesImpl_ {
        using namespace Stroika::Foundation::DataExchange::XML::Providers::Xerces;
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
                // @todo consider exposting this API outside the module, and/or providing option to wget missing namespaces, or have option for where to fetch from?
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
                return nullptr;
            }

        private:
            static InputSource* mkMemInputSrc_ (const Memory::BLOB& schemaData)
            {
                if (schemaData.empty ()) [[unlikely]] {
                    // not sure this is useful case? Should assert/throw?
                    return new MemBufInputSource{nullptr, 0, "", true};
                }
                else {
                    XMLByte* useBuf = new XMLByte[schemaData.GetSize ()];
                    memcpy (useBuf, schemaData.begin (), schemaData.GetSize ());
                    return new MemBufInputSource{useBuf, schemaData.GetSize (), "", true};
                }
            }
        };

        struct SchemaRep_ : Schema::IRep, IXercesSchemaRep {
            SchemaRep_ (const optional<URI>& targetNamespace, const Memory::BLOB& targetNamespaceData,
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
                XMLGrammarPoolImpl* grammarPool = new (XMLPlatformUtils::fgMemoryManager) XMLGrammarPoolImpl{XMLPlatformUtils::fgMemoryManager};
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
                        new (XMLPlatformUtils::fgMemoryManager) SAX2XMLReaderImpl{XMLPlatformUtils::fgMemoryManager, grammarPool});
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
            SchemaRep_ (const SchemaRep_&) = delete;
            virtual ~SchemaRep_ ()
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
                return fTargetNamespace; // should get from READING the schema itself! I THINK --LGP 2023-12-18
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
}
#endif

#if qStroika_Foundation_DataExchange_XML_SupportSchema
/*
 ********************************************************************************
 ************************************** ValidateFile ****************************
 ********************************************************************************
 */
Schema::Ptr Schema::New ([[maybe_unused]] Provider p, const optional<URI>& targetNamespace, const Memory::BLOB& targetNamespaceData,
                         const Sequence<SourceComponent>& sourceComponents, const NamespaceDefinitionsList& namespaceDefinitions)
{
#if qHasFeature_Xerces
    if (p == Provider::eXerces) {
        DependencyLibraryInitializer::sThe.UsingProvider (Provider::eXerces);
        return Ptr{make_shared<XercesImpl_::SchemaRep_> (targetNamespace, targetNamespaceData, sourceComponents, namespaceDefinitions)};
    }
#endif
#if qHasFeature_libxml2
    if (p == Provider::eLibXml2) {
        DependencyLibraryInitializer::sThe.UsingProvider (Provider::eLibXml2);
        return Ptr{Providers::kDefaultProvider ()->SchemaFactory (targetNamespace, targetNamespaceData, sourceComponents, namespaceDefinitions)};
    }
#endif
    return nullptr;
}

/*
 ********************************************************************************
 ************************************** ValidateFile ****************************
 ********************************************************************************
 */
void DataExchange::XML::ValidateFile (const filesystem::path& externalFileName, const Schema::Ptr& schema)
{
#if qHasFeature_Xerces
    START_LIB_EXCEPTION_MAPPER
    {
        using namespace XercesImpl_;
        shared_ptr<IXercesSchemaRep> accessSchema = dynamic_pointer_cast<IXercesSchemaRep> (schema.GetRep ());
        if (accessSchema) {
            shared_ptr<SAX2XMLReader> parser = shared_ptr<SAX2XMLReader> (
                XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema->GetCachedGrammarPool ()));
            SetupCommonParserFeatures (*parser, true);
            Map2StroikaExceptionsErrorReporter errReporter;
            parser->setErrorHandler (&errReporter);
            parser->parse (IO::FileSystem::FromPath (externalFileName).As<u16string> ().c_str ());
        }
    }
    END_LIB_EXCEPTION_MAPPER
#endif
#if qStroika_Foundation_DataExchange_XML_SupportParsing
    // not the most efficient impl but simple and generic, and good enuf for now
    struct noOpConsumer : StructuredStreamEvents::IConsumer {};
    noOpConsumer consumer;
    SAXParse (IO::FileSystem::FileInputStream::New (externalFileName), consumer, schema);
#endif
}
#endif

#endif