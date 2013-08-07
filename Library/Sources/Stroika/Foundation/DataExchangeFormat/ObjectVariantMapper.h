/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_h_
#define _Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_h_    1

#include    "../StroikaPreComp.h"

#include    <type_traits>
#include    <typeindex>

#include    "../Characters/String.h"
#include    "../Containers/Mapping.h"
#include    "../Containers/Sequence.h"
#include    "../Containers/Set.h"
#include    "../Memory/Common.h"
#include    "../Memory/VariantValue.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *          (USED IN HEALTHFRAME - BUT JUST A PROTOTYPE...
 *          PROBBALY MOVE THIS OUT OF FOUNDATION AND INTO A FRAMEWORK???
 *
 *  TODO:
 *
 *      @todo   Just a draft - think through what we really want here...
 *
 *      @todo   Add support for array to ResetToDefaultTypeRegistry - like we have for struct - using Sequence<>
 *              as the C++ type (maybe others too, vector, more?).
 *
 *              But unclear how todo ArrayOfWhat?? Maybe the CTOR takes to typeids - not sure how well that works?
 *
 *      @todo   EFFICIENCY NOTES AND TODO MAYBE IMPROVE?
 *
 *              This can be moderately efficient, but it is not highly efficient. The use of std::function
 *              for the serializer/desearizers adds costs.
 *
 *              When serializing / deserializing - (e.g to/from JSON or XML) - we construct DOM tree which is
 *              intrinsically not very cost effective. We DO have the XML sax parser (but that wont work with this).
 *
 *      @todo   NOTE and TODO
 *              The cast to Byte* loses some type safety (we may want to store the class size htrough template magic)
 *              in the struct type info record, so it can be validated against the offsets in the typeinfo.
 *
 *      @todo   [Long-Term] [Performance] The gist of this design / API is to map C++ objects from/to Memory::VariantValue
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
        namespace   DataExchangeFormat {


            using   Characters::String;
            using   Containers::Mapping;
            using   Containers::Sequence;
            using   Containers::Set;
            using   Memory::Byte;
            using   Memory::VariantValue;


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
             *          bool                    fEnabled;
             *          DateTime                fLastSynchronizedAt;
             *          Mapping<String,String>  fThisPHRsIDToSharedContactID;
             *
             *          SharedContactsConfig_ ()
             *              : fEnabled (false)
             *              , fLastSynchronizedAt ()
             *              , fThisPHRsIDToSharedContactID ()
             *          {
             *          }
             *      };
             *...
             *
             *  ObjectVariantMapper mapper;
             *
             *  // register each of your mappable (even private) types
             *  mapper.RegisterClass<SharedContactsConfig_> (Sequence<StructureFieldInfo> ({
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
             *  DataExchangeFormat::JSON::PrettyPrint (v, tmpStream);
             *
             *  // THEN deserialized, and mapped back to C++ object form
             *  tmp = mapper.ToObject<SharedContactsConfig_> (DataExchangeFormat::JSON::Reader   (tmpStream));
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
                 *  This clears the registry of type mappers, and resets it to the defaults - a set of builtin types,
                 *  like String, int, etc.
                 *
                 *  Builtin types include:
                 *      o   bool
                 *      o   int
                 *      o   float
                 *      o   double
                 *      o   Date
                 *      o   DateTime
                 *      o   String
                 *      o   Mapping<String, String>
                 *      o   Mapping<String, VariantValue>
                 *
                 *  @todo - IT SOON WILL CONTAIN Sequence<String>, and Sequence<Variant>, as well as vector<> of those types.
                 *
                 *  Note - to include any of your user-defined types (structs) - you must use RegisterTypeMapper() or
                 *  RegisterClass ().
                 *
                 *  @see GetTypeMappingRegistry
                 *  @see SetTypeMappingRegistry
                 *  @see RegisterClass
                 *  @see RegisterTypeMapper
                 */
                nonvirtual  void    ResetToDefaultTypeRegistry ();

            public:
                /**
                 */
                nonvirtual  void    RegisterTypeMapper (const TypeMappingDetails& serializerInfo);

            public:
                struct  StructureFieldInfo;

            public:
                /**
                 */
                template    <typename CLASS>
                nonvirtual  void    RegisterClass (const Sequence<StructureFieldInfo>& fieldDescriptions);

            public:
                /**
                 */
                nonvirtual  void    ToObject (const type_index& forTypeInfo, const VariantValue& d, Byte* into);
                template    <typename CLASS>
                nonvirtual  void    ToObject (const Memory::VariantValue& v, CLASS* into);
                template    <typename CLASS>
                nonvirtual  CLASS   ToObject (const Memory::VariantValue& v);

            public:
                /**
                 */
                nonvirtual  VariantValue    FromObject (const type_index& forTypeInfo, const Byte* objOfType);
                template    <typename CLASS>
                nonvirtual  VariantValue    FromObject (const CLASS& from);

            private:
                nonvirtual  TypeMappingDetails  Lookup_(const type_index& forTypeInfo) const;

            private:
                Set<TypeMappingDetails> fSerializers_;  // need Stroika set with separate traits-based key extractor/compare function
            };


            /**
             */
            struct  ObjectVariantMapper::TypeMappingDetails {
                type_index                                                                          fForType;
                std::function<VariantValue(ObjectVariantMapper* mapper, const Byte* objOfType)>     fToVariantMapper;
                std::function<void(ObjectVariantMapper* mapper, const VariantValue& d, Byte* into)> fFromVariantMapper;

                TypeMappingDetails (
                    const type_index& forTypeInfo,
                    const std::function<VariantValue(ObjectVariantMapper* mapper, const Byte* objOfType)>& toVariantMapper,
                    const std::function<void(ObjectVariantMapper* mapper, const VariantValue& d, Byte* into)>& fromVariantMapper
                );
                TypeMappingDetails (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields);


                // @todo
                //tmphack - so I can use Stroika Set<> code with smarter field extraction etc.., and fix to default CTOR issue
                // --LGP 2013-06-23
                TypeMappingDetails ()
                    : fForType (typeid(void))
                    , fToVariantMapper (nullptr)
                    , fFromVariantMapper (nullptr) {
                }
                bool operator== (const TypeMappingDetails& rhs) const {
                    return (fForType == rhs.fForType);
                }
                bool operator< (const TypeMappingDetails& rhs) const {
                    return (fForType < rhs.fForType);
                }
            };


            /**
             */
            struct  ObjectVariantMapper::StructureFieldInfo {
                size_t      fOffset;
                type_index  fTypeInfo;
                String      fSerializedFieldName;

                StructureFieldInfo (size_t fieldOffset = 0, type_index typeInfo = typeid(void), const String& serializedFieldName = String ());
            };


            /**
             *  Greatly regret adding Macro, but it just seems SO HELPFUL (makes things much more terse). No need to use - but some may
             *  find it helpfull...
             *
             *  I don't know of any way in C++ without macro - to capture a member name (for use in decltype thing and offsetof()).
             */
#define     ObjectVariantMapper_StructureFieldInfo_Construction_Helper(CLASS,MEMBER,NAME)\
    DataExchangeFormat::ObjectVariantMapper::StructureFieldInfo (offsetof (CLASS, MEMBER), typeid (decltype (CLASS::MEMBER)), NAME)


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ObjectVariantMapper.inl"

#endif  /*_Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_h_*/
