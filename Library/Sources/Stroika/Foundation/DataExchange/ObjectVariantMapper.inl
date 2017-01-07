/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_ObjectVariantMapper_inl_
#define _Stroika_Foundation_DataExchange_ObjectVariantMapper_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Characters/String_Constant.h"
#include    "../Characters/ToString.h"
#include    "../Containers/Adapters/Adder.h"
#include    "../Debug/Assertions.h"
#include    "../Execution/Exceptions.h"

#include    "BadFormatException.h"
#include    "CheckedConverter.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /*
             ********************************************************************************
             ********************** ObjectVariantMapper::StructFieldInfo ********************
             ********************************************************************************
             */
            inline  ObjectVariantMapper::StructFieldInfo::StructFieldInfo (const StructFieldMetaInfo& fieldMetaInfo, const String& serializedFieldName, NullFieldHandling nullFields)
                : fFieldMetaInfo (fieldMetaInfo)
                , fSerializedFieldName (serializedFieldName)
                , fNullFields (nullFields)
            {
                WeakAssert (false);         //_Deprecated_ ("USE StructFieldInfo (STRING,StructFieldMetaInfo) - reverse params - this deprecated v2.0a187")
            }
            inline  ObjectVariantMapper::StructFieldInfo::StructFieldInfo (const String& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo, NullFieldHandling nullFields)
                : fFieldMetaInfo (fieldMetaInfo)
                , fSerializedFieldName (serializedFieldName)
                , fNullFields (nullFields)
            {
            }
            template    <int SZ>
            inline ObjectVariantMapper::StructFieldInfo::StructFieldInfo (const wchar_t (&serializedFieldName)[SZ], const StructFieldMetaInfo& fieldMetaInfo, NullFieldHandling nullFields)
                : StructFieldInfo (Characters::String_Constant { serializedFieldName }, fieldMetaInfo, nullFields)
            {
            }


            /*
             ********************************************************************************
             *********** DataExchange::ObjectVariantMapper::TypeMappingDetails **************
             ********************************************************************************
             */
            inline  ObjectVariantMapper::TypeMappingDetails::TypeMappingDetails (const type_index& forTypeInfo, const ToVariantMapperType& toVariantMapper, const FromVariantMapperType& fromVariantMapper)
                : fForType (forTypeInfo)
                , fToVariantMapper (toVariantMapper)
                , fFromVariantMapper (fromVariantMapper)
            {
            }
            inline  bool ObjectVariantMapper::TypeMappingDetails::operator== (const TypeMappingDetails& rhs) const
            {
                return fForType == rhs.fForType;
            }
            inline  bool ObjectVariantMapper::TypeMappingDetails::operator< (const TypeMappingDetails& rhs) const
            {
                return fForType < rhs.fForType;
            }


            /*
             ********************************************************************************
             ************** DataExchange::ObjectVariantMapper::TypesRegistry ****************
             ********************************************************************************
             */
            inline  ObjectVariantMapper::TypesRegistry::TypesRegistry (const Mapping<type_index, TypeMappingDetails>& src)
                : fSerializers (src)
            {
            }
            inline  ObjectVariantMapper::TypesRegistry::TypesRegistry (const Set<TypeMappingDetails>& src)
            {
                src.Apply ([this](const TypeMappingDetails & t) { fSerializers.Add (t.fForType, t); });
            }
            inline  Memory::Optional<ObjectVariantMapper::TypeMappingDetails>    ObjectVariantMapper::TypesRegistry::Lookup (type_index t) const
            {
                return fSerializers.Lookup (t);
            }
            inline  void    ObjectVariantMapper::TypesRegistry::Add (const TypeMappingDetails& typeMapDetails)
            {
                fSerializers.Add (typeMapDetails.fForType, typeMapDetails);
            }
            inline  Traversal::Iterable<ObjectVariantMapper::TypeMappingDetails> ObjectVariantMapper::TypesRegistry::GetMappers () const
            {
                return fSerializers.MappedValues ();
            }



            /*
             ********************************************************************************
             ******************************** ObjectVariantMapper ***************************
             ********************************************************************************
             */
            inline  ObjectVariantMapper::TypesRegistry    ObjectVariantMapper::GetTypeMappingRegistry () const
            {
                return fTypeMappingRegistry_;
            }
            inline  void    ObjectVariantMapper::SetTypeMappingRegistry (const TypesRegistry& s)
            {
                fTypeMappingRegistry_ = s;
            }
            template    <typename T>
            inline  void    ObjectVariantMapper::AddCommonType ()
            {
                Add (MakeCommonSerializer<T> ());
            }
            template    <typename CLASS>
            inline  void    ObjectVariantMapper::AddClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, function<void(VariantValue*)> preflightBeforeToObject)
            {
#if     qDebug
                for (auto f : fieldDescriptions) {
                    (void)Lookup_ (f.fFieldMetaInfo.fTypeInfo); // for side-effect of internal Require
                }
#endif
                Add (MakeCommonSerializer_ForClassObject_ (typeid (CLASS), sizeof (CLASS), fieldDescriptions, preflightBeforeToObject, Memory::Optional<type_index> {}));
            }
            template    <typename CLASS, typename BASE_CLASS>
            inline  void    ObjectVariantMapper::AddSubClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, function<void(VariantValue*)> preflightBeforeToObject)
            {
#if     qDebug
                for (auto f : fieldDescriptions) {
                    (void)Lookup_ (f.fFieldMetaInfo.fTypeInfo); // for side-effect of internal Require
                }
#endif
                Add (MakeCommonSerializer_ForClassObject_ (typeid (CLASS), sizeof (CLASS), fieldDescriptions, preflightBeforeToObject, type_index { typeid (BASE_CLASS) }));
            }
            inline auto ObjectVariantMapper::ToObjectMapper (const type_index& forTypeInfo) const ->FromVariantMapperType
            {
                Require (Lookup_ (forTypeInfo).fFromVariantMapper);
                return Lookup_ (forTypeInfo).fFromVariantMapper;
            }
            template    <typename TYPE>
            inline  auto   ObjectVariantMapper::ToObjectMapper () const -> FromVariantMapperType
            {
                return ToObjectMapper (typeid (TYPE));
            }
            inline auto  ObjectVariantMapper::FromObjectMapper (const type_index& forTypeInfo) const -> ToVariantMapperType
            {
                Require (Lookup_ (forTypeInfo).fToVariantMapper);
                return Lookup_ (forTypeInfo).fToVariantMapper;
            }
            template    <typename TYPE>
            inline  void    ObjectVariantMapper::ToObject (const FromVariantMapperType& fromVariantMapper, const VariantValue& v, TYPE* into) const
            {
                RequireNotNull (into);
                RequireNotNull (fromVariantMapper);
                // LOGICALLY required but cannot compare == on std::function! Require (fromVariantMapper  == ToObject<TYPE> ());  // pass it in as optimization, but not change of semantics
                fromVariantMapper  (*this, v, reinterpret_cast<Byte*> (into));
            }
            template    <typename TYPE>
            inline  void    ObjectVariantMapper::ToObject (const VariantValue& v, TYPE* into) const
            {
                RequireNotNull (into);
                ToObject<TYPE> (ToObjectMapper<TYPE> (), v, reinterpret_cast<Byte*> (into));
            }
            template    <typename TYPE>
            inline  TYPE    ObjectVariantMapper::ToObject (const FromVariantMapperType& fromVariantMapper, const VariantValue& v) const
            {
                /*
                 *  NOTE: It is because of this line of code (the default CTOR for tmp) - that we ObjectVariantMapper requires
                 *  all its types to have a default constructor. To avoid that dependency, you may provide a template
                 *  specialization of this method, which passes specific (default) args to CLASS, and then they will be filled in/replaced
                 *  by the two argument ToObject.
                 */
                TYPE tmp;
                ToObject (fromVariantMapper, v, &tmp);
                return tmp;
            }
            template    <typename TYPE>
            inline  TYPE    ObjectVariantMapper::ToObject (const VariantValue& v) const
            {
                return ToObject<TYPE> (ToObjectMapper<TYPE> (), v);
            }
            template    <typename TYPE>
            inline  auto   ObjectVariantMapper::FromObjectMapper () const -> ToVariantMapperType
            {
                return FromObjectMapper (typeid (TYPE));
            }
            template    <typename TYPE>
            inline  VariantValue    ObjectVariantMapper::FromObject (const ToVariantMapperType& toVariantMapper, const TYPE& from) const
            {
                // LOGICALLY required but cannot compare == on std::function! Require (toVariantMapper  == FromObject<TYPE> ());  // pass it in as optimization, but not change of semantics
                return toVariantMapper (*this, reinterpret_cast<const Byte*> (&from));
            }
            template    <typename TYPE>
            inline  VariantValue    ObjectVariantMapper::FromObject (const TYPE& from) const
            {
                return FromObjectMapper<TYPE> () (*this, reinterpret_cast<const Byte*> (&from));
            }
            template    <typename ACTUAL_CONTAINER_TYPE>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_WithAdder ()
            {
                using   T   =   typename ACTUAL_CONTAINER_TYPE::value_type;
                auto toVariantMapper = [](const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    const ACTUAL_CONTAINER_TYPE*    actualMember    { reinterpret_cast<const ACTUAL_CONTAINER_TYPE*> (fromObjOfTypeT) };
                    Sequence<VariantValue>          s;
                    if (not actualMember->empty ())
                    {
                        ToVariantMapperType             valueMapper     { mapper.FromObjectMapper<T> () };
                        for (auto i : *actualMember) {
                            s.Append (mapper.FromObject<T> (valueMapper, i));
                        }
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [](const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    Sequence<VariantValue>      s        =  d.As<Sequence<VariantValue>> ();
                    if (not s.empty ())
                    {
                        FromVariantMapperType       valueMapper { mapper.ToObjectMapper<T> () };
                        ACTUAL_CONTAINER_TYPE*      actualInto  { reinterpret_cast<ACTUAL_CONTAINER_TYPE*> (intoObjOfTypeT) };
                        Assert (actualInto->empty ());
                        for (auto i : s) {
                            Containers::Adapters::Adder<ACTUAL_CONTAINER_TYPE>::Add (actualInto, mapper.ToObject<T> (valueMapper, i));
                        }
                    }
                };
                return TypeMappingDetails (typeid (ACTUAL_CONTAINER_TYPE), toVariantMapper, fromVariantMapper);
            }
            template    <typename T>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer ()
            {
                const T*  n = nullptr;    // arg unused, just for overloading
                return MakeCommonSerializer_ (n);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>*)
            {
                using   Characters::String_Constant;
                using   Containers::Bijection;
                auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    ToVariantMapperType                                 domainMapper    { mapper.FromObjectMapper<DOMAIN_TYPE> () };
                    ToVariantMapperType                                 rangeMapper     { mapper.FromObjectMapper<RANGE_TYPE> () };
                    Sequence<VariantValue>                              s;
                    const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>*   actualMember    { reinterpret_cast<const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>*> (fromObjOfTypeT) };
                    for (auto i : *actualMember)
                    {
                        Sequence<VariantValue>  encodedPair;
                        encodedPair.Append (mapper.FromObject<DOMAIN_TYPE> (domainMapper, i.first));
                        encodedPair.Append (mapper.FromObject<RANGE_TYPE> (rangeMapper, i.second));
                        s.Append (VariantValue (encodedPair));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    FromVariantMapperType                           domainMapper    { mapper.ToObjectMapper<DOMAIN_TYPE> () };
                    FromVariantMapperType                           rangeMapper     { mapper.ToObjectMapper<RANGE_TYPE> () };
                    Sequence<VariantValue>                          s               = d.As<Sequence<VariantValue>> ();
                    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>*     actualInto      { reinterpret_cast<Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>*> (intoObjOfTypeT) };
                    actualInto->clear ();
                    for (VariantValue encodedPair : s)
                    {
                        Sequence<VariantValue>  p   =   encodedPair.As<Sequence<VariantValue>> ();
                        if (p.size () != 2) {
                            DbgTrace (L"Bijection ('%s') element with item count (%d) other than 2", Characters::ToString (typeid (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>)).c_str (), static_cast<int> (p.size ()));
                            Execution::Throw (BadFormatException (String_Constant (L"Mapping element with item count other than 2")));
                        }
                        actualInto->Add (mapper.ToObject<DOMAIN_TYPE> (domainMapper, p[0]), mapper.ToObject<RANGE_TYPE> (rangeMapper, p[1]));
                    }
                };
                return TypeMappingDetails (typeid (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>), toVariantMapper, fromVariantMapper);
            }
            template    <typename T>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Containers::Collection<T>*)
            {
                return MakeCommonSerializer_WithAdder<Containers::Collection<T>> ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>*)
            {
                // NB: We default to aa because this is the overwhelmingly most commonly expected mapping (especially to JSON), but
                // MakeCommonSerializer_MappingAsArrayOfKeyValuePairs is more general. If KEY_TYPE is not convertible to a string, or you wish to strucutre
                // the VariantValue (typically think JSON representation) as an array of Key/Value pairs, then use MakeCommonSerializer_MappingAsArrayOfKeyValuePairs
                return MakeCommonSerializer_MappingWithStringishKey<Containers::Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>> ();
            }
            template    <typename T, typename TRAITS>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Memory::Optional<T, TRAITS>*)
            {
                using   Memory::Optional;
                auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    const Optional<T, TRAITS>*  actualMember    =   reinterpret_cast<const Optional<T, TRAITS>*> (fromObjOfTypeT);
                    if (actualMember->IsPresent ())
                    {
                        return mapper.FromObject<T> (**actualMember);
                    }
                    else {
                        return VariantValue ();
                    }
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    Optional<T, TRAITS>*    actualInto  =   reinterpret_cast<Optional<T, TRAITS>*> (intoObjOfTypeT);
                    // NOTE - until v2.0a100, this read if (d.empty ()) - but thats wrong beacuse it maps empty strings to null (missing) values
                    if (d.GetType () == VariantValue::Type::eNull)
                    {
                        actualInto->clear ();
                    }
                    else {
                        *actualInto = mapper.ToObject<T> (d);
                    }
                };
                return TypeMappingDetails (typeid (Optional<T, TRAITS>), toVariantMapper, fromVariantMapper);
            }
            template    <typename T, typename TRAITS>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Execution::Synchronized<T, TRAITS>*)
            {
                using   Execution::Synchronized;
                auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    const Synchronized<T, TRAITS>*  actualMember    =   reinterpret_cast<const Synchronized<T, TRAITS>*> (fromObjOfTypeT);
                    return mapper.FromObject<T> (**actualMember);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    Synchronized<T, TRAITS>*    actualInto  =   reinterpret_cast<Synchronized<T, TRAITS>*> (intoObjOfTypeT);
                    * actualInto = mapper.ToObject<T> (d);
                };
                return TypeMappingDetails (typeid (Synchronized<T, TRAITS>), toVariantMapper, fromVariantMapper);
            }
            template    <typename T>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Sequence<T>*)
            {
                return MakeCommonSerializer_WithAdder<Sequence<T>> ();
            }
            template    <typename T>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const vector<T>*)
            {
                return MakeCommonSerializer_WithAdder<vector<T>> ();
            }
            template    <typename T>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Set<T>*)
            {
                return MakeCommonSerializer_WithAdder<Set<T>> ();
            }
            template    <typename T>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Containers::SortedCollection<T>*)
            {
                return MakeCommonSerializer_WithAdder<Containers::SortedCollection<T>> ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Containers::SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>*)
            {
                // NB: We default to aa because this is the overwhelmingly most commonly expected mapping (especially to JSON), but
                // MakeCommonSerializer_MappingAsArrayOfKeyValuePairs is more general. If KEY_TYPE is not convertible to a string, or you wish to strucutre
                // the VariantValue (typically think JSON representation) as an array of Key/Value pairs, then use MakeCommonSerializer_MappingAsArrayOfKeyValuePairs
                return MakeCommonSerializer_MappingWithStringishKey<Containers::SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>> ();
            }
            template    <typename T>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Containers::SortedSet<T>*)
            {
                return MakeCommonSerializer_WithAdder<Containers::SortedSet<T>> ();
            }
            template    <typename T, typename TRAITS>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Traversal::DiscreteRange<T, TRAITS>*)
            {
                return MakeCommonSerializer_Range_<Traversal::DiscreteRange<T, TRAITS>> ();
            }
            template    <typename T, typename TRAITS>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Traversal::Range<T, TRAITS>*)
            {
                return MakeCommonSerializer_Range_<Traversal::Range<T, TRAITS>> ();
            }
            template    <typename T>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const T*, typename std::enable_if<std::is_enum<T>::value >::type*)
            {
                return MakeCommonSerializer_NamedEnumerations<T> ();
            }
            template    <typename T, size_t SZ>
            ObjectVariantMapper::TypeMappingDetails     ObjectVariantMapper::MakeCommonSerializer_ (const T (*)[SZ])
            {
                using   Characters::String_Constant;
                auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    ToVariantMapperType     valueMapper     { mapper.FromObjectMapper<T> () };     // optimization if > 1 array elt, and anti-optimization array.size == 0
                    Sequence<VariantValue>  s;
                    const T*                actualMember    { reinterpret_cast<const T*> (fromObjOfTypeT) };
                    for (auto i = actualMember; i < actualMember + SZ; ++i)
                    {
                        s.Append (mapper.FromObject<T> (valueMapper, *i));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    Sequence<VariantValue>  s               =   d.As<Sequence<VariantValue>> ();
                    T*                      actualMember    { reinterpret_cast<T*> (intoObjOfTypeT) };
                    if (s.size () > SZ)
                    {
                        DbgTrace (L"Array ('%s') actual size %d out of range", Characters::ToString (typeid (T[SZ])).c_str (), static_cast<int> (s.size ()));
                        Execution::Throw (BadFormatException (String_Constant (L"Array size out of range")));
                    }
                    FromVariantMapperType       valueMapper { mapper.ToObjectMapper<T> () };   // optimization if > 1 array elt, and anti-optimization array.size == 0
                    size_t idx = 0;
                    for (auto i : s)
                    {
                        actualMember[idx++] = mapper.ToObject<T> (valueMapper, i);
                    }
                    while (idx < SZ)
                    {
                        actualMember[idx++] = T ();
                    }
                };
                return TypeMappingDetails (typeid (T[SZ]), toVariantMapper, fromVariantMapper);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename ACTUAL_CONTAINER_TYPE>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_WithKeyValuePairAdd_ ()
            {
                using   Characters::String_Constant;
                auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    ToVariantMapperType             keyMapper       { mapper.FromObjectMapper<KEY_TYPE> () };
                    ToVariantMapperType             valueMapper     { mapper.FromObjectMapper<VALUE_TYPE> () };
                    Sequence<VariantValue>          s;
                    const ACTUAL_CONTAINER_TYPE*    actualMember    { reinterpret_cast<const ACTUAL_CONTAINER_TYPE*> (fromObjOfTypeT) };
                    for (auto i : *actualMember)
                    {
                        Sequence<VariantValue>  encodedPair;
                        encodedPair.Append (mapper.FromObject<KEY_TYPE> (keyMapper, i.fKey));
                        encodedPair.Append (mapper.FromObject<VALUE_TYPE> (valueMapper, i.fValue));
                        s.Append (VariantValue (encodedPair));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    /*
                     *  NB: When you mixup having an array and an object (say because of writing with
                     *  MakeCommonSerializer_ContainerWithStringishKey and reading back with this regular Mapping serializer?) or for other reasons,
                     *  the covnersion to d.As<Sequence<VariantValue>> () can fail with a format exception.
                     *
                     *  This requires you wrote with the above serializer.
                     */
                    FromVariantMapperType       keyMapper   { mapper.ToObjectMapper<KEY_TYPE> () };
                    FromVariantMapperType       valueMapper { mapper.ToObjectMapper<VALUE_TYPE> () };
                    Sequence<VariantValue>      s           = d.As<Sequence<VariantValue>> () ;
                    ACTUAL_CONTAINER_TYPE*      actualInto  { reinterpret_cast<ACTUAL_CONTAINER_TYPE*> (intoObjOfTypeT) };
                    actualInto->clear ();
                    for (VariantValue encodedPair : s)
                    {
                        Sequence<VariantValue>  p   =   encodedPair.As<Sequence<VariantValue>> ();
                        if (p.size () != 2) {
                            DbgTrace (L"Container with Key/Value pair ('%s') element with item count (%d) other than 2", Characters::ToString (typeid (ACTUAL_CONTAINER_TYPE)).c_str (), static_cast<int> (p.size ()));
                            Execution::Throw (BadFormatException (String_Constant (L"Container with Key/Value pair element with item count other than 2")));
                        }
                        actualInto->Add (mapper.ToObject<KEY_TYPE> (keyMapper, p[0]), mapper.ToObject<VALUE_TYPE> (valueMapper, p[1]));
                    }
                };
                return TypeMappingDetails (typeid (ACTUAL_CONTAINER_TYPE), toVariantMapper, fromVariantMapper);
            }
            template    <typename ENUM_TYPE>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap)
            {
                using   Characters::String_Constant;
                static_assert (std::is_enum<ENUM_TYPE>::value, "MakeCommonSerializer_NamedEnumerations only works for enum types");
                using   SerializeAsType     =   typename std::underlying_type<ENUM_TYPE>::type;
                static_assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE), "underlyingtype?");
                auto toVariantMapper = [nameMap] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    const ENUM_TYPE*  actualMember    =   reinterpret_cast<const ENUM_TYPE*> (fromObjOfTypeT);
                    Assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE));
                    Assert (static_cast<ENUM_TYPE> (static_cast<SerializeAsType> (*actualMember)) == *actualMember);    // no round-trip loss
                    return VariantValue (*nameMap.Lookup (*actualMember));
                };
                auto fromVariantMapper = [nameMap] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    ENUM_TYPE*  actualInto  =   reinterpret_cast<ENUM_TYPE*> (intoObjOfTypeT);
                    auto optVal = nameMap.InverseLookup (d.As<String> ());
                    if (optVal.IsMissing ())
                    {
                        DbgTrace (L"Enumeration ('%s') value '%s' out of range", Characters::ToString (typeid (ENUM_TYPE)).c_str (), d.As<String> ().c_str ());
                        Execution::Throw (BadFormatException (String_Constant (L"Enumeration value out of range")));
                    }
                    * actualInto = *optVal;
                };
                return TypeMappingDetails (typeid (ENUM_TYPE), toVariantMapper, fromVariantMapper);
            }
            template    <typename ENUM_TYPE>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap)
            {
                return MakeCommonSerializer_NamedEnumerations (Containers::Bijection<ENUM_TYPE, String> (nameMap));
            }
            template    <typename ENUM_TYPE>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_EnumAsInt ()
            {
                using   Characters::String_Constant;
                /*
                 *  Note: we cannot get the enumeration print names - in general. That would be nicer to read, but we dont have
                 *  the data, and this is simple and efficient.
                 *
                 *  See MakeCommonSerializer_NamedEnumerations
                 */
                static_assert (std::is_enum<ENUM_TYPE>::value, "This only works for enum types");
                using   SerializeAsType     =   typename std::underlying_type<ENUM_TYPE>::type;
                static_assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE), "underlyingtype?");
                auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    const ENUM_TYPE*  actualMember    =   reinterpret_cast<const ENUM_TYPE*> (fromObjOfTypeT);
                    Assert (static_cast<ENUM_TYPE> (static_cast<SerializeAsType> (*actualMember)) == *actualMember);    // no round-trip loss
                    return VariantValue (static_cast<SerializeAsType> (*actualMember));
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    ENUM_TYPE*  actualInto  =   reinterpret_cast<ENUM_TYPE*> (intoObjOfTypeT);
                    * actualInto = static_cast<ENUM_TYPE> (d.As<SerializeAsType> ());
                    Assert (static_cast<SerializeAsType> (*actualInto) == d.As<SerializeAsType> ());  // no round-trip loss
                    if (not (ENUM_TYPE::eSTART <= *actualInto and * actualInto < ENUM_TYPE::eEND))
                    {
                        DbgTrace (L"Enumeration ('%s') value %d out of range", Characters::ToString (typeid (ENUM_TYPE)).c_str (), static_cast<int> (*actualInto));
                        Execution::Throw (BadFormatException (String_Constant (L"Enumeration value out of range")));
                    }
                };
                return TypeMappingDetails (typeid (ENUM_TYPE), toVariantMapper, fromVariantMapper);
            }
            template    <typename ACTUAL_CONTAINTER_TYPE, typename KEY_TYPE, typename VALUE_TYPE>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_MappingWithStringishKey ()
            {
                auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    ToVariantMapperType             keyMapper       { mapper.FromObjectMapper<KEY_TYPE> () };
                    ToVariantMapperType             valueMapper     { mapper.FromObjectMapper<VALUE_TYPE> () };
                    const ACTUAL_CONTAINTER_TYPE*   actualMember    { reinterpret_cast<const ACTUAL_CONTAINTER_TYPE*> (fromObjOfTypeT) };
                    Mapping<String, VariantValue>   m;
                    for (Common::KeyValuePair<KEY_TYPE, VALUE_TYPE> i : *actualMember)
                    {
                        m.Add (mapper.FromObject<KEY_TYPE> (keyMapper, i.fKey).template As<String> (), mapper.FromObject<VALUE_TYPE> (valueMapper, i.fValue));
                    }
                    return VariantValue (m);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    FromVariantMapperType           keyMapper   { mapper.ToObjectMapper<KEY_TYPE> () };
                    FromVariantMapperType           valueMapper { mapper.ToObjectMapper<VALUE_TYPE> () };
                    Mapping<String, VariantValue>   m           { d.As<Mapping<String, VariantValue>> () };
                    ACTUAL_CONTAINTER_TYPE*         actualInto  { reinterpret_cast<ACTUAL_CONTAINTER_TYPE*> (intoObjOfTypeT) };
                    actualInto->clear ();
                    for (Common::KeyValuePair<String, VariantValue> p : m)
                    {
                        actualInto->Add (mapper.ToObject<KEY_TYPE> (keyMapper, p.fKey), mapper.ToObject<VALUE_TYPE> (valueMapper, p.fValue));
                    }
                };
                return TypeMappingDetails (typeid (ACTUAL_CONTAINTER_TYPE), toVariantMapper, fromVariantMapper);
            }
            template    <typename ACTUAL_CONTAINTER_TYPE, typename KEY_TYPE, typename VALUE_TYPE>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_MappingAsArrayOfKeyValuePairs ()
            {
                return MakeCommonSerializer_WithKeyValuePairAdd_<KEY_TYPE, VALUE_TYPE, ACTUAL_CONTAINTER_TYPE> ();
            }
            template <typename RANGE_TYPE>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Range_ ()
            {
                using   Characters::String_Constant;
                static  const   String_Constant kLowerBoundLabel_       { L"LowerBound" };
                static  const   String_Constant kUpperBoundLabel_       { L"UpperBound" };
                auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    using   value_type     =   typename RANGE_TYPE::value_type;
                    Mapping<String, VariantValue> m;
                    const RANGE_TYPE*  actualMember    =   reinterpret_cast<const RANGE_TYPE*> (fromObjOfTypeT);
                    if (actualMember->empty ())
                    {
                        return VariantValue ();
                    }
                    else {
                        ToVariantMapperType   valueMapper   { mapper.FromObjectMapper<value_type> () };
                        m.Add (kLowerBoundLabel_, mapper.FromObject<value_type> (valueMapper, actualMember->GetLowerBound ()));
                        m.Add (kUpperBoundLabel_, mapper.FromObject<value_type> (valueMapper, actualMember->GetUpperBound ()));
                        return VariantValue (m);
                    }
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    using   value_type     =   typename RANGE_TYPE::value_type;
                    Mapping<String, VariantValue>   m           { d.As<Mapping<String, VariantValue>> () };
                    RANGE_TYPE*                     actualInto  { reinterpret_cast<RANGE_TYPE*> (intoObjOfTypeT) };
                    if (m.empty ())
                    {
                        *actualInto = RANGE_TYPE ();    // empty maps to empty
                    }
                    else {
                        if (m.size () != 2)
                        {
                            DbgTrace (L"Range ('%s') element needs LowerBound and UpperBound", Characters::ToString (typeid (RANGE_TYPE)).c_str ());
                            Execution::Throw (BadFormatException (String_Constant (L"Range needs LowerBound and UpperBound")));
                        }
                        if (not m.ContainsKey (kLowerBoundLabel_))
                        {
                            DbgTrace (L"Range ('%s') element needs LowerBound", Characters::ToString (typeid (RANGE_TYPE)).c_str ());
                            Execution::Throw (BadFormatException (String_Constant (L"Range needs 'LowerBound' element")));
                        }
                        if (not m.ContainsKey (kUpperBoundLabel_))
                        {
                            DbgTrace (L"Range ('%s') element needs UpperBound", Characters::ToString (typeid (RANGE_TYPE)).c_str ());
                            Execution::Throw (BadFormatException (String_Constant (L"Range needs 'UpperBound' element")));
                        }
                        FromVariantMapperType   valueMapper { mapper.ToObjectMapper<value_type> () };
                        value_type              from        { mapper.ToObject<value_type> (valueMapper, *m.Lookup (kLowerBoundLabel_)) };
                        value_type              to          { mapper.ToObject<value_type> (valueMapper, *m.Lookup (kUpperBoundLabel_)) };
                        * actualInto = CheckedConverter_Range<RANGE_TYPE> (from, to);
                    }
                };
                return TypeMappingDetails (typeid (RANGE_TYPE), toVariantMapper, fromVariantMapper);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_ObjectVariantMapper_inl_*/
