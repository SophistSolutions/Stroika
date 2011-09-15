/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_h_
#define	_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_h_	1

#include	"../../StroikaPreComp.h"

#include	"../../Memory/SharedPtr.h"
#include	"../../Time/DateTime.h"

#include	"SAXReader.h"




namespace	Stroika {	
	namespace	Foundation {
		namespace	DataExchangeFormat {
			namespace	XML {


				/*
				 *		The basic idea of the SAXObjectReader is to make it easier to write C++ code to deserialize an XML source (via SAX), into
				 *	a C++ data structure. This tends to be MUCH MUCH harder than doing something similar by loading an XML DOM, and then traversing the DOM
				 *	with XPath. So why would you do it? This way is dramatically more efficeint. For one thing - there is no need to have the entire source in memory at
				 *	a time, and there is no need to ever construct intermediary DOM nodes.
				 *
				 *		We need good docs - on how to use this - but for the time being, just look at the example usage in the regression test.
				 */
				class SAXObjectReader {
					public:
						class	ObjectBase;

					public:
						nonvirtual	void	Push (const Memory::SharedPtr<ObjectBase>& elt);
						nonvirtual	void	Pop ();

					public:
						// puts docEltsBuilder on stack and then keeps reading from sax til done. Asserts buildStack is EMPTY at end of this call (and docEltsBuilder should ahve recieved
						// a HandleChildStar tand HandleEndTag() method call (exactly once).
						nonvirtual	void	Run (const Memory::SharedPtr<ObjectBase>& docEltBuilder, istream& in);
						nonvirtual	void	Run (const Memory::SharedPtr<ObjectBase>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, istream& in);
	
					private:
						vector<Memory::SharedPtr<ObjectBase>> fStack_;

					private:
						class	MyCallback_;
				};


				class	SAXObjectReader::ObjectBase {
					public:
						virtual ~ObjectBase ();
						virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) = 0;
						virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) = 0;
						virtual	void	HandleEndTag (SAXObjectReader &r) = 0;
				};


				/*
				 * BuiltinReader<> is not implemented for all types - just for the specialized ones listed below:
				 *		String
				 *		int
				 *		Time::DateTime
				 */
				template	<typename	T>
					class	BuiltinReader : public SAXObjectReader::ObjectBase {
						public:
							BuiltinReader (T* intoVal);
						
						private:
							T* value_;
	
						public:
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
					};

				template	<>
					class	BuiltinReader<String>;
				template	<>
					class	BuiltinReader<int>;
				template	<>
					class	BuiltinReader<Time::DateTime>;



				/*
				 * Helper class for reading complex (structured) objects.
				 */
				template	<typename	T>
					class	ComplexObjectReader : public SAXObjectReader::ObjectBase {
						protected:
							ComplexObjectReader (T* vp);

						public:
							T*	valuePtr;
	
						public:
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
						protected:
							nonvirtual	void	_PushNewObjPtr (SAXObjectReader &r, ObjectBase* newlyAllocatedObject2Push);
					};
	

				void	ThrowUnRecognizedStartElt (const String& uri, const String& localName);

			}
		}
	}
}
#endif	/*_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SAXObjectReader.inl"
