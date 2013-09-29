/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Containers/Tally.h"
#include    "../Debug/Trace.h"
#include    "../Time/Date.h"
#include    "../Time/DateTime.h"
#include    "../Time/Duration.h"

#include    "ObjectVariantMapper.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;

using   Time::Date;
using   Time::DateTime;
using   Time::Duration;
using   Time::TimeOfDay;



/*
 ********************************************************************************
 ******* DataExchangeFormat::ObjectVariantMapper::TypeMappingDetails ************
 ********************************************************************************
 */
ObjectVariantMapper::TypeMappingDetails::TypeMappingDetails (
    const type_index& forTypeInfo,
    const std::function<VariantValue(const ObjectVariantMapper* mapper, const Byte* objOfType)>& toVariantMapper,
    const std::function<void(const ObjectVariantMapper* mapper, const VariantValue& d, Byte* into)>& fromVariantMapper
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

    fToVariantMapper = [fields] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        Mapping<String, VariantValue> m;
        for (auto i : fields) {
            const Byte* fieldObj = fromObjOfTypeT + i.fOffset;
            m.Add (i.fSerializedFieldName, mapper->FromObject (i.fTypeInfo, fromObjOfTypeT + i.fOffset));
        }
        return VariantValue (m);
    };
    fFromVariantMapper = [fields] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        Mapping<String, VariantValue> m  =   d.As<Mapping<String, VariantValue>> ();
        for (auto i : fields) {
            Memory::Optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
            if (not o.empty ()) {
                mapper->ToObject (i.fTypeInfo, *o, intoObjOfTypeT + i.fOffset);
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
        auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
            return VariantValue (static_cast<UseVariantType> (*reinterpret_cast<const T*> (fromObjOfTypeT)));
        };
        auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
            *reinterpret_cast<T*> (intoObjOfTypeT) = static_cast<T> (d.As<UseVariantType> ());
        };
        return ObjectVariantMapper::TypeMappingDetails (typeid (T), toVariantMapper, fromVariantMapper);
    }
    Set<ObjectVariantMapper::TypeMappingDetails>    mkCommonSerializers_ ()
    {
        Set<ObjectVariantMapper::TypeMappingDetails>    result;
        result.Add (mkSerializerInfo_<bool, bool> ());
        result.Add (mkSerializerInfo_<signed char, signed char> ());
        result.Add (mkSerializerInfo_<short int, short int> ());
        result.Add (mkSerializerInfo_<int, int> ());
        result.Add (mkSerializerInfo_<long int, long int> ());
        result.Add (mkSerializerInfo_<long long int, long long int> ());
        result.Add (mkSerializerInfo_<unsigned char, unsigned char> ());
        result.Add (mkSerializerInfo_<unsigned short, unsigned short> ());
        result.Add (mkSerializerInfo_<unsigned int, unsigned int> ());
        result.Add (mkSerializerInfo_<unsigned long int, unsigned long int> ());
        result.Add (mkSerializerInfo_<unsigned long long int, unsigned long long int> ());
        result.Add (mkSerializerInfo_<float, float> ());
        result.Add (mkSerializerInfo_<double, double> ());
        result.Add (mkSerializerInfo_<long double, long double> ());
        result.Add (mkSerializerInfo_<Date, Date> ());
        result.Add (mkSerializerInfo_<DateTime, DateTime> ());
        result.Add (mkSerializerInfo_<String, String> ());

        {
            auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                return *(reinterpret_cast<const VariantValue*> (fromObjOfTypeT));
            };
            auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                *reinterpret_cast<VariantValue*> (intoObjOfTypeT) = d;
            };
            result.Add (ObjectVariantMapper::TypeMappingDetails (typeid (VariantValue), toVariantMapper, fromVariantMapper));
        }

        {
            auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                return VariantValue ((reinterpret_cast<const Duration*> (fromObjOfTypeT))->As<wstring> ());
            };
            auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                *reinterpret_cast<Duration*> (intoObjOfTypeT) = Duration (d.As<String> ().As<wstring> ());
            };
            result.Add (ObjectVariantMapper::TypeMappingDetails (typeid (Duration), toVariantMapper, fromVariantMapper));
        }

        {
            auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                return VariantValue ((reinterpret_cast<const TimeOfDay*> (fromObjOfTypeT))->GetAsSecondsCount ());
            };
            auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                *reinterpret_cast<TimeOfDay*> (intoObjOfTypeT) = TimeOfDay (d.As<uint32_t> ());
            };
            result.Add (ObjectVariantMapper::TypeMappingDetails (typeid (TimeOfDay), toVariantMapper, fromVariantMapper));
        }

        {
            typedef Mapping<String, String>  ACTUAL_ELEMENT_TYPE;
            auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                Mapping<String, VariantValue> m;
                const ACTUAL_ELEMENT_TYPE*  actualMember    =   reinterpret_cast<const ACTUAL_ELEMENT_TYPE*> (fromObjOfTypeT);
                for (auto i : *actualMember) {
                    // really could do either way - but second more efficient
                    //m.Add (i.first, mapper->Serialize (typeid (String), reinterpret_cast<const Byte*> (&i.second)));
                    m.Add (i.fKey, i.fValue);
                }
                return VariantValue (m);
            };
            auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                Mapping<String, VariantValue> m  =   d.As<Mapping<String, VariantValue>> ();
                ACTUAL_ELEMENT_TYPE*    actualInto  =   reinterpret_cast<ACTUAL_ELEMENT_TYPE*> (intoObjOfTypeT);
                actualInto->clear ();
                for (auto i : m) {
                    // really could do either way - but second more efficient
                    //actualInto->Add (i.first, mapper->ToObject<String> (i.second));
                    actualInto->Add (i.fKey, i.fValue.As<String> ());
                }
            };
            result.Add (ObjectVariantMapper::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
        }

        {
            typedef Mapping<String, VariantValue>    ACTUAL_ELEMENT_TYPE;
            auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
                const ACTUAL_ELEMENT_TYPE*  actualMember    =   reinterpret_cast<const ACTUAL_ELEMENT_TYPE*> (fromObjOfTypeT);
                return VariantValue (*actualMember);
            };
            auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
                ACTUAL_ELEMENT_TYPE*    actualInto  =   reinterpret_cast<ACTUAL_ELEMENT_TYPE*> (intoObjOfTypeT);
                * actualInto = d.As<Mapping<String, VariantValue>> ();
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

void    ObjectVariantMapper::Add (const TypeMappingDetails& s)
{
    fSerializers_.Add (s);
}

void    ObjectVariantMapper::Add (const Set<TypeMappingDetails>& s)
{
    fSerializers_.AddAll (s);
}

void    ObjectVariantMapper::RegisterTypeMapper (const TypeMappingDetails& serializerInfo)
{
    fSerializers_.Add (serializerInfo);
}

void    ObjectVariantMapper::ResetToDefaultTypeRegistry ()
{
    fSerializers_ = GetDefaultTypeMappers_ ();
}

VariantValue    ObjectVariantMapper::FromObject (const type_index& forTypeInfo, const Byte* objOfType) const
{
    Require (Lookup_ (forTypeInfo).fToVariantMapper);
    return Lookup_ (forTypeInfo).fToVariantMapper (this, objOfType);
}

void    ObjectVariantMapper::ToObject (const type_index& forTypeInfo, const VariantValue& d, Byte* into) const
{
    Require (Lookup_ (forTypeInfo).fFromVariantMapper);
    Lookup_ (forTypeInfo).fFromVariantMapper (this, d, into);
}

ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::Lookup_ (const type_index& forTypeInfo) const
{
    Debug::TraceContextBumper   ctx (SDKSTR ("ObjectVariantMapper::Lookup_"));
    DbgTrace ("(forTypeInfo = %s)", forTypeInfo.name ());
    TypeMappingDetails  foo (forTypeInfo, nullptr, nullptr);
    auto i  = fSerializers_.Lookup (foo);
    Require (i.IsPresent ());   // if not present, this is a usage error - only use types which are registered
    return *i;
}
