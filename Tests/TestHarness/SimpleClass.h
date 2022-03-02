/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_SimpleClass_h_
#define _Stroika_Foundation_Tests_SimpleClass_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

// included for qIteratorsRequireNoArgConstructorForT, which should be moved or eliminated
#include "Stroika/Foundation/Traversal/Iterator.h"

#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika {

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

        SimpleClass& operator= (SimpleClass&& rhs) = default;
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

        SimpleClassWithoutComparisonOperators& operator= (SimpleClassWithoutComparisonOperators&& rhs) = default;
        SimpleClassWithoutComparisonOperators& operator= (const SimpleClassWithoutComparisonOperators&) = default;

        nonvirtual size_t GetValue () const;
        static size_t     GetTotalLiveCount ();

        SimpleClassWithoutComparisonOperators operator+ (const SimpleClassWithoutComparisonOperators& rhs) const;

    private:
        size_t        fValue_;
        int           fConstructed_;
        static size_t sTotalLiveObjects_;
    };

};

#endif /* _Stroika_Foundation_Tests_SimpleClass_h_ */
