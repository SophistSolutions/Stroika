/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_ObjectVariantMapper_inl_
#define _Stroika_Foundation_DataExchange_ObjectVariantMapper_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <algorithm>

#include "../Characters/String_Constant.h"
#include "../Characters/ToString.h"
#include "../Containers/Adapters/Adder.h"
#include "../Containers/Concrete/Mapping_stdmap.h"
#include "../Debug/Assertions.h"
#include "../Debug/Sanitizer.h"
#include "../Execution/Throw.h"

#include "BadFormatException.h"
#include "CheckedConverter.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module (note the extra long name since its in a header)
//#define Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ********************** ObjectVariantMapper::StructFieldInfo ********************
     ********************************************************************************
     */
    inline ObjectVariantMapper::StructFieldInfo::StructFieldInfo (const String& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo, const optional<TypeMappingDetails>& overrideTypeMapper, NullFieldHandling nullFields)
        : fFieldMetaInfo{fieldMetaInfo}
        , fSerializedFieldName{serializedFieldName}
        , fOverrideTypeMapper{overrideTypeMapper}
        , fNullFields{nullFields}
    {
    }
    inline ObjectVariantMapper::StructFieldInfo::StructFieldInfo (const String& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo, NullFieldHandling nullFields)
        : StructFieldInfo{serializedFieldName, fieldMetaInfo, {}, nullFields}
    {
    }
    template <int SZ>
    inline ObjectVariantMapper::StructFieldInfo::StructFieldInfo (const wchar_t (&serializedFieldName)[SZ], const StructFieldMetaInfo& fieldMetaInfo, NullFieldHandling nullFields)
        : StructFieldInfo{Characters::String_Constant{serializedFieldName}, fieldMetaInfo, {}, nullFields}
    {
    }
    template <int SZ>
    inline ObjectVariantMapper::StructFieldInfo::StructFieldInfo (const wchar_t (&serializedFieldName)[SZ], const StructFieldMetaInfo& fieldMetaInfo, const optional<TypeMappingDetails>& overrideTypeMapper, NullFieldHandling nullFields)
        : StructFieldInfo{Characters::String_Constant{serializedFieldName}, fieldMetaInfo, overrideTypeMapper, nullFields}
    {
    }

    /*
     ********************************************************************************
     *********** DataExchange::ObjectVariantMapper::TypeMappingDetails **************
     ********************************************************************************
     */
    inline ObjectVariantMapper::TypeMappingDetails::TypeMappingDetails (const type_index& forTypeInfo, const FromGenericObjectMapperType& fromObjectMapper, const ToGenericObjectMapperType& toObjectMapper)
        : fForType{forTypeInfo}
        , fFromObjectMapper{fromObjectMapper}
        , fToObjectMapper{toObjectMapper}
    {
        RequireNotNull (fromObjectMapper);
        RequireNotNull (toObjectMapper);
    }
    template <typename T, enable_if_t<not is_same_v<T, void> and Debug::kBuiltWithUndefinedBehaviorSanitizer>*>
    inline ObjectVariantMapper::TypeMappingDetails::TypeMappingDetails (const type_index& forTypeInfo, const FromObjectMapperType<T>& fromObjectMapper, const ToObjectMapperType<T>& toObjectMapper)
        : TypeMappingDetails{forTypeInfo,
                             [fromObjectMapper] (const ObjectVariantMapper& mapper, const void* objOfType) -> VariantValue {
                                 return fromObjectMapper (mapper, reinterpret_cast<const T*> (objOfType));
                             },
                             [toObjectMapper] (const ObjectVariantMapper& mapper, const VariantValue& d, void* into) -> void {
                                 toObjectMapper (mapper, d, reinterpret_cast<T*> (into));
                             }}
    {
        Require (type_index{typeid (T)} == forTypeInfo);
    }
    template <typename T, enable_if_t<not is_same_v<T, void> and not Debug::kBuiltWithUndefinedBehaviorSanitizer>*>
    inline ObjectVariantMapper::TypeMappingDetails::TypeMappingDetails (const type_index& forTypeInfo, const FromObjectMapperType<T>& fromObjectMapper, const ToObjectMapperType<T>& toObjectMapper)
        : TypeMappingDetails{forTypeInfo, *reinterpret_cast<const FromGenericObjectMapperType*> (&fromObjectMapper), *reinterpret_cast<const ToGenericObjectMapperType*> (&toObjectMapper)}
    {
        Require (type_index{typeid (T)} == forTypeInfo);
    }
#if __cpp_impl_three_way_comparison >= 201907
    inline strong_ordering ObjectVariantMapper::TypeMappingDetails::operator<=> (const TypeMappingDetails& rhs) const
    {
#if __cpp_lib_three_way_comparison >= 201907
        return std::compare_three_way{}(fForType, rhs.fForType);
#else
        if (fForType < rhs.fForType) {
            return strong_ordering::less;
        }
        if (fForType == rhs.fForType) {
            return strong_ordering::equal;
        }
        if (fForType > rhs.fForType) {
            return strong_ordering::greater;
        }
        AssertNotReached ();
        return strong_ordering::equal;
#endif
    }
#else
    inline bool ObjectVariantMapper::TypeMappingDetails::operator< (const TypeMappingDetails& rhs) const
    {
        return fForType < rhs.fForType;
    }
#endif
    inline bool ObjectVariantMapper::TypeMappingDetails::operator== (const TypeMappingDetails& rhs) const
    {
        return fForType == rhs.fForType;
    }
    template <typename T>
    inline ObjectVariantMapper::FromObjectMapperType<T> ObjectVariantMapper::TypeMappingDetails::FromObjectMapper (const FromGenericObjectMapperType& fromObjectMapper)
    {
        // See https://stroika.atlassian.net/browse/STK-601 - properly/safely map the types, or do the more performant cast of the function objects
        if constexpr (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
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
        Require (type_index{typeid (T)} == fForType);
        return FromObjectMapper<T> (fFromObjectMapper);
    }
    template <typename T>
    inline ObjectVariantMapper::ToObjectMapperType<T> ObjectVariantMapper::TypeMappingDetails::ToObjectMapper (const ToGenericObjectMapperType& toObjectMapper)
    {
        // See https://stroika.atlassian.net/browse/STK-601 - properly/safely map the types, or do the more performant cast of the function objects
        if constexpr (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
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
        Require (type_index{typeid (T)} == fForType);
        return ToObjectMapper<T> (fToObjectMapper);
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
        Add (TypeMappingDetails{typeid (T), fromObjectMapper, toObjectMapper});
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
        Assert (serializer.fForType == typeid (T));
        Add (serializer);
    }
    template <typename CLASS>
    inline void ObjectVariantMapper::AddClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, function<void (VariantValue*)> preflightBeforeToObject)
    {
#if qDebug
        for (const auto& f : fieldDescriptions) {
            if (not f.fOverrideTypeMapper.has_value ()) {
                (void)Lookup_ (f.fFieldMetaInfo.fTypeInfo); // for side-effect of internal Require
            }
        }
#endif
        Add (MakeCommonSerializer_ForClassObject_<CLASS> (typeid (CLASS), sizeof (CLASS), fieldDescriptions, preflightBeforeToObject));
    }
    template <typename CLASS, typename BASE_CLASS>
    inline void ObjectVariantMapper::AddSubClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, function<void (VariantValue*)> preflightBeforeToObject)
    {
#if qDebug
        for (const auto& f : fieldDescriptions) {
            if (not f.fOverrideTypeMapper.has_value ()) {
                (void)Lookup_ (f.fFieldMetaInfo.fTypeInfo); // for side-effect of internal Require
            }
        }
#endif
        Add (MakeCommonSerializer_ForClassObject_<CLASS, BASE_CLASS> (typeid (CLASS), sizeof (CLASS), fieldDescriptions, preflightBeforeToObject, type_index{typeid (BASE_CLASS)}));
    }
    template <typename T>
    inline auto ObjectVariantMapper::ToObjectMapper () const -> ToObjectMapperType<T>
    {
        Require (Lookup_ (typeid (T)).fToObjectMapper);
        return Lookup_ (typeid (T)).ToObjectMapper<T> ();
    }
    template <typename T>
    inline auto ObjectVariantMapper::FromObjectMapper () const -> FromObjectMapperType<T>
    {
        Require (Lookup_ (typeid (T)).fFromObjectMapper);
        return Lookup_ (typeid (T)).FromObjectMapper<T> ();
    }
    template <typename T>
    inline void ObjectVariantMapper::ToObject (const ToObjectMapperType<T>& toObjectMapper, const VariantValue& v, T* into) const
    {
        RequireNotNull (into);
        RequireNotNull (toObjectMapper);
        // LOGICALLY require but cannot compare == on std::function! Require (toObjectMapper  == ToObjectMapper<T> ());  // pass it in as optimization, but not change of semantics
        toObjectMapper (*this, v, into);
    }
    template <typename T>
    inline void ObjectVariantMapper::ToObject (const VariantValue& v, T* into) const
    {
        RequireNotNull (into);
        ToObject<T> (ToObjectMapper<T> (), v, into);
    }
    template <typename T>
    inline T ObjectVariantMapper::ToObject (const ToObjectMapperType<T>& toObjectMapper, const VariantValue& v) const
    {
        /*
         *  NOTE: It is because of this line of code (the default CTOR for tmp) - that we ObjectVariantMapper requires
         *  all its types to have a default constructor. To avoid that dependency, you may provide a template
         *  specialization of this method, which passes specific (default) args to CLASS, and then they will be filled in/replaced
         *  by the two argument ToObject.
         */
        T tmp;
        ToObject (toObjectMapper, v, &tmp);
        return tmp;
    }
    template <typename T>
    inline T ObjectVariantMapper::ToObject (const VariantValue& v) const
    {
        return ToObject<T> (ToObjectMapper<T> (), v);
    }
    template <typename T>
    inline VariantValue ObjectVariantMapper::FromObject (const FromObjectMapperType<T>& fromObjectMapper, const T& from) const
    {
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
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE>*)
    {
#if qDebug
        (void)Lookup_ (typeid (DOMAIN_TYPE)); // just for side-effect of assert check
        (void)Lookup_ (typeid (RANGE_TYPE));  // just for side-effect of assert check
#endif
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (Containers::Collection<T>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename T, typename TRAITS>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Traversal::DiscreteRange<T, TRAITS>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (Containers::KeyedCollection<T, KEY_TYPE, TRAITS>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T));        // just for side-effect of assert check
        (void)Lookup_ (typeid (KEY_TYPE)); // just for side-effect of assert check
#endif
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Mapping<KEY_TYPE, VALUE_TYPE>*)
    {
#if qDebug
        (void)Lookup_ (typeid (KEY_TYPE));   // just for side-effect of assert check
        (void)Lookup_ (typeid (VALUE_TYPE)); // just for side-effect of assert check
#endif
    }
    template <typename T, typename TRAITS>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Traversal::Range<T, TRAITS>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Sequence<T>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Set<T>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Containers::SortedCollection<T>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (Containers::SortedKeyedCollection<T, KEY_TYPE, TRAITS>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T));        // just for side-effect of assert check
        (void)Lookup_ (typeid (KEY_TYPE)); // just for side-effect of assert check
#endif
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Containers::SortedMapping<KEY_TYPE, VALUE_TYPE>*)
    {
#if qDebug
        (void)Lookup_ (typeid (KEY_TYPE));   // just for side-effect of assert check
        (void)Lookup_ (typeid (VALUE_TYPE)); // just for side-effect of assert check
#endif
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Containers::SortedSet<T>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename T, typename TRAITS>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const Execution::Synchronized<T, TRAITS>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename T>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const vector<T>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename T1, typename T2>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const pair<T1, T2>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T1)); // just for side-effect of assert check
        (void)Lookup_ (typeid (T2)); // just for side-effect of assert check
#endif
    }
    template <typename T1>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const tuple<T1>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T1)); // just for side-effect of assert check
#endif
    }
    template <typename T1, typename T2>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const tuple<T1, T2>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T1)); // just for side-effect of assert check
        (void)Lookup_ (typeid (T2)); // just for side-effect of assert check
#endif
    }
    template <typename T1, typename T2, typename T3>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const tuple<T1, T2, T3>*)
    {
#if qDebug
        (void)Lookup_ (typeid (T1)); // just for side-effect of assert check
        (void)Lookup_ (typeid (T2)); // just for side-effect of assert check
        (void)Lookup_ (typeid (T3)); // just for side-effect of assert check
#endif
    }
    template <typename T, size_t SZ>
    inline void ObjectVariantMapper::AssertDependentTypesAlreadyInRegistry_ (const T (*)[SZ])
    {
#if qDebug
        (void)Lookup_ (typeid (T)); // just for side-effect of assert check
#endif
    }
    template <typename ACTUAL_CONTAINER_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_WithAdder ()
    {
        using T                                                      = typename ACTUAL_CONTAINER_TYPE::value_type;
        FromObjectMapperType<ACTUAL_CONTAINER_TYPE> fromObjectMapper = [] (const ObjectVariantMapper& mapper, const ACTUAL_CONTAINER_TYPE* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            Sequence<VariantValue> s;
            if (not fromObjOfTypeT->empty ()) {
                FromObjectMapperType<T> valueMapper{mapper.FromObjectMapper<T> ()};
                for (const auto& i : *fromObjOfTypeT) {
                    s.Append (mapper.FromObject<T> (valueMapper, i));
                }
            }
            return VariantValue (s);
        };
        ToObjectMapperType<ACTUAL_CONTAINER_TYPE> toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, ACTUAL_CONTAINER_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
            if (not s.empty ()) {
                ToObjectMapperType<T> valueMapper{mapper.ToObjectMapper<T> ()};
                Assert (intoObjOfTypeT->empty ());
                for (const auto& i : s) {
                    Containers::Adapters::Adder<ACTUAL_CONTAINER_TYPE>::Add (intoObjOfTypeT, mapper.ToObject<T> (valueMapper, i));
                }
            }
        };
        return TypeMappingDetails{typeid (ACTUAL_CONTAINER_TYPE), fromObjectMapper, toObjectMapper};
    }
    template <typename T, typename... ARGS>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer (ARGS&&... args)
    {
        const T*           n = nullptr; // arg unused, just for overloading
        TypeMappingDetails tmp{MakeCommonSerializer_ (n, forward<ARGS> (args)...)};
        // NB: beacuse of how we match on MakeCommonSerializer_, the type it sees maybe a base class of T, and we want to actually register the type the user specified.
        return TypeMappingDetails{typeid (T), tmp.fFromObjectMapper, tmp.fToObjectMapper};
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE>*)
    {
        using Containers::Bijection;
        FromObjectMapperType<Bijection<DOMAIN_TYPE, RANGE_TYPE>> fromObjectMapper = [] (const ObjectVariantMapper& mapper, const Bijection<DOMAIN_TYPE, RANGE_TYPE>* fromObjOfTypeT) -> VariantValue {
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
            return VariantValue (s);
        };
        ToObjectMapperType<Bijection<DOMAIN_TYPE, RANGE_TYPE>> toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, Bijection<DOMAIN_TYPE, RANGE_TYPE>* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            ToObjectMapperType<DOMAIN_TYPE> domainMapper{mapper.ToObjectMapper<DOMAIN_TYPE> ()};
            ToObjectMapperType<RANGE_TYPE>  rangeMapper{mapper.ToObjectMapper<RANGE_TYPE> ()};
            Sequence<VariantValue>          s = d.As<Sequence<VariantValue>> ();
            intoObjOfTypeT->clear ();
            for (const VariantValue& encodedPair : s) {
                Sequence<VariantValue> p = encodedPair.As<Sequence<VariantValue>> ();
                if (p.size () != 2) [[UNLIKELY_ATTR]] {
                    DbgTrace (L"Bijection ('%s') element with item count (%d) other than 2", Characters::ToString (typeid (Bijection<DOMAIN_TYPE, RANGE_TYPE>)).c_str (), static_cast<int> (p.size ()));
                    Execution::Throw (BadFormatException{L"Mapping element with item count other than 2"sv});
                }
                intoObjOfTypeT->Add (mapper.ToObject<DOMAIN_TYPE> (domainMapper, p[0]), mapper.ToObject<RANGE_TYPE> (rangeMapper, p[1]));
            }
        };
        return TypeMappingDetails{typeid (Bijection<DOMAIN_TYPE, RANGE_TYPE>), fromObjectMapper, toObjectMapper};
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
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const optional<T>*)
    {
        FromObjectMapperType<optional<T>> fromObjectMapper = [] (const ObjectVariantMapper& mapper, const optional<T>* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            if (fromObjOfTypeT->has_value ()) {
                return mapper.FromObject<T> (**fromObjOfTypeT);
            }
            else {
                return VariantValue ();
            }
        };
        ToObjectMapperType<optional<T>> toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, optional<T>* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            if (d.GetType () == VariantValue::eNull) {
                *intoObjOfTypeT = nullopt;
            }
            else {
                *intoObjOfTypeT = mapper.ToObject<T> (d);
            }
        };
        return TypeMappingDetails{typeid (optional<T>), fromObjectMapper, toObjectMapper};
    }
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
            typeid (pair<T1, T2>),
            FromObjectMapperType<pair<T1, T2>>{[] (const ObjectVariantMapper& mapper, const pair<T1, T2>* fromObj) -> VariantValue {
                return VariantValue{Sequence<VariantValue>{mapper.FromObject<T1> (fromObj->first), mapper.FromObject<T2> (fromObj->second)}};
            }},
            ToObjectMapperType<pair<T1, T2>>{[] (const ObjectVariantMapper& mapper, const VariantValue& d, pair<T1, T2>* intoObj) -> void {
                Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                if (s.size () < 2) {
                    Execution::Throw (BadFormatException{L"Array size out of range for pair"sv});
                };
                *intoObj = make_pair (mapper.ToObject<T1> (s[0]), mapper.ToObject<T2> (s[1]));
            }}};
    }
    template <typename T1>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const tuple<T1>*)
    {
        // render as array of one element
        return TypeMappingDetails{
            typeid (tuple<T1>),
            FromObjectMapperType<tuple<T1>>{[] (const ObjectVariantMapper& mapper, const tuple<T1>* fromObj) -> VariantValue {
                return VariantValue{Sequence<VariantValue>{mapper.FromObject<T1> (std::get<0> (*fromObj))}};
            }},
            ToObjectMapperType<tuple<T1>>{[] (const ObjectVariantMapper& mapper, const VariantValue& d, tuple<T1>* intoObj) -> void {
                Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                if (s.size () < 1) {
                    Execution::Throw (BadFormatException{L"Array size out of range for tuple/1"sv});
                };
                *intoObj = make_tuple (mapper.ToObject<T1> (s[0]));
            }}};
    }
    template <typename T1, typename T2>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const tuple<T1, T2>*)
    {
        // render as array of two elements
        return TypeMappingDetails{
            typeid (tuple<T1, T2>),
            FromObjectMapperType<tuple<T1, T2>>{[] (const ObjectVariantMapper& mapper, const tuple<T1, T2>* fromObj) -> VariantValue {
                return VariantValue{Sequence<VariantValue>{mapper.FromObject<T1> (std::get<0> (*fromObj)), mapper.FromObject<T2> (std::get<1> (*fromObj))}};
            }},
            ToObjectMapperType<tuple<T1, T2>>{[] (const ObjectVariantMapper& mapper, const VariantValue& d, tuple<T1, T2>* intoObj) -> void {
                Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                if (s.size () < 2) {
                    Execution::Throw (BadFormatException{L"Array size out of range for tuple/2"sv});
                };
                *intoObj = make_tuple (mapper.ToObject<T1> (s[0]), mapper.ToObject<T2> (s[1]));
            }}};
    }
    template <typename T1, typename T2, typename T3>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const tuple<T1, T2, T3>*)
    {
        // render as array of three elements
        return TypeMappingDetails{
            typeid (tuple<T1, T2, T3>),
            FromObjectMapperType<tuple<T1, T2, T3>>{[] (const ObjectVariantMapper& mapper, const tuple<T1, T2, T3>* fromObj) -> VariantValue {
                return VariantValue{Sequence<VariantValue>{mapper.FromObject<T1> (std::get<0> (*fromObj)), mapper.FromObject<T2> (std::get<1> (*fromObj)), mapper.FromObject<T3> (std::get<2> (*fromObj))}};
            }},
            ToObjectMapperType<tuple<T1, T2, T3>>{[] (const ObjectVariantMapper& mapper, const VariantValue& d, tuple<T1, T2, T3>* intoObj) -> void {
                Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                if (s.size () < 3) {
                    Execution::Throw (BadFormatException{L"Array size out of range for tuple/3"sv});
                };
                *intoObj = make_tuple (mapper.ToObject<T1> (s[0]), mapper.ToObject<T2> (s[1]), mapper.ToObject<T3> (s[2]));
            }}};
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
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Containers::SortedSet<T>*)
    {
        return MakeCommonSerializer_WithAdder<Containers::SortedSet<T>> ();
    }
    template <typename T, typename TRAITS>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Traversal::DiscreteRange<T, TRAITS>*)
    {
        return MakeCommonSerializer_Range_<Traversal::DiscreteRange<T, TRAITS>> ();
    }
    template <typename T, typename TRAITS>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Traversal::Range<T, TRAITS>*)
    {
        return MakeCommonSerializer_Range_<Traversal::Range<T, TRAITS>> ();
    }
    template <typename T>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const T*, enable_if_t<is_enum_v<T>>*)
    {
        return MakeCommonSerializer_NamedEnumerations<T> ();
    }
    template <typename T, size_t SZ>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const T (*)[SZ])
    {
        // @todo - see https://stroika.atlassian.net/browse/STK-581 - to switch from generic to 'T' based mapper.
        auto fromObjectMapper = [] (const ObjectVariantMapper& mapper, const void* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            FromObjectMapperType<T> valueMapper{mapper.FromObjectMapper<T> ()}; // optimization if > 1 array elt, and anti-optimization array.size == 0
            Sequence<VariantValue>  s;
            const T*                actualMember{reinterpret_cast<const T*> (fromObjOfTypeT)};
            for (auto i = actualMember; i < actualMember + SZ; ++i) {
                s.Append (mapper.FromObject<T> (valueMapper, *i));
            }
            return VariantValue{s};
        };
        auto toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, void* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
            T*                     actualMember{reinterpret_cast<T*> (intoObjOfTypeT)};
            if (s.size () > SZ) [[UNLIKELY_ATTR]] {
                DbgTrace (L"Array ('%s') actual size %d out of range", Characters::ToString (typeid (T[SZ])).c_str (), static_cast<int> (s.size ()));
                Execution::Throw (BadFormatException{L"Array size out of range"sv});
            }
            ToObjectMapperType<T>
                   valueMapper{mapper.ToObjectMapper<T> ()}; // optimization if > 1 array elt, and anti-optimization array.size == 0
            size_t idx = 0;
            for (const auto& i : s) {
                actualMember[idx++] = mapper.ToObject<T> (valueMapper, i);
            }
            while (idx < SZ) {
                actualMember[idx++] = T{};
            }
        };
        return TypeMappingDetails{typeid (T[SZ]), fromObjectMapper, toObjectMapper};
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
                if (p.size () != 2) [[UNLIKELY_ATTR]] {
                    using namespace Characters;
                    DbgTrace (L"Container with Key/Value pair ('%s') element with item count (%d) other than 2", Characters::ToString (typeid (ACTUAL_CONTAINER_TYPE)).c_str (), static_cast<int> (p.size ()));
                    Execution::Throw (BadFormatException{L"Container with Key/Value pair element with item count other than 2"_k});
                }
                intoObjOfTypeT->Add (mapper.ToObject<KEY_TYPE> (keyMapper, p[0]), mapper.ToObject<VALUE_TYPE> (valueMapper, p[1]));
            }
        };
        return TypeMappingDetails{typeid (ACTUAL_CONTAINER_TYPE), fromObjectMapper, toObjectMapper};
    }
    template <typename ENUM_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap)
    {
        static_assert (is_enum_v<ENUM_TYPE>, "MakeCommonSerializer_NamedEnumerations only works for enum types");
        using SerializeAsType = typename underlying_type<ENUM_TYPE>::type;
        static_assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE), "underlyingtype?");
        FromObjectMapperType<ENUM_TYPE> fromObjectMapper = [nameMap] ([[maybe_unused]] const ObjectVariantMapper& mapper, const ENUM_TYPE* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            Assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE));
            Assert (static_cast<ENUM_TYPE> (static_cast<SerializeAsType> (*fromObjOfTypeT)) == *fromObjOfTypeT); // no round-trip loss
#if qCompilerAndStdLib_maybe_unused_in_lambda_ignored_Buggy
            &mapper;
#endif
            return VariantValue{*nameMap.Lookup (*fromObjOfTypeT)};
        };
        ToObjectMapperType<ENUM_TYPE> toObjectMapper = [nameMap] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, ENUM_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
#if qCompilerAndStdLib_maybe_unused_in_lambda_ignored_Buggy
            &mapper;
#endif
            auto optVal = nameMap.InverseLookup (d.As<String> ());
            if (not optVal.has_value ()) [[UNLIKELY_ATTR]] {
                DbgTrace (L"Enumeration ('%s') value '%s' out of range", Characters::ToString (typeid (ENUM_TYPE)).c_str (), d.As<String> ().c_str ());
                Execution::Throw (BadFormatException{L"Enumeration value out of range"sv});
            }
            *intoObjOfTypeT = *optVal;
        };
        return TypeMappingDetails{typeid (ENUM_TYPE), fromObjectMapper, toObjectMapper};
    }
    template <typename ENUM_TYPE>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap)
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
        FromObjectMapperType<ENUM_TYPE> fromObjectMapper = [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const ENUM_TYPE* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            Assert (static_cast<ENUM_TYPE> (static_cast<SerializeAsType> (*fromObjOfTypeT)) == *fromObjOfTypeT); // no round-trip loss
            return VariantValue{static_cast<SerializeAsType> (*fromObjOfTypeT)};
        };
        ToObjectMapperType<ENUM_TYPE> toObjectMapper = [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, ENUM_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            *intoObjOfTypeT = static_cast<ENUM_TYPE> (d.As<SerializeAsType> ());
            Assert (static_cast<SerializeAsType> (*intoObjOfTypeT) == d.As<SerializeAsType> ()); // no round-trip loss
            if (not(ENUM_TYPE::eSTART <= *intoObjOfTypeT and *intoObjOfTypeT < ENUM_TYPE::eEND)) [[UNLIKELY_ATTR]] {
                using namespace Characters;
                DbgTrace (L"Enumeration ('%s') value %d out of range", Characters::ToString (typeid (ENUM_TYPE)).c_str (), static_cast<int> (*intoObjOfTypeT));
                Execution::Throw (BadFormatException{L"Enumeration value out of range"_k});
            }
        };
        return TypeMappingDetails{typeid (ENUM_TYPE), fromObjectMapper, toObjectMapper};
    }
    template <typename ACTUAL_CONTAINTER_TYPE, typename KEY_TYPE, typename VALUE_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_MappingWithStringishKey ()
    {
        FromObjectMapperType<ACTUAL_CONTAINTER_TYPE> fromObjectMapper = [] (const ObjectVariantMapper& mapper, const ACTUAL_CONTAINTER_TYPE* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            FromObjectMapperType<KEY_TYPE>   keyMapper{mapper.FromObjectMapper<KEY_TYPE> ()};
            FromObjectMapperType<VALUE_TYPE> valueMapper{mapper.FromObjectMapper<VALUE_TYPE> ()};
            map<String, VariantValue>        m; // use std::map instead of Mapping<> as slight speed tweak to avoid virtual call adding
            for (const Common::KeyValuePair<KEY_TYPE, VALUE_TYPE>& i : *fromObjOfTypeT) {
                m.insert ({mapper.FromObject<KEY_TYPE> (keyMapper, i.fKey).template As<String> (), mapper.FromObject<VALUE_TYPE> (valueMapper, i.fValue)});
            }
            return VariantValue{Containers::Concrete::Mapping_stdmap<String, VariantValue>{move (m)}};
        };
        ToObjectMapperType<ACTUAL_CONTAINTER_TYPE> toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, ACTUAL_CONTAINTER_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            ToObjectMapperType<KEY_TYPE>   keyMapper{mapper.ToObjectMapper<KEY_TYPE> ()};
            ToObjectMapperType<VALUE_TYPE> valueMapper{mapper.ToObjectMapper<VALUE_TYPE> ()};
            Mapping<String, VariantValue>  m{d.As<Mapping<String, VariantValue>> ()};
            intoObjOfTypeT->clear ();
            for (const Common::KeyValuePair<String, VariantValue>& p : m) {
                intoObjOfTypeT->Add (mapper.ToObject<KEY_TYPE> (keyMapper, p.fKey), mapper.ToObject<VALUE_TYPE> (valueMapper, p.fValue));
            }
        };
        return TypeMappingDetails{typeid (ACTUAL_CONTAINTER_TYPE), fromObjectMapper, toObjectMapper};
    }
    template <typename ACTUAL_CONTAINTER_TYPE, typename KEY_TYPE, typename VALUE_TYPE>
    inline ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_MappingAsArrayOfKeyValuePairs ()
    {
        return MakeCommonSerializer_WithKeyValuePairAdd_<KEY_TYPE, VALUE_TYPE, ACTUAL_CONTAINTER_TYPE> ();
    }
    template <typename RANGE_TYPE>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Range_ ()
    {
        static const String              kLowerBoundLabel_{L"LowerBound"sv};
        static const String              kUpperBoundLabel_{L"UpperBound"sv};
        FromObjectMapperType<RANGE_TYPE> fromObjectMapper = [] (const ObjectVariantMapper& mapper, const RANGE_TYPE* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            using value_type = typename RANGE_TYPE::value_type;
            Mapping<String, VariantValue> m;
            if (fromObjOfTypeT->empty ()) {
                return VariantValue{};
            }
            else {
                FromObjectMapperType<value_type> valueMapper{mapper.FromObjectMapper<value_type> ()};
                m.Add (kLowerBoundLabel_, mapper.FromObject<value_type> (valueMapper, fromObjOfTypeT->GetLowerBound ()));
                m.Add (kUpperBoundLabel_, mapper.FromObject<value_type> (valueMapper, fromObjOfTypeT->GetUpperBound ()));
                return VariantValue{m};
            }
        };
        ToObjectMapperType<RANGE_TYPE> toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, RANGE_TYPE* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            using value_type = typename RANGE_TYPE::value_type;
            Mapping<String, VariantValue> m{d.As<Mapping<String, VariantValue>> ()};
            if (m.empty ()) {
                *intoObjOfTypeT = RANGE_TYPE (); // empty maps to empty
            }
            else {
                if (m.size () != 2) [[UNLIKELY_ATTR]] {
                    DbgTrace (L"Range ('%s') element needs LowerBound and UpperBound", Characters::ToString (typeid (RANGE_TYPE)).c_str ());
                    Execution::Throw (BadFormatException{L"Range needs LowerBound and UpperBound"sv});
                }
                if (not m.ContainsKey (kLowerBoundLabel_)) [[UNLIKELY_ATTR]] {
                    DbgTrace (L"Range ('%s') element needs LowerBound", Characters::ToString (typeid (RANGE_TYPE)).c_str ());
                    Execution::Throw (BadFormatException{L"Range needs 'LowerBound' element"sv});
                }
                if (not m.ContainsKey (kUpperBoundLabel_)) [[UNLIKELY_ATTR]] {
                    DbgTrace (L"Range ('%s') element needs UpperBound", Characters::ToString (typeid (RANGE_TYPE)).c_str ());
                    Execution::Throw (BadFormatException{L"Range needs 'UpperBound' element"sv});
                }
                ToObjectMapperType<value_type> valueMapper{mapper.ToObjectMapper<value_type> ()};
                value_type                     from{mapper.ToObject<value_type> (valueMapper, *m.Lookup (kLowerBoundLabel_))};
                value_type                     to{mapper.ToObject<value_type> (valueMapper, *m.Lookup (kUpperBoundLabel_))};
                *intoObjOfTypeT = CheckedConverter_Range<RANGE_TYPE> (from, to);
            }
        };
        return TypeMappingDetails{typeid (RANGE_TYPE), fromObjectMapper, toObjectMapper};
    }
    template <typename CLASS>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, [[maybe_unused]] size_t n, const Traversal::Iterable<StructFieldInfo>& fields, const function<void (VariantValue*)>& preflightBeforeToObject) const
    {
#if qDebug
        for (const auto& i : fields) {
            Require (i.fFieldMetaInfo.fOffset < n);
        }
        {
            // assure each field unique
            Containers::MultiSet<size_t> t;
            for (const auto& i : fields) {
                t.Add (i.fFieldMetaInfo.fOffset);
            }
            for (const auto& i : t) {
                bool alreadyInListOfFields = not(i.fCount == 1);
                Require (not alreadyInListOfFields); //  not necessarily something we want to prohibit, but overwhelmingly likely a bug/typo
            }
        }
        {
            // Assure for each field type is registered. This is helpfull 99% of the time the assert is triggered.
            // If you ever need to avoid it (I don't see how because this mapper doesn't work with circular data structures)
            // you can just define a bogus mapper temporarily, and then reset it to the real one before use.
            for (const auto& i : fields) {
                // don't need to register the type mapper if its specified as a field
                if (not i.fOverrideTypeMapper.has_value ()) {
                    (void)Lookup_ (i.fFieldMetaInfo.fTypeInfo);
                }
            }
        }
#endif
        FromObjectMapperType<CLASS> fromObjectMapper = [fields] (const ObjectVariantMapper& mapper, const CLASS* fromObjOfTypeT) -> VariantValue {
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"ObjectVariantMapper::TypeMappingDetails::{}::fFromObjectMapper"};
#endif
            map<String, VariantValue> m; //slight performance tweak, build STL container and then convert
            for (const auto& i : fields) {
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"fieldname = %s, offset=%d", i.fSerializedFieldName.c_str (), i.fFieldMetaInfo.fOffset);
#endif
                FromGenericObjectMapperType toGenericVariantMapper = i.fOverrideTypeMapper ? i.fOverrideTypeMapper->fFromObjectMapper : mapper.Lookup_ (i.fFieldMetaInfo.fTypeInfo).fFromObjectMapper;
                VariantValue                vv                     = toGenericVariantMapper (mapper, reinterpret_cast<const std::byte*> (fromObjOfTypeT) + i.fFieldMetaInfo.fOffset);
                if (i.fNullFields == ObjectVariantMapper::StructFieldInfo::eIncludeNullFields or vv.GetType () != VariantValue::eNull) {
                    m.insert ({i.fSerializedFieldName, vv});
                }
            }
            return VariantValue{Containers::Concrete::Mapping_stdmap<String, VariantValue>{move (m)}};
        };
        ToObjectMapperType<CLASS> toObjectMapper = [fields, preflightBeforeToObject] (const ObjectVariantMapper& mapper, const VariantValue& d, CLASS* intoObjOfTypeT) -> void {
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"ObjectVariantMapper::TypeMappingDetails::{}::fToObjectMapper"};
#endif
            RequireNotNull (intoObjOfTypeT);
            VariantValue v2Decode = d;
            if (preflightBeforeToObject != nullptr) {
                preflightBeforeToObject (&v2Decode);
            }
            Mapping<String, VariantValue> m = v2Decode.As<Mapping<String, VariantValue>> ();
            for (const auto& i : fields) {
                optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"fieldname = %s, offset=%d, present=%d", i.fSerializedFieldName.c_str (), i.fFieldMetaInfo.fOffset, o.has_value ());
#endif
                if (o) {
                    ToGenericObjectMapperType fromGenericVariantMapper = i.fOverrideTypeMapper ? i.fOverrideTypeMapper->fToObjectMapper : mapper.Lookup_ (i.fFieldMetaInfo.fTypeInfo).fToObjectMapper;
                    fromGenericVariantMapper (mapper, *o, reinterpret_cast<std::byte*> (intoObjOfTypeT) + i.fFieldMetaInfo.fOffset);
                }
            }
        };

        return TypeMappingDetails{forTypeInfo, fromObjectMapper, toObjectMapper};
    }
    template <typename CLASS, typename BASE_CLASS>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, [[maybe_unused]] size_t n, const Traversal::Iterable<StructFieldInfo>& fields, const function<void (VariantValue*)>& preflightBeforeToObject, const optional<type_index>& baseClassTypeInfo) const
    {
#if qDebug
        for (const auto& i : fields) {
            Require (i.fFieldMetaInfo.fOffset < n);
        }
        {
            // assure each field unique
            Containers::MultiSet<size_t> t;
            for (const auto& i : fields) {
                t.Add (i.fFieldMetaInfo.fOffset);
            }
            for (const auto& i : t) {
                bool alreadyInListOfFields = not(i.fCount == 1);
                Require (not alreadyInListOfFields); //  not necessarily something we want to prohibit, but overwhelmingly likely a bug/typo
            }
        }
        {
            // Assure for each field type is registered. This is helpfull 99% of the time the assert is triggered.
            // If you ever need to avoid it (I don't see how because this mapper doesn't work with circular data structures)
            // you can just define a bogus mapper temporarily, and then reset it to the real one before use.
            for (const auto& i : fields) {
                // don't need to register the type mapper if its specified as a field
                if (not i.fOverrideTypeMapper.has_value ()) {
                    (void)Lookup_ (i.fFieldMetaInfo.fTypeInfo);
                }
            }
            if (baseClassTypeInfo) {
                (void)Lookup_ (*baseClassTypeInfo);
            }
        }
#endif
        FromObjectMapperType<CLASS> fromObjectMapper = [fields, baseClassTypeInfo] (const ObjectVariantMapper& mapper, const CLASS* fromObjOfTypeT) -> VariantValue {
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"ObjectVariantMapper::TypeMappingDetails::{}::fFromObjectMapper"};
#endif
            Mapping<String, VariantValue> m;
            if (baseClassTypeInfo) {
                m = mapper.Lookup_ (typeid (BASE_CLASS)).FromObjectMapper<BASE_CLASS> () (mapper, fromObjOfTypeT).template As<Mapping<String, VariantValue>> (); // so we can extend
            }
            for (const auto& i : fields) {
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"fieldname = %s, offset=%d", i.fSerializedFieldName.c_str (), i.fFieldMetaInfo.fOffset);
#endif
                FromGenericObjectMapperType fromGenericObjectMapper = i.fOverrideTypeMapper ? i.fOverrideTypeMapper->fFromObjectMapper : mapper.Lookup_ (i.fFieldMetaInfo.fTypeInfo).fFromObjectMapper;
                VariantValue                vv                      = fromGenericObjectMapper (mapper, reinterpret_cast<const std::byte*> (fromObjOfTypeT) + i.fFieldMetaInfo.fOffset);
                if (i.fNullFields == ObjectVariantMapper::StructFieldInfo::NullFieldHandling::eInclude or vv.GetType () != VariantValue::eNull) {
                    m.Add (i.fSerializedFieldName, vv);
                }
            }
            return VariantValue{m};
        };
        ToObjectMapperType<CLASS> toObjectMapper = [fields, baseClassTypeInfo, preflightBeforeToObject] (const ObjectVariantMapper& mapper, const VariantValue& d, CLASS* intoObjOfTypeT) -> void {
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"ObjectVariantMapper::TypeMappingDetails::{}::fToObjectMapper"};
#endif
            RequireNotNull (intoObjOfTypeT);
            VariantValue v2Decode = d;
            if (preflightBeforeToObject != nullptr) {
                preflightBeforeToObject (&v2Decode);
            }
            if (baseClassTypeInfo) {
                mapper.Lookup_ (typeid (BASE_CLASS)).ToObjectMapper<BASE_CLASS> () (mapper, d, intoObjOfTypeT);
            }
            Mapping<String, VariantValue> m = v2Decode.As<Mapping<String, VariantValue>> ();
            for (const auto& i : fields) {
                optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
#if Stroika_Foundation_DataExchange_ObjectVariantMapper_USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"fieldname = %s, offset=%d, present=%d", i.fSerializedFieldName.c_str (), i.fFieldMetaInfo.fOffset, o.has_value ());
#endif
                if (o) {
                    ToGenericObjectMapperType toGenericObjectMapper = i.fOverrideTypeMapper ? i.fOverrideTypeMapper->fToObjectMapper : mapper.Lookup_ (i.fFieldMetaInfo.fTypeInfo).fToObjectMapper;
                    toGenericObjectMapper (mapper, *o, reinterpret_cast<std::byte*> (intoObjOfTypeT) + i.fFieldMetaInfo.fOffset);
                }
            }
        };
        return TypeMappingDetails{forTypeInfo, fromObjectMapper, toObjectMapper};
    }
}

#endif /*_Stroika_Foundation_DataExchange_ObjectVariantMapper_inl_*/
