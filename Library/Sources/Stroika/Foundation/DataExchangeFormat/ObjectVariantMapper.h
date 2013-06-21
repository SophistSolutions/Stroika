/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_h_
#define _Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_h_    1

#include    "../StroikaPreComp.h"

#include    <set>           // just temporarily til I finish traits / compare customiztion support
#include    <type_traits>
#include    <typeindex>

#include    "../Characters/String.h"
#include    "../Containers/Mapping.h"
#include    "../Containers/Sequence.h"
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
 *      @todo   So support for array - like we have for struct - using Sequence<> as the C++ type (maybe others too)
 *              and then auto-map that. Maybe also support vector<T> as a builtin / registered type as well - and others?
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
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            using   Characters::String;
            using   Containers::Mapping;
            using   Containers::Sequence;
            using   Memory::Byte;
            using   Memory::VariantValue;


            /**
             * CLASS IS COPYABLE. Make one instance, register your types into it and use this to
             *  serialized/deserialize
             *
             *
             * EXAMPLE USAGE:
             *      struct SharedContactsConfig_
             *      {
             *          bool    fEnabled;
             *
             *          SharedContactsConfig_ ()
             *              : fEnabled (false)
             *          {
             *          }
             *      };
             *...
             *
             *  ObjectVariantMapper mapper;
             *
             *  // register each of your proivate types
             *  mapper.RegisterClass<SharedContactsConfig_> (Sequence<TYPEINFO> ({ TYPEINFO (offsetof (SharedContactsConfig_, fEnabled), typeid (bool), L"Enabled"), }));
             *
             *  SharedContactsConfig_   tmp;
             *  tmp.fEnabled = enabled;
             *  VariantValue v = mapper.Serialize  (tmp);
             *  // at this point - we should have VariantValue object with "Enabled" field.
             *  // This can then be serialized using
             *  JSON::Serialize....
             *
             *  // THEN deserialized
             *
             *  Then mapped back to C++ object form
             *  tmp = mapper.Deserialize (v);
             *
             *
             */
            class   ObjectVariantMapper {
            public:
                /**
                 *  Defaults to installing basic serializers.
                 */
                ObjectVariantMapper ();

            public:
                struct SerializerInfo;

            public:
                void    ClearRegistry ();
            public:
                void    ResetToDefaultRegistry ();

            public:
                void    RegisterSerializer (const SerializerInfo& serializerInfo);

            public:
                void    RegisterCommonSerializers ();


            public:
                struct TYPEINFO;

            public:
                /**
                * @todo - add sizeof class - for assert checking... just  in CTOR arg - no need in serializer itself
                 */
                SerializerInfo  mkSerializerForStruct (const type_index& forTypeInfo, Sequence<TYPEINFO> fields);

            public:
                /**
                 */
                template    <typename CLASS>
                void    RegisterClass (Sequence<TYPEINFO> typeInfo);

            public:
                /**
                 */
                void    Deserialize (const type_index& forTypeInfo, const VariantValue& d, Byte* into);
                template    <typename CLASS>
                void    Deserialize (const Memory::VariantValue& v, CLASS* into);
                template    <typename CLASS>
                CLASS    Deserialize (const Memory::VariantValue& v);

            public:
                /**
                 */
                VariantValue    Serialize (const type_index& forTypeInfo, const Byte* objOfType);
                template    <typename CLASS>
                VariantValue    Serialize (const CLASS& from);


            private:
                SerializerInfo  Lookup_(const type_index& forTypeInfo) const;

            private:
                set<SerializerInfo> fSerializers_;  // need Stroika set with separate traits-based key extractor/compare function
            };


            /**
             * RENAME - not calling serializaiton anynore???
             */
            struct ObjectVariantMapper::SerializerInfo {
                type_index                                              fForType;
                std::function<VariantValue(const Byte* objOfType)>      fSerializer;
                std::function<void(const VariantValue& d, Byte* into)>  fDeserializer;

                SerializerInfo (const type_index& forTypeInfo, const std::function<VariantValue(const Byte* objOfType)>& serializer, const std::function<void(const VariantValue& d, Byte* into)>& deserializer);

                //tmphack - so I can use set<> - but later fix so this is gone - and use Stroika Set<> code with smarter field extraction etc..
                bool operator< (const SerializerInfo& rhs) const {
                    return (fForType < rhs.fForType);
                }
            };


            /**
             * RENAME - JUST FOR ELTS OF A STRUCT
             */
            struct ObjectVariantMapper::TYPEINFO {
                size_t      fOffset;
                type_index  fTypeInfo;
                String      fSerializedFieldName;

                TYPEINFO(size_t fieldOffset = 0, type_index typeInfo = typeid(void), const String& serializedFieldName = String ());
            };



            /**
             *  Greatly regred adding Macro, but it just seems SO HELPFUL (makes things much more terse). No need to use - but some may
             *  find it helpfull...
             */
#define     ObjectVariantMapper_TYPEINFO_Construction_Helper(CLASS,MEMBER,NAME)\
    DataExchangeFormat::ObjectVariantMapper::TYPEINFO (offsetof (CLASS, MEMBER), typeid (CLASS::MEMBER), NAME)


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
