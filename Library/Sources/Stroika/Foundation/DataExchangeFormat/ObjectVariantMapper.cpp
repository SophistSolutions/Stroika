/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Containers/Tally.h"
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
ObjectVariantMapper::TypeMappingDetails::TypeMappingDetails (
    const type_index& forTypeInfo,
    const std::function<VariantValue(ObjectVariantMapper* mapper, const Byte* objOfType)>& toVariantMapper,
    const std::function<void(ObjectVariantMapper* mapper, const VariantValue& d, Byte* into)>& fromVariantMapper
)
    : fForType (forTypeInfo)
    , fToVariantMapper (toVariantMapper)
    , fFromVariantMapper (fromVariantMapper)
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
        auto toVariantMapper = [] (ObjectVariantMapper * mapper, const Byte * objOfType) -> VariantValue {
            return VariantValue (static_cast<UseVariantType> (*reinterpret_cast<const T*> (objOfType)));
        };
        auto fromVariantMapper = [] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * into) -> void {
            *reinterpret_cast<T*> (into) = static_cast<T> (d.As<UseVariantType> ());
        };
        return ObjectVariantMapper::TypeMappingDetails (typeid (T), toVariantMapper, fromVariantMapper);
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
    RegisterSerializer (mkSerializerInfo_<String, String> ());

    {
        typedef Mapping<String, String>  ACTUAL_ELEMENT_TYPE;
        auto toVariantMapper = [] (ObjectVariantMapper * mapper, const Byte * objOfType) -> VariantValue {
            Mapping<String, VariantValue> m;
            const ACTUAL_ELEMENT_TYPE*  actualMember    =   reinterpret_cast<const ACTUAL_ELEMENT_TYPE*> (objOfType);
            for (auto i : *actualMember) {
                // really could do either way - but second more efficient
                //m.Add (i.first, mapper->Serialize (typeid (String), reinterpret_cast<const Byte*> (&i.second)));
                m.Add (i.first, i.second);
            }
            return VariantValue (m);
        };
        auto fromVariantMapper = [] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * into) -> void {
            Mapping<String, VariantValue> m  =   d.As<Mapping<String, VariantValue>> ();
            ACTUAL_ELEMENT_TYPE*    actualInto  =   reinterpret_cast<ACTUAL_ELEMENT_TYPE*> (into);
            actualInto->clear ();
            for (auto i : m) {
                // really could do either way - but second more efficient
                //actualInto->Add (i.first, mapper->Deserialize<String> (i.second));
                actualInto->Add (i.first, i.second.As<String> ());
            }
        };
        RegisterSerializer (ObjectVariantMapper::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
    }

    {
        typedef Mapping<String, VariantValue>    ACTUAL_ELEMENT_TYPE;
        auto toVariantMapper = [] (ObjectVariantMapper * mapper, const Byte * objOfType) -> VariantValue {
            const ACTUAL_ELEMENT_TYPE*  actualMember    =   reinterpret_cast<const ACTUAL_ELEMENT_TYPE*> (objOfType);
            return VariantValue (*actualMember);
        };
        auto fromVariantMapper = [] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * into) -> void {
            ACTUAL_ELEMENT_TYPE*    actualInto  =   reinterpret_cast<ACTUAL_ELEMENT_TYPE*> (into);
            *actualInto = d.As<Mapping<String, VariantValue>> ();
        };
        RegisterSerializer (ObjectVariantMapper::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
    }

    // TODO - ARRAY??? Maybe using Sequence???
    {
    }
}

VariantValue    ObjectVariantMapper::Serialize (const type_index& forTypeInfo, const Byte* objOfType)
{
    Require (Lookup_ (forTypeInfo).fToVariantMapper);
    return Lookup_ (forTypeInfo).fToVariantMapper (this, objOfType);
}

void    ObjectVariantMapper::Deserialize (const type_index& forTypeInfo, const VariantValue& d, Byte* into)
{
    Require (Lookup_ (forTypeInfo).fFromVariantMapper);
    Lookup_ (forTypeInfo).fFromVariantMapper (this, d, into);
}

ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::mkSerializerForStruct (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields)
{
#if     qDebug
    for (auto i : fields) {
        Require (i.fOffset < n);
    }
    {
        // assure each field unique
        Containers::Tally<size_t> t;
        for (auto i : fields) {
            t.Add (i.fOffset);
        }
        for (auto i : t) {
            Require (i.fCount == 1);
        }
    }
    {
        // Assure for each field type is registered
        for (auto i : fields) {
            Require (Lookup_ (i.fTypeInfo).fFromVariantMapper);
            Require (Lookup_ (i.fTypeInfo).fToVariantMapper);
        }
    }
#endif

    auto toVariantMapper = [fields] (ObjectVariantMapper * mapper, const Byte * objOfType) -> VariantValue {
        Mapping<String, VariantValue> m;
        for (auto i : fields) {
            const Byte* fieldObj = objOfType + i.fOffset;
            m.Add (i.fSerializedFieldName, mapper->Serialize (i.fTypeInfo, objOfType + i.fOffset));
        }
        return VariantValue (m);
    };
    auto fromVariantMapper = [fields] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * into) -> void {
        Mapping<String, VariantValue> m  =   d.As<Mapping<String, VariantValue>> ();
        for (auto i : fields) {
            Memory::Optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
            if (not o.empty ()) {
                mapper->Deserialize (i.fTypeInfo, *o, into + i.fOffset);
            }
        }
    };
    return TypeMappingDetails (forTypeInfo, toVariantMapper, fromVariantMapper);
}

ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::Lookup_ (const type_index& forTypeInfo) const
{
    TypeMappingDetails  foo (forTypeInfo, nullptr, nullptr);
    auto i  = fSerializers_.Lookup (foo);
    Require (i.IsPresent ());   // if not present, this is a usage error - only use types which are registered
    return *i;
}
