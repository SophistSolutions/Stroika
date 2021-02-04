/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Property_h_
#define _Stroika_Foundation_Common_Property_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/TypeHints.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 * 
 * 
 *  Notes:
 *      I've long thought about doing something like this, but only recently got frustrated with
 *      the ulginess of tons of getters and setters. So I googled for solutions, and was suprised
 *      there was so little traction behind adding this feature to C++ (from C#).
 * 
 *      These links give a few hints about dicussions of adding this feature to c++:
 *          *   http://www.open-std.org/Jtc1/sc22/wg21/docs/papers/2004/n1615.pdf
 *          *   https://stackoverflow.com/questions/40559828/c-properties-with-setter-and-getter
 *          *   https://www.reddit.com/r/cpp/comments/61m9r1/what_do_you_think_about_properties_in_the_c/
 *
 *  TODO:
 *      @todo   Consider redoing so type of Getter/Setter embedded into the class, so it can 
 *              be saved RAW, and not converted to a function pointer. Trickier to do construction,
 *              but probably possible with template guides. But only bother if there is a clear
 *              performance betenfit, because this is simpler.
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
#define qStroika_Foundation_Common_Property_OuterObjPtr(property, propertyPtrToMember) \
    Memory::GetObjectOwningField (property, propertyPtrToMember)
#endif

    /**
     *  Implement C#-like syntax for read-only properties (syntactically like data members but backed by a getter function)
     *      \note   Typically not used - use Property
     *      \note   ANYHOW - see @Property for design overview
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
        template <typename G>
        constexpr ReadOnlyProperty (G getter);

    public:
        nonvirtual void operator= (const ReadOnlyProperty&) = delete;
        nonvirtual void operator= (const ReadOnlyProperty&&) = delete;

    public:
        /**
         */
        nonvirtual T Get () const;

    public:
        /**
         *  A ReadOnlyProperty can be automatically converted to its underlying base type.
         *  Due to how conversion operators work, this won't always be helpful (like with overloading
         *  or multiple levels of conversions, for example with optional<int> compare with int property).
         *  But when it works (80% of the time) - its helpful.
         * 
         *  When it doesn't work, simply throw in '()' - to use the 'operator()' call, or call Get()
         *  if you prefer that syntax.
         */
        nonvirtual operator const T () const;

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
         */
        nonvirtual const T operator() () const;

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
        template <typename S>
        constexpr WriteOnlyProperty (S setter);

    public:
        /**
         *  You can assign a value of the underlying type, but we do NOT support operator=(WriteOnlyProperty) because
         *  we cannot generically read from a write-only property to copy its value.
         */
        nonvirtual void operator= (const Configuration::ArgByValueType<T>& value);
        nonvirtual void operator= (const WriteOnlyProperty&) = delete;
        nonvirtual void operator= (const WriteOnlyProperty&&) = delete;

    public:
        /**
         */
        nonvirtual void Set (const Configuration::ArgByValueType<T>& value);

    private:
        const function<void (WriteOnlyProperty*, const Configuration::ArgByValueType<T>&)> fSetter_;
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
     *  \note New Stroika NAMING CONVENTION intruduced for instance variables of type Property<...>, which is to prepend a 'p'. This
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
     *              Property<unsigned int> pContentLength1; // all 3 refer to the private fContentLength_ field
     *              Property<unsigned int> pContentLength2;
     *              Property<unsigned int> pContentLength3;
     *
     *          private:
     *              unsigned int fContentLength_{0};
     *          };
     *          Headers::Headers ()
     *              // Can implement getter/setters with this capture (wastes a bit of space)
     *              : pContentLength1{
     *                    [this] ([[maybe_unused]] const auto* property) {
     *                        return fContentLength_;
     *                    },
     *                    [this] ([[maybe_unused]] auto* property, const auto& contentLength) {
     *                        fContentLength_ = contentLength;
     *                    }}
     *              // Can implement getter/setters with Memory::GetObjectOwningField - to save space, but counts on exact
     *              // storage layout and not totally legal with non- is_standard_layout<> - see Memory::GetObjectOwningField 
     *              , pContentLength2{
     *                    [] (const auto* property) {
     *                        const Headers* headerObj = Memory::GetObjectOwningField (property, &Headers::pContentLength2);
     *                        return headerObj->fContentLength_;
     *                    },
     *                    [] (auto* property, auto contentLength) {
     *                        Headers* headerObj         = Memory::GetObjectOwningField (property, &Headers::pContentLength2);
     *                        headerObj->fContentLength_ = contentLength;
     *                    }}
     *              // Use stroika #define to decide which strategy to use 
     *              , pContentLength3{
     *                    [qStroika_Foundation_Common_Property_EmbedThisInProperties] ([[maybe_unused]]const auto* property) {
     *                        const Headers* headerObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::pContentLength3);
     *                        return headerObj->fContentLength_;
     *                    },
     *                    [qStroika_Foundation_Common_Property_EmbedThisInProperties] ([[maybe_unused]]auto* property, auto contentLength) {
     *                        Headers* headerObj         = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Headers::pContentLength3);
     *                        headerObj->fContentLength_ = contentLength;
     *                    }}
     *          {
     *          }
     *          Headers::Headers (const Headers& src)
     *              : Headers{} // do default initialization of properties
     *          {
     *              // NOTE - cannot INITIALIZE properties with src.Properties values since they are not copy constructible
     *              // but they are assignable, so do that
     *              pContentLength1 = src.pContentLength1;
     *              pContentLength2 = src.pContentLength2;
     *              // COULD EITHER initialize fContentLength_ or pContentLength1/pContentLength2 - but no need to do both
     *          }
     *          Headers::Headers (Headers&& src)
     *              : Headers{} // do default initialization of properties
     *          {
     *              // NOTE - cannot MOVE properties with src.Properties values since they are not copy constructible
     *              // but they are assignable, so do that
     *              pContentLength1 = src.pContentLength1;
     *              pContentLength2 = src.pContentLength2;
     *              // COULD EITHER initialize fContentLength_ or pContentLength1/pContentLength2 - but no need to do both
     *          }
     *          Headers& Headers::operator= (Headers&& rhs)
     *          {
     *              // Could copy either properties or underlying field - no matter which
     *              fContentLength_ = rhs.fContentLength_;
     *              return *this;
     *          }
     * 
     * 
     *          //....
     * 
     *          Headers h;
     *          Assert (h.pContentLength1 == 0);
     *          h.pContentLength1 = 2;
     *          Assert (h.pContentLength2 == 2);
     *          h.pContentLength2 = 4;
     *          Assert (h.pContentLength1 == 4);
     *          Headers h2 = h;
     *          Assert (h2.pContentLength1 == 4);
     *          h.pContentLength2 = 5;
     *          Assert (h.pContentLength1 == 5);
     *          Assert (h2.pContentLength1 == 4);
     *
     *      \endcode
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md">SAME AS T/GETTER/SETTER - all methods have exactly the thread safety of the underlying GETTER/SETTER</a>
     */
    template <typename T>
    class Property : public ReadOnlyProperty<T>, public WriteOnlyProperty<T> {
    public:
        /**
         */
        using value_type = T;

    public:
        /**
         *  Properties are NOT movable, nor copy constructible: the data doesn't logically exist in the property itself,
         *  but in its relationship to some parent object; if it were copied, it might be copied TO some place that didn't
         *  have an appropriate enclosing object.
         */
        Property ()                = delete;
        Property (const Property&) = delete;
        Property (Property&&)      = delete;
        template <typename G, typename S>
        Property (G getter, S setter);

    public:
        /**
         */
        nonvirtual T operator        = (const Configuration::ArgByValueType<T>& value);
        nonvirtual Property& operator= (const Property& value);
        nonvirtual Property& operator= (const Property&&) = delete;

    public:
        using ReadOnlyProperty<T>::Get;

    public:
        using WriteOnlyProperty<T>::Set;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Property.inl"

#endif /*_Stroika_Foundation_Common_Property_h_*/
