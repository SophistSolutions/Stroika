﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>
#include <type_traits>
#include <typeindex>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Configuration/TypeHints.h"
#include "Stroika/Foundation/Containers/Adapters/Adder.h"
#include "Stroika/Foundation/Containers/Bijection.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Containers/SortedCollection.h"
#include "Stroika/Foundation/Containers/SortedMapping.h"
#include "Stroika/Foundation/Containers/SortedSet.h"
#include "Stroika/Foundation/DataExchange/StructFieldMetaInfo.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Traversal/Range.h"

#include "IConsumer.h"
#include "Name.h"

/**
 *
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 *  TODO:
 *
 *      @todo   The SimpleReader_ private classes could all be replaced with use of AddCommonReader_Simple()
 *              But the simplereader_ is probably slightly more efficient. Decide if worht the difference or way to
 *              combine these better.
 *
 *      @todo   Extend ObjectReaderRegistry::StructIFNO – with function (like in mixin class) to read stuff –
 *              merge it with mixinhelper – and then use that in all places – class reader, mixin reader, and RepeatedEltReader.
 *
 o   MixinEltTraits could have overload of Name instead of lambda returning bool,
     one for case = Name, and either one for case where != Name, or maybe ANY wlidvard
     with interuendeirng we do first one first.

 --

 o   Maybe have MixinEltTraits param for pick subelt more like macro to offset of –
     soemthign where you pass in just T, eltName and it goes the reinterpret_cast for you.

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

 ----
 Includein headers example for using objectreaderresgiter

 #if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
 public:
 bool    fTraceThisReader { false };       // very noisy - off by default even for tracemode
 nonvirtual  String TraceLeader_ () const;
 #endif

----

 *      @todo   Make AddCommonType() - when passed in an optional<T> - Require that
 *              the type T is already in the registry (like with AddClass). To debug!
 *
 *      @todo   Review names: I don't think we use the term reader and readerfactory totally uniformly, and
 *              we also need to clearly document why/when we use one versus the other (type system stores factories
 *              because you encounter them in parsing, and need instances to pop on stack to read a particular element).
 *
 *      @todo   Need much improved docs
 *
 *      @todo   Get rid of or do differently the Run() methods - so can turn on/off Context debugging easier.
 *
 *      @todo   USE UnknownSubElementDisposition more thoroughly...
 *
 *      @todo   http://stroika-bugs.sophists.com/browse/STK-408 - cleanup template specializations
 *
 *
 *  \em Design Overview
 *      This module provides a set of classes to support reading objects from a SAX (event oriented) data source
 *      (https://en.wikipedia.org/wiki/Simple_API_for_XML)
 *
 *      With SAX parsing, you get a nested series of OPEN TAG and matching CLOSE TAG events, mimicking the structure of
 *      the XML structured document.
 *
 *      Typically, you will have data structures which roughly map (structurally similarly) to the structure in the XML document
 *      (if not, this module will probably not help you).
 *
 *      These classes allow you to easily define readers for particular C++ types that correspond to sections of XML (so series
 *      of open/close tag and data in between events).
 *
 *      Think of each @IElementConsumer subclass (reader) as an object that knows how to read a section of XML (series of open/close
 *      tag events). Predefined classes like @ClassReader<> and @ListOfObjectsReader<> already know how to read certain kinds of XML
 *      and populate likely container objects. You may need to write your own reader class occasionally, but often you can reuse these.
 *
 *      Then you must be are of how readers are managed. They are created and destroyed as smart_ptr<> objects. And since you often
 *      need to be able to create new ones based on context, they are managed through factories (which create them as the
 *      sax parse encounters particular open/close events).
 *
 *      You register (in a @Registry) the mapping between a particular type, and its reader factory.
 *
 *      Because this mapping is sometimes context sensative, Registries can be copied, and are read from a context object.
 *      So in a particular read context, you can replace a particular reader factory for a particular type. Or - within the
 *      ClassReader object, you can provide an optional replacement reader factory for that partiular data member.
 *      
 */

namespace Stroika::Foundation::DataExchange::StructuredStreamEvents::ObjectReader {

    using Characters::String;
    using Containers::Collection;
    using Containers::Mapping;
    using Containers::Sequence;
    using Containers::Set;

    /**
     */
#ifndef qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
#define qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing qDebug
#endif

    /**
     */
    enum class UnknownSubElementDisposition {
        eIgnore,
        eEndObject
    };

    class Context;
    class IElementConsumer;

    /**
     */
    template <typename T>
    using ReaderFromTStarFactory = function<shared_ptr<IElementConsumer> (T* destinationObject)>;

    /**
     *  We store in our database factories that read into a 'void*' that must be of the right type,
     *  but we use (at the last minute) the appropriate type. This is typesafe iff the readers do casts
     *  safely (and all the readers we provide do).
     */
    using ReaderFromVoidStarFactory = ReaderFromTStarFactory<void>;

    /**
     *  This is just for use the with the ObjectReaderRegistry::AddClass<> (and related) methods, to describe a user-defined type (CLASS).
     */
    struct StructFieldInfo {
        Name                                fSerializedFieldName;
        StructFieldMetaInfo                 fFieldMetaInfo;
        optional<ReaderFromVoidStarFactory> fOverrideTypeMapper;

        /**
         */
        StructFieldInfo (const Name& serializedFieldName, const StructFieldMetaInfo& fieldMetaInfo,
                         const optional<ReaderFromVoidStarFactory>& typeMapper = nullopt);
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
     *  \note UNLIKE ObjectVariantReader - the constructor of Registry contains no default readers.
     *
     *  \par Example Usage
     *      \code
     *          struct  Person_ {
     *              String      firstName;
     *              String      lastName;
     *          };
     *          using   namespace   ObjectReader;
     *          Registry mapper;
     *          mapper.AddCommonType<String> ();
     *          mapper.AddClass<Person_> ({
     *              { Name { "FirstName" }, &Person_::firstName },
     *              { Name { "LastName" }, &Person_::lastName },
     *          });
     *          Person_ p;
     *          IConsumerDelegateToContext tmpCtx1 (mapper, make_shared<ReadDownToReader> (mapper.MakeContextReader (&p)));
     *          XML::SAXParse (mkdata_ (), tmpCtx1);
     *      \endcode
     *
     *  \par Example Usage2
     *      @see T3_SAXObjectReader_ReadDown2Sample_ in RegressionTest 'Foundation::DataExchangeFormat::XML::SaxParser'
     *
     *      \code
     *          .. start with the types and mapper from Example 1, and add
     *          Sequence<Person_> people;
     *          mapper.AddCommonType<Sequence<Person_>> (Name{"WithWhom"});
     *          IConsumerDelegateToContext tmpCtx2 { mapper, make_shared<ReadDownToReader> (newRegistry.MakeContextReader (&people)) };
     *          XML::SAXParse (mkdata_ (), tmp);
     *      \endcode
     */
    class Registry {
    public:
        /**
         *  \note UNLIKE ObjectVariantReader - the constructor of Registry contains no readers by default.
         */
        Registry ()                = default;
        Registry (const Registry&) = default;

    public:
        nonvirtual Registry& operator= (const Registry&) = default;

    public:
        /**
         */
        nonvirtual void Add (type_index forType, const ReaderFromVoidStarFactory& readerFactory);
        template <typename T>
        nonvirtual void Add (const ReaderFromTStarFactory<T>& readerFactory);

    public:
        /**
         *  Shortcut for Add (MakeCommonReader<T> ());
         *
         *  So - this is supported for any type for which (@see MakeCommonReader) is supported.
         *
         *  Note this this is not needed (because it's done by default), but is supported,
         *  for the builtin types.
         */
        template <typename T, typename... ARGS>
        nonvirtual void AddCommonType (ARGS&&... args);

    public:
        nonvirtual optional<ReaderFromVoidStarFactory> Lookup (type_index t) const;

    public:
        /**
         *  This creates serializers for many common types.
         *      o   String
         *      o   Time::DateTime
         *      o   Time::Duration
         *      o   Mapping<Key,Value>
         *      o   Optional<T>
         *      o   Collection<T>
         *      o   Sequence<T>
         *      o   vector<T>
         *      o   enum types (with eSTART/eEND @see Stroika_Define_Enum_Bounds for bounds checking)
         *      o   optional<T> for any type T already in the registry.
         *
         *  This assumes the template parameters for the above objects are also already defined (mostly 'T' above).
         *
         *  This function also works (but is generally unneeded for) any of the types defined in
         *  @see ResetToDefaultTypeRegistry () (int, short, String, etc).
         *
         *  Note - all these de-serializers will throw BadDataFormat exceptions if the data somehow doesn't
         *  fit what the deserailizer expects.
         *
         *  @see MakeClassReader
         *  @see MakeCommonReader_EnumAsInt
         *  @see MakeCommonReader_NamedEnumerations
         */
        template <typename T, typename... ARGS>
        static ReaderFromVoidStarFactory MakeCommonReader (ARGS&&... args);

    public:
        /**
         *  \req    AddCommonReader_Class<> requires that each field data type already be pre-loaded into the
         *          ObjectReader::Registry. To avoid this requirement, you an use MakeClassReader
         *          directly, but if this type is absent when you call AddClass<> - its most likely
         *          a bug.
         */
        template <typename CLASS>
        nonvirtual void AddCommonReader_Class (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions);

    public:
        /**
         *  Create a class factory for a class with the given fields. This does't check (because its static) if the types of the elements in the
         *  fields are part of the mapper.
         *
         *  @see AddClass<>
         */
        template <typename CLASS>
        static ReaderFromVoidStarFactory MakeClassReader (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions);

    public:
        /**
         *  @see MakeCommonReader
         *  @see MakeCommonReader_EnumAsInt
         */
        template <typename ENUM_TYPE>
        static ReaderFromVoidStarFactory MakeCommonReader_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap);
        template <typename ENUM_TYPE>
        static ReaderFromVoidStarFactory
        MakeCommonReader_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap = Configuration::DefaultNames<ENUM_TYPE>::k);

    public:
        /**
         *  \brief Add reader for any type that is stringish: provide convert function from string to T; Simple wrapper on Add<> and MakeCommonReader_Simple<>
         *
         *  \par Example Usage
         *      \code
         *          ...
         *          enum class GenderType_ {
         *              Male,
         *              Female,
         *              Stroika_Define_Enum_Bounds (Male, Female)
         *          };
         *          registry.AddCommonReader_NamedEnumerations<GenderType_> (Containers::Bijection<GenderType_, String>{
         *              pair<GenderType_, String>{GenderType_::Male, "Male"},
         *              pair<GenderType_, String>{GenderType_::Female, "Female"},
         *          });
         *      \endcode
         *
         *  \note if cannot convert, 'converterFromString2T' can/must throw (typically some sort of format exception)
         */
        template <typename ENUM_TYPE>
        nonvirtual void AddCommonReader_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap);
        template <typename ENUM_TYPE>
        nonvirtual void AddCommonReader_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap = Configuration::DefaultNames<ENUM_TYPE>::k);

    public:
        /**
         *  @see MakeCommonReader
         *  @see MakeCommonReader_NamedEnumerations
         */
        template <typename ENUM_TYPE>
        static ReaderFromVoidStarFactory MakeCommonReader_EnumAsInt ();

    public:
        /**
         *  \brief Add reader for any type that is stringish: provide convert function from string to T; Simple wrapper on Add<> and MakeCommonReader_Simple<>
         *
         *  \par Example Usage
         *      \code
         *  \par Example Usage
         *      \code
         *          ...
         *          enum class GenderType_ {
         *              Male,
         *              Female,
         *              Stroika_Define_Enum_Bounds (Male, Female)
         *          };
         *          registry.AddCommonReader_EnumAsInt<GenderType_> ();
         *      \endcode
         */
        template <typename ENUM_TYPE>
        nonvirtual void AddCommonReader_EnumAsInt ();

    public:
        /**
         *  Used to create a reader which maps a string contained in an addressible element (element or attribute) to a value.
         *  Used anywhere you could have used a 'String' reader.
         *
         *  \note This also ILLUSTATES how easy it is to create readers for more complex types. Just create your own function
         *        like this (clone code) that contains a private class that accomulates data for the stages of SAX parsing and produces
         *        the right type at the end
         *
         *  \par Example Usage
         *      \code
         *          ...
         *          struct GenderType_ {
         *              String fRep;
         *          };
         *          ...
         *          registry.Add<GenderType_> (registry.MakeCommonReader_Simple< GenderType_> ([](String s) ->GenderType_ { GenderType_ result; result.fRep = s; return result; }));
         *
         *      \endcode
         *
         *  \note if cannot convert, 'converterFromString2T' can/must throw (typically some sort of format exception)
         *
         *  @see MakeCommonReader
         *  @see MakeCommonReader_EnumAsInt
         */
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_Simple (const function<T (String)>& converterFromString2T);

    public:
        /**
         *  \brief Add reader for any type that is stringish: provide convert function from string to T; Simple wrapper on Add<> and MakeCommonReader_Simple<>
         *
         *  \par Example Usage
         *      \code
         *          ...
         *          struct GenderType_ {
         *              String fRep;
         *          };
         *          ...
         *          registry.AddCommonReader_Simple<GenderType_> ([](String s) ->GenderType_ { GenderType_ result; result.fRep = s; return result; });
         *
         *      \endcode
         *
         *  \note if cannot convert, 'converterFromString2T' can/must throw (typically some sort of format exception)
         */
        template <typename T>
        nonvirtual void AddCommonReader_Simple (const function<T (String)>& converterFromString2T);

    public:
        /**
         *  It's pretty easy to code up a 'reader' class - just a simple subclass of IElementConsumer.
         *  There are lots of examples in this module.
         *
         *  But the APIs in the ObjectReader::Registry require factories, and this utility function constructs
         *  a factory from the given READER class template parameter.
         */
        template <typename T, typename READER, typename... ARGS>
        static auto ConvertReaderToFactory (ARGS&&... args) -> ReaderFromVoidStarFactory;

    public:
        /**
         *  This is generally just called inside of another composite reader to read sub-objects.
         */
        nonvirtual shared_ptr<IElementConsumer> MakeContextReader (type_index ti, void* destinationObject) const;
        template <typename T>
        nonvirtual shared_ptr<IElementConsumer> MakeContextReader (T* destinationObject) const;

    private:
        template <typename T>
        class SimpleReader_;
        template <typename T>
        class OldOptionalTypesReader_;

    private:
        template <typename T>
        static ReaderFromVoidStarFactory cvtFactory_ (const ReaderFromTStarFactory<T>& tf);

    private:
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_SimpleReader_ ();

    private:
        static ReaderFromVoidStarFactory MakeCommonReader_ (const String*);
        static ReaderFromVoidStarFactory MakeCommonReader_ (const IO::Network::URI*);
        static ReaderFromVoidStarFactory MakeCommonReader_ (const Time::DateTime*);
        static ReaderFromVoidStarFactory MakeCommonReader_ (const Time::Duration*);
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_ (const T*)
            requires (is_enum_v<T>);
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_ (const T*)
            requires (is_trivially_copyable_v<T> and is_standard_layout_v<T> and not is_enum_v<T>);
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_ (const optional<T>*);
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_ (const vector<T>*);
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_ (const vector<T>*, const Name& name);
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_ (const Collection<T>*);
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_ (const Collection<T>*, const Name& name);
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_ (const Sequence<T>*);
        template <typename T>
        static ReaderFromVoidStarFactory MakeCommonReader_ (const Sequence<T>*, const Name& name);

    private:
        Mapping<type_index, ReaderFromVoidStarFactory> fFactories_;
    };

    template <>
    ReaderFromVoidStarFactory Registry::MakeCommonReader_ (const optional<Time::DateTime>*);

    /**
     *  Subclasses of this abstract class are responsible for consuming data at a given level
     *  of the SAX 'tree', and transform it into a related object.
     *
     *  These are generally 'readers' of input XML, and writers of inflated objects.
     */
    class IElementConsumer : public enable_shared_from_this<IElementConsumer> {
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
        virtual void Activated (Context& r);

    public:
        /**
         */
        virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name);

    public:
        /**
         *  NOTE that this may be called zero or more times between Activated and Deactivating, and generally
         *  all the text should be accumulated. Callers may split incoming text into arbitrarily many pieces.
         */
        virtual void HandleTextInside (const String& text);

    public:
        /**
         * About to pop from the stack.
         *
         * @see Activated
         */
        virtual void Deactivating ();

    public:
        /**
         *  Helper to convert a reader to a factory (something that creates the reader).
         *
         *  Equivalent to
         *      Registry::ConvertReaderToFactory<TARGET_TYPE, READER> ()
         *
         *  Typically shadowed in subclasses (actual readers) to fill in default parameters.
         */
        template <typename TARGET_TYPE, typename READER, typename... ARGS>
        static ReaderFromVoidStarFactory AsFactory (ARGS&&... args);
    };

    /**
     *  This concrete class is used to capture the state of an ongoing StructuredStreamParse/transformation. Logically, it
     *  maintains the 'stack' you would have in constructing a recursive decent object mapping.
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
     *          This means that the start and end tags for a given pair, go to different 'IElementConsumer'
     *          subclasses. The START goes to the parent (so it can create the right type), and the EndTag
     *          goes to the created/pushed type, so it can close itself and pop back to the parent context.
     *
     *  \note   Debugging Note
     *          It's often helpful to turn on fTraceThisReader while debugging SAX parsing, to see why something
     *          isn't being read.
     */
    class Context {
    public:
#if qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
    public:
        bool              fTraceThisReader{false}; // very noisy - off by default even for tracemode
        nonvirtual String TraceLeader_ () const;
#endif

    public:
        /**
         */
        Context (const Registry& registry);
        Context (const Registry& registry, const shared_ptr<IElementConsumer>& initialTop);
        Context (const Context&) = delete;
        Context (Context&& from) noexcept;
        nonvirtual Context& operator= (const Context&) = delete;

    public:
        nonvirtual const Registry& GetObjectReaderRegistry () const;

    public:
        nonvirtual void Push (const shared_ptr<IElementConsumer>& elt);

    public:
        nonvirtual void Pop ();

    public:
        nonvirtual shared_ptr<IElementConsumer> GetTop () const;

    public:
        nonvirtual bool empty () const;

    private:
        const Registry&                      fObjectReaderRegistry_;
        vector<shared_ptr<IElementConsumer>> fStack_;

    private:
        friend class ObjectReader;
    };

    /**
     *  @see Registry for examples of use
     */
    class IConsumerDelegateToContext : public StructuredStreamEvents::IConsumer {
    public:
        IConsumerDelegateToContext () = delete;
        IConsumerDelegateToContext (Context&& r);
        IConsumerDelegateToContext (const Registry& registry);
        IConsumerDelegateToContext (const Registry& registry, const shared_ptr<IElementConsumer>& initialTop);
        IConsumerDelegateToContext (const IConsumerDelegateToContext& r)          = delete;
        IConsumerDelegateToContext& operator= (const IConsumerDelegateToContext&) = delete;

    public:
        virtual void StartElement (const Name& name, const Mapping<Name, String>& attributes) override;
        virtual void EndElement (const Name& name) override;
        virtual void TextInsideElement (const String& text) override;

    public:
        Context fContext;
    };

    /**
     *  Push one of these Nodes onto the stack to handle 'reading' a node which is not to be read.
     *  This is necessary to balance out the Start Tag / End Tag combinations.
     */
    class IgnoreNodeReader : public IElementConsumer {
    public:
        /**
         *  Note the IGNORED* overload is so this can be used generically with a factory (though no clear reason
         *  I can think of why you would want to).
         */
        IgnoreNodeReader () = default;

    public:
        virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override;

    public:
        /**
         *  Helper to convert a reader to a factory (something that creates the reader).
         */
        static ReaderFromVoidStarFactory AsFactory ();
    };

    /**
     */
    template <typename T>
    class ClassReader : public IElementConsumer {
    public:
        ClassReader (const Traversal::Iterable<StructFieldInfo>& fieldDescriptions, T* vp);

    public:
        virtual void                         Activated (Context& r) override;
        virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override;
        virtual void                         HandleTextInside (const String& text) override;
        virtual void                         Deactivating () override;

    public:
        /**
         *  Helper to convert a reader to a factory (something that creates the reader).
         */
        static ReaderFromVoidStarFactory AsFactory ();

    private:
        nonvirtual ReaderFromVoidStarFactory LookupFactoryForName_ (const Name& name) const;

    private:
        Traversal::Iterable<StructFieldInfo> fFieldDescriptions_;
        Context*                             fActiveContext_{};
        T*                                   fValuePtr_{};
        Mapping<Name, StructFieldMetaInfo>   fFieldNameToTypeMap_;
        optional<StructFieldMetaInfo>        fValueFieldMetaInfo_;
        shared_ptr<IElementConsumer>         fValueFieldConsumer_;
        bool                                 fThrowOnUnrecongizedelts_{false}; // else ignore
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
    class ReadDownToReader : public IElementConsumer {
    public:
        ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader);
        ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff);
        ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& contextTag, const Name& tagToHandOff);
        ReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& contextTag1, const Name& contextTag2, const Name& tagToHandOff);

    public:
        virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override;

    public:
        /**
         *  Helper to convert a reader to a factory (something that creates the reader).
         */
        static ReaderFromVoidStarFactory AsFactory (const ReaderFromVoidStarFactory& theUseReader);
        static ReaderFromVoidStarFactory AsFactory (const ReaderFromVoidStarFactory& theUseReader, const Name& tagToHandOff);

    private:
        shared_ptr<IElementConsumer> fReader2Delegate2_;
        optional<Name>               fTagToHandOff_;
    };

    /**
     *  ListOfObjectReader<> can read a container (vector-like) of elements. You can optionally specify
     *  the name of each element, or omit that, to assume every sub-element is of the 'T' type.
     *
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
    template <typename CONTAINER_OF_T>
    class ListOfObjectsReader : public IElementConsumer {
    public:
        using ElementType = typename CONTAINER_OF_T::value_type;

    public:
        ListOfObjectsReader (CONTAINER_OF_T* v);
        ListOfObjectsReader (CONTAINER_OF_T* v, const Name& memberElementName);

    public:
        virtual void                         Activated (Context& r) override;
        virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override;
        virtual void                         Deactivating () override;

    public:
        /**
         *  Helper to convert a reader to a factory (something that creates the reader).
         */
        static ReaderFromVoidStarFactory AsFactory ();
        static ReaderFromVoidStarFactory AsFactory (const Name& memberElementName);

    private:
        CONTAINER_OF_T* fValuePtr_{};
        Context*        fActiveContext_{};
        optional<Name>  fMemberElementName_;
        bool            fThrowOnUnrecongizedelts_{false};
    };

    /**
     */
    template <typename T>
    struct OptionalTypesReader_DefaultTraits {
        static inline const T kDefaultValue = T{};
    };

    /**
     *  \brief Reader for optional<X> types - typically just use AddCommonType<optional<T>> () - but use this if you need to parameterize
     *
     *  OptionalTypesReader supports reads of optional types. This will work - for any types for
     *  which SimpleReader<T> is implemented.
     *
     *  Note - this ALWAYS produces a result. Its only called when the element in quesiton has
     *  already occurred. The reason for Optional<> part is because the caller had an optional
     *  element which might never have triggered the invocation of this class.
     */
    template <typename T, typename TRAITS = OptionalTypesReader_DefaultTraits<T>>
    class OptionalTypesReader : public IElementConsumer {
    public:
        OptionalTypesReader (optional<T>* intoVal);

    public:
        virtual void                         Activated (Context& r) override;
        virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override;
        virtual void                         HandleTextInside (const String& text) override;
        virtual void                         Deactivating () override;

    public:
        /**
         *  Helper to convert a reader to a factory (something that creates the reader).
         */
        static ReaderFromVoidStarFactory AsFactory ();

    private:
        optional<T>*                 fValue_{};
        T                            fProxyValue_{TRAITS::kDefaultValue};
        shared_ptr<IElementConsumer> fActualReader_{};
    };

    /**
     */
    template <typename T>
    class MixinReader : public IElementConsumer {
    public:
        struct MixinEltTraits {
            ReaderFromVoidStarFactory         fReaderFactory;
            function<bool (const Name& name)> fReadsName = [] (const Name& name) { return true; };
            function<bool ()>                 fReadsText = [] () { return true; };
            function<byte*(T*)>               fAddressOfSubElementFetcher;

            static const function<byte*(T*)> kDefaultAddressOfSubElementFetcher;

            MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory,
                            const function<byte*(T*)>&       addressOfSubEltFetcher = kDefaultAddressOfSubElementFetcher);
            MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool (const Name& name)>& readsName,
                            const function<byte*(T*)>& addressOfSubEltFetcher = kDefaultAddressOfSubElementFetcher);
            MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool ()>& readsText,
                            const function<byte*(T*)>& addressOfSubEltFetcher = kDefaultAddressOfSubElementFetcher);
            MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool (const Name& name)>& readsName,
                            const function<bool ()>& readsText, const function<byte*(T*)>& addressOfSubEltFetcher = kDefaultAddressOfSubElementFetcher);
        };

    public:
        /**
         */
        MixinReader (T* vp, const Traversal::Iterable<MixinEltTraits>& mixins);

    public:
        virtual void                         Activated (Context& r) override;
        virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override;
        virtual void                         HandleTextInside (const String& text) override;
        virtual void                         Deactivating () override;

    public:
        /**
         *  Helper to convert a reader to a factory (something that creates the reader).
         */
        static ReaderFromVoidStarFactory AsFactory (const Traversal::Iterable<MixinEltTraits>& mixins);

    private:
        Context*                               fActiveContext_{};
        T*                                     fValuePtr_{};
        Sequence<MixinEltTraits>               fMixins_;
        Sequence<shared_ptr<IElementConsumer>> fMixinReaders_;
        Set<shared_ptr<IElementConsumer>>      fActivatedReaders_;
    };

    /**
     *  \note   This is a public class instead of accessed via MakeCommonReader<T> since its very common that it will need
     *          extra parameters (specified through the CTOR/and can be specified through the AsFactory method) to specify the name elements
     *          to use for bounds attributes.
     */
    template <typename T>
    class RangeReader : public IElementConsumer {
    public:
        static const inline pair<Name, Name> kDefaultBoundsNames{Name{"LowerBound"sv, Name::eAttribute}, Name{"UpperBound"sv, Name::eAttribute}};

    public:
        /**
         */
        RangeReader (T* intoVal, const pair<Name, Name>& pairNames = kDefaultBoundsNames);

    public:
        virtual void                         Activated (Context& r) override;
        virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override;
        virtual void                         HandleTextInside (const String& text) override;
        virtual void                         Deactivating () override;

    public:
        /**
         *  Helper to convert a reader to a factory (something that creates the reader).
         */
        static ReaderFromVoidStarFactory AsFactory (const pair<Name, Name>& pairNames = kDefaultBoundsNames);

    private:
        using range_value_type_ = typename T::value_type;
        struct RangeData_ {
            range_value_type_ fLowerBound{};
            range_value_type_ fUpperBound{};
        };
        pair<Name, Name>             fPairNames;
        T*                           fValue_{};
        RangeData_                   fProxyValue_{};
        shared_ptr<IElementConsumer> fActualReader_{};
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
     *
     *      using   namespace   ObjectReader;
     *      Registry registry;
     *      registry.AddCommonType<String> ();
     *      registry.AddClass<Person_> ({
     *          { Name { "FirstName" }, &Person_::firstName },
     *          { Name { "LastName" }, &Person_::lastName },
     *      });
     *      registry.AddCommonType<vector<Person_>> ();
     *      registry.Add<vector<Person_>> (Registry::ConvertReaderToFactory <vector<Person_>, RepeatedElementReader<vector<Person_>>> ());
     *      registry.AddClass<Address_> ({
     *          { Name { "city" }, &Address_::city },
     *          { Name { "state" }, &Address_::state },
     *      });
     *      registry.Add<vector<Address_>> (Registry::ConvertReaderToFactory <vector<Address_>, RepeatedElementReader<vector<Address_>>> ());
     *      registry.AddClass<Data_> ({
     *          { Name { "person" }, &Data_::people },
     *          { Name { "address" }, &Data_::addresses },
     *      });
     *      Data_   data;
     *      Registry::IConsumerDelegateToContext ctx { registry, make_shared<ReadDownToReader> (registry.MakeContextReader (&data)) };
     *      XML::SAXParse (srcXMLStream, ctx);
     *      \endcode
     *
     *  \note   This is like @see ListOfElementsReader, except that it starts on the elements of the array itself, as opposed
     *          to just above.
     */
    template <typename CONTAINER_OF_T, typename CONTAINER_ADAPTER_ADDER = Containers::Adapters::Adder<CONTAINER_OF_T>>
    struct RepeatedElementReader_DefaultTraits {
        using ContainerAdapterAdder                  = CONTAINER_ADAPTER_ADDER;
        using TValueType                             = typename CONTAINER_OF_T::value_type;
        static inline const TValueType kDefaultValue = TValueType{};
    };
    template <typename T, typename TRAITS = RepeatedElementReader_DefaultTraits<T>>
    class RepeatedElementReader : public IElementConsumer {
    public:
        using ContainerType = T;
        using ElementType   = typename ContainerType::value_type;

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
        virtual void                         Activated (Context& r) override;
        virtual shared_ptr<IElementConsumer> HandleChildStart (const Name& name) override;
        virtual void                         HandleTextInside (const String& text) override;
        virtual void                         Deactivating () override;

    public:
        /**
         *  Helper to convert a reader to a factory (something that creates the reader).
         */
        static ReaderFromVoidStarFactory AsFactory ();
        static ReaderFromVoidStarFactory AsFactory (const Name& readonlyThisName, const ReaderFromVoidStarFactory& actualElementFactory);
        static ReaderFromVoidStarFactory AsFactory (const ReaderFromVoidStarFactory& actualElementFactory);
        static ReaderFromVoidStarFactory AsFactory (const Name& readonlyThisName);

    private:
        ContainerType*                      fValuePtr_{};
        optional<ReaderFromVoidStarFactory> fReaderRactory_{}; // if missing, use Context::GetObjectReaderRegistry ().MakeContextReader ()
        function<bool (Name)>               fReadThisName_{[] ([[maybe_unused]] const Name& n) { return true; }};
        ElementType                         fProxyValue_{TRAITS::kDefaultValue};
        shared_ptr<IElementConsumer>        fActiveSubReader_{};
    };

    /**
     */
    [[noreturn]] void ThrowUnRecognizedStartElt (const Name& name);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectReader.inl"

#endif /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_h_*/
