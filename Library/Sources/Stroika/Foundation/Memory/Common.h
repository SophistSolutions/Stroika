/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Common_h_
#define _Stroika_Foundation_Memory_Common_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <bit>
#include <compare>
#include <memory>
#include <span>
#include <type_traits>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"

/**
 */

namespace Stroika::Foundation::Memory {

    namespace Private_ {
        template <class>
        inline constexpr bool _Is_span_v = false;
        template <class _Ty, size_t _Extent>
        inline constexpr bool _Is_span_v<span<_Ty, _Extent>> = true;
    }
    /**
     *  For when you want to assert an argument is a SPAN, but you haven't yet deduced the type its a span of yet.
     * 
     *  \note matches span<T>, span<T,EXTENT>, span<const T>, span<const T,EXTENT>, but not things that
     *  are CONVERTIBLE to span<T>
     */
    template <typename SPAN_T>
    concept ISpan = Private_::_Is_span_v<SPAN_T>;
    static_assert (ISpan<span<int>> and ISpan<span<int, 3>>);
    static_assert (not ISpan<std::string> and not ISpan<int>); // we don't include <string> in this module, but sometimes helpful to test/debug/document

    /**
     *  \brief use ISpanOfT<T> as a concept declaration for parameters where you want a span, but accept either T or const T
     * 
     *  Sadly, if I declare a function
     *      f (span<int>) {}
     *  and try to call it with:
     *      f (span<const int>{}) - that fails, whereas I think, considering the logic/intent, it probably should work.
     * 
     *  Anyhow, replacing the f declaration with the (almost as clear);
     *      template <ISpanOfT<int> SPAN_OF_T>
     *      f (SPAN_OF_T) {}
     * 
     *  fixes the problem.
     * 
     *  \note Aliases:
     *      SpanOfPossiblyConstT - but name just too long (though might be clearer).
     * 
     *  note - matches span<T>, span<T,EXTENT>, span<const T>, span<const T,EXTENT>, but not things that
     * 
     *  are CONVERTIBLE to span<T>
     *  \see https://stackoverflow.com/questions/62688814/stdspanconst-t-as-parameter-in-function-template
     */
    template <typename SPAN_T, typename T>
    concept ISpanOfT = Common::IAnyOf<remove_cvref_t<SPAN_T>, span<T>, span<const T>, span<T, SPAN_T::extent>, span<const T, SPAN_T::extent>>;
    static_assert (ISpanOfT<span<int>, int> and ISpanOfT<span<const int>, int> and ISpanOfT<span<const int, 3>, int> and not ISpanOfT<span<int>, char>);

    /**
     *  \brief convert a (possibly already const) span to the same span, but with const on the 'T' argument
     *
     *  I hope this function isn't needed often. And wherever it is needed, I think is a std::span<> design
     *  defect (or something I don't yet understand about how to use span better).
     * 
     *  But it appears if you declare function argument span<const T,EXT> and pass in a non-const T span, you get
     *  error messages about failure to deduce (at least on visual studio).
     * 
     *  See https://stackoverflow.com/questions/62688814/stdspanconst-t-as-parameter-in-function-template
     * 
     * 
     *  @todo consider if still needed/useful!
     */
    template <class T, size_t EXTENT>
    constexpr span<const T, EXTENT> ConstSpan (span<T, EXTENT> s);

    /**
     *  \brief NEltsOf(X) returns the number of elements in array argument (ie return sizeof (arg)/sizeof(arg[0]))
     *
     *      @todo   Found std::begin() could be used to replace old StartOfArray() macro -
     *              see if this too can be replaced with something in C++11?
     */
    template <typename ARRAY_TYPE, size_t SIZE_OF_ARRAY>
    constexpr size_t NEltsOf ([[maybe_unused]] const ARRAY_TYPE (&arr)[SIZE_OF_ARRAY]);

    /**
     *  \brief 'cast' the given POD data type argument to a span<const byte> - a bit like std::as_bytes, but taking different arguments
     * 
     *  \note only works on POD (trivially_v<T>) data
     *  \note returns address of argument, so use results before argument goes out of scope
     *  \note Similar to std::bit_cast<>, but always converts to span of bytes, and doesn't COPY anything - just 'casts' the elt to a span of bytes
     */
    template <typename T>
    span<const byte> AsBytes (const T& elt)
        requires (is_trivial_v<T>);

    /**
     *  \brief - like std::memcmp() - except count is in units of T (not bytes) and this function is
     *           constexpr, and this function allows nullptr arguments (if count == 0).
     * 
     *  \note Aliases: memcmp, MemCmp
     * 
     *  Pointer Overload: 
     *      \req  (count == 0 or lhs != nullptr);
     *      \req  (count == 0 or rhs != nullptr);
     * 
     *  Span Overload: 
     *      \req  lhs.size () == rhs.size ()
     * 
     *  \note - like std::memcmp() it returns an int < 0 for less, == 0 for equal, and > 0 for greater, but that corresponds
     *          backward compatibly to the strong_ordering C++20 type, so we use that for clarity going forward.
     */
    template <typename T>
    constexpr strong_ordering CompareBytes (const T* lhs, const T* rhs, size_t count);
    template <typename T>
    constexpr strong_ordering CompareBytes (span<const T> lhs, span<const T> rhs);
    template <typename T>
    constexpr strong_ordering CompareBytes (span<T> lhs, span<T> rhs);

    /**
     * \brief return true iff intersection of the two spans is non-empty (contains any bytes)
     * 
     *  Note this is similar to Range::Intersects, except for the business about openness/closedness and details at the edge conditions
     * 
     *  The only known use for this is assertions in CopySpanData that the spans don't overlap (memcpy vs memmove)
     */
    template <typename T1, typename T2, size_t E1, size_t E2>
    constexpr bool Intersects (span<T1, E1> lhs, span<T2, E2> rhs);

    /**
     *  \brief 'cast' a span of one thing to another, as if as_bytes, from_bytes; require span<T1...> and span<T2...> such that one T size is a multiple of the other
     * 
     *  This requirement on the same size in bytes of elements sizeof FROM_T must evenly divide sizeof TO_T (or the reverse).
     *  This is to allow the returned span{} to cover the same number of bytes.
     * 
     *  \note - TO_T == std::byte, this is the same as std::as_bytes or std::as_writable_bytes
     * 
     *  \ens resulting span same size_bytes () as src.size_bytes().
     * 
     *  \note Though this CAN be used with fixed-extent spans, the caller must then specify the fixed extent,
     *        which must be correct. Probably works most simply if the EXTENT in the TO_SPAN is dynamic_extent (or omitted).
     * 
     *  \note until Stroika v3.0d12 this was called SpanReInterpretCast (but does more than re_interpret_cast, cuz can change constness too).
     */
    template <ISpan TO_SPAN, typename FROM_T, size_t FROM_EXTENT>
    constexpr TO_SPAN SpanBytesCast (span<FROM_T, FROM_EXTENT> src)
        requires (sizeof (FROM_T) % sizeof (typename TO_SPAN::value_type) == 0 or sizeof (typename TO_SPAN::value_type) % sizeof (FROM_T) == 0);

    /**
     *  \brief Span-flavored memcpy/std::copy (copies from, to) - requires argument spans not overlap
     *
     *  like std::copy, except copies the data the spans point to/reference. Target span maybe larger than src,
     *  but must (require) be no smaller than src span;
     * 
     *  \req src.size () <= target.size ()      -- so that all of source can always be copied (else would need api/indicator of how much copied)
     *  \req not Intersects (src, target) - so non-overlapping
     * 
     *  \note somewhat unlike memcpy, its fine if the spans{} are empty ()
     *  
     *  Returns the subset of the target span filled (so a subspan of target).
     * 
     *  @see also CopyOverlappingBytes for 'memmove' - same API but where the data can overlap
     */
    template <Common::trivially_copyable FROM_T, size_t FROM_E, Common::trivially_copyable TO_T, size_t TO_E>
    constexpr span<TO_T, TO_E> CopyBytes (span<FROM_T, FROM_E> src, span<TO_T, TO_E> target)
        requires (same_as<remove_cvref_t<FROM_T>, remove_cvref_t<TO_T>>);

    /*
     *  \brief Span-flavored std::copy (copies from, to), works with spans, not iterators
     *
     *  Similar to CopyBytes, but works with non-trivially copyable data, as well as with differently sized
     *  data, where the individuals are preserved (fully type-safe); No requirement in size_bytes on the two
     *  spans, just on the SIZE of the two spans.
     * 
     *  \req from.size() <= to.size()
     *  \req not Intersects (src, target) - so non-overlapping
     *
     *  \returns the subspan of the target which was just filled in.
     *
     *  \note sizeof (FROM_T) may differ from sizeof (TO_T). So this can be used to downshift char16_t data
     *        to plain ASCII so long as the caller assures the source data is truly ascii first.
     *
     *  \note WAS CALLED CopySpanData_StaticCast - briefly - until 3.0d12
     */
    template <typename FROM_T, size_t FROM_E, typename TO_T, size_t TO_E>
    constexpr span<TO_T, TO_E> CopySpanData (span<const FROM_T, FROM_E> src, span<TO_T, TO_E> target);

    /**
     *  \brief Span-flavored memmove/std::copy_backwards (copies from, to) - ALLOWING argument spans to overlap
     *
     *  like std::copy_backward, except copies the data the spans point to/reference. Target span maybe larger than src,
     *  but must (require) be no smaller than src span;
     * 
     *  \req src.size () <= target.size ()      -- so that all of source can always be copied (else would need api/indicator of how much copied)
     *  
     *  Returns the subset of the target span filled (so a subspan of target).
     * 
     *  @see also CopyBytes
     */
    template <Common::trivially_copyable FROM_T, size_t FROM_E, Common::trivially_copyable TO_T, size_t TO_E>
    constexpr span<TO_T, TO_E> CopyOverlappingBytes (span<FROM_T, FROM_E> src, span<TO_T, TO_E> target)
        requires (same_as<remove_cvref_t<FROM_T>, remove_cvref_t<TO_T>>);

    /**
     *  \brief use Memory::OffsetOf(&CLASS::MEMBER) in place of offsetof(CLASS,MEMBER) to avoid compiler warnings, and cuz easier to 
     *         map from other constructors (e.g. StructFieldMetaInfo) cuz ptr to member legit C++ object, whereas CLASS and MEMBER are not.
     * 
     *  REPLACE calls to:
     *       offsetof (CLASS, MEMBER)
     * with:
     *       OffsetOf (&CLASS::MEMBER)
     * 
     *  \note   The current implementation exploits UNDEFINED BEHAVIOR.
     * 
     *          expr.add-5.sentence-2
     *              "If the expressions P and Q point to, respectively, elements x[i] and x[j] of 
     *              the same array object x, the expression P - Q has the value i - j; otherwise, the behavior is undefined.
     * 
     *  \par Example Usage
     *      \code
     *          struct  Person {
     *              String  firstName;
     *              String  lastName;
     *          };
     *          constexpr size_t kOffset_ = OffsetOf(&Person::lastName);
     *          static_assert (OffsetOf (&Person::firstName) == 0);         // NOTE - we WANT this to work, but for now cannot get constexpr stuff working
     *      \endcode
     * 
     *  \par Example Usage
     *      \code
     *          struct X1 {
     *              int a;
     *              int b;
     *          };
     *          void DoTest ()
     *          {
     *              assert (OffsetOf (&X1::a) == 0);
     *              assert (OffsetOf (&X1::b) >= sizeof (int));
     *          }
     *      \endcode
     * 
     *  @see https://gist.github.com/graphitemaster/494f21190bb2c63c5516
     *  @see https://en.cppreference.com/w/cpp/types/offsetof
     *  @see https://stackoverflow.com/questions/65940393/c-why-the-restriction-on-offsetof-for-non-standard-layout-objects-or-how-t
     * 
     *  \note   Tricky to get this to work with constexpr. See implementation for details.
     */
    template <typename OUTER_OBJECT, typename DATA_MEMBER_TYPE>
    constexpr size_t OffsetOf (DATA_MEMBER_TYPE (OUTER_OBJECT::*dataMember));

    /**
     *  \brief UninitializedConstructorFlag::eUninitialized is a flag to some memory routines to allocate without initializing
     * 
     *  This is mainly used as a performance tweak, for objects that don't need to be initialized, and can just be copied into.
     * 
     *  \note the APIS that use this typically require  static_assert (is_trivially_copyable_v<T>);
     */
    enum class UninitializedConstructorFlag {
        eUninitialized
    };

    /**
     *  \brief eUninitialized is a flag to some memory routines to allocate without initializing
     * 
     *  \see UninitializedConstructorFlag
     */
    using UninitializedConstructorFlag::eUninitialized;

    inline namespace Literals {
        /**
         *  \brief A utility for declaring constant bytes (byte literals).
         * 
         *  \req b <= 0xff
         * 
         *  \see https://stackoverflow.com/questions/75411756/how-do-i-declare-and-initialize-an-array-of-bytes-in-c
         */
        constexpr byte operator"" _b (unsigned long long b);
    }

    /**
     *  API to return memory allocation statistics. Generally - these will be inaccurate,
     *  unless certain defines are set in Memory.cpp - but at least some stats can be
     *  returned in either case.
     *
     *  Note - this currently only produces useful answers on windows, but can easily pull
     *  similar values out of /proc fs stuff with linux (nyi).
     *
     *  @todo   Does this belong in "Execution" module"
     */
    struct GlobalAllocationStatistics {
        constexpr GlobalAllocationStatistics () = default;

        size_t fTotalOutstandingAllocations{0};
        size_t fTotalOutstandingBytesAllocated{0};
        size_t fPageFaultCount{0};
        size_t fWorkingSetSize{0};
        size_t fPagefileUsage{0};
    };
    GlobalAllocationStatistics GetGlobalAllocationStatistics ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Memory_Common_h_*/
