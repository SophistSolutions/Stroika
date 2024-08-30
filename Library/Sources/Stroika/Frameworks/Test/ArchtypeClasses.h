/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Test_ArchtypeClasses_h_
#define _Stroika_Frameworks_Test_ArchtypeClasses_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Configuration/Common.h"

/**
 *  This module needs a lot of work. Need a set of basic archetype classes for things that are not copyable, copyable, default constructible etc
 *  to use in regression test (especially container) test cases...
 */
namespace Stroika::Frameworks::Test::ArchtypeClasses {

    using Foundation::Common::ComparisonRelationDeclaration;
    using Foundation::Common::ComparisonRelationDeclarationBase;
    using Foundation::Common::ComparisonRelationType;

    /**
     * \brief IINTeroperable = constructible, and convertible to size_t int as well; for simple use in regression tests
     */
    template <typename T>
    concept IINTeroperable = requires (T t) {
        {
            static_cast<size_t> (T{1u})
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

        OnlyDefaultConstructibleAndMoveable& operator= (const OnlyDefaultConstructibleAndMoveable&)     = delete;
        OnlyDefaultConstructibleAndMoveable& operator= (OnlyDefaultConstructibleAndMoveable&&) noexcept = default;

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

    using NotCopyable [[deprecated ("Since Stroika v3.0d10 use OnlyDefaultConstructibleAndMoveable")]] = OnlyDefaultConstructibleAndMoveable;

    /**
     *  Object NOT default constructible, is copyable (and assignable); supports operator+ and operator==, and operator<, operator <=>, etc
     * 
     *  \note before Stroika v3.0d10 this was called 'SimpleClass'
     */
    class OnlyCopyableMoveableAndTotallyOrdered {
    public:
        OnlyCopyableMoveableAndTotallyOrdered () noexcept = delete;
        OnlyCopyableMoveableAndTotallyOrdered (OnlyCopyableMoveableAndTotallyOrdered&& src) noexcept;
        OnlyCopyableMoveableAndTotallyOrdered (const OnlyCopyableMoveableAndTotallyOrdered&) noexcept;
        OnlyCopyableMoveableAndTotallyOrdered (size_t v);
        ~OnlyCopyableMoveableAndTotallyOrdered ();

        OnlyCopyableMoveableAndTotallyOrdered& operator= (OnlyCopyableMoveableAndTotallyOrdered&& rhs)  = default;
        OnlyCopyableMoveableAndTotallyOrdered& operator= (const OnlyCopyableMoveableAndTotallyOrdered&) = default;

        [[deprecated ("Since Stroika v3.0d10 use static_cast<size_t>")]] size_t GetValue () const
        {
            return fValue_;
        }
        static size_t GetTotalLiveCount ();

        OnlyCopyableMoveableAndTotallyOrdered operator+ (const OnlyCopyableMoveableAndTotallyOrdered& rhs) const;
        explicit                              operator size_t () const;

        bool            operator== (const OnlyCopyableMoveableAndTotallyOrdered& rhs) const;
        strong_ordering operator<=> (const OnlyCopyableMoveableAndTotallyOrdered& rhs) const;

    private:
        size_t               fValue_;
        int                  fConstructed_;
        static inline size_t sTotalLiveObjects_{0};
    };
    static_assert (not default_initializable<OnlyCopyableMoveableAndTotallyOrdered>);
    static_assert (copyable<OnlyCopyableMoveableAndTotallyOrdered>);
    static_assert (move_constructible<OnlyCopyableMoveableAndTotallyOrdered>);
    static_assert (equality_comparable<OnlyCopyableMoveableAndTotallyOrdered>);
    static_assert (totally_ordered<OnlyCopyableMoveableAndTotallyOrdered>); // @todo allow this to be totally ordered...
    static_assert (not semiregular<OnlyCopyableMoveableAndTotallyOrdered>);
    static_assert (not regular<OnlyCopyableMoveableAndTotallyOrdered>);
    static_assert (IINTeroperable<OnlyCopyableMoveableAndTotallyOrdered>);

    using SimpleClass [[deprecated ("Since Stroika v3.0d10 use OnlyCopyableMoveableAndTotallyOrdered")]] = OnlyCopyableMoveableAndTotallyOrdered;

    /**
     *  Object NOT default constructible, is copyable, assignable; supports operator+, but NO COMPARISON operators
     *   \note before Stroika v3.0d10 this was called 'SimpleClassWithoutComparisonOperators'
     */
    class OnlyCopyableMoveable {
    public:
        OnlyCopyableMoveable () noexcept = delete;
        OnlyCopyableMoveable (OnlyCopyableMoveable&& src) noexcept;
        OnlyCopyableMoveable (const OnlyCopyableMoveable&) noexcept;
        OnlyCopyableMoveable (size_t v);
        ~OnlyCopyableMoveable ();

        OnlyCopyableMoveable& operator= (OnlyCopyableMoveable&& rhs)  = default;
        OnlyCopyableMoveable& operator= (const OnlyCopyableMoveable&) = default;

        [[deprecated ("Since Stroika v3.0d10 use static_cast<size_t>")]] size_t GetValue () const
        {
            return fValue_;
        }
        static size_t GetTotalLiveCount ();

        explicit             operator size_t () const;
        OnlyCopyableMoveable operator+ (const OnlyCopyableMoveable& rhs) const;

    private:
        size_t               fValue_;
        int                  fConstructed_;
        static inline size_t sTotalLiveObjects_{0};
    };
    static_assert (not default_initializable<OnlyCopyableMoveable>);
    static_assert (copyable<OnlyCopyableMoveable>);
    static_assert (move_constructible<OnlyCopyableMoveable>);
    static_assert (not equality_comparable<OnlyCopyableMoveable>);
    static_assert (not totally_ordered<OnlyCopyableMoveable>); // @todo allow this to be totally ordered...
    static_assert (not semiregular<OnlyCopyableMoveable>);
    static_assert (not regular<OnlyCopyableMoveable>);
    static_assert (IINTeroperable<OnlyCopyableMoveable>);

    using SimpleClassWithoutComparisonOperators [[deprecated ("Since Stroika v3.0d10 use OnlyCopyableMoveable")]] = OnlyCopyableMoveable;

    /**
     * \brief alias for 'regular' type (copyable, default constructible, etc)
     */
    using Regular = size_t;
    static_assert (regular<Regular>);
    static_assert (IINTeroperable<Regular>);

    /**
     *  For types that don't have operator== bulletin, this saves a bit of typing producing a generic comparer
     */
    template <IINTeroperable T>
    struct AsIntsEqualsComparer : ComparisonRelationDeclarationBase<ComparisonRelationType::eEquals> {
        bool operator() (T v1, T v2) const
        {
            return static_cast<size_t> (v1) == static_cast<size_t> (v2);
        }
    };

    /**
     *  For types that don't have operator== bulletin, this saves a bit of typing producing a generic comparer
     */
    template <IINTeroperable T>
    struct AsIntsLessComparer : ComparisonRelationDeclarationBase<ComparisonRelationType::eStrictInOrder> {
        bool operator() (T v1, T v2) const
        {
            return static_cast<size_t> (v1) < static_cast<size_t> (v2);
        }
    };

}

#endif /* _Stroika_Frameworks_Test_ArchtypeClasses_h_ */
