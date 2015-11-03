/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_   1

#include    "../../StroikaPreComp.h"

#include    <memory>

#include    "../../Memory/Optional.h"
#include    "../../Time/DateTime.h"

//tmphac
#include    "../XML/SAXReader.h"



/**
 *  \file
 *
 *              \\\ MIDDLE OF MASSIVE REFACTORING - SO DONT TOUCH FOR A WHILE TIL STABLE....
 *              \\\ AND MANY COMMENTS MAYBE WRONG/OUTDATED
 *  TODO:
 *      @todo   This was hard to use. See if I can find example of where I used it, and either cleanup,
 *              or maybe get rid of this - OBSOLETE - and see if easier using new ObjectVairantMapper stuff.
 *
 *              BUT - note - this had the advnatage of using no intermediate format!!! So see about
 *              capturing its adnvanteas?? Maybe re-use some stuff with new metadata?
 *
 *              Note - there is an EXAMPLE of use of this in the regression tests. Use that to evaluate.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {


#ifndef qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
#define qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing  qDebug
#endif


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
                class   ObjectReader {
                public:
                    class   IContextReader;

                public:
                    class   Context;

                public:
                    class   IConsumerDelegateToContext;

                public:
                    ObjectReader () = default;

                public:
                    // puts docEltsBuilder on stack and then keeps reading from sax til done. Asserts buildStack is EMPTY at end of this call (and docEltsBuilder should ahve received
                    // a HandleChildStar tand HandleEndTag() method call (exactly once).
                    nonvirtual  void    Run (const shared_ptr<IContextReader>& docEltBuilder, const Streams::InputStream<Memory::Byte>& in);
                    nonvirtual  void    Run (const shared_ptr<IContextReader>& docEltBuilder, const String& docEltUri, const String& docEltLocalName, const Streams::InputStream<Memory::Byte>& in);
                };


                /**
                 *  Subclasses of this abstract class are responsible for consuming data at a given level of the SAX 'tree', and transform
                 *  it into a related object.
                 */
                class   ObjectReader::IContextReader : public enable_shared_from_this<ObjectReader::IContextReader> {
                public:
                    virtual ~IContextReader () = default;
                    virtual void    HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) = 0;
                    virtual void    HandleTextInside (ObjectReader::Context& r, const String& text) = 0;
                    virtual void    HandleEndTag (ObjectReader::Context& r) = 0;
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
                 *          This means that the start and end tags for a given pair, go to differnt 'IContextReader'
                 *          subclasses. The START goes to the parent (so it can create the right type), and the EndTag
                 *          goes to the created/pushed type, so it can close itself and pop back to the parent context.
                 */
                class   ObjectReader::Context {
                public:
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
                public:
                    bool    fTraceThisReader { false };       // very noisy - off by default even for tracemode
                    nonvirtual  String TraceLeader_ () const;
#endif

                public:
                    Context () = default;
                    Context (const Context&) = delete;
                    Context& operator= (const Context&) = delete;

                public:
                    nonvirtual  void    Push (const shared_ptr<IContextReader>& elt);
                    nonvirtual  void    Pop ();

                public:
                    nonvirtual  shared_ptr<IContextReader>   GetTop () const;

                private:
                    vector<shared_ptr<IContextReader>> fStack_;

                private:
                    friend  class   ObjectReader;
                };


                class   ObjectReader::IConsumerDelegateToContext : public StructuredStreamEvents::IConsumer {
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
                 * BuiltinReader<> is not implemented for all types - just for the specialized ones listed below:
                 *      String
                 *      int
                 *      Time::DateTime
                 */
                template    <typename   T>
                class   BuiltinReader : public ObjectReader::IContextReader {
                public:
                    BuiltinReader (T* intoVal);

                private:
                    T* value_;

                public:
                    virtual void    HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader::Context& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader::Context& r) override;
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
                class   OptionalTypesReader : public ObjectReader::IContextReader {
                public:
                    OptionalTypesReader (Memory::Optional<T>* intoVal);

                private:
                    Memory::Optional<T>*    value_;
                    T                       proxyValue_;
                    ACTUAL_READER           actualReader_;  // this is why its crucial this partial specialization is only used on optional of types a real reader is available for

                public:
                    virtual void    HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader::Context& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader::Context& r) override;
                };


                /**
                 *  Push one of these Nodes onto the stack to handle 'reading' a node which is not to be read.
                 *  This is necessary to balance out the Start Tag / End Tag combinations.
                 */
                class   IgnoreNodeReader : public ObjectReader::IContextReader {
                public:
                    IgnoreNodeReader ();
                private:
                    int fDepth_;
                public:
                    virtual void    HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader::Context& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader::Context& r) override;
                };


                /**
                 *  Helper class for reading complex (structured) objects.
                 */
                template    <typename   T>
                class   ComplexObjectReader : public ObjectReader::IContextReader {
                protected:
                    ComplexObjectReader (T* vp);

                public:
                    T*  fValuePtr;

                public:
                    virtual void    HandleTextInside (ObjectReader::Context& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader::Context& r) override;
                protected:
                    nonvirtual  void    _PushNewObjPtr (ObjectReader::Context& r, const shared_ptr<IContextReader>& newlyAllocatedObject2Push);
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
                struct ListOfObjectReader: public ComplexObjectReader<vector<typename TRAITS::ElementType>> {
                private:
                    bool                            readingAT_;
                    typename TRAITS::ElementType    curTReading_;

                public:
                    ListOfObjectReader (vector<typename TRAITS::ElementType>* v);

                    virtual void HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void HandleEndTag (ObjectReader::Context& r) override;
                };


                /**
                 */
                void    ThrowUnRecognizedStartElt (const StructuredStreamEvents::Name& name);


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
