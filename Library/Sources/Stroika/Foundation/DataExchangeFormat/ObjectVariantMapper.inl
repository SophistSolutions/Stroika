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
            void    ObjectVariantMapper::RegisterClass (const Sequence<StructureFieldInfo>& fieldDescriptions)
            {
                RegisterTypeMapper (TypeMappingDetails (typeid (CLASS), sizeof (CLASS), fieldDescriptions));
            }
            template    <typename CLASS>
            void    ObjectVariantMapper::ToObject (const Memory::VariantValue& v, CLASS* into)
            {
                ToObject  (typeid (CLASS), v, reinterpret_cast<Byte*> (into));
            }
            template    <typename CLASS>
            CLASS    ObjectVariantMapper::ToObject (const Memory::VariantValue& v)
            {
                CLASS tmp;
                ToObject (v, &tmp);
                return tmp;
            }
            template    <typename CLASS>
            VariantValue    ObjectVariantMapper::FromObject (const CLASS& from)
            {
                return FromObject  (typeid (CLASS), reinterpret_cast<const Byte*> (&from));
            }
            template    <typename T>
            ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_Sequence ()
            {
                auto toVariantMapper = [] (ObjectVariantMapper * mapper, const Byte * objOfType) -> VariantValue {
                    Sequence<VariantValue> s;
                    const Sequence<T>*  actualMember    =   reinterpret_cast<const Sequence<T>*> (objOfType);
                    for (auto i : *actualMember) {
                        s.Append (mapper->FromObject<T> (i));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * into) -> void {
                    Sequence<VariantValue> s  =   d.As<Sequence<T>> ();
                    Sequence<T>*    actualInto  =   reinterpret_cast<Sequence<T>*> (into);
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
                auto toVariantMapper = [] (ObjectVariantMapper * mapper, const Byte * objOfType) -> VariantValue {
                    Sequence<VariantValue> s;
                    const Mapping<KEY_TYPE, VALUE_TYPE>*  actualMember    =   reinterpret_cast<const Mapping<KEY_TYPE, VALUE_TYPE>*> (objOfType);
                    for (auto i : *actualMember) {
                        Sequence<VariantValue>  encodedPair;
                        encodedPair.Append (mapper->FromObject<KEY_TYPE> (i.first));
                        encodedPair.Append (mapper->FromObject<VALUE_TYPE> (i.second));
                        s.Append (VariantValue (encodedPair));
                    }
                    return VariantValue (s);
                };
                auto fromVariantMapper = [] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * into) -> void {
                    Sequence<VariantValue>          s  =   d.As<Sequence<VariantValue>> ();
                    Mapping<KEY_TYPE, VALUE_TYPE>*  actualInto  =   reinterpret_cast<Mapping<KEY_TYPE, VALUE_TYPE>*> (into);
                    actualInto->clear ();
                    for (VariantValue encodedPair : s) {
                        Sequence<VariantValue>  p   =   p.As<Sequence<VariantValue>> ();
                        if (p.size () != 2) {
                            Execution::DoThrow<BadFormatExcepion> ();
                        }
                        actualInto->Add (mapper->ToObject<KEY_TYPE> (p[0]) mapper->ToObject<VALUE_TYPE> (p[1]));
                    }
                };
                return ObjectVariantMapper::TypeMappingDetails (typeid (Mapping<KEY_TYPE, VALUE_TYPE>), toVariantMapper, fromVariantMapper);
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
