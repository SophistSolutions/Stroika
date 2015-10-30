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
 ***************** StructuredStreamEvents::ObjectReader *************************
 ********************************************************************************
 */
#if     qDefaultTracingOn
wstring ObjectReader::TraceLeader_ () const
{
    wstring l;
    l.reserve (fStack_.size ());
    for (size_t i = 0; i < fStack_.size (); ++i) {
        l += L" ";
    }
    return l;
}
#endif
class   ObjectReader::MyCallback_ : public StructuredStreamEvents::IConsumer {
public:
    MyCallback_ (ObjectReader& r)
        : fSAXObjectReader_ (r)
    {
    }
private:
    ObjectReader&    fSAXObjectReader_;
public:
    virtual void    StartDocument () override
    {
    }
    virtual void    EndDocument () override
    {
    }
    virtual void    StartElement (const StructuredStreamEvents::Name& name) override
    {
        AssertNotNull (fSAXObjectReader_.GetTop ());
#if     qDefaultTracingOn
        if (fSAXObjectReader_.fTraceThisReader) {
            DbgTrace (L"%sCalling HandleChildStart ('%s',...)...", fSAXObjectReader_.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
        }
#endif
        fSAXObjectReader_.GetTop ()->HandleChildStart (fSAXObjectReader_, name);
    }
    virtual void    EndElement (const StructuredStreamEvents::Name& name) override
    {
        AssertNotNull (fSAXObjectReader_.GetTop ());
#if     qDefaultTracingOn
        if (fSAXObjectReader_.fTraceThisReader) {
            DbgTrace (L"%sCalling EndElement ('%s',...)...", fSAXObjectReader_.TraceLeader_ ().c_str (), name.fLocalName.c_str ());
        }
#endif
        fSAXObjectReader_.GetTop ()->HandleEndTag (fSAXObjectReader_);
    }
    virtual void    TextInsideElement (const String& text) override
    {
        AssertNotNull (fSAXObjectReader_.GetTop ());
#if     qDefaultTracingOn
        if (fSAXObjectReader_.fTraceThisReader) {
            DbgTrace (L"%sCalling TextInsideElement ('%s',...)...", fSAXObjectReader_.TraceLeader_ ().c_str (), text.c_str () );
        }
#endif
        fSAXObjectReader_.GetTop ()->HandleTextInside (fSAXObjectReader_, text);
    }
};

namespace   {
    struct DocumentReader_ : public ObjectReader::ObjectBase {
        shared_ptr<ObjectBase>          fDocEltBuilder;
        bool                            fAnyDocElt;
        String                          fDocEltURI;
        String                          fDocEltName;
        DocumentReader_ (const shared_ptr<ObjectBase>& docEltBuilder)
            : fDocEltBuilder (docEltBuilder)
            , fAnyDocElt (true)
            , fDocEltURI ()
            , fDocEltName ()
        {
        }
        DocumentReader_ (const shared_ptr<ObjectBase>& docEltBuilder, const String& checkURI, const String& checkDocEltName)
            : fDocEltBuilder (docEltBuilder)
            , fAnyDocElt (false)
            , fDocEltURI (checkURI)
            , fDocEltName (checkDocEltName)
        {
        }
        virtual void    HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name) override
        {
            if (not fAnyDocElt) {
                if (name.fLocalName != fDocEltName or name.fNamespaceURI.Value () != fDocEltURI) {
                    ThrowUnRecognizedStartElt (name);
                }
            }
            r.Push (fDocEltBuilder);
        }
        virtual void    HandleTextInside (ObjectReader& r, const String& text)  override
        {
            // OK so long as text is whitespace - or comment. Probably should check/assert, but KISS..., and count on validation to
            // assure input is valid
            Assert (text.IsWhitespace ());
        }
        virtual void    HandleEndTag (ObjectReader& r) override
        {
            r.Pop ();
        }
    };
}
void    ObjectReader::Run (const shared_ptr<ObjectBase>& docEltBuilder, const Streams::InputStream<Byte>& in)
{
    RequireNotNull (docEltBuilder);
    Require (fStack_.size () == 0);

    Push (shared_ptr<ObjectBase> (new DocumentReader_ (docEltBuilder)));

    MyCallback_ cb (*this);
    XML::SAXParse (in, cb);

    Pop (); // the docuemnt reader we just added

    Ensure (fStack_.size () == 0);
}

void    ObjectReader::Run (const shared_ptr<ObjectBase>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, const Streams::InputStream<Byte>& in)
{
    RequireNotNull (docEltBuilder);
    Require (fStack_.size () == 0);

    Push (shared_ptr<ObjectBase> (new DocumentReader_ (docEltBuilder, docEltUri, docEltLocalName)));

    MyCallback_ cb (*this);
    XML::SAXParse (in, cb);

    Pop (); // the docuemnt reader we just added

    Ensure (fStack_.size () == 0);
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

void    BuiltinReader<String>::HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<String>::HandleTextInside (ObjectReader& r, const String& text)
{
    (*value_) += text;
}

void    BuiltinReader<String>::HandleEndTag (ObjectReader& r)
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

void    BuiltinReader<int>::HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<int>::HandleTextInside (ObjectReader& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Int<int> (tmpVal_.As<wstring> ());
}

void    BuiltinReader<int>::HandleEndTag (ObjectReader& r)
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

void    BuiltinReader<unsigned int>::HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<unsigned int>::HandleTextInside (ObjectReader& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Int<int> (tmpVal_.As<wstring> ());
}

void    BuiltinReader<unsigned int>::HandleEndTag (ObjectReader& r)
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

void    BuiltinReader<bool>::HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<bool>::HandleTextInside (ObjectReader& r, const String& text)
{
    tmpVal_ += text;
    String  tmp =   tmpVal_.Trim ().ToLowerCase ();
    (*value_) = (tmp == L"true");
}

void    BuiltinReader<bool>::HandleEndTag (ObjectReader& r)
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

void    BuiltinReader<float>::HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<float>::HandleTextInside (ObjectReader& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = static_cast<float> (Characters::String2Float<float> (tmpVal_.As<wstring> ()));
}

void    BuiltinReader<float>::HandleEndTag (ObjectReader& r)
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

void    BuiltinReader<double>::HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<double>::HandleTextInside (ObjectReader& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Float<double> (tmpVal_.As<wstring> ());
}

void    BuiltinReader<double>::HandleEndTag (ObjectReader& r)
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

void    BuiltinReader<Time::DateTime>::HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name)
{
    ThrowUnRecognizedStartElt (name);
}

void    BuiltinReader<Time::DateTime>::HandleTextInside (ObjectReader& r, const String& text)
{
    tmpVal_ += text;
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall ((*value_) = Time::DateTime::Parse (tmpVal_.As<wstring> (), Time::DateTime::ParseFormat::eXML));
}

void    BuiltinReader<Time::DateTime>::HandleEndTag (ObjectReader& r)
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

void    IgnoreNodeReader::HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name)
{
    Require (fDepth_ >= 0);
    fDepth_++;
}

void    IgnoreNodeReader::HandleTextInside (ObjectReader& r, const String& text)
{
    // Ignore text
}

void    IgnoreNodeReader::HandleEndTag (ObjectReader& r)
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


