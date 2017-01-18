﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_    1

#include    "../../StroikaPreComp.h"

#include    <type_traits>
#include    <typeindex>

#include    "../../Characters/String.h"
#include    "../../Characters/StringBuilder.h"
#include    "../../Configuration/Enumeration.h"
#include    "../../Containers/Adapters/Adder.h"
#include    "../../Containers/Bijection.h"
#include    "../../Containers/Collection.h"
#include    "../../Containers/Mapping.h"
#include    "../../Containers/Sequence.h"
#include    "../../Containers/Set.h"
#include    "../../Containers/SortedCollection.h"
#include    "../../Containers/SortedMapping.h"
#include    "../../Containers/SortedSet.h"
#include    "../../Configuration/TypeHints.h"
#include    "../../Execution/Synchronized.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/Optional.h"
#include    "../../Streams/InputStream.h"
#include    "../../Time/DateTime.h"
#include    "../../Time/Duration.h"
#include    "../../Traversal/Range.h"

#include    "../StructFieldMetaInfo.h"

#include    "IConsumer.h"
#include    "Name.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:



 >>> cleanup regtest 30 (obejctreader) - with more using namespace ObjectReader to make more terse
 and update doc header examples similarly




 ----
        >>>> DO THIS FOR REASE v2.0a190
 struct  ObjectReaderRegistry:: RepeatedElementReader_DefaultTraits {
 // @todo
 // @todo - LOSE all reference to explicit traits use, and then LOSE ContainerType, ElementType, and MakeActualReader and
 // just retrain ContainerAdapterAdder.
 // MAYBE - OUTRIGHT lose Traits.. (cuz you dont need to use repeatedeltementreader if you dont want) - that maybe simpelst.


 -----

 Extend ObjectReaderRegistry::StructIFNO – with function (like in mixin class) to read stuff –
 merge it with mixinhelper – and then use that in all places – class reader, mixin reader, and RepeatedEltReader.

 ---

     MixinEltTraits could have overload of Name instead of lambda returning bool, one for case = Name, and either one for case where != Name, or maybe ANY wlidvard with interuendeirng we do first one first.
     Maybe have MixinEltTraits param for pick subelt more like macro to offset of – soemthign where you pass in just T, eltName and it goes the reinterpret_cast for you.

     Redo most of those readers using args passed to reader – instead of TRAITS – TRAITS just confusing to use.


 --

    redo regtest T8_SAXObjectReader_BLKQCL_ReadAlarms_ - to also use ListOfObjectsReader_NEW


 ----

 FIX DOCS:
 OptionalTypesReader_ supports reads of optional types. This will work - for any types for
 *  which SimpleReader<T> is implemented.
 *
 *  Note - this ALWAYS produces a result. Its only called when the element in quesiton has
 *  already occurred. The reaosn for Optional<> pa

 Works for any type where the registry already has a reader for t.

 ---

 FIXUP DOCS for “mapper.AddCommonType<Sequence<Person_>> (Name (L"WithWhom"));”
 Around like 210 – should include xml example to make sense – maybe same xml as down around line 610


 ----
 Includein headers example for using objectreaderresgiter

 #if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
 public:
 bool    fTraceThisReader { false };       // very noisy - off by default even for tracemode
 nonvirtual  String TraceLeader_ () const;
 #endif



> not sure we need MakeClassReader  - can just use ClassReader<T> {}.AsFactory ()





 *      @todo   Make AddCommonType() - when passed in an optional<T> - REquire that
 *              the type T is already in the registry (like with AddClass). To debug!
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-408 - cleanup template specializations
 *
 *      @todo   Review names: I dont think we use the term reader and readerfactory totally uniformly, and
 *              we also need to clearly document why/when we use one versus the other (type system stores factories
 *              because you encounter them in parsing, and need instances to pop on stack to read a particular element).
 *
 *      @todo   Need much improved ObjectReaderRegistry docs
 *
 *      @todo   make more nested READER classes PRIVATE, and improve public ADDCOMMON<T> methods to registry - like we do
 *              For ObjectVariantMapper.
 *
 *      @todo   Get rid of or do differently the Run() methods - so can turn on/off Context debugging easier.
 *
 *      @todo   USE UnknownSubElementDisposition more throughly...
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {
                namespace   ObjectReader {



                    using   Characters::String;
                    using   Containers::Mapping;
                    using   Containers::Sequence;
                    using   Containers::Set;
                    using   Memory::Byte;


                    /**
                     */
#ifndef qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
#define qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing  qDebug
#endif


                    /**
                     */
                    enum    class  UnknownSubElementDisposition {
                        eIgnore,
                        eEndObject
                    };


                    class   Context;
                    class   IElementConsumer;

                    // @todo - LOSE - forward needed only while we have deprecated mkReadDownToReader....
                    class   ReadDownToReader;


                    /**
                     */
                    template    <typename T>
                    using   ReaderFromTStarFactory = function<shared_ptr<IElementConsumer> (T* destinationObject)>;


                    /**
                    *  We store in our database factories that read into a 'void*' that must be of the right type,
                    *  but we use (at the last minute) the appropriate type. This is typesafe iff the readers do casts
                    *  safely (and all the readers we provide do).
                    */
                    using   ReaderFromVoidStarFactory = ReaderFromTStarFactory<void>;


                    /**
                    *  This is just for use the with the ObjectReaderRegistry::AddClass<> (and related) methods, to describe a user-defined type (CLASS).
                    */
                    struct  StructFieldInfo {
                        Name                                                            fSerializedFieldName;
                        StructFieldMetaInfo                                             fFieldMetaInfo;
                        Memory::Optional_Indirect_Storage<ReaderFromVoidStarFactory>    fOverrideTypeMapper;

                        /**
                         */
                        StructFieldInfo (const Name& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo, const Memory::Optional<ReaderFromVoidStarFactory>& typeMapper = {});
                    };


                    /**
                     *      The basic idea of the ObjectReaderRegistry is to make it easier to write C++ code
                     *  to deserialize an XML source (via SAX), into a C++ data structure. This tends to be
                     *  MUCH MUCH harder than doing something similar by loading an XML DOM, and then traversing
                     *  the DOM with XPath. So why would you do it? This way is dramatically more efficient.
                     *  For one thing - there is no need to have the entire source in memory at a time, and there
                     *  is no need to ever construct intermediary DOM nodes.
                     *
                     *      We need good docs - on how to use this - but for the time being, just look at the
                     *  example usage in the regression test.
                     *
                     *    Look back to DataExchange::ObjectVariantmapper, but for now - KISS
                     *
                     *  \par Example Usage
                     *      \code
                     *      struct  Person_ {
                     *          String      firstName;
                     *          String      lastName;
                     *      };
                     *      ObjectReaderRegistry mapper;
                     *      mapper.AddCommonType<String> ();
                     *      mapper.AddClass<Person_> (initializer_list<ObjectReaderRegistry::StructFieldInfo> {
                     *          { Name { L"FirstName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, firstName) },
                     *          { Name { L"LastName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, lastName) },
                     *      });
                     *      Person_ p;
                     *      ObjectReaderRegistry::IConsumerDelegateToContext tmpCtx1 (mapper, make_shared<ObjectReaderRegistry::ReadDownToReader> (mapper.MakeContextReader (&p)));
                     *      XML::SAXParse (mkdata_ (), tmpCtx1);
                     *      \endcode
                     *
                     *  \par Example Usage2
                     *      @see T3_SAXObjectReader_ReadDown2Sample_ in RegressionTest 'Foundation::DataExchangeFormat::XML::SaxParser'
                     *
                     *      \code
                     *      .. start with the types and mapper from Example 1, and add
                     *      Sequence<Person_> people;
                     *      mapper.AddCommonType<Sequence<Person_>> (Name (L"WithWhom"));
                     *      ObjectReaderRegistry::IConsumerDelegateToContext tmpCtx2 { mapper, make_shared<ObjectReaderRegistry::ReadDownToReader> (newRegistry.MakeContextReader (&people)) };
                     *      XML::SAXParse (mkdata_ (), tmp);
                     *      \endcode
                     */
                    class   Registry {
                    public:
                        Registry () = default;
                        Registry (const Registry&) = default;

                    public:
                        Registry& operator= (const Registry&) = default;

                    public:
                        /**
                         */
                        nonvirtual  void    Add (type_index forType, const ReaderFromVoidStarFactory& readerFactory);
                        template    <typename T>
                        nonvirtual  void    Add (const ReaderFromTStarFactory<T>& readerFactory);

                    public:
                        /**
                         *  Shortcut for Add (MakeCommonReader<T> ());
                         *
                         *  So - this is supported for any type for which (@see MakeCommonReader) is supported.
                         *
                         *  Note this this is not needed (because it's done by default), but is supported,
                         *  for the builtin types.
                         */
                        template    <typename T, typename... ARGS>
                        nonvirtual  void    AddCommonType (ARGS&& ... args);

                    public:
                        nonvirtual  Memory::Optional<ReaderFromVoidStarFactory>    Lookup (type_index t) const;

                    public:
                        /**
                         *  @see ReadDownToReader
                         *
                         *  @todo use perferct forwarding and/or something else to handle the overloading.list case.
                         *          So 2/Name acase - context is OUTER context, and tagToHandoff is closest context - so like using a/b in XPath.
                         */
                        _Deprecated_ ("USE make_shared<ObjectReader::ReadDownToReader> CTOR- deprecated v2.0a189")
                        static  shared_ptr<ReadDownToReader>    mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader);
                        _Deprecated_ ("USE make_shared<ObjectReader::ReadDownToReader> CTOR- deprecated v2.0a189")
                        static  shared_ptr<ReadDownToReader>    mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff);
                        _Deprecated_ ("USE make_shared<ObjectReader::ReadDownToReader> CTOR- deprecated v2.0a189")
                        static  shared_ptr<ReadDownToReader>    mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& contextTag, const Name& tagToHandOff);

                    public:
                        /**
                         *  This creates serializers for many common types.
                         *      o   String
                         *      o   Time::DateTime
                         *      o   Time::Duration
                         *      o   Mapping<Key,Value>
                         *      o   Optional<T>
                         *      o   Sequence<T>
                         *      o   vector<T>
                         *      o   enum types (with eSTART/eEND @see Stroika_Define_Enum_Bounds for bounds checking)
                         *
                         *  This assumes the template parameters for the above objects are also already defined (mostly 'T' above).
                         *
                         *  This function also works (but is generally unneeded for) any of the types defined in
                         *  @see ResetToDefaultTypeRegistry () (int, short, String, etc).
                         *
                         *  Note - all these de-serializers will throw BadDataFormat exceptions if the data somehow doesnt
                         *  fit what the deserailizer expects.
                         *
                         *  @see MakeClassReader
                         *  @see MakeCommonReader_EnumAsInt
                         *  @see MakeCommonReader_NamedEnumerations
                         */
                        template    <typename T, typename... ARGS>
                        static  ReaderFromVoidStarFactory  MakeCommonReader (ARGS&& ... args);

                    public:
                        /**
                         *  \req    AddClass<> requires that each field data type already be pre-loaded into the
                         *          ObjectReaderRegistry. To avoid this requirement, you an use MakeClassReader
                         *          directly, but if this type is absent when you call AddClass<> - its most likely
                         *          a bug.
                         */
                        template    <typename CLASS>
                        nonvirtual  void    AddClass (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions);
                        template    <typename CLASS>
                        _Deprecated_ ("USE AddClass(initializer_list<StructFieldInfo>)- deprecated v2.0a189")
                        nonvirtual  void    AddClass (const initializer_list<pair<Name, StructFieldMetaInfo>>& fieldDescriptions);

                    public:
                        /**
                         *  Create a class factory for a class with the given fields. This does't check (because its static) if the types of the elements in the
                         *  fields are part of the mapper.
                         *
                         *  @see AddClass<>
                         */
                        template    <typename CLASS>
                        static  ReaderFromVoidStarFactory    MakeClassReader (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions);
                        template    <typename CLASS>
                        _Deprecated_ ("USE MakeClassReader(initializer_list<StructFieldInfo>)- deprecated v2.0a189")
                        static  ReaderFromVoidStarFactory    MakeClassReader (const initializer_list<pair<Name, StructFieldMetaInfo>>& fieldDescriptions);

                    public:
                        /**
                         *  @see MakeCommonReader
                         *  @see MakeCommonReader_EnumAsInt
                         */
                        template    <typename ENUM_TYPE>
                        static  ReaderFromVoidStarFactory  MakeCommonReader_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap);
                        template    <typename ENUM_TYPE>
                        static  ReaderFromVoidStarFactory  MakeCommonReader_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap = Configuration::DefaultNames<ENUM_TYPE>::k);

                    public:
                        /**
                         *  @see MakeCommonReader
                         *  @see MakeCommonReader_NamedEnumerations
                         */
                        template    <typename ENUM_TYPE>
                        static  ReaderFromVoidStarFactory  MakeCommonReader_EnumAsInt ();

                    public:
                        /**
                         *  It's pretty easy to code up a 'reader' class - just a simple subclass of IElementConsumer.
                         *  There are lots of examples in this module.
                         *
                         *  But the APIs in the ObjectReaderRegistry require factories, and this utility function constructs
                         *  a factory from the given READER class template parameter.
                         */
                        template    <typename T, typename READER, typename... ARGS>
                        static  auto    ConvertReaderToFactory (ARGS&& ... args) -> ReaderFromVoidStarFactory;

                    public:
                        /**
                         *  This is generally just called inside of another composite reader to read sub-objects.
                         */
                        nonvirtual  shared_ptr<IElementConsumer>    MakeContextReader (type_index ti, void* destinationObject) const;
                        template    <typename T>
                        nonvirtual  shared_ptr<IElementConsumer>    MakeContextReader (T* destinationObject) const;

                    private:
                        template    <typename   T>
                        class   SimpleReader_;
                        template    <typename   T>
                        class   OptionalTypesReader_;

                    private:
                        template    <typename T>
                        static  ReaderFromVoidStarFactory   cvtFactory_ (const ReaderFromTStarFactory<T>& tf);
                    private:
                        template    <typename T>
                        static  ReaderFromVoidStarFactory   MakeCommonReader_SimpleReader_ ();
                    private:
                        static  ReaderFromVoidStarFactory   MakeCommonReader_ (const String*);
                        static  ReaderFromVoidStarFactory   MakeCommonReader_ (const Time::DateTime*);
                        static  ReaderFromVoidStarFactory   MakeCommonReader_ (const Time::Duration*);
                        template    <typename T>
                        static  ReaderFromVoidStarFactory   MakeCommonReader_ (const T*, typename std::enable_if<std::is_enum<T>::value >::type* = 0);
                        template    <typename T>
                        static  ReaderFromVoidStarFactory   MakeCommonReader_ (const T*, typename std::enable_if < std::is_pod<T>::value and !std::is_enum<T>::value >::type* = 0);
                        template    <typename T, typename TRAITS>
                        static  ReaderFromVoidStarFactory   MakeCommonReader_ (const Memory::Optional<T, TRAITS>*);
                        template    <typename T>
                        static  ReaderFromVoidStarFactory   MakeCommonReader_ (const vector<T>*);
                        template    <typename T>
                        static  ReaderFromVoidStarFactory   MakeCommonReader_ (const vector<T>*, const Name& name);
                        template    <typename T>
                        static  ReaderFromVoidStarFactory   MakeCommonReader_ (const Sequence<T>*);
                        template    <typename T>
                        static  ReaderFromVoidStarFactory   MakeCommonReader_ (const Sequence<T>*, const Name& name);

                    private:
                        Mapping<type_index, ReaderFromVoidStarFactory> fFactories_;
                    };


                    /**
                     *  Subclasses of this abstract class are responsible for consuming data at a given level
                     *  of the SAX 'tree', and transform it into a related object.
                     *
                     *  These are generally 'readers' of input XML, and writers of inflated objects.
                     */
                    class   IElementConsumer : public enable_shared_from_this<IElementConsumer> {
                    protected:
                        /**
                         */
                        IElementConsumer () = default;

                    public:
                        /**
                         */
                        virtual ~IElementConsumer () = default;

                    public:
                        /**
                         *  Pushed onto context stack.
                         *
                         *  Once activated, an element cannot be Activated () again until its been deactivated.
                         *  All elements that have been Activated () must be de-activated to disgorge/flush their work.
                         *
                         * @see Deactivating
                         */
                        virtual void    Activated (Context& r);

                    public:
                        /**
                         */
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name);

                    public:
                        /**
                         */
                        virtual void    HandleTextInside (const String& text);

                    public:
                        /**
                         * About to pop from the stack.
                         *
                         * @see Activated
                         */
                        virtual void    Deactivating ();

                    public:
                        /**
                         *  Helper to convert a reader to a factory (something that creates the reader).
                         *
                         *  Equivilent to
                         *      ObjectReaderRegistry::ConvertReaderToFactory<TARGET_TYPE, READER> ()
                         *
                         *  Typically shadowed in subclasses (actual readers) to fill in default parameters.
                         */
                        template    <typename TARGET_TYPE, typename READER, typename... ARGS>
                        static  ReaderFromVoidStarFactory   AsFactory (ARGS&& ... args);
                    };


                    /**
                     *  This concrete class is used to capture the state of an ongoing StructuredStreamParse/transformation. Logically, it
                     *  mainstains the 'stack' you would have in constructing a recursive decent object mapping.
                     *
                     *  \note   Important Design Principle for Reader
                     *
                     *          We only recognize a new 'type' by its 'Start' element. So its at that (or sometimes the start/end of a child) that
                     *          we can PUSH a new reader in place.
                     *
                     *          The current tags and text get delivered to the top of the stack.
                     *
                     *          When we encounter a close tag, we end that reading, and so pop.
                     *
                     *          This means that the start and end tags for a given pair, go to differnt 'IElementConsumer'
                     *          subclasses. The START goes to the parent (so it can create the right type), and the EndTag
                     *          goes to the created/pushed type, so it can close itself and pop back to the parent context.
                     *
                     *  \note   Debugging Note
                     *          It's often helpful to turn on fTraceThisReader while debugging SAX parsing, to see why something
                     *          isn't being read.
                     */
                    class   Context {
                    public:
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
                    public:
                        bool    fTraceThisReader { false };       // very noisy - off by default even for tracemode
                        nonvirtual  String TraceLeader_ () const;
#endif

                    public:
                        Context (const Registry& registry);
                        Context (const Registry& registry, const shared_ptr<IElementConsumer>& initialTop);
                        Context (const Context&) = delete;
                        Context (Context&& from);
                        Context& operator= (const Context&) = delete;

                    public:
                        nonvirtual  const   Registry&   GetObjectReaderRegistry () const;

                    public:
                        nonvirtual  void    Push (const shared_ptr<IElementConsumer>& elt);

                    public:
                        nonvirtual  void    Pop ();

                    public:
                        nonvirtual  shared_ptr<IElementConsumer>   GetTop () const;

                    public:
                        nonvirtual  bool    empty () const;

                    private:
                        const Registry&                         fObjectReaderRegistry_;
                        vector<shared_ptr<IElementConsumer>>    fStack_;

                    private:
                        friend  class   ObjectReader;
                    };


                    /**
                     *  @see Registry for examples of use
                     */
                    class   IConsumerDelegateToContext : public StructuredStreamEvents::IConsumer {
                    public:
                        IConsumerDelegateToContext () = delete;
                        IConsumerDelegateToContext (Context&& r);
                        IConsumerDelegateToContext (const Registry& registry);
                        IConsumerDelegateToContext (const Registry& registry, const shared_ptr<IElementConsumer>& initialTop);
                        IConsumerDelegateToContext (const IConsumerDelegateToContext& r) = default;
                        IConsumerDelegateToContext& operator= (const IConsumerDelegateToContext&) = delete;

                    public:
                        virtual void    StartElement (const Name& name) override;
                        virtual void    EndElement (const Name& name) override;
                        virtual void    TextInsideElement (const String& text) override;

                    public:
                        Context     fContext;
                    };


                    /**
                     *  Push one of these Nodes onto the stack to handle 'reading' a node which is not to be read.
                     *  This is necessary to balance out the Start Tag / End Tag combinations.
                     */
                    class   IgnoreNodeReader : public IElementConsumer {
                    public:
                        /**
                         *  Note the IGNORED* overload is so this can be used generically with a factory (though no clear reason
                         *  I can think of why you would want to).
                         */
                        IgnoreNodeReader () = default;

                    public:
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override;

                    public:
                        /**
                         *  Helper to convert a reader to a factory (something that creates the reader).
                         */
                        static  ReaderFromVoidStarFactory   AsFactory ();
                    };


                    /**
                     */
                    template    <typename   T>
                    class   ClassReader : public IElementConsumer {
                    public:
                        ClassReader (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, T* vp);
                        virtual void                            Activated (Context& r) override;
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override;
                        virtual void                            HandleTextInside (const String& text) override;
                        virtual void                            Deactivating () override;

                    public:
                        /**
                         *  Helper to convert a reader to a factory (something that creates the reader).
                         */
                        static  ReaderFromVoidStarFactory   AsFactory ();

                    private:
                        nonvirtual  ReaderFromVoidStarFactory   LookupFactoryForName_ (const Name& name) const;

                    private:
                        Traversal::Iterable<StructFieldInfo>    fFieldDescriptions_;
                        Context*                                fActiveContext_             {};
                        T*                                      fValuePtr_                  {};
                        Mapping<Name, StructFieldMetaInfo>      fFieldNameToTypeMap_;
                        Memory::Optional<StructFieldMetaInfo>   fValueFieldMetaInfo_;
                        shared_ptr<IElementConsumer>            fValueFieldConsumer_;
                        bool                                    fThrowOnUnrecongizedelts_   { false };       // else ignore
                    };


                    /**
                     *  Eat/ignore everything down the level named by 'tagToHandoff'.
                     *  If tagToHandoff is missing, start on the first element.
                     *  Note - its not an error to call theUseReader with multiple starts, but never two at a time.
                     *
                     *  \note - the tag == tagToHandoff will be handed to theUseReader.
                     *
                     *  \note   If multiple elements match, each one is started with theUseReader
                     */
                    class   ReadDownToReader : public IElementConsumer {
                    public:
                        ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader);
                        ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff);
                        ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& contextTag, const Name& tagToHandOff);
                        ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& contextTag1, const Name& contextTag2, const Name& tagToHandOff);

                    public:
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override;

                    public:
                        /**
                         *  Helper to convert a reader to a factory (something that creates the reader).
                         */
                        static  ReaderFromVoidStarFactory   AsFactory (const ReaderFromVoidStarFactory& theUseReader);
                        static  ReaderFromVoidStarFactory   AsFactory (const ReaderFromVoidStarFactory& theUseReader, const Name& tagToHandOff);

                    private:
                        shared_ptr<IElementConsumer>    fReader2Delegate2_;
                        Memory::Optional<Name>          fTagToHandOff_;
                    };


                    /**
                     *  ListOfObjectReader<> can read a container (vector-like) of elements. You can optionally specify
                     *  the name of each element, or omit that, to assume every sub-element is of the 'T' type.
                     */
                    template    <typename CONTAINER_OF_T, typename CONTAINER_ADAPTER_ADDER = Containers::Adapters::Adder<CONTAINER_OF_T>>
                    struct /* _Deprecated_ ("USE RepeatedElementReader- or ListOfObjectsReader_NEW deprecated v2.0a189") */ListOfObjectReader_DefaultTraits {
                        using   ContainerAdapterAdder = CONTAINER_ADAPTER_ADDER;
                    };
                    /***
                     *  DEPRECATED:
                     *  REPLACE
                            make_shared<ObjectReaderRegistry::ReadDownToReader> (
                    #if 1
                            make_shared<ObjectReaderRegistry::RepeatedElementReader<vector<Person_>>> (&people),
                            Name (L"envelope2"), Name (L"WithWhom")
                    #else
                            make_shared<ObjectReaderRegistry::ListOfObjectReader<vector<Person_>>> (&people, Name (L"WithWhom")),
                            Name (L"envelope2")
                    #endif
                            )
                    */
                    template    <typename   CONTAINER_OF_T, typename TRAITS = ListOfObjectReader_DefaultTraits<CONTAINER_OF_T>>
                    class
                    // disable cuz windows genrates crazy warnings, and not worth fiuxing cuz just one release
                    //tmphack disable _Deprecated_ ("USE RepeatedElementReader- or ListOfObjectsReader_NEW deprecated v2.0a189")
                        ListOfObjectReader: public IElementConsumer {
                    public:
                        using   ElementType = typename CONTAINER_OF_T::value_type;

                    public:
                        ListOfObjectReader (CONTAINER_OF_T* v);
                        ListOfObjectReader (CONTAINER_OF_T* v, const Name& memberElementName);

                    public:
                        virtual void                            Activated (Context& r) override;
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override;
                        virtual void                            Deactivating () override;

                    public:
                        /**
                         *  Helper to convert a reader to a factory (something that creates the reader).
                         */
                        static  ReaderFromVoidStarFactory   AsFactory ();
                        static  ReaderFromVoidStarFactory   AsFactory (const Name& memberElementName);

                    private:
                        Context*                fActiveContext_             {};
                        bool                    fReadingAT_                 { false };
                        ElementType             fCurTReading_               {};
                        Memory::Optional<Name>  fMemberElementName_;
                        CONTAINER_OF_T*         fValuePtr_                  {};
                        bool                    fThrowOnUnrecongizedelts_   { false };
                    };


                    /**
                     *  This is just like Repeated except it starts exactly one 'xml' level up from the target element.
                     *
                     *  This reader reads structured elements ('xml') content like:
                     *      <list>
                     *          <elt>
                     *          <elt>
                     *          ...
                     *      </list>
                     *  into some sort of sequenced container (like vector or sequence).
                     *
                     *  This is very similar to RepeatedElementReader (and uses it internally) - except that this starts
                     *  one level higher in teh Structured Element Stream (xml).
                     *
                     *  @see RepeatedElementReader
                     */
                    template    <typename CONTAINER_OF_T>
                    class   ListOfObjectsReader_NEW: public IElementConsumer {
                    public:
                        using   ElementType = typename CONTAINER_OF_T::value_type;

                    public:
                        ListOfObjectsReader_NEW (CONTAINER_OF_T* v);
                        ListOfObjectsReader_NEW (CONTAINER_OF_T* v, const Name& memberElementName);

                    public:
                        virtual void                            Activated (Context& r) override;
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override;
                        virtual void                            Deactivating () override;

                    public:
                        /**
                        *  Helper to convert a reader to a factory (something that creates the reader).
                        */
                        static  ReaderFromVoidStarFactory   AsFactory ();
                        static  ReaderFromVoidStarFactory   AsFactory (const Name& memberElementName);

                    private:
                        CONTAINER_OF_T*         fValuePtr_                  {};
                        Context*                fActiveContext_             {};
                        Memory::Optional<Name>  fMemberElementName_;
                        bool                    fThrowOnUnrecongizedelts_   { false };
                    };


                    /**
                     */
                    template    <typename   T>
                    class   MixinReader : public IElementConsumer {
                    public:
                        struct  MixinEltTraits {
                            ReaderFromVoidStarFactory           fReaderFactory;
                            function<bool (const Name& name)>   fReadsName = [](const Name& name) { return true; };
                            function<bool ()>                   fReadsText = []() { return true; };
                            function<Byte* (T*)>                fAddressOfSubElementFetcher;

                            static const function<Byte* (T*)>   kDefaultAddressOfSubElementFetcher;

                            MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<Byte* (T*)>& addressOfSubEltFetcher = kDefaultAddressOfSubElementFetcher);
                            MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool (const Name& name)>& readsName, const function<Byte* (T*)>& addressOfSubEltFetcher = kDefaultAddressOfSubElementFetcher);
                            MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool ()>& readsText, const function<Byte* (T*)>& addressOfSubEltFetcher = kDefaultAddressOfSubElementFetcher);
                            MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool (const Name& name)>& readsName, const function<bool ()>& readsText, const function<Byte* (T*)>& addressOfSubEltFetcher = kDefaultAddressOfSubElementFetcher);
                        };

                    public:
                        /**
                         */
                        MixinReader (T* vp, const Traversal::Iterable<MixinEltTraits>& mixins);

                    public:
                        virtual void                            Activated (Context& r) override;
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override;
                        virtual void                            HandleTextInside (const String& text) override;
                        virtual void                            Deactivating () override;

                    public:
                        /**
                         *  Helper to convert a reader to a factory (something that creates the reader).
                         */
                        static  ReaderFromVoidStarFactory   AsFactory (const Traversal::Iterable<MixinEltTraits>& mixins);

                    private:
                        Context*                                fActiveContext_{};
                        T*                                      fValuePtr_{};
                        Sequence<MixinEltTraits>                fMixins_;
                        Sequence<shared_ptr<IElementConsumer>>  fMixinReaders_;
                        Set<shared_ptr<IElementConsumer>>       fActivatedReaders_;
                    };


                    /**
                     *  \note   This is a public class instead of accessed via MakeCommonReader<T> since its very common that it will need
                     *          extra parameters (specified through the CTOR/and can be specified through the AsFactory method) to specify the name elements
                     *          to use for bounds attributes.
                     */
                    template    <typename   T>
                    class   RangeReader : public IElementConsumer {
                    public:
                        static  const   pair<Name, Name>    kDefaultBoundsNames;

                    public:
                        /**
                         */
                        RangeReader (T* intoVal, const pair<Name, Name>& pairNames = kDefaultBoundsNames);

                    public:
                        virtual void                            Activated (Context& r) override;
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override;
                        virtual void                            HandleTextInside (const String& text) override;
                        virtual void                            Deactivating () override;

                    public:
                        /**
                         *  Helper to convert a reader to a factory (something that creates the reader).
                         */
                        static  ReaderFromVoidStarFactory   AsFactory (const pair<Name, Name>& pairNames = kDefaultBoundsNames);

                    private:
                        using range_value_type_ = typename T::value_type;
                        struct RangeData_ {
                            range_value_type_ fLowerBound{};
                            range_value_type_ fUpperBound{};
                        };
                        pair<Name, Name>                fPairNames;
                        T*                              fValue_{};
                        RangeData_                      fProxyValue_{};
                        shared_ptr<IElementConsumer>    fActualReader_{};
                    };


                    /**
                     *  This is kind of like optional, but its for sequence elements - elements that are repeated inline.
                     *
                     *  To use RepeatedElementReader, just add the type to the type registry, and use it as normal for a regular field.
                     *  Only declare the field as a vector or Sequence <T>. This will invoke the right 'T' reader each time the name
                     *  is encountered, but instead of storing the value, it will append the value to the named object.
                     *
                     *  \par Example Usage
                     *      \code
                     *      // EXAMLPE DATA:
                     *              <envelope1>
                     *                    <person>
                     *                        <FirstName>Jim</FirstName>
                     *                        <LastName>Smith</LastName>
                     *                    </person>
                     *                    <person>
                     *                        <FirstName>Fred</FirstName>
                     *                        <LastName>Down</LastName>
                     *                    </person>
                     *                    <address>
                     *                        <city>Boston</city>
                     *                        <state>MA</state>
                     *                    </address>
                     *                    <address>
                     *                        <city>New York</city>
                     *                        <state>NY</state>
                     *                    </address>
                     *                    <address>
                     *                        <city>Albany</city>
                     *                        <state>NY</state>
                     *                    </address>
                     *              </envelope1>
                     *      struct  Person_ {
                     *          String firstName;
                     *          String lastName;
                     *      };
                     *      struct  Address_ {
                     *          String city;
                     *          String state;
                     *      };
                     *      struct Data_ {
                     *          vector<Person_>     people;
                     *          vector<Address_>    addresses;
                     *      };
                     *      ObjectReaderRegistry registry;
                     *      registry.AddCommonType<String> ();
                     *      registry.AddClass<Person_> (initializer_list<ObjectReaderRegistry::StructFieldInfo> {
                     *          { Name { L"FirstName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, firstName) },
                     *          { Name { L"LastName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, lastName) },
                     *      });
                     *      registry.AddCommonType<vector<Person_>> ();
                     *      registry.Add<vector<Person_>> (ObjectReaderRegistry::ConvertReaderToFactory <vector<Person_>, ObjectReaderRegistry::RepeatedElementReader<vector<Person_>>> ());
                     *      registry.AddClass<Address_> (initializer_list<ObjectReaderRegistry::StructFieldInfo> {
                     *          { Name { L"city" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Address_, city) },
                     *          { Name { L"state" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Address_, state) },
                     *      });
                     *      registry.Add<vector<Address_>> (ObjectReaderRegistry::ConvertReaderToFactory <vector<Address_>, ObjectReaderRegistry::RepeatedElementReader<vector<Address_>>> ());
                     *      registry.AddClass<Data_> (initializer_list<ObjectReaderRegistry::StructFieldInfo> {
                     *          { Name { L"person" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Data_, people) },
                     *          { Name { L"address" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Data_, addresses) },
                     *      });
                     *      Data_   data;
                     *      ObjectReaderRegistry::IConsumerDelegateToContext ctx { registry, make_shared<ObjectReaderRegistry::ReadDownToReader> (registry.MakeContextReader (&data)) };
                     *      XML::SAXParse (srcXMLStream, ctx);
                     *      \endcode
                     *
                     *  \note   This is like @see ListOfElementsReader, except that it starts on the elements of the array itself, as opposed
                     *          to just above.
                     */
                    template    <typename CONTAINER_OF_T, typename CONTAINER_ADAPTER_ADDER = Containers::Adapters::Adder<CONTAINER_OF_T>>
                    struct  RepeatedElementReader_DefaultTraits {
                        // @todo
                        // @todo - LOSE all reference to explicit traits use, and then LOSE ContainerType, ElementType, and MakeActualReader and
                        // just retrain ContainerAdapterAdder.
                        using   ContainerType = CONTAINER_OF_T;
                        using   ElementType = typename ContainerType::value_type;
                        using   ContainerAdapterAdder = CONTAINER_ADAPTER_ADDER;
                        static  shared_ptr<IElementConsumer>   MakeActualReader (Context& r, ElementType* proxyValue)
                        {
                            RequireNotNull (proxyValue);
                            return r.GetObjectReaderRegistry ().MakeContextReader (proxyValue);
                        }
                    };
                    template    <typename   T, typename TRAITS = RepeatedElementReader_DefaultTraits<T>>
                    class   RepeatedElementReader : public IElementConsumer {
                    public:
                        using   ContainerType = T;
                        using   ElementType = typename ContainerType::value_type;

                    public:
                        /**
                         *  If name specified, pass only that named subelelement to subeltreader.
                         *
                         *  If actualElementFactory specified, use that to create subelement reader, and otherwise use
                         *  Context::GetObjectReaderRegistry ().MakeContextReader ()
                         */
                        RepeatedElementReader (ContainerType* v);
                        RepeatedElementReader (ContainerType* v, const Name& readonlyThisName, const ReaderFromVoidStarFactory& actualElementFactory);
                        RepeatedElementReader (ContainerType* v, const ReaderFromVoidStarFactory& actualElementFactory);
                        RepeatedElementReader (ContainerType* v, const Name& readonlyThisName);

                    public:
                        virtual void                            Activated (Context& r) override;
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override;
                        virtual void                            HandleTextInside (const String& text) override;
                        virtual void                            Deactivating () override;

                    public:
                        /**
                         *  Helper to convert a reader to a factory (something that creates the reader).
                         */
                        static  ReaderFromVoidStarFactory   AsFactory ();
                        static  ReaderFromVoidStarFactory   AsFactory (const Name& readonlyThisName, const ReaderFromVoidStarFactory& actualElementFactory);
                        static  ReaderFromVoidStarFactory   AsFactory (const ReaderFromVoidStarFactory& actualElementFactory);
                        static  ReaderFromVoidStarFactory   AsFactory (const Name& readonlyThisName);

                    private:
                        ContainerType*                              fValuePtr_          {};
                        Memory::Optional<ReaderFromVoidStarFactory> fReaderRactory_     {};     // if missing, use Context::GetObjectReaderRegistry ().MakeContextReader ()
                        function<bool (Name)>                       fReadThisName_      { [] (const Name & n) { return true;  } };
                        ElementType                                 fProxyValue_        {};
                        shared_ptr<IElementConsumer>                fActiveSubReader_   {};
                    };


                    /**
                     */
                    [[noreturn]]    void    ThrowUnRecognizedStartElt (const Name& name);


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ObjectReader.inl"

#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_*/
