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
                auto toVariantMapper = [] (ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    Sequence<VariantValue> s;
                    const T*  actualMember    =   reinterpret_cast<const T*> (fromObjOfTypeT);
                    for (auto i = actualMember; i < actualMember + SZ; ++i) {
                        s.Append (mapper->FromObject<T> (*i));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    Sequence<VariantValue> s  =   d.As<Sequence<T>> ();
                    T*  actualMember    =   reinterpret_cast<T*> (intoObjOfTypeT);
                    if (s.size () > SZ) {
                        Execution::DoThrow<BadFormatException> (BadFormatException ());
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
                auto toVariantMapper = [] (ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    Sequence<VariantValue> s;
                    const Sequence<T>*  actualMember    =   reinterpret_cast<const Sequence<T>*> (fromObjOfTypeT);
                    for (auto i : *actualMember) {
                        s.Append (mapper->FromObject<T> (i));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
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
                auto toVariantMapper = [] (ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    Sequence<VariantValue> s;
                    const Mapping<KEY_TYPE, VALUE_TYPE>*  actualMember    =   reinterpret_cast<const Mapping<KEY_TYPE, VALUE_TYPE>*> (fromObjOfTypeT);
                    for (auto i : *actualMember) {
                        Sequence<VariantValue>  encodedPair;
                        encodedPair.Append (mapper->FromObject<KEY_TYPE> (i.first));
                        encodedPair.Append (mapper->FromObject<VALUE_TYPE> (i.second));
                        s.Append (VariantValue (encodedPair));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    Sequence<VariantValue>          s  =   d.As<Sequence<VariantValue>> ();
                    Mapping<KEY_TYPE, VALUE_TYPE>*  actualInto  =   reinterpret_cast<Mapping<KEY_TYPE, VALUE_TYPE>*> (intoObjOfTypeT);
                    actualInto->clear ();
                    for (VariantValue encodedPair : s) {
                        Sequence<VariantValue>  p   =   p.As<Sequence<VariantValue>> ();
                        if (p.size () != 2) {
                            Execution::DoThrow<BadFormatException> (BadFormatException ());
                        }
                        actualInto->Add (mapper->ToObject<KEY_TYPE> (p[0]), mapper->ToObject<VALUE_TYPE> (p[1]));
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Mapping<KEY_TYPE, VALUE_TYPE>), toVariantMapper, fromVariantMapper);
            }
            template <typename RANGE_TYPE>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Range ()
            {
                auto toVariantMapper = [] (ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                    typedef typename RANGE_TYPE::ElementType    ElementType;
                    Sequence<VariantValue> s;
                    const RANGE_TYPE*  actualMember    =   reinterpret_cast<const RANGE_TYPE*> (fromObjOfTypeT);
                    s.Append (mapper->FromObject<ElementType> (actualMember->begin ()));
                    s.Append (mapper->FromObject<ElementType> (actualMember->end ()));
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                    typedef typename RANGE_TYPE::ElementType    ElementType;
                    Sequence<VariantValue>          s  =   d.As<Sequence<VariantValue>> ();
                    RANGE_TYPE*  actualInto  =   reinterpret_cast<RANGE_TYPE*> (intoObjOfTypeT);
                    actualInto->clear ();
                    if (p.size () != 2) {
                        Execution::DoThrow<BadFormatException> (BadFormatException ());
                    }
                    ElementType from    =   mapper->ToObject<ElementType> (p[0]);
                    ElementType to      =   mapper->ToObject<ElementType> (p[1]);
                    if (not (RANGE_TYPE::kMin <= from and from <= RANGE_TYPE::kMax)) {
                        Execution::DoThrow<BadFormatException> (BadFormatException ());
                    }
                    if (not (RANGE_TYPE::kMin <= to and to <= RANGE_TYPE::kMax)) {
                        Execution::DoThrow<BadFormatException> (BadFormatException ());
                    }
                    * actualInto = RANGE_TYPE (from, to);
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (RANGE_TYPE), toVariantMapper, fromVariantMapper);
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
