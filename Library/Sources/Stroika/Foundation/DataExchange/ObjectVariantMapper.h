/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_ObjectVariantMapper_h_
#define _Stroika_Foundation_DataExchange_ObjectVariantMapper_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>

#include <filesystem>
#include <type_traits>
#include <typeindex>
#include <variant>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Configuration/StdCompat.h"
#include "Stroika/Foundation/Containers/Bijection.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/MultiSet.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Containers/SortedCollection.h"
#include "Stroika/Foundation/Containers/SortedKeyedCollection.h"
#include "Stroika/Foundation/Containers/SortedMapping.h"
#include "Stroika/Foundation/Containers/SortedMultiSet.h"
#include "Stroika/Foundation/Containers/SortedSet.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"
#include "Stroika/Foundation/Traversal/Range.h"

#include "StructFieldMetaInfo.h"
#include "VariantValue.h"

/**
 *
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-558 ObjectVariantMapper::TypesRegistry should use KeyedCollection when that code is ready
 *              use KeyedCollection<> instead of Mapping for fSerializers - was using Set<> which is closer API wise, but Set<> has misfeature
 *              that adding when already there does nothing, and new KeyedCollection will have property - like Mapping - of replacing value.
 *
 *      @todo   Fix MakeCommonSerializer() for tuple<> to be variadic.
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
 *              for the serializer/deserializers adds costs.
 *
 *              When serializing / deserializing - (e.g to/from JSON or XML) - we construct DOM tree which is
 *              intrinsically not very cost effective. We DO have the XML sax parser (but that wont work with this).
 *
 *      @todo   Current serializer/de-serializer API needlessly requires that objects have default CTOR.
 *              https://stroika.atlassian.net/browse/STK-1015
 *
 *      @todo   NOTE and TODO
 *              The cast to byte* loses some type safety (we may want to store the class size through template magic)
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
 *              This might allow this code to work better with a more efficient json-reader/writer/xml/reader/writer
 *              that also used these sorts of intermediate objects.
 *
 *              Of course, this ALSO might be implementable without any API changes - just using a special 'rep' for
 *              VariantValue - that was smart about how to it would accessed etc. Needs some thought.
 *
 *              Anyhow - this is a long-term todo item, so no need to work out details now.
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-743 - Add SupportsAdder concept and use that to simplify use in ObjectVariantMapper
 */

namespace Stroika::Foundation::DataExchange {
    class InternetMediaType;
}
namespace Stroika::Foundation::IO::Network {
    class CIDR;
    class InternetAddress;
    class URI;
}
namespace Stroika::Foundation::Memory {
    class BLOB;
}
namespace Stroika::Foundation::DataExchange {

    using Characters::String;
    using Containers::KeyedCollection;
    using Containers::Mapping;
    using Containers::Sequence;
    using Containers::Set;

    /**
     *  copyable is really needed; default_initializable is needed unless you do some work.
     *      &&DRAFT NOT USED YET
     */
    template <typename T>
    concept IObjectVariantMapper_Serializable = std::copy_constructible<T>;

    /**
     *      &&DRAFT NOT USED YET
     * &&& NOT RIGHT - JUST PROTYPE..fiddling
     */
    template <typename T>
    concept IObjectVariantMapper_SerializableOrGeneric = IObjectVariantMapper_Serializable<T> or same_as<T, void>;

    /**
     *      &&DRAFT NOT USED YET
     * &&& NOT RIGHT - JUST PROTYPE..fiddling
     */
    template <typename T>
    concept IObjectVariantMapper_AutomaticallySerializable = IObjectVariantMapper_Serializable<T> and default_initializable<T>;

    /**
     *  This isn't very expensive (if your compiler supports cheap thread_local variables) - but its not free, and not very
     *  useful if you have reliable data (just produces better exception messages decoding structured data).
     */
#if !defined(qStroika_Foundation_DataExchange_ObjectVariantMapper_Activities)
#define qStroika_Foundation_DataExchange_ObjectVariantMapper_Activities 1
#endif

    /**
     *  \brief  ObjectVariantMapper can be used to map C++ types to and from variant-union types, which can be transparently mapped into and out of XML, JSON, etc.
     *
     *  ObjectVariantMapper IS COPYABLE. Make one instance, register your types into it and use this to
     *  serialized/ToObject
     *
     *  \par Example Usage
     *      \code
     *          struct  SharedContactsConfig_ {
     *              bool                    fEnabled = false;
     *              optional<DateTime>      fLastSynchronizedAt;
     *              Mapping<String,String>  fThisPHRsIDToSharedContactID;
     *          };
     *
     *          ObjectVariantMapper mapper;
     *
     *          // register each of your mappable (even private) types
     *          mapper.AddClass<SharedContactsConfig_> ({
     *              ObjectVariantMapper::StructFieldInfo{ "Enabled"sv, StructFieldMetaInfo{&SharedContactsConfig_::fEnabled} },
     *              ObjectVariantMapper::StructFieldInfo{ "Last-Synchronized-At"sv, StructFieldMetaInfo{&SharedContactsConfig_::fLastSynchronizedAt} },
     *              ObjectVariantMapper::StructFieldInfo{ "This-HR-ContactID-To-SharedContactID-Map"sv, StructFieldMetaInfo{&SharedContactsConfig_::fThisPHRsIDToSharedContactID} },
     *          }
     *          , {.fOmitNullEntriesInFromObject = true});
     *
     *          // OR Equivalently (but more commonly/briefly)
     *          mapper.AddClass<SharedContactsConfig_> ({
     *              { "Enabled"sv, &SharedContactsConfig_::fEnabled },
     *              { "Last-Synchronized-At"sv, &SharedContactsConfig_::fLastSynchronizedAt },
     *              { "This-HR-ContactID-To-SharedContactID-Map"sv, &SharedContactsConfig_::fThisPHRsIDToSharedContactID },
     *          });
     *
     *          SharedContactsConfig_   tmp;
     *          tmp.fEnabled = enabled;
     *          VariantValue v = mapper.FromObject (tmp);
     *
     *          // at this point - we should have VariantValue object with "Enabled" field.
     *          // This can then be serialized using
     *
     *          Streams::MemoryStream<byte>   tmpStream;
     *          DataExchange::JSON::PrettyPrint (v, tmpStream);
     *
     *          // THEN deserialized, and mapped back to C++ object form
     *          tmp = mapper.ToObject<SharedContactsConfig_> (DataExchange::JSON::Reader (tmpStream));
     *          if (tmp.fEnabled) {
     *              ...
     *          }
     *      \endcode
     * 
     *  \note   Future Design Note:
     *              It would be nice of offer a way to define mappers using c++ user defined attributes
     *              if these existed (@see https://manu343726.github.io/2019-07-14-reflections-on-user-defined-attributes/)
     *              
     *              The current approach is more MODULAR (so not bad), but slightly more verbose, and most other languages
     *              (e.g. c#, java) allow directly annotating objects for serialization (more convenient/terse - appropriate
     *              for some cases like private-ish objects).
     * 
     *              But will have to wait a bit it appears (MSFT still doesn't support reflections TS, and I'm not sure
     *              this can be done with reflections TS).
     */
    class [[nodiscard]] ObjectVariantMapper {
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
         * 
         *  \note - Design Note:
         *          the choice of having an T* into parameter instead of returning T. The returning approach is
         *          in many ways more natural. Doing T *into has the DEFECT, that it requires T to be default constructible (or greatly encourages ;-)).
         *          But using T* into works with subclassing, whereas its less clear how to make the return T approach work with subclassing.
         * 
         *          @todo - perhaps have it RETURN a unique_ptr<T>?
         * 
         *          \see https://stroika.atlassian.net/browse/STK-1015  (we hope to lose the default_initializable at some point)
         */
        template <typename T>
        using ToObjectMapperType = function<void (const ObjectVariantMapper& mapper, const VariantValue& d, T* into)>;

    public:
        /**
         *  This is a low level mapper - use for a few internal purposes, like pointer to member (class member) mapping, and
         *  for internal storage of mappers.
         *
         *  \note For performance reasons, we treat this as interchangeable with the real FromObjectMapperType<T>, but
         *        see https://stroika.atlassian.net/browse/STK-601 for details but, with UBSan, we need todo an extra
         *        layer of lambdas mapping, cuz it detects this not totally kosher cast.
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
         *  \note For performance reasons, we treat this as interchangeable with the real FromObjectMapperType<T>, but
         *        see https://stroika.atlassian.net/browse/STK-601 for details but, with ubsan, we need todo an extra
         *        layer of lambdas mapping, cuz it detects this not totally kosher cast.
         *
         *  @see FromGenericObjectMapperType
         *  @see ToObjectMapperType<T>
         */
        using ToGenericObjectMapperType = ToObjectMapperType<void>;

    public:
        /**
         */
        template <Configuration::StdCompat::formattable<wchar_t> T>
        static const FromObjectMapperType<T> kTraceFromObjectMapper;

    public:
        /**
         */
        template <Configuration::StdCompat::formattable<wchar_t> T>
        static const ToObjectMapperType<T> kTraceToObjectMapper;

    public:
        /**
         *  Defaults to installing basic type mappers (@see ResetToDefaultTypeRegistry).
         */
        ObjectVariantMapper ();

    public:
        /**
         *  Structure to capture all the details of how to map between a VariantValue and an associated C++ structure.
         *  This CAN be directly constructed, and passed into the ObjectVariantMapper (via the Add method), but more commonly
         *  helpers like MakeCommonSerializer () or AddClass will be used.
         *
         *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>         * 
         *  \note fFromObjectMapper_ is nullptr, then this field is added as nullptr.
         *  \note toObjectMapper is nullptr, then it is simply not called (as if did nothing or empty function)
         */
        struct TypeMappingDetails {
        public:
            /**
             *  \par Example Usage
             *      \code
             *          return TypeMappingDetails{fromObjectMapper, toObjectMapper, typeid (ACTUAL_CONTAINER_TYPE)};
             *      \endcode
             *
             *  \par Example Usage
             *      \code
             *          auto myReadOnlyPropertyTypeMapper = ObjectVariantMapper::TypeMappingDetails{
             *              ObjectVariantMapper::FromObjectMapperType<MyType2Serialize1_> ([] (const ObjectVariantMapper& mapper, const MyType2Serialize1_* objOfType) -> VariantValue {
             *                  return VariantValue{objOfType->fEnabled ? 2 : 99};
             *              }),
             *              ObjectVariantMapper::ToObjectMapperType<MyType2Serialize1_> (nullptr)};
             *      \endcode
             */
            TypeMappingDetails ()                              = delete;
            TypeMappingDetails (const TypeMappingDetails&)     = default;
            TypeMappingDetails (TypeMappingDetails&&) noexcept = default;
            explicit TypeMappingDetails (const FromGenericObjectMapperType& fromObjectMapper,
                                         const ToGenericObjectMapperType& toObjectMapper, const type_index& forTypeInfo);
            template <typename T>
            TypeMappingDetails (const FromObjectMapperType<T>& fromObjectMapper, const ToObjectMapperType<T>& toObjectMapper,
                                const type_index& forTypeInfo = type_index{typeid (T)})
                requires (not same_as<T, void>);

        public:
            nonvirtual TypeMappingDetails& operator= (TypeMappingDetails&& rhs) noexcept = default;
            nonvirtual TypeMappingDetails& operator= (const TypeMappingDetails& rhs)     = default;

        public:
            /**
             */
            nonvirtual strong_ordering operator<=> (const TypeMappingDetails& rhs) const;

        public:
            /**
             */
            nonvirtual bool operator== (const TypeMappingDetails& rhs) const;

        public:
            /**
             *  \see FromGenericObjectMapperType
             *  \see GetGenericFromObjectMapper
             * 
             *  \req 'T' same_as one TypeMappingDetails constructed from (using type_info dynamic type compare)
             */
            template <typename T>
            static FromObjectMapperType<T> FromObjectMapper (const FromGenericObjectMapperType& fromObjectMapper);
            template <typename T>
            nonvirtual FromObjectMapperType<T> FromObjectMapper () const;

        public:
            /**
             *  \see ToGenericObjectMapperType
             *  \see GetGenericToObjectMapper
             * 
             *  \req 'T' same_as one TypeMappingDetails constructed from (using type_info dynamic type compare)
             */
            template <typename T>
            static ToObjectMapperType<T> ToObjectMapper (const ToGenericObjectMapperType& toObjectMapper);
            template <typename T>
            nonvirtual ToObjectMapperType<T> ToObjectMapper () const;

        public:
            /**
             */
            nonvirtual type_index GetForType () const;

        public:
            /**
             */
            nonvirtual FromGenericObjectMapperType GetGenericFromObjectMapper () const;

        public:
            /**
             */
            nonvirtual ToGenericObjectMapperType GetGenericToObjectMapper () const;

        public:
            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;

        public:
            [[deprecated ("Since Stroika v3.0d7 - use overload with typeInfo third")]] explicit TypeMappingDetails (
                const type_index& forTypeInfo, const FromGenericObjectMapperType& fromObjectMapper, const ToGenericObjectMapperType& toObjectMapper)
                : TypeMappingDetails{fromObjectMapper, toObjectMapper, forTypeInfo}
            {
            }
            template <typename T>
            [[deprecated ("Since Stroika v3.0d7 - use overload with typeInfo third")]] TypeMappingDetails (const type_index& forTypeInfo,
                                                                                                           const FromObjectMapperType<T>& fromObjectMapper,
                                                                                                           const ToObjectMapperType<T>& toObjectMapper)
                requires (not same_as<T, void>)
                : TypeMappingDetails{fromObjectMapper, toObjectMapper, forTypeInfo}
            {
            }

        private:
            template <typename T>
            static FromGenericObjectMapperType mkGenericFromMapper_ (const FromObjectMapperType<T>& fromObjectMapper);
            template <typename T>
            static ToGenericObjectMapperType mkGenericToMapper_ (const ToObjectMapperType<T>& toObjectMapper);

        private:
            type_index                  fForType_;
            FromGenericObjectMapperType fFromObjectMapper_;
            ToGenericObjectMapperType   fToObjectMapper_;
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
         *          using IO::Network::CIDR;
         *          ObjectVariantMapper mapper;
         *          mapper.Add<CIDR> ([](const ObjectVariantMapper& mapper, const CIDR* obj) -> VariantValue { return obj->ToString (); },
         *                            [](const ObjectVariantMapper& mapper, const VariantValue& d, CIDR* intoObj) -> void { *intoObj = CIDR{d.As<String> ()}; }
         *                          );
         *      \endcode
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
         *                  return "#" + Characters::Format (L"%02x%02x%02x", obj->red, obj->green, obj->blue);
         *              },
         *              [](const ObjectVariantMapper& mapper, const VariantValue& d, RGBColor* intoObj) -> void {
         *                  String tmpInBuf = d.As<String> ();
         *                  if (tmpInBuf.length () != 7) {
         *                      Execution::Throw (DataExchange::BadFormatException{"RGBColor should have length 7")};
         *                  }
         *                  if (tmpInBuf[0] != '#') {
         *                      Execution::Throw (DataExchange::BadFormatException{"RGBColor must start with #")};
         *                  }
         *                  auto readColorComponent = [](const wchar_t* start, const wchar_t* end) -> uint8_t {
         *                      wchar_t buf[1024];
         *                      Require (end - start < static_cast<ptrdiff_t> (NEltsOf (buf)));
         *                      memcpy (buf, start, (end - start) * sizeof (wchar_t));
         *                      buf[(end - start)] = '\0';
         *                      wchar_t* e      = nullptr;
         *                      auto     result = wcstoul (buf, &e, 16);
         *                      if (e != buf + 2) {
         *                          Execution::Throw (DataExchange::BadFormatException{"expected 6 hex bytes")};
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
         *      \code
         *          inline const DataExchange::ObjectVariantMapper PointerType::kMapper = [] () {
         *              DataExchange::ObjectVariantMapper mapper;
         *              mapper.Add<PointerType> (
         *                  [] (const ObjectVariantMapper& mapper, const PointerType* obj) -> VariantValue { return obj->As<String> (); },
         *                  [] (const ObjectVariantMapper& mapper, const VariantValue& d, PointerType* intoObj) -> void {
         *                      *intoObj = PointerType{d.As<String> ()};
         *                  });
         *              return mapper;
         *          }();
         *      \endcode
         *
         */
        nonvirtual void Add (const TypeMappingDetails& s);
        nonvirtual void Add (const Traversal::Iterable<TypeMappingDetails>& s);
        nonvirtual void Add (const TypesRegistry& s);
        nonvirtual void Add (const ObjectVariantMapper& s);
        template <typename T>
        nonvirtual void Add (const FromObjectMapperType<T>& fromObjectMapper, const ToObjectMapperType<T>& toObjectMapper);

    public:
        /**
         *  Alias for Add ()
         */
        nonvirtual void operator+= (const TypeMappingDetails& rhs);
        nonvirtual void operator+= (const Traversal::Iterable<TypeMappingDetails>& rhs);
        nonvirtual void operator+= (const TypesRegistry& rhs);
        nonvirtual void operator+= (const ObjectVariantMapper& rhs);

    public:
        /**
         *  Shortcut for Add (MakeCommonSerializer<T> ());
         *
         *  So - this is supported for any type for which (@see MakeCommonSerializer) is supported.
         *
         *  Note this this is not needed (because it's done by default), but is supported,
         *  for the builtin types.
         *
         *  @see MakeCommonSerializer for details, and restrictions.
         *
         *  \par Example Usage
         *      \code
         *          ObjectVariantMapper mapper;
         *          mapper.Add (ObjectVariantMapper::MakeCommonSerializer<optional<int>> ());   // long way
         *          mapper.AddCommonType<optional<int>> ();                                     // equivalent, but also checks/asserts underlying
         *                                                                                      // type has been entered into registry
         *      \endcode
         *
         */
        template <typename T, typename... ARGS>
        nonvirtual void AddCommonType (ARGS&&... args);

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
         *      o   std::filesystem::path
         *      o   Date
         *      o   DateTime
         *      o   Duration
         *      o   DurationSeconds
         *      o   Common::GUID
         *      o   InternetMediaType
         *      o   IO::Network::InternetAddress
         *      o   IO::Network::URI
         *      o   Mapping<String, String>
         *      o   Mapping<String, VariantValue>
         *      o   String
         *      o   TimeOfDay
         *      o   VariantValue
         *      o   Range<Duration>/Range<Date>/DiscreteRange<Date>/Range<DateTime>
         *      o   optional<EACH_OF_THE_ABOVE>
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
         */
        template <typename CLASS>
        struct ClassMapperOptions {
            /**
             *  \brief In ObjectVariantMapper::FromObject () -> VariantValue - decides if missing data mapped to null entry in map/object, or just missing
             */
            bool fOmitNullEntriesInFromObject{true};

            /**
             *  FromObjectMapperType (to variant value) called before the StructFieldInfo mappers in an AddClass () method. Typically null, but set in subclassing) 
             */
            FromObjectMapperType<CLASS> fBeforeFrom;

            /**
             *  ToObjectMapperType (to CLASS) called before the StructFieldInfo mappers in an AddClass () method. Typically null, but set in subclassing) 
             */
            ToObjectMapperType<CLASS> fBeforeTo;

            /**
             *  FromObjectMapperType (to variant value) after before the StructFieldInfo mappers in an AddClass () method. Typically null, but useful as a hook to refine mapping done by default algorithm 
             */
            function<void (const ObjectVariantMapper& mapper, const CLASS* objOfType, VariantValue* updateVariantValue)> fAfterFrom;

            /**
             *  ToObjectMapperType (to CLASS) after before the StructFieldInfo mappers in an AddClass () method. Typically null, but useful as a hook to refine mapping done by default algorithm 
             */
            ToObjectMapperType<CLASS> fAfterTo;
        };

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
         *          struct  MyConfig_ {
         *              IO::Network::URI        fURL1_;
         *              IO::Network::URI        fURL2_;
         *          };
         *
         *          ObjectVariantMapper mapper;
         *          mapper.AddCommonType<IO::Network::URI> ();      // add default type mapper (using default URL parse)
         *
         *          // register each of your mappable (even private) types
         *          mapper.AddClass<MyConfig_> ({
         *              { "fURL1_", &SharedContactsConfig_::fURL1_ },        // use default parser
         *              // for fURL2_ - instead - allow parsing of things like 'localhost:1234' - helpful for configuration files
         *              { "fURL2_", &SharedContactsConfig_::fURL2_, ObjectVariantMapper::MakeCommonSerializer<IO::Network::URI> ()  },
         *          });
         *
         *          MyConfig_   tmp;
         *          tmp.fURL2_ = IO::Network::URI{"http://localhost:1234"};
         *          VariantValue v = mapper.Serialize  (tmp);
         *
         *          Streams::MemoryStream<byte>   tmpStream;
         *          DataExchange::JSON::PrettyPrint (v, tmpStream);
         *
         *          // THEN deserialized, and mapped back to C++ object form
         *          tmp = mapper.ToObject<MyConfig_> (DataExchange::JSON::Reader{tmpStream});
         *      \endcode
         * 
         *  \note furtherDerivedClass operations to type performed AFTER the argument ones here, so that they can change values
         *        (either map to or from object) done in the base 'class' or set of properties being extended.
         */
        template <typename CLASS>
        nonvirtual void AddClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, const ClassMapperOptions<CLASS>& mapperOptions = {});

    public:
        /**
         *  \brief  Like @AddClass<> - adding a new class based on parameters - but based on the argument baseClass.
         *
         *  \par Example Usage
         *      \code
         *          struct BaseObj_ {
         *              int  fVV1{};
         *          };
         *          struct Derived_ : BaseObj_ {
         *              int  fVV2{};
         *          };
         *          ObjectVariantMapper mapper;
         *          mapper.AddClass<BaseObj_> ({
         *              {"fVV1", &BaseObj_, fVV1},
         *          });
         *          mapper.AddSubClass<Derived_, BaseObj_> ({
         *              {"fVV2", &Derived_::fVV2},
         *          });
         *      \endcode
         * 
         *  \note AddSubClass captures the existing mapping for BASE_CLASS at the time of this call, so this
         *        can be used to subclass in place, adding a few extra properties.
         *          \code
         *              #if qDebug
         *                  mapper.AddSubClass<Network, Network> ({
         *                      {"debugProps"sv, &Network::fDebugProps},
         *                  });
         *              #endif
         *          \endcode
         *
         *          // Another example - add 'virtual field' - readonly
         *          \code
         *              mapper.AddSubClass<Network, Network> (
         *              {},
         *              {.fAfterFrom = 
         *                  [] (const ObjectVariantMapper&, const Network* objOfType, VariantValue* updateResult) -> void {
         *                      Mapping<String, VariantValue> m = updateResult->As<Mapping<String, VariantValue>> ();
         *                      m.Add ("RandomValue"sv, VariantValue{objOfType->fEnabled ? 2 : 99});
         *                      *updateResult = VariantValue{m};
         *                  }
         *              });
         *          \endcode
         */
        template <typename CLASS, typename BASE_CLASS>
        nonvirtual void AddSubClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions,
                                     const ClassMapperOptions<CLASS>&            mapperOptions = {});

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
         * 
         *  \note due to https://stroika.atlassian.net/browse/STK-1015 - we require default_initializable<T> for overloads that construct the 'T'
         *        (but importantly NOT for overloads that ToObject into an address, so you can create a sucblass or your own instance with arguments and
         *        ToObject() into it)
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
         *  Same as ToObject, but (just about) any exceptions mapped to just returning nullopt
         * 
         *  \note - if copying T produces an exception, that will still be propagated.
         * 
         *  The purpose of this method is to make it easier when the input source data may or may not contain the fields needed
         *  to deserialize, and the caller wishes to map if they can, but simplify error handling (just ignore inability to read data)
         *  in the case where it cannot be mapped).
         */
        template <typename T>
        nonvirtual optional<T> ToObjectQuietly (const VariantValue& v) const;

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
         *      o   Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>
         *      o   Collection<T>
         *      o   Traversal::DiscreteRange<T, TRAITS>
         *      o   KeyedCollection<T, KEY_TYPE, TRAITS>
         *      o   Mapping<Key,Value>
         *      o   MultiSet<T>
         *      o   optional<T>
         *      o   Optional<T>
         *      o   Range<T,TRAITS>
         *      o   Sequence<T>
         *      o   Set<T>
         *      o   SortedCollection<T>
         *      o   SortedKeyedCollection<T, KEY_TYPE, TRAITS>
         *      o   SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>
         *      o   SortedMultiSet<T>
         *      o   SortedSet<T>
         *      o   Synchronized<T>
         *      o   vector<T>
         *      o   tuple<...ARGS>
         *      o   vector<T>
         *      o   enum types (with eSTART/eEND @see Stroika_Define_Enum_Bounds for bounds checking)
         *      o   T[N]
         *
         *  As well as
         *      o   all POD types (integer, floating point, bool)
         *      o   Time::Date
         *      o   Time::DateTime
         *      o   Characters::String
         *      o   VariantValue
         *      o   IO::Network::CIDR
         *      o   IO::Network::InternetAddress
         *      o   IO::Network::URI
         *      o   Time::TimeOfDay
         *      o   Common::GUID
         *
         *
         *  \note we USED to support type 'void' with the comment:
         *        o   void (yes - this is useful for templating)
         *        until Stroika v2.1d13
         *        But the trouble with this is that it matched any unknown type, and was just a no-op. Not crazy bad, but 
         *        could be CONFUSING for debugging - when you add a serializer for a type and it doesn't get serialized and you
         *        have no idea why. MAYBE it would be OK if the serializer logged "TYPE IGNORED" or something. Anyhow
         *        got rid of this feature until I can re-observe why its needed, and then maybe re-instate it more carefully.
         *
         *  This assumes the template parameters for the above objects are also already defined (mostly 'T' above).
         *
         *  This function also works (but is generally unneeded for) any of the types defined in
         *  @see ResetToDefaultTypeRegistry () (int, short, String, etc).
         *
         *  Note - all these de-serializers will throw BadDataFormat exceptions if the data somehow doesn't
         *  fit what the de-serializer expects.
         *
         *  \note   For type Mapping<KEY,VALUE>, this could use either the mapping function
         *          MakeCommonSerializer_MappingWithStringishKey or MakeCommonSerializer_MappingAsArrayOfKeyValuePairs.
         *          MakeCommonSerializer_MappingAsArrayOfKeyValuePairs is more general, but MakeCommonSerializer_MappingWithStringishKey
         *          is more commonly the desired output mapping, and so is the default.
         *
         *  \note   It is legal to call MakeCommonSerializer<> on a type where it only knows how to construct the base class type (struct derived : base {})
         *          in which case it produces a serializer that will still work with the given type T, but will only capture the data from base.
         *
         *          Note - the reason this works is because the object of type "T" is default-constructed outside the de-serializer, and then passed
         *          by address to the de-serializer.
         */
        template <typename T, typename... ARGS>
        static TypeMappingDetails MakeCommonSerializer (ARGS&&... args);

    public:
        /**
         *  Create a serializer for a CLASS 'T'
         *  @todo migrate this to be part of MakeCommonSerializer probably, but for now like AddClass, but less checking and doesn't add - just creates/returns
         * 
         *  Create a class serializer/de-serializer for the given field descriptions. Start with those from 'baseClass' - as if that was done first.
         *  and then if furtherDerivedClass provided, apply that type mapper as well last (so it gets final say on what's produced).
         */
        template <typename T>
        static TypeMappingDetails MakeClassSerializer (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions,
                                                       const ClassMapperOptions<T>&                options = {})
            requires (is_class_v<T>);

    public:
        template <typename T>
        [[deprecated]] static TypeMappingDetails MakeClassSerializer (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions,
                                                                      const optional<TypeMappingDetails>&         baseClass,
                                                                      const optional<TypeMappingDetails>& furtherDerivedClass = nullopt)
            requires (is_class_v<T>)
        {
            function<void (const ObjectVariantMapper& mapper, const T* objOfType, VariantValue* updateVariantValue)> afterFrom = nullptr;
            if (furtherDerivedClass and furtherDerivedClass->GetGenericFromObjectMapper()) {
                afterFrom = [=] (const ObjectVariantMapper& mapper, const T* objOfType, VariantValue* updateVariantValue) {
                    VariantValue vv = furtherDerivedClass->FromObjectMapper<T> () (mapper, objOfType);
                    *updateVariantValue = vv;
                };
            }
            return MakeClassSerializer<T> (
                fieldDescriptions,
                ClassMapperOptions<T>{
                    .fBeforeFrom = baseClass and baseClass->GetGenericFromObjectMapper() ? baseClass->FromObjectMapper<T> () : nullptr,
                    .fBeforeTo   = baseClass and baseClass->GetGenericToObjectMapper() ? baseClass->ToObjectMapper<T> () : nullptr,
                    .fAfterFrom  = afterFrom,
                    .fAfterTo = furtherDerivedClass and furtherDerivedClass->GetGenericToObjectMapper() ? furtherDerivedClass->ToObjectMapper<T> () : nullptr},
                nullptr);
        }

    private:
        template <typename T>
        static TypeMappingDetails MakeClassSerializer_ (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions,
                                                        const ClassMapperOptions<T>& options, const ObjectVariantMapper* mapperToCheckAgainst)
            requires (is_class_v<T>);

    public:
        /**
         *  Optional parameter to MakeCommonSerializer, and AddCommonType
         */
        struct RangeSerializerOptions {
            optional<String> fLowerBoundFieldName;
            optional<String> fUpperBoundFieldName;
        };

    public:
        /**
         *  Optional parameter to MakeCommonSerializer, and AddCommonType for object of type optional<T>, where you want to
         *  replace the underlying "T" serialization/deserialization as well.
         */
        struct OptionalSerializerOptions {
            optional<TypeMappingDetails> fTMapper;
        };

    public:
        /**
         */
        template <typename ENUM_TYPE>
        static TypeMappingDetails MakeCommonSerializer_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap);
        template <typename ENUM_TYPE>
        static TypeMappingDetails
        MakeCommonSerializer_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap = Configuration::DefaultNames<ENUM_TYPE>::k);

    public:
        /**
         */
        template <typename ENUM_TYPE>
        static TypeMappingDetails MakeCommonSerializer_EnumAsInt ();

    public:
        /**
         *  This works on Any Mapping<KEY,VALUE>, where the Key can be Mapped to / from a String, using
         *  an already defined type-mapper (from KEY_TYPE to/from String) or be of type String.
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
        template <Containers::Adapters::IAddableTo ACTUAL_CONTAINER_TYPE>
        static TypeMappingDetails MakeCommonSerializer_WithAdder ();

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        template <typename T>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (T*);
        template <typename DOMAIN_TYPE, typename RANGE_TYPE>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE>*);
        template <typename T>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (Containers::Collection<T>*);
        template <typename T, typename TRAITS>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const Traversal::DiscreteRange<T, TRAITS>*);
        template <typename T, typename KEY_TYPE, typename TRAITS>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (Containers::KeyedCollection<T, KEY_TYPE, TRAITS>*);
        template <typename KEY_TYPE, typename VALUE_TYPE>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const Mapping<KEY_TYPE, VALUE_TYPE>*);
        template <typename T>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (optional<T>*);
        template <typename T, typename TRAITS>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const Traversal::Range<T, TRAITS>*);
        template <typename T>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const Sequence<T>*);
        template <typename T>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const Set<T>*);
        template <typename T, typename TRAITS>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const Execution::Synchronized<T, TRAITS>*);
        template <typename T>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const vector<T>*);
        template <typename T1, typename T2>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const pair<T1, T2>*);
        template <typename T>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const Common::CountedValue<T>*);
        template <typename T1, typename T2>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const Common::KeyValuePair<T1, T2>*);
        template <typename T1>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const tuple<T1>*);
        template <typename T1, typename T2>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const tuple<T1, T2>*);
        template <typename T1, typename T2, typename T3>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const tuple<T1, T2, T3>*);
        template <typename T, size_t SZ>
        nonvirtual void AssertDependentTypesAlreadyInRegistry_ (const T (*)[SZ]);

    private:
        template <typename DOMAIN_TYPE, typename RANGE_TYPE>
        static TypeMappingDetails MakeCommonSerializer_ (const Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE>*);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const Containers::Collection<T>*);
        template <typename T, typename TRAITS>
        static TypeMappingDetails MakeCommonSerializer_ (const Traversal::DiscreteRange<T, TRAITS>*);
        template <typename KEY_TYPE, typename VALUE_TYPE>
        static TypeMappingDetails MakeCommonSerializer_ (const Mapping<KEY_TYPE, VALUE_TYPE>*);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const Containers::MultiSet<T>*);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const optional<T>*);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const optional<T>*, const OptionalSerializerOptions& options);
        template <typename T, typename KEY_TYPE, typename TRAITS>
        static TypeMappingDetails MakeCommonSerializer_ (const Containers::KeyedCollection<T, KEY_TYPE, TRAITS>*);
        template <typename T, typename TRAITS, typename... ARGS>
        static TypeMappingDetails MakeCommonSerializer_ (const Traversal::Range<T, TRAITS>*, ARGS&&... args);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const Sequence<T>*);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const Set<T>*);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const Containers::SortedCollection<T>*);
        template <typename T, typename KEY_TYPE, typename TRAITS>
        static TypeMappingDetails MakeCommonSerializer_ (const Containers::SortedKeyedCollection<T, KEY_TYPE, TRAITS>*);
        template <typename KEY_TYPE, typename VALUE_TYPE>
        static TypeMappingDetails MakeCommonSerializer_ (const Containers::SortedMapping<KEY_TYPE, VALUE_TYPE>*);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const Containers::SortedMultiSet<T>*);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const Containers::SortedSet<T>*);
        template <typename T, typename TRAITS>
        static TypeMappingDetails MakeCommonSerializer_ (const Execution::Synchronized<T, TRAITS>*);
        static TypeMappingDetails MakeCommonSerializer_ (const Memory::BLOB*);
        static TypeMappingDetails MakeCommonSerializer_ (const InternetMediaType*);
        static TypeMappingDetails MakeCommonSerializer_ (const IO::Network::CIDR*);
        static TypeMappingDetails MakeCommonSerializer_ (const IO::Network::InternetAddress*);
        static TypeMappingDetails MakeCommonSerializer_ (const IO::Network::URI*);
        static TypeMappingDetails MakeCommonSerializer_ (const Common::GUID*, VariantValue::Type representAs = VariantValue::Type::eString);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const vector<T>*);
        template <typename T1, typename T2>
        static TypeMappingDetails MakeCommonSerializer_ (const pair<T1, T2>*);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const Common::CountedValue<T>*);
        template <typename T1, typename T2>
        static TypeMappingDetails MakeCommonSerializer_ (const Common::KeyValuePair<T1, T2>*);
        template <typename T1>
        static TypeMappingDetails MakeCommonSerializer_ (const tuple<T1>*);
        template <typename T1, typename T2>
        static TypeMappingDetails MakeCommonSerializer_ (const tuple<T1, T2>*);
        template <typename T1, typename T2, typename T3>
        static TypeMappingDetails MakeCommonSerializer_ (const tuple<T1, T2, T3>*);
        template <typename T>
        static TypeMappingDetails MakeCommonSerializer_ (const T*)
            requires (is_enum_v<T>);
        template <typename T, size_t SZ>
        static TypeMappingDetails MakeCommonSerializer_ (const T (*)[SZ]);

    private:
        template <typename KEY_TYPE, typename VALUE_TYPE, typename ACTUAL_CONTAINER_TYPE>
        static TypeMappingDetails MakeCommonSerializer_WithKeyValuePairAdd_ ();

    private:
        template <typename RANGE_TYPE>
        static TypeMappingDetails MakeCommonSerializer_Range_ (const RangeSerializerOptions& options = {});

    private:
        template <typename CLASS>
        static TypeMappingDetails MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t sizeofObj,
                                                                        const Traversal::Iterable<StructFieldInfo>& fields,
                                                                        const ClassMapperOptions<CLASS>&            options);
        template <typename CLASS>
        static TypeMappingDetails MakeCommonSerializer_ForClassObject_and_check_ (const type_index& forTypeInfo, size_t sizeofObj,
                                                                                  const Traversal::Iterable<StructFieldInfo>& fields,
                                                                                  const ClassMapperOptions<CLASS>&            options,
                                                                                  const ObjectVariantMapper* use2Validate = nullptr);

    private:
        nonvirtual TypeMappingDetails Lookup_ (const type_index& forTypeInfo) const;

    public:
        /**
         */
        struct TypesRegistry {
        public:
            TypesRegistry (const Traversal::Iterable<TypeMappingDetails>& src);

        public:
            nonvirtual optional<TypeMappingDetails> Lookup (type_index t) const;

        public:
            /**
             *  \note Add () may update/replace an existing mapping, without prejudice.
             */
            nonvirtual void Add (const TypeMappingDetails& typeMapDetails);
            nonvirtual void Add (const Traversal::Iterable<TypeMappingDetails>& typeMapDetails);

        public:
            nonvirtual Traversal::Iterable<TypeMappingDetails> GetMappers () const;

        public:
            /**
             *  Alias for Add ()
             */
            nonvirtual void operator+= (const TypeMappingDetails& typeMapDetails);
            nonvirtual void operator+= (const Traversal::Iterable<TypeMappingDetails>& typeMapDetails);

        public:
            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;

        private:
            struct TypeMappingDetails_Extractor_ {
                auto operator() (const TypeMappingDetails& t) const -> type_index
                {
                    return t.GetForType ();
                };
            };
            using TypeMappingDetails_Traits_ = Containers::KeyedCollection_DefaultTraits<TypeMappingDetails, type_index, TypeMappingDetails_Extractor_>;
            KeyedCollection<TypeMappingDetails, type_index, TypeMappingDetails_Traits_> fSerializers_;
        };

    private:
        TypesRegistry fTypeMappingRegistry_;
    };

    // ToObject specializations - to handle lack of default constructors
    template <>
    Time::Date ObjectVariantMapper::ToObject (const ToObjectMapperType<Time::Date>& toObjectMapper, const VariantValue& v) const;
    template <>
    Time::DateTime ObjectVariantMapper::ToObject (const ToObjectMapperType<Time::DateTime>& toObjectMapper, const VariantValue& v) const;
    template <>
    Time::TimeOfDay ObjectVariantMapper::ToObject (const ToObjectMapperType<Time::TimeOfDay>& toObjectMapper, const VariantValue& v) const;

    /**
     *  This is just for use the with the ObjectVariantMapper::AddClass<> (and related) methods, to describe a
     *  user-defined type (CLASS).
     */
    struct ObjectVariantMapper::StructFieldInfo {
    public:
        /**
        * 
        * ***DEPRECATED SINCE STROIKA v3.0d8...
        * 
         *  \brief In ObjectVariantMapper::FromObject () -> VariantValue - decides if missing data mapped to null entry in map/object, or just missing
         */
        enum class NullFieldHandling {
            eOmit,
            eInclude,

            Stroika_Define_Enum_Bounds (eOmit, eInclude)
        };
        /**
        * ***DEPRECATED SINCE STROIKA v3.0d8...
         *  \brief In ObjectVariantMapper::FromObject () -> VariantValue - parent object created with missing values if missing from object
         */
        [[deprecated ("Since Stroika v3.0d8")]] static constexpr NullFieldHandling eOmitNullFields = NullFieldHandling::eOmit; // instead of using NullFieldHandling::eOmit

        /**
        * ***DEPRECATED SINCE STROIKA v3.0d8...
         *  \brief In ObjectVariantMapper::FromObject () -> VariantValue - parent object created with explicitly null values if missing from object
         */
        [[deprecated ("Since Stroika v3.0d8")]] static constexpr NullFieldHandling eIncludeNullFields =
            NullFieldHandling::eInclude; // instead of using NullFieldHandling::eInclude

    private:
        /*
         *  Required for a an actual field mapper, but if empty, implies a reader/writer for the entire object.
         * 
         *  \note Since v3.0d7 - this is required to be non-empty (or empty use deprecated rather)
         */
        String fSerializedFieldName_;

        /*
         * if missing - then pass in parent object, then fOverrideTypeMapper_ required ****THIS USE DEPRECATED*****
         *  \note Since v3.0d7 - this is required to be null-null (or null use deprecated rather)
         */
        optional<StructFieldMetaInfo> fFieldMetaInfo_;

        /*
         *  if fFieldMetaInfo_ == nullopt, fOverrideTypeMapper_ is required, and is the mapper used for the entire
         *  object. (NOTE SINCE 3.0d7 - fFieldMetaInfo_==nullopt deprecated); but this is still optional.
         */
        optional<TypeMappingDetails> fOverrideTypeMapper_;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          {"Int-1"sv, &SharedContactsConfig_::fInt1},
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          ObjectVariantMapper::StructFieldInfo{"Int-1"sv, StructFieldMetaInfo{&SharedContactsConfig_::fInt1}},
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          {"BasicArray1"sv, &SharedContactsConfig_::fBasicArray1, ObjectVariantMapper::MakeCommonSerializer<int[5]> ()},
         *      \endcode
         */
        StructFieldInfo (const String& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo);
        StructFieldInfo (const String& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo, const optional<TypeMappingDetails>& overrideTypeMapper);

    public:
        [[deprecated ("Since Stroika v3.0d8 - NullFieldHandling::eOmit is new default, use AddClass options arg "
                      "ClassMapperOptions<T>{.fOmitNullEntriesInFromObject = false} for eInclude behevior")]] StructFieldInfo (const String& serializedFieldName,
                                                                                                                               const StructFieldMetaInfo& fieldMetaInfo,
                                                                                                                               [[maybe_unused]] NullFieldHandling fromObjectNullHandling)
            : StructFieldInfo{serializedFieldName, fieldMetaInfo}
        {
            WeakAssert (fromObjectNullHandling == NullFieldHandling::eInclude); // the default changed for this case
        }
        [[deprecated ("Since Stroika v3.0d8 - NullFieldHandling::eOmit is new default, use AddClass options arg "
                      "ClassMapperOptions<T>{.fOmitNullEntriesInFromObject = false} for eInclude behevior")]] StructFieldInfo (const String& serializedFieldName,
                                                                                                                               const StructFieldMetaInfo& fieldMetaInfo,
                                                                                                                               const optional<TypeMappingDetails>& overrideTypeMapper,
                                                                                                                               [[maybe_unused]] NullFieldHandling fromObjectNullHandling)
            : StructFieldInfo{serializedFieldName, fieldMetaInfo, overrideTypeMapper}
        {
            WeakAssert (fromObjectNullHandling == NullFieldHandling::eInclude); // the default changed for this case
        }
        [[deprecated ("Since Stroika v3.0d7 - dont use StructFieldInfo with missing filedMetaInfo - instead use type override of owning "
                      "object)")]] StructFieldInfo (const String& serializedFieldName, TypeMappingDetails overrideTypeMapper,
                                                    [[maybe_unused]] NullFieldHandling fromObjectNullHandling = NullFieldHandling::eInclude)
            : fSerializedFieldName_{serializedFieldName}
            , fOverrideTypeMapper_{overrideTypeMapper}
        {
        }

    public:
        /**
         */
        nonvirtual String GetSerializedFieldName () const;

    public:
        /**
         */
        nonvirtual StructFieldMetaInfo GetStructFieldMetaInfo () const;

    public:
        /**
         */
        nonvirtual optional<TypeMappingDetails> GetOverrideTypeMapper () const;

    public:
        friend class ObjectVariantMapper;
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
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<filesystem::path> ();
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
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::DurationSeconds> ();
    template <>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Time::TimeOfDay> ();
    template <>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Mapping<Characters::String, Characters::String>> ();
    template <>
    ObjectVariantMapper::TypeMappingDetails ObjectVariantMapper::MakeCommonSerializer<Mapping<Characters::String, VariantValue>> ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectVariantMapper.inl"

#endif /*_Stroika_Foundation_DataExchange_ObjectVariantMapper_h_*/
