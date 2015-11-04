/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_    1

#include    "../../StroikaPreComp.h"

#include    <type_traits>
#include    <typeindex>

#include    "../../Characters/String.h"
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

#include    "ObjectReader.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *
 *      @todo    gross hacks - replciatigng basic objectreaders to get this limping along...

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
                class   ObjectReaderRegistry;


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
                 */
                class   IElementConsumer;

                class   Context;

                class   IConsumerDelegateToContext;




                /**
                 */
                enum    class  UnknownSubElementDisposition {
                    eIgnore,
                    eEndObject
                };


                /**
                 *  Subclasses of this abstract class are responsible for consuming data at a given level of the SAX 'tree', and transform
                 *  it into a related object.
                 */
                class   IElementConsumer : public enable_shared_from_this<IElementConsumer> {
                protected:
                    /**
                     */
                    IElementConsumer () = default;

                public:
                    virtual ~IElementConsumer () = default;
                    virtual void    HandleChildStart (Context& r, const StructuredStreamEvents::Name& name)  {};
                    virtual void    HandleTextInside (Context& r, const String& text) {};


                    // tmphack - return true if caller should Pop
                    virtual bool    HandleEndTag (Context& r) { return true; };

                    /**
                     *  pushed onto context stack
                     */
                    virtual void    Activated (Context& r) {};

                    /**
                     * About to pop from ontext stack
                     */
                    virtual void    Deactivating (Context& r) {};

#if 0
                    // not used yet



                    virtual void    HandleChildEnd (Context& r, const StructuredStreamEvents::Name& name) {};

#endif
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
                class   Context {
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


                class   IConsumerDelegateToContext : public StructuredStreamEvents::IConsumer {
                public:
                    IConsumerDelegateToContext () = delete;
                    IConsumerDelegateToContext (Context& r);
                    IConsumerDelegateToContext (const Context&) = delete;
                    IConsumerDelegateToContext& operator= (const IConsumerDelegateToContext&) = delete;
                private:
                    Context&    fContext_;
                public:
                    virtual void    StartElement (const StructuredStreamEvents::Name& name) override;
                    virtual void    EndElement (const StructuredStreamEvents::Name& name) override;
                    virtual void    TextInsideElement (const String& text) override;
                };




                /**
                 */
                void    ThrowUnRecognizedStartElt (const StructuredStreamEvents::Name& name);





                /**
                 * BuiltinReader<> is not implemented for all types - just for the specialized ones listed below:
                 *      String
                 *      int
                 *      Time::DateTime
                 */
                template    <typename   T>
                class   BuiltinReader : public IElementConsumer {
                public:
                    BuiltinReader (T* intoVal);

                private:
                    T* value_;

                public:
                    virtual void    HandleChildStart (Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (Context& r, const String& text) override;
                };
                template    <>
                class   BuiltinReader<String>;
                template    <>
                class   BuiltinReader<int>;
                template    <>
                class   BuiltinReader<unsigned int>;
                template    <>
                class   BuiltinReader<bool>;
                template    <>
                class   BuiltinReader<float>;
                template    <>
                class   BuiltinReader<double>;
                template    <>
                class   BuiltinReader<Time::DateTime>;


                /**
                 *  OptionalTypesReader supports reads of optional types. This will work - for any types for
                 *  which BuiltinReader<T> is implemented.
                 *
                 *  Note - this ALWAYS produces a result. Its only called when the element in quesiton has
                 *  already occurred. The reaosn for Optional<> part is because the caller had an optional
                 *  element which might never have triggered the invocation of this class.
                 */
                template    <typename   T, typename ACTUAL_READER = BuiltinReader<T>>
                class   OptionalTypesReader : public IElementConsumer {
                public:
                    OptionalTypesReader (Memory::Optional<T>* intoVal);

                private:
                    Memory::Optional<T>*    value_;
                    T                       proxyValue_;
                    ACTUAL_READER           actualReader_;  // this is why its crucial this partial specialization is only used on optional of types a real reader is available for

                public:
                    virtual void    HandleChildStart (Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (Context& r, const String& text) override;
                    virtual bool    HandleEndTag (Context& r) override;
                    virtual void    Deactivating (Context& r) override;
                };


                /**
                 *  Push one of these Nodes onto the stack to handle 'reading' a node which is not to be read.
                 *  This is necessary to balance out the Start Tag / End Tag combinations.
                 */
                class   IgnoreNodeReader : public IElementConsumer {
                public:
                    IgnoreNodeReader ();
                private:
                    int fDepth_;
                public:
                    virtual void    HandleChildStart (Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (Context& r, const String& text) override;
                    virtual bool    HandleEndTag (Context& r) override;
                };




                /**
                 *  The ListOfObjectReader<> template can be used to create a vector of type "T" -
                 *  to capture repeating elements in a sequence.
                 *
                 *  EXAMPLE TRAITS:
                 *      struct  ReaderTraits {
                 *              using   ElementType     =   String;
                 *              using   ReaderType      =   BuiltinReader<String>;
                 *              static  const wchar_t           ElementName[] =  L"Name";
                 *      };
                 *
                 *  @todo REPLACE THIS TRAITS API WITH A FACTORY BUILDING NEW READER_OF_T
                 */
                template    <typename TRAITS>
                struct  ListOfObjectReader: public IElementConsumer {
                public:
                    ListOfObjectReader (vector<typename TRAITS::ElementType>* v, UnknownSubElementDisposition unknownEltDisposition = UnknownSubElementDisposition::eEndObject);

                    virtual void HandleChildStart (Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual bool HandleEndTag (Context& r) override;
                    virtual void Deactivating (Context& r) override;

                private:
                    vector<typename TRAITS::ElementType>*  fValuePtr;
                    typename TRAITS::ElementType                fCurTReading_;
                    shared_ptr<typename TRAITS::ReaderType>     fCurReader_;
                    UnknownSubElementDisposition                fUnknownSubElementDisposition_;
                };



                /**
                // Look back to DataExchange::ObjectVariantmapper, but for now - KISS
                 */
                class   ObjectReaderRegistry {
                public:
                    using   ReaderFromVoidStarFactory = function<shared_ptr<IElementConsumer> (void* destinationObject)>;

                public:
                    void    Add (type_index forType, const ReaderFromVoidStarFactory& readerFactory)
                    {
                        fFactories_.Add (forType, readerFactory);
                    }
                    template    <typename T>
                    void    Add (const function<shared_ptr<IElementConsumer> (T*)>& readerFactory)
                    {
                        Add (typeid (T), [readerFactory] (void* data) { return readerFactory (reinterpret_cast<T*> (data)); });
                    }

                public:
                    shared_ptr<IElementConsumer>    MakeContextReader (type_index ti, void* destinationObject) const
                    {
                        ReaderFromVoidStarFactory  factory = *fFactories_.Lookup (ti); // must be found or caller/assert error
                        return factory (destinationObject);
                    }
                    template    <typename T>
                    shared_ptr<IElementConsumer>    MakeContextReader (T* destinationObject) const
                    {
                        return MakeContextReader (typeid (T), destinationObject);
                    }

                private:
                    Mapping<type_index, ReaderFromVoidStarFactory> fFactories_;
                };


                /// @todo - make these unneeded by exposing docEltBuilder helper or bette rclass

                // puts docEltsBuilder on stack and then keeps reading from sax til done. Asserts buildStack is EMPTY at end of this call (and docEltsBuilder should ahve received
                // a HandleChildStar tand HandleEndTag() method call (exactly once).
                nonvirtual  void    Run (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<IElementConsumer>& docEltBuilder, const Streams::InputStream<Memory::Byte>& in);
                nonvirtual  void    Run (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<IElementConsumer>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, const Streams::InputStream<Memory::Byte>& in);


                template    <typename   T>
                class   ComplexObjectReader2 : public IElementConsumer {
                public:
                    ComplexObjectReader2 (const Mapping<String, pair<type_index, size_t>>& maps, T* vp)
                        : IElementConsumer()
                        , fValuePtr (vp)
                        , fFieldNameToTypeMap (maps)
                    {
                    }
                    virtual void    HandleChildStart (StructuredStreamEvents::Context& r, const StructuredStreamEvents::Name& name) override
                    {
                        Optional<pair<type_index, size_t>>   ti = fFieldNameToTypeMap.Lookup (name);
                        if (ti) {
                            Byte*   operatingOnObj = reinterpret_cast<Byte*> (this->fValuePtr);
                            Byte*   operatingOnObjField = operatingOnObj + ti->second;
                            r.Push (r.GetObjectReaderRegistry ().MakeContextReader (ti->first, operatingOnObjField));
                        }
                        else if (fThrowOnUnrecongizedelts) {
                            ThrowUnRecognizedStartElt (name);
                        }
                        else {
                            r.Push (make_shared<IgnoreNodeReader> ());
                        }
                    }
                    T*  fValuePtr;;
                    Mapping<StructuredStreamEvents::Name, pair<type_index, size_t>>     fFieldNameToTypeMap;            // @todo fix to be mapping on Name but need op< etc defined
                    bool                                                                fThrowOnUnrecongizedelts;       // else ignroe
                };
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory mkComplexObjectReader2Factory (const Mapping<String, pair<type_index, size_t>>& fieldname2Typeamps)
                {
                    return [fieldname2Typeamps] (void* data) -> shared_ptr<IElementConsumer> { return make_shared<ComplexObjectReader2<T>> (fieldname2Typeamps, reinterpret_cast<T*> (data)); };
                }



                template    <typename ELEMENT_TYPE>
                struct ListOfObjectReader2: public IElementConsumer {
                    bool                    readingAT_;
                    ELEMENT_TYPE            curTReading_;
                    ObjectReaderRegistry&   fObjRegistry;
                    String                  fName;
                    vector<ELEMENT_TYPE>*    fValuePtr;

                    ListOfObjectReader2 (ObjectReaderRegistry* objReg, const String& name, vector<ELEMENT_TYPE>* v)
                        : IElementConsumer ()
                        , readingAT_ (false)
                        , fObjRegistry  (*objReg)
                        , fName  (name)
                        , fValuePtr (v)
                    {
                    }
                    virtual void HandleChildStart (Context& r, const StructuredStreamEvents::Name& name) override
                    {
                        if (name == fName) {
                            if (readingAT_) {
                                Containers::ReserveSpeedTweekAdd1 (*this->fValuePtr);
                                this->fValuePtr->push_back (curTReading_);
                                readingAT_ = false;
                            }
                            readingAT_ = true;
                            curTReading_ = ELEMENT_TYPE (); // clear because dont' want to keep values from previous elements
                            r.Push (fObjRegistry.MakeContextReader<ELEMENT_TYPE> (&curTReading_));
                        }
                        else {
                            ThrowUnRecognizedStartElt (name);
                        }
                    }
                    virtual void    Deactivating (Context& r) override
                    {
                        if (readingAT_) {
                            Containers::ReserveSpeedTweekAdd1 (*this->fValuePtr);
                            this->fValuePtr->push_back (curTReading_);
                            readingAT_ = false;
                        }
                    }
                };


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
