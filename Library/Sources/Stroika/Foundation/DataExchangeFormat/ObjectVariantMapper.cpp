/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"

#include    "ObjectVariantMapper.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;




/*
 ********************************************************************************
 ****************** DataExchangeFormat::ObjectVariantMapper *********************
 ********************************************************************************
 */
ObjectVariantMapper::ObjectVariantMapper ()
{
    RegisterCommonSerializers ();
}

void    ObjectVariantMapper::RegisterSerializer (const SerializerInfo& serializerInfo)
{
    fSerializers_.insert (serializerInfo);
}

void    ObjectVariantMapper::ClearRegistry ()
{
    fSerializers_.clear ();
}

void    ObjectVariantMapper::ResetToDefaultRegistry ()
{
    ClearRegistry ();
    RegisterCommonSerializers ();
}

namespace {
    template    <typename T, typename UseVariantType>
    ObjectVariantMapper::SerializerInfo mkSerializerInfo_ ()
    {
        auto serializer = [] (const Byte * objOfType) -> VariantValue {
            return VariantValue (static_cast<UseVariantType> (*reinterpret_cast<const T*> (objOfType)));
        };
        auto deserializer = [] (const VariantValue & d, Byte * into) -> void {
            *reinterpret_cast<T*> (into) = static_cast<T> (d.As<UseVariantType> ());
        };
        return ObjectVariantMapper::SerializerInfo (typeid (T), serializer, deserializer);
    }
}

void    ObjectVariantMapper::RegisterCommonSerializers ()
{
    RegisterSerializer (mkSerializerInfo_<bool, bool> ());
    RegisterSerializer (mkSerializerInfo_<int, int> ());
    RegisterSerializer (mkSerializerInfo_<float, VariantValue::FloatType> ());
    RegisterSerializer (mkSerializerInfo_<double, VariantValue::FloatType> ());
    RegisterSerializer (mkSerializerInfo_<String, String> ());
}

VariantValue    ObjectVariantMapper::Serialize (const type_index& forTypeInfo, const Byte* objOfType)
{
    return Lookup_ (forTypeInfo).fSerializer (objOfType);
}
void    ObjectVariantMapper::Deserialize (const type_index& forTypeInfo, const VariantValue& d, Byte* into)
{
    Lookup_ (forTypeInfo).fDeserializer (d, into);
}

ObjectVariantMapper::SerializerInfo  ObjectVariantMapper::mkSerializerForStruct (const type_index& forTypeInfo, Sequence<TYPEINFO> fields)
{
    // foo magic (could do cleaner?) to assure lifetime for whats captured in lambda
    struct foo {
        Sequence<TYPEINFO> fields;
    };
    shared_ptr<foo> fooptr (new foo ());
    fooptr->fields = fields;
    auto serializer = [fooptr, this] (const Byte * objOfType) -> VariantValue {
        Mapping<String, VariantValue> m;
        for (auto i : fooptr->fields) {
            const Byte* fieldObj = objOfType + i.fOffset;
            m.Add (i.fSerializedFieldName, Serialize (i.fTypeInfo, objOfType + i.fOffset));
        }
        return VariantValue (m);
    };
    auto deserializer = [fooptr, this] (const VariantValue & d, Byte * into) -> void {
        Mapping<String, VariantValue> m  =   d.As<Mapping<String, VariantValue>> ();
        for (auto i : fooptr->fields) {
            Memory::Optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
            if (not o.empty ()) {
                Deserialize (i.fTypeInfo, *o, into + i.fOffset);
            }
        }
    };
    return SerializerInfo (forTypeInfo, serializer, deserializer);
}

ObjectVariantMapper::SerializerInfo  ObjectVariantMapper::Lookup_(const type_index& forTypeInfo) const
{
    SerializerInfo  foo (forTypeInfo, nullptr, nullptr);
    auto i  = fSerializers_.find (foo);
    if (i == fSerializers_.end ()) {
        throw "OOPS";
    }
    return *i;
}

