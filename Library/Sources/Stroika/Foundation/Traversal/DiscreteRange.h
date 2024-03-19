/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DiscreteRange_h_
#define _Stroika_Foundation_Traversal_DiscreteRange_h_ 1

#include "../StroikaPreComp.h"

#include "Iterable.h"
#include "Range.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Consider if clearer having DiscreteRange aggregate Elements() instead of inheriting from
 *              Iterable. Would make construction, and coipying etc cheaper (esp when that feature
 *              is not used).


 >>>> DOCUMENT WHY DsicreateRange<> no longer Iterabkle<?>
    >>> avoids conflict on empty, and provides clearer 'getlength'.
    >>> avoids shared_ptr stuff (REP) for iterablke unles/until used.
    >>> this should allow constexpr discrete_rage!!!
    >>> and see other comments on why - in progress switchover...
    >>> (NOTE - downside is
        -            for (auto i : DiscreteRange<Color> (optional<Color> (), optional<Color> ())) {
        +            for (auto i : DiscreteRange<Color> (optional<Color> (), optional<Color> ()).Elements ()) {


 *
 *      @todo   Understand what you must do DiscreteRange<int> (1,10) instead of just DiscreteRange (1,10),
 *              and if something reasonable to be done about it - do so (compiler bug or ambiguity due to optional)
 *              or just that template inference of types doesn't work as well as I thought...
 *
 *      @todo   Try to rewrite using Generator<> code... See if that simplifies things...
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  \brief A DiscreteRange is a Range where the underlying endpoints are integral (discrete, not continuous); 
     *         this implies you can iterate over the members of the range, and its endpoints are closed.
     *
     *  DiscreteRange<> is an immutable type (once constructed, will never change), except for allowing operator=..
     * 
     *  \par Example Usage
     *      \code
     *          vector<int> v = DiscreteRange<int>{1,10}.Elements ().As<vector<int>> ();
     *          // equiv to vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
     *      \endcode
     *
     *  \par OR
     *      \code
     *          for (auto i : DiscreteRange<int>{1,10}) {
     *              ...i = 1..10
     *          }
     *      \endcode
     *
     *  \par OR
     *      \code
     *          enum class Color { red, blue, green,  Stroika_Define_Enum_Bounds (red, green) };
     *          for (auto i : DiscreteRange<Color>::FullRange ().Elements ()) {
     *              // iterate over each color - red, green, blue
     *          }
     *      \endcode
     *
     *  \note   Since the default traits for an enum with Stroika type annotation (Stroika_Define_Enum_Bounds)
     *          provides  the start/end, DiscreteRange<SOME_ENUM>::FullRange ().Elements () returns an
     *          iterable with all possible legal values of the enum.
     */
    template <typename T, typename TRAITS = RangeTraits::Default<T>>
    class DiscreteRange : public Range<T, TRAITS> {
    public:
        static_assert (TRAITS::kLowerBoundOpenness == Openness::eClosed);
        static_assert (TRAITS::kUpperBoundOpenness == Openness::eClosed);

    private:
        using inherited = Range<T, TRAITS>;

    public:
        /**
         */
        using value_type = typename inherited::value_type;

    public:
        /**
         */
        using SignedDifferenceType = typename inherited::SignedDifferenceType;

    public:
        /**
         */
        using UnsignedDifferenceType = typename inherited::UnsignedDifferenceType;

    private:
        struct MyIteratorRep_;

    public:
        /**
         *  DiscreteRange () with no arguments produces an empty sequence.
         *
         *  \req begin <= end (after substitution of optional values)
         *  \req src range must be eClosed on both sides
         */
        explicit constexpr DiscreteRange () = default;
        explicit constexpr DiscreteRange (T begin, T end);
        explicit constexpr DiscreteRange (const optional<T>& begin, const optional<T>& end);
        explicit constexpr DiscreteRange (const Range<T, TRAITS>& r);

    public:
        /**
         *  Like Range<>::FullRange () but returning a DiscreteRange<> type.
         */
        static constexpr DiscreteRange FullRange ();

    public:
        /**
         *  Like Range<>::Intersection (), but returning a DiscreteRange<> type.
         */
        constexpr Range<T, TRAITS> Intersection (const Range<T, TRAITS>& rhs) const;
        constexpr DiscreteRange    Intersection (const DiscreteRange& rhs) const;

    public:
        /**
         *  Like Range<>::UnionBounds (), but returning a DiscreteRange<> type.
         */
        constexpr Range<T, TRAITS> UnionBounds (const Range<T, TRAITS>& rhs) const;
        constexpr DiscreteRange    UnionBounds (const DiscreteRange& rhs) const;

    public:
        /**
         *  This returns the number of points from lower bound to upper bound inclusive.
         *  This equals GetDistanceSpanned () + 1 (roughly).
         *  If (empty ()) .... this returns 0;
         */
        constexpr UnsignedDifferenceType GetNumberOfContainedPoints () const;

    public:
        /**
         *  \req not empty
         *  \req the DiscreteRange produced by applying the given offset to *this remains valid with respect to the constraints on this DiscreteRange.
         */
        constexpr DiscreteRange Offset (SignedDifferenceType o) const;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          DisjointDiscreteRange<DiscreteRange<int>> t;
         *          for (T i : t.Elements ()) {
         *          }
         *      \endcode
         *
         *  Elements () makes no guarantees about whether or not modifications to the underlying DisjointDiscreteRange<> will
         *  appear in the Elements() Iterable<T>.
         * 
         *  \note Elements produces a generator, not an actual list of each item (internally). This should not be visible externally,
         *        except in how much memory or time is required to create an Elements() iterable for a large range (should be cheap).
         */
        nonvirtual Iterable<T> Elements () const;

    public:
        /**
         *  Alias for 'Elements' ()
         */
        nonvirtual operator Iterable<T> () const;

    public:
        /**
         * \see Elements () - this produces a generator iterator, so should be quite cheap even for a large range.
         */
        nonvirtual Iterator<T> begin () const;

    public:
        /**
         */
        nonvirtual Iterator<T> end () const;

    private:
        struct MyIterable_;
    };

    /**
     *  Intersection ()
     */
    template <typename T, typename TRAITS>
    DiscreteRange<T, TRAITS> operator^ (const DiscreteRange<T, TRAITS>& lhs, const DiscreteRange<T, TRAITS>& rhs);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "DiscreteRange.inl"

#endif /*_Stroika_Foundation_Traversal_DiscreteRange_h_ */
