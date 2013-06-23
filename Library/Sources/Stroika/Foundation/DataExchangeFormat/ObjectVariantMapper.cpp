/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Time/Date.h"
#include    "../Time/DateTime.h"

#include    "ObjectVariantMapper.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;

using   Time::Date;
using   Time::DateTime;



/*
 ********************************************************************************
 ******* DataExchangeFormat::ObjectVariantMapper::TypeMappingDetails ************
 ********************************************************************************
 */
ObjectVariantMapper::TypeMappingDetails::TypeMappingDetails (const type_index& forTypeInfo, const std::function<VariantValue(ObjectVariantMapper* mapper, const Byte* objOfType)>& serializer, const std::function<void(ObjectVariantMapper* mapper, const VariantValue& d, Byte* into)>& deserializer)
    : fForType (forTypeInfo)
    , fSerializer (serializer)
    , fDeserializer (deserializer)
{
}


/*
 ********************************************************************************
 ****************** DataExchangeFormat::ObjectVariantMapper *********************
 ********************************************************************************
 */
ObjectVariantMapper::ObjectVariantMapper ()
{
    RegisterCommonSerializers ();
}

void    ObjectVariantMapper::RegisterSerializer (const TypeMappingDetails& serializerInfo)
{
    fSerializers_.Add (serializerInfo);
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
    ObjectVariantMapper::TypeMappingDetails mkSerializerInfo_ ()
    {
        auto serializer = [] (ObjectVariantMapper * mapper, const Byte * objOfType) -> VariantValue {
            return VariantValue (static_cast<UseVariantType> (*reinterpret_cast<const T*> (objOfType)));
        };
        auto deserializer = [] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * into) -> void {
            *reinterpret_cast<T*> (into) = static_cast<T> (d.As<UseVariantType> ());
        };
        return ObjectVariantMapper::TypeMappingDetails (typeid (T), serializer, deserializer);
    }
}

void    ObjectVariantMapper::RegisterCommonSerializers ()
{
    RegisterSerializer (mkSerializerInfo_<bool, bool> ());
    RegisterSerializer (mkSerializerInfo_<int, int> ());
    RegisterSerializer (mkSerializerInfo_<float, VariantValue::FloatType> ());
    RegisterSerializer (mkSerializerInfo_<double, VariantValue::FloatType> ());
    RegisterSerializer (mkSerializerInfo_<Date, Date> ());
    RegisterSerializer (mkSerializerInfo_<DateTime, DateTime> ());
    /// TODO - ARRAY??? Maybe using Sequence???
    RegisterSerializer (mkSerializerInfo_<String, String> ());
}

VariantValue    ObjectVariantMapper::Serialize (const type_index& forTypeInfo, const Byte* objOfType)
{
    return Lookup_ (forTypeInfo).fSerializer (this, objOfType);
}

void    ObjectVariantMapper::Deserialize (const type_index& forTypeInfo, const VariantValue& d, Byte* into)
{
    Lookup_ (forTypeInfo).fDeserializer (this, d, into);
}

ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::mkSerializerForStruct (const type_index& forTypeInfo, const Sequence<StructureFieldInfo>& fields)
{
    // foo magic (could do cleaner?) to assure lifetime for whats captured in lambda
    struct foo {
        Sequence<StructureFieldInfo> fields;
    };
    shared_ptr<foo> fooptr (new foo ());
    fooptr->fields = fields;
    auto serializer = [fooptr] (ObjectVariantMapper * mapper, const Byte * objOfType) -> VariantValue {
        Mapping<String, VariantValue> m;
        for (auto i : fooptr->fields) {
            const Byte* fieldObj = objOfType + i.fOffset;
            m.Add (i.fSerializedFieldName, mapper->Serialize (i.fTypeInfo, objOfType + i.fOffset));
        }
        return VariantValue (m);
    };
    auto deserializer = [fooptr] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * into) -> void {
        Mapping<String, VariantValue> m  =   d.As<Mapping<String, VariantValue>> ();
        for (auto i : fooptr->fields) {
            Memory::Optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
            if (not o.empty ()) {
                mapper->Deserialize (i.fTypeInfo, *o, into + i.fOffset);
            }
        }
    };
    return TypeMappingDetails (forTypeInfo, serializer, deserializer);
}

ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::Lookup_(const type_index& forTypeInfo) const
{
    TypeMappingDetails  foo (forTypeInfo, nullptr, nullptr);
    auto i  = fSerializers_.Lookup (foo);
    if (i.empty ()) {
        throw "OOPS";
    }
    return *i;
}

