/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_h_
#define	_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_h_	1

#include	"../../StroikaPreComp.h"

#include	"../../Memory/SharedPtr.h"
#include	"../../Time/DateUtils.h"

#include	"SAXReader.h"




namespace	Stroika {	
	namespace	Foundation {
		namespace	DataExchangeFormat {
			namespace	XML {




				/*
				 *		NEEED EXPLANATION OF HOW TO USE - DOCS
				 */
				class SAXObjectReader {
					public:
						class	ObjectBase;

					public:
						nonvirtual	void	Push (const Memory::SharedPtr<ObjectBase>& elt);
						nonvirtual	void	Pop ();

					public:
						// puts docEltsBuilder on stack and then keeps reading from sax til done. Asserts buildStack is EMPTY at end of this call (and docEltsBuilder should ahve recieved
						// a HandleCHildStar tand HandleEndTag() method call (exactly once).
						virtual	void	Run (ObjectBase* docEltBuilder, istream& in);
	
					private:
						vector<Memory::SharedPtr<ObjectBase>> fStack;
				};


				class	SAXObjectReader::ObjectBase {
					public:
						virtual ~ObjectBase ();
						virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localname, const String& qname, const map<String,Memory::VariantValue>& attrs) = 0;
						virtual	void	HandleTextInside (SAXObjectReader &r, String text) = 0;
						virtual	void	HanldeEndTag (SAXObjectReader &r) = 0;
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
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localname, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, String text) override;
							virtual	void	HanldeEndTag (SAXObjectReader &r) override;
					};

				template	<>
					class	BuiltinReader<String>;
				template	<>
					class	BuiltinReader<int>;
				template	<>
					class	BuiltinReader<Time::DateTime>;

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
