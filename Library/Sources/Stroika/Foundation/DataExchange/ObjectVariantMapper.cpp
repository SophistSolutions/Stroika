/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Characters/StringBuilder.h"
#include "../Characters/ToString.h"
#include "../Containers/MultiSet.h"
#include "../Cryptography/Encoding/Algorithm/Base64.h"
#include "../Debug/Trace.h"
#include "../IO/FileSystem/PathName.h"
#include "../IO/Network/CIDR.h"
#include "../IO/Network/InternetAddress.h"
#include "../IO/Network/URI.h"
#include "../Time/Date.h"
#include "../Time/DateTime.h"
#include "../Time/Duration.h"

#include "InternetMediaType.h"

#include "ObjectVariantMapper.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

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
    sb << "{"sv;
    sb << "for-type: "sv << Characters::ToString (fForType);
    // @todo maybe also print function pointers? Not sure a good reason
    sb << "}"sv;
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
    sb << "{"sv;
    sb << "type-mappers: "sv << Characters::ToString (fSerializers_.Keys ()); // for now the values are not interesting
    sb << "}"sv;
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
        FromObjectMapperType<T> fromObjectMapper = [] (const ObjectVariantMapper&, const T* fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            return VariantValue{static_cast<UseVariantType> (*fromObjOfTypeT)};
        };
        ToObjectMapperType<T> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, T* intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            *intoObjOfTypeT = static_cast<T> (d.As<UseVariantType> ());
        };
        return TypeMappingDetails{typeid (T), fromObjectMapper, toObjectMapper};
    }
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
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<filesystem::path> ()
{
    FromObjectMapperType<filesystem::path> fromObjectMapper = [] (const ObjectVariantMapper&, const filesystem::path* fromObjOfTypeT) -> VariantValue {
        RequireNotNull (fromObjOfTypeT);
        return VariantValue{IO::FileSystem::FromPath (*fromObjOfTypeT)};
    };
    ToObjectMapperType<filesystem::path> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, filesystem::path* intoObjOfTypeT) -> void {
        RequireNotNull (intoObjOfTypeT);
        *intoObjOfTypeT = IO::FileSystem::ToPath (d.As<String> ());
    };
    return TypeMappingDetails{typeid (filesystem::path), fromObjectMapper, toObjectMapper};
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
    FromObjectMapperType<VariantValue> fromObjectMapper = [] (const ObjectVariantMapper&,
                                                              const VariantValue* fromObjOfTypeT) -> VariantValue { return *fromObjOfTypeT; };
    ToObjectMapperType<VariantValue> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, VariantValue* intoObjOfTypeT) -> void {
        *reinterpret_cast<VariantValue*> (intoObjOfTypeT) = d;
    };
    return TypeMappingDetails{typeid (VariantValue), fromObjectMapper, toObjectMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::Duration> ()
{
    FromObjectMapperType<Time::Duration> fromObjectMapper = [] (const ObjectVariantMapper&, const Time::Duration* fromObjOfTypeT) -> VariantValue {
        return VariantValue{(fromObjOfTypeT)->As<String> ()};
    };
    ToObjectMapperType<Time::Duration> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, Time::Duration* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = Duration{d.As<String> ()};
    };
    return TypeMappingDetails{typeid (Duration), fromObjectMapper, toObjectMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> ()
{
    FromObjectMapperType<Time::TimeOfDay> fromObjectMapper = [] ([[maybe_unused]] const ObjectVariantMapper&,
                                                                 const Time::TimeOfDay* fromObjOfTypeT) -> VariantValue {
        return VariantValue{fromObjOfTypeT->GetAsSecondsCount ()};
    };
    ToObjectMapperType<Time::TimeOfDay> toObjectMapper = [] ([[maybe_unused]] const ObjectVariantMapper&, const VariantValue& d,
                                                             Time::TimeOfDay* intoObjOfTypeT) -> void {
        *reinterpret_cast<TimeOfDay*> (intoObjOfTypeT) = TimeOfDay{d.As<uint32_t> ()};
    };
    return TypeMappingDetails{typeid (TimeOfDay), fromObjectMapper, toObjectMapper};
}

template <>
TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, Characters::String>> ()
{
    using ACTUAL_ELEMENT_TYPE = Mapping<String, String>;
    FromObjectMapperType<ACTUAL_ELEMENT_TYPE> fromObjectMapper = [] (const ObjectVariantMapper&, const ACTUAL_ELEMENT_TYPE* fromObjOfTypeT) -> VariantValue {
        Mapping<String, VariantValue> m;
        for (const auto& i : *fromObjOfTypeT) {
            // really could do either way - but second more efficient
            //m.Add (i.first, mapper.Serialize (typeid (String), reinterpret_cast<const byte*> (&i.second)));
            m.Add (i.fKey, i.fValue);
        }
        return VariantValue{m};
    };
    ToObjectMapperType<ACTUAL_ELEMENT_TYPE> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d,
                                                                 ACTUAL_ELEMENT_TYPE* intoObjOfTypeT) -> void {
        Mapping<String, VariantValue> m = d.As<Mapping<String, VariantValue>> ();
        intoObjOfTypeT->clear ();
        for (const auto& i : m) {
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
    using ACTUAL_ELEMENT_TYPE = Mapping<String, VariantValue>;
    FromObjectMapperType<ACTUAL_ELEMENT_TYPE> fromObjectMapper =
        [] (const ObjectVariantMapper&, const ACTUAL_ELEMENT_TYPE* fromObjOfTypeT) -> VariantValue { return VariantValue{*fromObjOfTypeT}; };
    ToObjectMapperType<ACTUAL_ELEMENT_TYPE> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d,
                                                                 ACTUAL_ELEMENT_TYPE* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = d.As<Mapping<String, VariantValue>> ();
    };
    return TypeMappingDetails{typeid (ACTUAL_ELEMENT_TYPE), fromObjectMapper, toObjectMapper};
}

TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Memory::BLOB*)
{
    // No super obvious way to convert BLOB to/from JSON, but base64 encoding appears the best default
    // Note - callers can easily replace this converter
    using T                                  = Memory::BLOB;
    FromObjectMapperType<T> fromObjectMapper = [] (const ObjectVariantMapper&, const T* fromObjOfTypeT) -> VariantValue {
        return VariantValue{String{Cryptography::Encoding::Algorithm::EncodeBase64 (*fromObjOfTypeT)}};
    };
    ToObjectMapperType<T> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, T* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = Cryptography::Encoding::Algorithm::DecodeBase64 (d.As<String> ());
    };
    return TypeMappingDetails{typeid (T), fromObjectMapper, toObjectMapper};
}

TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const InternetMediaType*)
{
    using T                                  = InternetMediaType;
    FromObjectMapperType<T> fromObjectMapper = [] (const ObjectVariantMapper&, const T* fromObjOfTypeT) -> VariantValue {
        return VariantValue{fromObjOfTypeT->As<String> ()};
    };
    ToObjectMapperType<T> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, T* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = T{d.As<String> ()};
    };
    return TypeMappingDetails{typeid (T), fromObjectMapper, toObjectMapper};
}

TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const IO::Network::CIDR*)
{
    using T                                  = IO::Network::CIDR;
    FromObjectMapperType<T> fromObjectMapper = [] (const ObjectVariantMapper&, const T* fromObjOfTypeT) -> VariantValue {
        return VariantValue{fromObjOfTypeT->As<String> ()};
    };
    ToObjectMapperType<T> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, T* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = T{d.As<String> ()};
    };
    return TypeMappingDetails{typeid (T), fromObjectMapper, toObjectMapper};
}

TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const IO::Network::InternetAddress*)
{
    using T                                  = IO::Network::InternetAddress;
    FromObjectMapperType<T> fromObjectMapper = [] (const ObjectVariantMapper&, const T* fromObjOfTypeT) -> VariantValue {
        return VariantValue{fromObjOfTypeT->As<String> ()};
    };
    ToObjectMapperType<T> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, T* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = T{d.As<String> ()};
    };
    return TypeMappingDetails{typeid (T), fromObjectMapper, toObjectMapper};
}

TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const IO::Network::URI*)
{
    using T                                  = IO::Network::URI;
    FromObjectMapperType<T> fromObjectMapper = [] (const ObjectVariantMapper&, const T* fromObjOfTypeT) -> VariantValue {
        return VariantValue{fromObjOfTypeT->As<String> ()};
    };
    ToObjectMapperType<T> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, T* intoObjOfTypeT) -> void {
        *intoObjOfTypeT = T::Parse (d.As<String> ());
    };
    return TypeMappingDetails{typeid (T), fromObjectMapper, toObjectMapper};
}

TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const Common::GUID*, VariantValue::Type representAs)
{
    using T = Common::GUID;
    Require (representAs == VariantValue::eBLOB or representAs == VariantValue::eString);
    switch (representAs) {
        case VariantValue::eBLOB: {
            FromObjectMapperType<T> fromObjectMapper = [] (const ObjectVariantMapper&, const T* fromObjOfTypeT) -> VariantValue {
                return fromObjOfTypeT->As<Memory::BLOB> ();
            };
            ToObjectMapperType<T> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, T* intoObjOfTypeT) -> void {
                *intoObjOfTypeT = Common::GUID{d.As<Memory::BLOB> ()};
            };
            return TypeMappingDetails{typeid (T), fromObjectMapper, toObjectMapper};
        }
        case VariantValue::eString: {
            FromObjectMapperType<T> fromObjectMapper = [] (const ObjectVariantMapper&, const T* fromObjOfTypeT) -> VariantValue {
                return fromObjOfTypeT->As<String> ();
            };
            ToObjectMapperType<T> toObjectMapper = [] (const ObjectVariantMapper&, const VariantValue& d, T* intoObjOfTypeT) -> void {
                *intoObjOfTypeT = Common::GUID{d.As<String> ()};
            };
            return TypeMappingDetails{typeid (T), fromObjectMapper, toObjectMapper};
        }
        default:
            RequireNotReached ();
            return TypeMappingDetails{typeid (T), nullptr, nullptr};
    }
}

namespace {
    ObjectVariantMapper::TypesRegistry GetDefaultTypeMappers_ ()
    {
        using TypesRegistry                   = ObjectVariantMapper::TypesRegistry;
        static const TypesRegistry sDefaults_ = {Set<TypeMappingDetails>{
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
            ObjectVariantMapper::MakeCommonSerializer<filesystem::path> (),
            ObjectVariantMapper::MakeCommonSerializer<Date> (),
            ObjectVariantMapper::MakeCommonSerializer<DateTime> (),
            ObjectVariantMapper::MakeCommonSerializer<Duration> (),
            ObjectVariantMapper::MakeCommonSerializer<Common::GUID> (),
            ObjectVariantMapper::MakeCommonSerializer<InternetMediaType> (),
            ObjectVariantMapper::MakeCommonSerializer<IO::Network::InternetAddress> (),
            ObjectVariantMapper::MakeCommonSerializer<IO::Network::URI> (),
            ObjectVariantMapper::MakeCommonSerializer<Mapping<String, String>> (),
            ObjectVariantMapper::MakeCommonSerializer<Mapping<String, VariantValue>> (),
            ObjectVariantMapper::MakeCommonSerializer<String> (),
            ObjectVariantMapper::MakeCommonSerializer<TimeOfDay> (),
            ObjectVariantMapper::MakeCommonSerializer<Traversal::Range<Duration>> (),
            ObjectVariantMapper::MakeCommonSerializer<Traversal::Range<Date>> (),
            ObjectVariantMapper::MakeCommonSerializer<Traversal::DiscreteRange<Date>> (),
            ObjectVariantMapper::MakeCommonSerializer<Traversal::Range<DateTime>> (),
            ObjectVariantMapper::MakeCommonSerializer<VariantValue> (),

            ObjectVariantMapper::MakeCommonSerializer<optional<bool>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<signed char>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<short int>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<int>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<long int>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<long long int>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<unsigned char>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<unsigned short>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<unsigned int>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<unsigned long int>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<unsigned long long int>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<float>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<double>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<long double>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<filesystem::path>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<Date>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<DateTime>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<Duration>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<Common::GUID>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<InternetMediaType>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<IO::Network::InternetAddress>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<IO::Network::URI>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<Mapping<String, String>>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<Mapping<String, VariantValue>>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<String>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<TimeOfDay>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<Traversal::Range<Duration>>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<Traversal::Range<Date>>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<Traversal::DiscreteRange<Date>>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<Traversal::Range<DateTime>>> (),
            ObjectVariantMapper::MakeCommonSerializer<optional<VariantValue>> (),
        }};
        return sDefaults_;
    }
}

ObjectVariantMapper::ObjectVariantMapper ()
    : fTypeMappingRegistry_{GetDefaultTypeMappers_ ()}
{
}

void ObjectVariantMapper::Add (const TypeMappingDetails& s)
{
    fTypeMappingRegistry_.Add (s);
}

void ObjectVariantMapper::Add (const Traversal::Iterable<TypeMappingDetails>& s)
{
    s.Apply ([this] (const TypeMappingDetails& arg) { Add (arg); });
}

void ObjectVariantMapper::Add (const TypesRegistry& s)
{
    s.GetMappers ().Apply ([this] (const TypeMappingDetails& arg) { Add (arg); });
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
    if constexpr (qDebug) {
        if (not i.has_value ()) {
            Debug::TraceContextBumper ctx{"ObjectVariantMapper::Lookup_"};
            DbgTrace (L"(forTypeInfo = %s) - UnRegistered Type!", Characters::ToString (forTypeInfo).c_str ());
        }
    }
    Require (i.has_value ()); // if not present, this is a usage error - only use types which are registered
    return *i;
}

template <>
Time::Date ObjectVariantMapper::ToObject (const ToObjectMapperType<Time::Date>& toObjectMapper, const VariantValue& v) const
{
    // Generally best to serialize/deserialize with optional<Date> - else you cannot tell if you actually found one (defaults to min Date)
    Time::Date tmp{Time::Date::kMin};
    ToObject (toObjectMapper, v, &tmp);
    return tmp;
}
template <>
Time::DateTime ObjectVariantMapper::ToObject (const ToObjectMapperType<Time::DateTime>& toObjectMapper, const VariantValue& v) const
{
    // Generally best to serialize/deserialize with optional<DateTime> - else you cannot tell if you actually found one (defaults to DateTime::min)
    Time::DateTime tmp{Time::DateTime::kMin};
    ToObject (toObjectMapper, v, &tmp);
    return tmp;
}
template <>
Time::TimeOfDay ObjectVariantMapper::ToObject (const ToObjectMapperType<Time::TimeOfDay>& toObjectMapper, const VariantValue& v) const
{
    // Generally best to serialize/deserialize with optional<TimeOfDay> - else you cannot tell if you actually found one (defaults to min TimeOfDay)
    Time::TimeOfDay tmp{Time::TimeOfDay::kMin};
    ToObject (toObjectMapper, v, &tmp);
    return tmp;
}

String ObjectVariantMapper::ToString () const
{
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "type-map-registry: "sv << Characters::ToString (fTypeMappingRegistry_);
    sb << "}";
    return sb.str ();
}

//  https://stroika.atlassian.net/browse/STK-910
template <>
ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ (const optional<IO::Network::CIDR>*)
{
    using T = IO::Network::CIDR;
    FromObjectMapperType<optional<T>> fromObjectMapper = [] (const ObjectVariantMapper& mapper, const optional<T>* fromObjOfTypeT) -> VariantValue {
        RequireNotNull (fromObjOfTypeT);
        if (fromObjOfTypeT->has_value ()) {
            return mapper.FromObject<T> (**fromObjOfTypeT);
        }
        else {
            return VariantValue{};
        }
    };
    ToObjectMapperType<optional<T>> toObjectMapper = [] (const ObjectVariantMapper& mapper, const VariantValue& d, optional<T>* intoObjOfTypeT) -> void {
        RequireNotNull (intoObjOfTypeT);
        if (d.GetType () == VariantValue::eNull) {
            *intoObjOfTypeT = nullopt;
        }
        else {
            // SEE https://stroika.atlassian.net/browse/STK-910
            // fix here - I KNOW I have something there, but how to construct
            T tmp{IO::Network::V4::kLocalhost};
            mapper.ToObject<T> (d, &tmp);
            *intoObjOfTypeT = tmp;
        }
    };
    return TypeMappingDetails{typeid (optional<T>), fromObjectMapper, toObjectMapper};
}
