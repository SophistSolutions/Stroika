/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <algorithm>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Adapters/Adder.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Debug/Trace.h"
#if qStroika_Foundation_DataExchange_ObjectVariantMapper_Activities
#include "Stroika/Foundation/Execution/Activity.h"
#endif
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/CheckedConverter.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/IO/Network/CIDR.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module (note the extra long name since its in a header)
//#define Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ************************** DefaultConstructForRead *****************************
     ********************************************************************************
     */
    template <>
    struct DefaultConstructForRead<Time::Date> {
        constexpr Time::Date operator() () const
        {
            return Time::Date::kMin;
        }
    };
    template <>
    struct DefaultConstructForRead<Time::DateTime> {
        constexpr Time::DateTime operator() () const
        {
            return Time::DateTime::kMin;
        }
    };
    template <>
    struct DefaultConstructForRead<Time::TimeOfDay> {
        constexpr Time::TimeOfDay operator() () const
        {
            return Time::TimeOfDay::kMin;
        }
    };
    template <>
    struct DefaultConstructForRead<IO::Network::CIDR> {
        IO::Network::CIDR operator() () const
        {
            using namespace IO::Network;
            return CIDR{InternetAddress{}, 0};
        }
    };

    /*
     ********************************************************************************
     ******************** ObjectVariantMapper::StructFieldInfo **********************
     ********************************************************************************
     */
    inline ObjectVariantMapper::StructFieldInfo::StructFieldInfo (const String& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo)
        : StructFieldInfo{serializedFieldName, fieldMetaInfo, optional<TypeMappingDetails>{}}
    {
    }
    inline ObjectVariantMapper::StructFieldInfo::StructFieldInfo (const String& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo,
                                                                  const optional<TypeMappingDetails>& overrideTypeMapper)
        : fSerializedFieldName_{serializedFieldName}
        , fFieldMetaInfo_{fieldMetaInfo}
        , fOverrideTypeMapper_{overrideTypeMapper}
    {
        Require (not serializedFieldName.empty ()); // Since Stroika v3.0d7
    }

    inline String ObjectVariantMapper::StructFieldInfo::GetSerializedFieldName () const
    {
        return fSerializedFieldName_;
    }
    inline StructFieldMetaInfo ObjectVariantMapper::StructFieldInfo::GetStructFieldMetaInfo () const
    {
        return *fFieldMetaInfo_;
    }
    inline auto ObjectVariantMapper::StructFieldInfo::GetOverrideTypeMapper () const -> optional<TypeMappingDetails>
    {
        return fOverrideTypeMapper_;
    }
    /*
     ********************************************************************************
     *********** DataExchange::ObjectVariantMapper::TypeMappingDetails **************
     ********************************************************************************
     */
    inline ObjectVariantMapper::TypeMappingDetails::TypeMappingDetails (const FromGenericObjectMapperType& fromObjectMapper,
                                                                        const ToGenericObjectMapperType& toObjectMapper, const type_index& forTypeInfo)
        : fForType_{forTypeInfo}
        , fFromObjectMapper_{fromObjectMapper}
        , fToObjectMapper_{toObjectMapper}
    {
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails::TypeMappingDetails (const FromObjectMapperType<T>& fromObjectMapper,
                                                                        const ToObjectMapperType<T>& toObjectMapper, const type_index& forTypeInfo)
        requires (not same_as<T, void>)
        : TypeMappingDetails{mkGenericFromMapper_ (fromObjectMapper), mkGenericToMapper_ (toObjectMapper), forTypeInfo}
    {
    }
    inline strong_ordering ObjectVariantMapper::TypeMappingDetails::operator<=> (const TypeMappingDetails& rhs) const
    {
#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy or qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
        return Common::compare_three_way_BWA{}(fForType_, rhs.fForType_);
#else
        return fForType_ <=> rhs.fForType_; // just compare types, not functions
#endif
    }
    inline bool ObjectVariantMapper::TypeMappingDetails::operator== (const TypeMappingDetails& rhs) const
    {
        return fForType_ == rhs.fForType_; // just compare types, not functions
    }
    inline type_index ObjectVariantMapper::TypeMappingDetails::GetForType () const
    {
        return fForType_;
    }
    inline auto ObjectVariantMapper::TypeMappingDetails::GetGenericFromObjectMapper () const -> FromGenericObjectMapperType
    {
        return fFromObjectMapper_;
    }
    inline auto ObjectVariantMapper::TypeMappingDetails::GetGenericToObjectMapper () const -> ToGenericObjectMapperType
    {
        return fToObjectMapper_;
    }
    template <typename T>
    inline ObjectVariantMapper::FromObjectMapperType<T> ObjectVariantMapper::TypeMappingDetails::FromObjectMapper (const FromGenericObjectMapperType& fromObjectMapper)
    {
        // See http://stroika-bugs.sophists.com/browse/STK-601 - properly/safely map the types, or do the more performant cast of the function objects
        if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
            return [fromObjectMapper] (const ObjectVariantMapper& mapper, const T* objOfType) -> VariantValue {
                return fromObjectMapper (mapper, objOfType);
            };
        }
        else {
            return *reinterpret_cast<const FromObjectMapperType<T>*> (&fromObjectMapper);
        }
    }
    template <typename T>
    inline ObjectVariantMapper::FromObjectMapperType<T> ObjectVariantMapper::TypeMappingDetails::FromObjectMapper () const
    {
        Require (type_index{typeid (T)} == fForType_);
        return FromObjectMapper<T> (fFromObjectMapper_);
    }
    template <typename T>
    inline ObjectVariantMapper::ToObjectMapperType<T> ObjectVariantMapper::TypeMappingDetails::ToObjectMapper (const ToGenericObjectMapperType& toObjectMapper)
    {
        // See http://stroika-bugs.sophists.com/browse/STK-601 - properly/safely map the types, or do the more performant cast of the function objects
        if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
            return [toObjectMapper] (const ObjectVariantMapper& mapper, const VariantValue& d, T* into) -> void {
                toObjectMapper (mapper, d, into);
            };
        }
        else {
            return *reinterpret_cast<const ToObjectMapperType<T>*> (&toObjectMapper);
        }
    }
    template <typename T>
    inline ObjectVariantMapper::ToObjectMapperType<T> ObjectVariantMapper::TypeMappingDetails::ToObjectMapper () const
    {
        Require (type_index{typeid (T)} == fForType_);
        return ToObjectMapper<T> (fToObjectMapper_);
    }
    template <typename T>
    inline ObjectVariantMapper::FromGenericObjectMapperType
    ObjectVariantMapper::TypeMappingDetails::mkGenericFromMapper_ (const FromObjectMapperType<T>& fromObjectMapper)
    {
        // See http://stroika-bugs.sophists.com/browse/STK-601 - properly/safely map the types, or do the more performant cast of the function objects
        if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
            return [fromObjectMapper] (const ObjectVariantMapper& mapper, const void* objOfType) -> VariantValue {
                return fromObjectMapper (mapper, reinterpret_cast<const T*> (objOfType));
            };
        }
        else {
            return *reinterpret_cast<const FromGenericObjectMapperType*> (&fromObjectMapper);
        }
    }
    template <typename T>
    inline ObjectVariantMapper::ToGenericObjectMapperType ObjectVariantMapper::TypeMappingDetails::mkGenericToMapper_ (const ToObjectMapperType<T>& toObjectMapper)
    {
        if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
            return [toObjectMapper] (const ObjectVariantMapper& mapper, const VariantValue& d, void* into) -> void {
                toObjectMapper (mapper, d, reinterpret_cast<T*> (into));
            };
        }
        else {
            return *reinterpret_cast<const ToGenericObjectMapperType*> (&toObjectMapper);
        }
    }

    /*
     ********************************************************************************
     ************** DataExchange::ObjectVariantMapper::TypesRegistry ****************
     ********************************************************************************
     */
    inline ObjectVariantMapper::TypesRegistry::TypesRegistry (const Traversal::Iterable<TypeMappingDetails>& src)
        : fSerializers_{src}
    {
    }
    inline optional<ObjectVariantMapper::TypeMappingDetails> ObjectVariantMapper::TypesRegistry::Lookup (type_index t) const
    {
        return fSerializers_.Lookup (t);
    }
    inline void ObjectVariantMapper::TypesRegistry::Add (const TypeMappingDetails& typeMapDetails)
    {
        fSerializers_.Add (typeMapDetails);
    }
    inline void ObjectVariantMapper::TypesRegistry::Add (const Traversal::Iterable<TypeMappingDetails>& typeMapDetails)
    {
        fSerializers_ += typeMapDetails;
    }
    inline void ObjectVariantMapper::TypesRegistry::operator+= (const TypeMappingDetails& typeMapDetails)
    {
        Add (typeMapDetails);
    }
    inline void ObjectVariantMapper::TypesRegistry::operator+= (const Traversal::Iterable<TypeMappingDetails>& typeMapDetails)
    {
        Add (typeMapDetails);
    }
    inline Traversal::Iterable<ObjectVariantMapper::TypeMappingDetails> ObjectVariantMapper::TypesRegistry::GetMappers () const
    {
        return fSerializers_;
    }

    /*
     ********************************************************************************
     ******************************** ObjectVariantMapper ***************************
     ********************************************************************************
     */
    template <Configuration::StdCompat::formattable<wchar_t> T>
    inline const ObjectVariantMapper::FromObjectMapperType<T> ObjectVariantMapper::kTraceFromObjectMapper =
        [] (const ObjectVariantMapper& mapper, const T* objOfType) -> VariantValue {
        DbgTrace ("FromObject<{}>(mapper, {}) called", typeid (T), objOfType);
        return {};
    };
    template <Configuration::StdCompat::formattable<wchar_t> T>
    inline const ObjectVariantMapper::ToObjectMapperType<T> ObjectVariantMapper::kTraceToObjectMapper =
        [] (const ObjectVariantMapper& mapper, const VariantValue& d, T* into) -> void {
        DbgTrace ("ToObject<{}>(mapper, {}, {}) called", typeid (T), d, into);
    };
    inline ObjectVariantMapper::TypesRegistry ObjectVariantMapper::GetTypeMappingRegistry () const
    {
        return fTypeMappingRegistry_;
    }
    inline void ObjectVariantMapper::SetTypeMappingRegistry (const TypesRegistry& s)
    {
        fTypeMappingRegistry_ = s;
    }
    template <typename T>
    inline void ObjectVariantMapper::Add (const FromObjectMapperType<T>& fromObjectMapper, const ToObjectMapperType<T>& toObjectMapper)
    {
        Add (TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (T)});
    }
    inline void ObjectVariantMapper::operator+= (const TypeMappingDetails& rhs)
    {
        Add (rhs);
    }
    inline void ObjectVariantMapper::operator+= (const Traversal::Iterable<TypeMappingDetails>& rhs)
    {
        Add (rhs);
    }
    inline void ObjectVariantMapper::operator+= (const TypesRegistry& rhs)
    {
        Add (rhs);
    }
    inline void ObjectVariantMapper::operator+= (const ObjectVariantMapper& rhs)
    {
        Add (rhs);
    }
    template <typename T, typename... ARGS>
    inline void ObjectVariantMapper::AddCommonType (ARGS&&... args)
    {
        const T* n = nullptr; // arg unused, just for overloading
        AssertDependentTypesAlreadyInRegistry_ (n);
        auto&& serializer = MakeCommonSerializer<T> (forward<ARGS> (args)...);
        Assert (serializer.GetForType () == typeid (T));
        Add (serializer);
    }
    template <typename CLASS>
    inline void ObjectVariantMapper::AddClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, const ClassMapperOptions<CLASS>& mapperOptions)
    {
        Add (MakeClassSerializer_<CLASS> (fieldDescriptions, mapperOptions, this));
    }
    template <typename CLASS, typename BASE_CLASS>
    void ObjectVariantMapper::AddSubClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, const ClassMapperOptions<CLASS>& mapperOptions)
    {
        TypeMappingDetails        baseClassTypeMapper = Lookup_ (typeid (BASE_CLASS));
        ClassMapperOptions<CLASS> useOptions          = mapperOptions;
        Require (useOptions.fBeforeFrom == nullptr);
        Require (useOptions.fBeforeTo == nullptr);
        // subtle, but OK - function taking ptr to BASE_CLASS can be used as a function ptr taking SUBCLASS (since ptr always satisfies rule is base_class)
        useOptions.fBeforeFrom = baseClassTypeMapper.FromObjectMapper<BASE_CLASS> ();
        useOptions.fBeforeTo   = baseClassTypeMapper.ToObjectMapper<BASE_CLASS> ();
        Add (MakeClassSerializer_<CLASS> (fieldDescriptions, useOptions, this));
    }
    template <typename T>
    inline auto ObjectVariantMapper::ToObjectMapper () const -> ToObjectMapperType<T>
    {
        Require (Lookup_ (typeid (T)).GetGenericToObjectMapper ());
        return Lookup_ (typeid (T)).ToObjectMapper<T> ();
    }
    template <typename T>
    inline auto ObjectVariantMapper::FromObjectMapper () const -> FromObjectMapperType<T>
    {
        return Lookup_ (typeid (T)).FromObjectMapper<T> ();
    }
    template <typename T>
    inline void ObjectVariantMapper::ToObject (const ToObjectMapperType<T>& toObjectMapper, const VariantValue& v, T* into) const
    {
        RequireNotNull (into);
        // if toObjectMapper, this does nothing to into
        // LOGICALLY require but cannot compare == on std::function! Require (toObjectMapper  == ToObjectMapper<T> ());  // pass it in as optimization, but not change of semantics
        if (toObjectMapper != nullptr) {
            toObjectMapper (*this, v, into);
        }
    }
    template <typename T>
    inline void ObjectVariantMapper::ToObject (const VariantValue& v, T* into) const
    {
        RequireNotNull (into);
        ToObject<T> (ToObjectMapper<T> (), v, into);
    }
    template <IDefaultConstructForRead T>
    inline T ObjectVariantMapper::ToObject (const ToObjectMapperType<T>& toObjectMapper, const VariantValue& v) const
    {
        T tmp = DefaultConstructForRead<T>{}();
        ToObject (toObjectMapper, v, &tmp);
        return tmp;
    }
    template <IDefaultConstructForRead T>
    inline T ObjectVariantMapper::ToObject (const VariantValue& v) const
    {
        return ToObject<T> (ToObjectMapper<T> (), v);
    }
    template <typename T>
    inline optional<T> ObjectVariantMapper::ToObjectQuietly (const VariantValue& v) const
    {
        try {
            return ToObject<T> (ToObjectMapper<T> (), v);
        }
        catch (...) {
            return nullopt;
        }
    }
    template <typename T>
    inline VariantValue ObjectVariantMapper::FromObject (const FromObjectMapperType<T>& fromObjectMapper, const T& from) const
    {
        // define null fromObjectMapper as just retuning a null variant value
        if (fromObjectMapper == nullptr) {
            return VariantValue{};
        }
        // LOGICALLY require but cannot compare == on std::function! Require (fromObjectMapper  == FromObjectMapper<T> ());  // pass it in as optimization, but not change of semantics
        return fromObjectMapper (*this, &from);
    }
    template <typename T>
    inline VariantValue ObjectVariantMapper::FromObject (const T& from) const
    {
        return FromObjectMapper<T> () (*this, &from);
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (T*)
    {
        // by default nothing to check, just check certain partial specializations
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (optional<T>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T)); // just for side-effect of assert check
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (DOMAIN_TYPE)); // just for side-effect of assert check
            (void)Lookup_ (typeid (RANGE_TYPE));  // just for side-effect of assert check
        }
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (Containers::Collection<T>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T)); // just for side-effect of assert check
        }
    }
    template <typename T, typename TRAITS>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Traversal::DiscreteRange<T, TRAITS>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T)); // just for side-effect of assert check
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (Containers::KeyedCollection<T, KEY_TYPE, TRAITS>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T));        // just for side-effect of assert check
            (void)Lookup_ (typeid (KEY_TYPE)); // just for side-effect of assert check
        }
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Mapping<KEY_TYPE, VALUE_TYPE>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (KEY_TYPE));   // just for side-effect of assert check
            (void)Lookup_ (typeid (VALUE_TYPE)); // just for side-effect of assert check
        }
    }
    template <typename T, typename TRAITS>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Traversal::Range<T, TRAITS>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T)); // just for side-effect of assert check
        }
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Sequence<T>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T)); // just for side-effect of assert check
        }
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Set<T>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T)); // just for side-effect of assert check
        }
    }
    template <typename T, typename TRAITS>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Execution::Synchronized<T, TRAITS>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T)); // just for side-effect of assert check
        }
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const vector<T>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T)); // just for side-effect of assert check
        }
    }
    template <typename T1, typename T2>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const pair<T1, T2>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T1)); // just for side-effect of assert check
            (void)Lookup_ (typeid (T2)); // just for side-effect of assert check
        }
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Common::CountedValue<T>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename T1, typename T2>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Common::KeyValuePair<T1, T2>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T1)); // just for side-effect of assert check
            (void)Lookup_ (typeid (T2)); // just for side-effect of assert check
        }
    }
    template <typename T1>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const tuple<T1>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T1)); // just for side-effect of assert check
        }
    }
    template <typename T1, typename T2>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const tuple<T1, T2>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T1)); // just for side-effect of assert check
            (void)Lookup_ (typeid (T2)); // just for side-effect of assert check
        }
    }
    template <typename T1, typename T2, typename T3>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const tuple<T1, T2, T3>*)
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T1)); // just for side-effect of assert check
            (void)Lookup_ (typeid (T2)); // just for side-effect of assert check
            (void)Lookup_ (typeid (T3)); // just for side-effect of assert check
        }
    }
    template <typename T, size_t SZ>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const T (*)[SZ])
    {
        if constexpr (qDebug) {
            (void)Lookup_ (typeid (T)); // just for side-effect of assert check
        }
    }
    template <Containers::Adapters::IAddableTo ACTUAL_CONTAINER_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_WithAdder ()
    {
        using namespace Characters::Literals;
        using T                                                      = typename ACTUAL_CONTAINER_TYPE::value_type;
        FromObjectMapperType<ACTUAL_CONTAINER_TYPE> fromObjectMapper = [] (const ObjectVariantMapper&   mapper,
                                                                           const ACTUAL_CONTAINER_TYPE* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            Sequence<VariantValue> s;
            if (not fromObjOfTypeT->empty ()) {
                FromObjectMapperType<T> valueMapper{mapper.FromObjectMapper<T> ()};
                for (const auto& i : *fromObjOfTypeT) {
                    s.Append (mapper.FromObject<T> (valueMapper, i));
                }
            }
            return VariantValue{s};
        };
        ToObjectMapperType<ACTUAL_CONTAINER_TYPE> toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d,
                                                                       ACTUAL_CONTAINER_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            Require (intoObjOfTypeT->empty ());
#if qStroika_Foundation_DataExchange_ObjectVariantMapper_Activities
            auto                       decodingClassActivity = Execution::LazyEvalActivity{[&] () -> String {
                return Characters::Format ("Decoding {:.100} into class {}"_f, d, type_index{typeid (ACTUAL_CONTAINER_TYPE)});
            }};
            Execution::DeclareActivity da{&decodingClassActivity};
#endif
            Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
            if (not s.empty ()) {
                ToObjectMapperType<T> valueMapper{mapper.ToObjectMapper<T> ()};
                for (const auto& i : s) {
                    Containers::Adapters::Adder<ACTUAL_CONTAINER_TYPE>::Add (intoObjOfTypeT, mapper.ToObject<T> (valueMapper, i));
                }
            }
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (ACTUAL_CONTAINER_TYPE)};
    }
    template <typename T, typename... ARGS>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer (ARGS&&... args)
    {
        const T*           n = nullptr; // arg unused, just for overloading
        TypeMappingDetails tmp{MakeCommonSerializer_ (n, forward<ARGS> (args)...)};
        // NB: because of how we match on MakeCommonSerializer_, the type it sees maybe a base class of T, and we want to actually register the type the user specified.
        return TypeMappingDetails{tmp.GetGenericFromObjectMapper (), tmp.GetGenericToObjectMapper (), typeid (T)};
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeClassSerializer (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions,
                                                                                             const ClassMapperOptions<T>& options)
        requires (is_class_v<T>)
    {
        return MakeClassSerializer_<T> (fieldDescriptions, options, nullptr);
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeClassSerializer_ (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions,
                                                                                              const ClassMapperOptions<T>& options,
                                                                                              const ObjectVariantMapper* mapperToCheckAgainst)
        requires (is_class_v<T>)
    {
        return MakeCommonSerializer_ForClassObject_and_check_<T> (typeid (T), sizeof (T), fieldDescriptions, options, mapperToCheckAgainst);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE>*)
    {
        using namespace Characters;
        using Containers::Bijection;
        FromObjectMapperType<Bijection<DOMAIN_TYPE, RANGE_TYPE>> fromObjectMapper =
            [] (const ObjectVariantMapper& mapper, const Bijection<DOMAIN_TYPE, RANGE_TYPE>* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            FromObjectMapperType<DOMAIN_TYPE> domainMapper{mapper.FromObjectMapper<DOMAIN_TYPE> ()};
            FromObjectMapperType<RANGE_TYPE>  rangeMapper{mapper.FromObjectMapper<RANGE_TYPE> ()};
            Sequence<VariantValue>            s;
            for (const auto& i : *fromObjOfTypeT) {
                Sequence<VariantValue> encodedPair;
                encodedPair.Append (mapper.FromObject<DOMAIN_TYPE> (domainMapper, i.first));
                encodedPair.Append (mapper.FromObject<RANGE_TYPE> (rangeMapper, i.second));
                s.Append (VariantValue{encodedPair});
            }
            return VariantValue{s};
        };
        ToObjectMapperType<Bijection<DOMAIN_TYPE, RANGE_TYPE>> toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d,
                                                                                    Bijection<DOMAIN_TYPE, RANGE_TYPE>* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            ToObjectMapperType<DOMAIN_TYPE> domainMapper{mapper.ToObjectMapper<DOMAIN_TYPE> ()};
            ToObjectMapperType<RANGE_TYPE>  rangeMapper{mapper.ToObjectMapper<RANGE_TYPE> ()};
            Sequence<VariantValue>          s = d.As<Sequence<VariantValue>> ();
            intoObjOfTypeT->clear ();
            for (const VariantValue& encodedPair : s) {
                Sequence<VariantValue> p = encodedPair.As<Sequence<VariantValue>> ();
                if (p.size () != 2) [[unlikely]] {
                    DbgTrace ("Bijection ('{}') element with item count ({}) other than 2"_f,
                              type_index{typeid (Bijection<DOMAIN_TYPE, RANGE_TYPE>)}, static_cast<int> (p.size ()));
                    static const auto kException_ = BadFormatException{"Mapping element with item count other than 2"sv};
                    Execution::Throw (kException_);
                }
                intoObjOfTypeT->Add (mapper.ToObject<DOMAIN_TYPE> (domainMapper, p[0]), mapper.ToObject<RANGE_TYPE> (rangeMapper, p[1]));
            }
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (Bijection<DOMAIN_TYPE, RANGE_TYPE>)};
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::Collection<T>*)
    {
        return MakeCommonSerializer_WithAdder<Containers::Collection<T>> ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::KeyedCollection<T, KEY_TYPE, TRAITS>*)
    {
        return MakeCommonSerializer_WithAdder<Containers::KeyedCollection<T, KEY_TYPE, TRAITS>> ();
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Mapping<KEY_TYPE, VALUE_TYPE>*)
    {
        // NB: We default to 'json object/mapping' because this is the overwhelmingly most commonly expected mapping (especially to JSON), but
        // MakeCommonSerializer_MappingAsArrayOfKeyValuePairs is more general. If KEY_TYPE is not convertible to a string, or you wish to strucutre
        // the VariantValue (typically think JSON representation) as an array of Key/Value pairs, then use MakeCommonSerializer_MappingAsArrayOfKeyValuePairs
        return MakeCommonSerializer_MappingWithStringishKey<Containers::Mapping<KEY_TYPE, VALUE_TYPE>> ();
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::MultiSet<T>*)
    {
        return MakeCommonSerializer_WithAdder<Containers::MultiSet<T>> ();
    }
    template <typename T>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const optional<T>*)
    {
        FromObjectMapperType<optional<T>> fromObjectMapper = [] (const ObjectVariantMapper& mapper, const optional<T>* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            if (fromObjOfTypeT->has_value ()) {
                return mapper.FromObject<T> (**fromObjOfTypeT);
            }
            else {
                return VariantValue{};
            }
        };
        ToObjectMapperType<optional<T>> toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, optional<T>* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            if (d.GetType () == VariantValue::eNull) {
                *intoObjOfTypeT = nullopt;
            }
            else {
                // SEE http://stroika-bugs.sophists.com/browse/STK-910
                // fix here - I KNOW I have something there, but how to construct
                *intoObjOfTypeT = mapper.ToObject<T> (d);
            }
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (optional<T>)};
    }
    template <typename T>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const optional<T>*, const OptionalSerializerOptions& options)
    {
        if (not options.fTMapper.has_value ()) {
            return MakeCommonSerializer_ ((const optional<T>*)nullptr);
        }
        FromObjectMapperType<optional<T>> fromObjectMapper = [options] (const ObjectVariantMapper& mapper, const optional<T>* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            if (fromObjOfTypeT->has_value ()) {
                return options.fTMapper->FromObjectMapper<T> () (mapper, &**fromObjOfTypeT);
            }
            else {
                return VariantValue{};
            }
        };
        ToObjectMapperType<optional<T>> toObjectMapper = [options] (const ObjectVariantMapper& mapper, const VariantValue& d,
                                                                    optional<T>* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            if (d.GetType () == VariantValue::eNull) {
                *intoObjOfTypeT = nullopt;
            }
            else {
                // SEE http://stroika-bugs.sophists.com/browse/STK-910
                // fix here - I KNOW I have something there, but how to construct
                T tmp{};
                options.fTMapper->ToObjectMapper<T> () (mapper, d, &tmp);
                *intoObjOfTypeT = tmp;
            }
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (optional<T>)};
    }
    //  http://stroika-bugs.sophists.com/browse/STK-910
    template <>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const optional<IO::Network::CIDR>*);
    template <typename T, typename TRAITS>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Execution::Synchronized<T, TRAITS>*)
    {
        using Execution::Synchronized;
        auto fromObjectMapper = [] (const ObjectVariantMapper& mapper, const Synchronized<T, TRAITS>* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            return mapper.FromObject<T> (**fromObjOfTypeT);
        };
        auto toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, Synchronized<T, TRAITS>* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            *intoObjOfTypeT = mapper.ToObject<T> (d);
        };
        return TypeMappingDetails{typeid (Synchronized<T, TRAITS>), fromObjectMapper, toObjectMapper};
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Sequence<T>*)
    {
        return MakeCommonSerializer_WithAdder<Sequence<T>> ();
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const vector<T>*)
    {
        return MakeCommonSerializer_WithAdder<vector<T>> ();
    }
    template <typename T1, typename T2>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const pair<T1, T2>*)
    {
        // render as array of two elements
        return TypeMappingDetails{
            FromObjectMapperType<pair<T1, T2>>{[] (const ObjectVariantMapper& mapper, const pair<T1, T2>* fromObj) -> VariantValue {
                return VariantValue{Sequence<VariantValue>{mapper.FromObject<T1> (fromObj->first), mapper.FromObject<T2> (fromObj->second)}};
            }},
            ToObjectMapperType<pair<T1, T2>>{[] (const ObjectVariantMapper& mapper, const VariantValue& d, pair<T1, T2>* intoObj) -> void {
                Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                if (s.size () < 2) {
                    static const auto kException_ = BadFormatException{"Array size out of range for pair"sv};
                    Execution::Throw (kException_);
                };
                *intoObj = make_pair (mapper.ToObject<T1> (s[0]), mapper.ToObject<T2> (s[1]));
            }},
            typeid (pair<T1, T2>)};
    }
    template <typename T>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Common::CountedValue<T>*)
    {
        // render as array of two elements
        using CountedValue = typename Common::CountedValue<T>;
        using CounterType  = typename CountedValue::CounterType;
        return TypeMappingDetails{
            FromObjectMapperType<CountedValue>{[] (const ObjectVariantMapper& mapper, const CountedValue* fromObj) -> VariantValue {
                return VariantValue{Sequence<VariantValue>{mapper.FromObject<T> (fromObj->fValue), mapper.FromObject<CounterType> (fromObj->fCount)}};
            }},
            ToObjectMapperType<CountedValue>{[] (const ObjectVariantMapper& mapper, const VariantValue& d, CountedValue* intoObj) -> void {
                Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                if (s.size () < 2) {
                    static const auto kException_ = BadFormatException{"Array size out of range for CountedValue"sv};
                    Execution::Throw (kException_);
                };
                *intoObj = CountedValue{mapper.ToObject<T> (s[0]), mapper.ToObject<CounterType> (s[1])};
            }},
            typeid (CountedValue)};
    }
    template <typename T1, typename T2>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Common::KeyValuePair<T1, T2>*)
    {
        // render as array of two elements
        return TypeMappingDetails{
            FromObjectMapperType<Common::KeyValuePair<T1, T2>>{[] (const ObjectVariantMapper& mapper, const Common::KeyValuePair<T1, T2>* fromObj) -> VariantValue {
                return VariantValue{Sequence<VariantValue>{mapper.FromObject<T1> (fromObj->fKey), mapper.FromObject<T2> (fromObj->fValue)}};
            }},
            ToObjectMapperType<Common::KeyValuePair<T1, T2>>{
                [] (const ObjectVariantMapper& mapper, const VariantValue& d, Common::KeyValuePair<T1, T2>* intoObj) -> void {
                    Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                    if (s.size () < 2) {
                        static const auto kException_ = BadFormatException{"Array size out of range for KeyValuePair"sv};
                        Execution::Throw (kException_);
                    };
                    *intoObj = Common::KeyValuePair<T1, T2>{mapper.ToObject<T1> (s[0]), mapper.ToObject<T2> (s[1])};
                }},
            typeid (Common::KeyValuePair<T1, T2>)};
    }
    template <typename T1>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const tuple<T1>*)
    {
        // render as array of one element
        return TypeMappingDetails{FromObjectMapperType<tuple<T1>>{[] (const ObjectVariantMapper& mapper, const tuple<T1>* fromObj) -> VariantValue {
                                      return VariantValue{Sequence<VariantValue>{mapper.FromObject<T1> (std::get<0> (*fromObj))}};
                                  }},
                                  ToObjectMapperType<tuple<T1>>{[] (const ObjectVariantMapper& mapper, const VariantValue& d, tuple<T1>* intoObj) -> void {
                                      Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                                      if (s.size () < 1) {
                                          static const auto kException_ = BadFormatException{"Array size out of range for tuple/1"sv};
                                          Execution::Throw (kException_);
                                      };
                                      *intoObj = make_tuple (mapper.ToObject<T1> (s[0]));
                                  }},
                                  typeid (tuple<T1>)};
    }
    template <typename T1, typename T2>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const tuple<T1, T2>*)
    {
        // render as array of two elements
        return TypeMappingDetails{
            FromObjectMapperType<tuple<T1, T2>>{[] (const ObjectVariantMapper& mapper, const tuple<T1, T2>* fromObj) -> VariantValue {
                return VariantValue{
                    Sequence<VariantValue>{mapper.FromObject<T1> (std::get<0> (*fromObj)), mapper.FromObject<T2> (std::get<1> (*fromObj))}};
            }},
            ToObjectMapperType<tuple<T1, T2>>{[] (const ObjectVariantMapper& mapper, const VariantValue& d, tuple<T1, T2>* intoObj) -> void {
                Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                if (s.size () < 2) {
                    static const auto kException_ = BadFormatException{"Array size out of range for tuple/2"sv};
                    Execution::Throw (kException_);
                };
                *intoObj = make_tuple (mapper.ToObject<T1> (s[0]), mapper.ToObject<T2> (s[1]));
            }},
            typeid (tuple<T1, T2>)};
    }
    template <typename T1, typename T2, typename T3>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const tuple<T1, T2, T3>*)
    {
        // render as array of three elements
        return TypeMappingDetails{
            FromObjectMapperType<tuple<T1, T2, T3>>{[] (const ObjectVariantMapper& mapper, const tuple<T1, T2, T3>* fromObj) -> VariantValue {
                return VariantValue{Sequence<VariantValue>{mapper.FromObject<T1> (std::get<0> (*fromObj)), mapper.FromObject<T2> (std::get<1> (*fromObj)),
                                                           mapper.FromObject<T3> (std::get<2> (*fromObj))}};
            }},
            ToObjectMapperType<tuple<T1, T2, T3>>{[] (const ObjectVariantMapper& mapper, const VariantValue& d, tuple<T1, T2, T3>* intoObj) -> void {
                Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                if (s.size () < 3) {
                    static const auto kException_ = BadFormatException{"Array size out of range for tuple/3"sv};
                    Execution::Throw (kException_);
                };
                *intoObj = make_tuple (mapper.ToObject<T1> (s[0]), mapper.ToObject<T2> (s[1]), mapper.ToObject<T3> (s[2]));
            }},
            typeid (tuple<T1, T2, T3>)};
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Set<T>*)
    {
        return MakeCommonSerializer_WithAdder<Set<T>> ();
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::SortedCollection<T>*)
    {
        return MakeCommonSerializer_WithAdder<Containers::SortedCollection<T>> ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::SortedKeyedCollection<T, KEY_TYPE, TRAITS>*)
    {
        return MakeCommonSerializer_WithAdder<Containers::SortedKeyedCollection<T, KEY_TYPE, TRAITS>> ();
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::SortedMapping<KEY_TYPE, VALUE_TYPE>*)
    {
        // NB: We default to 'json object/mapping' because this is the overwhelmingly most commonly expected mapping (especially to JSON), but
        // MakeCommonSerializer_MappingAsArrayOfKeyValuePairs is more general. If KEY_TYPE is not convertible to a string, or you wish to strucutre
        // the VariantValue (typically think JSON representation) as an array of Key/Value pairs, then use MakeCommonSerializer_MappingAsArrayOfKeyValuePairs
        return MakeCommonSerializer_MappingWithStringishKey<Containers::SortedMapping<KEY_TYPE, VALUE_TYPE>> ();
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::SortedMultiSet<T>*)
    {
        return MakeCommonSerializer_WithAdder<Containers::SortedMultiSet<T>> ();
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::SortedSet<T>*)
    {
        return MakeCommonSerializer_WithAdder<Containers::SortedSet<T>> ();
    }
    template <typename T, typename TRAITS>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Traversal::DiscreteRange<T, TRAITS>*)
    {
        return MakeCommonSerializer_Range_<Traversal::DiscreteRange<T, TRAITS>> ();
    }
    template <typename T, typename TRAITS, typename... ARGS>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Traversal::Range<T, TRAITS>*, ARGS&&... args)
    {
        return MakeCommonSerializer_Range_<Traversal::Range<T, TRAITS>> (forward<ARGS> (args)...);
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const T*)
        requires (is_enum_v<T>)
    {
        return MakeCommonSerializer_NamedEnumerations<T> ();
    }
    template <typename T, size_t SZ>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const T (*)[SZ])
    {
        using namespace Characters::Literals;
        // @todo - see http://stroika-bugs.sophists.com/browse/STK-581 - to switch from generic to 'T' based mapper.
        auto fromObjectMapper = [] (const ObjectVariantMapper& mapper, const void* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            FromObjectMapperType<T> valueMapper{mapper.FromObjectMapper<T> ()}; // optimization if > 1 array elt, and anti-optimization array.size == 0
            Sequence<VariantValue> s;
            const T*               actualMember{reinterpret_cast<const T*> (fromObjOfTypeT)};
            for (auto i = actualMember; i < actualMember + SZ; ++i) {
                s.Append (mapper.FromObject<T> (valueMapper, *i));
            }
            return VariantValue{s};
        };
        auto toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, void* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
            T*                     actualMember{reinterpret_cast<T*> (intoObjOfTypeT)};
            if (s.size () > SZ) [[unlikely]] {
                DbgTrace ("Array ('{}') actual size {} out of range"_f, type_index{typeid (T[SZ])}, static_cast<int> (s.size ()));
                static const auto kException_ = BadFormatException{"Array size out of range"sv};
                Execution::Throw (kException_);
            }
            ToObjectMapperType<T> valueMapper{mapper.ToObjectMapper<T> ()}; // optimization if > 1 array elt, and anti-optimization array.size == 0
            size_t idx = 0;
            for (const auto& i : s) {
                actualMember[idx++] = mapper.ToObject<T> (valueMapper, i);
            }
            while (idx < SZ) {
                actualMember[idx++] = T{};
            }
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (T[SZ])};
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename ACTUAL_CONTAINER_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_WithKeyValuePairAdd_ ()
    {
        auto fromObjectMapper = [] (const ObjectVariantMapper& mapper, const ACTUAL_CONTAINER_TYPE* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            FromObjectMapperType<KEY_TYPE>   keyMapper{mapper.FromObjectMapper<KEY_TYPE> ()};
            FromObjectMapperType<VALUE_TYPE> valueMapper{mapper.FromObjectMapper<VALUE_TYPE> ()};
            Sequence<VariantValue>           s;
            for (const auto& i : *fromObjOfTypeT) {
                Sequence<VariantValue> encodedPair;
                encodedPair.Append (mapper.FromObject<KEY_TYPE> (keyMapper, i.fKey));
                encodedPair.Append (mapper.FromObject<VALUE_TYPE> (valueMapper, i.fValue));
                s.Append (VariantValue{encodedPair});
            }
            return VariantValue{s};
        };
        auto toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, ACTUAL_CONTAINER_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            /*
             *  NB: When you mixup having an array and an object (say because of writing with
             *  MakeCommonSerializer_ContainerWithStringishKey and reading back with this regular Mapping serializer?) or for other reasons,
             *  the covnersion to d.As<Sequence<VariantValue>> () can fail with a format exception.
             *
             *  This requires you wrote with the above serializer.
             */
            ToObjectMapperType<KEY_TYPE>   keyMapper{mapper.ToObjectMapper<KEY_TYPE> ()};
            ToObjectMapperType<VALUE_TYPE> valueMapper{mapper.ToObjectMapper<VALUE_TYPE> ()};
            Sequence<VariantValue>         s = d.As<Sequence<VariantValue>> ();
            intoObjOfTypeT->clear ();
            for (const VariantValue& encodedPair : s) {
                Sequence<VariantValue> p = encodedPair.As<Sequence<VariantValue>> ();
                if (p.size () != 2) [[unlikely]] {
                    using namespace Characters;
                    DbgTrace ("Container with Key/Value pair ('{}') element with item count ({}) other than 2"_f,
                              type_index{typeid (ACTUAL_CONTAINER_TYPE)}, static_cast<int> (p.size ()));
                    static const auto kException_ = BadFormatException{"Container with Key/Value pair element with item count other than 2"_k};
                    Execution::Throw (kException_);
                }
                intoObjOfTypeT->Add (mapper.ToObject<KEY_TYPE> (keyMapper, p[0]), mapper.ToObject<VALUE_TYPE> (valueMapper, p[1]));
            }
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (ACTUAL_CONTAINER_TYPE)};
    }
    template <typename ENUM_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap)
    {
        using namespace Characters::Literals;
        static_assert (is_enum_v<ENUM_TYPE>, "MakeCommonSerializer_NamedEnumerations only works for enum types");
        using SerializeAsType = typename underlying_type<ENUM_TYPE>::type;
        static_assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE), "underlyingtype?");
        FromObjectMapperType<ENUM_TYPE> fromObjectMapper = [nameMap] ([[maybe_unused]] const ObjectVariantMapper& mapper,
                                                                      const ENUM_TYPE* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            Assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE));
            Assert (static_cast<ENUM_TYPE> (static_cast<SerializeAsType> (*fromObjOfTypeT)) == *fromObjOfTypeT); // no round-trip loss
            return VariantValue{*nameMap.Lookup (*fromObjOfTypeT)};
        };
        ToObjectMapperType<ENUM_TYPE> toObjectMapper = [nameMap] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d,
                                                                  ENUM_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            auto optVal = nameMap.InverseLookup (d.As<String> ());
            if (not optVal.has_value ()) [[unlikely]] {
                DbgTrace ("Enumeration ('{}') value '{}' out of range"_f, type_index{typeid (ENUM_TYPE)}, d);
                static const auto kException_ = BadFormatException{"Enumeration value out of range"sv};
                Execution::Throw (kException_);
            }
            *intoObjOfTypeT = *optVal;
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (ENUM_TYPE)};
    }
    template <typename ENUM_TYPE>
    inline ObjectVariantMapper::TypeMappingDetails
    ObjectVariantMapper::MakeCommonSerializer_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap)
    {
        return MakeCommonSerializer_NamedEnumerations (Containers::Bijection<ENUM_TYPE, String>{nameMap});
    }
    template <typename ENUM_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_EnumAsInt ()
    {
        /*
         *  Note: we cannot get the enumeration print names - in general. That would be nicer to read, but we don't have
         *  the data, and this is simple and efficient.
         *
         *  See MakeCommonSerializer_NamedEnumerations
         */
        static_assert (is_enum_v<ENUM_TYPE>, "This only works for enum types");
        using SerializeAsType = typename underlying_type<ENUM_TYPE>::type;
        static_assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE), "underlyingtype?");
        FromObjectMapperType<ENUM_TYPE> fromObjectMapper = [] ([[maybe_unused]] const ObjectVariantMapper& mapper,
                                                               const ENUM_TYPE*                            fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            Assert (static_cast<ENUM_TYPE> (static_cast<SerializeAsType> (*fromObjOfTypeT)) == *fromObjOfTypeT); // no round-trip loss
            return VariantValue{static_cast<SerializeAsType> (*fromObjOfTypeT)};
        };
        ToObjectMapperType<ENUM_TYPE> toObjectMapper = [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d,
                                                           ENUM_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            *intoObjOfTypeT = static_cast<ENUM_TYPE> (d.As<SerializeAsType> ());
            Assert (static_cast<SerializeAsType> (*intoObjOfTypeT) == d.As<SerializeAsType> ()); // no round-trip loss
            if (not(ENUM_TYPE::eSTART <= *intoObjOfTypeT and *intoObjOfTypeT < ENUM_TYPE::eEND)) [[unlikely]] {
                using namespace Characters;
                DbgTrace ("Enumeration ('{}') value {} out of range"_f, type_index{typeid (ENUM_TYPE)}, static_cast<int> (*intoObjOfTypeT));
                static const auto kException_ = BadFormatException{"Enumeration value out of range"_k};
                Execution::Throw (kException_);
            }
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (ENUM_TYPE)};
    }
    template <typename ACTUAL_CONTAINTER_TYPE, IDefaultConstructForRead KEY_TYPE, IDefaultConstructForRead VALUE_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_MappingWithStringishKey ()
    {
        FromObjectMapperType<ACTUAL_CONTAINTER_TYPE> fromObjectMapper = [] (const ObjectVariantMapper&    mapper,
                                                                            const ACTUAL_CONTAINTER_TYPE* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            FromObjectMapperType<KEY_TYPE>   keyMapper{mapper.FromObjectMapper<KEY_TYPE> ()};
            FromObjectMapperType<VALUE_TYPE> valueMapper{mapper.FromObjectMapper<VALUE_TYPE> ()};
            map<String, VariantValue>        m; // use std::map instead of Mapping<> as slight speed tweak to avoid virtual call adding
            for (const Common::KeyValuePair<KEY_TYPE, VALUE_TYPE>& i : *fromObjOfTypeT) {
                m.insert ({mapper.FromObject<KEY_TYPE> (keyMapper, i.fKey).template As<String> (),
                           mapper.FromObject<VALUE_TYPE> (valueMapper, i.fValue)});
            }
            return VariantValue{Containers::Concrete::Mapping_stdmap<String, VariantValue>{move (m)}};
        };
        ToObjectMapperType<ACTUAL_CONTAINTER_TYPE> toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d,
                                                                        ACTUAL_CONTAINTER_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            ToObjectMapperType<KEY_TYPE>   keyMapper{mapper.ToObjectMapper<KEY_TYPE> ()};
            ToObjectMapperType<VALUE_TYPE> valueMapper{mapper.ToObjectMapper<VALUE_TYPE> ()};
            Mapping<String, VariantValue>  m{d.As<Mapping<String, VariantValue>> ()};
            intoObjOfTypeT->clear ();
            for (const Common::KeyValuePair<String, VariantValue>& p : m) {
                intoObjOfTypeT->Add (mapper.ToObject<KEY_TYPE> (keyMapper, p.fKey), mapper.ToObject<VALUE_TYPE> (valueMapper, p.fValue));
            }
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (ACTUAL_CONTAINTER_TYPE)};
    }
    template <typename ACTUAL_CONTAINTER_TYPE, IDefaultConstructForRead KEY_TYPE, IDefaultConstructForRead VALUE_TYPE>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_MappingAsArrayOfKeyValuePairs ()
    {
        return MakeCommonSerializer_WithKeyValuePairAdd_<KEY_TYPE, VALUE_TYPE, ACTUAL_CONTAINTER_TYPE> ();
    }
    template <typename RANGE_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Range_ (const RangeSerializerOptions& options)
    {
        using namespace Characters::Literals;
        static const String kLowerBoundLabel_{"LowerBound"sv};
        static const String kUpperBoundLabel_{"UpperBound"sv};
        String              lowerBoundLabel = options.fLowerBoundFieldName.value_or (kLowerBoundLabel_);
        String              upperBoundLabel = options.fUpperBoundFieldName.value_or (kUpperBoundLabel_);
        FromObjectMapperType<RANGE_TYPE> fromObjectMapper = [=] (const ObjectVariantMapper& mapper, const RANGE_TYPE* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            Mapping<String, VariantValue> m;
            if (fromObjOfTypeT->empty ()) {
                return VariantValue{};
            }
            else {
                using value_type = typename RANGE_TYPE::value_type;
                FromObjectMapperType<value_type> valueMapper{mapper.FromObjectMapper<value_type> ()};
                m.Add (lowerBoundLabel, mapper.FromObject<value_type> (valueMapper, fromObjOfTypeT->GetLowerBound ()));
                m.Add (upperBoundLabel, mapper.FromObject<value_type> (valueMapper, fromObjOfTypeT->GetUpperBound ()));
                return VariantValue{m};
            }
        };
        ToObjectMapperType<RANGE_TYPE> toObjectMapper = [=] (const ObjectVariantMapper& mapper, const VariantValue& d, RANGE_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            Mapping<String, VariantValue> m{d.As<Mapping<String, VariantValue>> ()};
            if (m.empty ()) {
                *intoObjOfTypeT = RANGE_TYPE{}; // empty maps to empty
            }
            else {
                if (m.size () != 2) [[unlikely]] {
                    DbgTrace ("Range ('{}') element needs LowerBound and UpperBound"_f, type_index{typeid (RANGE_TYPE)});
                    static const auto kException_ = BadFormatException{"Range needs LowerBound and UpperBound"sv};
                    Execution::Throw (kException_);
                }
                if (not m.ContainsKey (lowerBoundLabel)) [[unlikely]] {
                    DbgTrace ("Range ('{}') element needs LowerBound"_f, type_index{typeid (RANGE_TYPE)});
                    static const auto kException_ = BadFormatException{"Range needs 'LowerBound' element"sv};
                    Execution::Throw (kException_);
                }
                if (not m.ContainsKey (upperBoundLabel)) [[unlikely]] {
                    DbgTrace ("Range ('{}') element needs UpperBound"_f, type_index{typeid (RANGE_TYPE)});
                    static const auto kException_ = BadFormatException{"Range needs 'UpperBound' element"sv};
                    Execution::Throw (kException_);
                }
                using value_type = typename RANGE_TYPE::value_type;
                ToObjectMapperType<value_type> valueMapper{mapper.ToObjectMapper<value_type> ()};
                value_type                     from{mapper.ToObject<value_type> (valueMapper, *m.Lookup (lowerBoundLabel))};
                value_type                     to{mapper.ToObject<value_type> (valueMapper, *m.Lookup (upperBoundLabel))};
                *intoObjOfTypeT = CheckedConverter_Range<RANGE_TYPE> (from, to);
            }
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (RANGE_TYPE)};
    }
    template <typename CLASS>
    ObjectVariantMapper::TypeMappingDetails
    ObjectVariantMapper::MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, [[maybe_unused]] size_t sizeofObj,
                                                               const Traversal::Iterable<StructFieldInfo>& fields,
                                                               const ClassMapperOptions<CLASS>&            options)
    {
        using namespace Characters::Literals;
        if constexpr (qDebug) {
            {
                // assure each field unique
                Containers::MultiSet<StructFieldMetaInfo> t;
                for (const auto& i : fields) {
                    if (i.fFieldMetaInfo_) {
                        t.Add (*i.fFieldMetaInfo_);
                    }
                }
            }
            {
                // assure each field unique
                Containers::MultiSet<StructFieldMetaInfo> t;
                for (const auto& i : fields) {
                    if (i.fFieldMetaInfo_) {
                        t.Add (*i.fFieldMetaInfo_);
                    }
                }
                for (const auto& i : t) {
                    [[maybe_unused]] bool alreadyInListOfFields = not(i.fCount == 1);
                    WeakAssert (not alreadyInListOfFields); //  not necessarily something we want to prohibit, but overwhelmingly likely a bug/typo
                }
            }
        }
        FromObjectMapperType<CLASS> fromObjectMapper = [fields, options] (const ObjectVariantMapper& mapper, const CLASS* fromObjOfTypeT) -> VariantValue {
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"ObjectVariantMapper::TypeMappingDetails::{}::FromObjectMapper"};
#endif

#if qStroika_Foundation_DataExchange_ObjectVariantMapper_Activities
            auto decodingClassActivity =
                Execution::LazyEvalActivity{[&] () -> String { return Characters::Format ("Encoding {:.50}"_f, type_index{typeid (CLASS)}); }};
            Execution::DeclareActivity da{&decodingClassActivity};
#endif
            Mapping<String, VariantValue> m;
            if (options.fBeforeFrom) [[unlikely]] {
                m = options.fBeforeFrom (mapper, fromObjOfTypeT).template As<Mapping<String, VariantValue>> (); // so we can extend
            }
            for (const auto& i : fields) {
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("fieldname = {}, offset={}"_f, i.fSerializedFieldName_, i.fFieldMetaInfo_);
#endif
                VariantValue vv = [&] () {
                    const byte* b = i.fFieldMetaInfo_ ? i.fFieldMetaInfo_->GetAddressOfMember (fromObjOfTypeT)
                                                      : reinterpret_cast<const byte*> (fromObjOfTypeT);
                    if (i.fOverrideTypeMapper_) [[unlikely]] {
                        return i.fOverrideTypeMapper_->GetGenericFromObjectMapper () (mapper, b);
                    }
                    else {
                        Require (i.fFieldMetaInfo_);
                        return mapper.Lookup_ (i.fFieldMetaInfo_->GetTypeInfo ()).GetGenericFromObjectMapper () (mapper, b);
                    }
                }();
                if (not options.fOmitNullEntriesInFromObject or vv.GetType () != VariantValue::eNull) [[likely]] {
                    m.Add (i.fSerializedFieldName_, vv);
                }
            }
            VariantValue result{m};
            if (options.fAfterFrom) [[unlikely]] {
                options.fAfterFrom (mapper, fromObjOfTypeT, &result);
            }
            return result;
        };
        ToObjectMapperType<CLASS> toObjectMapper = [fields, options] (const ObjectVariantMapper& mapper, const VariantValue& d,
                                                                      CLASS* intoObjOfTypeT) -> void {
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"ObjectVariantMapper::TypeMappingDetails::{}::ToObjectMapper"};
#endif
            RequireNotNull (intoObjOfTypeT);
#if qStroika_Foundation_DataExchange_ObjectVariantMapper_Activities
            auto decodingClassActivity = Execution::LazyEvalActivity{
                [&] () -> String { return Characters::Format ("Decoding {:.100} into class {}"_f, d, type_index{typeid (CLASS)}); }};
            Execution::DeclareActivity da{&decodingClassActivity};
#endif
            if (options.fBeforeTo) {
                options.fBeforeTo (mapper, d, intoObjOfTypeT);
            }
            Mapping<String, VariantValue> m = d.As<Mapping<String, VariantValue>> ();
            for (const auto& i : fields) {
#if qStroika_Foundation_DataExchange_ObjectVariantMapper_Activities
                auto decodingFieldActivity = Execution::LazyEvalActivity{
                    [&] () -> String { return Characters::Format ("Decoding field {}"_f, i.fSerializedFieldName_); }};
                [[maybe_unused]] Execution::DeclareActivity daf{&decodingFieldActivity};
#endif
                optional<VariantValue> o = m.Lookup (i.fSerializedFieldName_);
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("fieldname = {}, offset={}, present={}"_f, i.fSerializedFieldName_, i.fFieldMetaInfo_, o.has_value ());
#endif
                if (o) {
                    byte* b = i.fFieldMetaInfo_ ? i.fFieldMetaInfo_->GetAddressOfMember (intoObjOfTypeT) : reinterpret_cast<byte*> (intoObjOfTypeT);
                    if (i.fOverrideTypeMapper_) {
                        i.fOverrideTypeMapper_->GetGenericToObjectMapper () (mapper, *o, b);
                    }
                    else {
                        Require (i.fFieldMetaInfo_);
                        mapper.Lookup_ (i.fFieldMetaInfo_->GetTypeInfo ()).GetGenericToObjectMapper () (mapper, *o, b);
                    }
                }
            }
            if (options.fAfterTo) {
                options.fAfterTo (mapper, d, intoObjOfTypeT);
            }
        };
        return TypeMappingDetails{fromObjectMapper, toObjectMapper, forTypeInfo};
    }
    template <typename CLASS>
    inline ObjectVariantMapper::TypeMappingDetails
    ObjectVariantMapper::MakeCommonSerializer_ForClassObject_and_check_ (const type_index& forTypeInfo, [[maybe_unused]] size_t sizeofObj,
                                                                         const Traversal::Iterable<StructFieldInfo>& fields,
                                                                         const ClassMapperOptions<CLASS>& options, const ObjectVariantMapper* use2Validate)
    {
        if constexpr (qDebug) {
            // Assure for each field type is registered. This is helpful 99% of the time the assert is triggered.
            // If you ever need to avoid it (I don't see how because this mapper doesn't work with circular data structures)
            // you can just define a bogus mapper temporarily, and then reset it to the real one before use.
            for (const auto& i : fields) {
                Require (i.fOverrideTypeMapper_ or i.fFieldMetaInfo_); // don't need to register the type mapper if its specified as a field
                if (not i.fOverrideTypeMapper_) {
                    Assert (i.fFieldMetaInfo_); // cuz need type mapper if fFieldMetaInfo_ not present
                    if (use2Validate != nullptr) {
                        (void)use2Validate->Lookup_ (i.fFieldMetaInfo_->GetTypeInfo ());
                    }
                }
            }
        }
        return MakeCommonSerializer_ForClassObject_<CLASS> (forTypeInfo, sizeofObj, fields, options);
    }
}
