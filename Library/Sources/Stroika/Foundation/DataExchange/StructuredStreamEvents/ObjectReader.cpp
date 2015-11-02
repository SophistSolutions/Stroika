/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/CString/Utilities.h"
#include    "../../Characters/FloatConversion.h"
#include    "../../Characters/Format.h"
#include    "../../Characters/String2Int.h"
#include    "../BadFormatException.h"

#include    "ObjectReader.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::StructuredStreamEvents;


using   Memory::Byte;



/*
 ********************************************************************************
 ************ StructuredStreamEvents::ObjectReader::Context *********************
 ********************************************************************************
 */
#if     qDefaultTracingOn
wstring ObjectReader::Context::TraceLeader_ () const
{
    wstring l;
    l.reserve (fStack_.size ());
    for (size_t i = 0; i < fStack_.size (); ++i) {
        l += L" ";
    }
    return l;
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
#if     qDefaultTracingOn
    if (fContext_.fTraceThisReader) {
        DbgTrace (L"%sCalling HandleChildStart ('%s',...)...", fContext_.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
    }
#endif
    fContext_.GetTop ()->HandleChildStart (fContext_, name);
}
void    ObjectReader::IConsumerDelegateToContext::EndElement (const StructuredStreamEvents::Name& name)
{
    AssertNotNull (fContext_.GetTop ());
#if     qDefaultTracingOn
    if (fContext_.fTraceThisReader) {
        DbgTrace (L"%sCalling EndElement ('%s',...)...", fContext_.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
    }
#endif
    fContext_.GetTop ()->HandleEndTag (fContext_);
}
void    ObjectReader::IConsumerDelegateToContext::TextInsideElement (const String& text)
{
    AssertNotNull (fContext_.GetTop ());
#if     qDefaultTracingOn
    if (fContext_.fTraceThisReader) {
        DbgTrace (L"%sCalling TextInsideElement ('%s',...)...", fContext_.TraceLeader_ ().c_str (), text.c_str () );
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
void    ObjectReader::Run (const shared_ptr<IContextReader>& docEltBuilder, const Streams::InputStream<Byte>& in)
{
    // @todo see https://stroika.atlassian.net/browse/STK-284
    RequireNotNull (docEltBuilder);
    Context ctx;
    Require (ctx.fStack_.size () == 0);

    ctx.Push (make_shared<DocumentReader_> (docEltBuilder));

    IConsumerDelegateToContext cb (ctx);
    XML::SAXParse (in, cb);

    ctx.Pop (); // the docuemnt reader we just added

    Ensure (ctx.fStack_.size () == 0);
}

void    ObjectReader::Run (const shared_ptr<IContextReader>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, const Streams::InputStream<Byte>& in)
{
    // @todo see https://stroika.atlassian.net/browse/STK-284
    RequireNotNull (docEltBuilder);
    Context ctx;
    Require (ctx.fStack_.size () == 0);

    ctx.Push (make_shared<DocumentReader_> (docEltBuilder, docEltUri, docEltLocalName));

    IConsumerDelegateToContext cb (ctx);
    XML::SAXParse (in, cb);

    ctx.Pop (); // the docuemnt reader we just added

    Ensure (ctx.fStack_.size () == 0);
}








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





/*
 ********************************************************************************
 ****************** StructuredStreamEvents::ThrowUnRecognizedStartElt ***********
 ********************************************************************************
 */
void    StructuredStreamEvents::ThrowUnRecognizedStartElt (const StructuredStreamEvents::Name& name)
{
    Execution::DoThrow (BadFormatException (Characters::CString::Format (L"Unrecognized start tag '%s'", name.fLocalName.c_str ())));
}


