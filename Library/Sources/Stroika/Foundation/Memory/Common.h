/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Common_h_
#define _Stroika_Foundation_Memory_Common_h_ 1

#include "../StroikaPreComp.h"

#include <memory>
#include <span>

#include "../Configuration/Common.h"

/**
 */

namespace Stroika::Foundation::Memory {

    /**
     *  \brief NEltsOf(X) returns the number of elements in array argument (ie return sizeof (arg)/sizeof(arg[0]))
     *
     *      @todo   Found std::begin() could be used to replace old StartOfArray() macro -
     *              see if this too can be replaced with something in C++11?
     */
    template <typename ARRAY_TYPE, size_t SIZE_OF_ARRAY>
    constexpr size_t NEltsOf ([[maybe_unused]] const ARRAY_TYPE (&arr)[SIZE_OF_ARRAY]);

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

    /**
     *  \brief - like std::memcmp() - except count is in units of T (not bytes) and this function is
     *           constexpr, and this function allows nullptr arguments (if count == 0).
     * 
     *  Pointer Overload: 
     *      \req  (count == 0 or lhs != nullptr);
     *      \req  (count == 0 or rhs != nullptr);
     * 
     *  Span Overload: 
     *      \req  lhs.size () == rhs.size ()
     */
    template <typename T>
    constexpr int MemCmp (const T* lhs, const T* rhs, size_t count);
    template <typename T>
    constexpr int MemCmp (span<const T> lhs, span<const T> rhs);
    template <typename T>
    constexpr int MemCmp (span<T> lhs, span<T> rhs);

    /**
     *  \brief use Memory::OffsetOf(&CLASS::MEMBER) in place of offsetof(CLASS,MEMBER) to avoid compiler warnings, and cuz easier to 
     *         map from other constructors (e.g. StructFieldMetaInfo) cuz ptr to member legit C++ object, whereas CLASS and MEMBER are not.
     * 
     *  REPLACE calls to:
     *       offsetof (CLASS, MEMBER)
     * with:
     *       OffsetOf (&CLASS::MEMBER)
     * 
     *  \note   This exploits UNDEFINED BEHAVIOR.
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
     *          static_assert (OffsetOf (&Person::firstName) == 0);
     *      \endcode
     * 
     *  @see https://gist.github.com/graphitemaster/494f21190bb2c63c5516
     *  @see https://en.cppreference.com/w/cpp/types/offsetof
     *  @see https://stackoverflow.com/questions/65940393/c-why-the-restriction-on-offsetof-for-non-standard-layout-objects-or-how-t
     * 
     *  \note   Tricky to get this to work with constexpr. Experimenting with OffsetOf_Constexpr
     * 
     *  TODO:
     *      @todo   Try to get this working more uniformly - regardless of is_default_constructible_v, and with constexpr, and
     *              more reliably portably, and detect errors somehow for cases where this cannot work, but not as widely warning
     *              as offsetof() - care about case of struct x { private: int a; public: int b; working}.
     */
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
    size_t OffsetOf (FIELD_VALUE_TYPE OWNING_OBJECT::*member);
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
    size_t constexpr OffsetOf_Constexpr (FIELD_VALUE_TYPE OWNING_OBJECT::*member);

    /**
     *  \brief UninitializedConstructorFlag::eUninitialized is a flag to some memory routines to allocate without initializing
     * 
     *  This is mainly used as a performance tweek, for objects that don't need to be initialized, and can just be copied into.
     * 
     *  \note the APIS that use this typically require  static_assert (is_trivially_copyable_v<T>);
     */
    enum class UninitializedConstructorFlag {
        eUninitialized
    };

    /**
     *  \brief UninitializedConstructorFlag::eUninitialized is a flag to some memory routines to allocate without initializing
     * 
     *  This is mainly used as a performance tweek, for objects that don't need to be initialized, and can just be copied into.
     * 
     *  \note the APIS that use this typically require  static_assert (is_trivially_copyable_v<T>);
     */
    constexpr UninitializedConstructorFlag eUninitialized = UninitializedConstructorFlag::eUninitialized;

    /**
     *  \brief A utility for declaring constant bytes (byte literals).
     * 
     *  \req b <= 0xff
     * 
     *  \see https://stackoverflow.com/questions/75411756/how-do-i-declare-and-initialize-an-array-of-bytes-in-c
     */
    constexpr std::byte operator""_b (unsigned long long b);

// clang liblib on macos missing this in xcode 14
#if __cpp_lib_bit_cast >= 201806L
    using std::bit_cast;
#else
    template <class To, class From>
    std::enable_if_t<sizeof (To) == sizeof (From) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>, To>
    // constexpr support needs compiler magic
    bit_cast (const From& src) noexcept
    {
        static_assert (std::is_trivially_constructible_v<To>, "This implementation additionally requires "
                                                              "destination type to be trivially constructible");

        To dst;
        std::memcpy (&dst, &src, sizeof (To));
        return dst;
    }
#endif

#if __cpp_lib_byteswap >= 202110L
    using std::byteswap;
#else
    template <class T>
    constexpr T byteswap (T n) noexcept
    {
        using std::byte;
        static_assert (std::has_unique_object_representations_v<T>, "T may not have padding bits");
        auto value_representation = bit_cast<array<byte, sizeof (T)>> (n);
        for (size_t i = 0; i < value_representation.size () / 2; ++i) {
            swap (value_representation[i], value_representation[value_representation.size () - i]);
        }
        return bit_cast<T> (value_representation);
#if 0
            if constexpr (sizeof (T) == 1) {
                return n;
            }
            else if constexpr (sizeof (T) == 2) {
                 uint8_t* na = reinterpret_cast< uint8_t*> (&n);
                std::swap (na[0], na[1]);
                return n;
            }
            else if constexpr (sizeof (T) == 4) {
                 uint16_t* na = reinterpret_cast< uint16_t*> (&n);
                 na[0] = byteswap (na[0]);
                 na[1] = byteswap (na[1]);
                std::swap (na[0], na[1]);
                return n;
            }
            else if constexpr (sizeof (T) == 8) {
                 uint32_t* na = reinterpret_cast< uint32_t*> (&n);
                 na[0] = byteswap (na[0]);
                 na[1] = byteswap (na[1]);
                std::swap (na[0], na[1]);
                return n;
            }
#endif
    }
#endif
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Memory_Common_h_*/
