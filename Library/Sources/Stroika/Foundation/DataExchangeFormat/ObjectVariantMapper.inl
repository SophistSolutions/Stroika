/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_inl_
#define _Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Execution/Exceptions.h"

#include    "BadFormatException.h"
#include    "CheckedConverter.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {



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
            template    <typename CLASS>
            inline  void    ObjectVariantMapper::RegisterClass (const Sequence<StructureFieldInfo>& fieldDescriptions)
            {
                RegisterTypeMapper (TypeMappingDetails (typeid (CLASS), sizeof (CLASS), fieldDescriptions));
            }
            template    <typename CLASS>
            inline  void    ObjectVariantMapper::ToObject (const Memory::VariantValue& v, CLASS* into) const
            {
                ToObject  (typeid (CLASS), v, reinterpret_cast<Byte*> (into));
            }
            template    <typename CLASS>
            inline  CLASS    ObjectVariantMapper::ToObject (const Memory::VariantValue& v) const
            {
                CLASS tmp;
                ToObject (v, &tmp);
                return tmp;
            }
            template    <typename CLASS>
            inline  VariantValue    ObjectVariantMapper::FromObject (const CLASS& from) const
            {
                return FromObject  (typeid (CLASS), reinterpret_cast<const Byte*> (&from));
            }
            template <typename T, size_t SZ>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Array ()
            {
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    Sequence<VariantValue> s;
                    const T*  actualMember    =   reinterpret_cast<const T*> (fromObjOfTypeT);
                    for (auto i = actualMember; i < actualMember + SZ; ++i) {
                        s.Append (mapper->FromObject<T> (*i));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    Sequence<VariantValue> s  =   d.As<Sequence<T>> ();
                    T*  actualMember    =   reinterpret_cast<T*> (intoObjOfTypeT);
                    if (s.size () > SZ) {
                        DbgTrace ("Array ('%s') actual size %d out of range", typeid (T[SZ]).name (), static_cast<int> (s.size ()));
                        Execution::DoThrow<BadFormatException> (BadFormatException (L"Array size out of range"));
                    }
                    size_t idx = 0;
                    for (auto i : s) {
                        actualMember[idx++] = mapper->ToObject<T> (i);
                    }
                    while (idx < SZ) {
                        actualMember[idx++] = T ();
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Sequence<T>), toVariantMapper, fromVariantMapper);
            }
            template    <typename T>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Sequence ()
            {
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    Sequence<VariantValue> s;
                    const Sequence<T>*  actualMember    =   reinterpret_cast<const Sequence<T>*> (fromObjOfTypeT);
                    for (auto i : *actualMember) {
                        s.Append (mapper->FromObject<T> (i));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    Sequence<VariantValue> s  =   d.As<Sequence<T>> ();
                    Sequence<T>*    actualInto  =   reinterpret_cast<Sequence<T>*> (intoObjOfTypeT);
                    actualInto->clear ();
                    for (auto i : s) {
                        actualInto->Append (mapper->ToObject<T> (i));
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Sequence<T>), toVariantMapper, fromVariantMapper);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Mapping ()
            {
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    Sequence<VariantValue> s;
                    const Mapping<KEY_TYPE, VALUE_TYPE>*  actualMember    =   reinterpret_cast<const Mapping<KEY_TYPE, VALUE_TYPE>*> (fromObjOfTypeT);
                    for (auto i : *actualMember) {
                        Sequence<VariantValue>  encodedPair;
                        encodedPair.Append (mapper->FromObject<KEY_TYPE> (i.fKey));
                        encodedPair.Append (mapper->FromObject<VALUE_TYPE> (i.fValue));
                        s.Append (VariantValue (encodedPair));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    Sequence<VariantValue>          s  =   d.As<Sequence<VariantValue>> ();
                    Mapping<KEY_TYPE, VALUE_TYPE>*  actualInto  =   reinterpret_cast<Mapping<KEY_TYPE, VALUE_TYPE>*> (intoObjOfTypeT);
                    actualInto->clear ();
                    for (VariantValue encodedPair : s) {
                        Sequence<VariantValue>  p   =   p.As<Sequence<VariantValue>> ();
                        if (p.size () != 2) {
                            DbgTrace ("Mapping ('%s') element with item count (%d) other than 2", typeid (Mapping<KEY_TYPE, VALUE_TYPE>).name (), static_cast<int> (p.size ()));
                            Execution::DoThrow<BadFormatException> (BadFormatException (L"Mapping element with item count other than 2"));
                        }
                        actualInto->Add (mapper->ToObject<KEY_TYPE> (p[0]), mapper->ToObject<VALUE_TYPE> (p[1]));
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Mapping<KEY_TYPE, VALUE_TYPE>), toVariantMapper, fromVariantMapper);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_MappingWithStringishKey ()
            {
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    const Mapping<KEY_TYPE, VALUE_TYPE>*  actualMember    =   reinterpret_cast<const Mapping<KEY_TYPE, VALUE_TYPE>*> (fromObjOfTypeT);
                    Mapping<String, VariantValue> m;
                    for (Common::KeyValuePair<KEY_TYPE, VALUE_TYPE> i : *actualMember) {
                        m.Add (mapper->FromObject<KEY_TYPE> (i.fKey).template As<String> (), mapper->FromObject<VALUE_TYPE> (i.fValue));
                    }
                    return VariantValue (m);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    Mapping<String, VariantValue>    m  =   d.As<Mapping<String, VariantValue>> ();
                    Mapping<KEY_TYPE, VALUE_TYPE>*  actualInto  =   reinterpret_cast<Mapping<KEY_TYPE, VALUE_TYPE>*> (intoObjOfTypeT);
                    actualInto->clear ();
                    for (Common::KeyValuePair<String, VariantValue> p : m) {
                        actualInto->Add (mapper->ToObject<KEY_TYPE> (p.fKey), mapper->ToObject<VALUE_TYPE> (p.fValue));
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Mapping<KEY_TYPE, VALUE_TYPE>), toVariantMapper, fromVariantMapper);
            }
            template <typename RANGE_TYPE>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Range ()
            {
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    typedef typename RANGE_TYPE::ElementType    ElementType;
                    Mapping<String, VariantValue> m;
                    const RANGE_TYPE*  actualMember    =   reinterpret_cast<const RANGE_TYPE*> (fromObjOfTypeT);
                    m.Add (L"Begin", mapper->FromObject<ElementType> (actualMember->begin ()));
                    m.Add (L"End", mapper->FromObject<ElementType> (actualMember->end ()));
                    return VariantValue (m);
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    typedef typename RANGE_TYPE::ElementType    ElementType;
                    Mapping<String, VariantValue>          m  =   d.As<Mapping<String, VariantValue>> ();
                    RANGE_TYPE*  actualInto  =   reinterpret_cast<RANGE_TYPE*> (intoObjOfTypeT);
                    if (m.size () != 2) {
                        DbgTrace ("Range ('%s') element needs begin and end", typeid (RANGE_TYPE).name ());
                        Execution::DoThrow<BadFormatException> (BadFormatException (L"Range needs begin and end"));
                    }
                    if (not m.ContainsKey (L"Begin")) {
                        DbgTrace ("Range ('%s') needs begin", typeid (RANGE_TYPE).name ());
                        Execution::DoThrow<BadFormatException> (BadFormatException (L"Range needs 'Begin' element"));
                    }
                    if (not m.ContainsKey (L"End")) {
                        DbgTrace ("Range ('%s') needs end", typeid (RANGE_TYPE).name ());
                        Execution::DoThrow<BadFormatException> (BadFormatException (L"Range needs 'End' element"));
                    }
                    ElementType from    =   mapper->ToObject<ElementType> (*m.Lookup (L"Begin"));
                    ElementType to      =   mapper->ToObject<ElementType> (*m.Lookup (L"End"));
                    * actualInto = CheckedConverter_Range<RANGE_TYPE> (from, to);
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (RANGE_TYPE), toVariantMapper, fromVariantMapper);
            }
            template <typename ENUM_TYPE>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Enumeration ()
            {
                /*
                 *  Note: we cannot get the enumeration print names - in general. That would be nicer to read, but we dont have
                 *  the data, and this is simple and efficient.
                 */
                Require (std::is_enum<ENUM_TYPE>::value);
#if     qCompilerAndStdLib_Supports_TypeTraits_underlying_type
                typedef typename std::underlying_type<ENUM_TYPE>::type SerializeAsType;
#else
                typedef long long SerializeAsType;
#endif
                auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    const ENUM_TYPE*  actualMember    =   reinterpret_cast<const ENUM_TYPE*> (fromObjOfTypeT);
#if     qCompilerAndStdLib_Supports_TypeTraits_underlying_type
                    Assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE));
#endif
                    Assert (static_cast<ENUM_TYPE> (static_cast<SerializeAsType> (*actualMember)) == *actualMember);    // no round-trip loss
                    return VariantValue (static_cast<SerializeAsType> (*actualMember));
                };
                auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    ENUM_TYPE*  actualInto  =   reinterpret_cast<ENUM_TYPE*> (intoObjOfTypeT);
                    * actualInto = static_cast<ENUM_TYPE> (d.As<SerializeAsType> ());
#if     qCompilerAndStdLib_Supports_TypeTraits_underlying_type
                    Assert (sizeof (SerializeAsType) == sizeof (ENUM_TYPE));
#endif
                    Assert (static_cast<SerializeAsType> (*actualInto) == d.As<SerializeAsType> ());  // no round-trip loss
#if     qCompilerAndStdLib_Supports_CompareStronglyTypedEnums
                    if (not (ENUM_TYPE::eSTART <= *actualInto and * actualInto <= ENUM_TYPE::eEND)) {
                        DbgTrace ("Enumeration ('%s') value %d out of range", typeid (ENUM_TYPE).name (), static_cast<int> (*actualInto));
                        Execution::DoThrow<BadFormatException> (BadFormatException (L"Enumeration value out of range"));
                    }
#else
                    if (not (static_cast<underlying_type<ENUM_TYPE>::type> (ENUM_TYPE::eSTART) <= static_cast<underlying_type<ENUM_TYPE>::type> (*actualInto) and static_cast<underlying_type<ENUM_TYPE>::type> (*actualInto) <= static_cast<underlying_type<ENUM_TYPE>::type> (ENUM_TYPE::eEND))) {
                        DbgTrace ("Enumeration ('%s') value %d out of range", typeid (ENUM_TYPE).name (), static_cast<int> (*actualInto));
                        Execution::DoThrow<BadFormatException> (BadFormatException (L"Enumeration value out of range"));
                    }
#endif
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (ENUM_TYPE), toVariantMapper, fromVariantMapper);
            }

#if 0
            template    <typename T>
            inline  ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Sequence<T>> ()
            {
                return mkSerializerInfoFor_Sequence_<T> ();
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_inl_*/
