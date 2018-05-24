/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/FloatConversion.h"
#include "../../Characters/Format.h"
#include "../../Characters/String2Int.h"
#include "../../Characters/String_Constant.h"
#include "../../Debug/Trace.h"
#include "../../Time/Date.h"
#include "../../Time/DateRange.h"
#include "../../Time/DateTime.h"
#include "../../Time/DateTimeRange.h"
#include "../../Time/Duration.h"
#include "../../Time/DurationRange.h"

#include "../BadFormatException.h"

#include "ObjectReader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::StructuredStreamEvents;
using namespace Stroika::Foundation::DataExchange::StructuredStreamEvents::ObjectReader;

using Characters::String_Constant;
using Time::Date;
using Time::DateTime;
using Time::Duration;
using Time::TimeOfDay;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ***************** ObjectReaderIElementConsumer ***********************
 ********************************************************************************
 */
shared_ptr<IElementConsumer> IElementConsumer::HandleChildStart (const Name& name)
{
    return nullptr;
}

void IElementConsumer::HandleTextInside (const String& text)
{
}

void IElementConsumer::Activated (Context& r)
{
}

void IElementConsumer::Deactivating ()
{
}

/*
 ********************************************************************************
 ******************* Registry::SimpleReader_<> ********************
 ********************************************************************************
 */
template <>
void Registry::SimpleReader_<String>::Deactivating ()
{
    *fValue_ = fBuf_.str ();
}

template <>
void Registry::SimpleReader_<char>::Deactivating ()
{
    *fValue_ = Characters::String2Int<char> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<unsigned char>::Deactivating ()
{
    *fValue_ = Characters::String2Int<unsigned char> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<short>::Deactivating ()
{
    *fValue_ = Characters::String2Int<short> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<unsigned short>::Deactivating ()
{
    *fValue_ = Characters::String2Int<unsigned short> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<int>::Deactivating ()
{
    *fValue_ = Characters::String2Int<int> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<unsigned int>::Deactivating ()
{
    *fValue_ = Characters::String2Int<unsigned int> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<long int>::Deactivating ()
{
    *fValue_ = Characters::String2Int<long int> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<unsigned long int>::Deactivating ()
{
    *fValue_ = Characters::String2Int<unsigned long int> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<long long int>::Deactivating ()
{
    *fValue_ = Characters::String2Int<long long int> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<unsigned long long int>::Deactivating ()
{
    *fValue_ = Characters::String2Int<unsigned long long int> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<bool>::Deactivating ()
{
    *fValue_ = (fBuf_.str ().ToLowerCase () == L"true");
}

template <>
void Registry::SimpleReader_<float>::Deactivating ()
{
    (*fValue_) = Characters::String2Float<float> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<double>::Deactivating ()
{
    (*fValue_) = Characters::String2Float<double> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<long double>::Deactivating ()
{
    (*fValue_) = Characters::String2Float<long double> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<Time::DateTime>::Deactivating ()
{
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall (*fValue_ = Time::DateTime::Parse (fBuf_.str (), Time::DateTime::ParseFormat::eXML));
}

template <>
void Registry::SimpleReader_<Time::Duration>::Deactivating ()
{
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall (*fValue_ = Time::Duration (fBuf_.str ()));
}

/*
 ********************************************************************************
 ******************** IgnoreNodeReader ********************
 ********************************************************************************
 */
shared_ptr<IElementConsumer> IgnoreNodeReader::HandleChildStart (const StructuredStreamEvents::Name& name)
{
    return shared_from_this ();
}

/*
 ********************************************************************************
 ********************** Context ***************************
 ********************************************************************************
 */
Context::Context (const Registry& registry, const shared_ptr<IElementConsumer>& initialTop)
    : fObjectReaderRegistry_ (registry)
{
    Push (initialTop);
}

#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
String Context::TraceLeader_ () const
{
    static const String_Constant kOneTabLevel_{L"    "};
    return kOneTabLevel_.Repeat (static_cast<unsigned int> (fStack_.size ()));
}
#endif

/*
 ********************************************************************************
 ************ Registry::IConsumerDelegateToContext ******************
 ********************************************************************************
 */
void IConsumerDelegateToContext::StartElement (const StructuredStreamEvents::Name& name)
{
    AssertNotNull (fContext.GetTop ());
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext.fTraceThisReader) {
        DbgTrace (L"%sCalling IConsumerDelegateToContext::HandleChildStart ('%s')...", fContext.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
    }
#endif
    shared_ptr<IElementConsumer> eltToPush = fContext.GetTop ()->HandleChildStart (name);
    AssertNotNull (eltToPush);
    fContext.Push (eltToPush);
}
void IConsumerDelegateToContext::EndElement (const StructuredStreamEvents::Name& name)
{
    AssertNotNull (fContext.GetTop ());
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext.fTraceThisReader) {
        DbgTrace (L"%sCalling IConsumerDelegateToContext::EndElement ('%s')...", fContext.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
    }
#endif
    fContext.Pop ();
}
void IConsumerDelegateToContext::TextInsideElement (const String& text)
{
    AssertNotNull (fContext.GetTop ());
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext.fTraceThisReader) {
        DbgTrace (L"%sCalling IConsumerDelegateToContext::TextInsideElement ('%s')...", fContext.TraceLeader_ ().c_str (), text.LimitLength (50).c_str ());
    }
#endif
    fContext.GetTop ()->HandleTextInside (text);
}

/*
 ********************************************************************************
 ****************** Registry::ReadDownToReader **********************
 ********************************************************************************
 */
ReadDownToReader::ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader)
    : fReader2Delegate2_ (theUseReader)
{
    RequireNotNull (theUseReader);
}

ReadDownToReader::ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff)
    : fReader2Delegate2_ (theUseReader)
    , fTagToHandOff_ (tagToHandOff)
{
    RequireNotNull (theUseReader);
}

ReadDownToReader::ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& contextTag, const Name& tagToHandOff)
    : ReadDownToReader (make_shared<ReadDownToReader> (theUseReader, tagToHandOff), contextTag)
{
    RequireNotNull (theUseReader);
}

ReadDownToReader::ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& contextTag1, const Name& contextTag2, const Name& tagToHandOff)
    : ReadDownToReader (make_shared<ReadDownToReader> (theUseReader, contextTag2, tagToHandOff), contextTag1)
{
    RequireNotNull (theUseReader);
}

shared_ptr<IElementConsumer> ReadDownToReader::HandleChildStart (const Name& name)
{
    if (not fTagToHandOff_.has_value () or *fTagToHandOff_ == name) {
        return fReader2Delegate2_;
    }
    else {
        return shared_from_this ();
    }
}

/*
 ********************************************************************************
 ****** StructuredStreamEvents::ObjectReader::ThrowUnRecognizedStartElt *********
 ********************************************************************************
 */
[[noreturn]] void StructuredStreamEvents::ObjectReader::ThrowUnRecognizedStartElt (const StructuredStreamEvents::Name& name) {
    Execution::Throw (BadFormatException (Characters::CString::Format (L"Unrecognized start tag '%s'", name.fLocalName.c_str ())));
}
