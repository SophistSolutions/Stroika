/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DiscreteRange_h_
#define _Stroika_Foundation_Traversal_DiscreteRange_h_ 1

#include "../StroikaPreComp.h"

#include "Iterable.h"
#include "Range.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
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
 *      @todo   GetNext() not quite right - using 'int'.
 *
 *      @todo   DefaultDiscreteRangeTraits_Enum  : ExplicitDiscreteRangeTraits<T, T::eSTART, T::eLAST, int, unsigned int> {
 *              should use  underlying_type - but not sure why it didnt work easily.
 *
 *      @todo   Try to rewrite using Generator<> code... See if that simplifies things...
 *
 *      @todo   DefaultDiscreteRangeTraits_Enum and DefaultDiscreteRangeTraits_Integral<> should do better
 *              auto-computing the 'differnce' type
 */

namespace Stroika::Foundation::Traversal {

    namespace RangeTraits {

        /**
         */
        template <typename T, T MIN, T MAX, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
        struct ExplicitDiscreteRangeTraits : ExplicitRangeTraits_Integral<T, MIN, MAX, Openness::eClosed, Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
            static constexpr T GetNext (T n)
            {
                return static_cast<T> (static_cast<int> (n) + 1);
            }
            static constexpr T GetPrevious (T n)
            {
                return static_cast<T> (static_cast<int> (n) - 1);
            }
            using RangeTraitsType = ExplicitRangeTraits_Integral<T, MIN, MAX, Openness::eClosed, Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>;
        };

        /**
         *  DefaultDiscreteRangeTraits_Enum<> can be used to generate an automatic traits object (with bounds)
         *  if you've applied the Stroika_Define_Enum_Bounds() macro to the given enumeration.
         */
        template <typename T>
        struct DefaultDiscreteRangeTraits_Enum : ExplicitDiscreteRangeTraits<T, T::eSTART, T::eLAST, int, unsigned int> {
        };

        /**
         */
        template <typename T>
        struct DefaultDiscreteRangeTraits_Integral : ExplicitDiscreteRangeTraits<T, numeric_limits<T>::lowest (), numeric_limits<T>::max (), decltype (T{} - T{}), make_unsigned_t<decltype (T{} - T{})>> {
        };

        /**
         */
        template <typename T>
        struct DefaultDiscreteRangeTraits_DefaultRangeTraits : DefaultRangeTraits<T> {
            using RangeTraitsType = DefaultRangeTraits<T>;
        };

        /**
        * @todo redo using using - document it would be nice if there was some select_t (https://stackoverflow.com/questions/32785105/implementing-a-switch-type-trait-with-stdconditional-t-chain-calls)
        * 
         */
        template <typename T>
        struct DefaultDiscreteRangeTraits : conditional_t<
                                                is_enum_v<T>, DefaultDiscreteRangeTraits_Enum<T>,
                                                conditional_t<
                                                    is_integral_v<T>, DefaultDiscreteRangeTraits_Integral<T>,
                                                    DefaultDiscreteRangeTraits_DefaultRangeTraits<T>>> {
        };

    }

    /**
     *  \par Example Usage
     *      \code
     *          vector<int> v = DiscreteRange<int> (1,10).Elements ().As<vector<int>> ();
     *          // equiv to vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
     *      \endcode
     *
     *  \par OR
     *      \code
     *          for (auto i : DiscreteRange<int> (1,10)) {
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
     *  \note   Since the default traits for an enum with Stroika type anotation (Stroika_Define_Enum_Bounds)
     *          provides  the start/end, DiscreteRange<SOME_ENUM>::FullRange ().Elements () returns an
     *          iterable with all possible legal values of the enum.
     */
    template <typename T, typename TRAITS = RangeTraits::DefaultDiscreteRangeTraits<T>>
    class DiscreteRange : public Range<T, typename TRAITS::RangeTraitsType> {
    private:
        using inherited = Range<T, typename TRAITS::RangeTraitsType>;

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
         */
        explicit constexpr DiscreteRange () = default;
        explicit constexpr DiscreteRange (T begin, T end);
        explicit constexpr DiscreteRange (const optional<T>& begin, const optional<T>& end);
        explicit constexpr DiscreteRange (const Range<T, typename TRAITS::RangeTraitsType>& r);

    public:
        /**
         *  Like Range<>::FullRange () but returing a DiscreteRange<> type.
         */
        static constexpr DiscreteRange FullRange ();

    public:
        /**
         *  Like Range<>::Intersection (), but returing a DiscreteRange<> type.
         */
        constexpr Range<T, TRAITS> Intersection (const Range<T, TRAITS>& rhs) const;
        constexpr DiscreteRange    Intersection (const DiscreteRange& rhs) const;

    public:
        /**
         *  Like Range<>::UnionBounds (), but returing a DiscreteRange<> type.
         */
        constexpr Range<T, TRAITS> UnionBounds (const Range<T, TRAITS>& rhs) const;
        constexpr DiscreteRange    UnionBounds (const DiscreteRange& rhs) const;

    public:
        /**
         *  This returns the number of points from lower bound to upperbound inclusive.
         *  This equals GetDistancespanned () + 1 (roughly).
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
         *  Elements () makes no guarantess about whether or not modifications to the underlying DisjointDiscreteRange<> will
         *  appear in the Elements() Iterable<T>.
         */
        nonvirtual Iterable<T> Elements () const;

    public:
        /**
         *  Alias for 'Elements' ()
         */
        nonvirtual operator Iterable<T> () const;

    public:
        /**
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
