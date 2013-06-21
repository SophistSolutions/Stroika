/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_h_
#define _Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_h_    1

#include    "../StroikaPreComp.h"

#include    <set>           // just temporarily til I finish traits / compare customiztion support
#include    <type_traits>
#include    <typeindex>

#include    "../Characters/String.h"
#include    "../Containers/Mapping.h"
#include    "../Containers/Sequence.h"
#include    "../Memory/Common.h"
#include    "../Memory/VariantValue.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *          (USED IN HEALTHFRAME - BUT JUST A PROTOTYPE...
 *          PROBBALY MOVE THIS OUT OF FOUNDATION AND INTO A FRAMEWORK???
 *
 *  TODO:
 *
 *      @todo   Just a draft - think through what we really want here...
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            using   Characters::String;
            using   Containers::Mapping;
            using   Containers::Sequence;
            using   Memory::Byte;
            using   Memory::VariantValue;


            /**
             * CLASS IS COPYABLE. Make one instance, register your types into it and use this to
             *  serialized/deserialize
             */
            class   Serailizion {
                struct SerializerInfo {
                    type_index fForType;
                    std::function<VariantValue(const Byte* objOfType)>  fSerializer;
                    std::function<void(const VariantValue& d, Byte* into)>  fDeserializer;

                    SerializerInfo (const type_index& forTypeInfo, const std::function<VariantValue(const Byte* objOfType)>& serializer, const std::function<void(const VariantValue& d, Byte* into)>& deserializer)
                        : fForType (forTypeInfo)
                        , fSerializer (serializer)
                        , fDeserializer (deserializer) {
                    }
                    bool operator< (const SerializerInfo& rhs) const {
                        return (fForType < rhs.fForType);
                    }
                };

                void    ClearRegistry ();
                void    ResetToDefaultRegistry ();

                set<SerializerInfo> s_Serializers;  // need Stroika set with separate traits-based key extractor/compare function

                void    RegisterSerializer (const SerializerInfo& serializerInfo) {
                    s_Serializers.insert (serializerInfo);
                }
                void    RegisterCommonSerializers () {
                    RegisterSerializer (SerializerInfo (
                                            typeid (bool),
                    [] (const Byte * objOfType) -> VariantValue {
                        return VariantValue (*reinterpret_cast<const bool*> (objOfType));
                    },
                    [] (const VariantValue & d, Byte * into) -> void {
                        *reinterpret_cast<bool*> (into) = d.As<bool> ();
                    }
                                        )
                                       );
                    RegisterSerializer (SerializerInfo (
                                            typeid (Characters::String),
                    [] (const Byte * objOfType) -> VariantValue {
                        return VariantValue (*reinterpret_cast<const Characters::String*> (objOfType));
                    },
                    [] (const VariantValue & d, Byte * into) -> void {
                        *reinterpret_cast<Characters::String*> (into) =  d.As<Characters::String> ();
                    }
                                        )
                                       );
                }

                SerializerInfo  Lookup_(const type_index& forTypeInfo) {
                    SerializerInfo  foo (forTypeInfo, nullptr, nullptr);
                    auto i  = s_Serializers.find (foo);
                    if (i == s_Serializers.end ()) {
                        throw "OOPS";
                    }
                    return *i;
                }

                VariantValue    Serialize (const type_index& forTypeInfo, const Byte* objOfType) {
                    return Lookup_ (forTypeInfo).fSerializer (objOfType);
                }
                void    Deserialize (const type_index& forTypeInfo, const VariantValue& d, Byte* into) {
                    Lookup_ (forTypeInfo).fDeserializer (d, into);
                }


                struct TYPEINFO {
                    size_t      fOffset;
                    type_index  fTypeInfo;
                    String      fSerializedFieldName;


                    TYPEINFO(size_t fieldOffset = 0, type_index typeInfo = typeid(void), const String& serializedFieldName = String () )
                        : fOffset (fieldOffset)
                        , fTypeInfo (typeInfo)
                        , fSerializedFieldName (serializedFieldName) {
                    }
                };
                SerializerInfo  mkSerializerForStruct (const type_index& forTypeInfo, Sequence<TYPEINFO> fields) {
                    struct foo {
                        Sequence<TYPEINFO> fields;
                    };
                    shared_ptr<foo> fooptr (new foo ());
                    fooptr->fields = fields;
                    return SerializerInfo (
                               forTypeInfo,
                    [fooptr, this] (const Byte * objOfType) -> VariantValue {
                        Mapping<String, VariantValue> m;
                        for (auto i : fooptr->fields) {
                            const Byte* fieldObj = objOfType + i.fOffset;
                            m.Add (i.fSerializedFieldName, Serialize (i.fTypeInfo, objOfType + i.fOffset));
                        }
                        return VariantValue (m);
                    },
                    [fooptr, this] (const VariantValue & d, Byte * into) -> void {
                        Mapping<String, VariantValue> m  =   d.As<Mapping<String, VariantValue>> ();
                        for (auto i : fooptr->fields) {
                            Memory::Optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
                            if (not o.empty ()) {
                                Deserialize (i.fTypeInfo, *o, into + i.fOffset);
                            }
                        }
                    }
                           );
                }

                template    <typename CLASS>
                void    RegisterClass (Sequence<TYPEINFO> typeInfo) {
                    RegisterSerializer (mkSerializerForStruct (typeid (CLASS), typeInfo));
                }

                template    <typename CLASS>
                void    Deserialize (const Memory::VariantValue& v, CLASS* into) {
                    Deserialize  (typeid (CLASS), v, reinterpret_cast<Byte*> (into));
                }
                template    <typename CLASS>
                VariantValue    Serialize (const CLASS& from) {
                    return Serialize  (typeid (CLASS), reinterpret_cast<const Byte*> (&from));
                }
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ObjectVariantMapper.inl"

#endif  /*_Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_h_*/
