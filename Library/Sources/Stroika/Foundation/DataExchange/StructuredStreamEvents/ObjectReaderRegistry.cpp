/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/Format.h"
#include    "../../Containers/MultiSet.h"
#include    "../../Debug/Trace.h"
#include    "../../Time/Date.h"
#include    "../../Time/DateRange.h"
#include    "../../Time/DateTime.h"
#include    "../../Time/DateTimeRange.h"
#include    "../../Time/Duration.h"
#include    "../../Time/DurationRange.h"

#include    "ObjectReaderRegistry.h"


using   namespace   Stroika::Foundation;
//using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::StructuredStreamEvents;

using   DataExchange::VariantValue;
using   Time::Date;
using   Time::DateTime;
using   Time::Duration;
using   Time::TimeOfDay;







// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1





/*
 ********************************************************************************
 ********************* DataExchange::ObjectReaderRegistry ************************
 ********************************************************************************
 */
namespace {
    template    <typename T, typename UseVariantType>
    ObjectReaderRegistry::TypeMappingDetails mkSerializerInfo_ ()
    {
        auto toVariantMapper = [] (const ObjectReaderRegistry & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
            RequireNotNull (fromObjOfTypeT);
            return VariantValue (static_cast<UseVariantType> (*reinterpret_cast<const T*> (fromObjOfTypeT)));
        };
        auto fromVariantMapper = [] (const ObjectReaderRegistry & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
            RequireNotNull (intoObjOfTypeT);
            * reinterpret_cast<T*> (intoObjOfTypeT) = static_cast<T> (d.As<UseVariantType> ());
        };
        return ObjectReaderRegistry::TypeMappingDetails (typeid (T), toVariantMapper, fromVariantMapper);
    }
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<bool> ()
{
    return mkSerializerInfo_<bool, bool> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<signed char> ()
{
    return mkSerializerInfo_<signed char, signed char> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<short int> ()
{
    return mkSerializerInfo_<short int, short int> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<int> ()
{
    return mkSerializerInfo_<int, int> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<long int> ()
{
    return mkSerializerInfo_<long int, long int> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<long long int> ()
{
    return mkSerializerInfo_<long long int, long long int> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<unsigned char> ()
{
    return mkSerializerInfo_<unsigned char, unsigned char> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<unsigned short int> ()
{
    return mkSerializerInfo_<unsigned short int, unsigned short int> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<unsigned int> ()
{
    return mkSerializerInfo_<unsigned int, unsigned int> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<unsigned long int> ()
{
    return mkSerializerInfo_<unsigned long int, unsigned long int> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<unsigned long long int> ()
{
    return mkSerializerInfo_<unsigned long long int, unsigned long long int> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<float> ()
{
    return mkSerializerInfo_<float, float> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<double> ()
{
    return mkSerializerInfo_<double, double> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<long double> ()
{
    return mkSerializerInfo_<long double, long double> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Time::Date> ()
{
    return mkSerializerInfo_<Time::Date, Time::Date> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Time::DateTime> ()
{
    return mkSerializerInfo_<Time::DateTime, Time::DateTime> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Characters::String> ()
{
    return mkSerializerInfo_<Characters::String, Characters::String> ();
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<VariantValue> ()
{
    auto toVariantMapper = [] (const ObjectReaderRegistry & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        return *(reinterpret_cast<const VariantValue*> (fromObjOfTypeT));
    };
    auto fromVariantMapper = [] (const ObjectReaderRegistry & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        *reinterpret_cast<VariantValue*> (intoObjOfTypeT) = d;
    };
    return (ObjectReaderRegistry::TypeMappingDetails (typeid (VariantValue), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Time::Duration> ()
{
    auto toVariantMapper = [] (const ObjectReaderRegistry & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        return VariantValue ((reinterpret_cast<const Duration*> (fromObjOfTypeT))->As<String> ());
    };
    auto fromVariantMapper = [] (const ObjectReaderRegistry & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        *reinterpret_cast<Duration*> (intoObjOfTypeT) = Duration (d.As<String> ());
    };
    return (ObjectReaderRegistry::TypeMappingDetails (typeid (Duration), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Time::TimeOfDay> ()
{
    auto toVariantMapper = [] (const ObjectReaderRegistry & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        return VariantValue ((reinterpret_cast<const TimeOfDay*> (fromObjOfTypeT))->GetAsSecondsCount ());
    };
    auto fromVariantMapper = [] (const ObjectReaderRegistry & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        *reinterpret_cast<TimeOfDay*> (intoObjOfTypeT) = TimeOfDay (d.As<uint32_t> ());
    };
    return (ObjectReaderRegistry::TypeMappingDetails (typeid (TimeOfDay), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Containers::Mapping<Characters::String, Characters::String>> ()
{
    using   ACTUAL_ELEMENT_TYPE =   Mapping<String, String>;
    auto toVariantMapper = [] (const ObjectReaderRegistry & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
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
    auto fromVariantMapper = [] (const ObjectReaderRegistry & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
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
    return (ObjectReaderRegistry::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
}

template    <>
ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Containers::Mapping<Characters::String, VariantValue>> ()
{
    using   ACTUAL_ELEMENT_TYPE     =   Mapping<String, VariantValue>;
    auto toVariantMapper = [] (const ObjectReaderRegistry & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        const ACTUAL_ELEMENT_TYPE*  actualMember    =   reinterpret_cast<const ACTUAL_ELEMENT_TYPE*> (fromObjOfTypeT);
        return VariantValue (*actualMember);
    };
    auto fromVariantMapper = [] (const ObjectReaderRegistry & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        ACTUAL_ELEMENT_TYPE*    actualInto  =   reinterpret_cast<ACTUAL_ELEMENT_TYPE*> (intoObjOfTypeT);
        * actualInto = d.As<Mapping<String, VariantValue>> ();
    };
    return (ObjectReaderRegistry::TypeMappingDetails (typeid(ACTUAL_ELEMENT_TYPE), toVariantMapper, fromVariantMapper));
}





namespace   {
    Set<ObjectReaderRegistry::TypeMappingDetails>    mkCommonSerializers_ ()
    {
        Set<ObjectReaderRegistry::TypeMappingDetails>    result;

        result.Add (ObjectReaderRegistry::MakeCommonSerializer<bool> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<signed char> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<short int> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<int> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<long int> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<long long int> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<unsigned char> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<unsigned short> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<unsigned int> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<unsigned long int> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<unsigned long long int> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<float> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<double> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<long double> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<Date> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<DateTime> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<String> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<VariantValue> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<Time::Duration> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<Time::TimeOfDay> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<Mapping<String, String>> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<Mapping<String, VariantValue>> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<Time::DurationRange> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<Time::DateRange> ());
        result.Add (ObjectReaderRegistry::MakeCommonSerializer<Time::DateTimeRange> ());

        return result;
    }

    // Construct the default map once, so that it never needs be re-created (though it could easily get cloned when modified)
    ObjectReaderRegistry::TypesRegistry    GetDefaultTypeMappers_ ()
    {
        static  ObjectReaderRegistry::TypesRegistry    sDefaults_ = { mkCommonSerializers_ () };
        return sDefaults_;
    }
}


ObjectReaderRegistry::ObjectReaderRegistry ()
    : fTypeMappingRegistry_ (GetDefaultTypeMappers_ ())
{
}

void    ObjectReaderRegistry::Add (const TypeMappingDetails& s)
{
    fTypeMappingRegistry_.fSerializers.Add (s);
}

void    ObjectReaderRegistry::Add (const Set<TypeMappingDetails>& s)
{
    fTypeMappingRegistry_.fSerializers.AddAll (s);
}

void    ObjectReaderRegistry::Add (const TypesRegistry& s)
{
    fTypeMappingRegistry_.fSerializers.AddAll (s.fSerializers);
}

void    ObjectReaderRegistry::Add (const ObjectReaderRegistry& s)
{
    fTypeMappingRegistry_.fSerializers.AddAll (s.fTypeMappingRegistry_.fSerializers);
}

void    ObjectReaderRegistry::ResetToDefaultTypeRegistry ()
{
    fTypeMappingRegistry_ = GetDefaultTypeMappers_ ();
}

ObjectReaderRegistry::TypeMappingDetails ObjectReaderRegistry::MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields) const
{
    return MakeCommonSerializer_ForClassObject_ (forTypeInfo, n, fields, [] (VariantValue*) {});
}

ObjectReaderRegistry::TypeMappingDetails ObjectReaderRegistry::MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields, function<void(VariantValue*)> preflightBeforeToObject) const
{
#if     qDebug
    for (auto i : fields) {
        Require (i.fOffset < n);
    }
    {
        // assure each field unique
        Containers::MultiSet<size_t> t;
        for (auto i : fields) {
            t.Add (i.fOffset);
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
            (void)Lookup_ (i.fTypeInfo);
        }
    }
#endif

    auto toVariantMapper = [fields] (const ObjectReaderRegistry & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"ObjectReaderRegistry::TypeMappingDetails::{}::fToVariantMapper");
#endif
        Mapping<String, VariantValue> m;
        for (auto i : fields)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"(fieldname = %s, offset=%d", i.fSerializedFieldName.c_str (), i.fOffset);
#endif
            const Byte* fieldObj = fromObjOfTypeT + i.fOffset;
            VariantValue    vv = mapper.FromObjectMapper (i.fTypeInfo) (mapper, fromObjOfTypeT + i.fOffset);
            if (i.fNullFields == ObjectReaderRegistry::StructureFieldInfo::NullFieldHandling::eInclude or vv.GetType () != VariantValue::Type::eNull) {
                m.Add (i.fSerializedFieldName, vv);
            }
        }
        return VariantValue (m);
    };
    auto fromVariantMapper = [fields, preflightBeforeToObject] (const ObjectReaderRegistry & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"ObjectReaderRegistry::TypeMappingDetails::{}::fFromVariantMapper");
#endif
        VariantValue v2Decode = d;
        preflightBeforeToObject (&v2Decode);
        Mapping<String, VariantValue> m = v2Decode.As<Mapping<String, VariantValue>> ();
        for (auto i : fields)
        {
            Memory::Optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"(fieldname = %s, offset=%d, present=%d)", i.fSerializedFieldName.c_str (), i.fOffset, o.IsPresent ());
#endif
            if (o) {
                switch (i.fSpecialArrayHandling) {
                    case StructureFieldInfo::ArrayElementHandling::eExact: {
                            mapper.ToObjectMapper (i.fTypeInfo) (mapper, *o, intoObjOfTypeT + i.fOffset);
                        }
                        break;
                    case StructureFieldInfo::ArrayElementHandling::eTryExtraArray: {
                            exception_ptr savedException;
                            try {
                                mapper.ToObjectMapper (i.fTypeInfo) (mapper, *o, intoObjOfTypeT + i.fOffset);
                            }
                            catch (...) {
                                // Because of ambiguity in xml between arrays and single elements, we optionally allow special mapping to array
                                // but then if that fails, throw the original exception
                                savedException = current_exception ();
                                Sequence<VariantValue> v;
                                v.Append (*o);
                                try {
                                    mapper.ToObjectMapper (i.fTypeInfo) (mapper, VariantValue (v), intoObjOfTypeT + i.fOffset);
                                }
                                catch (...) {
                                    Execution::DoReThrow (savedException);
                                }
                            }
                        }
                        break;
                    default: {
                            AssertNotReached ();
                        }
                }
            }
        }
    };

    return TypeMappingDetails (forTypeInfo, toVariantMapper, fromVariantMapper);
}

ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::Lookup_ (const type_index& forTypeInfo) const
{
    TypeMappingDetails  foo (forTypeInfo, nullptr, nullptr);
    auto i  = fTypeMappingRegistry_.fSerializers.Lookup (foo);
#if     qDebug
    if (not i.IsPresent ()) {
        Debug::TraceContextBumper   ctx ("ObjectReaderRegistry::Lookup_");
        DbgTrace ("(forTypeInfo = %s) - UnRegistered Type!", forTypeInfo.name ());
    }
#endif
    Require (i.IsPresent ());   // if not present, this is a usage error - only use types which are registered
    return *i;
}
