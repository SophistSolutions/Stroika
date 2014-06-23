/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_ObjectVariantMapper_h_
#define _Stroika_Foundation_DataExchange_ObjectVariantMapper_h_    1

#include    "../StroikaPreComp.h"

#include    <type_traits>
#include    <typeindex>

#include    "../Characters/String.h"
#include    "../Containers/Bijection.h"
#include    "../Containers/Collection.h"
#include    "../Containers/Mapping.h"
#include    "../Containers/Sequence.h"
#include    "../Containers/Set.h"
#include    "../Containers/SortedCollection.h"
#include    "../Containers/SortedMapping.h"
#include    "../Containers/SortedSet.h"
#include    "../Memory/Common.h"
#include    "../Memory/Optional.h"
#include    "../Traversal/DiscreteRange.h"
#include    "../Traversal/Range.h"

#include    "VariantValue.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *
 *      @todo   Further cleanups of MakeCommonSerializer<> are needed, but this is probably the right way to go. Use more enable_if
 *              stuff.
 *
 *              OLD RELATED NOTE WHICH IS PARTLY DONE BUT COMPLETE:
 *                  Add support for array to ResetToDefaultTypeRegistry - like we have for struct - using Sequence<>
 *                  as the C++ type (maybe others too, vector, more?).
 *
 *                  But unclear how todo ArrayOfWhat?? Maybe the CTOR takes to typeids - not sure how well that works?
 *
 *      @todo   Redo examples (maybe small bits of API) using new Reader/Writer abstract impl
 *              stuff (backends to XML or JSON)
 *
 *      @todo   Consider moving this out of the Foundation into a framework?
 *
 *      @todo   EFFICIENCY NOTES AND TODO MAYBE IMPROVE?
 *
 *              This can be moderately efficient, but it is not highly efficient. The use of function
 *              for the serializer/desearizers adds costs.
 *
 *              When serializing / deserializing - (e.g to/from JSON or XML) - we construct DOM tree which is
 *              intrinsically not very cost effective. We DO have the XML sax parser (but that wont work with this).
 *
 *      @todo   Current serializer/deserializer API needlessly requires that objects have default CTOR.
 *
 *              template    <typename CLASS>
 *                  inline  CLASS    ObjectVariantMapper::ToObject (const VariantValue& v) const
 *                  {
 *                      CLASS tmp;
 *                      ToObject (v, &tmp);
 *                      return tmp;
 *                  }
 *
 *                  I THINK - if we redefined the to-mapper - to return a "T" instead of taking a Byte* array
 *                  to fill in, we could avoid this issue. However, doing so is NOT possible for the
 *                  automated 'struct' mapper (key). This is its core strategy - to construct an object and
 *                  then fill in (possibly a subset) of its fields. We would need to somehow automate
 *                  calling a particular CTOR instead and that seems tricky given C++'s meager metapramming
 *                  features.
 *
 *                  But I THINK it would be possible to define the TypeMappingDetails object to take EITHER
 *                  soemthing in the currnet form, or something using "T" - and map between them. And then
 *                  users of a particular type (e.g. Range()) - could avoid calling the default ctor, and just
 *                  explicitly call the right CTOR.
 *
 *                  Probably relatively LOW priority to fix, however.
 *
 *      @todo   NOTE and TODO
 *              The cast to Byte* loses some type safety (we may want to store the class size htrough template magic)
 *              in the struct type info record, so it can be validated against the offsets in the typeinfo.
 *
 *      @todo   [Long-Term] [Performance] The gist of this design / API is to map C++ objects from/to VariantValue
 *              objects (logically - these are RAM-based representations of an XML or JSON tree - for example).
 *
 *              We could restructure the API so that it was more SAX-like: instead of taking a VariantValue, it took
 *              something that virtually acted like a VariantValue (gettype, and for array and map types, iterate over the
 *              sub-parts). Similarly - on the FromObject () code - generate a stream of 'write' like calls that
 *              contained atom Variants, and start-array/end-array/start-map/end-map calls.
 *
 *              This might allow this code to work better with a more effient json-reader/writer/xml/reader/writer
 *              that also used these sorts of intermediate objects.
 *
 *              Of course, this ALSO might be implementable without any API changes - just using a special 'rep' for
 *              VariantValue - that was smart about how to it would accessed etc. Needs some thought.
 *
 *              Anyhow - this is a long-term todo item, so no need to work out details now.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            using   Characters::String;
            using   Containers::Mapping;
            using   Containers::Sequence;
            using   Containers::Set;
            using   Memory::Byte;


            /**
             *  \brief  ObjectVariantMapper can be used to map C++ types to variant-union types, which can be transparently mapped into and out of XML, JSON, etc.
             *
             *  ObjectVariantMapper IS COPYABLE. Make one instance, register your types into it and use this to
             *  serialized/ToObject
             *
             *
             * EXAMPLE USAGE:
             *      struct SharedContactsConfig_
             *      {
             *          bool                    fEnabled = false;
             *          DateTime                fLastSynchronizedAt;
             *          Mapping<String,String>  fThisPHRsIDToSharedContactID;
             *      };
             *...
             *
             *  ObjectVariantMapper mapper;
             *
             *  // register each of your mappable (even private) types
             *  mapper.AddClass<SharedContactsConfig_> (Sequence<StructureFieldInfo> ({
             *          ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fEnabled, L"Enabled"),
             *          ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fLastSynchronizedAt, L"Last-Synchronized-At"),
             *          ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fThisPHRsIDToSharedContactID, L"This-HR-ContactID-To-SharedContactID-Map"),
             *  }));
             *
             *  SharedContactsConfig_   tmp;
             *  tmp.fEnabled = enabled;
             *  VariantValue v = mapper.Serialize  (tmp);
             *
             *  // at this point - we should have VariantValue object with "Enabled" field.
             *  // This can then be serialized using
             *
             *  Streams::BasicBinaryInputOutputStream   tmpStream;
             *  DataExchange::JSON::PrettyPrint (v, tmpStream);
             *
             *  // THEN deserialized, and mapped back to C++ object form
             *  tmp = mapper.ToObject<SharedContactsConfig_> (DataExchange::JSON::Reader   (tmpStream));
             *  if (tmp.fEnabled) {
             *  ...
             *  }
             *
             *
             */
            class   ObjectVariantMapper {
            public:
                /**
                 *  Defaults to installing basic type mappers (@see ResetToDefaultTypeRegistry).
                 */
                ObjectVariantMapper ();

            public:
                struct  TypeMappingDetails;

            public:
                /**
                 *  Returns the current set of type mappers.
                 */
                nonvirtual  Set<TypeMappingDetails>    GetTypeMappingRegistry () const;

            public:
                /**
                 *  Sets the current set of type mappers.
                 */
                nonvirtual  void    SetTypeMappingRegistry (const Set<TypeMappingDetails>& s);

            public:
                /**
                 *  Adds the given type mapper(s)
                 */
                nonvirtual  void    Add (const TypeMappingDetails& s);
                nonvirtual  void    Add (const Set<TypeMappingDetails>& s);

            public:
                /**
                 *  Shortcut for Add (MakeCommonSerializer<T> ());
                 *
                 *  So - this is supported for any type for which (@see MakeCommonSerializer) is supported.
                 *
                 *  Note this this is not needed (because it's done by default), but is supported,
                 *  for the builtin types.
                 */
                template    <typename T>
                nonvirtual  void    AddCommonType ();

            public:
                /**
                 *  This clears the registry of type mappers, and resets it to the defaults - a set of builtin types,
                 *  like String, int, etc.
                 *
                 *  Builtin types include:
                 *      o   bool
                 *      o   signed char, short int, int, long int, long long int
                 *      o   unsigned char, unsigned short int, unsigned int, unsigned long int, unsigned long long int
                 *      o   float, double, long double
                 *      o   Date
                 *      o   DateTime
                 *      o   Duration
                 *      o   String
                 *      o   Mapping<String, String>
                 *      o   Mapping<String, VariantValue>
                 *      o   TimeOfDay
                 *      o   VariantValue
                 *      o   DurationRange/DateRange/DateTimeRange
                 *
                 *  @todo - IT SOON WILL CONTAIN Sequence<String>, and Sequence<Variant>, as well as vector<> of those types.
                 *
                 *  Note - to include any of your user-defined types (structs) - you must use Add () or
                 *  AddClass ().
                 *
                 *  @see GetTypeMappingRegistry
                 *  @see SetTypeMappingRegistry
                 *  @see AddClass
                 *  @see Add
                 *  @see MakeCommonSerializer
                 */
                nonvirtual  void    ResetToDefaultTypeRegistry ();

            public:
                struct  StructureFieldInfo;

            public:
                /**
                 *  Adds the given class (defined in explicit template argument) with the given list of field.
                 *  Also, optionally provide a 'readPreflight' function to be applied to the read-in VariantValue object before
                 *  decomposing (into C++ structs), as a helpful backward compatible file format hook.
                 */
                template    <typename CLASS>
                nonvirtual  void    AddClass (const Sequence<StructureFieldInfo>& fieldDescriptions);
#if     !qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug
                template    <typename CLASS>
                nonvirtual  void    AddClass (const std::initializer_list<StructureFieldInfo>& fieldDescriptions);
#endif
                template    <typename CLASS>
                nonvirtual  void    AddClass (const StructureFieldInfo* fieldDescriptionsStart, const StructureFieldInfo* fieldDescriptionsEnd);
                template    <typename CLASS>
                nonvirtual  void    AddClass (const Sequence<StructureFieldInfo>& fieldDescriptions, function<void(VariantValue*)> preflightBeforeToObject);
#if     !qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug
                template    <typename CLASS>
                nonvirtual  void    AddClass (const std::initializer_list<StructureFieldInfo>& fieldDescriptions, function<void(VariantValue*)> preflightBeforeToObject);
#endif
                template    <typename CLASS>
                nonvirtual  void    AddClass (const StructureFieldInfo* fieldDescriptionsStart, const StructureFieldInfo* fieldDescriptionsEnd, function<void(VariantValue*)> preflightBeforeToObject);

            public:
                /**
                 *  Convert a VariantValue object into any C++ object - using the type converters already registered in
                 *  this mapper.
                 */
                nonvirtual  void    ToObject (const type_index& forTypeInfo, const VariantValue& d, Byte* into) const;
                template    <typename CLASS>
                nonvirtual  void    ToObject (const VariantValue& v, CLASS* into) const;
                template    <typename CLASS>
                nonvirtual  CLASS   ToObject (const VariantValue& v) const;

            public:
                /**
                 *  Convert a C++ object to a VariantValue object - using the type converters already registered in
                 *  this mapper.
                 */
                nonvirtual  VariantValue    FromObject (const type_index& forTypeInfo, const Byte* objOfType) const;
                template    <typename CLASS>
                nonvirtual  VariantValue    FromObject (const CLASS& from) const;

            public:
                /**
                 *  This creates serializers for many common types.
                 *      o   Mapping<Key,Value>
                 *      o   Optional<T>
                 *      o   Range<T,TRAITS>
                 *      o   Sequence<T>
                 *      o   Set<T>
                 *  ###NYI    o   T[N]      -- so far cannot get to work
                 *      o   enum types (with eSTART/eEND @see Stroika_Define_Enum_Bounds for bounds checking)
                 *
                 *  This assumes the template parameters for the above objects are also already defined (mostly 'T' above).
                 *
                 *  This function also works (but is generally unneeded for) any of the types defined in
                 *  @see ResetToDefaultTypeRegistry () (int, short, String, etc).
                 *
                 *  Note - all these de-serializers will throw BadDataFormat exceptions if the data somehow doesnt
                 *  fit what the deserailizer expects.
                 */
                template    <typename T>
                static  TypeMappingDetails  MakeCommonSerializer ();

            public:
                /**
                 */
                template    <typename ENUM_TYPE>
                static  TypeMappingDetails  MakeCommonSerializer_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap);

            private:
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>&);
                template    <typename T>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Containers::Collection<T>&);
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>&);
                template    <typename T>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Memory::Optional<T>&);
                template    <typename T>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Sequence<T>&);
                template    <typename T>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Set<T>&);
                template    <typename T>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Containers::SortedCollection<T>&);
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Containers::SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>&);
                template    <typename T>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Containers::SortedSet<T>&);
#if 0
                // haven't gotten to work
                template    <typename T, size_t SZ>
                static  TypeMappingDetails MakeCommonSerializer_ (T ar[SZ]);
#endif
                template    <typename T, typename TRAITS>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Traversal::DiscreteRange<T, TRAITS>&);
                template    <typename T, typename TRAITS>
                static  TypeMappingDetails  MakeCommonSerializer_ (const Traversal::Range<T, TRAITS>&);
                template    <typename ENUM_TYPE>
                static  TypeMappingDetails  MakeCommonSerializer_ (const ENUM_TYPE&,  typename std::enable_if<std::is_enum<ENUM_TYPE>::value >::type* = 0);

            private:
                template    <typename ACTUAL_CONTAINER_TYPE>
                static  TypeMappingDetails  MakeCommonSerializer_WithSimpleAdd_ ();

#if 1
            public:
                //
                // soon to be private:??? @see MakeCommonSerializer
                //
                // this is for builtin C++ array (int a[3]) - not std::array).
                template    <typename T, size_t SZ>
                static  ObjectVariantMapper::TypeMappingDetails MakeCommonSerializer_Array ();
#endif

            private:
                template    <typename RANGE_TYPE>
                static  ObjectVariantMapper::TypeMappingDetails MakeCommonSerializer_Range_ ();


            public:
                //
                // soon to be private:??? @see MakeCommonSerializer
                //
                // by default serialize as an array (cuz serializing as mapping only works if LHS is
                // string).
                //
                // For that case - SHOULD use partial specializaiton (not sure how)
                //
                //  NB: To use this - you must add to your typemapper a mapping from KEY_TYPE to/from String (unless it is String).
                template    <typename KEY_TYPE, typename VALUE_TYPE>
                static  ObjectVariantMapper::TypeMappingDetails MakeCommonSerializer_MappingWithStringishKey ();

            private:
                nonvirtual  ObjectVariantMapper::TypeMappingDetails  MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields) const;
                nonvirtual  ObjectVariantMapper::TypeMappingDetails  MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields, function<void(VariantValue*)> preflightBeforeToObject) const;

            private:
                nonvirtual  TypeMappingDetails  Lookup_(const type_index& forTypeInfo) const;

            private:
                Set<TypeMappingDetails> fSerializers_;  // need Stroika set with separate traits-based key extractor/compare function
            };


            /**
             *  Structure to capture all the details of how to map between a VariantValue and an associated C++ structure.
             *  This CAN be direclyly constructed, and passed into the ObjectVariantMapper (via the Add method), but more commonly
             *  helpers like MakeCommonSerializer () or AddClass will be used.
             */
            struct  ObjectVariantMapper::TypeMappingDetails {
                type_index                                                                              fForType;
                function<VariantValue(const ObjectVariantMapper* mapper, const Byte* objOfType)>        fToVariantMapper;
                function<void(const ObjectVariantMapper* mapper, const VariantValue& d, Byte* into)>    fFromVariantMapper;

                TypeMappingDetails (
                    const type_index& forTypeInfo,
                    const function<VariantValue(const ObjectVariantMapper* mapper, const Byte* objOfType)>& toVariantMapper,
                    const function<void(const ObjectVariantMapper* mapper, const VariantValue& d, Byte* into)>& fromVariantMapper
                );

                nonvirtual  bool operator== (const TypeMappingDetails& rhs) const;
                nonvirtual  bool operator< (const TypeMappingDetails& rhs) const;
            };


            /**
             *  This is just for use the with the ObjectVariantMapper::AddClass<> (and related) methods, to describe a user-defined type (CLASS).
             */
            struct  ObjectVariantMapper::StructureFieldInfo {
                size_t      fOffset;
                type_index  fTypeInfo;
                String      fSerializedFieldName;

                StructureFieldInfo (size_t fieldOffset, type_index typeInfo, const String& serializedFieldName);
            };


            /**
             *  Greatly regret adding Macro, but it just seems SO HELPFUL (makes things much more terse).
             *  No need to use - but some may find it helpfull...
             *
             *  I don't know of any way in C++ without macro - to capture a member name (for use in decltype
             *  thing and offsetof()).
             *
             *  \note
             *      This macro uses offsetof(). According to the C++11 spec, offsetof() is only supported
             *      for 'standard-layout' objects (e.g. those without virtual functions, or other fancy
             *      c++ stuff). As near as I can tell, this always works, but we may need to revisit
             *      the approach/question (could  we use pointer to member?).
             */
#define     ObjectVariantMapper_StructureFieldInfo_Construction_Helper(CLASS,MEMBER,NAME)\
    DataExchange::ObjectVariantMapper::StructureFieldInfo (offsetof (CLASS, MEMBER), typeid (decltype (CLASS::MEMBER)), NAME)


            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<bool> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<signed char> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<short int> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<int> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<long int> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<long long int> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<unsigned char> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<unsigned short int> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<unsigned int> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<unsigned long int> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<unsigned long long int> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<float> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<double> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<long double> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Time::Date> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Time::DateTime> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Characters::String> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<VariantValue> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Time::Duration> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Mapping<Characters::String, Characters::String>> ();
            template    <>
            ObjectVariantMapper::TypeMappingDetails  ObjectVariantMapper::MakeCommonSerializer<Mapping<Characters::String, VariantValue>> ();


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ObjectVariantMapper.inl"

#endif  /*_Stroika_Foundation_DataExchange_ObjectVariantMapper_h_*/
