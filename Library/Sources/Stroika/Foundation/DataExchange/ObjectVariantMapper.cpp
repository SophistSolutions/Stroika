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

namespace {
    template <typename T>
    using ToVariantMapperType = ObjectVariantMapper::ToVariantMapperType<T>;
    template <typename T>
    using FromVariantMapperType = ObjectVariantMapper::FromVariantMapperType<T>;

    using TypeMappingDetails = ObjectVariantMapper::TypeMappingDetails;
}

/*
 ********************************************************************************
 ********************* DataExchange::ObjectVariantMapper ************************
 ********************************************************************************
 */
namespace {
    template <typename T, typename UseVariantType>
    TypeMappingDetails mkSerializerInfo_ ()
    {
        ToVariantMapperType<T> toVariantMapper = [](const ObjectVariantMapper& mapper, const T* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            return VariantValue (static_cast<UseVariantType> (*fromObjOfTypeT));
        };
        FromVariantMapperType<T> fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, T* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            *intoObjOfTypeT = static_cast<T> (d.As<UseVariantType> ());
        };
        return TypeMappingDetails{typeid (T), toVariantMapper, fromVariantMapper};
    }
}

TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const void*)
{
    ToVariantMapperType<void> toVariantMapper = [](const ObjectVariantMapper& mapper, const void* fromObjOfTypeT) -> VariantValue {
        return VariantValue{};
    };
    FromVariantMapperType<void> fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, void* intoObjOfTypeT) -> void {
    };
    return TypeMappingDetails{typeid (void), toVariantMapper, fromVariantMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<bool> ()
{
    return mkSerializerInfo_<bool, bool> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<signed char> ()
{
    return mkSerializerInfo_<signed char, signed char> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<short int> ()
{
    return mkSerializerInfo_<short int, short int> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<int> ()
{
    return mkSerializerInfo_<int, int> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<long int> ()
{
    return mkSerializerInfo_<long int, long int> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<long long int> ()
{
    return mkSerializerInfo_<long long int, long long int> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned char> ()
{
    return mkSerializerInfo_<unsigned char, unsigned char> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned short int> ()
{
    return mkSerializerInfo_<unsigned short int, unsigned short int> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned int> ()
{
    return mkSerializerInfo_<unsigned int, unsigned int> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned long int> ()
{
    return mkSerializerInfo_<unsigned long int, unsigned long int> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned long long int> ()
{
    return mkSerializerInfo_<unsigned long long int, unsigned long long int> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<float> ()
{
    return mkSerializerInfo_<float, float> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<double> ()
{
    return mkSerializerInfo_<double, double> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<long double> ()
{
    return mkSerializerInfo_<long double, long double> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::Date> ()
{
    return mkSerializerInfo_<Time::Date, Time::Date> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::DateTime> ()
{
    return mkSerializerInfo_<Time::DateTime, Time::DateTime> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Characters::String> ()
{
    return mkSerializerInfo_<Characters::String, Characters::String> ();
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<VariantValue> ()
{
    ToVariantMapperType<VariantValue> toVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue* fromObjOfTypeT) -> VariantValue {
        return *fromObjOfTypeT;
    };
    FromVariantMapperType<VariantValue> fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, VariantValue* intoObjOfTypeT) -> void {
        *reinterpret_cast<VariantValue*> (intoObjOfTypeT) = d;
    };
    return TypeMappingDetails{typeid (VariantValue), toVariantMapper, fromVariantMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::Duration> ()
{
    ToVariantMapperType<Time::Duration> toVariantMapper = [](const ObjectVariantMapper& mapper, const Time::Duration* fromObjOfTypeT) -> VariantValue {
        return VariantValue ((fromObjOfTypeT)->As<String> ());
    };
    FromVariantMapperType<Time::Duration> fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, Time::Duration* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = Duration (d.As<String> ());
    };
    return TypeMappingDetails{typeid (Duration), toVariantMapper, fromVariantMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> ()
{
    ToVariantMapperType<Time::TimeOfDay> toVariantMapper = [](const ObjectVariantMapper& mapper, const Time::TimeOfDay* fromObjOfTypeT) -> VariantValue {
        return VariantValue (fromObjOfTypeT->GetAsSecondsCount ());
    };
    FromVariantMapperType<Time::TimeOfDay> fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, Time::TimeOfDay* intoObjOfTypeT) -> void {
        *reinterpret_cast<TimeOfDay*> (intoObjOfTypeT) = TimeOfDay (d.As<uint32_t> ());
    };
    return TypeMappingDetails{typeid (TimeOfDay), toVariantMapper, fromVariantMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, Characters::String>> ()
{
    using ACTUAL_ELEMENT_TYPE                                = Mapping<String, String>;
    ToVariantMapperType<ACTUAL_ELEMENT_TYPE> toVariantMapper = [](const ObjectVariantMapper& mapper, const ACTUAL_ELEMENT_TYPE* fromObjOfTypeT) -> VariantValue {
        Mapping<String, VariantValue> m;
        for (auto i : *fromObjOfTypeT) {
            // really could do either way - but second more efficient
            //m.Add (i.first, mapper.Serialize (typeid (String), reinterpret_cast<const Byte*> (&i.second)));
            m.Add (i.fKey, i.fValue);
        }
        return VariantValue (m);
    };
    FromVariantMapperType<ACTUAL_ELEMENT_TYPE> fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, ACTUAL_ELEMENT_TYPE* intoObjOfTypeT) -> void {
        Mapping<String, VariantValue> m = d.As<Mapping<String, VariantValue>> ();
        intoObjOfTypeT->clear ();
        for (auto i : m) {
            // really could do either way - but second more efficient
            //actualInto->Add (i.first, mapper.ToObject<String> (i.second));
            intoObjOfTypeT->Add (i.fKey, i.fValue.As<String> ());
        }
    };
    return TypeMappingDetails{typeid (ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, VariantValue>> ()
{
    using ACTUAL_ELEMENT_TYPE                                = Mapping<String, VariantValue>;
    ToVariantMapperType<ACTUAL_ELEMENT_TYPE> toVariantMapper = [](const ObjectVariantMapper& mapper, const ACTUAL_ELEMENT_TYPE* fromObjOfTypeT) -> VariantValue {
        return VariantValue (*fromObjOfTypeT);
    };
    FromVariantMapperType<ACTUAL_ELEMENT_TYPE> fromVariantMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, ACTUAL_ELEMENT_TYPE* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = d.As<Mapping<String, VariantValue>> ();
    };
    return TypeMappingDetails{typeid (ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper};
}

TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const IO::Network::URL*, IO::Network::URL::ParseOptions parseOptions)
{
    using T                                = IO::Network::URL;
    ToVariantMapperType<T> toVariantMapper = [](const ObjectVariantMapper& mapper, const T* fromObjOfTypeT) -> VariantValue {
        return VariantValue (fromObjOfTypeT->GetFullURL ());
    };
    FromVariantMapperType<T> fromVariantMapper = [parseOptions](const ObjectVariantMapper& mapper, const VariantValue& d, T* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = IO::Network::URL (d.As<String> (), parseOptions);
    };
    return TypeMappingDetails{typeid (IO::Network::URL), toVariantMapper, fromVariantMapper};
}

namespace {
    ObjectVariantMapper::TypesRegistry GetDefaultTypeMappers_ ()
    {
        using TypesRegistry                   = ObjectVariantMapper::TypesRegistry;
        static const TypesRegistry sDefaults_ = {Set<TypeMappingDetails>{
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
