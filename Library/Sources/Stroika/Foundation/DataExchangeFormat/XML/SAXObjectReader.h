/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_h_
#define	_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_h_	1

#include	"../../StroikaPreComp.h"

#include	"../../Memory/Optional.h"
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
						SAXObjectReader ();

					#if		qDefaultTracingOn
					public:
						bool	fTraceThisReader;		// very noisy - off by default even for tracemode
						wstring	TraceLeader_ () const;
					#endif

					public:
						nonvirtual	void	Push (const Memory::SharedPtr<ObjectBase>& elt);
						nonvirtual	void	Pop ();

					public:
						nonvirtual	Memory::SharedPtr<ObjectBase>	GetTop () const;

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
							BuiltinReader (T* intoVal, const map<String,Memory::VariantValue>& attrs = map<String,Memory::VariantValue> ());
						
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
				 * OptionalTypesReader supports reads of optional types. This will work - for any types for which BuiltinReader<T>
				 * is implemented.
				 * 
				 *	Note - this ALWAYS produces a result. Its only called when the element in quesiton has already occurred. The reaosn for Optional<>
				 *	part is because the caller had an optional element which might never have triggered the invocation of this class.
				 */
				template	<typename	T, typename ACTUAL_READER = BuiltinReader<T>>
					class	OptionalTypesReader : public SAXObjectReader::ObjectBase {
						public:
							OptionalTypesReader (Memory::Optional<T>* intoVal, const map<String,Memory::VariantValue>& attrs = map<String,Memory::VariantValue> ());
						
						private:
							Memory::Optional<T>*	value_;
							T						proxyValue_;
							ACTUAL_READER			actualReader_;	// this is why its crucial this partial specialization is only used on optional of types a real reader is available for
	
						public:
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
					};




				/*
				 * Push one of these Nodes onto the stack to handle 'reading' a node which is not to be read. This is necessary to balance out
				 * the Start Tag / End Tag combinations.
				 */
				class	IgnoreNodeReader : public SAXObjectReader::ObjectBase {
					public:
						IgnoreNodeReader ();
					private:
						int	fDepth_;
					public:
						virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
						virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
						virtual	void	HandleEndTag (SAXObjectReader &r) override;
				};



				/*
				 * Helper class for reading complex (structured) objects.
				 */
				template	<typename	T>
					class	ComplexObjectReader : public SAXObjectReader::ObjectBase {
						protected:
							ComplexObjectReader (T* vp, const map<String,Memory::VariantValue>& attrs = map<String,Memory::VariantValue> ());

						public:
							T*	fValuePtr;
	
						public:
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
						protected:
							nonvirtual	void	_PushNewObjPtr (SAXObjectReader &r, ObjectBase* newlyAllocatedObject2Push);
					};
	



				/*
				 * The ListOfObjectReader<> template can be used to create a vector of type "T" - to capture repeating elements
				 * in a sequence.
				 *
				 *
				 *  EXAMPLE TRAITS:
				 *  	struct	ReaderTraits {
				 *  			typedef	String					ElementType;
				 *  			typedef	BuiltinReader<String>	ReaderType;
				 *  			static	const wchar_t			ElementName[] =  L"Name";
				 *  	};
				 */
				template	<typename TRAITS>
					struct ListOfObjectReader: public ComplexObjectReader<vector<typename TRAITS::ElementType>> {
						bool							readingAT_;
						typename TRAITS::ElementType	curTReading_;

						ListOfObjectReader (vector<typename TRAITS::ElementType>* v, const map<String,Memory::VariantValue>& attrs = map<String,Memory::VariantValue> ());
						
						virtual void HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String, Memory::VariantValue>& attrs) override;
						virtual void HandleEndTag (SAXObjectReader &r) override;
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
