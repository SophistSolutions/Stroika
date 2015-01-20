/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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

using   Time::Date;
using   Time::DateTime;
using   Time::Duration;
using   Time::TimeOfDay;





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
        *actualInto = d.As<Mapping<String, VariantValue>> ();
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
    ObjectVariantMapper::TypesRegistry    GetDefaultTypeMappers_ ()
    {
        static  ObjectVariantMapper::TypesRegistry    sDefaults_ = { mkCommonSerializers_ () };
        return sDefaults_;
    }
}


ObjectVariantMapper::ObjectVariantMapper ()
    : fTypeMappingRegistry_ (GetDefaultTypeMappers_ ())
{
}

void    ObjectVariantMapper::Add (const TypeMappingDetails& s)
{
    fTypeMappingRegistry_.fSerializers.Add (s);
}

void    ObjectVariantMapper::Add (const Set<TypeMappingDetails>& s)
{
    fTypeMappingRegistry_.fSerializers.AddAll (s);
}

void    ObjectVariantMapper::Add (const TypesRegistry& s)
{
    fTypeMappingRegistry_.fSerializers.AddAll (s.fSerializers);
}

void    ObjectVariantMapper::Add (const ObjectVariantMapper& s)
{
    fTypeMappingRegistry_.fSerializers.AddAll (s.fTypeMappingRegistry_.fSerializers);
}

void    ObjectVariantMapper::ResetToDefaultTypeRegistry ()
{
    fTypeMappingRegistry_ = GetDefaultTypeMappers_ ();
}

ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields) const
{
    return MakeCommonSerializer_ForClassObject_ (forTypeInfo, n, fields, [] (VariantValue*) {});
}

ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields, function<void(VariantValue*)> preflightBeforeToObject) const
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

    auto toVariantMapper = [fields] (const ObjectVariantMapper & mapper, const Byte * fromObjOfTypeT) -> VariantValue {
        //Debug::TraceContextBumper ctx (L"ObjectVariantMapper::TypeMappingDetails::{}::fToVariantMapper");
        Mapping<String, VariantValue> m;
        for (auto i : fields)
        {
            //DbgTrace (L"(fieldname = %s, offset=%d", i.fSerializedFieldName.c_str (), i.fOffset);
            const Byte* fieldObj = fromObjOfTypeT + i.fOffset;
            VariantValue    vv = mapper.FromObjectMapper (i.fTypeInfo) (mapper, fromObjOfTypeT + i.fOffset);
            if (i.fNullFields == ObjectVariantMapper::StructureFieldInfo::NullFieldHandling::eInclude or vv.GetType () != VariantValue::Type::eNull) {
                m.Add (i.fSerializedFieldName, vv);
            }
        }
        return VariantValue (m);
    };
    auto fromVariantMapper = [fields, preflightBeforeToObject] (const ObjectVariantMapper & mapper, const VariantValue & d, Byte * intoObjOfTypeT) -> void {
        //Debug::TraceContextBumper ctx (L"ObjectVariantMapper::TypeMappingDetails::{}::fFromVariantMapper");
        VariantValue v2Decode = d;
        preflightBeforeToObject (&v2Decode);
        Mapping<String, VariantValue> m = v2Decode.As<Mapping<String, VariantValue>> ();
        for (auto i : fields)
        {
            //DbgTrace (L"(fieldname = %s, offset=%d", i.fSerializedFieldName.c_str (), i.fOffset);
            Memory::Optional<VariantValue> o = m.Lookup (i.fSerializedFieldName);
            if (not o.IsMissing ()) {
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

ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::Lookup_ (const type_index& forTypeInfo) const
{
    TypeMappingDetails  foo (forTypeInfo, nullptr, nullptr);
    auto i  = fTypeMappingRegistry_.fSerializers.Lookup (foo);
#if     qDebug
    if (not i.IsPresent ()) {
        Debug::TraceContextBumper   ctx (SDKSTR ("ObjectVariantMapper::Lookup_"));
        DbgTrace ("(forTypeInfo = %s) - UnRegistered Type!", forTypeInfo.name ());
    }
#endif
    Require (i.IsPresent ());   // if not present, this is a usage error - only use types which are registered
    return *i;
}
