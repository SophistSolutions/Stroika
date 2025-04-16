/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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

#if qCompilerAndStdLib_illunderstood_ispan_Buggy
    namespace Private_ {
        template <class>
        inline constexpr bool _Is_span_v = false;
        template <class _Ty, size_t _Extent>
        inline constexpr bool _Is_span_v<span<_Ty, _Extent>> = true;
    }
#endif

    /**
     *  For when you want to assert an argument is a SPAN, but you haven't yet deduced the type its a span of yet.
     * 
     *  \note matches span<T>, span<T,EXTENT>, span<const T>, span<const T,EXTENT>, but not things that
     *  are CONVERTIBLE to span<T>
     */
#if qCompilerAndStdLib_illunderstood_ispan_Buggy
    template <typename SPAN_T>
    concept ISpan = Private_::_Is_span_v<SPAN_T>;
#else
    template <typename SPAN_T>
    concept ISpan = requires (SPAN_T t) {
        {
            []<typename T1, size_t E1> (span<T1, E1>) {}(t)
        };
    };
#endif
#if qCompilerAndStdLib_illunderstood_ispan_Buggy
    static_assert (ISpan<span<int>> and ISpan<span<int, 3>>);
#else
    static_assert (ISpan<span<int>> and ISpan<span<int, 3>> and ISpan<const span<const int, 3>>);
#endif
    static_assert (not ISpan<std::string> and not ISpan<int> and not ISpan<vector<int>>); // we don't include <string>/<vector> in this module, but sometimes helpful to test/debug/document

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
     *  @aliases
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
     *  \brief Can safely cast span<FROM_T,FROM_EXTENT> to a TO_SPAN (where the underlying types are POD - plain old data - types - roughly)
     *
     *  \note - this requires the two spans to have the same number of bytes (cannot always be fully determined at compile time).
     *          But this returns true if its possible.
     * 
     *  \note its perfectly reasonable to span cast from span<uint32_t> to span<byte> - so long as the two spans have the same size_bytes()
     * 
     *  \note this also requires trivially_copyable on the types. Nothing REALLY requires that. But its more likely a bug than
     *        a feature if you are using types for which that is not true, so fail here. And force a more careful exam with explicit
     *        reinterpret_casts...
     * 
     *  \note This doesn't allow casting away constness of the underlying value_type (though it ignores the
     *        constness of the span itself).
     */
    template <typename TO_SPAN, typename FROM_SPAN>
    concept ISpanBytesCastable = (ISpan<TO_SPAN> and Common::trivially_copyable<typename TO_SPAN::value_type>) and
                                 (ISpan<FROM_SPAN> and Common::trivially_copyable<typename FROM_SPAN::value_type>) and
                                 (is_const_v<typename TO_SPAN::value_type> or not is_const_v<typename FROM_SPAN::value_type>) and
                                 (sizeof (typename FROM_SPAN::value_type) % sizeof (typename TO_SPAN::value_type) == 0 or
                                  sizeof (typename TO_SPAN::value_type) % sizeof (typename FROM_SPAN::value_type) == 0);

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
     *  \note this is sometimes useful to reduce deduction ambiguities, and cases where templates convert between
     *        multiple levels of deduction/inference
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
     *  \brief 'cast' the given POD data type argument to a span<const byte> - a bit like std::as_bytes, but any 'trivial' type T as argument (as_bytes takes span)
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
     *           constexpr, and this function allows nullptr arguments (if count == 0), and it returns strong_ordering, and provides
     *           helpful overloads.
     * 
     *  @aliases memcmp, MemCmp
     * 
     *  Pointer Overload: 
     *      \pre  (count == 0 or lhs != nullptr);
     *      \pre  (count == 0 or rhs != nullptr);
     * 
     *  Span Overload: 
     *      \pre  lhs.size () == rhs.size ()
     * 
     *  \note - like std::memcmp() it returns an int < 0 for less, == 0 for equal, and > 0 for greater, but that corresponds
     *          backward compatibly to the strong_ordering C++20 type, so we use that for clarity going forward.
     * 
     *  \note - This is LOGICALLY CompareBytes (span<const T> lhs, span<const T> rhs), but use use span<T> because unfortunately
     *          static_assert (not assignable_from<span<int>, span<const int>>);    // which makes no sense to me
     */
    template <typename T>
    constexpr strong_ordering CompareBytes (const T* lhs, const T* rhs, size_t count);
    template <typename TL, size_t EL, typename TR, size_t ER>
    constexpr strong_ordering CompareBytes (span<TL, EL> lhs, span<TR, ER> rhs)
        requires (same_as<remove_cvref_t<TL>, remove_cvref_t<TR>> and is_trivial_v<TL>);

    /**
     * \brief return true iff intersection of the two spans is non-empty (contains any bytes)
     * 
     *  Note this is similar to Range::Intersects, except for the business about openness/closedness and details at the edge conditions
     * 
     *  \note - I remember from the days of segmented architectures, this may not be 100% safe, or done correctly. But occasionally important to check
     *        and not sure how todo better. Should work fine on any modern processor I'm aware of --LGP 2025-04-16
     */
    template <typename T1, typename T2, size_t E1, size_t E2>
    constexpr bool Intersects (span<T1, E1> lhs, span<T2, E2> rhs);

    /**
     *  \brief 'cast' a span of one thing to another, as if as_bytes, from_bytes; require span<T1...> and span<T2...> such that one T size is a multiple of the other
     * 
     *  \pre ((src.size_bytes () / sizeof (TO_T)) * sizeof (TO_T) == src.size_bytes ());    - so this doesn't change size in bytes of span
     * 
     *  This requirement on the same size in bytes of elements sizeof FROM_T must evenly divide sizeof TO_T (or the reverse).
     *  This is to allow the returned span{} to cover the same number of bytes.
     * 
     *  \note - TO_T == std::byte, this is the same as std::as_bytes or std::as_writable_bytes
     * 
     *  \post resulting span same size_bytes () as src.size_bytes().
     * 
     *  \note Though this CAN be used with fixed-extent spans, the caller must then specify the fixed extent,
     *        which must be correct. Probably works most simply if the EXTENT in the TO_SPAN is dynamic_extent (or omitted).
     * 
     *  \note until Stroika v3.0d12 this was called SpanReInterpretCast
     * 
     *  \note this allows compiling SpanBytesCast<span<uint32_t>> (span<byte>{}) - which may work fine, or may assert out if size of argument
     *        not divisible by 4 (EXAMPLE).
     */
    template <ISpan TO_SPAN, ISpanBytesCastable<TO_SPAN> FROM_SPAN>
    constexpr TO_SPAN SpanBytesCast (FROM_SPAN src);

    /**
     *  \brief Span-flavored memcpy/std::copy (copies from, to) - requires argument spans not overlap, requires src.size <= to.size()
     *
     *  like std::copy, except copies the data the spans point to/reference. Target span maybe larger than src,
     *  but must (require) be no smaller than src span;
     * 
     *  \pre src.size () <= target.size ()      -- so that all of source can always be copied (else would need api/indicator of how much copied)
     *  \pre not Intersects (src, target) - so non-overlapping
     * 
     *  \note somewhat unlike memcpy, its fine if the spans{} are empty ()
     *  
     *  Returns the subset of the target span filled (so a subspan of target).
     * 
     *  @see also CopyOverlappingBytes for 'memmove' - same API but where the data can overlap
     */
    template <Common::trivially_copyable FROM_T, size_t FROM_E, Common::trivially_copyable TO_T, size_t TO_E>
    constexpr span<TO_T, TO_E> CopyBytes (span<FROM_T, FROM_E> src, span<TO_T, TO_E> target) noexcept
        requires (same_as<remove_cv_t<FROM_T>, TO_T>);

    /**
     *  \brief Span-flavored memmove/ (copies from, to) - ALLOWING argument spans to overlap
     *
     *  like std::copy_backward, except copies the data the spans point to/reference. Target span maybe larger than src,
     *  but must (require) be no smaller than src span;
     * 
     *  \pre src.size () <= target.size ()      -- so that all of source can always be copied (else would need api/indicator of how much copied)
     *  
     *  Returns the subset of the target span filled (so a subspan of target).
     * 
     *  @aliases could have been called CopyPotentiallyOverlappingBytes or CopyPossiblyOverlappingBytes
     * 
     *  @see also CopyBytes
     */
    template <Common::trivially_copyable FROM_T, size_t FROM_E, Common::trivially_copyable TO_T, size_t TO_E>
    constexpr span<TO_T, TO_E> CopyOverlappingBytes (span<FROM_T, FROM_E> src, span<TO_T, TO_E> target) noexcept
        requires (same_as<remove_cv_t<FROM_T>, TO_T>);

    /*
     *  \brief Span-flavored std::copy (copies from, to), works with spans, not iterators, works with different sized from/to types
     *
     *  Similar to CopyBytes, but works with non-trivially copyable data, as well as with differently sized
     *  data (strides), where the individuals are preserved (fully type-safe); No requirement in size_bytes on the two
     *  spans, just on the SIZE of the two spans.
     * 
     *  \pre from.size() <= to.size()
     *  \pre not Intersects (src, target) - so non-overlapping
     *
     *  \returns the subspan of the target which was just filled in.
     * 
     *  \note @todo CONSIDER how similar/different this is from ranges::copy, or ranges_uninitialized_copy
     *
     *  \note sizeof (FROM_T) may differ from sizeof (TO_T). So this can be used to downshift char16_t data
     *        to plain ASCII so long as the caller assures the source data is truly ascii first.
     *
     *  \note WAS CALLED CopySpanData_StaticCast - briefly - until 3.0d12
     */
    template <typename FROM_T, size_t FROM_E, typename TO_T, size_t TO_E>
    constexpr span<TO_T, TO_E> CopySpanData (span<FROM_T, FROM_E> src, span<TO_T, TO_E> target)
        requires (not is_const_v<TO_T>);

    /**
     * like CopySpanData but src and target are POTENTIALLY overlapping
     * BUT for this - dont need separate FROM_T and TO_T
     *
     *  @aliases could have been called CopyPotentiallyOverlappingSpanData or CopyPossiblyOverlappingSpanData
     */
    template <typename T, size_t FROM_E, size_t TO_E>
    constexpr span<T, TO_E> CopyOverlappingSpanData (span<T, FROM_E> src, span<T, TO_E> target)
        requires (not is_const_v<T>);

    /*
     * \brief Logic to insert a span of elements into another span of elements (assuming caller externally assured enuf space)
     * 
     *  \req intoLiveSpan subspan of intoReservedSpan, starting at same offset
     *  \req enough space in intoReservedSpan to insert copyFrom (at position at) - ie at + copyFrom.size() <= intoReservedSpan.size()
     * 
     *  Leaves intoReservedSpan unchanged (doesn't allocate memory).
     *  But expands intoLiveSpan, and returns updated span.
     * 
     *  This assures the right constructors/destructors/move operators called on appropriate span elements.
     * 
     *  \note - this code assumes no exceptions copying elements. Generally safe, but not 100% guaranteed.
     *          @todo add appropriate 'T' is noexcept copyable requirement
     * 
     *  \par Example Usage
     *      \code
     *          // first ensure capacity large enuf, then...
     *          this->fSize_ = Memory::Insert (span{this->data (), size ()}, span{this->data (), capacity ()}, at, copyFrom).size ();
     *      \endcode
     * 
     *  @todo consider if we could allow 'copyFrom' to also be a 'moveFrom'. Generally wouldn't matter, but might
     *        if copy of objects was expensive, but move cheap (like std::vector = T for example).
     */
    template <ISpan INTO_SPAN, ISpan FROM_SPAN>
        requires (same_as<remove_const_t<typename INTO_SPAN::value_type>, remove_const_t<typename FROM_SPAN::value_type>>)
    remove_cvref_t<INTO_SPAN> Insert (const INTO_SPAN& intoLiveSpan, const INTO_SPAN& intoReservedSpan, size_t at, const FROM_SPAN& copyFrom) noexcept;

    /*
     * \brief Logic to remove span of elements from another span of elements (handling calling right move/ctor/dtors for elements)
     * 
     *  \req intoLiveSpan subspan of intoReservedSpan, starting at same offset
     *  \req enough space in intoReservedSpan to insert copyFrom (at position at)
     * 
     *  Leaves intoReservedSpan unchanged (doesn't allocate memory).
     *  But expands intoLiveSpan, and returns updated live span.
     * 
     *  This assures the right constructors/destructors/move operators called on appropriate span elements.
     * 
     *  \note - this code assumes no exceptions copying elements. Generally safe, but not 100% guaranteed.
     *          @todo add appropriate 'T' is noexcept copyable requirement
     * 
     *  \par Example Usage
     *      \code
     *          this->fSize_ = Memory::Remove (span{this->data (), size ()}, span{this->data (), capacity ()}, from, to).size ();
     *      \endcode
     */
    template <ISpan FROM_SPAN>
        requires (not is_const_v<typename FROM_SPAN::value_type>)
    remove_cvref_t<FROM_SPAN> Remove (FROM_SPAN&& spanToEdit, FROM_SPAN&& reservedSpan, size_t from, size_t to) noexcept;

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
    constexpr size_t OffsetOf (DATA_MEMBER_TYPE (OUTER_OBJECT::* dataMember));

    /**
     *  \brief UninitializedConstructorFlag::eUninitialized is a flag to some memory routines to allocate without initializing
     * 
     *  This is mainly used as a performance tweak, for objects that don't need to be initialized, and can just be copied into.
     * 
     *  \note the APIS that use this typically require  static_assert (is_trivially_copyable_v<T>);
     */
    enum class UninitializedConstructorFlag {
        eUninitialized,
        eUninitializedIfTrivial
    };

    /**
     *  \brief eUninitialized is a flag to some memory routines to allocate without initializing (static_assert T is trivial)
     * 
     *  \see UninitializedConstructorFlag
     */
    using UninitializedConstructorFlag::eUninitialized;

    /**
     *  \brief eUninitialized is a flag to some memory routines to allocate without initializing (if T is not trivial)
     * 
     *  \see UninitializedConstructorFlag
     */
    using UninitializedConstructorFlag::eUninitializedIfTrivial;

    inline namespace Literals {
        /**
         *  \brief A utility for declaring constant bytes (byte literals).
         * 
         *  \pre b <= 0xff
         * 
         *  \see https://stackoverflow.com/questions/75411756/how-do-i-declare-and-initialize-an-array-of-bytes-in-c
         */
        constexpr byte operator""_b (unsigned long long b);
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
