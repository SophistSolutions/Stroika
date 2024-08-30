/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Test_ArchtypeClasses_h_
#define _Stroika_Frameworks_Test_ArchtypeClasses_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

/**
 *  This module needs a lot of work. Need a set of basic archetype classes for things that are not copyable, copyable, default constructible etc
 *  to use in regression test (especially container) test cases...
 */
namespace Stroika::Frameworks::Test::ArchtypeClasses {

    // CONSTRUCIBLE_FROM_INT, and convertible to size_t int as well
    // Handy in regtests...
    template <typename T>
    concept IINTeroperable = requires (T t) {
        {
            static_cast<size_t> (T{1u})
        } -> convertible_to<size_t>;
        {
            T{1u}.GetValue ()
        } -> convertible_to<size_t>;
        {
            T{1u} + T{1u}
        } -> convertible_to<T>;
    };

    /**
     *  \note Until Stroika v3.0d10 - this was called 'NotCopyable'
     */
    struct OnlyDefaultConstructibleAndMoveable {
        OnlyDefaultConstructibleAndMoveable ()                                           = default;
        OnlyDefaultConstructibleAndMoveable (const OnlyDefaultConstructibleAndMoveable&) = delete;
        OnlyDefaultConstructibleAndMoveable (OnlyDefaultConstructibleAndMoveable&&)      = default;

        nonvirtual OnlyDefaultConstructibleAndMoveable& operator= (const OnlyDefaultConstructibleAndMoveable&) = delete;
        nonvirtual OnlyDefaultConstructibleAndMoveable& operator= (OnlyDefaultConstructibleAndMoveable&&)      = default;

        void method (){};
        void const_method () const {};
    };
    static_assert (default_initializable<OnlyDefaultConstructibleAndMoveable>);
    static_assert (not copyable<OnlyDefaultConstructibleAndMoveable>);
    static_assert (move_constructible<OnlyDefaultConstructibleAndMoveable>);
    static_assert (not equality_comparable<OnlyDefaultConstructibleAndMoveable>);
    static_assert (not totally_ordered<OnlyDefaultConstructibleAndMoveable>);
    static_assert (not semiregular<OnlyDefaultConstructibleAndMoveable>);
    static_assert (not regular<OnlyDefaultConstructibleAndMoveable>);

    /**
     *  Object NOT default constructible, is copyable, assignable; supports operator+ and operator==, and operator<
     */
    class SimpleClass {
    public:
        SimpleClass () noexcept = delete;
        SimpleClass (SimpleClass&& src) noexcept;
        SimpleClass (const SimpleClass&) noexcept;
        SimpleClass (size_t v);
        ~SimpleClass ();

        SimpleClass& operator= (SimpleClass&& rhs)  = default;
        SimpleClass& operator= (const SimpleClass&) = default;

        nonvirtual size_t GetValue () const;
        static size_t     GetTotalLiveCount ();

        SimpleClass operator+ (const SimpleClass& rhs) const;
        explicit    operator size_t () const;

        bool operator== (const SimpleClass& rhs) const;
        bool operator< (const SimpleClass& rhs) const;

    private:
        size_t        fValue_;
        int           fConstructed_;
        static size_t sTotalLiveObjects_;
    };
    static_assert (not default_initializable<SimpleClass>);
    static_assert (copyable<SimpleClass>);
    static_assert (move_constructible<SimpleClass>);
    static_assert (equality_comparable<SimpleClass>);
    static_assert (not totally_ordered<SimpleClass>); // @todo allow this to be totally ordered...
    static_assert (not semiregular<SimpleClass>);
    static_assert (not regular<SimpleClass>);
    static_assert (IINTeroperable<SimpleClass>);

    /**
     *  Object NOT default constructible, is copyable, assignable; supports operator+, but NO COMPARISON operators
     */
    class SimpleClassWithoutComparisonOperators {
    public:
        SimpleClassWithoutComparisonOperators () noexcept = delete;
        SimpleClassWithoutComparisonOperators (SimpleClassWithoutComparisonOperators&& src) noexcept;
        SimpleClassWithoutComparisonOperators (const SimpleClassWithoutComparisonOperators&) noexcept;
        SimpleClassWithoutComparisonOperators (size_t v);
        ~SimpleClassWithoutComparisonOperators ();

        SimpleClassWithoutComparisonOperators& operator= (SimpleClassWithoutComparisonOperators&& rhs)  = default;
        SimpleClassWithoutComparisonOperators& operator= (const SimpleClassWithoutComparisonOperators&) = default;

        nonvirtual size_t GetValue () const;
        static size_t     GetTotalLiveCount ();

        SimpleClassWithoutComparisonOperators operator+ (const SimpleClassWithoutComparisonOperators& rhs) const;

    private:
        size_t        fValue_;
        int           fConstructed_;
        static size_t sTotalLiveObjects_;
    };
    static_assert (not default_initializable<SimpleClassWithoutComparisonOperators>);
    static_assert (copyable<SimpleClassWithoutComparisonOperators>);
    static_assert (move_constructible<SimpleClassWithoutComparisonOperators>);
    // static_assert (equality_comparable<SimpleClassWithoutComparisonOperators>);
    static_assert (not totally_ordered<SimpleClassWithoutComparisonOperators>); // @todo allow this to be totally ordered...
    static_assert (not semiregular<SimpleClassWithoutComparisonOperators>);
    static_assert (not regular<SimpleClassWithoutComparisonOperators>);
   // static_assert (IINTeroperable<SimpleClassWithoutComparisonOperators>);

}

#endif /* _Stroika_Frameworks_Test_ArchtypeClasses_h_ */
