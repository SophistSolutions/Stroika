/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Time/Date.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "ObjectReader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::StructuredStreamEvents;
using namespace Stroika::Foundation::DataExchange::StructuredStreamEvents::ObjectReader;

using Time::Date;
using Time::DateTime;
using Time::Duration;
using Time::TimeOfDay;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 *********************** ObjectReaderIElementConsumer ***************************
 ********************************************************************************
 */
shared_ptr<IElementConsumer> IElementConsumer::HandleChildStart ([[maybe_unused]] const Name& name)
{
    return nullptr;
}

void IElementConsumer::HandleTextInside ([[maybe_unused]] const String& text)
{
}

void IElementConsumer::Activated ([[maybe_unused]] Context& r)
{
}

void IElementConsumer::Deactivating ()
{
}

/*
 ********************************************************************************
 **************************** Registry::SimpleReader_<> *************************
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
    *fValue_ = (fBuf_.str ().ToLowerCase () == "true"sv);
}

template <>
void Registry::SimpleReader_<float>::Deactivating ()
{
    (*fValue_) = Characters::FloatConversion::ToFloat<float> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<double>::Deactivating ()
{
    (*fValue_) = Characters::FloatConversion::ToFloat<double> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<long double>::Deactivating ()
{
    (*fValue_) = Characters::FloatConversion::ToFloat<long double> (fBuf_.str ());
}

template <>
void Registry::SimpleReader_<IO::Network::URI>::Deactivating ()
{
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall (*fValue_ = IO::Network::URI::Parse (fBuf_.str ()));
}

template <>
void Registry::SimpleReader_<Time::DateTime>::Deactivating ()
{
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall (*fValue_ = Time::DateTime::Parse (fBuf_.str (), Time::DateTime::kISO8601Format));
}

template <>
void Registry::SimpleReader_<Time::Duration>::Deactivating ()
{
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall (*fValue_ = Time::Duration{fBuf_.str ()});
}

/*
 ********************************************************************************
 ****************************** IgnoreNodeReader ********************************
 ********************************************************************************
 */
shared_ptr<IElementConsumer> IgnoreNodeReader::HandleChildStart ([[maybe_unused]] const StructuredStreamEvents::Name& name)
{
    return shared_from_this ();
}

/*
 ********************************************************************************
 *********************************** Context ************************************
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
    static const String kOneTabLevel_{"    "sv};
    return kOneTabLevel_.Repeat (static_cast<unsigned int> (fStack_.size ()));
}
#endif

/*
 ********************************************************************************
 ********************* Registry::IConsumerDelegateToContext *********************
 ********************************************************************************
 */
void IConsumerDelegateToContext::StartElement (const Name& name, const Mapping<Name, String>& attributes)
{
    AssertNotNull (fContext.GetTop ());
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext.fTraceThisReader) {
        DbgTrace ("{}Calling IConsumerDelegateToContext::HandleChildStart ('{}')..."_f, fContext.TraceLeader_ (), name.fLocalName);
    }
#endif
    shared_ptr<IElementConsumer> eltToPush = fContext.GetTop ()->HandleChildStart (name);
    AssertNotNull (eltToPush);
    fContext.Push (eltToPush);
    // and push each attribute, as if called with StartElement (as was done before Stroika v3.0d5)
    for (auto attr : attributes) {
        static const Mapping<Name, String> kEmpty_;
        StartElement (attr.fKey, kEmpty_);
        TextInsideElement (attr.fValue);
        EndElement (attr.fKey);
    }
}
void IConsumerDelegateToContext::EndElement ([[maybe_unused]] const StructuredStreamEvents::Name& name)
{
    AssertNotNull (fContext.GetTop ());
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext.fTraceThisReader) {
        DbgTrace ("{}Calling IConsumerDelegateToContext::EndElement ('{}')..."_f, fContext.TraceLeader_ (), name.fLocalName);
    }
#endif
    fContext.Pop ();
}
void IConsumerDelegateToContext::TextInsideElement (const String& text)
{
    AssertNotNull (fContext.GetTop ());
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    if (fContext.fTraceThisReader) {
        DbgTrace ("{}Calling IConsumerDelegateToContext::TextInsideElement ('{}')..."_f, fContext.TraceLeader_ (), text.LimitLength (50));
    }
#endif
    fContext.GetTop ()->HandleTextInside (text);
}

/*
 ********************************************************************************
 ****************************** Registry::ReadDownToReader **********************
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
[[noreturn]] void StructuredStreamEvents::ObjectReader::ThrowUnRecognizedStartElt (const StructuredStreamEvents::Name& name)
{
    Execution::Throw (BadFormatException{Characters::CString::Format (L"Unrecognized start tag '%s'", name.fLocalName.As<wstring> ().c_str ())});
}
