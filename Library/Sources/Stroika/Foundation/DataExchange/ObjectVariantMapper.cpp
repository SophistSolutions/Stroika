/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Characters/StringBuilder.h"
#include "../Characters/ToString.h"
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
    using FromObjectMapperType = ObjectVariantMapper::FromObjectMapperType<T>;
    template <typename T>
    using ToObjectMapperType = ObjectVariantMapper::ToObjectMapperType<T>;

    using TypeMappingDetails = ObjectVariantMapper::TypeMappingDetails;
}

/*
 ********************************************************************************
 **************** DataExchange::ObjectVariantMapper::TypeMappingDetails *********
 ********************************************************************************
 */
String ObjectVariantMapper::TypeMappingDetails::ToString () const
{
    Characters::StringBuilder sb;
    sb += L"{";
    sb += L"for-type: " + Characters::ToString (fForType);
    // @todo maybe also print function pointers? Not sure a good reason
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ****************** DataExchange::ObjectVariantMapper::TypesRegistry ************
 ********************************************************************************
 */
String ObjectVariantMapper::TypesRegistry::ToString () const
{
    Characters::StringBuilder sb;
    sb += L"{";
    sb += L"type-mappers: " + Characters::ToString (fSerializers.Keys ()); // for now the values are not interesting
    sb += L"}";
    return sb.str ();
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
        FromObjectMapperType<T> fromObjectMapper = [](const ObjectVariantMapper& mapper, const T* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            return VariantValue (static_cast<UseVariantType> (*fromObjOfTypeT));
        };
        ToObjectMapperType<T> toObjectMapper = [](const ObjectVariantMapper&, const VariantValue& d, T* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            *intoObjOfTypeT = static_cast<T> (d.As<UseVariantType> ());
        };
        return TypeMappingDetails{typeid (T), fromObjectMapper, toObjectMapper};
    }
}

TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const void*)
{
    FromObjectMapperType<void> fromObjectMapper = [](const ObjectVariantMapper&, const void*) -> VariantValue {
        return VariantValue{};
    };
    ToObjectMapperType<void> toObjectMapper = [](const ObjectVariantMapper&, const VariantValue&, void*) -> void {
    };
    return TypeMappingDetails{typeid (void), fromObjectMapper, toObjectMapper};
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
    FromObjectMapperType<VariantValue> fromObjectMapper = [](const ObjectVariantMapper&, const VariantValue* fromObjOfTypeT) -> VariantValue {
        return *fromObjOfTypeT;
    };
    ToObjectMapperType<VariantValue> toObjectMapper = [](const ObjectVariantMapper&, const VariantValue& d, VariantValue* intoObjOfTypeT) -> void {
        *reinterpret_cast<VariantValue*> (intoObjOfTypeT) = d;
    };
    return TypeMappingDetails{typeid (VariantValue), fromObjectMapper, toObjectMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::Duration> ()
{
    FromObjectMapperType<Time::Duration> fromObjectMapper = [](const ObjectVariantMapper& mapper, const Time::Duration* fromObjOfTypeT) -> VariantValue {
        return VariantValue ((fromObjOfTypeT)->As<String> ());
    };
    ToObjectMapperType<Time::Duration> toObjectMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, Time::Duration* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = Duration (d.As<String> ());
    };
    return TypeMappingDetails{typeid (Duration), fromObjectMapper, toObjectMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> ()
{
    FromObjectMapperType<Time::TimeOfDay> fromObjectMapper = []([[maybe_unused]] const ObjectVariantMapper& mapper, const Time::TimeOfDay* fromObjOfTypeT) -> VariantValue {
        return VariantValue (fromObjOfTypeT->GetAsSecondsCount ());
    };
    ToObjectMapperType<Time::TimeOfDay> toObjectMapper = []([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, Time::TimeOfDay* intoObjOfTypeT) -> void {
        *reinterpret_cast<TimeOfDay*> (intoObjOfTypeT) = TimeOfDay (d.As<uint32_t> ());
    };
    return TypeMappingDetails{typeid (TimeOfDay), fromObjectMapper, toObjectMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, Characters::String>> ()
{
    using ACTUAL_ELEMENT_TYPE                                  = Mapping<String, String>;
    FromObjectMapperType<ACTUAL_ELEMENT_TYPE> fromObjectMapper = [](const ObjectVariantMapper& mapper, const ACTUAL_ELEMENT_TYPE* fromObjOfTypeT) -> VariantValue {
        Mapping<String, VariantValue> m;
        for (auto i : *fromObjOfTypeT) {
            // really could do either way - but second more efficient
            //m.Add (i.first, mapper.Serialize (typeid (String), reinterpret_cast<const Byte*> (&i.second)));
            m.Add (i.fKey, i.fValue);
        }
        return VariantValue (m);
    };
    ToObjectMapperType<ACTUAL_ELEMENT_TYPE> toObjectMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, ACTUAL_ELEMENT_TYPE* intoObjOfTypeT) -> void {
        Mapping<String, VariantValue> m = d.As<Mapping<String, VariantValue>> ();
        intoObjOfTypeT->clear ();
        for (auto i : m) {
            // really could do either way - but second more efficient
            //actualInto->Add (i.first, mapper.ToObject<String> (i.second));
            intoObjOfTypeT->Add (i.fKey, i.fValue.As<String> ());
        }
    };
    return TypeMappingDetails{typeid (ACTUAL_ELEMENT_TYPE), fromObjectMapper, toObjectMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, VariantValue>> ()
{
    using ACTUAL_ELEMENT_TYPE                                  = Mapping<String, VariantValue>;
    FromObjectMapperType<ACTUAL_ELEMENT_TYPE> fromObjectMapper = [](const ObjectVariantMapper& mapper, const ACTUAL_ELEMENT_TYPE* fromObjOfTypeT) -> VariantValue {
        return VariantValue (*fromObjOfTypeT);
    };
    ToObjectMapperType<ACTUAL_ELEMENT_TYPE> toObjectMapper = [](const ObjectVariantMapper& mapper, const VariantValue& d, ACTUAL_ELEMENT_TYPE* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = d.As<Mapping<String, VariantValue>> ();
    };
    return TypeMappingDetails{typeid (ACTUAL_ELEMENT_TYPE), fromObjectMapper, toObjectMapper};
}

TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const IO::Network::URL*, IO::Network::URL::ParseOptions parseOptions)
{
    using T                                  = IO::Network::URL;
    FromObjectMapperType<T> fromObjectMapper = [](const ObjectVariantMapper& mapper, const T* fromObjOfTypeT) -> VariantValue {
        return VariantValue (fromObjOfTypeT->GetFullURL ());
    };
    ToObjectMapperType<T> toObjectMapper = [parseOptions](const ObjectVariantMapper& mapper, const VariantValue& d, T* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = IO::Network::URL (d.As<String> (), parseOptions);
    };
    return TypeMappingDetails{typeid (IO::Network::URL), fromObjectMapper, toObjectMapper};
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
    if (not i.has_value ()) {
        Debug::TraceContextBumper ctx ("ObjectVariantMapper::Lookup_");
        DbgTrace (L"(forTypeInfo = %s) - UnRegistered Type!", Characters::ToString (forTypeInfo).c_str ());
    }
#endif
    Require (i.has_value ()); // if not present, this is a usage error - only use types which are registered
    return *i;
}

String ObjectVariantMapper::ToString () const
{
    Characters::StringBuilder sb;
    sb += L"{";
    sb += L"type-map-registry: " + Characters::ToString (fTypeMappingRegistry_);
    sb += L"}";
    return sb.str ();
}