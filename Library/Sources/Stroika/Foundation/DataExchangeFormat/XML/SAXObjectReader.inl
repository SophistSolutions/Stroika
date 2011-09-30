/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_inl_
#define	_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../../Containers/Common.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	DataExchangeFormat {
			namespace	XML {

				//	class	SAXObjectReader
				inline	SAXObjectReader::SAXObjectReader ()
					#if		qDefaultTracingOn
						: fTraceThisReader (false)
					#endif
					{
					}
				inline	void	SAXObjectReader::Push (const Memory::SharedPtr<ObjectBase>& elt)
					{
						#if		qDefaultTracingOn
							if (fTraceThisReader) {
								DbgTrace ("%sSAXObjectReader::Push", TraceLeader_ ().c_str ());
							}
						#endif
						Containers::ReserveSpeedTweekAdd1 (fStack_);
						fStack_.push_back (elt);
					}
				inline	void	SAXObjectReader::Pop ()
					{
						fStack_.pop_back ();
						#if		qDefaultTracingOn
							if (fTraceThisReader) {
								DbgTrace ("%sSAXObjectReader::Popped", TraceLeader_ ().c_str ());
							}
						#endif
					}
				inline	Memory::SharedPtr<SAXObjectReader::ObjectBase>	SAXObjectReader::GetTop () const
					{
						Require (not fStack_.empty ());
						return fStack_.back ();
					}


				template	<>
					class	BuiltinReader<String> : public SAXObjectReader::ObjectBase {
						public:
							BuiltinReader (String* intoVal, const map<String,Memory::VariantValue>& attrs = map<String,Memory::VariantValue> ());
						private:
							String* value_;
						public:
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
					};
				template	<>
					class	BuiltinReader<int> : public SAXObjectReader::ObjectBase {
						public:
							BuiltinReader (int* intoVal, const map<String,Memory::VariantValue>& attrs = map<String,Memory::VariantValue> ());
						private:
							String	tmpVal_;
							int*	value_;
						public:
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
					};
				template	<>
					class	BuiltinReader<unsigned int> : public SAXObjectReader::ObjectBase {
						public:
							BuiltinReader (unsigned int* intoVal, const map<String,Memory::VariantValue>& attrs = map<String,Memory::VariantValue> ());
						private:
							String	tmpVal_;
							unsigned int*	value_;
						public:
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
					};
				template	<>
					class	BuiltinReader<float> : public SAXObjectReader::ObjectBase {
						public:
							BuiltinReader (float* intoVal, const map<String,Memory::VariantValue>& attrs = map<String,Memory::VariantValue> ());
						private:
							String	tmpVal_;
							float*	value_;
						public:
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
					};
				template	<>
					class	BuiltinReader<bool> : public SAXObjectReader::ObjectBase {
						public:
							BuiltinReader (bool* intoVal, const map<String,Memory::VariantValue>& attrs = map<String,Memory::VariantValue> ());
						private:
							String	tmpVal_;
							bool*	value_;
						public:
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
					};
				template	<>
					class	BuiltinReader<Time::DateTime> : public SAXObjectReader::ObjectBase {
						public:
							BuiltinReader (Time::DateTime* intoVal, const map<String,Memory::VariantValue>& attrs = map<String,Memory::VariantValue> ());
						private:
							String			tmpVal_;
							Time::DateTime* value_;
						public:
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
					};


					
					
					
					
				template	<typename	T, typename ACTUAL_READER>
					OptionalTypesReader<T,ACTUAL_READER>::OptionalTypesReader (Memory::Optional<T>* intoVal, const map<String,Memory::VariantValue>& attrs)
						: value_ (intoVal)
						, proxyValue_ ()
						, actualReader_ (&proxyValue_)
						{
						}
				template	<typename	T, typename ACTUAL_READER>
					void	OptionalTypesReader<T,ACTUAL_READER>::HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
						{
							actualReader_.HandleChildStart (r, uri, localName, qname, attrs);
						}
				template	<typename	T, typename ACTUAL_READER>
					void	OptionalTypesReader<T,ACTUAL_READER>::HandleTextInside (SAXObjectReader &r, const String& text) override
						{
							actualReader_.HandleTextInside (r, text);
						}
				template	<typename	T, typename ACTUAL_READER>
					void	OptionalTypesReader<T,ACTUAL_READER>::HandleEndTag (SAXObjectReader &r) override
						{
							Memory::SharedPtr<ObjectBase>	saveCopyOfUs		=	r.GetTop ();	// bump our reference count til the end of the procedure
																									// because the HandleEndTag will typically cause a POP on the reader that destroys us!
																									// However, we cannot do the copy back to value beofre the base POP, because
																									// it also might do some additioanl processing on its value
							actualReader_.HandleEndTag (r);
							*value_ = proxyValue_;
						}






				template	<typename	T>
					inline	ComplexObjectReader<T>::ComplexObjectReader (T* vp, const map<String,Memory::VariantValue>& attrs)
						: fValuePtr (vp)
						{
							RequireNotNull (vp);
						}
				template	<typename	T>
					void	ComplexObjectReader<T>::HandleTextInside (SAXObjectReader &r, const String& text) override
						{
							// OK so long as text is whitespace - or comment. Probably should check/assert, but KISS..., and count on validation to
							// assure input is valid
							Assert (text.IsWhitespace ());
						}
				template	<typename	T>
					void	ComplexObjectReader<T>::HandleEndTag (SAXObjectReader &r) override
						{
							r.Pop ();
						}
				template	<typename	T>
					void	ComplexObjectReader<T>::_PushNewObjPtr (SAXObjectReader &r, ObjectBase* newlyAllocatedObject2Push)
						{
							RequireNotNull (newlyAllocatedObject2Push);
							r.Push (Memory::SharedPtr<ObjectBase> (newlyAllocatedObject2Push));
						}





				template	<typename TRAITS>
					ListOfObjectReader<TRAITS>::ListOfObjectReader (vector<typename TRAITS::ElementType>* v, const map<String,Memory::VariantValue>& attrs)
						: ComplexObjectReader<vector<typename TRAITS::ElementType>> (v)
						, readingAT_ (false)
						{
						}
				template	<typename TRAITS>
					void ListOfObjectReader<TRAITS>::HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String, Memory::VariantValue>& attrs) override
						{
							if (localName == TRAITS::ElementName) {
								if (readingAT_) {
									Containers::ReserveSpeedTweekAdd1 (*this->fValuePtr);
									this->fValuePtr->push_back (curTReading_);
									readingAT_ = false;
								}
								readingAT_ = true;
								curTReading_ = typename TRAITS::ElementType (); // clear because dont' want to keep values from previous elements
								_PushNewObjPtr (r, new typename TRAITS::ReaderType (&curTReading_, attrs));
							}
							else {
								ThrowUnRecognizedStartElt (uri, localName);
							}
						}
				template	<typename TRAITS>
					void ListOfObjectReader<TRAITS>::HandleEndTag (SAXObjectReader &r) override
						{
							if (readingAT_) {
								Containers::ReserveSpeedTweekAdd1 (*this->fValuePtr);
								this->fValuePtr->push_back (curTReading_);
								readingAT_ = false;
							}
							ComplexObjectReader<vector<typename TRAITS::ElementType>>::HandleEndTag (r);
						}



			}
		}
	}
}
#endif	/*_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_inl_*/
	