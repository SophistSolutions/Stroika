/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Containers/MultiSet.h"
#include "../Debug/Trace.h"
#include "../Time/Date.h"
#include "../Time/DateRange.h"
#include "../Time/DateTime.h"
#include "../Time/DateTimeRange.h"
#include "../Time/Duration.h"
#include "../Time/DurationRange.h"

#include "ObjectVariantMapper.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using Memory::Optional;
using Time::Date;
using Time::DateTime;
using Time::Duration;
using Time::TimeOfDay;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************* DataExchange::ObjectVariantMapper ************************
 ********************************************************************************
 */
namespace {
    template <typename T, typename UseVariantType>
    ObjectVariantMapper::TypeMappingDetails mkSerializerInfo_ ()
    {
        auto toVariantMapper = [](const ObjectVariantMapper& mapper, const Byte* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            return VariantValue (static_cast<UseVariantType> (*reinterpret_cast<const T*> (fromObjOfTypeT)));
        };
        auto fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, Byte* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            *reinterpret_cast<T*> (intoObjOfTypeT) = static_cast<T> (d.As<UseVariantType> ());
        };
        return ObjectVariantMapper::TypeMappingDetails (typeid (T), toVariantMapper, fromVariantMapper);
    }
}

ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const void*)
{
    auto toVariantMapper = [](const ObjectVariantMapper& mapper, const Byte* fromObjOfTypeT) -> VariantValue {
        return VariantValue{};
    };
    auto fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, Byte* intoObjOfTypeT) -> void {
    };
    return ObjectVariantMapper::TypeMappingDetails (typeid (void), toVariantMapper, fromVariantMapper);
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<bool> ()
{
    return mkSerializerInfo_<bool, bool> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<signed char> ()
{
    return mkSerializerInfo_<signed char, signed char> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<short int> ()
{
    return mkSerializerInfo_<short int, short int> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<int> ()
{
    return mkSerializerInfo_<int, int> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<long int> ()
{
    return mkSerializerInfo_<long int, long int> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<long long int> ()
{
    return mkSerializerInfo_<long long int, long long int> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned char> ()
{
    return mkSerializerInfo_<unsigned char, unsigned char> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned short int> ()
{
    return mkSerializerInfo_<unsigned short int, unsigned short int> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned int> ()
{
    return mkSerializerInfo_<unsigned int, unsigned int> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned long int> ()
{
    return mkSerializerInfo_<unsigned long int, unsigned long int> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned long long int> ()
{
    return mkSerializerInfo_<unsigned long long int, unsigned long long int> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<float> ()
{
    return mkSerializerInfo_<float, float> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<double> ()
{
    return mkSerializerInfo_<double, double> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<long double> ()
{
    return mkSerializerInfo_<long double, long double> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::Date> ()
{
    return mkSerializerInfo_<Time::Date, Time::Date> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::DateTime> ()
{
    return mkSerializerInfo_<Time::DateTime, Time::DateTime> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Characters::String> ()
{
    return mkSerializerInfo_<Characters::String, Characters::String> ();
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<VariantValue> ()
{
    auto toVariantMapper = [](const ObjectVariantMapper& mapper, const Byte* fromObjOfTypeT) -> VariantValue {
        return *(reinterpret_cast<const VariantValue*> (fromObjOfTypeT));
    };
    auto fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, Byte* intoObjOfTypeT) -> void {
        *reinterpret_cast<VariantValue*> (intoObjOfTypeT) = d;
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (VariantValue), toVariantMapper, fromVariantMapper));
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::Duration> ()
{
    auto toVariantMapper = [](const ObjectVariantMapper& mapper, const Byte* fromObjOfTypeT) -> VariantValue {
        return VariantValue ((reinterpret_cast<const Duration*> (fromObjOfTypeT))->As<String> ());
    };
    auto fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, Byte* intoObjOfTypeT) -> void {
        *reinterpret_cast<Duration*> (intoObjOfTypeT) = Duration (d.As<String> ());
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (Duration), toVariantMapper, fromVariantMapper));
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> ()
{
    auto toVariantMapper = [](const ObjectVariantMapper& mapper, const Byte* fromObjOfTypeT) -> VariantValue {
        return VariantValue ((reinterpret_cast<const TimeOfDay*> (fromObjOfTypeT))->GetAsSecondsCount ());
    };
    auto fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, Byte* intoObjOfTypeT) -> void {
        *reinterpret_cast<TimeOfDay*> (intoObjOfTypeT) = TimeOfDay (d.As<uint32_t> ());
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (TimeOfDay), toVariantMapper, fromVariantMapper));
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, Characters::String>> ()
{
    using ACTUAL_ELEMENT_TYPE = Mapping<String, String>;
    auto toVariantMapper      = [](const ObjectVariantMapper& mapper, const Byte* fromObjOfTypeT) -> VariantValue {
        Mapping<String, VariantValue> m;
        const ACTUAL_ELEMENT_TYPE* actualMember = reinterpret_cast<const ACTUAL_ELEMENT_TYPE*> (fromObjOfTypeT);
        for (auto i : *actualMember) {
            // really could do either way - but second more efficient
            //m.Add (i.first, mapper.Serialize (typeid (String), reinterpret_cast<const Byte*> (&i.second)));
            m.Add (i.fKey, i.fValue);
        }
        return VariantValue (m);
    };
    auto fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, Byte* intoObjOfTypeT) -> void {
        Mapping<String, VariantValue> m = d.As<Mapping<String, VariantValue>> ();
        ACTUAL_ELEMENT_TYPE* actualInto = reinterpret_cast<ACTUAL_ELEMENT_TYPE*> (intoObjOfTypeT);
        actualInto->clear ();
        for (auto i : m) {
            // really could do either way - but second more efficient
            //actualInto->Add (i.first, mapper.ToObject<String> (i.second));
            actualInto->Add (i.fKey, i.fValue.As<String> ());
        }
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
}

template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, VariantValue>> ()
{
    using ACTUAL_ELEMENT_TYPE = Mapping<String, VariantValue>;
    auto toVariantMapper      = [](const ObjectVariantMapper& mapper, const Byte* fromObjOfTypeT) -> VariantValue {
        const ACTUAL_ELEMENT_TYPE* actualMember = reinterpret_cast<const ACTUAL_ELEMENT_TYPE*> (fromObjOfTypeT);
        return VariantValue (*actualMember);
    };
    auto fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, Byte* intoObjOfTypeT) -> void {
        ACTUAL_ELEMENT_TYPE* actualInto = reinterpret_cast<ACTUAL_ELEMENT_TYPE*> (intoObjOfTypeT);
        *actualInto                     = d.As<Mapping<String, VariantValue>> ();
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
}

ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const IO::Network::URL*, IO::Network::URL::ParseOptions parseOptions)
{
    auto toVariantMapper = [](const ObjectVariantMapper& mapper, const Byte* fromObjOfTypeT) -> VariantValue {
        return VariantValue ((reinterpret_cast<const IO::Network::URL*> (fromObjOfTypeT))->GetFullURL ());
    };
    auto fromVariantMapper = [parseOptions](const ObjectVariantMapper& mapper, const VariantValue& d, Byte* intoObjOfTypeT) -> void {
        *reinterpret_cast<IO::Network::URL*> (intoObjOfTypeT) = IO::Network::URL (d.As<String> (), parseOptions);
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (IO::Network::URL), toVariantMapper, fromVariantMapper));
}

namespace {
    ObjectVariantMapper::TypesRegistry GetDefaultTypeMappers_ ()
    {
        using TypeMappingDetails              = ObjectVariantMapper::TypeMappingDetails;
        using TypesRegistry                   = ObjectVariantMapper::TypesRegistry;
        static const TypesRegistry sDefaults_ = {Set<ObjectVariantMapper::TypeMappingDetails>{
            ObjectVariantMapper::MakeCommonSerializer<void> (),
            ObjectVariantMapper::MakeCommonSerializer<bool> (),
            ObjectVariantMapper::MakeCommonSerializer<signed char> (),
            ObjectVariantMapper::MakeCommonSerializer<short int> (),
            ObjectVariantMapper::MakeCommonSerializer<int> (),
            ObjectVariantMapper::MakeCommonSerializer<long int> (),
            ObjectVariantMapper::MakeCommonSerializer<long long int> (),
            ObjectVariantMapper::MakeCommonSerializer<unsigned char> (),
            ObjectVariantMapper::MakeCommonSerializer<unsigned short> (),
            ObjectVariantMapper::MakeCommonSerializer<unsigned int> (),
            ObjectVariantMapper::MakeCommonSerializer<unsigned long int> (),
            ObjectVariantMapper::MakeCommonSerializer<unsigned long long int> (),
            ObjectVariantMapper::MakeCommonSerializer<float> (),
            ObjectVariantMapper::MakeCommonSerializer<double> (),
            ObjectVariantMapper::MakeCommonSerializer<long double> (),
            ObjectVariantMapper::MakeCommonSerializer<Date> (),
            ObjectVariantMapper::MakeCommonSerializer<DateTime> (),
            ObjectVariantMapper::MakeCommonSerializer<String> (),
            ObjectVariantMapper::MakeCommonSerializer<VariantValue> (),
            ObjectVariantMapper::MakeCommonSerializer<Time::Duration> (),
            ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> (),
            ObjectVariantMapper::MakeCommonSerializer<Mapping<String, String>> (),
            ObjectVariantMapper::MakeCommonSerializer<Mapping<String, VariantValue>> (),
            ObjectVariantMapper::MakeCommonSerializer<Time::DurationRange> (),
            ObjectVariantMapper::MakeCommonSerializer<Time::DateRange> (),
            ObjectVariantMapper::MakeCommonSerializer<Time::DateTimeRange> (),
        }};
        return sDefaults_;
    }
}

ObjectVariantMapper::ObjectVariantMapper ()
    : fTypeMappingRegistry_ (GetDefaultTypeMappers_ ())
{
}

void ObjectVariantMapper::Add (const TypeMappingDetails& s)
{
    fTypeMappingRegistry_.Add (s);
}

void ObjectVariantMapper::Add (const Set<TypeMappingDetails>& s)
{
    s.Apply ([this](const TypeMappingDetails& arg) { Add (arg); });
}

void ObjectVariantMapper::Add (const TypesRegistry& s)
{
    s.GetMappers ().Apply ([this](const TypeMappingDetails& arg) { Add (arg); });
}

void ObjectVariantMapper::Add (const ObjectVariantMapper& s)
{
    Add (s.fTypeMappingRegistry_);
}

void ObjectVariantMapper::ResetToDefaultTypeRegistry ()
{
    fTypeMappingRegistry_ = GetDefaultTypeMappers_ ();
}

ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::Lookup_ (const type_index& forTypeInfo) const
{
    auto i = fTypeMappingRegistry_.Lookup (forTypeInfo);
#if qDebug
    if (not i.IsPresent ()) {
        Debug::TraceContextBumper ctx ("ObjectVariantMapper::Lookup_");
        DbgTrace (L"(forTypeInfo = %s) - UnRegistered Type!", Characters::ToString (forTypeInfo).c_str ());
    }
#endif
    Require (i.IsPresent ()); // if not present, this is a usage error - only use types which are registered
    return *i;
}
