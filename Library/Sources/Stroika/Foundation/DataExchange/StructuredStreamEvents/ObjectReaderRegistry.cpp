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
#include    "../XML/SAXReader.h"

#include    "ObjectReaderRegistry.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::StructuredStreamEvents;

using   Characters::String_Constant;
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
 ******************** StructuredStreamEvents::SimpleReader_<> ********************
 ********************************************************************************
 */
template <>
void   ObjectReaderRegistry::SimpleReader_<String>::Deactivating (Context& r)
{
    *fValue_ = fBuf_.str ();
}

template <>
void   ObjectReaderRegistry::SimpleReader_<int>::Deactivating (Context& r)
{
    *fValue_ = Characters::String2Int<int> (fBuf_.str ());
}

template <>
void   ObjectReaderRegistry::SimpleReader_<unsigned int>::Deactivating (Context& r)
{
    //@ todo fix
    *fValue_ = Characters::String2Int<int> (fBuf_.str ());
}

template <>
void   ObjectReaderRegistry::SimpleReader_<bool>::Deactivating (Context& r)
{
    *fValue_ = (fBuf_.str ().ToLowerCase () == L"true");
}

template <>
void   ObjectReaderRegistry::SimpleReader_<float>::Deactivating (Context& r)
{
    (*fValue_) = Characters::String2Float<float> (fBuf_.str ());
}

template <>
void   ObjectReaderRegistry::SimpleReader_<double>::Deactivating (Context& r)
{
    (*fValue_) = Characters::String2Float<double> (fBuf_.str ());
}

template <>
void   ObjectReaderRegistry::SimpleReader_<Time::DateTime>::Deactivating (Context& r)
{
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall (*fValue_ = Time::DateTime::Parse (fBuf_.str (), Time::DateTime::ParseFormat::eXML));
}







/*
 ********************************************************************************
 ******************** ObjectReaderRegistry::IgnoreNodeReader ********************
 ********************************************************************************
 */
shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::IgnoreNodeReader::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
{
    return shared_from_this ();
}








/*
 ********************************************************************************
 ********************** ObjectReaderRegistry::Context ***************************
 ********************************************************************************
 */
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
String ObjectReaderRegistry::Context::TraceLeader_ () const
{
    static  const   String_Constant     kOneTabLevel_ { L"    " };
    return kOneTabLevel_.Repeat (fStack_.size ());
}
#endif




/*
 ********************************************************************************
 ************ ObjectReaderRegistry::IConsumerDelegateToContext ******************
 ********************************************************************************
 */
ObjectReaderRegistry::IConsumerDelegateToContext::IConsumerDelegateToContext (Context& r)
    : fContext_ (r)
{
}

void    ObjectReaderRegistry::IConsumerDelegateToContext::StartElement (const StructuredStreamEvents::Name& name)
{
    AssertNotNull (fContext_.GetTop ());
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext_.fTraceThisReader) {
        DbgTrace (L"%sCalling IConsumerDelegateToContext::HandleChildStart ('%s')...", fContext_.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
    }
#endif
    shared_ptr<IElementConsumer> eltToPush = fContext_.GetTop ()->HandleChildStart (fContext_, name);
    AssertNotNull (eltToPush);
    fContext_.Push (eltToPush);
}
void    ObjectReaderRegistry::IConsumerDelegateToContext::EndElement (const StructuredStreamEvents::Name& name)
{
    AssertNotNull (fContext_.GetTop ());
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext_.fTraceThisReader) {
        DbgTrace (L"%sCalling IConsumerDelegateToContext::EndElement ('%s')...", fContext_.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
    }
#endif
    fContext_.Pop ();
}
void    ObjectReaderRegistry::IConsumerDelegateToContext::TextInsideElement (const String& text)
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
void    StructuredStreamEvents::Run (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<ObjectReaderRegistry::IElementConsumer>& docEltBuilder, const Streams::InputStream<Byte>& in)
{
    // @todo see https://stroika.atlassian.net/browse/STK-284
    RequireNotNull (docEltBuilder);
    ObjectReaderRegistry::Context ctx { objectReaderRegistry };
    Require (ctx.empty ());

    ctx.Push (make_shared<ObjectReaderRegistry::ReadDownToReader> (docEltBuilder));

    ObjectReaderRegistry::IConsumerDelegateToContext cb (ctx);
    XML::SAXParse (in, cb);

    ctx.Pop (); // the docuemnt reader we just added

    Require (ctx.empty ());
}

void    StructuredStreamEvents::Run (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<ObjectReaderRegistry::IElementConsumer>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, const Streams::InputStream<Byte>& in)
{
    // @todo see https://stroika.atlassian.net/browse/STK-284
    RequireNotNull (docEltBuilder);
    ObjectReaderRegistry::Context ctx { objectReaderRegistry };
    Require (ctx.empty ());

    ctx.Push (make_shared<ObjectReaderRegistry::ReadDownToReader> (docEltBuilder, Name {docEltLocalName, docEltUri}));

    ObjectReaderRegistry::IConsumerDelegateToContext cb (ctx);
    XML::SAXParse (in, cb);

    ctx.Pop (); // the docuemnt reader we just added

    Require (ctx.empty ());
}








/*
 ********************************************************************************
 ****************** ObjectReaderRegistry::ReadDownToReader **********************
 ********************************************************************************
 */
ObjectReaderRegistry::ReadDownToReader::ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader)
    : fReader2Delegate2_ (theUseReader)
{
    RequireNotNull (theUseReader);
}

ObjectReaderRegistry::ReadDownToReader::ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff)
    : fReader2Delegate2_ (theUseReader)
    , fTagToHandOff_ (tagToHandOff)
{
    RequireNotNull (theUseReader);
}

shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::ReadDownToReader::HandleChildStart (Context& r, const Name& name)
{
    if (fTagToHandOff_.IsMissing () or * fTagToHandOff_ == name) {
        return fReader2Delegate2_;
    }
    else {
        return shared_from_this ();
    }
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







