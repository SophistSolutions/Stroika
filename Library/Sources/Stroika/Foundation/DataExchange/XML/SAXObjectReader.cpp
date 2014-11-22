/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/CString/Utilities.h"
#include    "../../Characters/Format.h"
#include    "../../Characters/String2Float.h"
#include    "../../Characters/String2Int.h"
#include    "../BadFormatException.h"

#include    "SAXObjectReader.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::XML;







/*
 ********************************************************************************
 ****************************** XML::SAXObjectReader ****************************
 ********************************************************************************
 */
#if     qDefaultTracingOn
wstring SAXObjectReader::TraceLeader_ () const
{
    wstring l;
    l.reserve (fStack_.size ());
    for (size_t i = 0; i < fStack_.size (); ++i) {
        l += L" ";
    }
    return l;
}
#endif
class   SAXObjectReader::MyCallback_ : public SAXCallbackInterface {
public:
    MyCallback_ (SAXObjectReader& r)
        : fSAXObjectReader_ (r)
    {
    }
private:
    SAXObjectReader&    fSAXObjectReader_;
public:
    virtual void    StartDocument () override
    {
    }
    virtual void    EndDocument () override
    {
    }
    virtual void    StartElement (const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs) override
    {
        AssertNotNull (fSAXObjectReader_.GetTop ());
#if     qDefaultTracingOn
        if (fSAXObjectReader_.fTraceThisReader) {
            DbgTrace (L"%sCalling HandleChildStart ('%s',...)...", fSAXObjectReader_.TraceLeader_ ().c_str (), localName.c_str ());
        }
#endif
        fSAXObjectReader_.GetTop ()->HandleChildStart (fSAXObjectReader_, uri, localName, qname, attrs);
    }
    virtual void    EndElement (const String& uri, const String& localName, const String& qname) override
    {
        AssertNotNull (fSAXObjectReader_.GetTop ());
#if     qDefaultTracingOn
        if (fSAXObjectReader_.fTraceThisReader) {
            DbgTrace (L"%sCalling EndElement ('%s',...)...", fSAXObjectReader_.TraceLeader_ ().c_str (), localName.c_str ());
        }
#endif
        fSAXObjectReader_.GetTop ()->HandleEndTag (fSAXObjectReader_);
    }
    virtual void    CharactersInsideElement (const String& text) override
    {
        AssertNotNull (fSAXObjectReader_.GetTop ());
#if     qDefaultTracingOn
        if (fSAXObjectReader_.fTraceThisReader) {
            DbgTrace (L"%sCalling CharactersInsideElement ('%s',...)...", fSAXObjectReader_.TraceLeader_ ().c_str (), text.c_str () );
        }
#endif
        fSAXObjectReader_.GetTop ()->HandleTextInside (fSAXObjectReader_, text);
    }
};

namespace   {
    struct DocumentReader_ : public SAXObjectReader::ObjectBase {
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
        virtual void    HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs) override
        {
            if (not fAnyDocElt) {
                if (localName != fDocEltName or uri != fDocEltURI) {
                    ThrowUnRecognizedStartElt (uri, localName);
                }
            }
            r.Push (fDocEltBuilder);
        }
        virtual void    HandleTextInside (SAXObjectReader& r, const String& text)  override
        {
            // OK so long as text is whitespace - or comment. Probably should check/assert, but KISS..., and count on validation to
            // assure input is valid
            Assert (text.IsWhitespace ());
        }
        virtual void    HandleEndTag (SAXObjectReader& r) override
        {
            r.Pop ();
        }
    };
}
void    SAXObjectReader::Run (const shared_ptr<ObjectBase>& docEltBuilder, const Streams::BinaryInputStream& in)
{
    RequireNotNull (docEltBuilder);
    Require (fStack_.size () == 0);

    Push (shared_ptr<ObjectBase> (new DocumentReader_ (docEltBuilder)));

    MyCallback_ cb (*this);
    SAXParse (in, cb);

    Pop (); // the docuemnt reader we just added

    Ensure (fStack_.size () == 0);
}

void    SAXObjectReader::Run (const shared_ptr<ObjectBase>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, const Streams::BinaryInputStream& in)
{
    RequireNotNull (docEltBuilder);
    Require (fStack_.size () == 0);

    Push (shared_ptr<ObjectBase> (new DocumentReader_ (docEltBuilder, docEltUri, docEltLocalName)));

    MyCallback_ cb (*this);
    SAXParse (in, cb);

    Pop (); // the docuemnt reader we just added

    Ensure (fStack_.size () == 0);
}







/*
 ********************************************************************************
 ********************* XML::SAXObjectReader::ObjectBase *************************
 ********************************************************************************
 */
SAXObjectReader::ObjectBase::~ObjectBase ()
{
}






/*
 ********************************************************************************
 ************************* XML::BuiltinReader<String> ***************************
 ********************************************************************************
 */
BuiltinReader<String>::BuiltinReader (String* intoVal, const Mapping<String, VariantValue>& attrs)
    : value_ (intoVal)
{
    RequireNotNull (intoVal);
    *intoVal = String ();
}

void    BuiltinReader<String>::HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs)
{
    ThrowUnRecognizedStartElt (uri, localName);
}

void    BuiltinReader<String>::HandleTextInside (SAXObjectReader& r, const String& text)
{
    (*value_) += text;
}

void    BuiltinReader<String>::HandleEndTag (SAXObjectReader& r)
{
    r.Pop ();
}








/*
 ********************************************************************************
 **************************** XML::BuiltinReader<int> ***************************
 ********************************************************************************
 */
BuiltinReader<int>::BuiltinReader (int* intoVal, const Mapping<String, VariantValue>& attrs)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
}

void    BuiltinReader<int>::HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs)
{
    ThrowUnRecognizedStartElt (uri, localName);
}

void    BuiltinReader<int>::HandleTextInside (SAXObjectReader& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Int<int> (tmpVal_.As<wstring> ());
}

void    BuiltinReader<int>::HandleEndTag (SAXObjectReader& r)
{
    r.Pop ();
}






/*
 ********************************************************************************
 ******************* XML::BuiltinReader<unsigned int> ***************************
 ********************************************************************************
 */
BuiltinReader<unsigned int>::BuiltinReader (unsigned int* intoVal, const Mapping<String, VariantValue>& attrs)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
}

void    BuiltinReader<unsigned int>::HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs)
{
    ThrowUnRecognizedStartElt (uri, localName);
}

void    BuiltinReader<unsigned int>::HandleTextInside (SAXObjectReader& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Int<int> (tmpVal_.As<wstring> ());
}

void    BuiltinReader<unsigned int>::HandleEndTag (SAXObjectReader& r)
{
    r.Pop ();
}








/*
 ********************************************************************************
 *************************** XML::BuiltinReader<bool> ***************************
 ********************************************************************************
 */
BuiltinReader<bool>::BuiltinReader (bool* intoVal, const Mapping<String, VariantValue>& attrs)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
}

void    BuiltinReader<bool>::HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs)
{
    ThrowUnRecognizedStartElt (uri, localName);
}

void    BuiltinReader<bool>::HandleTextInside (SAXObjectReader& r, const String& text)
{
    tmpVal_ += text;
    String  tmp =   tmpVal_.Trim ().ToLowerCase ();
    (*value_) = (tmp == L"true");
}

void    BuiltinReader<bool>::HandleEndTag (SAXObjectReader& r)
{
    r.Pop ();
}










/*
 ********************************************************************************
 ************************** XML::BuiltinReader<float> ***************************
 ********************************************************************************
 */
BuiltinReader<float>::BuiltinReader (float* intoVal, const Mapping<String, VariantValue>& attrs)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
}

void    BuiltinReader<float>::HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs)
{
    ThrowUnRecognizedStartElt (uri, localName);
}

void    BuiltinReader<float>::HandleTextInside (SAXObjectReader& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = static_cast<float> (Characters::String2Float<float> (tmpVal_.As<wstring> ()));
}

void    BuiltinReader<float>::HandleEndTag (SAXObjectReader& r)
{
    r.Pop ();
}





/*
 ********************************************************************************
 ************************** XML::BuiltinReader<double> **************************
 ********************************************************************************
 */
BuiltinReader<double>::BuiltinReader (double* intoVal, const Mapping<String, VariantValue>& attrs)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
}

void    BuiltinReader<double>::HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs)
{
    ThrowUnRecognizedStartElt (uri, localName);
}

void    BuiltinReader<double>::HandleTextInside (SAXObjectReader& r, const String& text)
{
    tmpVal_ += text;
    (*value_) = Characters::String2Float<double> (tmpVal_.As<wstring> ());
}

void    BuiltinReader<double>::HandleEndTag (SAXObjectReader& r)
{
    r.Pop ();
}






/*
 ********************************************************************************
 ********************** XML::BuiltinReader<Time::DateTime> **********************
 ********************************************************************************
 */
BuiltinReader<Time::DateTime>::BuiltinReader (Time::DateTime* intoVal, const Mapping<String, VariantValue>& attrs)
    : tmpVal_ ()
    , value_ (intoVal)
{
    RequireNotNull (intoVal);
    *intoVal = Time::DateTime ();
}

void    BuiltinReader<Time::DateTime>::HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs)
{
    ThrowUnRecognizedStartElt (uri, localName);
}

void    BuiltinReader<Time::DateTime>::HandleTextInside (SAXObjectReader& r, const String& text)
{
    tmpVal_ += text;
    // not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
    IgnoreExceptionsForCall ((*value_) = Time::DateTime::Parse (tmpVal_.As<wstring> (), Time::DateTime::ParseFormat::eXML));
}

void    BuiltinReader<Time::DateTime>::HandleEndTag (SAXObjectReader& r)
{
    r.Pop ();
}






/*
 ********************************************************************************
 ******************************* XML::IgnoreNodeReader **************************
 ********************************************************************************
 */
IgnoreNodeReader::IgnoreNodeReader ()
    : fDepth_ (0)
{
}

void    IgnoreNodeReader::HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs)
{
    Require (fDepth_ >= 0);
    fDepth_++;
}

void    IgnoreNodeReader::HandleTextInside (SAXObjectReader& r, const String& text)
{
    // Ignore text
}

void    IgnoreNodeReader::HandleEndTag (SAXObjectReader& r)
{
    Require (fDepth_ >= 0);
    --fDepth_;
    if (fDepth_ < 0) {
        r.Pop ();
    }
}





/*
 ********************************************************************************
 ************************** XML::ThrowUnRecognizedStartElt **********************
 ********************************************************************************
 */
void    XML::ThrowUnRecognizedStartElt (const String& uri, const String& localName)
{
    Execution::DoThrow (BadFormatException (Characters::CString::Format (L"Unrecognized start tag '%s'", localName.c_str ())));
}


