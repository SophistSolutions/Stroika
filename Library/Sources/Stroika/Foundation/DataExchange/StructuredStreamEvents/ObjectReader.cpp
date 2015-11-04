/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/CString/Utilities.h"
#include    "../../Characters/FloatConversion.h"
#include    "../../Characters/Format.h"
#include    "../../Characters/String2Int.h"
#include    "../../Characters/String_Constant.h"
#include    "../BadFormatException.h"

#include    "ObjectReader.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::StructuredStreamEvents;


using   Memory::Byte;



/*
 ********************************************************************************
 ************ StructuredStreamEvents::ObjectReader::Context *********************
 ********************************************************************************
 */
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
String ObjectReader::Context::TraceLeader_ () const
{
    static  const   String_Constant     kOneTabLevel_ { L"    " };
    return kOneTabLevel_.Repeat (fStack_.size ());
}
#endif




/*
 ********************************************************************************
 ******** StructuredStreamEvents::ObjectReader::IConsumerDelegateToContext ******
 ********************************************************************************
 */
ObjectReader::IConsumerDelegateToContext::IConsumerDelegateToContext (Context& r)
    : fContext_ (r)
{
}

void    ObjectReader::IConsumerDelegateToContext::StartElement (const StructuredStreamEvents::Name& name)
{
    AssertNotNull (fContext_.GetTop ());
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext_.fTraceThisReader) {
        DbgTrace (L"%sCalling IConsumerDelegateToContext::HandleChildStart ('%s')...", fContext_.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
    }
#endif
    fContext_.GetTop ()->HandleChildStart (fContext_, name);
}
void    ObjectReader::IConsumerDelegateToContext::EndElement (const StructuredStreamEvents::Name& name)
{
    AssertNotNull (fContext_.GetTop ());
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext_.fTraceThisReader) {
        DbgTrace (L"%sCalling IConsumerDelegateToContext::EndElement ('%s')...", fContext_.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
    }
#endif
    fContext_.GetTop ()->HandleEndTag (fContext_);
}
void    ObjectReader::IConsumerDelegateToContext::TextInsideElement (const String& text)
{
    AssertNotNull (fContext_.GetTop ());
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext_.fTraceThisReader) {
        DbgTrace (L"%sCalling IConsumerDelegateToContext::TextInsideElement ('%s')...", fContext_.TraceLeader_ ().c_str (), text.LimitLength (50).c_str ());
    }
#endif
    fContext_.GetTop ()->HandleTextInside (fContext_, text);
}


/*
 ********************************************************************************
 ***************** StructuredStreamEvents::ObjectReader *************************
 ********************************************************************************
 */
namespace   {
    // @todo see https://stroika.atlassian.net/browse/STK-284
    struct DocumentReader_ : public ObjectReader::IContextReader {
        shared_ptr<IContextReader>      fDocEltBuilder;
        bool                            fAnyDocElt;
        String                          fDocEltURI;
        String                          fDocEltName;
        DocumentReader_ (const shared_ptr<IContextReader>& docEltBuilder)
            : fDocEltBuilder (docEltBuilder)
            , fAnyDocElt (true)
            , fDocEltURI ()
            , fDocEltName ()
        {
        }
        DocumentReader_ (const shared_ptr<IContextReader>& docEltBuilder, const String& checkURI, const String& checkDocEltName)
            : fDocEltBuilder (docEltBuilder)
            , fAnyDocElt (false)
            , fDocEltURI (checkURI)
            , fDocEltName (checkDocEltName)
        {
        }
        virtual void    HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override
        {
            if (not fAnyDocElt) {
                if (name.fLocalName != fDocEltName or name.fNamespaceURI.Value () != fDocEltURI) {
                    ThrowUnRecognizedStartElt (name);
                }
            }
            r.Push (fDocEltBuilder);
        }
        virtual void    HandleTextInside (ObjectReader::Context& r, const String& text)  override
        {
            // OK so long as text is whitespace - or comment. Probably should check/assert, but KISS..., and count on validation to
            // assure input is valid
            Assert (text.IsWhitespace ());
        }
        virtual void    HandleEndTag (ObjectReader::Context& r) override
        {
            r.Pop ();
        }
    };
}
void    ObjectReader::Run (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<IContextReader>& docEltBuilder, const Streams::InputStream<Byte>& in)
{
    // @todo see https://stroika.atlassian.net/browse/STK-284
    RequireNotNull (docEltBuilder);
    Context ctx { objectReaderRegistry };
    Require (ctx.fStack_.size () == 0);

    ctx.Push (make_shared<DocumentReader_> (docEltBuilder));

    IConsumerDelegateToContext cb (ctx);
    XML::SAXParse (in, cb);

    ctx.Pop (); // the docuemnt reader we just added

    Ensure (ctx.fStack_.size () == 0);
}

void    ObjectReader::Run (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<IContextReader>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, const Streams::InputStream<Byte>& in)
{
    // @todo see https://stroika.atlassian.net/browse/STK-284
    RequireNotNull (docEltBuilder);
    Context ctx { objectReaderRegistry };
    Require (ctx.fStack_.size () == 0);

    ctx.Push (make_shared<DocumentReader_> (docEltBuilder, docEltUri, docEltLocalName));

    IConsumerDelegateToContext cb (ctx);
    XML::SAXParse (in, cb);

    ctx.Pop (); // the docuemnt reader we just added

    Ensure (ctx.fStack_.size () == 0);
}








/*
 ********************************************************************************
 ****************** StructuredStreamEvents::ThrowUnRecognizedStartElt ***********
 ********************************************************************************
 */
void    StructuredStreamEvents::ThrowUnRecognizedStartElt (const StructuredStreamEvents::Name& name)
{
    Execution::DoThrow (BadFormatException (Characters::CString::Format (L"Unrecognized start tag '%s'", name.fLocalName.c_str ())));
}


