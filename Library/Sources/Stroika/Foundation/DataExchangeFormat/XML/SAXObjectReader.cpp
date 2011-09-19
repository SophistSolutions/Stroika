/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	"../../Characters/StringUtils.h"
#include	"../BadFormatException.h"

#include	"SAXObjectReader.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::DataExchangeFormat;
using	namespace	Stroika::Foundation::DataExchangeFormat::XML;







/*
 ********************************************************************************
 ****************************** XML::SAXObjectReader ****************************
 ********************************************************************************
 */
#if		qDefaultTracingOn
wstring	SAXObjectReader::TraceLeader_ () const
{
	wstring	l;
	for (size_t i = 0; i < fStack_.size (); ++i) {
		l += L" ";
	}
	return l;
}
#endif
class	SAXObjectReader::MyCallback_ : public SAXCallbackInterface {
	public:
		MyCallback_ (SAXObjectReader& r)
			: fSAXObjectReader_ (r)
			{
			}
	private:
		SAXObjectReader&	fSAXObjectReader_;
	public:
		virtual	void	StartDocument () override
			{
			}
		virtual	void	EndDocument () override
			{
			}
		virtual	void	StartElement (const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
			{
				AssertNotNull (fSAXObjectReader_.GetTop ());
				#if		qDefaultTracingOn
					if (fSAXObjectReader_.fTraceThisReader) {
						DbgTrace (L"%sCalling HandleChildStart ('%s',...)...", fSAXObjectReader_.TraceLeader_ ().c_str (), localName.As<wstring> ().c_str ());
					}
				#endif
				fSAXObjectReader_.GetTop ()->HandleChildStart (fSAXObjectReader_, uri, localName, qname, attrs);
			}
		virtual	void	EndElement (const String& uri, const String& localName, const String& qname) override
			{
				AssertNotNull (fSAXObjectReader_.GetTop ());
				#if		qDefaultTracingOn
					if (fSAXObjectReader_.fTraceThisReader) {
						DbgTrace (L"%sCalling EndElement ('%s',...)...", fSAXObjectReader_.TraceLeader_ ().c_str (), localName.As<wstring> ().c_str ());
					}
				#endif
				fSAXObjectReader_.GetTop ()->HandleEndTag (fSAXObjectReader_);
			}
		virtual	void	CharactersInsideElement (const String& text) override
			{
				AssertNotNull (fSAXObjectReader_.GetTop ());
				#if		qDefaultTracingOn
					if (fSAXObjectReader_.fTraceThisReader) {
						DbgTrace (L"%sCalling CharactersInsideElement ('%s',...)...", fSAXObjectReader_.TraceLeader_ ().c_str (), text.As<wstring> ().c_str () );
					}
				#endif
				fSAXObjectReader_.GetTop ()->HandleTextInside (fSAXObjectReader_, text);
			}
};

namespace	{
	struct DocumentReader_ : public SAXObjectReader::ObjectBase {
		Memory::SharedPtr<ObjectBase>	fDocEltBuilder;
		bool							fAnyDocElt;
		String							fDocEltURI;
		String							fDocEltName;
		DocumentReader_ (const Memory::SharedPtr<ObjectBase>& docEltBuilder)
			: fDocEltBuilder (docEltBuilder)
			, fAnyDocElt (true)
			, fDocEltURI ()
			, fDocEltName ()
			{
			}
		DocumentReader_ (const Memory::SharedPtr<ObjectBase>& docEltBuilder, const String& checkURI, const String& checkDocEltName)
			: fDocEltBuilder (docEltBuilder)
			, fAnyDocElt (false)
			, fDocEltURI (checkURI)
			, fDocEltName (checkDocEltName)
			{
			}
		virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
			{
				if (not fAnyDocElt) {
					if (localName != fDocEltName or uri != fDocEltURI) {
						ThrowUnRecognizedStartElt (uri, localName);
					}
				}
				r.Push (fDocEltBuilder);
			}
		virtual	void	HandleTextInside (SAXObjectReader &r, const String& text)  override
			{
				// OK so long as text is whitespace - or comment. Probably should check/assert, but KISS..., and count on validation to
				// assure input is valid
				Assert (text.IsWhitespace ());
			}
		virtual	void	HandleEndTag (SAXObjectReader &r) override
			{
				r.Pop ();
			}
	};
}
void	SAXObjectReader::Run (const Memory::SharedPtr<ObjectBase>& docEltBuilder, istream& in)
{
	RequireNotNull (docEltBuilder);
	Require (fStack_.size () == 0);

	Push (Memory::SharedPtr<ObjectBase> (new DocumentReader_ (docEltBuilder)));

	MyCallback_ cb (*this);
	SAXParse (in, cb);

	Pop ();	// the docuemnt reader we just added

	Ensure (fStack_.size () == 0);
}

void	SAXObjectReader::Run (const Memory::SharedPtr<ObjectBase>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, istream& in)
{
	RequireNotNull (docEltBuilder);
	Require (fStack_.size () == 0);

	Push (Memory::SharedPtr<ObjectBase> (new DocumentReader_ (docEltBuilder, docEltUri, docEltLocalName)));

	MyCallback_ cb (*this);
	SAXParse (in, cb);

	Pop ();	// the docuemnt reader we just added

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
BuiltinReader<String>::BuiltinReader (String* intoVal, const map<String,Memory::VariantValue>& attrs)
	: value_ (intoVal)
{
	RequireNotNull (intoVal);
	*intoVal = String ();
}

void	BuiltinReader<String>::HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
{
	ThrowUnRecognizedStartElt (uri, localName);
}

void	BuiltinReader<String>::HandleTextInside (SAXObjectReader &r, const String& text) override
{
	(*value_) += text;
}

void	BuiltinReader<String>::HandleEndTag (SAXObjectReader &r) override
{
	r.Pop ();
}








/*
 ********************************************************************************
 **************************** XML::BuiltinReader<int> ***************************
 ********************************************************************************
 */
BuiltinReader<int>::BuiltinReader (int* intoVal, const map<String,Memory::VariantValue>& attrs)
	: value_ (intoVal)
	, tmpVal_ ()
{
	RequireNotNull (intoVal);
}

void	BuiltinReader<int>::HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
{
	ThrowUnRecognizedStartElt (uri, localName);
}

void	BuiltinReader<int>::HandleTextInside (SAXObjectReader &r, const String& text) override
{
	tmpVal_ += text;
	(*value_) = Characters::String2Int (tmpVal_.As<wstring> ());
}

void	BuiltinReader<int>::HandleEndTag (SAXObjectReader &r) override
{
	r.Pop ();
}






/*
 ********************************************************************************
 ********************** XML::BuiltinReader<Time::DateTime> **********************
 ********************************************************************************
 */
BuiltinReader<Time::DateTime>::BuiltinReader (Time::DateTime* intoVal, const map<String,Memory::VariantValue>& attrs)
	: value_ (intoVal)
	, tmpVal_ ()
{
	RequireNotNull (intoVal);
	*intoVal = Time::DateTime ();
}

void	BuiltinReader<Time::DateTime>::HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
{
	ThrowUnRecognizedStartElt (uri, localName);
}

void	BuiltinReader<Time::DateTime>::HandleTextInside (SAXObjectReader &r, const String& text) override
{
	tmpVal_ += text;
	// not 100% right to ignore exceptions, but tricky to do more right (cuz not necesarily all text given us at once)
	IgnoreExceptionsForCall ((*value_) = Time::DateTime (tmpVal_.As<wstring> (), Time::DateTime::eXML));
}

void	BuiltinReader<Time::DateTime>::HandleEndTag (SAXObjectReader &r) override
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

void	IgnoreNodeReader::HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
{
	Require (fDepth_ >= 0);
	fDepth_++;
}

void	IgnoreNodeReader::HandleTextInside (SAXObjectReader &r, const String& text) override
{
	// Ignore text
}

void	IgnoreNodeReader::HandleEndTag (SAXObjectReader &r) override
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
void	XML::ThrowUnRecognizedStartElt (const String& uri, const String& localName)
{
	Execution::DoThrow (BadFormatException (Characters::Format (L"Unrecognized start tag '%s'", localName.c_str ())));
}


