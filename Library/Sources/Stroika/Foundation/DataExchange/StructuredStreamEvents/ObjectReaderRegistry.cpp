/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/CString/Utilities.h"
#include    "../../Characters/FloatConversion.h"
#include    "../../Characters/Format.h"
#include    "../../Characters/String2Int.h"
#include    "../../Characters/String_Constant.h"
#include    "../../Containers/MultiSet.h"
#include    "../../Debug/Trace.h"
#include    "../../Time/Date.h"
#include    "../../Time/DateRange.h"
#include    "../../Time/DateTime.h"
#include    "../../Time/DateTimeRange.h"
#include    "../../Time/Duration.h"
#include    "../../Time/DurationRange.h"

#include    "../BadFormatException.h"

#include    "ObjectReaderRegistry.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::StructuredStreamEvents;

using   Characters::String_Constant;
using   DataExchange::VariantValue;
using   Time::Date;
using   Time::DateTime;
using   Time::Duration;
using   Time::TimeOfDay;







// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1





/*
 ********************************************************************************
 ********************* DataExchange::ObjectReaderRegistry ************************
 ********************************************************************************
 */





/*
 ********************************************************************************
 ******************** StructuredStreamEvents::SimpleReader<> ********************
 ********************************************************************************
 */
template <>
void   SimpleReader<String>::Deactivating (Context& r)
{
    *fValue_ = fBuf_.str ();
}

template <>
void   SimpleReader<int>::Deactivating (Context& r)
{
    *fValue_ = Characters::String2Int<int> (fBuf_.str ());
}

template <>
void   SimpleReader<unsigned int>::Deactivating (Context& r)
{
    //@ todo fix
    *fValue_ = Characters::String2Int<int> (fBuf_.str ());
}

template <>
void   SimpleReader<bool>::Deactivating (Context& r)
{
    *fValue_ = (fBuf_.str ().ToLowerCase () == L"true");
}

template <>
void   SimpleReader<float>::Deactivating (Context& r)
{
    (*fValue_) = Characters::String2Float<float> (fBuf_.str ());
}

template <>
void   SimpleReader<double>::Deactivating (Context& r)
{
    (*fValue_) = Characters::String2Float<double> (fBuf_.str ());
}

template <>
void   SimpleReader<Time::DateTime>::Deactivating (Context& r)
{
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall (*fValue_ = Time::DateTime::Parse (fBuf_.str (), Time::DateTime::ParseFormat::eXML));
}







/*
 ********************************************************************************
 ****************** StructuredStreamEvents::IgnoreNodeReader ********************
 ********************************************************************************
 */
IgnoreNodeReader::IgnoreNodeReader ()
    : fDepth_ (0)
{
}

shared_ptr<IElementConsumer>    IgnoreNodeReader::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
{
    return shared_from_this ();
}








/*
 ********************************************************************************
 ************ StructuredStreamEvents::Context *********************
 ********************************************************************************
 */
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
String Context::TraceLeader_ () const
{
    static  const   String_Constant     kOneTabLevel_ { L"    " };
    return kOneTabLevel_.Repeat (fStack_.size ());
}
#endif




/*
 ********************************************************************************
 ******** StructuredStreamEvents::IConsumerDelegateToContext ******
 ********************************************************************************
 */
IConsumerDelegateToContext::IConsumerDelegateToContext (Context& r)
    : fContext_ (r)
{
}

void    IConsumerDelegateToContext::StartElement (const StructuredStreamEvents::Name& name)
{
    AssertNotNull (fContext_.GetTop ());
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext_.fTraceThisReader) {
        DbgTrace (L"%sCalling IConsumerDelegateToContext::HandleChildStart ('%s')...", fContext_.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
    }
#endif
    if (shared_ptr<IElementConsumer> eltToPush = fContext_.GetTop ()->HandleChildStart (fContext_, name)) {
        fContext_.Push (eltToPush);
    }
}
void    IConsumerDelegateToContext::EndElement (const StructuredStreamEvents::Name& name)
{
    AssertNotNull (fContext_.GetTop ());
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext_.fTraceThisReader) {
        DbgTrace (L"%sCalling IConsumerDelegateToContext::EndElement ('%s')...", fContext_.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
    }
#endif
    if (fContext_.GetTop ()->HandleEndTag (fContext_)) {
        fContext_.Pop ();
    }
}
void    IConsumerDelegateToContext::TextInsideElement (const String& text)
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
    struct DocumentReader_ : public IElementConsumer {
        shared_ptr<IElementConsumer>      fDocEltBuilder;
        bool                            fAnyDocElt;
        String                          fDocEltURI;
        String                          fDocEltName;
        DocumentReader_ (const shared_ptr<IElementConsumer>& docEltBuilder)
            : fDocEltBuilder (docEltBuilder)
            , fAnyDocElt (true)
            , fDocEltURI ()
            , fDocEltName ()
        {
        }
        DocumentReader_ (const shared_ptr<IElementConsumer>& docEltBuilder, const String& checkURI, const String& checkDocEltName)
            : fDocEltBuilder (docEltBuilder)
            , fAnyDocElt (false)
            , fDocEltURI (checkURI)
            , fDocEltName (checkDocEltName)
        {
        }
        virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const StructuredStreamEvents::Name& name) override
        {
            if (not fAnyDocElt) {
                if (name.fLocalName != fDocEltName or name.fNamespaceURI.Value () != fDocEltURI) {
                    ThrowUnRecognizedStartElt (name);
                }
            }
            return (fDocEltBuilder);
        }
        virtual void    HandleTextInside (Context& r, const String& text)  override
        {
            // OK so long as text is whitespace - or comment. Probably should check/assert, but KISS..., and count on validation to
            // assure input is valid
            Assert (text.IsWhitespace ());
        }
    };
}
void    StructuredStreamEvents::Run (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<IElementConsumer>& docEltBuilder, const Streams::InputStream<Byte>& in)
{
    // @todo see https://stroika.atlassian.net/browse/STK-284
    RequireNotNull (docEltBuilder);
    Context ctx { objectReaderRegistry };
    Require (ctx.empty ());

    ctx.Push (make_shared<DocumentReader_> (docEltBuilder));

    IConsumerDelegateToContext cb (ctx);
    XML::SAXParse (in, cb);

    ctx.Pop (); // the docuemnt reader we just added

    Require (ctx.empty ());
}

void    StructuredStreamEvents::Run (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<IElementConsumer>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, const Streams::InputStream<Byte>& in)
{
    // @todo see https://stroika.atlassian.net/browse/STK-284
    RequireNotNull (docEltBuilder);
    Context ctx { objectReaderRegistry };
    Require (ctx.empty ());

    ctx.Push (make_shared<DocumentReader_> (docEltBuilder, docEltUri, docEltLocalName));

    IConsumerDelegateToContext cb (ctx);
    XML::SAXParse (in, cb);

    ctx.Pop (); // the docuemnt reader we just added

    Require (ctx.empty ());
}








/*
 ********************************************************************************
 ****************** StructuredStreamEvents::ThrowUnRecognizedStartElt ***********
 ********************************************************************************
 */
void    _NoReturn_  StructuredStreamEvents::ThrowUnRecognizedStartElt (const StructuredStreamEvents::Name& name)
{
    Execution::DoThrow (BadFormatException (Characters::CString::Format (L"Unrecognized start tag '%s'", name.fLocalName.c_str ())));
}


