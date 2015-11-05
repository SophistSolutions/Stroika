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
#include    "../../Traversal/DiscreteRange.h"
#include    "../../Traversal/Range.h"

#include    "../StructFieldMetaInfo.h"

#include    "ObjectReader.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *
 *      @todo    gross hacks - replciatigng basic objectreaders to get this limping along...
 *
 *      @todo   make nested READER classes PRIVATE, and have public ADDCOMMON<T> methods to registry - like we do
 *              For ObjectVariantMapper.
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

                &&&&&& TODO - @todo - rewrite all these docs
                *
                 *      The basic idea of the ObjectReader is to make it easier to write C++ code
                 *  to deserialize an XML source (via SAX), into a C++ data structure. This tends to be
                 *  MUCH MUCH harder than doing something similar by loading an XML DOM, and then traversing
                 *  the DOM with XPath. So why would you do it? This way is dramatically more efficeint.
                 *  For one thing - there is no need to have the entire source in memory at a time, and there
                 *  is no need to ever construct intermediary DOM nodes.
                 *
                 *      We need good docs - on how to use this - but for the time being, just look at the
                 *  example usage in the regression test.

                   *    Look back to DataExchange::ObjectVariantmapper, but for now - KISS
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
                    class   ReadDownToReader;
                    template    <typename   T>
                    class   SimpleReader;
                    template    <typename   T>
                    class   ListOfObjectReader;
                    template    <typename   T>
                    class   OptionalTypesReader;

                public:
                    using   ReaderFromVoidStarFactory = function<shared_ptr<IElementConsumer> (void* destinationObject)>;

                public:
                    void    Add (type_index forType, const ReaderFromVoidStarFactory& readerFactory);
                    template    <typename T>
                    void    Add (const function<shared_ptr<IElementConsumer> (T*)>& readerFactory);

                public:
                    /**
                     *  This is generally just called inside of another composite reader to read sub-objects.
                     */
                    shared_ptr<IElementConsumer>    MakeContextReader (type_index ti, void* destinationObject) const;
                    template    <typename T>
                    shared_ptr<IElementConsumer>    MakeContextReader (T* destinationObject) const;

                private:
                    Mapping<type_index, ReaderFromVoidStarFactory> fFactories_;
                };


                /// @todo - make these unneeded by exposing docEltBuilder helper or bette rclass

                // puts docEltsBuilder on stack and then keeps reading from sax til done. Asserts buildStack is EMPTY at end of this call (and docEltsBuilder should ahve received
                // a HandleChildStar  method call (exactly once).
                nonvirtual  void    Run (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<ObjectReaderRegistry::IElementConsumer>& docEltBuilder, const Streams::InputStream<Memory::Byte>& in);
                nonvirtual  void    Run (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<ObjectReaderRegistry::IElementConsumer>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, const Streams::InputStream<Memory::Byte>& in);


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

                private:
                    const ObjectReaderRegistry& fObjectReaderRegistry_;

                public:
                    Context (const ObjectReaderRegistry& objectReaderRegistry);
                    Context (const Context&) = delete;
                    Context& operator= (const Context&) = delete;

                public:
                    const   ObjectReaderRegistry&   GetObjectReaderRegistry () const;

                public:
                    nonvirtual  void    Push (const shared_ptr<IElementConsumer>& elt);
                    nonvirtual  void    Pop ();

                public:
                    nonvirtual  shared_ptr<IElementConsumer>   GetTop () const;

                public:
                    bool    empty () const { return fStack_.empty (); }

                private:
                    vector<shared_ptr<IElementConsumer>> fStack_;

                private:
                    friend  class   ObjectReader;
                };


                /**
                 */
                class   ObjectReaderRegistry::IConsumerDelegateToContext : public StructuredStreamEvents::IConsumer {
                public:
                    IConsumerDelegateToContext () = delete;
                    IConsumerDelegateToContext (Context& r);
                    IConsumerDelegateToContext (const Context&) = delete;
                    IConsumerDelegateToContext& operator= (const IConsumerDelegateToContext&) = delete;
                private:
                    Context&    fContext_;
                public:
                    virtual void    StartElement (const Name& name) override;
                    virtual void    EndElement (const Name& name) override;
                    virtual void    TextInsideElement (const String& text) override;
                };


                /**
                 *  Push one of these Nodes onto the stack to handle 'reading' a node which is not to be read.
                 *  This is necessary to balance out the Start Tag / End Tag combinations.
                 */
                class   ObjectReaderRegistry::IgnoreNodeReader : public IElementConsumer {
                public:
                    IgnoreNodeReader ();
                private:
                    int fDepth_;
                public:
                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name) override;
                };


                /**
                 */
                template    <typename   T>
                class   ObjectReaderRegistry::ClassReader : public IElementConsumer {
                public:
                    ClassReader (const Mapping<String, StructFieldMetaInfo>& maps, T* vp);
                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name) override;
                private:
                    T*  fValuePtr;;
                    Mapping<Name, StructFieldMetaInfo>  fFieldNameToTypeMap;            // @todo fix to be mapping on Name but need op< etc defined
                    bool                                fThrowOnUnrecongizedelts { false };       // else ignroe
                };
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory mkClassReaderFactory (const Mapping<String, StructFieldMetaInfo>& fieldname2Typeamps);


                /**
                 *   Eat/ignore everything down the level named by 'tagToHandoff'.
                 *    Note - its not an error to call theUseReader with multiple starts, but never two at a time.
                 *
                 *    \note - the tag == tagToHandoff will be handed to theUseReader.
                 */
                class   ObjectReaderRegistry::ReadDownToReader : public IElementConsumer {
                public:
                    ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff);

                public:
                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name) override;

                private:
                    shared_ptr<IElementConsumer>    fReader2Delegate2_;
                    Name                            fTagToHandOff_;
                };


                /**
                &&& @todo - docoment - to specialize - just replace Deactivating with type specific value
                *
                 * SimpleReader<> is not implemented for all types - just for the specialized ones listed below:
                 *      String
                 *      int
                 *      Time::DateTime
                 */
                template    <typename   T>
                class   ObjectReaderRegistry::SimpleReader : public IElementConsumer {
                public:
                    SimpleReader (T* intoVal);

                private:
                    Characters::StringBuilder   fBuf_;
                    T*                          fValue_;

                public:
                    virtual shared_ptr<IElementConsumer>    HandleChildStart (Context& r, const Name& name) override;
                    virtual void                            HandleTextInside (Context& r, const String& text) override;
                    virtual void                            Deactivating (Context& r) override;
                };


                template <>
                void   ObjectReaderRegistry::SimpleReader<String>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader<int>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader<unsigned int>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader<bool>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader<float>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader<double>::Deactivating (Context& r);
                template <>
                void   ObjectReaderRegistry::SimpleReader<Time::DateTime>::Deactivating (Context& r);


                /**
                 */
                template    <typename T>
                class   ObjectReaderRegistry::ListOfObjectReader: public IElementConsumer {
                public:
                    using   ElementType = T;

                public:
                    ListOfObjectReader (const Name& name, vector<ElementType>* v);
                    virtual shared_ptr<IElementConsumer> HandleChildStart (Context& r, const Name& name) override;
                    virtual void    Deactivating (Context& r) override;

                private:
                    bool                    fReadingAT_;
                    T                       fCurTReading_;
                    Name                    fName;
                    vector<ElementType>*    fValuePtr;
                    bool                    fThrowOnUnrecongizedelts { false };
                };


                /**
                 *  @todo merge this into SimpleTypeReader, or at least make sure handled atuoamtically by ObjectRegistryReader (hidden)
                 *
                 *
                 *  OptionalTypesReader supports reads of optional types. This will work - for any types for
                 *  which SimpleReader<T> is implemented.
                 *
                 *  Note - this ALWAYS produces a result. Its only called when the element in quesiton has
                 *  already occurred. The reaosn for Optional<> part is because the caller had an optional
                 *  element which might never have triggered the invocation of this class.
                 */
                template    <typename   T>
                class   ObjectReaderRegistry::OptionalTypesReader : public IElementConsumer {
                public:
                    OptionalTypesReader (Memory::Optional<T>* intoVal);

                private:
                    Memory::Optional<T>*    value_;
                    T                       proxyValue_;
                    SimpleReader<T>         actualReader_;  // this is why its crucial this partial specialization is only used on optional of types a real reader is available for

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
