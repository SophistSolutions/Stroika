/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../DataExchange/BadFormatException.h"
#include "../../../Debug/Trace.h"
#include "../../../Execution/Throw.h"
#include "../../../Memory/Common.h"
#include "../../../Memory/MemoryAllocator.h"

#include "../../../Debug/Trace.h"
#include "../../../Memory/Common.h"
#include "../../../Memory/MemoryAllocator.h"

#include "Xerces.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::XML::Schema;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Streams;

using std::byte;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

static_assert (qHasFeature_Xerces, "Don't compile this file if qHasFeature_Xerces not set");

CompileTimeFlagChecker_SOURCE (Stroika::Foundation::DataExchange::XML, qHasFeature_Xerces, qHasFeature_Xerces);

using namespace Stroika::Foundation::DataExchange::XML;
using namespace Providers::Xerces;

#if qCompilerAndStdLib_clangWithLibStdCPPStringConstexpr_Buggy
namespace {
    inline std::u16string clang_string_BWA_ (const char16_t* a, const char16_t* b)
    {
        return {a, b};
    }
}
#endif

/*
     *  A helpful class to isolete Xerces (etc) memory management calls. Could be the basis
     *  of future perfomance/memory optimizations, but for now, just a helpful debugging/tracking
     *  class.
     */
struct Provider::MyXercesMemMgr_ : public MemoryManager {
public:
    MyXercesMemMgr_ ()
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
        : fAllocator{fBaseAllocator}
#endif
    {
    }
    ~MyXercesMemMgr_ ()
    {
        Assert (fAllocator.GetSnapshot ().fAllocations.empty ()); // else we have a memory leak
    }

#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
public:
    Memory::SimpleAllocator_CallLIBCNewDelete             fBaseAllocator;
    Memory::LeakTrackingGeneralPurposeAllocator           fAllocator;
    mutex                                                 fLastSnapshot_CritSection;
    Memory::LeakTrackingGeneralPurposeAllocator::Snapshot fLastSnapshot;
#endif

public:
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
    void DUMPCurMemStats ()
    {
        TraceContextBumper          ctx{"MyXercesMemMgr_::DUMPCurMemStats"};
        [[maybe_unused]] lock_guard critSec{fLastSnapshot_CritSection};
        fAllocator.DUMPCurMemStats (fLastSnapshot);
        // now copy current map to prev for next time this gets called
        fLastSnapshot = fAllocator.GetSnapshot ();
    }
#endif

public:
    virtual MemoryManager* getExceptionMemoryManager () override
    {
        return this;
    }
    virtual void* allocate (XMLSize_t size) override
    {
        try {
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            return fAllocator.Allocate (size);
#else
            return ::operator new (size);
#endif
        }
        catch (...) {
            // NB: use throw not Exception::Throw () since that requires its a subclass of exception (or SilentException)
            throw (OutOfMemoryException{}); // quirk cuz this is the class Xerces expects and catches internally (why not bad_alloc?) - sigh...
        }
    }
    virtual void deallocate (void* p) override
    {
        if (p != nullptr) {
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            return fAllocator.Deallocate (p);
#else
            ::operator delete (p);
#endif
        }
    }
};

namespace {
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

namespace {
    using namespace Stroika::Foundation::DataExchange::XML::Providers::Xerces;

    // These SHOULD be part of xerces! Perhaps someday post them?
    class StdIStream_InputSource_ : public InputSource {
    protected:
        class StdIStream_InputStream : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream {
        public:
            StdIStream_InputStream (InputStream::Ptr<byte> in)
                : fSource{in}
            {
            }
            ~StdIStream_InputStream () = default;

        public:
            virtual XMLFilePos curPos () const override
            {
                return fSource.GetOffset ();
            }
            virtual XMLSize_t readBytes (XMLByte* const toFill, const XMLSize_t maxToRead) override
            {
                return fSource.Read (span{reinterpret_cast<byte*> (toFill), maxToRead}).size ();
            }
            virtual const XMLCh* getContentType () const override
            {
                return nullptr;
            }

        protected:
            InputStream::Ptr<byte> fSource;
        };

    public:
        StdIStream_InputSource_ (InputStream::Ptr<byte> in, const XMLCh* const bufId = nullptr)
            : InputSource{bufId}
            , fSource{in}
        {
        }
        virtual BinInputStream* makeStream () const override
        {
            return new (getMemoryManager ()) StdIStream_InputStream{fSource};
        }

    protected:
        InputStream::Ptr<byte> fSource;
    };

#if 0
    // my variations of StdIInputSrc with progresstracker callback
    class StdIStream_InputSourceWithProgress_ : public StdIStream_InputSource_ {
    protected:
        class ISWithProg : public StdIStream_InputSource_::StdIStream_InputStream {
        public:
            ISWithProg (const InputStream::Ptr<byte>& in, ProgressMonitor::Updater progressCallback)
                : StdIStream_InputStream{in}
                , fProgress_{progressCallback, 0.0f, 1.0f}
                , fTotalSize_{0.0f}
            {
                // @todo - redo for if non-seekable streams - just set flag saying not seeakble and do right thing with progress. ....
                /// for now we raise exceptions
                SeekOffsetType start = in.GetOffset ();
                in.Seek (Whence::eFromEnd, 0);
                SeekOffsetType totalSize = in.GetOffset ();
                Assert (start <= totalSize);
                in.Seek (start);
                fTotalSize_ = static_cast<float> (totalSize);
            }

        public:
            virtual XMLSize_t readBytes (XMLByte* const toFill, const XMLSize_t maxToRead) override
            {
                using ProgressRangeType            = ProgressMonitor::ProgressRangeType;
                ProgressRangeType curOffset        = 0.0;
                bool              doProgressBefore = (maxToRead > 10 * 1024); // only bother calling both before & after if large read
                if (fTotalSize_ > 0.0f and doProgressBefore) {
                    curOffset = static_cast<ProgressRangeType> (fSource.GetOffset ());
                    fProgress_.SetProgress (curOffset / fTotalSize_);
                }
                XMLSize_t result = fSource.Read (span{reinterpret_cast<byte*> (toFill), maxToRead}).size ();
                if (fTotalSize_ > 0) {
                    curOffset = static_cast<ProgressRangeType> (fSource.GetOffset ());
                    fProgress_.SetProgress (curOffset / fTotalSize_);
                }
                return result;
            }

        private:
            ProgressMonitor::Updater fProgress_;
            float                    fTotalSize_;
        };

    public:
        StdIStream_InputSourceWithProgress_ (InputStream::Ptr<byte> in, ProgressMonitor::Updater progressUpdater, const XMLCh* const bufId = nullptr)
            : StdIStream_InputSource_{in, bufId}
            , fProgressCallback_{progressUpdater}
        {
        }
        virtual BinInputStream* makeStream () const override
        {
            return new (getMemoryManager ()) ISWithProg{fSource, fProgressCallback_};
        }

    private:
        ProgressMonitor::Updater fProgressCallback_;
    };
#endif
    class SAX2PrintHandlers_ : public DefaultHandler {
    private:
        StructuredStreamEvents::IConsumer& fCallback_;

    public:
        SAX2PrintHandlers_ (StructuredStreamEvents::IConsumer& callback)
            : fCallback_{callback}
        {
        }

    public:
        virtual void startDocument () override
        {
            fCallback_.StartDocument ();
        }
        virtual void endDocument () override
        {
            fCallback_.EndDocument ();
        }
        virtual void startElement (const XMLCh* const uri, const XMLCh* const localName, const XMLCh* const /*qname*/, const Attributes& attributes) override
        {
            Require (uri != nullptr);
            Require (localName != nullptr);
            using Name = StructuredStreamEvents::Name;
            Mapping<Name, String> useAttrs;
            size_t                attributesLen = attributes.getLength ();
            for (XMLSize_t i = 0; i < attributesLen; ++i) {
                Name attrName{xercesString2String (attributes.getURI (i)), xercesString2String (attributes.getLocalName (i)), Name::eAttribute};
                useAttrs.Add (attrName, xercesString2String (attributes.getValue (i)));
            }
            fCallback_.StartElement (Name{xercesString2String (uri), xercesString2String (localName)}, useAttrs);
        }
        virtual void endElement (const XMLCh* const uri, const XMLCh* const localName, [[maybe_unused]] const XMLCh* const qname) override
        {
            Require (uri != nullptr);
            Require (localName != nullptr);
            Require (qname != nullptr);
            fCallback_.EndElement (StructuredStreamEvents::Name{xercesString2String (uri), xercesString2String (localName)});
        }
        virtual void characters (const XMLCh* const chars, const XMLSize_t length) override
        {
            Require (chars != nullptr);
            Require (length != 0);
            fCallback_.TextInsideElement (xercesString2String (chars, chars + length));
        }
    };

}

/*
 ********************************************************************************
 *************** Provider::Xerces::Map2StroikaExceptionsErrorReporter ***********
 ********************************************************************************
 */
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

/*
 ********************************************************************************
 ********************* Provider::Xerces::SetupCommonParserFeatures **************
 ********************************************************************************
 */
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

/*
 ********************************************************************************
 ********************* Provider::Xerces::xercesString2String ********************
 ********************************************************************************
 */
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

/*
 ********************************************************************************
 ***************** XML::Providers::LibXML2::Provider ***************
 ********************************************************************************
 */
Providers::Xerces::Provider::Provider ()
{
    TraceContextBumper ctx{"Xerces::Provider::CTOR"};
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
    fUseXercesMemoryManager = new MyXercesMemMgr_{};
#endif
    XMLPlatformUtils::Initialize (XMLUni::fgXercescDefaultLocale, 0, 0, fUseXercesMemoryManager);
}

Providers::Xerces::Provider::~Provider ()
{
    TraceContextBumper ctx{"Xerces::Provider::DTOR"};
    XMLPlatformUtils::Terminate ();
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
    delete fUseXercesMemoryManager; // checks for leaks
#endif
}

shared_ptr<Schema::IRep> Providers::Xerces::Provider::SchemaFactory (const optional<URI>& targetNamespace, const BLOB& targetNamespaceData,
                                                                     const Sequence<Schema::SourceComponent>& sourceComponents,
                                                                     const NamespaceDefinitionsList&          namespaceDefinitions) const
{
    return make_shared<SchemaRep_> (targetNamespace, targetNamespaceData, sourceComponents, namespaceDefinitions);
}
shared_ptr<DOM::Document::IRep> Providers::Xerces::Provider::DocumentFactory (const String& documentElementName, const optional<URI>& ns) const
{
    AssertNotImplemented ();
    return nullptr;
}

shared_ptr<DOM::Document::IRep> Providers::Xerces::Provider::DocumentFactory (const Streams::InputStream::Ptr<byte>& in,
                                                                              const Schema::Ptr& schemaToValidateAgainstWhileReading) const
{
    AssertNotImplemented ();
    return nullptr;
}
void Providers::Xerces::Provider::SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer& callback,
                                            const Schema::Ptr& schema) const
{
    SAX2PrintHandlers_           handler{callback};
    shared_ptr<SAX2XMLReader>    parser;
    shared_ptr<IXercesSchemaRep> accessSchema;
    if (schema != nullptr) {
        accessSchema = dynamic_pointer_cast<IXercesSchemaRep> (schema.GetRep ());
    }
    if (accessSchema) {
        parser = shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema->GetCachedGrammarPool ()));
    }
    else {
        parser = shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager));
    }
    SetupCommonParserFeatures (*parser, accessSchema != nullptr);
    parser->setContentHandler (&handler);
    Map2StroikaExceptionsErrorReporter mErrorReproter_;
    parser->setErrorHandler (&mErrorReproter_);
    // constexpr XMLCh kBufID[] = {'S', 'A', 'X', ':', 'P', 'a', 'r', 's', 'e', '\0'};
    parser->parse (StdIStream_InputSource_{in});
}
