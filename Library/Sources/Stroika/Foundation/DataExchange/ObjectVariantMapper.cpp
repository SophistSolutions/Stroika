/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Containers/MultiSet.h"
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

using   Memory::Optional;
using   Time::Date;
using   Time::DateTime;
using   Time::Duration;
using   Time::TimeOfDay;




// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1





/*
 ********************************************************************************
 ********************* DataExchange::ObjectVariantMapper ************************
 ********************************************************************************
 */
namespace {
    template    <typename T, typename UseVariantType>
    ObjectVariantMapper::TypeMappingDetails mkSerializerInfo_ ()
    {
        auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            return VariantValue (static_cast<UseVariantType> (*reinterpret_cast<const T*> (fromObjOfTypeT)));
        };
        auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
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
    auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        return *(reinterpret_cast<const VariantValue*> (fromObjOfTypeT));
    };
    auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        *reinterpret_cast<VariantValue*> (intoObjOfTypeT) = d;
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (VariantValue), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Time::Duration> ()
{
    auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        return VariantValue ((reinterpret_cast<const Duration*> (fromObjOfTypeT))->As<String> ());
    };
    auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        *reinterpret_cast<Duration*> (intoObjOfTypeT) = Duration (d.As<String> ());
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (Duration), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> ()
{
    auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        return VariantValue ((reinterpret_cast<const TimeOfDay*> (fromObjOfTypeT))->GetAsSecondsCount ());
    };
    auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        *reinterpret_cast<TimeOfDay*> (intoObjOfTypeT) = TimeOfDay (d.As<uint32_t> ());
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid (TimeOfDay), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, Characters::String>> ()
{
    using   ACTUAL_ELEMENT_TYPE =   Mapping<String, String>;
    auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        Mapping<String, VariantValue> m;
        const ACTUAL_ELEMENT_TYPE*  actualMember    =   reinterpret_cast<const ACTUAL_ELEMENT_TYPE*> (fromObjOfTypeT);
        for (auto i : *actualMember)
        {
            // really could do either way - but second more efficient
            //m.Add (i.first, mapper.Serialize (typeid (String), reinterpret_cast<const Byte*> (&i.second)));
            m.Add (i.fKey, i.fValue);
        }
        return VariantValue (m);
    };
    auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        Mapping<String, VariantValue> m  =   d.As<Mapping<String, VariantValue>> ();
        ACTUAL_ELEMENT_TYPE*    actualInto  =   reinterpret_cast<ACTUAL_ELEMENT_TYPE*> (intoObjOfTypeT);
        actualInto->clear ();
        for (auto i : m)
        {
            // really could do either way - but second more efficient
            //actualInto->Add (i.first, mapper.ToObject<String> (i.second));
            actualInto->Add (i.fKey, i.fValue.As<String> ());
        }
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Containers::Mapping<Characters::String, VariantValue>> ()
{
    using   ACTUAL_ELEMENT_TYPE     =   Mapping<String, VariantValue>;
    auto toVariantMapper = [] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        const ACTUAL_ELEMENT_TYPE*  actualMember    =   reinterpret_cast<const ACTUAL_ELEMENT_TYPE*> (fromObjOfTypeT);
        return VariantValue (*actualMember);
    };
    auto fromVariantMapper = [] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        ACTUAL_ELEMENT_TYPE*    actualInto  =   reinterpret_cast<ACTUAL_ELEMENT_TYPE*> (intoObjOfTypeT);
        * actualInto = d.As<Mapping<String, VariantValue>> ();
    };
    return (ObjectVariantMapper::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
}





namespace   {
    ObjectVariantMapper::TypesRegistry    GetDefaultTypeMappers_ ()
    {
        using TypeMappingDetails    = ObjectVariantMapper::TypeMappingDetails;
        using TypesRegistry         = ObjectVariantMapper::TypesRegistry;
        static  const   TypesRegistry    sDefaults_ = { Set<ObjectVariantMapper::TypeMappingDetails> {
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
            }
        };
        return sDefaults_;
    }
}


ObjectVariantMapper::ObjectVariantMapper ()
    : fTypeMappingRegistry_ (GetDefaultTypeMappers_ ())
{
}

void    ObjectVariantMapper::Add (const TypeMappingDetails& s)
{
    fTypeMappingRegistry_.Add (s);
}

void    ObjectVariantMapper::Add (const Set<TypeMappingDetails>& s)
{
    s.Apply ([this](const TypeMappingDetails & arg) { Add (arg); });
}

void    ObjectVariantMapper::Add (const TypesRegistry& s)
{
    s.GetMappers ().Apply ([this](const TypeMappingDetails & arg) { Add (arg); });
}

void    ObjectVariantMapper::Add (const ObjectVariantMapper& s)
{
    Add (s.fTypeMappingRegistry_);
}

void    ObjectVariantMapper::ResetToDefaultTypeRegistry ()
{
    fTypeMappingRegistry_ = GetDefaultTypeMappers_ ();
}

ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Traversal::Iterable<StructFieldInfo>& fields, const function<void(VariantValue*)>& preflightBeforeToObject, const Optional<type_index>& baseClassTypeInfo) const
{
#if     qDebug
    for (auto i : fields) {
        Require (i.fFieldMetaInfo.fOffset < n);
    }
    {
        // assure each field unique
        Containers::MultiSet<size_t> t;
        for (auto i : fields) {
            t.Add (i.fFieldMetaInfo.fOffset);
        }
        for (auto i : t) {
            Require (i.fCount == 1);        //  not necessarily something we want to prohibit?
        }
    }
    {
        // Assure for each field type is registered. This is helpfull 99% of the time the assert is triggered.
        // If you ever need to avoid it (I dont see how because this mapper doesnt work with circular data structures)
        // you can just define a bogus mapper temporarily, and then reset it to the real one before use.
        for (auto i : fields) {
            // dont need to register the type mapper if its specified as a field
            if (i.fOverrideTypeMapper.IsMissing ()) {
                (void)Lookup_ (i.fFieldMetaInfo.fTypeInfo);
            }
        }
        if (baseClassTypeInfo) {
            (void)Lookup_ (*baseClassTypeInfo);
        }
    }
#endif

    auto toVariantMapper = [fields, baseClassTypeInfo] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"ObjectVariantMapper::TypeMappingDetails::{}::fToVariantMapper");
#endif
        Mapping<String, VariantValue> m;
        if (baseClassTypeInfo)
        {
            m = mapper.FromObjectMapper (*baseClassTypeInfo) (mapper, fromObjOfTypeT).As<Mapping<String, VariantValue>> (); // so we can extend
        }
        for (auto i : fields)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"fieldname = %s, offset=%d", i.fSerializedFieldName.c_str (), i.fFieldMetaInfo.fOffset);
#endif
            const Byte* fieldObj = fromObjOfTypeT + i.fFieldMetaInfo.fOffset;
            VariantValue    vv = i.fOverrideTypeMapper ? i.fOverrideTypeMapper->fToVariantMapper (mapper, fromObjOfTypeT + i.fFieldMetaInfo.fOffset) : mapper.FromObjectMapper (i.fFieldMetaInfo.fTypeInfo) (mapper, fromObjOfTypeT + i.fFieldMetaInfo.fOffset);
            if (i.fNullFields == ObjectVariantMapper::StructFieldInfo::NullFieldHandling::eInclude or vv.GetType () != VariantValue::Type::eNull) {
                m.Add (i.fSerializedFieldName, vv);
            }
        }
        return VariantValue (m);
    };
    auto fromVariantMapper = [fields, baseClassTypeInfo, preflightBeforeToObject] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"ObjectVariantMapper::TypeMappingDetails::{}::fFromVariantMapper");
#endif
        RequireNotNull (intoObjOfTypeT);
        VariantValue v2Decode = d;
        if (preflightBeforeToObject != nullptr)
        {
            preflightBeforeToObject (&v2Decode);
        }
        if (baseClassTypeInfo)
        {
            mapper.ToObjectMapper (*baseClassTypeInfo) (mapper, d, intoObjOfTypeT);
        }
        Mapping<String, VariantValue> m = v2Decode.As<Mapping<String, VariantValue>> ();
        for (auto i : fields)
        {
            Memory::Optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"fieldname = %s, offset=%d, present=%d", i.fSerializedFieldName.c_str (), i.fFieldMetaInfo.fOffset, o.IsPresent ());
#endif
            if (o) {
                if (i.fOverrideTypeMapper) {
                    i.fOverrideTypeMapper->fFromVariantMapper (mapper, *o, intoObjOfTypeT + i.fFieldMetaInfo.fOffset);
                }
                else {
                    mapper.ToObjectMapper (i.fFieldMetaInfo.fTypeInfo) (mapper, *o, intoObjOfTypeT + i.fFieldMetaInfo.fOffset);
                }
            }
        }
    };

    return TypeMappingDetails (forTypeInfo, toVariantMapper, fromVariantMapper);
}

ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::Lookup_ (const type_index& forTypeInfo) const
{
    auto i  = fTypeMappingRegistry_.Lookup (forTypeInfo);
#if     qDebug
    if (not i.IsPresent ()) {
        Debug::TraceContextBumper   ctx ("ObjectVariantMapper::Lookup_");
        DbgTrace ("(forTypeInfo = %s) - UnRegistered Type!", forTypeInfo.name ());
    }
#endif
    Require (i.IsPresent ());   // if not present, this is a usage error - only use types which are registered
    return *i;
}
