/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/FloatConversion.h"
#include    "../../Characters/Format.h"
#include    "../../Characters/String2Int.h"
#include    "../../Containers/MultiSet.h"
#include    "../../Debug/Trace.h"
#include    "../../Time/Date.h"
#include    "../../Time/DateRange.h"
#include    "../../Time/DateTime.h"
#include    "../../Time/DateTimeRange.h"
#include    "../../Time/Duration.h"
#include    "../../Time/DurationRange.h"

#include    "ObjectReaderRegistry.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::StructuredStreamEvents;

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

void    BuiltinReader<String>::HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<String>::HandleTextInside (ObjectReader::Context& r, const String& text)
{
    (*value_) += text;
}

void    BuiltinReader<String>::HandleEndTag (ObjectReader::Context& r)
{
    r.Pop ();
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

void    BuiltinReader<int>::HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<int>::HandleTextInside (ObjectReader::Context& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Int<int> (tmpVal_.As<wstring> ());
}

void    BuiltinReader<int>::HandleEndTag (ObjectReader::Context& r)
{
    r.Pop ();
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

void    BuiltinReader<unsigned int>::HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<unsigned int>::HandleTextInside (ObjectReader::Context& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Int<int> (tmpVal_.As<wstring> ());
}

void    BuiltinReader<unsigned int>::HandleEndTag (ObjectReader::Context& r)
{
    r.Pop ();
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

void    BuiltinReader<bool>::HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<bool>::HandleTextInside (ObjectReader::Context& r, const String& text)
{
    tmpVal_ += text;
    String  tmp =   tmpVal_.Trim ().ToLowerCase ();
    (*value_) = (tmp == L"true");
}

void    BuiltinReader<bool>::HandleEndTag (ObjectReader::Context& r)
{
    r.Pop ();
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

void    BuiltinReader<float>::HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<float>::HandleTextInside (ObjectReader::Context& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = static_cast<float> (Characters::String2Float<float> (tmpVal_.As<wstring> ()));
}

void    BuiltinReader<float>::HandleEndTag (ObjectReader::Context& r)
{
    r.Pop ();
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

void    BuiltinReader<double>::HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<double>::HandleTextInside (ObjectReader::Context& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Float<double> (tmpVal_.As<wstring> ());
}

void    BuiltinReader<double>::HandleEndTag (ObjectReader::Context& r)
{
    r.Pop ();
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

void    BuiltinReader<Time::DateTime>::HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<Time::DateTime>::HandleTextInside (ObjectReader::Context& r, const String& text)
{
    tmpVal_ += text;
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall ((*value_) = Time::DateTime::Parse (tmpVal_.As<wstring> (), Time::DateTime::ParseFormat::eXML));
}

void    BuiltinReader<Time::DateTime>::HandleEndTag (ObjectReader::Context& r)
{
    r.Pop ();
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

void    IgnoreNodeReader::HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name)
{
    Require (fDepth_ >= 0);
    fDepth_++;
}

void    IgnoreNodeReader::HandleTextInside (ObjectReader::Context& r, const String& text)
{
    // Ignore text
}

void    IgnoreNodeReader::HandleEndTag (ObjectReader::Context& r)
{
    Require (fDepth_ >= 0);
    --fDepth_;
    if (fDepth_ < 0) {
        r.Pop ();
    }
}



