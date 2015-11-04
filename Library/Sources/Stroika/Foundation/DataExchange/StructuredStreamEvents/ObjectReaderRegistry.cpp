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
 ******************** StructuredStreamEvents::BuiltinReader<String> *************
 ********************************************************************************
 */
BuiltinReader<String>::BuiltinReader (String* intoVal)
    : value_ (intoVal)
{
    RequireNotNull (intoVal);
    *intoVal = String ();
}

void    BuiltinReader<String>::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<String>::HandleTextInside (Context& r, const String& text)
{
    (*value_) += text;
}





/*
 ********************************************************************************
 *************** StructuredStreamEvents::BuiltinReader<int> *********************
 ********************************************************************************
 */
BuiltinReader<int>::BuiltinReader (int* intoVal)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
}

void    BuiltinReader<int>::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<int>::HandleTextInside (Context& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Int<int> (tmpVal_.As<wstring> ());
}






/*
 ********************************************************************************
 *************** StructuredStreamEvents::BuiltinReader<unsigned int> ************
 ********************************************************************************
 */
BuiltinReader<unsigned int>::BuiltinReader (unsigned int* intoVal)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
}

void    BuiltinReader<unsigned int>::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<unsigned int>::HandleTextInside (Context& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Int<int> (tmpVal_.As<wstring> ());
}







/*
 ********************************************************************************
 ******************* StructuredStreamEvents::BuiltinReader<bool> ****************
 ********************************************************************************
 */
BuiltinReader<bool>::BuiltinReader (bool* intoVal)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
}

void    BuiltinReader<bool>::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<bool>::HandleTextInside (Context& r, const String& text)
{
    tmpVal_ += text;
    String  tmp =   tmpVal_.Trim ().ToLowerCase ();
    (*value_) = (tmp == L"true");
}









/*
 ********************************************************************************
 *********** StructuredStreamEvents::BuiltinReader<float> ***********************
 ********************************************************************************
 */
BuiltinReader<float>::BuiltinReader (float* intoVal)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
}

void    BuiltinReader<float>::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<float>::HandleTextInside (Context& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = static_cast<float> (Characters::String2Float<float> (tmpVal_.As<wstring> ()));
}





/*
 ********************************************************************************
 **************** StructuredStreamEvents::BuiltinReader<double> *****************
 ********************************************************************************
 */
BuiltinReader<double>::BuiltinReader (double* intoVal)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
}

void    BuiltinReader<double>::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<double>::HandleTextInside (Context& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Float<double> (tmpVal_.As<wstring> ());
}







/*
 ********************************************************************************
 ************* StructuredStreamEvents::BuiltinReader<Time::DateTime> ************
 ********************************************************************************
 */
BuiltinReader<Time::DateTime>::BuiltinReader (Time::DateTime* intoVal)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
    *intoVal = Time::DateTime ();
}

void    BuiltinReader<Time::DateTime>::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<Time::DateTime>::HandleTextInside (Context& r, const String& text)
{
    tmpVal_ += text;
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall ((*value_) = Time::DateTime::Parse (tmpVal_.As<wstring> (), Time::DateTime::ParseFormat::eXML));
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

void    IgnoreNodeReader::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
{
    Require (fDepth_ >= 0);
    fDepth_++;
}

void    IgnoreNodeReader::HandleTextInside (Context& r, const String& text)
{
    // Ignore text
}

bool    IgnoreNodeReader::HandleEndTag (Context& r)
{
    Require (fDepth_ >= 0);
    --fDepth_;
    if (fDepth_ < 0) {
        r.Pop ();
    }
    return false;
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
    fContext_.GetTop ()->HandleChildStart (fContext_, name);
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
        virtual void    HandleChildStart (Context& r, const StructuredStreamEvents::Name& name) override
        {
            if (not fAnyDocElt) {
                if (name.fLocalName != fDocEltName or name.fNamespaceURI.Value () != fDocEltURI) {
                    ThrowUnRecognizedStartElt (name);
                }
            }
            r.Push (fDocEltBuilder);
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


