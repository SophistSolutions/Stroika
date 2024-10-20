/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"

namespace Stroika::Foundation::DataExchange::StructuredStreamEvents::ObjectReader {

    /**
     *  [private]
     *  SimpleReader_<> is not implemented for all types - just for the with Deactivating specialized below;
     *
     *  The class (template) generically accumulates the text from inside the element, but then the Deactivating () override must
     *  be specialized for each class to 'convert' from the accumulated string to the fValue.
     */
    template <typename T>
    class Registry::SimpleReader_ : public IElementConsumer {
    public:
        SimpleReader_ (T* intoVal)
            : fValue_{intoVal}
        {
            RequireNotNull (intoVal);
        }

    public:
        virtual void Activated ([[maybe_unused]] Context& r) override
        {
            fBuf_.clear (); // readers can legally be re-used
        }
        virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override
        {
            ThrowUnRecognizedStartElt (name);
        }
        virtual void HandleTextInside (const String& text) override
        {
            fBuf_ += text;
        }
        virtual void Deactivating () override;

    public:
        /**
         *  Helper to convert a reader to a factory (something that creates the reader).
         */
        static ReaderFromVoidStarFactory AsFactory ()
        {
            return IElementConsumer::AsFactory<T, SimpleReader_> ();
        }

    private:
        Characters::StringBuilder<> fBuf_{};
        T*                          fValue_{};
    };
    template <>
    void Registry::SimpleReader_<String>::Deactivating ();
    template <>
    void Registry::SimpleReader_<char>::Deactivating ();
    template <>
    void Registry::SimpleReader_<unsigned char>::Deactivating ();
    template <>
    void Registry::SimpleReader_<short>::Deactivating ();
    template <>
    void Registry::SimpleReader_<unsigned short>::Deactivating ();
    template <>
    void Registry::SimpleReader_<int>::Deactivating ();
    template <>
    void Registry::SimpleReader_<unsigned int>::Deactivating ();
    template <>
    void Registry::SimpleReader_<long int>::Deactivating ();
    template <>
    void Registry::SimpleReader_<unsigned long int>::Deactivating ();
    template <>
    void Registry::SimpleReader_<long long int>::Deactivating ();
    template <>
    void Registry::SimpleReader_<unsigned long long int>::Deactivating ();
    template <>
    void Registry::SimpleReader_<bool>::Deactivating ();
    template <>
    void Registry::SimpleReader_<float>::Deactivating ();
    template <>
    void Registry::SimpleReader_<double>::Deactivating ();
    template <>
    void Registry::SimpleReader_<long double>::Deactivating ();
    template <>
    void Registry::SimpleReader_<Time::DateTime>::Deactivating ();
    template <>
    void Registry::SimpleReader_<Time::Duration>::Deactivating ();

    /*
     ********************************************************************************
     ********************************** IElementConsumer ****************************
     ********************************************************************************
     */
    template <typename TARGET_TYPE, typename READER, typename... ARGS>
    inline ReaderFromVoidStarFactory IElementConsumer::AsFactory (ARGS&&... args)
    {
        return Registry::ConvertReaderToFactory<TARGET_TYPE, READER> (forward<ARGS> (args)...);
    }

    /*
     ********************************************************************************
     ******************************** Registry::Context *****************************
     ********************************************************************************
     */
    inline Context::Context (const Registry& registry)
        : fObjectReaderRegistry_{registry}
    {
    }
    inline Context::Context (Context&& from) noexcept
        : fObjectReaderRegistry_{from.fObjectReaderRegistry_}
        , fStack_{move (from.fStack_)}
    {
    }
    inline void Context::Push (const shared_ptr<IElementConsumer>& elt)
    {
        using namespace Characters::Literals;
        RequireNotNull (elt);
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
        if (fTraceThisReader) {
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wpotentially-evaluated-expression\"");
            DbgTrace ("{}Context::Push [{}]"_f, TraceLeader_ (), type_index{typeid (*elt.get ())});
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wpotentially-evaluated-expression\"");
        }
#endif
        Containers::Support::ReserveTweaks::Reserve4Add1 (fStack_);
        fStack_.push_back (elt);
        elt->Activated (*this);
    }
    inline void Context::Pop ()
    {
        using namespace Characters::Literals;
        fStack_.back ()->Deactivating ();
        fStack_.pop_back ();
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
        if (fTraceThisReader) {
            if (fStack_.empty ()) {
                DbgTrace ("{}Context::Popped [empty stack]"_f, TraceLeader_ ());
            }
            else {
                DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wpotentially-evaluated-expression\"");
                DbgTrace (L"{}Context::Popped [back to: {}]"_f, TraceLeader_ (), type_index{typeid (*GetTop ().get ())});
                DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wpotentially-evaluated-expression\"");
            }
        }
#endif
    }
    inline shared_ptr<IElementConsumer> Context::GetTop () const
    {
        Require (not fStack_.empty ());
        return fStack_.back ();
    }
    inline const Registry& Context::GetObjectReaderRegistry () const
    {
        return fObjectReaderRegistry_;
    }
    inline bool Context::empty () const
    {
        return fStack_.empty ();
    }

    /*
     ********************************************************************************
     ************* ObjectReaderRegistry::IConsumerDelegateToContext *****************
     ********************************************************************************
     */
    inline IConsumerDelegateToContext::IConsumerDelegateToContext (Context&& r)
        : fContext{move (r)}
    {
    }
    inline IConsumerDelegateToContext::IConsumerDelegateToContext (const Registry& registry)
        : fContext{registry}
    {
    }
    inline IConsumerDelegateToContext::IConsumerDelegateToContext (const Registry& registry, const shared_ptr<IElementConsumer>& initialTop)
        : fContext{registry, initialTop}
    {
    }

    /*
     ********************************************************************************
     ************** ObjectReaderRegistry::StructFieldInfo ***************************
     ********************************************************************************
     */
    inline StructFieldInfo::StructFieldInfo (const Name& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo,
                                             const optional<ReaderFromVoidStarFactory>& typeMapper)
        : fSerializedFieldName{serializedFieldName}
        , fFieldMetaInfo{fieldMetaInfo}
        , fOverrideTypeMapper{typeMapper}
    {
    }

    /*
     ********************************************************************************
     ************************************ ClassReader *******************************
     ********************************************************************************
     */
    template <typename T>
    ClassReader<T>::ClassReader (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, T* vp)
        : fFieldDescriptions_{fieldDescriptions}
        , fValuePtr_{vp}
    {
        RequireNotNull (vp);
        for (const StructFieldInfo& i : fieldDescriptions) {
            if (i.fSerializedFieldName.fType == Name::eValue) {
                fValueFieldMetaInfo_ = i.fFieldMetaInfo;
            }
            else {
                fFieldNameToTypeMap_.Add (i.fSerializedFieldName, i.fFieldMetaInfo);
            }
        }
    }
    template <typename T>
    void ClassReader<T>::Activated (Context& r)
    {
        Require (fActiveContext_ == nullptr);
        fActiveContext_ = &r;
    }
    template <typename T>
    shared_ptr<IElementConsumer> ClassReader<T>::HandleChildStart (const Name& name)
    {
        RequireNotNull (fActiveContext_);
        optional<StructFieldMetaInfo> ti = fFieldNameToTypeMap_.Lookup (name);
        if (ti) {
            byte*                     operatingOnObjField = ti->GetAddressOfMember (this->fValuePtr_);
            ReaderFromVoidStarFactory factory             = LookupFactoryForName_ (name);
            return factory (operatingOnObjField);
        }
        else if (fThrowOnUnrecongizedelts_) {
            ThrowUnRecognizedStartElt (name);
        }
        else {
            return make_shared<IgnoreNodeReader> ();
        }
    }
    template <typename T>
    void ClassReader<T>::HandleTextInside (const String& text)
    {
        RequireNotNull (fActiveContext_);
        if (fValueFieldMetaInfo_) {
            Assert (fValueFieldConsumer_ == nullptr);
            byte*                     operatingOnObjField = fValueFieldMetaInfo_->GetAddressOfMember (this->fValuePtr_);
            ReaderFromVoidStarFactory factory             = LookupFactoryForName_ (Name{Name::NameType::eValue});
            fValueFieldConsumer_                          = (factory)(operatingOnObjField);
            fValueFieldConsumer_->Activated (*fActiveContext_);
            fValueFieldMetaInfo_ = nullopt;
        }
        if (fValueFieldConsumer_) {
            fValueFieldConsumer_->HandleTextInside (text);
        }
    }
    template <typename T>
    void ClassReader<T>::Deactivating ()
    {
        RequireNotNull (fActiveContext_);
        if (fValueFieldConsumer_) {
            fValueFieldConsumer_->Deactivating ();
        }
        fActiveContext_ = nullptr;
    }
    template <typename T>
    inline ReaderFromVoidStarFactory ClassReader<T>::AsFactory ()
    {
        return IElementConsumer::AsFactory<T, ClassReader> ();
    }
    template <typename T>
    ReaderFromVoidStarFactory ClassReader<T>::LookupFactoryForName_ (const Name& name) const
    {
        using namespace Characters::Literals;
        RequireNotNull (fActiveContext_);
        for (const StructFieldInfo& i : fFieldDescriptions_) {
            if (i.fSerializedFieldName == name) {
                if (i.fOverrideTypeMapper) {
                    return *i.fOverrideTypeMapper;
                }
                else {
                    optional<ReaderFromVoidStarFactory> o = fActiveContext_->GetObjectReaderRegistry ().Lookup (i.fFieldMetaInfo.GetTypeInfo ());
                    if constexpr (qDebug) {
                        if (not o.has_value ()) {
                            DbgTrace ("(forTypeInfo = {}) - UnRegistered Type!"_f, i.fFieldMetaInfo);
                            AssertNotReached ();
                        }
                    }
                    return *o;
                }
            }
        }
        AssertNotReached (); // cuz we pre-check - mame must be in passed in args, and we check presence in registry in this proc above
        return nullptr;
    }

    /*
     ********************************************************************************
     ********************* ObjectReaderRegistry::ListOfObjectsReader ****************
     ********************************************************************************
     */
    template <typename CONTAINER_OF_T>
    inline ListOfObjectsReader<CONTAINER_OF_T>::ListOfObjectsReader (CONTAINER_OF_T* v)
        : fValuePtr_{v}
    {
        RequireNotNull (v);
    }
    template <typename CONTAINER_OF_T>
    inline ListOfObjectsReader<CONTAINER_OF_T>::ListOfObjectsReader (CONTAINER_OF_T* v, const Name& memberElementName)
        : fValuePtr_ (v)
        , fMemberElementName_ (memberElementName)
    {
        RequireNotNull (v);
    }
    template <typename CONTAINER_OF_T>
    void ListOfObjectsReader<CONTAINER_OF_T>::Activated (Context& r)
    {
        Require (fActiveContext_ == nullptr);
        fActiveContext_ = &r;
    }
    template <typename CONTAINER_OF_T>
    shared_ptr<IElementConsumer> ListOfObjectsReader<CONTAINER_OF_T>::HandleChildStart (const StructuredStreamEvents::Name& name)
    {
        RequireNotNull (fActiveContext_);
        if (not fMemberElementName_.has_value () or name == *fMemberElementName_) {
            return make_shared<RepeatedElementReader<CONTAINER_OF_T>> (fValuePtr_);
        }
        else if (fThrowOnUnrecongizedelts_) {
            ThrowUnRecognizedStartElt (name);
        }
        else {
            return make_shared<IgnoreNodeReader> ();
        }
    }
    template <typename CONTAINER_OF_T>
    void ListOfObjectsReader<CONTAINER_OF_T>::Deactivating ()
    {
        RequireNotNull (fActiveContext_);
        fActiveContext_ = nullptr;
    }
    template <typename CONTAINER_OF_T>
    inline ReaderFromVoidStarFactory ListOfObjectsReader<CONTAINER_OF_T>::AsFactory ()
    {
        return IElementConsumer::AsFactory<CONTAINER_OF_T, ListOfObjectsReader> ();
    }
    template <typename CONTAINER_OF_T>
    inline ReaderFromVoidStarFactory ListOfObjectsReader<CONTAINER_OF_T>::AsFactory (const Name& memberElementName)
    {
        return IElementConsumer::AsFactory<CONTAINER_OF_T, ListOfObjectsReader> (memberElementName);
    }

    /*
      ********************************************************************************
      *************** ObjectReaderRegistry::OptionalTypesReader **********************
      ********************************************************************************
      */
    template <typename T, typename TRAITS>
    OptionalTypesReader<T, TRAITS>::OptionalTypesReader (optional<T>* intoVal)
        : fValue_{intoVal}
    {
        RequireNotNull (intoVal);
    }
    template <typename T, typename TRAITS>
    void OptionalTypesReader<T, TRAITS>::Activated (Context& r)
    {
        Assert (fActualReader_ == nullptr);
        fActualReader_ = r.GetObjectReaderRegistry ().MakeContextReader (&fProxyValue_);
        fActualReader_->Activated (r);
    }
    template <typename T, typename TRAITS>
    shared_ptr<IElementConsumer> OptionalTypesReader<T, TRAITS>::HandleChildStart (const Name& name)
    {
        AssertNotNull (fActualReader_);
        return fActualReader_->HandleChildStart (name);
    }
    template <typename T, typename TRAITS>
    void OptionalTypesReader<T, TRAITS>::HandleTextInside (const String& text)
    {
        AssertNotNull (fActualReader_);
        fActualReader_->HandleTextInside (text);
    }
    template <typename T, typename TRAITS>
    void OptionalTypesReader<T, TRAITS>::Deactivating ()
    {
        AssertNotNull (fActualReader_);
        fActualReader_->Deactivating ();
        fActualReader_.reset ();
        *fValue_ = fProxyValue_;
    }
    template <typename T, typename TRAITS>
    ReaderFromVoidStarFactory OptionalTypesReader<T, TRAITS>::AsFactory ()
    {
        return IElementConsumer::AsFactory<optional<T>, OptionalTypesReader> ();
    }

    /*
     ********************************************************************************
     ********************* ObjectReaderRegistry::MixinReader ************************
     ********************************************************************************
     */
    template <typename T>
    const function<byte*(T*)> MixinReader<T>::MixinEltTraits::kDefaultAddressOfSubElementFetcher =
        [] (T* b) { return reinterpret_cast<byte*> (b); };
    template <typename T>
    inline MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<byte*(T*)>& addressOfSubEltFetcher)
        : fReaderFactory{readerFactory}
        , fAddressOfSubElementFetcher{addressOfSubEltFetcher}
    {
    }
    template <typename T>
    inline MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory,
                                                           const function<bool (const Name& name)>& readsName, const function<byte*(T*)>& addressOfSubEltFetcher)
        : fReaderFactory{readerFactory}
        , fReadsName{readsName}
        , fAddressOfSubElementFetcher{addressOfSubEltFetcher}
    {
    }
    template <typename T>
    inline MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool ()>& readsText,
                                                           const function<byte*(T*)>& addressOfSubEltFetcher)
        : fReaderFactory{readerFactory}
        , fReadsText{readsText}
        , fAddressOfSubElementFetcher{addressOfSubEltFetcher}
    {
    }
    template <typename T>
    inline MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory&         readerFactory,
                                                           const function<bool (const Name& name)>& readsName,
                                                           const function<bool ()>& readsText, const function<byte*(T*)>& addressOfSubEltFetcher)
        : fReaderFactory{readerFactory}
        , fReadsName{readsName}
        , fReadsText{readsText}
        , fAddressOfSubElementFetcher{addressOfSubEltFetcher}
    {
    }

    template <typename T>
    MixinReader<T>::MixinReader (T* vp, const Traversal::Iterable<MixinEltTraits>& mixins)
        : fValuePtr_{vp}
        , fMixins_{mixins}
    {
        RequireNotNull (vp);
        for (const MixinEltTraits& m : mixins) {
            fMixinReaders_ += m.fReaderFactory (m.fAddressOfSubElementFetcher (vp));
        }
    }
    template <typename T>
    void MixinReader<T>::Activated (Context& r)
    {
        Require (fActiveContext_ == nullptr);
        fActiveContext_ = &r;
    }
    template <typename T>
    shared_ptr<IElementConsumer> MixinReader<T>::HandleChildStart (const Name& name)
    {
        RequireNotNull (fActiveContext_);
        size_t idx = 0;
        for (const MixinEltTraits& m : fMixins_) {
            if (m.fReadsName (name)) {
                shared_ptr<IElementConsumer> reader = fMixinReaders_[idx];
                if (not fActivatedReaders_.Contains (reader)) {
                    reader->Activated (*fActiveContext_);
                    fActivatedReaders_.Add (reader);
                }
                return reader->HandleChildStart (name);
            }
            ++idx;
        }
        return make_shared<IgnoreNodeReader> ();
    }
    template <typename T>
    void MixinReader<T>::HandleTextInside (const String& text)
    {
        RequireNotNull (fActiveContext_);
        size_t idx = 0;
        for (const MixinEltTraits& m : fMixins_) {
            if (m.fReadsText ()) {
                shared_ptr<IElementConsumer> reader = fMixinReaders_[idx];
                if (not fActivatedReaders_.Contains (reader)) {
                    reader->Activated (*fActiveContext_);
                    fActivatedReaders_.Add (reader);
                }
                fMixinReaders_[idx]->HandleTextInside (text);
            }
            ++idx;
        }
    }
    template <typename T>
    void MixinReader<T>::Deactivating ()
    {
        RequireNotNull (fActiveContext_);
        for (shared_ptr<IElementConsumer> reader : fActivatedReaders_) {
            reader->Deactivating ();
        }
        fActiveContext_ = nullptr;
    }
    template <typename T>
    inline ReaderFromVoidStarFactory MixinReader<T>::AsFactory (const Traversal::Iterable<MixinEltTraits>& mixins)
    {
        return IElementConsumer::AsFactory<T, MixinReader> (mixins);
    }

    /*
     ********************************************************************************
     ********************* ObjectReaderRegistry::RangeReader ************************
     ********************************************************************************
     */
    template <typename T>
    RangeReader<T>::RangeReader (T* intoVal, const pair<Name, Name>& pairNames)
        : fPairNames{pairNames}
        , fValue_{intoVal}
    {
        RequireNotNull (intoVal);
    }
    template <typename T>
    void RangeReader<T>::Activated (Context& r)
    {
        Assert (fActualReader_ == nullptr);
        fActualReader_ = Registry::MakeClassReader<RangeData_> ({
            {fPairNames.first, &RangeData_::fLowerBound},
            {fPairNames.second, &RangeData_::fUpperBound},
        }) (&fProxyValue_);
        fActualReader_->Activated (r);
    }
    template <typename T>
    shared_ptr<IElementConsumer> RangeReader<T>::HandleChildStart (const Name& name)
    {
        AssertNotNull (fActualReader_);
        return fActualReader_->HandleChildStart (name);
    }
    template <typename T>
    void RangeReader<T>::HandleTextInside (const String& text)
    {
        AssertNotNull (fActualReader_);
        fActualReader_->HandleTextInside (text);
    }
    template <typename T>
    void RangeReader<T>::Deactivating ()
    {
        AssertNotNull (fActualReader_);
        fActualReader_->Deactivating ();
        fActualReader_.reset ();
        *fValue_ = T{fProxyValue_.fLowerBound, fProxyValue_.fUpperBound};
    }
    template <typename T>
    ReaderFromVoidStarFactory RangeReader<T>::AsFactory (const pair<Name, Name>& pairNames)
    {
        return Registry::ConvertReaderToFactory<T, RangeReader> (pairNames);
    }

    /*
     ********************************************************************************
     ****************** ObjectReaderRegistry::IgnoreNodeReader **********************
     ********************************************************************************
     */
    inline ReaderFromVoidStarFactory IgnoreNodeReader::AsFactory ()
    {
        return [] (void*) -> shared_ptr<IElementConsumer> { return make_shared<IgnoreNodeReader> (); };
    }

    /*
     ********************************************************************************
     ****************** ObjectReaderRegistry::ReadDownToReader **********************
     ********************************************************************************
     */
    inline ReaderFromVoidStarFactory ReadDownToReader::AsFactory (const ReaderFromVoidStarFactory& theUseReader)
    {
        return [theUseReader] (void* p) -> shared_ptr<IElementConsumer> { return make_shared<ReadDownToReader> (theUseReader (p)); };
    }
    inline ReaderFromVoidStarFactory ReadDownToReader::AsFactory (const ReaderFromVoidStarFactory& theUseReader, const Name& tagToHandOff)
    {
        return [theUseReader, tagToHandOff] (void* p) -> shared_ptr<IElementConsumer> {
            return make_shared<ReadDownToReader> (theUseReader (p), tagToHandOff);
        };
    }

    /*
     ********************************************************************************
     ************** ObjectReaderRegistry::RepeatedElementReader *********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline RepeatedElementReader<T, TRAITS>::RepeatedElementReader (ContainerType* v)
        : fValuePtr_{v}
    {
    }
    template <typename T, typename TRAITS>
    inline RepeatedElementReader<T, TRAITS>::RepeatedElementReader (ContainerType* pv, const ReaderFromVoidStarFactory& actualElementFactory)
        : fValuePtr_{pv}
        , fReaderRactory_{actualElementFactory}
    {
    }
    template <typename T, typename TRAITS>
    inline RepeatedElementReader<T, TRAITS>::RepeatedElementReader (ContainerType* v, const Name& readonlyThisName,
                                                                    const ReaderFromVoidStarFactory& actualElementFactory)
        : fValuePtr_{v}
        , fReaderRactory_{actualElementFactory}
        , fReadThisName_{[readonlyThisName] (const Name& n) { return n == readonlyThisName; }}
    {
    }
    template <typename T, typename TRAITS>
    inline RepeatedElementReader<T, TRAITS>::RepeatedElementReader (ContainerType* v, const Name& readonlyThisName)
        : fValuePtr_{v}
        , fReadThisName_{[readonlyThisName] (const Name& n) { return n == readonlyThisName; }}
    {
    }
    template <typename T, typename TRAITS>
    void RepeatedElementReader<T, TRAITS>::Activated (Context& r)
    {
        Assert (fActiveSubReader_ == nullptr);
        fActiveSubReader_ = fReaderRactory_ ? (*fReaderRactory_) (&fProxyValue_) : r.GetObjectReaderRegistry ().MakeContextReader (&fProxyValue_);
        fActiveSubReader_->Activated (r);
    }
    template <typename T, typename TRAITS>
    shared_ptr<IElementConsumer> RepeatedElementReader<T, TRAITS>::HandleChildStart (const Name& name)
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
    template <typename T, typename TRAITS>
    void RepeatedElementReader<T, TRAITS>::HandleTextInside (const String& text)
    {
        AssertNotNull (fActiveSubReader_);
        fActiveSubReader_->HandleTextInside (text);
    }
    template <typename T, typename TRAITS>
    void RepeatedElementReader<T, TRAITS>::Deactivating ()
    {
        AssertNotNull (fActiveSubReader_);
        fActiveSubReader_->Deactivating ();
        fActiveSubReader_.reset ();
        TRAITS::ContainerAdapterAdder::Add (fValuePtr_, fProxyValue_);
    }
    template <typename T, typename TRAITS>
    inline ReaderFromVoidStarFactory RepeatedElementReader<T, TRAITS>::AsFactory ()
    {
        return IElementConsumer::AsFactory<T, RepeatedElementReader> ();
    }
    template <typename T, typename TRAITS>
    inline ReaderFromVoidStarFactory RepeatedElementReader<T, TRAITS>::AsFactory (const Name&                      readonlyThisName,
                                                                                  const ReaderFromVoidStarFactory& actualElementFactory)
    {
        return IElementConsumer::AsFactory<T, RepeatedElementReader> (readonlyThisName, actualElementFactory);
    }
    template <typename T, typename TRAITS>
    inline ReaderFromVoidStarFactory RepeatedElementReader<T, TRAITS>::AsFactory (const ReaderFromVoidStarFactory& actualElementFactory)
    {
        return IElementConsumer::AsFactory<T, RepeatedElementReader> (actualElementFactory);
    }
    template <typename T, typename TRAITS>
    inline ReaderFromVoidStarFactory RepeatedElementReader<T, TRAITS>::AsFactory (const Name& readonlyThisName)
    {
        return IElementConsumer::AsFactory<T, RepeatedElementReader> (readonlyThisName);
    }

    /*
     ********************************************************************************
     ***************************** ObjectReaderRegistry *****************************
     ********************************************************************************
     */
    inline void Registry::Add (type_index forType, const ReaderFromVoidStarFactory& readerFactory)
    {
        fFactories_.Add (forType, readerFactory);
    }
    template <typename T>
    void Registry::Add (const ReaderFromTStarFactory<T>& readerFactory)
    {
        Add (typeid (T), [readerFactory] (void* data) { return readerFactory (reinterpret_cast<T*> (data)); });
    }
    inline shared_ptr<IElementConsumer> Registry::MakeContextReader (type_index ti, void* destinationObject) const
    {
        if constexpr (qDebug) {
            if (not fFactories_.ContainsKey (ti)) {
                using namespace Characters;
                Debug::TraceContextBumper ctx{"Registry::MakeContextReader",
                                              "FAILED TO FIND READER! (forTypeInfo = {}) - Use of UnRegistered Type!"_f, ti};
            }
        }
        ReaderFromVoidStarFactory factory = *fFactories_.Lookup (ti); // must be found or caller/assert error
        return factory (destinationObject);
    }
    template <typename T>
    inline shared_ptr<IElementConsumer> Registry::MakeContextReader (T* destinationObject) const
    {
        return MakeContextReader (typeid (T), destinationObject);
    }
    template <typename T, typename... ARGS>
    inline void Registry::AddCommonType (ARGS&&... args)
    {
        Add<T> (MakeCommonReader<T> (forward<ARGS> (args)...));
    }
    inline optional<ReaderFromVoidStarFactory> Registry::Lookup (type_index t) const
    {
        return fFactories_.Lookup (t);
    }
    template <typename CLASS>
    void Registry::AddCommonReader_Class (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions)
    {
        if constexpr (qDebug) {
            for (const auto& kv : fieldDescriptions) {
                if (not kv.fOverrideTypeMapper.has_value () and not fFactories_.ContainsKey (kv.fFieldMetaInfo.GetTypeInfo ())) {
                    using namespace Characters;
                    Debug::TraceContextBumper ctx{"Registry::AddCommonReader_Class",
                                                  "CLASS={} field-TypeInfo-not-found = {}, for field named '{}' - UnRegistered Type!"_f,
                                                  type_index{typeid (CLASS)}, kv.fFieldMetaInfo.GetTypeInfo (), kv.fSerializedFieldName};
                    RequireNotReached ();
                }
            }
        }
        Add<CLASS> (MakeClassReader<CLASS> (fieldDescriptions));
    }
    template <typename CLASS>
    auto Registry::MakeClassReader (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions) -> ReaderFromVoidStarFactory
    {
        return [fieldDescriptions] (void* data) -> shared_ptr<IElementConsumer> {
            return make_shared<ClassReader<CLASS>> (fieldDescriptions, reinterpret_cast<CLASS*> (data));
        };
    }
    template <typename T, typename READER, typename... ARGS>
    auto Registry::ConvertReaderToFactory (ARGS&&... args) -> ReaderFromVoidStarFactory
    {
        ReaderFromTStarFactory<T> tmpFactory{
            [args...] (T* o) -> shared_ptr<IElementConsumer> { return make_shared<READER> (o, forward<ARGS> (args)...); }};
        return [tmpFactory] (void* data) { return tmpFactory (reinterpret_cast<T*> (data)); };
    }
    template <typename T>
    auto Registry::cvtFactory_ (const ReaderFromTStarFactory<T>& tf) -> ReaderFromVoidStarFactory
    {
        return [tf] (void* data) { return tf (reinterpret_cast<T*> (data)); };
    }
    template <typename T>
    auto Registry::MakeCommonReader_SimpleReader_ () -> ReaderFromVoidStarFactory
    {
        return cvtFactory_<T> ([] (T* o) -> shared_ptr<IElementConsumer> { return make_shared<SimpleReader_<T>> (o); });
    }
    template <typename ENUM_TYPE>
    auto Registry::MakeCommonReader_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap) -> ReaderFromVoidStarFactory
    {
        using namespace Characters::Literals;
        struct myReader_ : public IElementConsumer {
            Containers::Bijection<ENUM_TYPE, String> fNameMap;
            myReader_ (const Containers::Bijection<ENUM_TYPE, String>& nameMap, ENUM_TYPE* intoVal)
                : fNameMap{nameMap}
                , fValue_{intoVal}
            {
                RequireNotNull (intoVal);
            }
            Characters::StringBuilder<>          fBuf_{};
            ENUM_TYPE*                           fValue_{};
            virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override
            {
                ThrowUnRecognizedStartElt (name);
            }
            virtual void HandleTextInside (const String& text) override
            {
                fBuf_ += text;
            }
            virtual void Deactivating () override
            {
                if (auto optVal = fNameMap.InverseLookup (fBuf_.str ())) [[likely]] {
                    *fValue_ = *optVal;
                }
                else {
                    DbgTrace ("Enumeration ('{}') value '{}' out of range"_f, type_index{typeid (ENUM_TYPE)}, fBuf_.str ());
                    static const auto kException_ = BadFormatException{"Enumeration value out of range"sv};
                    Execution::Throw (kException_);
                }
            }
        };
        return cvtFactory_<ENUM_TYPE> (
            [nameMap] (ENUM_TYPE* o) -> shared_ptr<IElementConsumer> { return make_shared<myReader_> (nameMap, o); });
    };
    template <typename ENUM_TYPE>
    auto Registry::MakeCommonReader_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap) -> ReaderFromVoidStarFactory
    {
        return MakeCommonReader_NamedEnumerations (Containers::Bijection<ENUM_TYPE, String> (nameMap));
    }
    template <typename ENUM_TYPE>
    inline void Registry::AddCommonReader_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap)
    {
        Add<ENUM_TYPE> (MakeCommonReader_NamedEnumerations<ENUM_TYPE> (nameMap));
    }
    template <typename ENUM_TYPE>
    inline void Registry::AddCommonReader_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap)
    {
        Add<ENUM_TYPE> (MakeCommonReader_NamedEnumerations<ENUM_TYPE> (nameMap));
    }
    template <typename ENUM_TYPE>
    auto Registry::MakeCommonReader_EnumAsInt () -> ReaderFromVoidStarFactory
    {
        using namespace Characters;
        struct myReader_ : public IElementConsumer {
            myReader_ (ENUM_TYPE* intoVal)
                : fValue_{intoVal}
            {
                RequireNotNull (intoVal);
            }
            Characters::StringBuilder<>          fBuf_{};
            ENUM_TYPE*                           fValue_{};
            virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override
            {
                ThrowUnRecognizedStartElt (name);
            }
            virtual void HandleTextInside (const String& text) override
            {
                fBuf_ += text;
            }
            virtual void Deactivating () override
            {
                using SerializeAsType = typename std::underlying_type<ENUM_TYPE>::type;
                SerializeAsType tmp   = Characters::String2Int<SerializeAsType> (fBuf_.str ());
                if (Configuration::ToInt (ENUM_TYPE::eSTART) <= tmp and tmp < Configuration::ToInt (ENUM_TYPE::eEND)) [[likely]] {
                    *fValue_ = Configuration::ToEnum<ENUM_TYPE> (tmp);
                }
                else {
                    DbgTrace ("Enumeration ('{}') value '{}' out of range"_f, type_index{typeid (ENUM_TYPE)}, fBuf_);
                    static const auto kException_ = BadFormatException{"Enumeration value out of range"sv};
                    Execution::Throw (kException_);
                }
            }
        };
        return cvtFactory_<ENUM_TYPE> ([] (ENUM_TYPE* o) -> shared_ptr<IElementConsumer> { return make_shared<myReader_> (o); });
    }
    template <typename ENUM_TYPE>
    inline void Registry::AddCommonReader_EnumAsInt ()
    {
        Add<ENUM_TYPE> (MakeCommonReader_EnumAsInt<ENUM_TYPE> ());
    }
    template <typename T>
    auto Registry::MakeCommonReader_Simple (const function<T (String)>& converterFromString2T) -> ReaderFromVoidStarFactory
    {
        using namespace Characters;
        struct myReader_ : public IElementConsumer {
            function<T (String)> fString2TMapper_;
            myReader_ (const function<T (String)>& converterFromString2T, T* intoVal)
                : fString2TMapper_{converterFromString2T}
                , fValue_{intoVal}
            {
                RequireNotNull (intoVal);
            }
            Characters::StringBuilder<>          fBuf_{};
            T*                                   fValue_{};
            virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override
            {
                ThrowUnRecognizedStartElt (name);
            }
            virtual void HandleTextInside (const String& text) override
            {
                fBuf_ += text;
            }
            virtual void Deactivating () override
            {
                *fValue_ = fString2TMapper_ (fBuf_.str ()); // its up to fString2TMapper_ to throw if this conversion cannot be done
            }
        };
        return cvtFactory_<T> (
            [converterFromString2T] (T* o) -> shared_ptr<IElementConsumer> { return make_shared<myReader_> (converterFromString2T, o); });
    };
    template <typename T>
    inline void Registry::AddCommonReader_Simple (const function<T (String)>& converterFromString2T)
    {
        Add<T> (MakeCommonReader_Simple<T> (converterFromString2T));
    }
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const String*)
    {
        return MakeCommonReader_SimpleReader_<String> ();
    }
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const IO::Network::URI*)
    {
        return MakeCommonReader_SimpleReader_<IO::Network::URI> ();
    }
    template <typename T>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const T*)
        requires (is_enum_v<T>)
    {
        return MakeCommonReader_NamedEnumerations<T> ();
    }
    template <typename T>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const T*)
        requires (is_trivially_copyable_v<T> and is_standard_layout_v<T> and not is_enum_v<T>)
    {
        return MakeCommonReader_SimpleReader_<T> ();
    }
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const Time::DateTime*)
    {
        return MakeCommonReader_SimpleReader_<Time::DateTime> ();
    }
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const Time::Duration*)
    {
        return MakeCommonReader_SimpleReader_<Time::Duration> ();
    }
    template <typename T>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const optional<T>*)
    {
        return OptionalTypesReader<T>::AsFactory ();
    }
    template <typename T>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const vector<T>*)
    {
        return ListOfObjectsReader<vector<T>>::AsFactory ();
    }
    template <typename T>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const vector<T>*, const Name& name)
    {
        return ListOfObjectsReader<vector<T>>::AsFactory (name);
    }
    template <typename T>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const Collection<T>*)
    {
        return ListOfObjectsReader<Collection<T>>::AsFactory ();
    }
    template <typename T>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const Collection<T>*, const Name& name)
    {
        return ListOfObjectsReader<Collection<T>>::AsFactory (name);
    }
    template <typename T>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const Sequence<T>*)
    {
        return ListOfObjectsReader<Sequence<T>>::AsFactory ();
    }
    template <typename T>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const Sequence<T>*, const Name& name)
    {
        return ListOfObjectsReader<Sequence<T>>::AsFactory (name);
    }
    namespace PRIVATE_ {
        struct OptionalTypesReader_DateTime_DefaultTraits_ {
            static inline const Time::DateTime kDefaultValue = Time::DateTime::kMin;
        };
    }
    template <>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const optional<Time::DateTime>*)
    {
        return OptionalTypesReader<Time::DateTime, PRIVATE_::OptionalTypesReader_DateTime_DefaultTraits_>::AsFactory ();
    }
    template <typename T, typename... ARGS>
    inline ReaderFromVoidStarFactory Registry::MakeCommonReader (ARGS&&... args)
    {
        const T* n = nullptr; // arg unused, just for overloading
        DISABLE_COMPILER_MSC_WARNING_START (6011)
        return MakeCommonReader_ (n, forward<ARGS> (args)...);
        DISABLE_COMPILER_MSC_WARNING_END (6011)
    }

}
