/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Property_h_
#define _Stroika_Foundation_Common_Property_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <concepts>
#include <forward_list>
#include <functional>
#include <mutex>
#include <optional>

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/TypeHints.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 * 
 *  Notes:
 *      I've long thought about doing something like this, but only recently got frustrated with
 *      the ugliness of tons of getters and setters. So I googled for solutions, and was surprised
 *      there was so little traction behind adding this feature to C++ (from C#).
 * 
 *      These links give a few hints about discussions of adding this feature to c++:
 *          *   http://www.open-std.org/Jtc1/sc22/wg21/docs/papers/2004/n1615.pdf
 *          *   https://stackoverflow.com/questions/40559828/c-properties-with-setter-and-getter
 *          *   https://www.reddit.com/r/cpp/comments/61m9r1/what_do_you_think_about_properties_in_the_c/
 *
 *  TODO:
 *      @todo   Consider redoing so type of Getter/Setter embedded into the class, so it can 
 *              be saved RAW, and not converted to a function pointer. Trickier to do construction,
 *              but probably possible with template guides. But only bother if there is a clear
 *              performance benefit, because this is simpler.
 * 
 */

namespace Stroika::Foundation::Common {

    /**
     *  There are two good ways to access the address of the outer object from the property.
     *  One is clearly and straight-forwardly portable safe C++, and the other is more efficient
     *  and generally safe. Users of this class can choose on their own which style to use.
     *  But this #define provides a configurable choice between the two approaches if you use it - and Stroika does internally -
     *  between those approaches so you can change it through configuration.
     *  To use this automatically use qStroika_Foundation_Common_Property_EmbedThisInProperties, and qStroika_Foundation_Common_Property_OuterObjPtr
     *  as below in the example documentation for Property constructor
     * 
     *  \see For more information on this topic, see 
     *       https://stackoverflow.com/questions/65940393/c-why-the-restriction-on-offsetof-for-non-standard-layout-objects-or-how-t?noredirect=1#comment116600269_65940393
     *
     *  \see http://stroika-bugs.sophists.com/browse/STK-939 - probably LOSE This define and REQUIRE its always zero, so can COPY properties safely.
     */
#ifndef qStroika_Foundation_Common_Property_EmbedThisInProperties
#define qStroika_Foundation_Common_Property_EmbedThisInProperties 0
#endif

#if qStroika_Foundation_Common_Property_EmbedThisInProperties
    /**
     * @see qStroika_Foundation_Common_Property_EmbedThisInProperties
     */
#define qStroika_Foundation_Common_Property_ExtraCaptureStuff this
    /**
     * @see qStroika_Foundation_Common_Property_EmbedThisInProperties
     */
#define qStroika_Foundation_Common_Property_OuterObjPtr(property, propertyPtrToMember) this
#else
    /**
     * @see qStroika_Foundation_Common_Property_EmbedThisInProperties
     */
#define qStroika_Foundation_Common_Property_ExtraCaptureStuff
    /**
     * @see qStroika_Foundation_Common_Property_EmbedThisInProperties
     */
#define qStroika_Foundation_Common_Property_OuterObjPtr(property, propertyPtrToMember)                                                     \
    Memory::GetObjectOwningField (property, propertyPtrToMember)
#endif

    struct PropertyCommon {
        /**
         *  read-only Properties (or the read method of a normal Property) can be used to 'simulate' read-write access (by having it return a mutable reference)
         *  This flag tells is that is going on, so we can make our accessor methods non-const in that case.
         * 
         *  \note This is SUBTLE but important for 'constness' to work properly.
         *        if we use
         *           struct OwningObject {
         *              ReadOnlyProperty<FOO&> p;
         *        then we want code:
         *              OwningObject& o = ...;
         *              const OwningObject& co = o;
         *              o.p().constOrNonConstMethod();  // OK
         *              co.p().constMethod (); // OK
         *              co.p().nonConstMethod ();    // should fail to compile
         * 
         *      kIsMutatableType, plus appropriate requires's on the various accessor methods are designed to accomplish that.
         */
        template <typename TT>
        static constexpr bool kIsMutatableType = not is_const_v<remove_reference_t<TT>> and is_reference_v<TT>;

        enum class PropertyChangedEventResultType {
            eSilentlyCutOffProcessing,
            eContinueProcessing,
        };
    };

    template <typename T>
    concept IPropertyMutatable = PropertyCommon::kIsMutatableType<T>;

    /**
     *  Implement C#-like syntax for read-only properties (syntactically like data members but backed by a getter function)
     *      \note   Typically not used - use Property
     *      \note   ANYHOW - see @Property for design overview
     *
     *  \note   We allow T to be of REFERENCE type for ReadOnlyProperties. Not sure its a good idea to be
     *          returning a const T& from a property, but it works/is allowed (for now).
     * 
     *          Note - unlike with functions - you cannot overload properties, making one return const T& and one return T&
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md">SAME AS T/GETTER - all methods have exactly the thread safety of the underlying GETTER</a>
     */
    template <typename T>
    class ReadOnlyProperty {
    public:
        /**
         */
        using value_type = T;

    public:
        /**
         *  ReadOnlyProperty are NOT movable, nor copy constructible: the data doesn't logically exist in the property itself,
         *  but in its relationship to some parent object; if it were copied, it might be copied TO some place that didn't
         *  have an appropriate enclosing object.
         */
        ReadOnlyProperty ()                        = delete;
        ReadOnlyProperty (const ReadOnlyProperty&) = delete;
        ReadOnlyProperty (ReadOnlyProperty&&)      = delete;
        template <qCompilerAndStdLib_RequiresNotMatchXXXDefined_1_BWA (invocable<const ReadOnlyProperty<T>*>) G>
        constexpr ReadOnlyProperty (G getter)
            qCompilerAndStdLib_RequiresNotMatchXXXDefined_2_BWA (requires (convertible_to<invoke_result_t<G, const ReadOnlyProperty<T>*>, T>));

    public:
        nonvirtual ReadOnlyProperty& operator= (const ReadOnlyProperty&)  = delete;
        nonvirtual ReadOnlyProperty& operator= (const ReadOnlyProperty&&) = delete;

    public:
        /**
         *  Returns the value of the given property T (by calling the underlying 'getter' for the property.
         *  This is a non-const method if PropertyCommon::kIsMutatableType<T>, and otherwise a const method.
         */
        nonvirtual T Get () const
            requires (not IPropertyMutatable<T>);
        nonvirtual T Get ()
            requires (IPropertyMutatable<T>);

    public:
        /**
         *  A ReadOnlyProperty can be automatically converted to its underlying base type.
         *  Due to how conversion operators work, this won't always be helpful (like with overloading
         *  or multiple levels of conversions, for example with optional<int> compare with int property).
         *  But when it works (80% of the time) - its helpful.
         * 
         *  When it doesn't work, simply throw in '()' - to use the 'operator()' call, or call Get()
         *  if you prefer that syntax.
         * 
         *  This is a non-const method if PropertyCommon::kIsMutatableType<T>, and otherwise a const method.
         */
        nonvirtual operator const T () const
            requires (not IPropertyMutatable<T>);
        nonvirtual operator T ()
            requires (IPropertyMutatable<T>);

    public:
        /**
         *  This works 100% of the time. Just use the function syntax, and you get back a copy of the desired
         *  underlying type.
         *
         *  \par Example Usage
         *      \code
         *          namespace PredefinedInternetMediaType {  const inline Common::VirtualConstant<InternetMediaType> kPNG...
         *
         *          bool checkIsImage1 = PredefinedInternetMediaType::kPNG().IsImageFormat ();
         *      \endcode
         * 
         *  This is a non-const method if PropertyCommon::kIsMutatableType<T>, and otherwise a const method.
         */
        nonvirtual const T operator() () const
            requires (not IPropertyMutatable<T>);
        nonvirtual T operator() ()
            requires (IPropertyMutatable<T>);

    private:
        const function<T (const ReadOnlyProperty*)> fGetter_;
    };

    /**
     *  Implement C#-like syntax for write-only properties (syntactically like data members but backed by a setter function)
     *      \note   Typically not used - use Property
     *      \note   ANYHOW - see @Property for design overview
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md">SAME AS T/SETTER - all methods have exactly the thread safety of the underlying SETTER</a>
     */
    template <typename T>
    class WriteOnlyProperty {
    public:
        /**
         */
        using value_type = T;

    public:
        /**
         *  WriteOnlyProperty are NOT movable, nor copy constructible: the data doesn't logically exist in the property itself,
         *  but in its relationship to some parent object; if it were copied, it might be copied TO some place that didn't
         *  have an appropriate enclosing object.
         */
        WriteOnlyProperty ()                         = delete;
        WriteOnlyProperty (const WriteOnlyProperty&) = delete;
        WriteOnlyProperty (WriteOnlyProperty&&)      = delete;

        template <qCompilerAndStdLib_RequiresNotMatchXXXDefined_1_BWA (invocable<WriteOnlyProperty<T>*, T>) S>
        constexpr WriteOnlyProperty (S setter);

    public:
        /**
         *  You can assign a value of the underlying type, but we do NOT support operator=(WriteOnlyProperty) because
         *  we cannot generically read from a write-only property to copy its value.
         */
        nonvirtual WriteOnlyProperty& operator= (Configuration::ArgByValueType<T> value);
        nonvirtual WriteOnlyProperty& operator= (const WriteOnlyProperty&)  = delete;
        nonvirtual WriteOnlyProperty& operator= (const WriteOnlyProperty&&) = delete;

    public:
        /**
         *  Alternate syntax for setting the property value.
         */
        nonvirtual void Set (Configuration::ArgByValueType<T> value);

    public:
        /**
         *  Alternate syntax for setting the property value.
         */
        nonvirtual void operator() (Configuration::ArgByValueType<T> value);

    private:
        const function<void (WriteOnlyProperty*, Configuration::ArgByValueType<T>)> fSetter_;
    };

    /**
     *  Implement C#-like syntax for properties (syntactically like data members but backed by a getter and a setter function).
     * 
     *  Properties are NOT movable, nor copy constructible: the data doesn't logically exist in the property itself,
     *  but in its relationship to some parent object; if it were copied, it might be copied TO some place that didn't
     *  have an appropriate enclosing object.
     *
     *  But, properties are ASSIGNABLE TO, because here we retain our getter/setter, and just treat assignment as copying the value.
     * 
     *  \note This has implications for classes that use Property objects: they will not be able to use X(const X&) = default;
     *        nor X(X&&) = default; Classes with properties can be copy constructed/move constructed, but just not automatically
     *        (memberwise). It wouldn't make sense to do memberwise because typically the property just 'knows how to find the data'
     *        stored elsewhere in the owning object.
     * 
     *  \note Though this looks syntactically much like using a direct data member, it will likely have some performance overhead
     *        due to forcing the use of a std::function wrapping a lambda for each access to the underlying object of type T.
     * 
     *  \note see base class ReadOnlyProperty and WriteOnlyProperty for details on APIs to read/write the underlying data.
     * 
     *  \note New Stroika NAMING CONVENTION introduced for instance variables of type Property<...>, which is to prepend a 'p'. This
     *        convention is to emphasize that these names DONT work QUITE like fields, but only somewhat like fields.
     * 
     *  \par Example Usage
     *      \code
     *          struct Headers {
     *          public:
     *              Headers ();
     *              Headers (const Headers& src);
     *              Headers (Headers&& src);
     *              nonvirtual Headers& operator= (const Headers& rhs) = default; // properties are assignable, so this is OK
     *              nonvirtual Headers& operator                       = (Headers&& rhs);
     *
     *              Property<unsigned int> contentLength1; // all 3 refer to the private fContentLength_ field
     *              Property<unsigned int> contentLength2;
     *              Property<unsigned int> contentLength3;
     *
     *          private:
     *              unsigned int fContentLength_{0};
     *          };
     *          Headers::Headers ()
     *              // Can implement getter/setters with this capture (wastes a bit of space)
     *              : contentLength1{
     *                    [this] ([[maybe_unused]] const auto* property) {
     *                        return fContentLength_;
     *                    },
     *                    [this] ([[maybe_unused]] auto* property, const auto& contentLength) {
     *                        fContentLength_ = contentLength;
     *                    }}
     *              // Can implement getter/setters with Memory::GetObjectOwningField - to save space, but counts on exact
     *              // storage layout and not totally legal with non- is_standard_layout<> - see Memory::GetObjectOwningField 
     *              , contentLength2{
     *                    [] (const auto* property) {
     *                        const Headers* headerObj = Memory::GetObjectOwningField (property, &Headers::contentLength2);
     *                        return headerObj->fContentLength_;
     *                    },
     *                    [] (auto* property, auto contentLength) {
     *                        Headers* headerObj         = Memory::GetObjectOwningField (property, &Headers::contentLength2);
     *                        headerObj->fContentLength_ = contentLength;
     *                    }}
     *              // Use stroika #define to decide which strategy to use 
     *              , contentLength3{
     *                    [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]]const auto* property) {
     *                        const Headers* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength3);
     *                        return thisObj->fContentLength_;
     *                    },
     *                    [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]]auto* property, auto contentLength) {
     *                        Headers* thisObj         = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::contentLength3);
     *                        thisObj->fContentLength_ = contentLength;
     *                    }}
     *          {
     *          }
     *          Headers::Headers (const Headers& src)
     *              : Headers{} // do default initialization of properties
     *          {
     *              // NOTE - cannot INITIALIZE properties with src.Properties values since they are not copy constructible
     *              // but they are assignable, so do that
     *              contentLength1 = src.contentLength1;
     *              contentLength2 = src.contentLength2;
     *              // COULD EITHER initialize fContentLength_ or contentLength1/contentLength2 - but no need to do both
     *          }
     *          Headers::Headers (Headers&& src)
     *              : Headers{} // do default initialization of properties
     *          {
     *              // NOTE - cannot MOVE properties with src.Properties values since they are not copy constructible
     *              // but they are assignable, so do that
     *              contentLength1 = src.contentLength1;
     *              contentLength2 = src.contentLength2;
     *              // COULD EITHER initialize fContentLength_ or contentLength1/contentLength2 - but no need to do both
     *          }
     *          Headers& Headers::operator= (Headers&& rhs)
     *          {
     *              // Could copy either properties or underlying field - no matter which
     *              fContentLength_ = rhs.fContentLength_;
     *              return *this;
     *          }
     *          //....
     * 
     *          Headers h;
     *          Assert (h.contentLength1 == 0);
     *          h.contentLength1 = 2;
     *          Assert (h.contentLength2 == 2);
     *          h.contentLength2 = 4;
     *          Assert (h.contentLength1 == 4);
     *          Headers h2 = h;
     *          Assert (h2.contentLength1 == 4);
     *          h.contentLength2 = 5;
     *          Assert (h.contentLength1 == 5);
     *          Assert (h2.contentLength1 == 4);
     *
     *      \endcode
     * 
     *  Another important scenario, is where you have a complex, and want to update it. Properties only allow for
     *  getters and setters, not updaters. The simple solution to this is to have the Property return a REFERENCE to the
     *  object in question.
     * 
     *  \par Example Usage (EXTENDING the Header example above)
     *      \code
     *          // HTTP 'Response' object
     *          class Response  {
     *          public:
     *              Response ()                    = delete;
     *              Response (const Response&)     = delete;
     *              Response (Response&& src);
     *              Response (const IO::Network::Socket::Ptr& s, const Streams::OutputStream::Ptr<byte>& outStream, const optional<InternetMediaType>& ct = nullopt);
     *              ~Response () = default;
     *              nonvirtual Response& operator= (const Response&) = delete;
     *          public:
     *              Common::ReadOnlyProperty<const IO::Network::HTTP::Headers&> headers;
     *          public:
     *              Common::Property<IO::Network::HTTP::Headers&> rwHeaders;
     *      ...
     *          // then the ABOVE checks/assignments become
     *          Response& r = get_from_somewhere();
     *          Assert (r.headers().contentLength1 == 0);
     *          r.rwHeaders().contentLength1 = 2;
     *          Assert (r.headers().contentLength2 == 2);
     *          h.contentLength2 = 4;
     *          Assert (r.headers().contentLength1 == 4);
     *          Headers h2 = r.headers;
     *          Assert (h2.contentLength1 == 4);
     *          r.rwHeaders().contentLength2 = 5;
     *          Assert (r.headers().contentLength1 == 5);
     *          Assert (h2.contentLength1 == 4);
     *      \endcode
     *
     *      \note when using Properties, its often helpful to combine (for thread safety checking) with 
     *            Debug::AssertExternallySynchronizedMutex and SetAssertExternallySynchronizedMutexContext ()
     * 
     *      \see See the example usage (above outlined) in Frameworks/WebServer/Request.h, Message.h, Response.h, and Foundation/IO/Network/HTTP/Headers
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md">SAME AS T/GETTER/SETTER - all methods have exactly the thread safety of the underlying GETTER/SETTER</a>
     */
    template <typename T>
    class Property : public ReadOnlyProperty<T>, public WriteOnlyProperty<remove_cvref_t<T>> {
    public:
        /**
         * \brief base_value_type is T the type declared, and decayed_value_type is similar, but with the references etc removed (so can be set/stored)
         */
        using base_value_type    = T;
        using decayed_value_type = remove_cvref_t<T>;

    public:
        /**
         *  Properties are NOT movable, nor copy constructible: the data doesn't logically exist in the property itself,
         *  but in its relationship to some parent object; if it were copied, it might be copied TO some place that didn't
         *  have an appropriate enclosing object.
         */
        Property ()                = delete;
        Property (const Property&) = delete;
        Property (Property&&)      = delete;
        template <invocable<const ReadOnlyProperty<T>*> G, invocable<WriteOnlyProperty<remove_cvref_t<T>>*, remove_cvref_t<T>> S>
        Property (G getter, S setter)
            requires (convertible_to<invoke_result_t<G, const ReadOnlyProperty<T>*>, T>);

    public:
        /**
         */
        nonvirtual Property& operator= (Configuration::ArgByValueType<decayed_value_type> value);
        nonvirtual Property& operator= (const Property& value);
        nonvirtual Property& operator= (const Property&&) = delete;

    public:
        template <typename TT>
        nonvirtual bool operator== (const TT& rhs) const;

    public:
        using ReadOnlyProperty<T>::operator();
        using ReadOnlyProperty<T>::Get;

    public:
        using WriteOnlyProperty<decayed_value_type>::Set;
        using WriteOnlyProperty<decayed_value_type>::operator();
    };

    /**
     *  \brief ConstantProperty is a 'virtual constant' - computed once at app startup, and thence forward just the cached constant value
     * 
     *  A ConstantProperty is just something that makes a function returning a value look
     *  like a constant, except that the ConstantProperty calls the construction function once, lazily;
     * 
     *  This can be useful if you have a logical constant, but one that depends on other 'logical constants'
     *  but want to avoid the nasty C++ deadly embrace of startup intermodule initialization.
     *
     *  In C++, you sometimes want to define a global constant, but run into problems because of order of initialization
     *  of global constants (across files). This class solves that problem by allowing you to manage the construction
     *  timing of your constant in a provided function, 
     * 
     *  This isn't guaranteed to always solve that problem no matter what, it allows you to declare a constant
     *  globally, and yet delay when its constructed until its first used (as opposed to some unspecified time
     *  before main).
     * 
     *  \note \alias Prior to Stroika v2.1b12, this was called VirtualConstant
     * 
     *  \note Unlike other properties (which are generally associated with some object)
     *        ConstantProperty objects generally are not associated with a particular object.
     * 
     *  \note Design Note: 
     *        Don't inherit from ReadOnlyProperty because if it already defining extra function object
     *        we don't need. API OK (though more general than needed).
     * 
     *  \par Example Usage
     *      \code
     *          inline String                 kXGetter_ () { return "X"; }
     *          const ConstantProperty<String> kX {kXGetter_};
     *          ...
     *          const String a = kX;
     *      \endcode
     *
     * 
&&&& OLD DOCS FROM VIRTUALCONSTNAT - SOME HELPFUL
     *  This doesn't work perfectly (e.g. see below about operator.) - but its pretty usable.
     * 
     *  \note The basic idea - any time you have a constant whose initializer depends on other constants and get into trouble
     *        with mutual constructor order issues before main, replace the dependent constant with a VirtualConstant
     *
     *  \par Example Usage
     *      \code
     *          const Execution::VirtualConstant<String> kX {[] () { return "6"; }};
     *          ...
     *          const String a = kX;
     *      \endcode
     *
     *  \note   it would be HIGHLY DESIRABLE if C++ allowed operator. overloading, as accessing one of these
     *          values without assigning to a temporary first - means that you cannot directly call its methods.
     *          That's a bit awkward.
     *
     *          So if you have a type T, with method m(), and variable of type T t.
     *          Your starter code might be:
     *              T   t;
     *              t.m ();
     *          When you replace 'T t' with
     *              VirtualConstant<T> t;
     *              you must call t().m();
     *          OR
     *              you must call t->m();
     *
     *  \note   C++ also only allows one level of automatic operator conversions, so things like comparing
     *          optional<T> {} == VirtualConstant<T,...> {} won't work. To workaround, simply
     *          apply () after the VirtualConstant<> instance.
     *
     *  TODO:
     *      @todo   Using optional<> and fValueInitialized_ (once_flag) is REDUNDANT, and wasteful of space.
     *              But re-using these APIs is tricky without keeping both 'flags'. Probably just store in byte array
     *              (re-implementing parts of Optional) - and do right magic destruct/etc...
     */
    template <typename T>
    class ConstantProperty {
    public:
        /**
         *  oneTimeGetter is a function (can be a lambda()) which computes the given value. It is called 
         *  just once, and LAZILY, the first time the given VirtualConstant value is required.
         */
        ConstantProperty () = delete;
        template <invocable F>
        constexpr ConstantProperty (F oneTimeGetter)
            requires (convertible_to<invoke_result_t<F>, T>);

    public:
        /**
         */
        ConstantProperty& operator= (const ConstantProperty&) = delete;

    public:
        /**
         */
        ~ConstantProperty () = default;

    public:
        /**
         *  A virtual constant can be automatically assigned to its underlying base type.
         *  Due to how conversion operators work, this won't always be helpful (like with overloading
         *  or multiple levels of conversions). But when it works (80% of the time) - its helpful.
         */
        nonvirtual operator const T () const;

    public:
        /**
         *  This works 100% of the time. Just use the function syntax, and you get back a constant of the desired
         *  underlying type.
         *
         *  \par Example Usage
         *      \code
         *          namespace PredefinedInternetMediaType {  const inline Execution::ConstantProperty<InternetMediaType> kPNG...
         *
         *          bool checkIsImage1 = PredefinedInternetMediaType::kPNG().IsImageFormat ();
         *      \endcode
         */
        nonvirtual const T operator() () const;

    public:
        /**
         *  This works 100% of the time. Just use the operator-> syntax, and you get back a constant of the desired
         *  underlying type.
         *
         *  \par Example Usage
         *      \code
         *          namespace PredefinedInternetMediaType {  const inline Execution::ConstantProperty<InternetMediaType> kPNG = ...
         *
         *          bool checkIsImage2 = PredefinedInternetMediaType::kPNG->IsImageFormat ();
         *      \endcode
         */
        nonvirtual const T* operator->() const;

    private:
        const function<T ()> fOneTimeGetter_;
        mutable optional<T> fCachedValue_; // @todo Could use array of bytes and manually construct/destruct - more work, but saves a few bytes
        mutable once_flag fValueInitialized_;
        const T&          Getter_ () const;
    };

    /**
     *  \brief ExtendableProperty is a Property which has callbacks associated with it, to be notified when it is accessed or updated
     * 
     *  These callbacks which you can attach to an ExtendableProperty effectively allow OVERRIDING the property
     *  because you can suppress update or handle it differently yourself.
     * 
     *  Use Cases:
     *    -  HTTP Response
     *       - IO::Network::HTTP::Response base class
     *       - Frameworks::WebServer::Response subclass
     *         in Response::rwHeader() property, I want to (in subclass) ASSERT that state is in-progress (or some such)
     *
     *   VIABLE APPROACHES:
     *     - Implement 'events' so when a property changes a hook gets called (possibly even before/after hooks, with before hooks
     *       possibly aborting change)
     *     - Provide some method for REPLACING the GETTER/SETTER hooks
     *     - Simply 'hide' the member in the subclass (so 2 properties with the same name)
     * 
     *     The third approach sucks cuz waste of space, and inconsistent behavior if you access property through ptr
     *     to base class.
     * 
     *     'Events' approach nice in that it is more generally useful (listeners could be largely unrelated - external - like vtable methods
     *     vs 'function' ptr objects). But its COSTLY when not used (must maintain a list of callbacks, or worse two). Can mitigate cost
     *     as mentioned above, by only having subclass of Property (PropertyWithEvents) that supports events.
     * 
     *     REPLACING the GETTER/SETTER seems quite viable, except that it appears to really kill modularity. No way (I can think of) within
     *     c++ to capture any kind of public/private thing. Anybody would be replacing GETTERS or SETTERS (if anybody can) (cannot use
     *     protected cuz not subclassing, and forcing extra subclassing would be awkward).
     * 
     *  \note since properties and therefore ExtendableProperty cannot be copied, its natural to note that their 'event handlers' also
     *        are not generally copied.
     * 
     *        Though its up to any object which uses properties, its generally presumed and recommended that of you copy objects
     *        O1 (of type O, with ExtendableProperty P) to object O2, then the event handlers watching properties P (from O1)
     *        will NOT be copied to object O2 (to its property P).
     * 
     *  \note @todo - it we use a std::forward_list in the interest of being very cheap when not used (often), but this class is a PITA, if we need to
     *        control the order of additions (like append) - which is a PITA with forward_list. Consider alternatives.
     *     
     */
    template <typename T>
    class ExtendableProperty : public Property<T> {
    public:
        using typename Property<T>::decayed_value_type;
        using typename Property<T>::base_value_type;

    public:
        /**
         */
        ExtendableProperty ()                          = delete;
        ExtendableProperty (const ExtendableProperty&) = delete;
        ExtendableProperty (ExtendableProperty&&)      = delete;
        template <qCompilerAndStdLib_RequiresNotMatchXXXDefined_1_BWA (invocable<const ExtendableProperty<T>*>) G,
                  qCompilerAndStdLib_RequiresNotMatchXXXDefined_1_BWA (invocable<ExtendableProperty<T>*, remove_cvref_t<T>>) S>
        ExtendableProperty (G getter, S setter)
            qCompilerAndStdLib_RequiresNotMatchXXXDefined_2_BWA (requires (convertible_to<invoke_result_t<G, const ExtendableProperty<T>*>, T>));

    public:
        /**
         */
        nonvirtual ExtendableProperty& operator= (Configuration::ArgByValueType<T> value);
        nonvirtual ExtendableProperty& operator= (const ExtendableProperty& value);
        nonvirtual ExtendableProperty& operator= (const ExtendableProperty&&) = delete;

    public:
        /**
         */
        struct PropertyChangedEvent {
            decayed_value_type fPreviousValue;
            decayed_value_type fNewValue;
        };

    public:
        /**
         * if return result is false, this silently cuts-off processing. EventHandlers can also 
         * throw also can be used to prevent further processing, but then calling code will see that as an error.
         * So event handler ordering matters.
         * If any event propertyChangedHandlers present, they are handled in-order, with the underlying SETTER being the final 'eventhandler'
         *
         *  TODO
         *      @todo be CAREFUL about copying these propertyChangedHandlers (what they reference) - maybe they should take parent obj ptr param*? or propery*?
         *  
         * \note These handlers can get called even when no change has actually occurred (no compare != of new/Prev value) - in order to avoid
         *       building in a dependency in this code on using types that are comparable.
         * 
         *       This means you may find you need to do such comparisons yourself in callbacks before doing much to 'process' the change.
         */
        using PropertyChangedEventHandler = std::function<PropertyCommon::PropertyChangedEventResultType (const PropertyChangedEvent&)>;

    public:
        /**
         *  Note - that assigning/coping reference values and using them with optional works poorly, so when T is a reference type,
         *  the PropertyReadEventHandler just takes and returns a pointer to T instead to avoid this unpleasantness.
         * 
         * #if 0
         *  This must return optional<base_value_type> because base_value_type is what is returned by the Get() function.
         *  But C++  doesn't allow optional<reference type>. So - for reference types, PropertyReadEventHandler returns a pointer.
         * 
         *  @todo - must copy constness of that pointer from constness of base_value_type (or address of it... so ignore for now)
         * #endif
         */
        using PropertyReadEventHandlerArgAndReturnValue_ = conditional_t<is_reference_v<base_value_type>, decayed_value_type*, base_value_type>;

    public:
        /**
         *  this gets handed the original base value (stored), but then each handler gets a crack at overriding the 
         *  value.
         */
        using PropertyReadEventHandler =
            std::function<PropertyReadEventHandlerArgAndReturnValue_ (const PropertyReadEventHandlerArgAndReturnValue_&)>;

    public:
        /**
         * Use forward_list instead of Sequence<> to avoid a dependency on Stroika containers in a potentially low level component
         */
        ReadOnlyProperty<const std::forward_list<PropertyReadEventHandler>&> propertyReadHandlers;
        Property<std::forward_list<PropertyReadEventHandler>&>               rwPropertyReadHandlers;

    public:
        /**
         * Use forward_list instead of Sequence<> to avoid a dependency on Stroika containers in a potentially low level component
         */
        ReadOnlyProperty<const std::forward_list<PropertyChangedEventHandler>&> propertyChangedHandlers;
        Property<std::forward_list<PropertyChangedEventHandler>&>               rwPropertyChangedHandlers;

    private:
        std::forward_list<PropertyReadEventHandler>    fPropertyReadHandlers_;
        std::forward_list<PropertyChangedEventHandler> fPropertyChangedHandlers_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Property.inl"

#endif /*_Stroika_Foundation_Common_Property_h_*/
