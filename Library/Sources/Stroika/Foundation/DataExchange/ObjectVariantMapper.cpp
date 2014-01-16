/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Containers/Tally.h"
#include    "../Debug/Trace.h"
#include    "../Time/Date.h"
#include    "../Time/DateRange.h"
#include    "../Time/DateTime.h"
#include    "../Time/DateTimeRange.h"
#include    "../Time/Duration.h"
#include    "../Time/DurationRange.h"

#include    "ObjectVariantMapper.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;

using   Time::Date;
using   Time::DateTime;
using   Time::Duration;
using   Time::TimeOfDay;



/*
 ********************************************************************************
 ******* DataExchange::ObjectVariantMapper::TypeMappingDetails ************
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
            Require (i.fCount == 1);        //  not necessarily something we want to prohibit?
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
        //Debug::TraceContextBumper ctx (L"ObjectVariantMapper::TypeMappingDetails::{}::fToVariantMapper");
        Mapping<String, VariantValue> m;
        for (auto i : fields)
        {
            //DbgTrace (L"(fieldname = %s, offset=%d", i.fSerializedFieldName.c_str (), i.fOffset);
            const Byte* fieldObj = fromObjOfTypeT + i.fOffset;
            m.Add (i.fSerializedFieldName, mapper->FromObject (i.fTypeInfo, fromObjOfTypeT + i.fOffset));
        }
        return VariantValue (m);
    };
    fFromVariantMapper = [fields] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        //Debug::TraceContextBumper ctx (L"ObjectVariantMapper::TypeMappingDetails::{}::fFromVariantMapper");
        Mapping<String, VariantValue> m = d.As<Mapping<String, VariantValue>> ();
        for (auto i : fields)
        {
            //DbgTrace (L"(fieldname = %s, offset=%d", i.fSerializedFieldName.c_str (), i.fOffset);
            Memory::Optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
            if (not o.IsMissing ()) {
                mapper->ToObject (i.fTypeInfo, *o, intoObjOfTypeT + i.fOffset);
            }
        }
    };
}


/*
 ********************************************************************************
 ****************** DataExchange::ObjectVariantMapper *********************
 ********************************************************************************
 */
namespace {
    template    <typename T, typename UseVariantType>
    ObjectVariantMapper::TypeMappingDetails mkSerializerInfo_ ()
    {
        auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            return VariantValue (static_cast<UseVariantType> (*reinterpret_cast<const T*> (fromObjOfTypeT)));
        };
        auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            * reinterpret_cast<T*> (intoObjOfTypeT) = static_cast<T> (d.As<UseVariantType> ());
        };
        return ObjectVariantMapper::TypeMappingDetails (typeid (T), toVariantMapper, fromVariantMapper);
    }
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<bool> ()
{
    return mkSerializerInfo_<bool, bool> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<signed char> ()
{
    return mkSerializerInfo_<signed char, signed char> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<short int> ()
{
    return mkSerializerInfo_<short int, short int> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<int> ()
{
    return mkSerializerInfo_<int, int> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<long int> ()
{
    return mkSerializerInfo_<long int, long int> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<long long int> ()
{
    return mkSerializerInfo_<long long int, long long int> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<unsigned char> ()
{
    return mkSerializerInfo_<unsigned char, unsigned char> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<unsigned short int> ()
{
    return mkSerializerInfo_<unsigned short int, unsigned short int> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<unsigned int> ()
{
    return mkSerializerInfo_<unsigned int, unsigned int> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<unsigned long int> ()
{
    return mkSerializerInfo_<unsigned long int, unsigned long int> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<unsigned long long int> ()
{
    return mkSerializerInfo_<unsigned long long int, unsigned long long int> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<float> ()
{
    return mkSerializerInfo_<float, float> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<double> ()
{
    return mkSerializerInfo_<double, double> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<long double> ()
{
    return mkSerializerInfo_<long double, long double> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Time::Date> ()
{
    return mkSerializerInfo_<Time::Date, Time::Date> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Time::DateTime> ()
{
    return mkSerializerInfo_<Time::DateTime, Time::DateTime> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Characters::String> ()
{
    return mkSerializerInfo_<Characters::String, Characters::String> ();
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<VariantValue> ()
{
    auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        return *(reinterpret_cast<const VariantValue*> (fromObjOfTypeT));
    };
    auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        *reinterpret_cast<VariantValue*> (intoObjOfTypeT) = d;
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (VariantValue), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Time::Duration> ()
{
    auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        return VariantValue ((reinterpret_cast<const Duration*> (fromObjOfTypeT))->As<wstring> ());
    };
    auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        *reinterpret_cast<Duration*> (intoObjOfTypeT) = Duration (d.As<String> ().As<wstring> ());
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (Duration), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> ()
{
    auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        return VariantValue ((reinterpret_cast<const TimeOfDay*> (fromObjOfTypeT))->GetAsSecondsCount ());
    };
    auto fromVariantMapper = [] (const ObjectVariantMapper * mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        *reinterpret_cast<TimeOfDay*> (intoObjOfTypeT) = TimeOfDay (d.As<uint32_t> ());
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (TimeOfDay), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, Characters::String>> ()
{
    typedef Mapping<String, String>  ACTUAL_ELEMENT_TYPE;
    auto toVariantMapper = [] (const ObjectVariantMapper * mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        Mapping<String, VariantValue> m;
        const ACTUAL_ELEMENT_TYPE*  actualMember    =   reinterpret_cast<const ACTUAL_ELEMENT_TYPE*> (fromObjOfTypeT);
        for (auto i : *actualMember)
        {
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
        for (auto i : m)
        {
            // really could do either way - but second more efficient
            //actualInto->Add (i.first, mapper->ToObject<String> (i.second));
            actualInto->Add (i.fKey, i.fValue.As<String> ());
        }
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, VariantValue>> ()
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
    return (ObjectVariantMapper::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
}





namespace   {
    Set<ObjectVariantMapper::TypeMappingDetails>    mkCommonSerializers_ ()
    {
        Set<ObjectVariantMapper::TypeMappingDetails>    result;

        result.Add (ObjectVariantMapper::MakeCommonSerializer<bool> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<signed char> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<short int> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<int> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<long int> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<long long int> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<unsigned char> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<unsigned short> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<unsigned int> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<unsigned long int> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<unsigned long long int> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<float> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<double> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<long double> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<Date> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<DateTime> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<String> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<VariantValue> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<Time::Duration> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<Mapping<String, String>> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<Mapping<String, VariantValue>> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<Time::DurationRange> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<Time::DateRange> ());
        result.Add (ObjectVariantMapper::MakeCommonSerializer<Time::DateTimeRange> ());

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
    TypeMappingDetails  foo (forTypeInfo, nullptr, nullptr);
    auto i  = fSerializers_.Lookup (foo);
#if     qDebug
    if (not i.IsPresent ()) {
        Debug::TraceContextBumper   ctx (SDKSTR ("ObjectVariantMapper::Lookup_"));
        DbgTrace ("(forTypeInfo = %s) - UnRegistered Type!", forTypeInfo.name ());
    }
#endif
    Require (i.IsPresent ());   // if not present, this is a usage error - only use types which are registered
    return *i;
}
