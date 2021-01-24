/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Property_h_
#define _Stroika_Foundation_Execution_Property_h_ 1

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
 *      o   @todo Consider redoing so type of Getter/Setter embedded into the class, so it can 
 *          be saved RAW, and not converted to a function pointer. Trickier to do construction,
 *          but probably possible with template guides. But only bother if there is a clear
 *          performance betenfit, because this is simpler.
 */

namespace Stroika::Foundation::Execution {

    /**
     *  Implement C#-like syntax for read-only properties (syntactically like data members but backed by a getter function)
     *      \note   Typically not used - use Property
     */
    template <typename T>
    class ReadOnlyProperty {
    public:
        /**
         */
        using value_type = T;

    public:
        /**
         */
        ReadOnlyProperty () = delete;
        template <typename G>
        constexpr ReadOnlyProperty (G getter);

    public:
        /**
         */
        nonvirtual T Get () const;

    public:
        /**
         *  A ReadOnlyProperty can be automatically converted to its underlying base type.
         *  Due to how conversion operators work, this won't always be helpful (like with overloading
         *  or multiple levels of conversions). But when it works (80% of the time) - its helpful.
         */
        nonvirtual operator const T () const;

    public:
        /**
         *  This works 100% of the time. Just use the function syntax, and you get back a copy of the desired
         *  underlying type.
         *
         *  \par Example Usage
         *      \code
         *          namespace PredefinedInternetMediaType {  const inline Execution::VirtualConstant<InternetMediaType> kPNG...
         *
         *          bool checkIsImage1 = PredefinedInternetMediaType::kPNG().IsImageFormat ();
         *      \endcode
         */
        nonvirtual const T operator() () const;

    private:
        const function<T ()> fGetter_;
    };

    /**
     *  Implement C#-like syntax for write-only properties (syntactically like data members but backed by a setter function)
     *      \note   Typically not used - use Property
     */
    template <typename T>
    class WriteOnlyProperty {
    public:
        /**
         */
        using value_type = T;

    public:
        /**
         */
        WriteOnlyProperty () = delete;
        template <typename S>
        constexpr WriteOnlyProperty (S setter);

    public:
        /**
         *  You can assign a value of the underlying type, but we do NOT support operator=(WriteOnlyProperty) because
         *  we cannot generically read from a write-only property to copy its value.
         */
        nonvirtual T operator= (const Configuration::ArgByValueType<T>& value);

    public:
        /**
         */
        nonvirtual void Set (const Configuration::ArgByValueType<T>& value);

    private:
        const function<void (const Configuration::ArgByValueType<T>&)> fSetter_;
    };

    /**
     *  Implement C#-like syntax for properties (syntactically like data members but backed by a getter and a setter function)
     *  Properties are NOT copy constructible (because the getter/setter usually contain a binding to the actual data source).
     *  But the are ASSIGNABLE TO, because here we retain our getter/setter, and just treat assignment as copying the value.
     * 
     *  \note This has implications for classes that use Property objects: they will not be able to use X(const X&) = default;
     * 
     *  \note Though this looks syntactically much like using a direct data member, it will likely have some performance overhead
     *        due to forcing the use of a std::function wrapping a lambda for each access to the underlying object of type T.
     */
    template <typename T>
    class Property : public ReadOnlyProperty<T>, public WriteOnlyProperty<T> {
    public:
        /**
         */
        using value_type = T;

    public:
        /**
         */
        Property () = delete;
        template <typename G, typename S>
        Property (G getter, S setter);

    public:
        /**
         */
        nonvirtual T operator        = (const Configuration::ArgByValueType<T>& value);
        nonvirtual Property& operator= (const Property& value);

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

#endif /*_Stroika_Foundation_Execution_Property_h_*/
