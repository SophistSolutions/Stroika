/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_    1

#include    "../../StroikaPreComp.h"

#include    <type_traits>
#include    <typeindex>

#include    "../../Characters/String.h"
#include    "../../Characters/StringBuilder.h"
#include    "../../Configuration/Enumeration.h"
#include    "../../Containers/Bijection.h"
#include    "../../Containers/Collection.h"
#include    "../../Containers/Mapping.h"
#include    "../../Containers/Sequence.h"
#include    "../../Containers/Set.h"
#include    "../../Containers/SortedCollection.h"
#include    "../../Containers/SortedMapping.h"
#include    "../../Containers/SortedSet.h"
#include    "../../Execution/Synchronized.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/Optional.h"
#include    "../../Streams/InputStream.h"
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
 *
 *      @todo    Need much improved ObjectReaderRegistry docs
 *
 *      @todo   make more nested READER classes PRIVATE, and improve public ADDCOMMON<T> methods to registry - like we do
 *              For ObjectVariantMapper.
 *
 *      @todo   ConvertReaderToFactory() should do varargs perfect forwarding of extra params!
 *
 *      @todo   Get rid of or do diffenrtly the Run() methods - so can turn on/off Contextg debugging easier.
 *
 *      @todo   USE UnknownSubElementDisposition more throughly...
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {


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
                 *      mapper.AddClass<Person_> (initializer_list<pair<Name, StructFieldMetaInfo>> {
                 *          { Name { L"FirstName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, firstName) },
                 *          { Name { L"LastName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, lastName) },
                 *      });
                 *      Person_ p;
                 *      ObjectReaderRegistry::IConsumerDelegateToContext tmp (mapper, mapper.mkReadDownToReader (mapper.MakeContextReader (&p)));
                 *      XML::SAXParse (mkdata_ (), tmp);
                 *      \endcode
                 */
                class   ObjectReaderRegistry {
                public:
                    ObjectReaderRegistry () = default;
                    ObjectReaderRegistry (const ObjectReaderRegistry&) = default;

                public:
                    ObjectReaderRegistry& operator= (const ObjectReaderRegistry&) = default;

                public:
                    class   Context;
                    class   IElementConsumer;
                    class   IConsumerDelegateToContext;
                    class   IgnoreNodeReader;
                    template    <typename   T>
                    class   ClassReader;
                    template    <typename   T>
                    class   ListOfObjectReader;

                private:
                    template    <typename   T>
                    class   SimpleReader_;
                    template    <typename   T>
                    class   OptionalTypesReader_;

                public:
                    template    <typename T>
                    using   ReaderFromTStarFactory = function<shared_ptr<IElementConsumer> (T* destinationObject)>;

                public:
                    using   ReaderFromVoidStarFactory = function<shared_ptr<IElementConsumer> (void* destinationObject)>;

                public:
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
                    class   ReadDownToReader;

                public:
                    static  shared_ptr<ReadDownToReader>    mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader);
                    static  shared_ptr<ReadDownToReader>    mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff);

                public:
                    /**

                    &todo
                    &&&& FROM ObjectVaarianeMapper... .nDOPCS NOT RIGHT BUT A START
                    ....
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
                    template    <typename T, typename... ARGS>
                    static  ReaderFromVoidStarFactory  MakeCommonReader (ARGS&& ... args);

                public:
                    /**
                     */
                    template    <typename CLASS>
                    nonvirtual  void    AddClass (const Mapping<Name, StructFieldMetaInfo>& fieldInfo);

                public:
                    /**
                     */
                    template    <typename CLASS>
                    nonvirtual  ReaderFromVoidStarFactory    MakeClassReader (const Mapping<Name, StructFieldMetaInfo>& fieldInfo);

                public:
                    /**
                     *  It's pretty easy to code up a 'reader' class - just a simple subclass of IElementConsumer.
                     *  There are lots of examples in this module.
                     *
                     *  But the APIs in the ObjectReaderRegistry require factories, and this utility function constructs
                     *  a factory from the given READER class template parameter.
                     */
                    template    <typename T, typename READER>
                    static  auto    ConvertReaderToFactory () -> ObjectReaderRegistry::ReaderFromVoidStarFactory;

                public:
                    /**
                     *  This is generally just called inside of another composite reader to read sub-objects.
                     */
                    shared_ptr<IElementConsumer>    MakeContextReader (type_index ti, void* destinationObject) const;
                    template    <typename T>
                    shared_ptr<IElementConsumer>    MakeContextReader (T* destinationObject) const;

                private:
                    template    <typename T>
                    static  ReaderFromVoidStarFactory   cvtFactory_ (const ReaderFromTStarFactory<T>& tf );
                    template    <typename T>
                    static  ReaderFromVoidStarFactory   MakeCommonReader_SIMPLEREADER_ ();
                private:
                    template    <typename T>
                    static  ReaderFromVoidStarFactory  MakeCommonReader_ (const T*);
                    template    <typename T, typename TRAITS>
                    static  ReaderFromVoidStarFactory  MakeCommonReader_ (const Memory::Optional<T, TRAITS>*);
                    template    <typename T>
                    static  ReaderFromVoidStarFactory  MakeCommonReader_ (const vector<T>*);
                    template    <typename T>
                    static  ReaderFromVoidStarFactory  MakeCommonReader_ (const vector<T>*, const Name& name);
                    template    <typename T>
                    static  ReaderFromVoidStarFactory  MakeCommonReader_ (const Sequence<T>*);
                    template    <typename T>
                    static  ReaderFromVoidStarFactory  MakeCommonReader_ (const Sequence<T>*, const Name& name);

                private:
                    Mapping<type_index, ReaderFromVoidStarFactory> fFactories_;
                };


                /**
                 *  Subclasses of this abstract class are responsible for consuming data at a given level of the SAX 'tree', and transform
                 *  it into a related object.
                 */
                class   ObjectReaderRegistry::IElementConsumer : public enable_shared_from_this<IElementConsumer> {
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
                     */
                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name)  { return nullptr; };

                public:
                    /**
                     */
                    virtual void    HandleTextInside (Context& r, const String& text) {};

                public:
                    /**
                     *  pushed onto context stack
                     */
                    virtual void    Activated (Context& r) {};

                public:
                    /**
                     * About to pop from ontext stack
                     */
                    virtual void    Deactivating (Context& r) {};
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
                 */
                class   ObjectReaderRegistry::Context {
                public:
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
                public:
                    bool    fTraceThisReader { false };       // very noisy - off by default even for tracemode
                    nonvirtual  String TraceLeader_ () const;
#endif

                public:
                    Context (const ObjectReaderRegistry& objectReaderRegistry);
                    Context (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<IElementConsumer>& initialTop);
                    Context (const Context&) = delete;
                    Context (Context&& from);
                    Context& operator= (const Context&) = delete;

                public:
                    const   ObjectReaderRegistry&   GetObjectReaderRegistry () const;

                public:
                    nonvirtual  void    Push (const shared_ptr<IElementConsumer>& elt);

                public:
                    nonvirtual  void    Pop ();

                public:
                    nonvirtual  shared_ptr<IElementConsumer>   GetTop () const;

                public:
                    nonvirtual  bool    empty () const;

                private:
                    const ObjectReaderRegistry&             fObjectReaderRegistry_;
                    vector<shared_ptr<IElementConsumer>>    fStack_;

                private:
                    friend  class   ObjectReader;
                };


                /**
                 */
                class   ObjectReaderRegistry::IConsumerDelegateToContext : public StructuredStreamEvents::IConsumer {
                public:
                    IConsumerDelegateToContext () = delete;
                    IConsumerDelegateToContext (Context&& r);
                    IConsumerDelegateToContext (const ObjectReaderRegistry& objectReaderRegistry);
                    IConsumerDelegateToContext (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<IElementConsumer>& initialTop);
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
                class   ObjectReaderRegistry::IgnoreNodeReader : public IElementConsumer {
                public:
                    IgnoreNodeReader () = default;
                public:
                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name) override;
                };


                /**
                 */
                template    <typename   T>
                class   ObjectReaderRegistry::ClassReader : public IElementConsumer {
                public:
                    ClassReader (const Mapping<Name, StructFieldMetaInfo>& maps, T* vp);
                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name) override;
                    virtual void                            HandleTextInside (Context& r, const String& text) override;
                    virtual void                            Deactivating (Context& r) override;
                private:
                    T*                                      fValuePtr_;
                    Mapping<Name, StructFieldMetaInfo>      fFieldNameToTypeMap_;
                    Memory::Optional<StructFieldMetaInfo>   fValueFieldMetaInfo_;
                    shared_ptr<IElementConsumer>            fValueFieldConsumer_;
                    bool                                    fThrowOnUnrecongizedelts_ { false };       // else ignore
                };


                /**
                 *   Eat/ignore everything down the level named by 'tagToHandoff'.
                 *    Note - its not an error to call theUseReader with multiple starts, but never two at a time.
                 *
                 *    \note - the tag == tagToHandoff will be handed to theUseReader.
                 */
                class   ObjectReaderRegistry::ReadDownToReader : public IElementConsumer {
                public:
                    ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader);
                    ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff);

                public:
                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name) override;

                private:
                    shared_ptr<IElementConsumer>    fReader2Delegate2_;
                    Memory::Optional<Name>          fTagToHandOff_;
                };


                /**
                 *  [private]
                 *  SimpleReader_<> is not implemented for all types - just for the with Deactivating specialized below;
                 */
                template    <typename   T>
                class   ObjectReaderRegistry::SimpleReader_ : public IElementConsumer {
                public:
                    SimpleReader_ (T* intoVal);

                private:
                    Characters::StringBuilder   fBuf_;
                    T*                          fValue_;

                public:
                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name) override;
                    virtual void                            HandleTextInside (Context& r, const String& text) override;
                    virtual void                            Deactivating (Context& r) override;
                };


                template <>
                void   ObjectReaderRegistry::SimpleReader_<String>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader_<int>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader_<unsigned int>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader_<bool>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader_<float>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader_<double>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader_<Time::DateTime>::Deactivating (Context& r);


                template <>
                ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const String*);
                template <>
                ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const int*);
                template <>
                ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const unsigned int*);
                template <>
                ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const bool*);
                template <>
                ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const float*);
                template <>
                ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const double*);
                template <>
                ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const Time::DateTime*);


                /**
                 *  ListOfObjectReader<> can read a container (vector-like) of elements. You can optionally specify
                 *  the name of each element, or omit that, to assume every sub-element is of the 'T' type.
                 */
                template    <typename CONTAINER_OF_T>
                class   ObjectReaderRegistry::ListOfObjectReader: public IElementConsumer {
                public:
                    using   ElementType = typename CONTAINER_OF_T::value_type;

                public:
                    ListOfObjectReader (CONTAINER_OF_T* v);
                    ListOfObjectReader (CONTAINER_OF_T* v, const Name& memberElementName);

                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name) override;
                    virtual void                            Deactivating (Context& r) override;

                private:
                    bool                    fReadingAT_;
                    ElementType             fCurTReading_;
                    Memory::Optional<Name>  fMemberElementName_;
                    CONTAINER_OF_T*         fValuePtr_;
                    bool                    fThrowOnUnrecongizedelts_ { false };
                };


                /**
                 *  [private]
                 *
                 *  OptionalTypesReader_ supports reads of optional types. This will work - for any types for
                 *  which SimpleReader<T> is implemented.
                 *
                 *  Note - this ALWAYS produces a result. Its only called when the element in quesiton has
                 *  already occurred. The reaosn for Optional<> part is because the caller had an optional
                 *  element which might never have triggered the invocation of this class.
                 */
                template    <typename   T>
                class   ObjectReaderRegistry::OptionalTypesReader_ : public IElementConsumer {
                public:
                    OptionalTypesReader_ (Memory::Optional<T>* intoVal);

                private:
                    Memory::Optional<T>*    fValue_;
                    T                       fProxyValue_;
                    SimpleReader_<T>        fActualReader_;  // this is why its crucial this partial specialization is only used on optional of types a real reader is available for

                public:
                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name) override;
                    virtual void                            HandleTextInside (Context& r, const String& text) override;
                    virtual void                            Deactivating (Context& r) override;
                };


                /**
                 */
                void    _NoReturn_  ThrowUnRecognizedStartElt (const Name& name);


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ObjectReaderRegistry.inl"

#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_*/
