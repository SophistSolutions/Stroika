/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_inl_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Characters/String2Int.h"
#include    "../../Characters/String_Constant.h"
#include    "../../Characters/ToString.h"
#include    "../BadFormatException.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {
                namespace   ObjectReader {


                    /**
                     *  [private]
                     *  SimpleReader_<> is not implemented for all types - just for the with Deactivating specialized below;
                     *
                     *  The class (template) generically accumulates the text from inside the element, but then the Deactivating () override must
                     *  be specialized for each class to 'convert' from the accumulated string to the fValue.
                     */
                    template    <typename   T>
                    class   Registry::SimpleReader_ : public IElementConsumer {
                    public:
                        SimpleReader_ (T* intoVal)
                            : fValue_ (intoVal)
                        {
                            RequireNotNull (intoVal);
                        }

                    public:
                        virtual void    Activated (Context& r) override
                        {
                            fBuf_.clear (); // readers can legally be re-used
                        }
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override
                        {
                            ThrowUnRecognizedStartElt (name);
                        }
                        virtual void                            HandleTextInside (const String& text) override
                        {
                            fBuf_ += text;
                        }
                        virtual void                            Deactivating () override;

                    public:
                        /**
                         *  Helper to convert a reader to a factory (something that creates the reader).
                         */
                        static  ReaderFromVoidStarFactory   AsFactory ()
                        {
                            return IElementConsumer::AsFactory<T, SimpleReader_> ();
                        }

                    private:
                        Characters::StringBuilder   fBuf_{};
                        T*                          fValue_{};
                    };
                    template <>
                    void   Registry::SimpleReader_<String>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<char>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<unsigned char>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<short>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<unsigned short>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<int>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<unsigned int>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<long int>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<unsigned long int>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<long long int>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<unsigned long long int>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<bool>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<float>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<double>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<long double>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<Time::DateTime>::Deactivating ();
                    template <>
                    void   Registry::SimpleReader_<Time::Duration>::Deactivating ();
                    template    <typename   T>
                    void                            Registry::SimpleReader_<T>::Deactivating ()
                    {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                        RequireNotReached ();
#else
                        static_assert (false, "Only specifically specialized variants are supported");
#endif
                    }


                    /**
                     *  [private]
                     *
                     *  OptionalTypesReader_ supports reads of optional types. This will work - for any types for
                     *  which SimpleReader<T> is implemented.
                     *
                     *  Note - this ALWAYS produces a result. Its only called when the element in quesiton has
                     *  already occurred. The reaosn for Optional<> part is because the caller had an optional
                     *  element which might never have triggered the invocation of this class.
                     */
                    template    <typename   T>
                    class   Registry::OptionalTypesReader_ : public IElementConsumer {
                    public:
                        OptionalTypesReader_ (Memory::Optional<T>* intoVal)
                            : fValue_ (intoVal)
                        {
                            RequireNotNull (intoVal);
                        }

                    public:
                        virtual void                            Activated (Context& r) override
                        {
                            Assert (fActualReader_ == nullptr);
                            fActualReader_ = r.GetObjectReaderRegistry ().MakeContextReader (&fProxyValue_);
                            fActualReader_->Activated (r);
                        }
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override
                        {
                            AssertNotNull (fActualReader_);
                            return fActualReader_->HandleChildStart (name);
                        }
                        virtual void                            HandleTextInside (const String& text) override
                        {
                            AssertNotNull (fActualReader_);
                            fActualReader_->HandleTextInside (text);
                        }
                        virtual void                            Deactivating () override
                        {
                            AssertNotNull (fActualReader_);
                            fActualReader_->Deactivating ();
                            fActualReader_.reset ();
                            *fValue_ = fProxyValue_;
                        }

                    public:
                        /**
                         *  Helper to convert a reader to a factory (something that creates the reader).
                         */
                        static  ReaderFromVoidStarFactory   AsFactory ()
                        {
                            return IElementConsumer::AsFactory<Memory::Optional<T>, OptionalTypesReader_> ();
                        }

                    private:
                        Memory::Optional<T>*            fValue_{};
                        T                               fProxyValue_{};
                        shared_ptr<IElementConsumer>    fActualReader_{};
                    };


                    /*
                     ********************************************************************************
                     ********************** IElementConsumer ******************
                     ********************************************************************************
                     */
                    template    <typename TARGET_TYPE, typename READER, typename... ARGS>
                    ReaderFromVoidStarFactory IElementConsumer::AsFactory (ARGS&& ... args)
                    {
                        return Registry::ConvertReaderToFactory<TARGET_TYPE, READER> (forward<ARGS> (args)...);
                    }


                    /*
                     ********************************************************************************
                     ********************** Registry::Context ***************************
                     ********************************************************************************
                     */
                    inline   Context::Context (const Registry& registry)
                        : fObjectReaderRegistry_ (registry)
                    {
                    }
                    inline   Context::Context (Context&& from)
                        : fObjectReaderRegistry_ (from.fObjectReaderRegistry_)
                        , fStack_ (move (from.fStack_))
                    {
                    }
                    inline  void    Context::Push (const shared_ptr<IElementConsumer>& elt)
                    {
                        RequireNotNull (elt);
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
                        if (fTraceThisReader) {
                            DbgTrace (L"%sContext::Push [%s]", TraceLeader_ ().c_str (), Characters::ToString (typeid (*elt.get ())).c_str ());
                        }
#endif
                        Containers::ReserveSpeedTweekAdd1 (fStack_);
                        fStack_.push_back (elt);
                        elt->Activated (*this);
                    }
                    inline  void    Context::Pop ()
                    {
                        fStack_.back()->Deactivating ();
                        fStack_.pop_back ();
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
                        if (fTraceThisReader) {
                            if (fStack_.empty ()) {
                                DbgTrace (L"%sContext::Popped [empty stack]", TraceLeader_ ().c_str ());
                            }
                            else {
                                DbgTrace (L"%sContext::Popped [back to: %s]", TraceLeader_ ().c_str (), Characters::ToString (typeid (*GetTop ().get ())).c_str ());
                            }
                        }
#endif
                    }
                    inline  shared_ptr<IElementConsumer>  Context::GetTop () const
                    {
                        Require (not fStack_.empty ());
                        return fStack_.back ();
                    }
                    inline  const   Registry&   Context::GetObjectReaderRegistry () const
                    {
                        return fObjectReaderRegistry_;
                    }
                    inline  bool    Context::empty () const
                    {
                        return fStack_.empty ();
                    }


                    /*
                     ********************************************************************************
                     ************* ObjectReaderRegistry::IConsumerDelegateToContext *****************
                     ********************************************************************************
                     */
                    inline  IConsumerDelegateToContext::IConsumerDelegateToContext (Context&& r)
                        : fContext (move (r))
                    {
                    }
                    inline  IConsumerDelegateToContext::IConsumerDelegateToContext (const Registry& registry)
                        : fContext (registry)
                    {
                    }
                    inline  IConsumerDelegateToContext::IConsumerDelegateToContext (const Registry& registry, const shared_ptr<IElementConsumer>& initialTop)
                        : fContext (registry, initialTop)
                    {
                    }


                    /*
                     ********************************************************************************
                     ************** ObjectReaderRegistry::StructFieldInfo ***************************
                     ********************************************************************************
                     */
                    inline  StructFieldInfo::StructFieldInfo (const Name& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo)
                        : fSerializedFieldName (serializedFieldName)
                        , fFieldMetaInfo (fieldMetaInfo)
                    {
                    }


                    /*
                     ********************************************************************************
                     ************************************ ClassReader *******************************
                     ********************************************************************************
                     */
                    template    <typename   T>
                    ClassReader<T>::ClassReader (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, T* vp)
                        : fValuePtr_ (vp)
                    {
                        RequireNotNull (vp);
                        for (StructFieldInfo i : fieldDescriptions) {
                            if (i.fSerializedFieldName.fType == Name::eValue) {
                                fValueFieldMetaInfo_ = i.fFieldMetaInfo;
                            }
                            else {
                                fFieldNameToTypeMap_.Add (i.fSerializedFieldName, i.fFieldMetaInfo);
                            }
                        }
                    }
                    template    <typename   T>
                    void    ClassReader<T>::Activated (Context& r)
                    {
                        Require (fActiveContext_ == nullptr);
                        fActiveContext_ = &r;
                    }
                    template    <typename   T>
                    shared_ptr<IElementConsumer>    ClassReader<T>::HandleChildStart (const Name& name)
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
                    void    ClassReader<T>::HandleTextInside (const String& text)
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
                    void    ClassReader<T>::Deactivating ()
                    {
                        RequireNotNull (fActiveContext_);
                        if (fValueFieldConsumer_) {
                            fValueFieldConsumer_->Deactivating ();
                        }
                        fActiveContext_ = nullptr;
                    }
                    template    <typename   T>
                    inline  ReaderFromVoidStarFactory   ClassReader<T>::AsFactory ()
                    {
                        return IElementConsumer::AsFactory<T, ClassReader> ();
                    }


                    /*
                     ********************************************************************************
                     ******************* ObjectReaderRegistry::ListOfObjectReader *******************
                     ********************************************************************************
                     */
                    template    <typename CONTAINER_OF_T, typename TRAITS>
                    inline  ListOfObjectReader<CONTAINER_OF_T, TRAITS>::ListOfObjectReader (CONTAINER_OF_T* v)
                        : fValuePtr_ (v)
                    {
                        WeakAssert (false); // class deprecated
                        RequireNotNull (v);
                    }
                    template    <typename CONTAINER_OF_T, typename TRAITS>
                    inline  ListOfObjectReader<CONTAINER_OF_T, TRAITS>::ListOfObjectReader (CONTAINER_OF_T* v, const Name& memberElementName)
                        : fMemberElementName_  (memberElementName)
                        , fValuePtr_ (v)
                    {
                        WeakAssert (false); // class deprecated
                        RequireNotNull (v);
                    }
                    template    <typename CONTAINER_OF_T, typename TRAITS>
                    void    ListOfObjectReader<CONTAINER_OF_T, TRAITS>::Activated (Context& r)
                    {
                        Require (fActiveContext_ == nullptr);
                        fActiveContext_ = &r;
                    }
                    template    <typename CONTAINER_OF_T, typename TRAITS>
                    shared_ptr<IElementConsumer> ListOfObjectReader<CONTAINER_OF_T, TRAITS>::HandleChildStart (const StructuredStreamEvents::Name& name)
                    {
                        RequireNotNull (fActiveContext_);
                        if (fMemberElementName_.IsMissing () or name == *fMemberElementName_) {
                            if (fReadingAT_) {
                                TRAITS::ContainerAdapterAdder::Add (this->fValuePtr_, fCurTReading_);
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
                    template    <typename CONTAINER_OF_T, typename TRAITS>
                    void    ListOfObjectReader<CONTAINER_OF_T, TRAITS>::Deactivating ()
                    {
                        RequireNotNull (fActiveContext_);
                        if (fReadingAT_) {
                            TRAITS::ContainerAdapterAdder::Add (this->fValuePtr_, fCurTReading_);
                            fReadingAT_ = false;
                        }
                        fActiveContext_ = nullptr;
                    }
                    template    <typename CONTAINER_OF_T, typename TRAITS>
                    inline  ReaderFromVoidStarFactory   ListOfObjectReader<CONTAINER_OF_T, TRAITS>::AsFactory ()
                    {
                        return IElementConsumer::AsFactory<CONTAINER_OF_T, ListOfObjectReader> ();
                    }
                    template    <typename CONTAINER_OF_T, typename TRAITS>
                    inline  ReaderFromVoidStarFactory   ListOfObjectReader<CONTAINER_OF_T, TRAITS>::AsFactory (const Name& memberElementName)
                    {
                        return IElementConsumer::AsFactory<CONTAINER_OF_T, ListOfObjectReader> (memberElementName);
                    }


                    /*
                     ********************************************************************************
                     ****************** ObjectReaderRegistry::ListOfObjectsReader_NEW ***************
                     ********************************************************************************
                     */
                    template    <typename CONTAINER_OF_T>
                    inline  ListOfObjectsReader_NEW<CONTAINER_OF_T>::ListOfObjectsReader_NEW (CONTAINER_OF_T* v)
                        : fValuePtr_ (v)
                    {
                        RequireNotNull (v);
                    }
                    template    <typename CONTAINER_OF_T>
                    inline  ListOfObjectsReader_NEW<CONTAINER_OF_T>::ListOfObjectsReader_NEW (CONTAINER_OF_T* v, const Name& memberElementName)
                        : fValuePtr_ (v)
                        , fMemberElementName_  (memberElementName)
                    {
                        RequireNotNull (v);
                    }
                    template    <typename CONTAINER_OF_T>
                    void    ListOfObjectsReader_NEW<CONTAINER_OF_T>::Activated (Context& r)
                    {
                        Require (fActiveContext_ == nullptr);
                        fActiveContext_ = &r;
                    }
                    template    <typename CONTAINER_OF_T>
                    shared_ptr<IElementConsumer> ListOfObjectsReader_NEW<CONTAINER_OF_T>::HandleChildStart (const StructuredStreamEvents::Name& name)
                    {
                        RequireNotNull (fActiveContext_);
                        if (fMemberElementName_.IsMissing () or name == *fMemberElementName_) {
                            return make_shared <RepeatedElementReader<CONTAINER_OF_T>> (fValuePtr_);
                        }
                        else if (fThrowOnUnrecongizedelts_) {
                            ThrowUnRecognizedStartElt (name);
                        }
                        else {
                            return make_shared<IgnoreNodeReader> ();
                        }
                    }
                    template    <typename CONTAINER_OF_T>
                    void    ListOfObjectsReader_NEW<CONTAINER_OF_T>::Deactivating ()
                    {
                        RequireNotNull (fActiveContext_);
                        fActiveContext_ = nullptr;
                    }
                    template    <typename CONTAINER_OF_T>
                    inline  ReaderFromVoidStarFactory   ListOfObjectsReader_NEW<CONTAINER_OF_T>::AsFactory ()
                    {
                        return IElementConsumer::AsFactory<CONTAINER_OF_T, ListOfObjectsReader_NEW> ();
                    }
                    template    <typename CONTAINER_OF_T>
                    inline  ReaderFromVoidStarFactory   ListOfObjectsReader_NEW<CONTAINER_OF_T>::AsFactory (const Name& memberElementName)
                    {
                        return IElementConsumer::AsFactory<CONTAINER_OF_T, ListOfObjectsReader_NEW> (memberElementName);
                    }


                    /*
                     ********************************************************************************
                     ********************* ObjectReaderRegistry::MixinReader ************************
                     ********************************************************************************
                     */
                    template    <typename   T>
                    const function<Byte* (T*)>  MixinReader<T>::MixinEltTraits::kDefaultAddressOfSubElementFetcher = [](T* b) { return reinterpret_cast<Byte*> (b);  };
                    template    <typename   T>
                    inline  MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<Byte* (T*)>& addressOfSubEltFetcher)
                        : fReaderFactory (readerFactory)
                        , fAddressOfSubElementFetcher (addressOfSubEltFetcher)
                    {
                    }
                    template    <typename   T>
                    inline  MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool (const Name& name)>& readsName, const function<Byte* (T*)>& addressOfSubEltFetcher)
                        : fReaderFactory (readerFactory)
                        , fReadsName (readsName)
                        , fAddressOfSubElementFetcher (addressOfSubEltFetcher)
                    {
                    }
                    template    <typename   T>
                    inline  MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool ()>& readsText, const function<Byte* (T*)>& addressOfSubEltFetcher)
                        : fReaderFactory (readerFactory)
                        , fReadsText (readsText)
                        , fAddressOfSubElementFetcher (addressOfSubEltFetcher)
                    {
                    }
                    template    <typename   T>
                    inline  MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool (const Name& name)>& readsName, const function<bool ()>& readsText, const function<Byte* (T*)>& addressOfSubEltFetcher)
                        : fReaderFactory (readerFactory)
                        , fReadsName (readsName)
                        , fReadsText (readsText)
                        , fAddressOfSubElementFetcher (addressOfSubEltFetcher)
                    {
                    }

                    template    <typename   T>
                    MixinReader<T>::MixinReader (T* vp, const Traversal::Iterable<MixinEltTraits>& mixins)
                        : fValuePtr_ (vp)
                        , fMixins_ (mixins)
                    {
                        RequireNotNull (vp);
                        for (MixinEltTraits m : mixins) {
                            fMixinReaders_ += m.fReaderFactory (m.fAddressOfSubElementFetcher (vp));
                        }
                    }
                    template    <typename   T>
                    void    MixinReader<T>::Activated (Context& r)
                    {
                        Require (fActiveContext_ == nullptr);
                        fActiveContext_ = &r;
                    }
                    template    <typename   T>
                    shared_ptr<IElementConsumer>    MixinReader<T>::HandleChildStart (const Name& name)
                    {
                        RequireNotNull (fActiveContext_);
                        size_t  idx = 0;
                        for (MixinEltTraits m : fMixins_) {
                            if (m.fReadsName (name)) {
                                shared_ptr<IElementConsumer>    reader = fMixinReaders_[idx];
                                if (not fActivatedReaders_.Contains (reader)) {
                                    reader->Activated (*fActiveContext_);
                                    fActivatedReaders_.Add (reader);
                                }
                                return reader->HandleChildStart (name);
                            }
                            idx++;
                        }
                        return make_shared<IgnoreNodeReader> ();
                    }
                    template    <typename   T>
                    void    MixinReader<T>::HandleTextInside (const String& text)
                    {
                        RequireNotNull (fActiveContext_);
                        size_t  idx = 0;
                        for (MixinEltTraits m : fMixins_) {
                            if (m.fReadsText ()) {
                                shared_ptr<IElementConsumer>    reader = fMixinReaders_[idx];
                                if (not fActivatedReaders_.Contains (reader)) {
                                    reader->Activated (*fActiveContext_);
                                    fActivatedReaders_.Add (reader);
                                }
                                fMixinReaders_[idx]->HandleTextInside (text);
                            }
                            idx++;
                        }
                    }
                    template    <typename   T>
                    void    MixinReader<T>::Deactivating ()
                    {
                        RequireNotNull (fActiveContext_);
                        for (shared_ptr<IElementConsumer> reader : fActivatedReaders_) {
                            reader->Deactivating ();
                        }
                        fActiveContext_ = nullptr;
                    }
                    template    <typename   T>
                    inline  ReaderFromVoidStarFactory   MixinReader<T>::AsFactory (const Traversal::Iterable<MixinEltTraits>& mixins)
                    {
                        return IElementConsumer::AsFactory<T, MixinReader> (mixins);
                    }


                    /*
                     ********************************************************************************
                     ********************* ObjectReaderRegistry::RangeReader ************************
                     ********************************************************************************
                     */
                    template    <typename   T>
                    const   pair<Name, Name>    RangeReader<T>::kDefaultBoundsNames{ Name { Characters::String_Constant { L"LowerBound" }, Name::eAttribute }, Name { Characters::String_Constant { L"UpperBound" }, Name::eAttribute } };
                    template    <typename   T>
                    RangeReader<T>::RangeReader (T* intoVal, const pair<Name, Name>& pairNames)
                        : fPairNames (pairNames)
                        , fValue_ (intoVal)
                    {
                        RequireNotNull (intoVal);
                    }
                    template    <typename   T>
                    void                            RangeReader<T>::Activated (Context& r)
                    {
                        Assert (fActualReader_ == nullptr);
                        fActualReader_ = Registry::MakeClassReader<RangeData_> (
                        initializer_list<StructFieldInfo> {
#if     qCompilerAndStdLib_StructFieldMetaInfoOfNestedClassInTemplate_Buggy
                            { fPairNames.first, StructFieldMetaInfo { offsetof (RangeData_, fLowerBound), typeid (range_value_type_) } },
                            { fPairNames.second, StructFieldMetaInfo { offsetof (RangeData_, fUpperBound), typeid (range_value_type_) } },
#else
                            { fPairNames.first, Stroika_Foundation_DataExchange_StructFieldMetaInfo (RangeData_, fLowerBound) },
                            { fPairNames.second, Stroika_Foundation_DataExchange_StructFieldMetaInfo (RangeData_, fUpperBound) },
#endif
                        }
                                         ) (&fProxyValue_);
                        fActualReader_->Activated (r);
                    }
                    template    <typename   T>
                    shared_ptr<IElementConsumer>    RangeReader<T>::HandleChildStart (const Name& name)
                    {
                        AssertNotNull (fActualReader_);
                        return fActualReader_->HandleChildStart (name);
                    }
                    template    <typename   T>
                    void    RangeReader<T>::HandleTextInside (const String& text)
                    {
                        AssertNotNull (fActualReader_);
                        fActualReader_->HandleTextInside (text);
                    }
                    template    <typename   T>
                    void    RangeReader<T>::Deactivating ()
                    {
                        AssertNotNull (fActualReader_);
                        fActualReader_->Deactivating ();
                        fActualReader_.reset ();
                        *fValue_ = T{ fProxyValue_.fLowerBound, fProxyValue_.fUpperBound };
                    }
                    template    <typename   T>
                    ReaderFromVoidStarFactory   RangeReader<T>::AsFactory (const pair<Name, Name>& pairNames)
                    {
                        return Registry::ConvertReaderToFactory<T, RangeReader> (pairNames);
                    }


                    /*
                     ********************************************************************************
                     ****************** ObjectReaderRegistry::IgnoreNodeReader **********************
                     ********************************************************************************
                     */
                    inline  ReaderFromVoidStarFactory   IgnoreNodeReader::AsFactory ()
                    {
                        return [](void*) -> shared_ptr<IElementConsumer> { return make_shared<IgnoreNodeReader> (); };
                    }


                    /*
                     ********************************************************************************
                     ****************** ObjectReaderRegistry::ReadDownToReader **********************
                     ********************************************************************************
                     */
                    inline  ReaderFromVoidStarFactory   ReadDownToReader::AsFactory (const ReaderFromVoidStarFactory& theUseReader)
                    {
                        return [theUseReader](void* p) -> shared_ptr<IElementConsumer> { return make_shared<ReadDownToReader> (theUseReader (p)); };
                    }
                    inline  ReaderFromVoidStarFactory   ReadDownToReader::AsFactory (const ReaderFromVoidStarFactory& theUseReader, const Name& tagToHandOff)
                    {
                        return [theUseReader, tagToHandOff](void* p) -> shared_ptr<IElementConsumer> { return make_shared<ReadDownToReader> (theUseReader (p), tagToHandOff); };
                    }


                    /*
                     ********************************************************************************
                     ************** ObjectReaderRegistry::RepeatedElementReader *********************
                     ********************************************************************************
                     */
                    template    <typename T, typename TRAITS>
                    inline  RepeatedElementReader<T, TRAITS>::RepeatedElementReader (ContainerType* v)
                        : fValuePtr_ (v)
                    {
                    }
                    template    <typename T, typename TRAITS>
                    inline  RepeatedElementReader<T, TRAITS>::RepeatedElementReader (ContainerType* pv, const ReaderFromVoidStarFactory& actualElementFactory)
                        : fValuePtr_ (pv)
                        , fReaderRactory_ (actualElementFactory)
                    {
                    }
                    template    <typename T, typename TRAITS>
                    inline  RepeatedElementReader<T, TRAITS>::RepeatedElementReader (ContainerType* v, const Name& readonlyThisName, const ReaderFromVoidStarFactory& actualElementFactory)
                        : fValuePtr_ (v)
                        , fReaderRactory_ (actualElementFactory)
                        , fReadThisName_ ([readonlyThisName] (const Name & n) { return n == readonlyThisName;  } )
                    {
                    }
                    template    <typename T, typename TRAITS>
                    inline  RepeatedElementReader<T, TRAITS>::RepeatedElementReader (ContainerType* v, const Name& readonlyThisName)
                        : fValuePtr_ (v)
                        , fReadThisName_ ([readonlyThisName] (const Name & n) { return n == readonlyThisName;  } )
                    {
                    }
                    template    <typename T, typename TRAITS>
                    void    RepeatedElementReader<T, TRAITS>::Activated (Context& r)
                    {
                        Assert (fActiveSubReader_ == nullptr);
                        // @todo - DEPRECATED USE OF TRAITS - REPLACE THIS WITH using context to create type
                        fActiveSubReader_ = fReaderRactory_ ? (*fReaderRactory_) (&fProxyValue_) : TRAITS::MakeActualReader (r, &fProxyValue_);
                        fActiveSubReader_->Activated (r);
                    }
                    template    <typename T, typename TRAITS>
                    shared_ptr<IElementConsumer>    RepeatedElementReader<T, TRAITS>::HandleChildStart (const Name& name)
                    {
                        AssertNotNull (fActiveSubReader_);
                        if (fReadThisName_ (name)) {
                            return fActiveSubReader_->HandleChildStart (name);
                        }
#if 0
                        // not sure we want this
                        else if (fThrowOnUnrecongizedelts_) {
                            ThrowUnRecognizedStartElt (name);
                        }
#endif
                        else {
                            return make_shared<IgnoreNodeReader> ();
                        }
                    }
                    template    <typename T, typename TRAITS>
                    void    RepeatedElementReader<T, TRAITS>::HandleTextInside (const String& text)
                    {
                        AssertNotNull (fActiveSubReader_);
                        fActiveSubReader_->HandleTextInside (text);
                    }
                    template    <typename T, typename TRAITS>
                    void    RepeatedElementReader<T, TRAITS>::Deactivating ()
                    {
                        AssertNotNull (fActiveSubReader_);
                        fActiveSubReader_->Deactivating ();
                        fActiveSubReader_.reset ();
                        TRAITS::ContainerAdapterAdder::Add (fValuePtr_, fProxyValue_);
                    }
                    template    <typename T, typename TRAITS>
                    inline  ReaderFromVoidStarFactory   RepeatedElementReader<T, TRAITS>::AsFactory ()
                    {
                        return IElementConsumer::AsFactory<T, RepeatedElementReader> ();
                    }
                    template    <typename T, typename TRAITS>
                    inline  ReaderFromVoidStarFactory   RepeatedElementReader<T, TRAITS>::AsFactory (const Name& readonlyThisName, const ReaderFromVoidStarFactory& actualElementFactory)
                    {
                        return IElementConsumer::AsFactory<T, RepeatedElementReader> (readonlyThisName, actualElementFactory);
                    }
                    template    <typename T, typename TRAITS>
                    inline  ReaderFromVoidStarFactory   RepeatedElementReader<T, TRAITS>::AsFactory (const ReaderFromVoidStarFactory& actualElementFactory)
                    {
                        return IElementConsumer::AsFactory<T, RepeatedElementReader> (actualElementFactory);
                    }
                    template    <typename T, typename TRAITS>
                    inline  ReaderFromVoidStarFactory   RepeatedElementReader<T, TRAITS>::AsFactory (const Name& readonlyThisName)
                    {
                        return IElementConsumer::AsFactory<T, RepeatedElementReader> (readonlyThisName);
                    }


                    /*
                     ********************************************************************************
                     ***************************** ObjectReaderRegistry *****************************
                     ********************************************************************************
                     */
                    inline void    Registry::Add (type_index forType, const ReaderFromVoidStarFactory& readerFactory)
                    {
                        fFactories_.Add (forType, readerFactory);
                    }
                    template    <typename T>
                    void    Registry::Add (const ReaderFromTStarFactory<T>& readerFactory)
                    {
                        Add (typeid (T), [readerFactory] (void* data) { return readerFactory (reinterpret_cast<T*> (data)); });
                    }
                    inline shared_ptr<IElementConsumer>    Registry::MakeContextReader (type_index ti, void* destinationObject) const
                    {
#if     qDebug
                        if (not fFactories_.ContainsKey (ti)) {
                            Debug::TraceContextBumper   ctx ("Registry::MakeContextReader");
                            DbgTrace (L"(forTypeInfo = %s) - UnRegistered Type!", Characters::ToString (ti).c_str ());
                        }
#endif
                        ReaderFromVoidStarFactory  factory = *fFactories_.Lookup (ti); // must be found or caller/assert error
                        return factory (destinationObject);
                    }
                    template    <typename T>
                    inline  shared_ptr<IElementConsumer>    Registry::MakeContextReader (T* destinationObject) const
                    {
                        return MakeContextReader (typeid (T), destinationObject);
                    }
                    template    <typename T, typename... ARGS>
                    inline  void    Registry::AddCommonType (ARGS&& ... args)
                    {
                        Add<T> (MakeCommonReader<T> (forward<ARGS> (args)...));
                    }
                    inline  shared_ptr<ReadDownToReader>      Registry::mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader)
                    {
                        return make_shared<ReadDownToReader> (theUseReader);
                    }
                    inline  shared_ptr<ReadDownToReader>      Registry::mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff)
                    {
                        return make_shared<ReadDownToReader> (theUseReader, tagToHandOff);
                    }
                    inline  shared_ptr<ReadDownToReader>      Registry::mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& contextTag, const Name& tagToHandOff)
                    {
                        return make_shared<ReadDownToReader> (theUseReader, contextTag, tagToHandOff);
                    }
                    template    <typename CLASS>
                    void    Registry::AddClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions)
                    {
#if     qDebug
                        for (auto kv : fieldDescriptions) {
                            if (not fFactories_.ContainsKey (kv.fFieldMetaInfo.fTypeInfo)) {
                                Debug::TraceContextBumper   ctx ("Registry::AddClass");
                                DbgTrace (L"(CLASS=%s field-TypeInfo-not-found = %s, for field named '%s') - UnRegistered Type!", Characters::ToString (typeid (CLASS)).c_str (), Characters::ToString (kv.fFieldMetaInfo.fTypeInfo).c_str (), Characters::ToString (kv.fSerializedFieldName).c_str ());
                                RequireNotReached ();
                            }
                        }
#endif
                        Add<CLASS> (MakeClassReader<CLASS> (fieldDescriptions));
                    }
                    template    <typename CLASS>
                    // _Deprecated_ ("USE AddClass(initializer_list<StructFieldInfo>)- deprecated v2.0a189");
                    nonvirtual  void    Registry::AddClass (const initializer_list<pair<Name, StructFieldMetaInfo>>& fieldDescriptions)
                    {
                        Containers::Sequence<StructFieldInfo>   tmp;
                        for (auto i : fieldDescriptions) {
                            tmp += StructFieldInfo{ i };
                        }
                        AddClass<CLASS> (tmp);
                    }
                    template    <typename CLASS>
                    auto    Registry::MakeClassReader (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions) -> ReaderFromVoidStarFactory {
                        return [fieldDescriptions] (void* data) -> shared_ptr<IElementConsumer> { return make_shared<ClassReader<CLASS>> (fieldDescriptions, reinterpret_cast<CLASS*> (data)); };
                    }
                    template    <typename CLASS>
                    // _Deprecated_ ("USE MakeClassReader(initializer_list<StructFieldInfo>)- deprecated v2.0a189")
                    ReaderFromVoidStarFactory    Registry::MakeClassReader (const initializer_list<pair<Name, StructFieldMetaInfo>>& fieldDescriptions)
                    {
                        Containers::Sequence<StructFieldInfo>   tmp;
                        for (auto i : fieldDescriptions) {
                            tmp += StructFieldInfo{ i };
                        }
                        return MakeClassReader<CLASS> (tmp);
                    }
                    template    <typename T, typename READER, typename... ARGS>
                    auto    Registry::ConvertReaderToFactory (ARGS&& ... args) -> ReaderFromVoidStarFactory {
                        ReaderFromTStarFactory<T>   tmpFactory      { [args...] (T * o) -> shared_ptr<IElementConsumer> { return make_shared<READER> (o, forward<ARGS> (args)...); } };
                        return [tmpFactory] (void* data) { return tmpFactory (reinterpret_cast<T*> (data)); };
                    }
                    template    <typename T>
                    auto   Registry::cvtFactory_ (const ReaderFromTStarFactory<T>& tf) -> ReaderFromVoidStarFactory {
                        return [tf] (void* data) { return tf (reinterpret_cast<T*> (data)); };
                    }
                    template    <typename T>
                    auto   Registry::MakeCommonReader_SimpleReader_ () -> ReaderFromVoidStarFactory {
                        return cvtFactory_<T> ([] (T * o) -> shared_ptr<IElementConsumer> { return make_shared<SimpleReader_<T>> (o); });
                    }
                    template    <typename ENUM_TYPE>
                    auto  Registry::MakeCommonReader_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap) -> ReaderFromVoidStarFactory {
                        struct   myReader_ : public IElementConsumer {
                            Containers::Bijection<ENUM_TYPE, String>    fNameMap;
                            myReader_ (const Containers::Bijection<ENUM_TYPE, String>& nameMap, ENUM_TYPE * intoVal)
                                : fNameMap (nameMap)
                                , fValue_ (intoVal)
                            {
                                RequireNotNull (intoVal);
                            }
                            Characters::StringBuilder   fBuf_   {};
                            ENUM_TYPE*                  fValue_ {};
                            virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name & name) override
                            {
                                ThrowUnRecognizedStartElt (name);
                            }
                            virtual void                            HandleTextInside (const String & text) override
                            {
                                fBuf_ += text;
                            }
                            virtual void                            Deactivating () override
                            {
                                if (auto optVal = fNameMap.InverseLookup (fBuf_.str ()))
                                {
                                    *fValue_ = *optVal;
                                }
                                else
                                {
                                    DbgTrace (L"Enumeration ('%s') value '%s' out of range", Characters::ToString (typeid (ENUM_TYPE)).c_str (), fBuf_.str ().c_str ());
                                    Execution::Throw (BadFormatException (Characters::String_Constant { L"Enumeration value out of range" }));
                                }
                            }
                        };
                        return cvtFactory_<ENUM_TYPE> ([nameMap] (ENUM_TYPE * o) -> shared_ptr<IElementConsumer> { return make_shared<myReader_> (nameMap, o); });
                    };
                    template    <typename ENUM_TYPE>
                    auto  Registry::MakeCommonReader_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap) -> ReaderFromVoidStarFactory {
                        return MakeCommonReader_NamedEnumerations (Containers::Bijection<ENUM_TYPE, String> (nameMap));
                    }
                    template    <typename ENUM_TYPE>
                    auto  Registry::MakeCommonReader_EnumAsInt () -> ReaderFromVoidStarFactory {
                        struct   myReader_ : public IElementConsumer {
                            myReader_ (ENUM_TYPE * intoVal)
                                : fValue_ (intoVal)
                            {
                                RequireNotNull (intoVal);
                            }
                            Characters::StringBuilder   fBuf_   {};
                            ENUM_TYPE*                  fValue_ {};
                            virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name & name) override
                            {
                                ThrowUnRecognizedStartElt (name);
                            }
                            virtual void                            HandleTextInside (const String & text) override
                            {
                                fBuf_ += text;
                            }
                            virtual void                            Deactivating () override
                            {
                                using   SerializeAsType     =   typename std::underlying_type<ENUM_TYPE>::type;
                                SerializeAsType tmp     =   Characters::String2Int<SerializeAsType> (fBuf_.str ());
                                if (Configuration::ToInt (ENUM_TYPE::eSTART) <= tmp and tmp < Configuration::ToInt (ENUM_TYPE::eEND))
                                {
                                    *fValue_ = Configuration::ToEnum<ENUM_TYPE> (tmp);
                                }
                                else
                                {
                                    DbgTrace (L"Enumeration ('%s') value '%s' out of range", Characters::ToString (typeid (ENUM_TYPE)).c_str (), fBuf_.str ().c_str ());
                                    Execution::Throw (BadFormatException (Characters::String_Constant{ L"Enumeration value out of range" }));
                                }
                            }
                        };
                        return cvtFactory_<ENUM_TYPE> ([] (ENUM_TYPE * o) -> shared_ptr<IElementConsumer> { return make_shared<myReader_> (o); });
                    }
                    inline  ReaderFromVoidStarFactory   Registry::MakeCommonReader_ (const String*)
                    {
                        return MakeCommonReader_SimpleReader_<String> ();
                    }
                    template    <typename T>
                    inline  ReaderFromVoidStarFactory   Registry::MakeCommonReader_ (const T*, typename std::enable_if<std::is_enum<T>::value >::type*)
                    {
                        return MakeCommonReader_NamedEnumerations<T> ();
                    }
                    template    <typename T>
                    inline  ReaderFromVoidStarFactory   Registry::MakeCommonReader_ (const T*, typename std::enable_if < std::is_pod<T>::value and !std::is_enum<T>::value >::type*)
                    {
                        return MakeCommonReader_SimpleReader_<T> ();
                    }
                    inline  ReaderFromVoidStarFactory   Registry::MakeCommonReader_ (const Time::DateTime*)
                    {
                        return MakeCommonReader_SimpleReader_<Time::DateTime> ();
                    }
                    inline  ReaderFromVoidStarFactory   Registry::MakeCommonReader_ (const Time::Duration*)
                    {
                        return MakeCommonReader_SimpleReader_<Time::Duration> ();
                    }
                    template    <typename T, typename TRAITS>
                    inline   ReaderFromVoidStarFactory  Registry::MakeCommonReader_ (const Memory::Optional<T, TRAITS>*)
                    {
                        return cvtFactory_<Memory::Optional<T>> ([] (Memory::Optional<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<OptionalTypesReader_<T>> (o); });
                    }
                    template    <typename T>
                    ReaderFromVoidStarFactory  Registry::MakeCommonReader_ (const vector<T>*)
                    {
                        return cvtFactory_<vector<T>> ([] (vector<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectsReader_NEW<vector<T>>> (o); });
                    }
                    template    <typename T>
                    ReaderFromVoidStarFactory  Registry::MakeCommonReader_ (const vector<T>*, const Name& name)
                    {
                        return cvtFactory_<vector<T>> ([name] (vector<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectsReader_NEW<vector<T>>> (o, name); });
                    }
                    template    <typename T>
                    ReaderFromVoidStarFactory  Registry::MakeCommonReader_ (const Sequence<T>*)
                    {
                        return cvtFactory_<Sequence<T>> ([] (Sequence<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectsReader_NEW<Sequence<T>>> (o); });
                    }
                    template    <typename T>
                    ReaderFromVoidStarFactory  Registry::MakeCommonReader_ (const Sequence<T>*, const Name& name)
                    {
                        return cvtFactory_<Sequence<T>> ([name] (Sequence<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectsReader_NEW<Sequence<T>>> (o, name); });
                    }
                    template    <typename T, typename... ARGS>
                    inline  ReaderFromVoidStarFactory  Registry::MakeCommonReader (ARGS&& ... args)
                    {
                        const T*  n = nullptr;    // arg unused, just for overloading
                        DISABLE_COMPILER_MSC_WARNING_START (6011)
                        return MakeCommonReader_ (n, forward<ARGS> (args)...);
                        DISABLE_COMPILER_MSC_WARNING_END (6011)
                    }


                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_inl_*/
