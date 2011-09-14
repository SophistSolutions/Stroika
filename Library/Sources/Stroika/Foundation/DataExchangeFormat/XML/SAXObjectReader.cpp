/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	"../../Characters/StringUtils.h"

#include	"SAXObjectReader.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::DataExchangeFormat;
using	namespace	Stroika::Foundation::DataExchangeFormat::XML;



namespace	{
	void	ThrowUnRecognizedStartElt_ (const String& uri, const String& localName)
		{
			//tmphack impl
			throw Execution::StringException (L"FIX _ DO SPECIAL TYPE FOR THIS - UNRECOGNIZED START ELT IN XML PARSOING");
		}
}


/*
 ********************************************************************************
 ****************************** XML::SAXObjectReader ****************************
 ********************************************************************************
 */
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
				AssertNotNull (fSAXObjectReader_.fStack.back ());
				fSAXObjectReader_.fStack.back ()->HandleChildStart (fSAXObjectReader_, uri, localName, qname, attrs);
			}
		virtual	void	EndElement (const String& uri, const String& localName, const String& qname) override
			{
				AssertNotNull (fSAXObjectReader_.fStack.back ());
				fSAXObjectReader_.fStack.back ()->HandleEndTag (fSAXObjectReader_);
			}
		virtual	void	CharactersInsideElement (const String& text) override
			{
				AssertNotNull (fSAXObjectReader_.fStack.back ());
				fSAXObjectReader_.fStack.back ()->HandleTextInside (fSAXObjectReader_, text);
			}
};

void	SAXObjectReader::Run (ObjectBase* docEltBuilder, istream& in)
{
	RequireNotNull (docEltBuilder);
	Require (fStack.size () == 0);

	Push (Memory::SharedPtr<ObjectBase> (docEltBuilder));

	MyCallback_ cb (*this);
	SAXParse (in, cb);

	Require (fStack.size () == 0);
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
BuiltinReader<String>::BuiltinReader (String* intoVal)
	: value_ (intoVal)
{
}

void	BuiltinReader<String>::HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
{
	ThrowUnRecognizedStartElt_ (uri, localName);
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
BuiltinReader<int>::BuiltinReader (int* intoVal)
	: value_ (intoVal)
	, tmpVal_ ()
{
}

void	BuiltinReader<int>::HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
{
	ThrowUnRecognizedStartElt_ (uri, localName);
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
 ************************* XML::BuiltinReader<Time::DateTime> ***************************
 ********************************************************************************
 */
BuiltinReader<Time::DateTime>::BuiltinReader (Time::DateTime* intoVal)
	: value_ (intoVal)
	, tmpVal_ ()
{
}

void	BuiltinReader<Time::DateTime>::HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
{
	ThrowUnRecognizedStartElt_ (uri, localName);
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
