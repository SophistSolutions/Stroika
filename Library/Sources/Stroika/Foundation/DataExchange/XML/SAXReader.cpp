/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <atomic>

#include "../../Debug/Trace.h"
#include "../../Execution/Common.h"
#include "../../Execution/ProgressMonitor.h"
#include "../../Memory/Common.h"
#include "../../Memory/MemoryAllocator.h"

#include "../BadFormatException.h"

#include "SAXReader.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qHasFeature_Xerces
#include "Providers/Xerces.h"

using namespace Stroika::Foundation::DataExchange::XML::Providers::Xerces;

#endif

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

CompileTimeFlagChecker_SOURCE (Stroika::Foundation::DataExchange::XML, qHasFeature_Xerces, qHasFeature_Xerces);
#if qHasFeature_Xerces
namespace {
    // These SHOULD be part of xerces! Perhaps someday post them?
    class StdIStream_InputSource_ : public InputSource {
    protected:
        class StdIStream_InputStream : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream {
        public:
            StdIStream_InputStream (InputStream<byte>::Ptr in)
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
                return fSource.Read (reinterpret_cast<byte*> (toFill), reinterpret_cast<byte*> (toFill) + maxToRead);
            }
            virtual const XMLCh* getContentType () const override
            {
                return nullptr;
            }

        protected:
            InputStream<byte>::Ptr fSource;
        };

    public:
        StdIStream_InputSource_ (InputStream<byte>::Ptr in, const XMLCh* const bufId = nullptr)
            : InputSource{bufId}
            , fSource{in}
        {
        }
        virtual BinInputStream* makeStream () const override
        {
            return new (getMemoryManager ()) StdIStream_InputStream{fSource};
        }

    protected:
        InputStream<byte>::Ptr fSource;
    };

    // my variations of StdIInputSrc with progresstracker callback
    class StdIStream_InputSourceWithProgress_ : public StdIStream_InputSource_ {
    protected:
        class ISWithProg : public StdIStream_InputSource_::StdIStream_InputStream {
        public:
            ISWithProg (const InputStream<byte>::Ptr& in, ProgressMonitor::Updater progressCallback)
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
                XMLSize_t result = fSource.Read (reinterpret_cast<byte*> (toFill), reinterpret_cast<byte*> (toFill) + maxToRead);
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
        StdIStream_InputSourceWithProgress_ (InputStream<byte>::Ptr in, ProgressMonitor::Updater progressUpdater, const XMLCh* const bufId = nullptr)
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
}
#endif

/*
 ********************************************************************************
 ************************************* SAXParse *********************************
 ********************************************************************************
 */
#if qHasFeature_Xerces
namespace {
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

#endif

#if qStroika_Foundation_DataExchange_XML_SupportParsing

void XML::SAXParse ([[maybe_unused]] const Streams::InputStream<byte>::Ptr& in, [[maybe_unused]] StructuredStreamEvents::IConsumer& callback,
                    const Schema::Ptr& schema, [[maybe_unused]] ProgressMonitor::Updater progress)
{
#if qHasFeature_Xerces
    DependencyLibraryInitializer::sThe.UsingProvider (Provider::eXerces);
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
    constexpr XMLCh kBufID[] = {'S', 'A', 'X', ':', 'P', 'a', 'r', 's', 'e', '\0'};
    parser->parse (StdIStream_InputSourceWithProgress_{in, ProgressMonitor::Updater{progress, 0.1f, 0.9f}, kBufID});
#endif
}

void XML::SAXParse (const Memory::BLOB& in, StructuredStreamEvents::IConsumer& callback, const Schema::Ptr& schema, ProgressMonitor::Updater progress)
{
    SAXParse (in.As<Streams::InputStream<byte>::Ptr> (), callback, schema, progress);
}
#endif