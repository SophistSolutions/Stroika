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

ObjectVariantMapper::TypeMappingDetails::TypeMappingDetails (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields)
    : fForType (forTypeInfo)
    , fToVariantMapper ()
    , fFromVariantMapper ()
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
#if 0
    // GOOD TODO but cannot since no longer a member of the ObjectMapper class...
    {
        // Assure for each field type is registered
        for (auto i : fields) {
            Require (Lookup_ (i.fTypeInfo).fFromVariantMapper);
            Require (Lookup_ (i.fTypeInfo).fToVariantMapper);
        }
    }
#endif
#endif

    fToVariantMapper = [fields] (ObjectVariantMapper * mapper, const Byte * objOfType) -> VariantValue {
        Mapping<String, VariantValue> m;
        for (auto i : fields) {
            const Byte* fieldObj = objOfType + i.fOffset;
            m.Add (i.fSerializedFieldName, mapper->FromObject (i.fTypeInfo, objOfType + i.fOffset));
        }
        return VariantValue (m);
    };
    fFromVariantMapper = [fields] (ObjectVariantMapper * mapper, const VariantValue & d, Byte * into) -> void {
        Mapping<String, VariantValue> m  =   d.As<Mapping<String, VariantValue>> ();
        for (auto i : fields) {
            Memory::Optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
            if (not o.empty ()) {
                mapper->ToObject (i.fTypeInfo, *o, into + i.fOffset);
            }
        }
    };
}


/*
 ********************************************************************************
 ****************** DataExchangeFormat::ObjectVariantMapper *********************
 ********************************************************************************
 */
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
    Set<ObjectVariantMapper::TypeMappingDetails>    mkCommonSerializers_ ()
    {
        Set<ObjectVariantMapper::TypeMappingDetails>    result;
        result.Add (mkSerializerInfo_<bool, bool> ());
        result.Add (mkSerializerInfo_<int, int> ());
        // @todo - fixup int type serialization (bigger int types - probably need to extend VariantValue class - and maybe should for unsigned?
        result.Add (mkSerializerInfo_<unsigned int, int> ());
        result.Add (mkSerializerInfo_<long, int> ());
        result.Add (mkSerializerInfo_<unsigned long, int> ());
        result.Add (mkSerializerInfo_<float, VariantValue::FloatType> ());
        result.Add (mkSerializerInfo_<double, VariantValue::FloatType> ());
        result.Add (mkSerializerInfo_<Date, Date> ());
        result.Add (mkSerializerInfo_<DateTime, DateTime> ());
        result.Add (mkSerializerInfo_<String, String> ());

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
                    //actualInto->Add (i.first, mapper->ToObject<String> (i.second));
                    actualInto->Add (i.first, i.second.As<String> ());
                }
            };
            result.Add (ObjectVariantMapper::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
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
            result.Add (ObjectVariantMapper::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
        }

        // TODO - ARRAY??? Maybe using Sequence???
        {
        }
        return result;
    }

    // Construct the default map once, so that it never needs be re-created (though it could easily get cloned when modified)
    Set<ObjectVariantMapper::TypeMappingDetails>    GetDefaultTypeMappers_ ()
    {
        static  Set<ObjectVariantMapper::TypeMappingDetails>    sDefaults_ = mkCommonSerializers_ ();
        return sDefaults_;
    }
}


ObjectVariantMapper::ObjectVariantMapper ()
    : fSerializers_ (GetDefaultTypeMappers_ ())
{
}

void    ObjectVariantMapper::RegisterTypeMapper (const TypeMappingDetails& serializerInfo)
{
    fSerializers_.Add (serializerInfo);
}

void    ObjectVariantMapper::ResetToDefaultTypeRegistry ()
{
    fSerializers_ = GetDefaultTypeMappers_ ();
}

VariantValue    ObjectVariantMapper::FromObject (const type_index& forTypeInfo, const Byte* objOfType)
{
    Require (Lookup_ (forTypeInfo).fToVariantMapper);
    return Lookup_ (forTypeInfo).fToVariantMapper (this, objOfType);
}

void    ObjectVariantMapper::ToObject (const type_index& forTypeInfo, const VariantValue& d, Byte* into)
{
    Require (Lookup_ (forTypeInfo).fFromVariantMapper);
    Lookup_ (forTypeInfo).fFromVariantMapper (this, d, into);
}

ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::Lookup_ (const type_index& forTypeInfo) const
{
    TypeMappingDetails  foo (forTypeInfo, nullptr, nullptr);
    auto i  = fSerializers_.Lookup (foo);
    Require (i.IsPresent ());   // if not present, this is a usage error - only use types which are registered
    return *i;
}
