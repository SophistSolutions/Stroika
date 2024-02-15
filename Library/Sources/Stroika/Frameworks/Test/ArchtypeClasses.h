/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Test_ArchtypeClasses_h_
#define _Stroika_Frameworks_Test_ArchtypeClasses_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

/**
 *  This module needs alot of work. Need a set of basic archtype classes for things that are not copyable, copyable, default constuctible etc
 *  to use in regtest (especially container) test cases...
 */
namespace Stroika::Frameworks::Test::ArchtypeClasses {

    struct NotCopyable {
        NotCopyable ()                   = default;
        NotCopyable (const NotCopyable&) = delete;
        NotCopyable (NotCopyable&&)      = default;

        nonvirtual NotCopyable& operator= (const NotCopyable&) = delete;
        nonvirtual NotCopyable& operator= (NotCopyable&&)      = default;

        void method (){};
        void const_method () const {};
    };

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

}

#endif /* _Stroika_Frameworks_Test_ArchtypeClasses_h_ */
