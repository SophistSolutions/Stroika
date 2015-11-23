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
                 ********************** ObjectReaderRegistry::Context ***************************
                 ********************************************************************************
                 */
                inline   ObjectReaderRegistry::Context::Context (const ObjectReaderRegistry& objectReaderRegistry)
                    : fObjectReaderRegistry_ (objectReaderRegistry)
                {
                }
                inline   ObjectReaderRegistry::Context::Context (Context&& from)
                    : fObjectReaderRegistry_ (from.fObjectReaderRegistry_)
                    , fStack_ (move (from.fStack_))
                {
                }
                inline  void    ObjectReaderRegistry::Context::Push (const shared_ptr<IElementConsumer>& elt)
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
                inline  void    ObjectReaderRegistry::Context::Pop ()
                {
                    fStack_.back()->Deactivating ();
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
                inline  shared_ptr<ObjectReaderRegistry::IElementConsumer>  ObjectReaderRegistry::Context::GetTop () const
                {
                    Require (not fStack_.empty ());
                    return fStack_.back ();
                }
                inline  const   ObjectReaderRegistry&   ObjectReaderRegistry::Context::GetObjectReaderRegistry () const
                {
                    return fObjectReaderRegistry_;
                }
                inline  bool    ObjectReaderRegistry::Context::empty () const
                {
                    return fStack_.empty ();
                }


                /*
                  ********************************************************************************
                  ************* ObjectReaderRegistry::IConsumerDelegateToContext *****************
                  ********************************************************************************
                  */
                inline  ObjectReaderRegistry::IConsumerDelegateToContext::IConsumerDelegateToContext (Context&& r)
                    : fContext (move (r))
                {
                }
                inline  ObjectReaderRegistry::IConsumerDelegateToContext::IConsumerDelegateToContext (const ObjectReaderRegistry& objectReaderRegistry)
                    : fContext (objectReaderRegistry)
                {
                }
                inline  ObjectReaderRegistry::IConsumerDelegateToContext::IConsumerDelegateToContext (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<IElementConsumer>& initialTop)
                    : fContext (objectReaderRegistry, initialTop)
                {
                }


                /*
                 ********************************************************************************
                 ************************************ ClassReader *******************************
                 ********************************************************************************
                 */
                template    <typename   T>
                ObjectReaderRegistry::ClassReader<T>::ClassReader (const Mapping<Name, StructFieldMetaInfo>& maps, T* vp)
                    : fValuePtr_ (vp)
                {
                    RequireNotNull (vp);
                    for (Common::KeyValuePair<Name, StructFieldMetaInfo> i : maps) {
                        if (i.fKey.fType == Name::eValue) {
                            fValueFieldMetaInfo_ = i.fValue;
                        }
                        else {
                            fFieldNameToTypeMap_.Add (i.fKey, i.fValue);
                        }
                    }
                }
                template    <typename   T>
                void    ObjectReaderRegistry::ClassReader<T>::Activated (Context& r)
                {
                    Require (fActiveContext_ == nullptr);
                    fActiveContext_ = &r;
                }
                template    <typename   T>
                shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::ClassReader<T>::HandleChildStart (const Name& name)
                {
                    RequireNotNull (fActiveContext_);
                    Optional<StructFieldMetaInfo>   ti = fFieldNameToTypeMap_.Lookup (name);
                    if (ti) {
                        Byte*   operatingOnObj      = reinterpret_cast<Byte*> (this->fValuePtr_);
                        Byte*   operatingOnObjField = operatingOnObj + ti->fOffset;
                        return fActiveContext_->GetObjectReaderRegistry ().MakeContextReader (ti->fTypeInfo, operatingOnObjField);
                    }
                    else if (fThrowOnUnrecongizedelts_) {
                        ThrowUnRecognizedStartElt (name);
                    }
                    else {
                        return make_shared<IgnoreNodeReader> ();
                    }
                }
                template    <typename   T>
                void    ObjectReaderRegistry::ClassReader<T>::HandleTextInside (const String& text)
                {
                    RequireNotNull (fActiveContext_);
                    if (fValueFieldMetaInfo_) {
                        Assert (fValueFieldConsumer_ == nullptr);
                        Byte*   operatingOnObj      = reinterpret_cast<Byte*> (this->fValuePtr_);
                        Byte*   operatingOnObjField = operatingOnObj + fValueFieldMetaInfo_->fOffset;
                        fValueFieldConsumer_ = fActiveContext_->GetObjectReaderRegistry ().MakeContextReader (fValueFieldMetaInfo_->fTypeInfo, operatingOnObjField);
                        fValueFieldConsumer_->Activated (*fActiveContext_);
                        fValueFieldMetaInfo_.clear ();
                    }
                    if (fValueFieldConsumer_) {
                        fValueFieldConsumer_->HandleTextInside (text);
                    }
                }
                template    <typename   T>
                void    ObjectReaderRegistry::ClassReader<T>::Deactivating ()
                {
                    RequireNotNull (fActiveContext_);
                    if (fValueFieldConsumer_) {
                        fValueFieldConsumer_->Deactivating ();
                    }
                    fActiveContext_ = nullptr;
                }


                /*
                 ********************************************************************************
                 ******************* ObjectReaderRegistry::ListOfObjectReader *******************
                 ********************************************************************************
                 */
                template    <typename CONTAINER_OF_T>
                ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T>::ListOfObjectReader (CONTAINER_OF_T* v)
                    : fValuePtr_ (v)
                {
                    RequireNotNull (v);
                }
                template    <typename CONTAINER_OF_T>
                ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T>::ListOfObjectReader (CONTAINER_OF_T* v, const Name& memberElementName)
                    : fMemberElementName_  (memberElementName)
                    , fValuePtr_ (v)
                {
                    RequireNotNull (v);
                }
                template    <typename CONTAINER_OF_T>
                void    ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T>::Activated (Context& r)
                {
                    Require (fActiveContext_ == nullptr);
                    fActiveContext_ = &r;
                }
                template    <typename CONTAINER_OF_T>
                shared_ptr<ObjectReaderRegistry::IElementConsumer> ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T>::HandleChildStart (const StructuredStreamEvents::Name& name)
                {
                    RequireNotNull (fActiveContext_);
                    if (fMemberElementName_.IsMissing () or name == *fMemberElementName_) {
                        if (fReadingAT_) {
                            this->fValuePtr_->push_back (fCurTReading_);
                            fReadingAT_ = false;
                        }
                        fReadingAT_ = true;
                        fCurTReading_ = ElementType (); // clear because dont' want to keep values from previous elements
                        return fActiveContext_->GetObjectReaderRegistry ().MakeContextReader<ElementType> (&fCurTReading_);
                    }
                    else if (fThrowOnUnrecongizedelts_) {
                        ThrowUnRecognizedStartElt (name);
                    }
                    else {
                        return make_shared<IgnoreNodeReader> ();
                    }
                }
                template    <typename CONTAINER_OF_T>
                void    ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T>::Deactivating ()
                {
                    RequireNotNull (fActiveContext_);
                    if (fReadingAT_) {
                        this->fValuePtr_->push_back (fCurTReading_);
                        fReadingAT_ = false;
                    }
                    fActiveContext_ = nullptr;
                }


                /*
                 ********************************************************************************
                 ************** ObjectReaderRegistry::RepeatedElementReader *********************
                 ********************************************************************************
                 */
                template    <typename CONTAINER_OF_T>
                ObjectReaderRegistry::RepeatedElementReader<CONTAINER_OF_T>::RepeatedElementReader (CONTAINER_OF_T* pv)
                    : fValuePtr_ (pv)
                {
                }
                template    <typename CONTAINER_OF_T>
                void    ObjectReaderRegistry::RepeatedElementReader<CONTAINER_OF_T>::Activated (ObjectReaderRegistry::Context& r)
                {
                    Assert (fActualReader_ == nullptr);
                    fActualReader_ = r.GetObjectReaderRegistry ().MakeContextReader (&fProxyValue_);
                    fActualReader_->Activated (r);
                }
                template    <typename CONTAINER_OF_T>
                shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::RepeatedElementReader<CONTAINER_OF_T>::HandleChildStart (const Name& name)
                {
                    AssertNotNull (fActualReader_);
                    return fActualReader_->HandleChildStart (name);
                }
                template    <typename CONTAINER_OF_T>
                void    ObjectReaderRegistry::RepeatedElementReader<CONTAINER_OF_T>::HandleTextInside (const String& text)
                {
                    AssertNotNull (fActualReader_);
                    fActualReader_->HandleTextInside (text);
                }
                template    <typename CONTAINER_OF_T>
                void    ObjectReaderRegistry::RepeatedElementReader<CONTAINER_OF_T>::Deactivating ()
                {
                    AssertNotNull (fActualReader_);
                    fActualReader_->Deactivating ();
                    fValuePtr_->Append (fProxyValue_);
                }


                /*
                 ********************************************************************************
                 ***************************** ObjectReaderRegistry *****************************
                 ********************************************************************************
                 */
                inline void    ObjectReaderRegistry::Add (type_index forType, const ReaderFromVoidStarFactory& readerFactory)
                {
                    fFactories_.Add (forType, readerFactory);
                }
                template    <typename T>
                void    ObjectReaderRegistry::Add (const ReaderFromTStarFactory<T>& readerFactory)
                {
                    Add (typeid (T), [readerFactory] (void* data) { return readerFactory (reinterpret_cast<T*> (data)); });
                }
                inline shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::MakeContextReader (type_index ti, void* destinationObject) const
                {
#if     qDebug
                    if (not fFactories_.ContainsKey (ti)) {
                        Debug::TraceContextBumper   ctx ("ObjectReaderRegistry::MakeContextReader");
                        DbgTrace ("(forTypeInfo = %s) - UnRegistered Type!", ti.name ());
                    }
#endif
                    ReaderFromVoidStarFactory  factory = *fFactories_.Lookup (ti); // must be found or caller/assert error
                    return factory (destinationObject);
                }
                template    <typename T>
                inline  shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::MakeContextReader (T* destinationObject) const
                {
                    return MakeContextReader (typeid (T), destinationObject);
                }
                template    <typename T, typename... ARGS>
                inline  void    ObjectReaderRegistry::AddCommonType (ARGS&& ... args)
                {
                    Add<T> (MakeCommonReader<T> (forward<ARGS> (args)...));
                }
                inline  shared_ptr<ObjectReaderRegistry::ReadDownToReader>      ObjectReaderRegistry::mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader)
                {
                    return make_shared<ReadDownToReader> (theUseReader);
                }
                inline  shared_ptr<ObjectReaderRegistry::ReadDownToReader>      ObjectReaderRegistry::mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff)
                {
                    return make_shared<ReadDownToReader> (theUseReader, tagToHandOff);
                }
                template    <typename CLASS>
                void    ObjectReaderRegistry::AddClass (const Mapping<Name, StructFieldMetaInfo>& fieldInfo)
                {
#if     qDebug
                    for (auto kv : fieldInfo) {
                        if (not fFactories_.ContainsKey (kv.fValue.fTypeInfo)) {
                            Debug::TraceContextBumper   ctx ("ObjectReaderRegistry::AddClass");
                            DbgTrace ("(forTypeInfo = %s) - UnRegistered Type!", kv.fValue.fTypeInfo.name ());
                            RequireNotReached ();
                        }
                    }
#endif
                    Add<CLASS> (MakeClassReader<CLASS> (fieldInfo));
                }
                template    <typename CLASS>
                auto    ObjectReaderRegistry::MakeClassReader (const Mapping<Name, StructFieldMetaInfo>& fieldInfo) -> ReaderFromVoidStarFactory {
                    return [fieldInfo] (void* data) -> shared_ptr<ObjectReaderRegistry::IElementConsumer> { return make_shared<ObjectReaderRegistry::ClassReader<CLASS>> (fieldInfo, reinterpret_cast<CLASS*> (data)); };
                }
                template    <typename T, typename READER>
                auto    ObjectReaderRegistry::ConvertReaderToFactory () -> ReaderFromVoidStarFactory {
                    ObjectReaderRegistry::ReaderFromTStarFactory<T>   tmpFactory      { [] (T * o) -> shared_ptr<ObjectReaderRegistry::IElementConsumer> { return make_shared<READER> (o); } };
                    return [tmpFactory] (void* data) { return tmpFactory (reinterpret_cast<T*> (data)); };
                }
                template    <typename T>
                auto   ObjectReaderRegistry::cvtFactory_ (const ReaderFromTStarFactory<T>& tf) -> ReaderFromVoidStarFactory {
                    return [tf] (void* data) { return tf (reinterpret_cast<T*> (data)); };
                }
                template    <typename T>
                auto   ObjectReaderRegistry::MakeCommonReader_SimpleReader_ () -> ReaderFromVoidStarFactory {
                    return cvtFactory_<T> ( [] (T * o) -> shared_ptr<IElementConsumer> { return make_shared<SimpleReader_<T>> (o); });
                }
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const String*)
                {
                    return MakeCommonReader_SimpleReader_<String> ();
                }
                template    <typename T>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const T*, typename std::enable_if<std::is_pod<T>::value >::type*)
                {
                    return MakeCommonReader_SimpleReader_<T> ();
                }
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const Time::DateTime*)
                {
                    return MakeCommonReader_SimpleReader_<Time::DateTime> ();
                }
                template    <typename T, typename TRAITS>
                inline   ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader_ (const Memory::Optional<T, TRAITS>*)
                {
                    return cvtFactory_<Memory::Optional<T>> ( [] (Memory::Optional<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<OptionalTypesReader_<T>> (o); });
                }
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader_ (const vector<T>*)
                {
                    return cvtFactory_<vector<T>> ( [] (vector<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectReader<vector<T>>> (o); });
                }
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader_ (const vector<T>*, const Name& name)
                {
                    return cvtFactory_<vector<T>> ( [name] (vector<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectReader<vector<T>>> (o, name); });
                }
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader_ (const Sequence<T>*)
                {
                    return cvtFactory_<Sequence<T>> ( [] (Sequence<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectReader<Sequence<T>>> (o); });
                }
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader_ (const Sequence<T>*, const Name& name)
                {
                    return cvtFactory_<Sequence<T>> ( [name] (Sequence<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectReader<Sequence<T>>> (o, name); });
                }
                template    <typename T, typename... ARGS>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader (ARGS&& ... args)
                {
                    const T*  n = nullptr;    // arg unused, just for overloading
                    DISABLE_COMPILER_MSC_WARNING_START (6011)
                    return MakeCommonReader_ (n, forward<ARGS> (args)...);
                    DISABLE_COMPILER_MSC_WARNING_END (6011)
                }


                /*
                 ********************************************************************************
                 ******************* ObjectReaderRegistry::SimpleReader_ ************************
                 ********************************************************************************
                 */
                template    <typename   T>
                inline  ObjectReaderRegistry::SimpleReader_<T>::SimpleReader_ (T* intoVal)
                    : fValue_ (intoVal)
                {
                    RequireNotNull (intoVal);
                }
                template    <typename   T>
                shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::SimpleReader_<T>::HandleChildStart (const StructuredStreamEvents::Name& name)
                {
                    ThrowUnRecognizedStartElt (name);
                }
                template    <typename   T>
                void    ObjectReaderRegistry::SimpleReader_<T>::HandleTextInside (const String& text)
                {
                    fBuf_ += text;
                }
                template    <typename   T>
                void   ObjectReaderRegistry::SimpleReader_<T>::Deactivating ()
                {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                    RequireNotReached ();
#else
                    static_assert (false, "Only specifically specialized variants are supported");
#endif
                }


                /*
                 ********************************************************************************
                 ************* ObjectReaderRegistry::OptionalTypesReader_<T> ********************
                 ********************************************************************************
                 */
                template    <typename   T>
                inline  ObjectReaderRegistry::OptionalTypesReader_<T>::OptionalTypesReader_ (Memory::Optional<T>* intoVal)
                    : fValue_ (intoVal)
                {
                    RequireNotNull (intoVal);
                }
                template    <typename   T>
                void    ObjectReaderRegistry::OptionalTypesReader_<T>::Activated (Context& r)
                {
                    Assert (fActualReader_ == nullptr);
                    fActualReader_ = r.GetObjectReaderRegistry ().MakeContextReader (&fProxyValue_);
                    fActualReader_->Activated (r);
                }
                template    <typename   T>
                shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::OptionalTypesReader_<T>::HandleChildStart (const StructuredStreamEvents::Name& name)
                {
                    AssertNotNull (fActualReader_);
                    return fActualReader_->HandleChildStart (name);
                }
                template    <typename   T>
                void    ObjectReaderRegistry::OptionalTypesReader_<T>::HandleTextInside (const String& text)
                {
                    AssertNotNull (fActualReader_);
                    fActualReader_->HandleTextInside (text);
                }
                template    <typename   T>
                void    ObjectReaderRegistry::OptionalTypesReader_<T>::Deactivating ()
                {
                    AssertNotNull (fActualReader_);
                    fActualReader_->Deactivating ();
                    *fValue_ = fProxyValue_;
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_inl_*/
