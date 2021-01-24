/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Property_h_
#define _Stroika_Foundation_Execution_Property_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 * 
 * 
http://www.open-std.org/Jtc1/sc22/wg21/docs/papers/2004/n1615.pdf
https://stackoverflow.com/questions/40559828/c-properties-with-setter-and-getter
https://www.reddit.com/r/cpp/comments/61m9r1/what_do_you_think_about_properties_in_the_c/

TODO:
  @todo Consider redoing so type of Getter/Setter embedded into the class, so it can be saved RAW, and not converted to
    a function pointer. Trickier to do construction, but probably possible with template guides. But only bother
    if there is a clear performance betenfit, because this is simpler.
 */

namespace Stroika::Foundation::Execution {

    /**
     *  Implement C#-like syntax for read-only properties (syntactically like data members but backed by a getter function)
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
         *  A ReadOnlyProperty can be automatically assigned to its underlying base type.
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
        nonvirtual T operator= (T const& value);

    public:
        /**
         */
        nonvirtual void Set (T const& value);

    private:
        const function<void (T)> fSetter_;
    };

    /**
     *  Implement C#-like syntax for properties (syntactically like data members but backed by a getter and a setter function)
     *  Properties are NOT copy constructible (because the getter/setter usually contain a binding to the actual data source).
     *  But the are ASSIGNABLE TO, because here we retain our getter/setter, and just treat assignment as copying the value.
     * 
     *  \note This has implications to classes that use Property objects - they will not be able to use operator(const X&) = default;
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
        T operator= (const T& value);
        Property& operator= (const Property& value);

    public:
        nonvirtual T Get () const;

    public:
        nonvirtual void Set (const T& value);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Property.inl"

#endif /*_Stroika_Foundation_Execution_Property_h_*/
