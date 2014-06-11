/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_ObjectVariantMapper_inl_
#define _Stroika_Foundation_DataExchange_ObjectVariantMapper_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Characters/String_Constant.h"
#include    "../Execution/Exceptions.h"

#include    "BadFormatException.h"
#include    "CheckedConverter.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /*
             ********************************************************************************
             ******************* ObjectVariantMapper::StructureFieldInfo ********************
             ********************************************************************************
             */
            inline  ObjectVariantMapper::StructureFieldInfo::StructureFieldInfo (size_t fieldOffset, type_index typeInfo, const String& serializedFieldName)
                : fOffset (fieldOffset)
                , fTypeInfo (typeInfo)
                , fSerializedFieldName (serializedFieldName)
            {
            }


            /*
             ********************************************************************************
             ******************************** ObjectVariantMapper ***************************
             ********************************************************************************
             */
            inline  Set<ObjectVariantMapper::TypeMappingDetails>    ObjectVariantMapper::GetTypeMappingRegistry () const
            {
                return fSerializers_;
            }
            inline  void    ObjectVariantMapper::SetTypeMappingRegistry (const Set<TypeMappingDetails>& s)
            {
                fSerializers_ = s;
            }
            template    <typename T>
            inline  void    ObjectVariantMapper::AddCommonType ()
            {
                Add (MakeCommonSerializer<T> ());
            }
            template    <typename CLASS>
            inline  void    ObjectVariantMapper::AddClass (const Sequence<StructureFieldInfo>& fieldDescriptions)
            {
                Add (TypeMappingDetails (typeid (CLASS), sizeof (CLASS), fieldDescriptions));
            }
#if     !qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug
            template    <typename CLASS>
            void    ObjectVariantMapper::AddClass (const std::initializer_list<StructureFieldInfo>& fieldDescriptions)
            {
                return AddClass<CLASS> (Sequence<StructureFieldInfo> (fieldDescriptions));
            }
#endif
            template    <typename CLASS>
            void    ObjectVariantMapper::AddClass (const StructureFieldInfo* fieldDescriptionsStart, const StructureFieldInfo* fieldDescriptionsEnd)
            {
                return AddClass<CLASS> (Sequence<StructureFieldInfo> (fieldDescriptionsStart, fieldDescriptionsEnd));
            }
            template    <typename CLASS>
            inline  void    ObjectVariantMapper::ToObject (const VariantValue& v, CLASS* into) const
            {
                RequireNotNull (into);
                ToObject  (typeid (CLASS), v, reinterpret_cast<Byte*> (into));
            }
            template    <typename CLASS>
            inline  CLASS    ObjectVariantMapper::ToObject (const VariantValue& v) const
            {
                /*
                 *  NOTE: It is because of this line of code (the default CTOR for tmp) - that we ObjectVariantMapper requires
                 *  all its types to have a default constructor. To avoid that dependency, you may provide a template
                 *  specialization of this method, which passes specific (default) args to CLASS, and then they will be filled in/replaced
                 *  by the two argument ToObject.
                 */
                CLASS tmp;
                ToObject (v, &tmp);
                return tmp;
            }
            template    <typename CLASS>
            inline  VariantValue    ObjectVariantMapper::FromObject (const CLASS& from) const
            {
                return FromObject  (typeid (CLASS), reinterpret_cast<const Byte*> (&from));
            }
            template    <typename T>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer ()
            {
                const T*  n = nullptr;    // arg unused, just for overloading
                return MakeCommonSerializer_ (*n);
            }
            template    <typename T>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Memory::Optional<T>&)
            {
                using   Memory::Optional;
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    const Optional<T>*  actualMember    =   reinterpret_cast<const Optional<T>*> (fromObjOfTypeT);
                    if (actualMember->IsPresent ())
                    {
                        return mapper->FromObject<T> (**actualMember);
                    }
                    else {
                        return VariantValue ();
                    }
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    Optional<T>*    actualInto  =   reinterpret_cast<Optional<T>*> (intoObjOfTypeT);
                    if (d.empty ())
                    {
                        actualInto->clear ();
                    }
                    else {
                        *actualInto = mapper->ToObject<T> (d);
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Optional<T>), toVariantMapper, fromVariantMapper);
            }
            template    <typename T>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Sequence<T>&)
            {
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    Sequence<VariantValue> s;
                    const Sequence<T>*  actualMember    =   reinterpret_cast<const Sequence<T>*> (fromObjOfTypeT);
                    for (auto i : *actualMember)
                    {
                        s.Append (mapper->FromObject<T> (i));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    Sequence<VariantValue> s  =   d.As<Sequence<VariantValue>> ();
                    Sequence<T>*    actualInto  =   reinterpret_cast<Sequence<T>*> (intoObjOfTypeT);
                    actualInto->clear ();
                    for (auto i : s)
                    {
                        actualInto->Append (mapper->ToObject<T> (i));
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Sequence<T>), toVariantMapper, fromVariantMapper);
            }
            template    <typename T>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Set<T>&)
            {
                auto toVariantMapper = [](const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    Sequence<VariantValue> s;
                    const Set<T>*  actualMember = reinterpret_cast<const Set<T>*> (fromObjOfTypeT);
                    for (auto i : *actualMember)
                    {
                        s.Append (mapper->FromObject<T> (i));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [](const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    Sequence<VariantValue> s = d.As<Sequence<VariantValue>> ();
                    Set<T>*    actualInto = reinterpret_cast<Set<T>*> (intoObjOfTypeT);
                    actualInto->clear ();
                    for (auto i : s)
                    {
                        actualInto->Add (mapper->ToObject<T> (i));
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Set<T>), toVariantMapper, fromVariantMapper);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>&)
            {
                using   Characters::String_Constant;
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    Sequence<VariantValue> s;
                    const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>*  actualMember    =   reinterpret_cast<const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>*> (fromObjOfTypeT);
                    for (auto i : *actualMember)
                    {
                        Sequence<VariantValue>  encodedPair;
                        encodedPair.Append (mapper->FromObject<DOMAIN_TYPE> (i.first));
                        encodedPair.Append (mapper->FromObject<RANGE_TYPE> (i.second));
                        s.Append (VariantValue (encodedPair));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    Sequence<VariantValue>          s  =   d.As<Sequence<VariantValue>> ();
                    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>*  actualInto  =   reinterpret_cast<Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>*> (intoObjOfTypeT);
                    actualInto->clear ();
                    for (VariantValue encodedPair : s)
                    {
                        Sequence<VariantValue>  p   =   encodedPair.As<Sequence<VariantValue>> ();
                        if (p.size () != 2) {
                            DbgTrace ("Bijection ('%s') element with item count (%d) other than 2", typeid (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>).name (), static_cast<int> (p.size ()));
                            Execution::DoThrow<BadFormatException> (BadFormatException (String_Constant (L"Mapping element with item count other than 2")));
                        }
                        actualInto->Add (mapper->ToObject<DOMAIN_TYPE> (p[0]), mapper->ToObject<RANGE_TYPE> (p[1]));
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>), toVariantMapper, fromVariantMapper);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>&)
            {
                using   Characters::String_Constant;
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    Sequence<VariantValue> s;
                    const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>*  actualMember    =   reinterpret_cast<const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>*> (fromObjOfTypeT);
                    for (auto i : *actualMember)
                    {
                        Sequence<VariantValue>  encodedPair;
                        encodedPair.Append (mapper->FromObject<KEY_TYPE> (i.fKey));
                        encodedPair.Append (mapper->FromObject<VALUE_TYPE> (i.fValue));
                        s.Append (VariantValue (encodedPair));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    /*
                     *  NB: When you mixup having an array and an object (say because of writing with
                     *  MakeCommonSerializer_MappingWithStringishKeym and reading back with this regular Mapping serializer?) or for other reasons,
                     *  the covnersion to d.As<Sequence<VariantValue>> () can fail with a format exception.
                     *
                     *  This requires you wrote with the above serializer.
                     */
                    Sequence<VariantValue>          s  =   d.As<Sequence<VariantValue>> ();
                    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>*  actualInto  =   reinterpret_cast<Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>*> (intoObjOfTypeT);
                    actualInto->clear ();
                    for (VariantValue encodedPair : s)
                    {
                        Sequence<VariantValue>  p   =   encodedPair.As<Sequence<VariantValue>> ();
                        if (p.size () != 2) {
                            DbgTrace ("Mapping ('%s') element with item count (%d) other than 2", typeid (Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>).name (), static_cast<int> (p.size ()));
                            Execution::DoThrow<BadFormatException> (BadFormatException (String_Constant (L"Mapping element with item count other than 2")));
                        }
                        actualInto->Add (mapper->ToObject<KEY_TYPE> (p[0]), mapper->ToObject<VALUE_TYPE> (p[1]));
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>), toVariantMapper, fromVariantMapper);
            }
            template    <typename T, size_t SZ>
#if 1
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Array ()
#else
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (T ar[SZ])
#endif
            {
                using   Characters::String_Constant;
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    Sequence<VariantValue> s;
                    const T*  actualMember    =   reinterpret_cast<const T*> (fromObjOfTypeT);
                    for (auto i = actualMember; i < actualMember + SZ; ++i)
                    {
                        s.Append (mapper->FromObject<T> (*i));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    Sequence<VariantValue> s  =   d.As<Sequence<VariantValue>> ();
                    T*  actualMember    =   reinterpret_cast<T*> (intoObjOfTypeT);
                    if (s.size () > SZ)
                    {
                        DbgTrace ("Array ('%s') actual size %d out of range", typeid (T[SZ]).name (), static_cast<int> (s.size ()));
                        Execution::DoThrow<BadFormatException> (BadFormatException (String_Constant (L"Array size out of range")));
                    }
                    size_t idx = 0;
                    for (auto i : s)
                    {
                        actualMember[idx++] = mapper->ToObject<T> (i);
                    }
                    while (idx < SZ)
                    {
                        actualMember[idx++] = T ();
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (T[SZ]), toVariantMapper, fromVariantMapper);
            }
            template    <typename T, typename TRAITS>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Traversal::DiscreteRange<T, TRAITS>&)
            {
                return MakeCommonSerializer_Range_<Traversal::DiscreteRange<T, TRAITS>> ();
            }
            template    <typename T, typename TRAITS>
            inline  ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const Traversal::Range<T, TRAITS>&)
            {
                return MakeCommonSerializer_Range_<Traversal::Range<T, TRAITS>> ();
            }
            template    <typename ENUM_TYPE>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_ (const ENUM_TYPE&,  typename std::enable_if<std::is_enum<ENUM_TYPE>::value >::type*)
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
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    const ENUM_TYPE*  actualMember    =   reinterpret_cast<const ENUM_TYPE*> (fromObjOfTypeT);
                    Assert (static_cast<ENUM_TYPE> (static_cast<SerializeAsType> (*actualMember)) == *actualMember);    // no round-trip loss
                    return VariantValue (static_cast<SerializeAsType> (*actualMember));
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    ENUM_TYPE*  actualInto  =   reinterpret_cast<ENUM_TYPE*> (intoObjOfTypeT);
                    * actualInto = static_cast<ENUM_TYPE> (d.As<SerializeAsType> ());
                    Assert (static_cast<SerializeAsType> (*actualInto) == d.As<SerializeAsType> ());  // no round-trip loss
#if     !qCompilerAndStdLib_CompareStronglyTypedEnums_Buggy
                    if (not (ENUM_TYPE::eSTART <= *actualInto and * actualInto <= ENUM_TYPE::eEND))
                    {
                        DbgTrace ("Enumeration ('%s') value %d out of range", typeid (ENUM_TYPE).name (), static_cast<int> (*actualInto));
                        Execution::DoThrow<BadFormatException> (BadFormatException (String_Constant (L"Enumeration value out of range")));
                    }
#else
                    if (not (static_cast<typename underlying_type<ENUM_TYPE>::type> (ENUM_TYPE::eSTART) <= static_cast<typename underlying_type<ENUM_TYPE>::type> (*actualInto) and static_cast<typename underlying_type<ENUM_TYPE>::type> (*actualInto) <= static_cast<typename underlying_type<ENUM_TYPE>::type> (ENUM_TYPE::eEND)))
                    {
                        DbgTrace ("Enumeration ('%s') value %d out of range", typeid (ENUM_TYPE).name (), static_cast<int> (*actualInto));
                        Execution::DoThrow<BadFormatException> (BadFormatException (String_Constant (L"Enumeration value out of range")));
                    }
#endif
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (ENUM_TYPE), toVariantMapper, fromVariantMapper);
            }
            template    <typename ENUM_TYPE>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer_NamedEnumerations (const Bijection<ENUM_TYPE, String>& nameMap)
            {
                using   Characters::String_Constant;
                static_assert (std::is_enum<ENUM_TYPE>::value, "MakeCommonSerializer_NamedEnumerations only works for enum types");
                using   SerializeAsType     =   typename std::underlying_type<ENUM_TYPE>::type;
                static_assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE), "underlyingtype?");
                auto toVariantMapper = [nameMap] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    const ENUM_TYPE*  actualMember    =   reinterpret_cast<const ENUM_TYPE*> (fromObjOfTypeT);
                    Assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE));
                    Assert (static_cast<ENUM_TYPE> (static_cast<SerializeAsType> (*actualMember)) == *actualMember);    // no round-trip loss
                    return VariantValue (*nameMap.Lookup (*actualMember));
                };
                auto fromVariantMapper = [nameMap] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    ENUM_TYPE*  actualInto  =   reinterpret_cast<ENUM_TYPE*> (intoObjOfTypeT);
                    auto optVal = nameMap.InverseLookup (d.As<String> ());
                    if (optVal.IsMissing ())
                    {
                        DbgTrace ("Enumeration ('%s') value %d out of range", typeid (ENUM_TYPE).name (), d.As<String> ().c_str ());
                        Execution::DoThrow<BadFormatException> (BadFormatException (String_Constant (L"Enumeration value out of range")));
                    }
                    * actualInto = *optVal;
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (ENUM_TYPE), toVariantMapper, fromVariantMapper);
            }
            template <typename RANGE_TYPE>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Range_ ()
            {
                using   Characters::String_Constant;
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    using   ElementType     =   typename RANGE_TYPE::ElementType;
                    Mapping<String, VariantValue> m;
                    const RANGE_TYPE*  actualMember    =   reinterpret_cast<const RANGE_TYPE*> (fromObjOfTypeT);
                    if (actualMember->empty ())
                    {
                        return VariantValue ();
                    }
                    else {
                        m.Add (String_Constant (L"LowerBound"), mapper->FromObject<ElementType> (actualMember->GetLowerBound ()));
                        m.Add (String_Constant (L"UpperBound"), mapper->FromObject<ElementType> (actualMember->GetUpperBound ()));
                        return VariantValue (m);
                    }
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    using   ElementType     =   typename RANGE_TYPE::ElementType;
                    Mapping<String, VariantValue>          m  =   d.As<Mapping<String, VariantValue>> ();
                    RANGE_TYPE*  actualInto  =   reinterpret_cast<RANGE_TYPE*> (intoObjOfTypeT);
                    if (m.empty ())
                    {
                        *actualInto = RANGE_TYPE ();    // empty maps to empty
                    }
                    else {
                        if (m.size () != 2)
                        {
                            DbgTrace ("Range ('%s') element needs LowerBound and UpperBound", typeid (RANGE_TYPE).name ());
                            Execution::DoThrow<BadFormatException> (BadFormatException (String_Constant (L"Range needs LowerBound and UpperBound")));
                        }
                        // temporary backward compat -- LGP 2013-11-01
#if 0
                        if (1)
                        {
                            if (not m.ContainsKey (L"LowerBound") and m.ContainsKey (L"Begin")) {
                                m.Add (L"LowerBound", m.LookupValue (L"Begin"));
                            }
                            if (not m.ContainsKey (L"UpperBound") and m.ContainsKey (L"End")) {
                                m.Add (String_Constant (L"UpperBound"), m.LookupValue (L"End"));
                            }
                        }
#endif
                        if (not m.ContainsKey (String_Constant (L"LowerBound")))
                        {
                            DbgTrace ("Range ('%s') needs LowerBound", typeid (RANGE_TYPE).name ());
                            Execution::DoThrow<BadFormatException> (BadFormatException (String_Constant (L"Range needs 'LowerBound' element")));
                        }
                        if (not m.ContainsKey (String_Constant (L"UpperBound")))
                        {
                            DbgTrace ("Range ('%s') needs UpperBound", typeid (RANGE_TYPE).name ());
                            Execution::DoThrow<BadFormatException> (BadFormatException (String_Constant (L"Range needs 'UpperBound' element")));
                        }
                        ElementType from = mapper->ToObject<ElementType> (*m.Lookup (String_Constant (L"LowerBound")));
                        ElementType to = mapper->ToObject<ElementType> (*m.Lookup (String_Constant (L"UpperBound")));
                        * actualInto = CheckedConverter_Range<RANGE_TYPE> (from, to);
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (RANGE_TYPE), toVariantMapper, fromVariantMapper);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_MappingWithStringishKey ()
            {
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    RequireNotNull (fromObjOfTypeT);
                    const Mapping<KEY_TYPE, VALUE_TYPE>*  actualMember    =   reinterpret_cast<const Mapping<KEY_TYPE, VALUE_TYPE>*> (fromObjOfTypeT);
                    Mapping<String, VariantValue> m;
                    for (Common::KeyValuePair<KEY_TYPE, VALUE_TYPE> i : *actualMember)
                    {
                        m.Add (mapper->FromObject<KEY_TYPE> (i.fKey).template As<String> (), mapper->FromObject<VALUE_TYPE> (i.fValue));
                    }
                    return VariantValue (m);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    RequireNotNull (intoObjOfTypeT);
                    Mapping<String, VariantValue>    m  =   d.As<Mapping<String, VariantValue>> ();
                    Mapping<KEY_TYPE, VALUE_TYPE>*  actualInto  =   reinterpret_cast<Mapping<KEY_TYPE, VALUE_TYPE>*> (intoObjOfTypeT);
                    actualInto->clear ();
                    for (Common::KeyValuePair<String, VariantValue> p : m)
                    {
                        actualInto->Add (mapper->ToObject<KEY_TYPE> (p.fKey), mapper->ToObject<VALUE_TYPE> (p.fValue));
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Mapping<KEY_TYPE, VALUE_TYPE>), toVariantMapper, fromVariantMapper);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_ObjectVariantMapper_inl_*/
