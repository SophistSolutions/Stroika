/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_ObjectVariantMapper_h_
#define _Stroika_Foundation_DataExchange_ObjectVariantMapper_h_ 1

#include "../StroikaPreComp.h"

#include <type_traits>
#include <typeindex>

#include "../Characters/String.h"
#include "../Characters/ToString.h"
#include "../Configuration/Enumeration.h"
#include "../Containers/Bijection.h"
#include "../Containers/Collection.h"
#include "../Containers/Mapping.h"
#include "../Containers/MultiSet.h"
#include "../Containers/Sequence.h"
#include "../Containers/Set.h"
#include "../Containers/SortedCollection.h"
#include "../Containers/SortedMapping.h"
#include "../Containers/SortedSet.h"
#include "../Execution/Synchronized.h"
#include "../IO/Network/URL.h"
#include "../Memory/Common.h"
#include "../Memory/Optional.h"
#include "../Traversal/DiscreteRange.h"
#include "../Traversal/Range.h"

#include "StructFieldMetaInfo.h"
#include "VariantValue.h"

/**
 *
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 *      @todo   Make AddCommonType() - when passed in an optional<T> - Require that
 *              the type T is already in the registry (like with AddClass). To debug!
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-558 ObjectVariantMapper::TypesRegistry should use KeyedCollection when that code is ready
 *              use KeyedCollection<> instead of Mapping for fSerializers - was using Set<> which is closer API wise, but Set<> has misfeature
 *              that adding when already there does nothing, and new KeyedCollection will have property - lilke Mapping - of replacing value.
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
 *                  something in the currnet form, or something using "T" - and map between them. And then
 *                  users of a particular type (e.g. Range()) - could avoid calling the default ctor, and just
 *                  explicitly call the right CTOR.
 *
 *                  Probably relatively LOW priority to fix, however.
 *
 *                  Also - this could cause some issues with the interoperability of To/FromGenericVariantMapper
 *                  and From/ToObjectMapper<T>.
 *
 *      @todo   NOTE and TODO
 *              The cast to Byte* loses some type safety (we may want to store the class size through template magic)
 *              in the struct type info record, so it can be validated against the offsets in the typeinfo (in debug builds).
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

namespace Stroika::Foundation {
    namespace DataExchange {

        using Characters::String;
        using Containers::Mapping;
        using Containers::Sequence;
        using Containers::Set;
        using Memory::Byte;

        /**
         *  \brief  ObjectVariantMapper can be used to map C++ types to and from variant-union types, which can be transparently mapped into and out of XML, JSON, etc.
         *
         *  ObjectVariantMapper IS COPYABLE. Make one instance, register your types into it and use this to
         *  serialized/ToObject
         *
         *
         *  \par Example Usage
         *      \code
         *      struct  SharedContactsConfig_ {
         *          bool                    fEnabled = false;
         *          DateTime                fLastSynchronizedAt;
         *          Mapping<String,String>  fThisPHRsIDToSharedContactID;
         *      };
         *
         *      ObjectVariantMapper mapper;
         *
         *      // register each of your mappable (even private) types
         *      mapper.AddClass<SharedContactsConfig_> ({
         *          ObjectVariantMapper::StructFieldInfo { L"Enabled", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fEnabled) },
         *          ObjectVariantMapper::StructFieldInfo { L"Last-Synchronized-At", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fLastSynchronizedAt) },
         *          ObjectVariantMapper::StructFieldInfo { L"This-HR-ContactID-To-SharedContactID-Map", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fThisPHRsIDToSharedContactID) },
         *      });
         *
         *      // OR Equivilently
         *      mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo> {
         *          { L"Enabled", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fEnabled) },
         *          { L"Last-Synchronized-At", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fLastSynchronizedAt) },
         *          { L"This-HR-ContactID-To-SharedContactID-Map", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fThisPHRsIDToSharedContactID) },
         *      });
         *
         *      SharedContactsConfig_   tmp;
         *      tmp.fEnabled = enabled;
         *      VariantValue v = mapper.Serialize  (tmp);
         *
         *      // at this point - we should have VariantValue object with "Enabled" field.
         *      // This can then be serialized using
         *
         *      Streams::MemoryStream<Byte>   tmpStream;
         *      DataExchange::JSON::PrettyPrint (v, tmpStream);
         *
         *      // THEN deserialized, and mapped back to C++ object form
         *      tmp = mapper.ToObject<SharedContactsConfig_> (DataExchange::JSON::Reader (tmpStream));
         *      if (tmp.fEnabled) {
         *          ...
         *      }
         *      \endcode
         */
        class ObjectVariantMapper {
        public:
            /**
             *  FromObjectMapperType<T> defines how to map from a given type to a VariantValue.
             *
             *  @see FromGenericObjectMapperType
             *  @see ToObjectMapperType
             */
            template <typename T>
            using FromObjectMapperType = function<VariantValue (const ObjectVariantMapper& mapper, const T* objOfType)>;

        public:
            /**
             *  ToObjectMapperType<T> defines how to map from a VariantValue to the given type;
             * 
             *  @see ToGenericObjectMapperType
             *  @see FromObjectMapperType
             */
            template <typename T>
            using ToObjectMapperType = function<void(const ObjectVariantMapper& mapper, const VariantValue& d, T* into)>;

        public:
            /**
             *  This is a low level mapper - use for a few internal purposes, like pointer to member (class member) mapping, and
             *  for internal storage of mappers.
             *
             *  For the short term, we assume this mapper is interchangable (binary copyable) to/from any FromObjectMapperType<T>
             *
             *  @see ToGenericObjectMapperType
             *  @see FromObjectMapperType<T>
             */
            using FromGenericObjectMapperType = FromObjectMapperType<void>;

        public:
            /**
             *  This is a low level mapper - use for a few internal purposes, like pointer to member (class member) mapping, and
             *  for internal storage of mappers.
             *
             *  For the short term, we assume this mapper is interchangable (binary copyable) to/from any ToObjectMapperType<T>
             *
             *  @see FromGenericObjectMapperType
             *  @see ToObjectMapperType<T>
             */
            using ToGenericObjectMapperType = ToObjectMapperType<void>;

        public:
            /**
             *  Defaults to installing basic type mappers (@see ResetToDefaultTypeRegistry).
             */
            ObjectVariantMapper ();

        public:
            /**
             *  Structure to capture all the details of how to map between a VariantValue and an associated C++ structure.
             *  This CAN be direclyly constructed, and passed into the ObjectVariantMapper (via the Add method), but more commonly
             *  helpers like MakeCommonSerializer () or AddClass will be used.
             */
            struct TypeMappingDetails {
                type_index                  fForType;
                FromGenericObjectMapperType fFromObjecttMapper;
                ToGenericObjectMapperType   fToObjectMapper;

                TypeMappingDetails ()                          = delete;
                TypeMappingDetails (const TypeMappingDetails&) = default;
                explicit TypeMappingDetails (const type_index& forTypeInfo, const FromGenericObjectMapperType& fromObjectMapper, const ToGenericObjectMapperType& toObjectMapper);
                template <typename T, typename ENABLE_IF = enable_if_t<not is_same_v<T, void>>>
                TypeMappingDetails (const type_index& forTypeInfo, const FromObjectMapperType<T>& fromObjectMapper, const ToObjectMapperType<T>& toObjectMapper);

                nonvirtual TypeMappingDetails& operator= (const TypeMappingDetails& rhs) = default;

                nonvirtual bool operator== (const TypeMappingDetails& rhs) const;
                nonvirtual bool operator< (const TypeMappingDetails& rhs) const;

                template <typename T>
                static FromObjectMapperType<T> FromObjectMapper (const FromGenericObjectMapperType& fromObjectMapper);
                template <typename T>
                nonvirtual FromObjectMapperType<T> FromObjectMapper () const;
                template <typename T>
                static ToObjectMapperType<T> ToObjectMapper (const ToGenericObjectMapperType& toObjectMapper);
                template <typename T>
                nonvirtual ToObjectMapperType<T> ToObjectMapper () const;

                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual String ToString () const;
            };

        public:
            struct TypesRegistry;

        public:
            /**
             *  Returns the current set of type mappers.
             */
            nonvirtual TypesRegistry GetTypeMappingRegistry () const;

        public:
            /**
             *  Sets the current set of type mappers.
             */
            nonvirtual void SetTypeMappingRegistry (const TypesRegistry& s);

        public:
            /**
             *  Adds the given type mapper(s). This could have been called 'Merge'.
             *
             *  \note   If a type already exists, the subsequent calls overwrite previous mappings. Only one mapping can exist
             *          at a time for a given type.
             *
             *  \par Example Usage **Add custom object reader**
             *      \code
             *           struct RGBColor {
             *               uint8_t red;
             *               uint8_t green;
             *               uint8_t blue;
             *           };
             *
             *           ObjectVariantMapper mapper;
             *
             *           mapper.Add<RGBColor> (
             *              [](const ObjectVariantMapper& mapper, const RGBColor* obj) -> VariantValue {
             *                  return L"#" + Characters::Format (L"%02x%02x%02x", obj->red, obj->green, obj->blue);
             *              },
             *              [](const ObjectVariantMapper& mapper, const VariantValue& d, RGBColor* intoObj) -> void {
             *                  String tmpInBuf = d.As<String> ();
             *                  if (tmpInBuf.length () != 7) {
             *                      Execution::Throw (DataExchange::BadFormatException (L"RGBColor should have length 7"));
             *                  }
             *                  if (tmpInBuf[0] != '#') {
             *                      Execution::Throw (DataExchange::BadFormatException (L"RGBColor must start with #"));
             *                  }
             *                  auto readColorComponent = [](const wchar_t* start, const wchar_t* end) -> uint8_t {
             *                      wchar_t buf[1024];
             *                      Require (end - start < static_cast<ptrdiff_t> (NEltsOf (buf)));
             *                      memcpy (buf, start, (end - start) * sizeof (wchar_t));
             *                      buf[(end - start)] = '\0';
             *                      wchar_t* e      = nullptr;
             *                      auto     result = std::wcstoul (buf, &e, 16);
             *                      if (e != buf + 2) {
             *                          Execution::Throw (DataExchange::BadFormatException (L"expected 6 hex bytes"));
             *                      }
             *                      Assert (result <= 255);
             *                      return static_cast<uint8_t> (result);
             *                  };
             *                  intoObj->red   = readColorComponent (tmpInBuf.c_str () + 1, tmpInBuf.c_str () + 3);
             *                  intoObj->green = readColorComponent (tmpInBuf.c_str () + 3, tmpInBuf.c_str () + 5);
             *                  intoObj->blue  = readColorComponent (tmpInBuf.c_str () + 5, tmpInBuf.c_str () + 7);
             *              }
             *          );
             *      \endcode
             *
             */
            nonvirtual void Add (const TypeMappingDetails& s);
            nonvirtual void Add (const Set<TypeMappingDetails>& s);
            nonvirtual void Add (const TypesRegistry& s);
            nonvirtual void Add (const ObjectVariantMapper& s);
            template <typename T>
            nonvirtual void Add (const FromObjectMapperType<T>& fromObjectMapper, const ToObjectMapperType<T>& toObjectMapper);

        public:
            /**
             *  Shortcut for Add (MakeCommonSerializer<T> ());
             *
             *  So - this is supported for any type for which (@see MakeCommonSerializer) is supported.
             *
             *  Note this this is not needed (because it's done by default), but is supported,
             *  for the builtin types.
             *
             *  @see MakeCommonSerializer
             */
            template <typename T>
            nonvirtual void AddCommonType ();

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
             *      o   IO::Network::URL
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
            nonvirtual void ResetToDefaultTypeRegistry ();

        public:
            struct StructFieldInfo;

        public:
            /**
             *  Adds the given class (defined in explicit template argument) with the given list of field.
             *  Also, optionally provide a 'readPreflight' function to be applied to the read-in VariantValue object before
             *  decomposing (into C++ structs), as a helpful backward compatible file format hook.
             *
             *  \req    AddClass<> requires that each field data type already be pre-loaded into the
             *          Registry, opr be provided as an optional parameter to the StructFieldInfo.
             *
             *  \par Example Usage
             *      \code
             *      struct  MyConfig_ {
             *          IO::Network::URL        fURL1_;
             *          IO::Network::URL        fURL2_;
             *      };
             *
             *      ObjectVariantMapper mapper;
             *      mapper.AddCommonType<IO::Network::URL> ();      // add default type mapper (using default URL parse)
             *
             *      // register each of your mappable (even private) types
             *      mapper.AddClass<MyConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo> {
             *          { L"fURL1_", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fURL1_) },        // use default parser
             *          // for fURL2_ - instead - allow parsing of things like 'localhost:1234' - helpful for configuration files
             *          { L"fURL2_", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fURL2_), ObjectVariantMapper::MakeCommonSerializer<IO::Network::URL> (IO::Network::URL::eFlexiblyAsUI)  },
             *      });
             *
             *      MyConfig_   tmp;
             *      tmp.fURL2_ = IO::Network::URL (L"localhost:1234", IO::Network::URL::eFlexiblyAsUI);
             *      VariantValue v = mapper.Serialize  (tmp);
             *
             *      Streams::MemoryStream<Byte>   tmpStream;
             *      DataExchange::JSON::PrettyPrint (v, tmpStream);
             *
             *      // THEN deserialized, and mapped back to C++ object form
             *      tmp = mapper.ToObject<MyConfig_> (DataExchange::JSON::Reader (tmpStream));
             *      \endcode
             */
            template <typename CLASS>
            nonvirtual void AddClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, function<void(VariantValue*)> preflightBeforeToObject = nullptr);

        public:
            /**
             *  \brief  Like @AddClass<> - adding a new class based on parameters - but based on the argument baseClass.
             *
             *  \par Example Usage
             *      \code
             *      struct BaseObj_ {
             *          int  fVV1{};
             *      };
             *      struct Derived_ : BaseObj_ {
             *          int  fVV2{};
             *      };
             *      ObjectVariantMapper mapper;
             *      mapper.AddClass<BaseObj_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
             *          {L"fVV1", Stroika_Foundation_DataExchange_StructFieldMetaInfo (BaseObj_, fVV1)},
             *      });
             *      mapper.AddSubClass<Derived_, BaseObj_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
             *          {L"fVV2", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Derived_, fVV2)},
             *      });
             *      \endcode
             */
            template <typename CLASS, typename BASE_CLASS>
            nonvirtual void AddSubClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, function<void(VariantValue*)> preflightBeforeToObject = nullptr);

        public:
            /**
             *  Returns the function that does the data mapping. This can be used as an optimization to
             *  avoid multiple lookups of the mapper for a given type (say when reading or writing an array).
            */
            template <typename T>
            nonvirtual ToObjectMapperType<T> ToObjectMapper () const;

        public:
            /**
             *  Convert a VariantValue object into any C++ object - using the type converters already registered in
             *  this mapper.
             *
             *  The overloads that takes 'toObjectMapper' are just an optimization, and need not be used, but if used, the value
             *  passed in MUST the the same as that returned by ToObjectMapper ().
             */
            template <typename T>
            nonvirtual T ToObject (const VariantValue& v) const;
            template <typename T>
            nonvirtual void ToObject (const VariantValue& v, T* into) const;
            template <typename T>
            nonvirtual void ToObject (const ToObjectMapperType<T>& toObjectMapper, const VariantValue& v, T* into) const;
            template <typename T>
            nonvirtual T ToObject (const ToObjectMapperType<T>& toObjectMapper, const VariantValue& v) const;

        public:
            /**
             *  Returns the function that does the data mapping. This can be used as an optimization to
             *  avoid multiple lookups of the mapper for a given type (say when reading or writing an array).
             */
            template <typename T>
            nonvirtual FromObjectMapperType<T> FromObjectMapper () const;

        public:
            /**
             *  Convert a C++ object to a VariantValue object - using the type converters already registered in
             *  this mapper.
             *
             *  The overload that takes 'fromObjectMapper' is just an optimization, and need not be used, but if used, the value
             *  passed in MUST the the same as that returned by FromObjectMapper ().
             */
            template <typename T>
            nonvirtual VariantValue FromObject (const T& from) const;
            template <typename T>
            nonvirtual VariantValue FromObject (const FromObjectMapperType<T>& fromObjectMapper, const T& from) const;

        public:
            /**
             *  This creates serializers for many common types.
             *      o   Bjjection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>
             *      o   Collection<T>
             *      o   Traversal::DiscreteRange<T, TRAITS>
             *      o   Mapping<Key,Value>
             *      o   optional<T>
             *      o   Optional<T>
             *      o   Range<T,TRAITS>
             *      o   Sequence<T>
             *      o   Set<T>
             *      o   SortedCollection<T>
             *      o   SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>
             *      o   SortedSet<T>
             *      o   Synchronized<T>
             *      o   vector<T>
             *      o   enum types (with eSTART/eEND @see Stroika_Define_Enum_Bounds for bounds checking)
             *      o   T[N]
             *
             *  As well as
             *      o   all POD types (integer, floating point, bool)
             *      o   void (yes - this is useful for templating)
             *      o   Time::Date
             *      o   Time::DateTime
             *      o   Characters::String
             *      o   VariantValue
             *      o   IO::Network::URL
             *      o   Time::TimeOfDay
             *
             *  This assumes the template parameters for the above objects are also already defined (mostly 'T' above).
             *
             *  This function also works (but is generally unneeded for) any of the types defined in
             *  @see ResetToDefaultTypeRegistry () (int, short, String, etc).
             *
             *  Note - all these de-serializers will throw BadDataFormat exceptions if the data somehow doesn't
             *  fit what the deserailizer expects.
             *
             *  \note   For type Mapping<KEY,VALUE>, this could use either the mapping function
             *          MakeCommonSerializer_MappingWithStringishKey or MakeCommonSerializer_MappingAsArrayOfKeyValuePairs.
             *          MakeCommonSerializer_MappingAsArrayOfKeyValuePairs is more general, but MakeCommonSerializer_MappingWithStringishKey
             *          is more commonly the desired output mapping, and so is the default.
             *
             *  \note   MakeCommonSerializer<IO::Network::URL> takes an optional argument IO::Network::URL::ParseOptions which defaults to AsFulLURL, but can
             *          be set to IO::Network::URL::ParseOptions::eFlexiblyAsUI to allow easier use for configuration files.
             *
             *  \note   It is legal to call MakeCommonSerializer<> on a type where it only knows how to construct the base class type (struct derived : base {})
             *          in which case it produces a serializer that will still work with the given type T, but will only capture the data from base.
             */
            template <typename T, typename... ARGS>
            static TypeMappingDetails MakeCommonSerializer (ARGS&&... args);

        public:
            /**
             */
            template <typename ENUM_TYPE>
            static TypeMappingDetails MakeCommonSerializer_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap);
            template <typename ENUM_TYPE>
            static TypeMappingDetails MakeCommonSerializer_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap = Configuration::DefaultNames<ENUM_TYPE>::k);

        public:
            /**
             */
            template <typename ENUM_TYPE>
            static TypeMappingDetails MakeCommonSerializer_EnumAsInt ();

        public:
            /**
             *  This works on Any Mapping<KEY,VALUE>, where the Key can be Mapped to / from a String, using
             *  an already defined typemapper (from KEY_TYPE to/from String) or be of type String.
             *
             *  It produces a (JSON) output of { 'field1': value1, 'field2' : value2 ... } representation of the mapping.
             *
             *  @see MakeCommonSerializer_MappingAsArrayOfKeyValuePairs
             */
            template <typename ACTUAL_CONTAINTER_TYPE, typename KEY_TYPE = typename ACTUAL_CONTAINTER_TYPE::key_type, typename VALUE_TYPE = typename ACTUAL_CONTAINTER_TYPE::mapped_type>
            static TypeMappingDetails MakeCommonSerializer_MappingWithStringishKey ();

        public:
            /**
             *  This works on Any Mapping<KEY,VALUE>.
             *
             *  It produces a (JSON) output of [ [ field1, value1 ], [ field2, value2 ], ... } representation of the mapping.
             *
             *  @see MakeCommonSerializer_MappingWithStringishKey
             */
            template <typename ACTUAL_CONTAINTER_TYPE, typename KEY_TYPE = typename ACTUAL_CONTAINTER_TYPE::key_type, typename VALUE_TYPE = typename ACTUAL_CONTAINTER_TYPE::mapped_type>
            static TypeMappingDetails MakeCommonSerializer_MappingAsArrayOfKeyValuePairs ();

        public:
            /**
             *  Can be used with any container class which you append with Containers::Adapters::Adder
             */
            template <typename ACTUAL_CONTAINER_TYPE>
            static TypeMappingDetails MakeCommonSerializer_WithAdder ();

        public:
            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;

        private:
            static TypeMappingDetails MakeCommonSerializer_ (const void*);
            template <typename DOMAIN_TYPE, typename RANGE_TYPE>
            static TypeMappingDetails MakeCommonSerializer_ (const Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE>*);
            template <typename T>
            static TypeMappingDetails MakeCommonSerializer_ (const Containers::Collection<T>*);
            template <typename T, typename TRAITS>
            static TypeMappingDetails MakeCommonSerializer_ (const Traversal::DiscreteRange<T, TRAITS>*);
            template <typename KEY_TYPE, typename VALUE_TYPE>
            static TypeMappingDetails MakeCommonSerializer_ (const Mapping<KEY_TYPE, VALUE_TYPE>*);
            template <typename T, typename TRAITS>
            static TypeMappingDetails MakeCommonSerializer_ (const Memory::Optional<T, TRAITS>*);
            template <typename T>
            static TypeMappingDetails MakeCommonSerializer_ (const optional<T>*);
            template <typename T, typename TRAITS>
            static TypeMappingDetails MakeCommonSerializer_ (const Traversal::Range<T, TRAITS>*);
            template <typename T>
            static TypeMappingDetails MakeCommonSerializer_ (const Sequence<T>*);
            template <typename T>
            static TypeMappingDetails MakeCommonSerializer_ (const Set<T>*);
            template <typename T>
            static TypeMappingDetails MakeCommonSerializer_ (const Containers::SortedCollection<T>*);
            template <typename KEY_TYPE, typename VALUE_TYPE>
            static TypeMappingDetails MakeCommonSerializer_ (const Containers::SortedMapping<KEY_TYPE, VALUE_TYPE>*);
            template <typename T>
            static TypeMappingDetails MakeCommonSerializer_ (const Containers::SortedSet<T>*);
            template <typename T, typename TRAITS>
            static TypeMappingDetails MakeCommonSerializer_ (const Execution::Synchronized<T, TRAITS>*);
            static TypeMappingDetails MakeCommonSerializer_ (const IO::Network::URL*, IO::Network::URL::ParseOptions parseOptions = IO::Network::URL::eAsFullURL);
            template <typename T>
            static TypeMappingDetails MakeCommonSerializer_ (const vector<T>*);
            template <typename T>
            static TypeMappingDetails MakeCommonSerializer_ (const T*, enable_if_t<is_enum_v<T>>* = 0);
            template <typename T, size_t SZ>
            static TypeMappingDetails MakeCommonSerializer_ (const T (*)[SZ]);

        private:
            template <typename KEY_TYPE, typename VALUE_TYPE, typename ACTUAL_CONTAINER_TYPE>
            static TypeMappingDetails MakeCommonSerializer_WithKeyValuePairAdd_ ();

        private:
            template <typename RANGE_TYPE>
            static TypeMappingDetails MakeCommonSerializer_Range_ ();

        private:
            template <typename CLASS>
            nonvirtual TypeMappingDetails MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Traversal::Iterable<StructFieldInfo>& fields, const function<void(VariantValue*)>& preflightBeforeToObject) const;
            template <typename CLASS, typename BASE_CLASS>
            nonvirtual TypeMappingDetails MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Traversal::Iterable<StructFieldInfo>& fields, const function<void(VariantValue*)>& preflightBeforeToObject, const optional<type_index>& baseClassTypeInfo) const;

        private:
            nonvirtual TypeMappingDetails Lookup_ (const type_index& forTypeInfo) const;

        public:
            /**
             */
            struct TypesRegistry {
            public:
                TypesRegistry (const Mapping<type_index, TypeMappingDetails>& src);
                TypesRegistry (const Set<TypeMappingDetails>& src);

            public:
                nonvirtual optional<TypeMappingDetails> Lookup (type_index t) const;

            public:
                nonvirtual void Add (const TypeMappingDetails& typeMapDetails);

            public:
                nonvirtual Traversal::Iterable<TypeMappingDetails> GetMappers () const;

            public:
                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual String ToString () const;

            private:
                Mapping<type_index, TypeMappingDetails> fSerializers;
            };

        private:
            TypesRegistry fTypeMappingRegistry_;
        };

        /**
         *  This is just for use the with the ObjectVariantMapper::AddClass<> (and related) methods, to describe a
         *  user-defined type (CLASS).
         *
         *  \par Example Usage
         *       \code
         *          ObjectVariantMapper::StructFieldInfo {L"fInt1", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fInt1)},
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          ObjectVariantMapper::StructFieldInfo {L"fInt2", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fInt2), ObjectVariantMapper::StructFieldInfo::eOmitNullFields},
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          ObjectVariantMapper::StructFieldInfo {L"fBasicArray1", Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fBasicArray1), ObjectVariantMapper::MakeCommonSerializer<int[5]> ()},
         *      \endcode
         *
         */
        struct ObjectVariantMapper::StructFieldInfo {
            /**
             */
            enum class NullFieldHandling {
                eOmit,
                eInclude,

                Stroika_Define_Enum_Bounds (eOmit, eInclude)
            };
            static constexpr NullFieldHandling eOmitNullFields    = NullFieldHandling::eOmit;
            static constexpr NullFieldHandling eIncludeNullFields = NullFieldHandling::eInclude;

            StructFieldMetaInfo fFieldMetaInfo;
            String              fSerializedFieldName;
            // nb: we use Optional_Indirect_Storage - both to save space in the overwhelmingly most common case - not used
            Memory::Optional_Indirect_Storage<TypeMappingDetails> fOverrideTypeMapper;
            NullFieldHandling                                     fNullFields;

            /**
             *  \note   - the serializedFieldName parameter to the template (const wchar_t) overload of StructFieldInfo must be an array
             *          with application lifetime (that is static C++ constant). This is to make the common case slightly more efficient.
             */
            StructFieldInfo (const String& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo, const optional<TypeMappingDetails>& overrideTypeMapper = {}, NullFieldHandling nullFields = NullFieldHandling::eInclude);
            StructFieldInfo (const String& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo, NullFieldHandling nullFields);
            template <int SZ>
            StructFieldInfo (const wchar_t (&serializedFieldName)[SZ], const StructFieldMetaInfo& fieldMetaInfo, NullFieldHandling nullFields);
            template <int SZ>
            StructFieldInfo (const wchar_t (&serializedFieldName)[SZ], const StructFieldMetaInfo& fieldMetaInfo, const optional<TypeMappingDetails>& overrideTypeMapper = {}, NullFieldHandling nullFields = NullFieldHandling::eInclude);
        };

        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<bool> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<signed char> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<short int> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<int> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<long int> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<long long int> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned char> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned short int> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned int> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned long int> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<unsigned long long int> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<float> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<double> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<long double> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::Date> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::DateTime> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Characters::String> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<VariantValue> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::Duration> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Mapping<Characters::String, Characters::String>> ();
        template <>
        ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Mapping<Characters::String, VariantValue>> ();
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectVariantMapper.inl"

#endif /*_Stroika_Foundation_DataExchange_ObjectVariantMapper_h_*/
