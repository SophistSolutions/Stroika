/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_inl_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {



                /*
                  ********************************************************************************
                  **************************** Context *****************************
                  ********************************************************************************
                  */
                inline   Context::Context (const ObjectReaderRegistry& objectReaderRegistry)
                    : fObjectReaderRegistry_ (objectReaderRegistry)
                {
                }                inline  void    Context::Push (const shared_ptr<IElementConsumer>& elt)
                {
                    RequireNotNull (elt);
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
                    if (fTraceThisReader) {
                        DbgTrace (L"%sContext::Push [%s]", TraceLeader_ ().c_str (), String::FromNarrowSDKString (typeid (*elt.get ()).name ()).c_str ());
                    }
#endif
                    Containers::ReserveSpeedTweekAdd1 (fStack_);
                    fStack_.push_back (elt);
                    elt->Activated (*this);
                }
                inline  void    Context::Pop ()
                {
                    fStack_.back()->Deactivating (*this);
                    fStack_.pop_back ();
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
                    if (fTraceThisReader) {
                        if (fStack_.empty ()) {
                            DbgTrace (L"%sContext::Popped [empty stack]", TraceLeader_ ().c_str ());
                        }
                        else {
                            DbgTrace (L"%sContext::Popped [back to: %s]", TraceLeader_ ().c_str (), String::FromNarrowSDKString (typeid (*GetTop ().get ()).name ()).c_str ());
                        }
                    }
#endif
                }
                inline  shared_ptr<IElementConsumer>  Context::GetTop () const
                {
                    Require (not fStack_.empty ());
                    return fStack_.back ();
                }
                inline  const   ObjectReaderRegistry&   Context::GetObjectReaderRegistry () const
                {
                    return fObjectReaderRegistry_;
                }




                /*
                 ********************************************************************************
                 ********************************** SimpleReader *******************************
                 ********************************************************************************
                 */
                template    <typename   T>
                SimpleReader<T>::SimpleReader (T* intoVal)
                    : fBuf_ ()
                    , fValue_ (intoVal)
                {
                }
                template    <typename   T>
                shared_ptr<IElementConsumer>    SimpleReader<T>::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
                {
                    ThrowUnRecognizedStartElt (name);
                }
                template    <typename   T>
                void    SimpleReader<T>::HandleTextInside (Context& r, const String& text)
                {
                    fBuf_ += text;
                }
                template    <typename   T>
                void   SimpleReader<T>::Deactivating (Context& r)
                {
                    AssertNotReached ();   // redo with static asserts, but issues on some compilers - anyhow - SB template specialzied away
                    // *value = CONVERTFROM (fBuf_.str ());
                }


                /*
                 ********************************************************************************
                 ******************** OptionalTypesReader<T, ACTUAL_READER> *********************
                 ********************************************************************************
                 */
                template    <typename   T, typename ACTUAL_READER>
                OptionalTypesReader<T, ACTUAL_READER>::OptionalTypesReader (Memory::Optional<T>* intoVal)
                    : value_ (intoVal)
                    , proxyValue_ ()
                    , actualReader_ (&proxyValue_)
                {
                }
                template    <typename   T, typename ACTUAL_READER>
                shared_ptr<IElementConsumer>    OptionalTypesReader<T, ACTUAL_READER>::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
                {
                    return actualReader_.HandleChildStart (r, name);
                }
                template    <typename   T, typename ACTUAL_READER>
                void    OptionalTypesReader<T, ACTUAL_READER>::HandleTextInside (Context& r, const String& text)
                {
                    actualReader_.HandleTextInside (r, text);
                }
                template    <typename   T, typename ACTUAL_READER>
                bool    OptionalTypesReader<T, ACTUAL_READER>::HandleEndTag (Context& r)
                {
                    return actualReader_.HandleEndTag (r);
                }
                template    <typename   T, typename ACTUAL_READER>
                void    OptionalTypesReader<T, ACTUAL_READER>::Deactivating (Context& r)
                {
                    actualReader_.Deactivating (r);
                    *value_ = proxyValue_;
                }



                /*
                 ********************************************************************************
                 *************************** ListOfObjectReader<TRAITS> *************************
                 ********************************************************************************
                 */
                template    <typename TRAITS>
                ListOfObjectReader<TRAITS>::ListOfObjectReader (vector<typename TRAITS::ElementType>* v, UnknownSubElementDisposition unknownEltDisposition)
                    : IElementConsumer ()
                    , fValuePtr (v)
                    , fUnknownSubElementDisposition_ (unknownEltDisposition)
                {
                }
                template    <typename TRAITS>
                shared_ptr<IElementConsumer>    ListOfObjectReader<TRAITS>::HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)
                {
                    // if we have an existing reader, we must save the data from it, and close it out
                    if (fCurReader_ != nullptr) {
                        this->fValuePtr->push_back (fCurTReading_);
                        fCurReader_ = nullptr;
                    }
                    if (fCurReader_ == nullptr and name.fLocalName == TRAITS::ElementName) {
                        fCurTReading_ = typename TRAITS::ElementType (); // clear because dont' want to keep values from previous elements
                        fCurReader_ = make_shared<typename TRAITS::ReaderType> (&fCurTReading_);
                        return (fCurReader_);
                    }
                    else {
                        //// @todo SHOULD allow for EITHER pass back to parent or just keep going and ignore other elements
                        /// we are at the top of the stack, but we want to pop, and hand this 'new child' to our parent
                        if (fUnknownSubElementDisposition_ == UnknownSubElementDisposition::eEndObject) {
                            auto save  = this->shared_from_this ();
                            r.Pop ();
                            r.GetTop ()->HandleChildStart (r, name);
                        }
                        else {
                            AssertNotImplemented ();    // @todo
                        }
                        return nullptr;
                    }
                }
                template    <typename TRAITS>
                bool    ListOfObjectReader<TRAITS>::HandleEndTag (Context& r)
                {

                    /// tricky - we need to rewrite this/REDO???
                    // if we have an existing reader, we must save the data from it, and close it out
                    if (fCurReader_ != nullptr) {
                        this->fValuePtr->push_back (fCurTReading_);
                        fCurReader_ = nullptr;
                    }

                    r.Pop ();
                    return false;
                }
                template    <typename TRAITS>
                void    ListOfObjectReader<TRAITS>::Deactivating (Context& r)
                {
                    // if we have an existing reader, we must save the data from it, and close it out
                    if (fCurReader_ != nullptr) {
                        this->fValuePtr->push_back (fCurTReading_);
                        fCurReader_ = nullptr;
                    }
                }

            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_inl_*/
