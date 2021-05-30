/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Common_inl_
#define _Stroika_Foundation_Memory_Common_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     ********************* Memory::GlobalAllocationStatistics ***********************
     ********************************************************************************
     */
    inline GlobalAllocationStatistics::GlobalAllocationStatistics ()
        : fTotalOutstandingAllocations{0}
        , fTotalOutstandingBytesAllocated{0}
        , fPageFaultCount{0}
        , fWorkingSetSize{0}
        , fPagefileUsage{0}
    {
    }

    /*
     ********************************************************************************
     ********************************* Memory::MemCmp *******************************
     ********************************************************************************
     */
    template <>
    constexpr int MemCmp (const uint8_t* lhs, const uint8_t* rhs, std::size_t count)
    {
        //Require (count == 0 or lhs != nullptr);
        //Require (count == 0 or rhs != nullptr);
        const uint8_t* li = lhs;
        const uint8_t* ri = rhs;
        for (; count--; li++, ri++) {
            if (int cmp = static_cast<int> (*li) - static_cast<int> (*ri)) {
                return cmp;
            }
        }
        return 0;
    }
    template <typename T>
    constexpr int MemCmp (const T* lhs, const T* rhs, size_t count)
    {
        return MemCmp (reinterpret_cast<const uint8_t*> (lhs), reinterpret_cast<const uint8_t*> (rhs), count * sizeof (T));
    }

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
    namespace Private {
        template <class _InIt, class _OutIt>
        inline void VC_BWA_std_copy (_InIt _First, _InIt _Last, _OutIt _Dest)
        {
            auto o = _Dest;
            for (auto i = _First; i != _Last; ++i, ++o) {
                *o = *i;
            }
        }
    }
#endif

    namespace PRIVATE_ {
        // @see https://gist.github.com/graphitemaster/494f21190bb2c63c5516 for more info on maybe how to
        // get this working with constexpr and without static object
        template <typename T1, typename T2>
        struct OffsetOfRequiringDefaultConstructibleObjectType_ {
#if qCompilerAndStdLib_default_constructor_initialization_issueWithExplicit_Buggy
            static inline constexpr T2 sObj_{T2{}};
#else
            static inline constexpr T2 sObj_{};
#endif
            static constexpr size_t offset (T1 T2::*member)
            {
                /*
                 *  UNDEFINED BEHAVIOR: it is undefined, but for the following reason: expr.add-5.sentence-2
                 * "If the expressions P and Q point to, respectively, elements x[i] and x[j] of 
                 * the same array object x, the expression P - Q has the value i - j; otherwise, the behavior is undefined."]
                 */
                return size_t (&(OffsetOfRequiringDefaultConstructibleObjectType_<T1, T2>::sObj_.*member)) - size_t (&OffsetOfRequiringDefaultConstructibleObjectType_<T1, T2>::sObj_);
            }
        };
    }

    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
    inline size_t constexpr OffsetOf_Constexpr (FIELD_VALUE_TYPE OWNING_OBJECT::*member)
    {
        return PRIVATE_::OffsetOfRequiringDefaultConstructibleObjectType_<FIELD_VALUE_TYPE, OWNING_OBJECT>::offset (member);
    }
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
    inline size_t OffsetOf (FIELD_VALUE_TYPE OWNING_OBJECT::*member)
    {
#if 0
        auto o = declval<OWNING_OBJECT> (); // function only valid in unevaluated contexts - produces link errors
        auto result = size_t (reinterpret_cast<const char*> (&(o.*member)) - reinterpret_cast<const char*> (&o));
#elif 0
        // gcc doesn't allow reinterpret_cast of nullptr: invalid cast from type �std::nullptr_t� to type �const ...*�
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wnull-dereference\"")
        const OWNING_OBJECT& o = *reinterpret_cast<const OWNING_OBJECT*> (nullptr);
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wnull-dereference\"")
        auto result = size_t (reinterpret_cast<const char*> (&(o.*member)) - reinterpret_cast<const char*> (&o));
#elif 0
        //TSAN detects undefined behavior
        const OWNING_OBJECT& o      = *reinterpret_cast<const OWNING_OBJECT*> (0);
        auto                 result = size_t (reinterpret_cast<const char*> (&(o.*member)) - reinterpret_cast<const char*> (&o));
#else
        // Still not totally legal for non-std-layout classes, but seems to work, and I haven't found a better way
        //      --LGP 2021-05-27
        alignas (OWNING_OBJECT) std::byte buf[sizeof (OWNING_OBJECT)]{};
        const OWNING_OBJECT&              o      = *reinterpret_cast<const OWNING_OBJECT*> (&buf);
        auto                              result = size_t (reinterpret_cast<const char*> (&(o.*member)) - reinterpret_cast<const char*> (&o));
#endif
        // Avoid #include - Ensure (result <= sizeof (OWNING_OBJECT));
        return result;
    }

}
#endif /*_Stroika_Foundation_Memory_Common_inl_*/
