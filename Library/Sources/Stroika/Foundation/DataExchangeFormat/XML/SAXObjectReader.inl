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
							BuiltinReader (String* intoVal);
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
							BuiltinReader (int* intoVal);
						private:
							String	tmpVal_;
							int*	value_;
						public:
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
					};
				template	<>
					class	BuiltinReader<Time::DateTime> : public SAXObjectReader::ObjectBase {
						public:
							BuiltinReader (Time::DateTime* intoVal);
						private:
							String			tmpVal_;
							Time::DateTime* value_;
						public:
							virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override;
							virtual	void	HandleTextInside (SAXObjectReader &r, const String& text) override;
							virtual	void	HandleEndTag (SAXObjectReader &r) override;
					};




				template	<typename	T>
					inline	ComplexObjectReader<T>::ComplexObjectReader (T* vp)
						: valuePtr (vp)
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


			}
		}
	}
}
#endif	/*_Stroika_Foundation_DataExchangeFormat_XML_SAXObjectReader_inl_*/
	