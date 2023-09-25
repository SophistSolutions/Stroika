/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Common_inl_
#define _Stroika_Foundation_Memory_Common_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <cstring>

#include "../Common/Compare.h"
#include "../Debug/Assertions.h"
#include "../Debug/Sanitizer.h"

#include "Span.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     ********************************* Memory::NEltsOf ******************************
     ********************************************************************************
     */
    template <typename ARRAY_TYPE, size_t SIZE_OF_ARRAY>
    inline constexpr size_t NEltsOf ([[maybe_unused]] const ARRAY_TYPE (&arr)[SIZE_OF_ARRAY])
    {
        return SIZE_OF_ARRAY;
    }

    /*
     ********************************************************************************
     ********************************* Memory::MemCmp *******************************
     ********************************************************************************
     */
    template <>
    constexpr strong_ordering MemCmp (const uint8_t* lhs, const uint8_t* rhs, std::size_t count)
    {
        DISABLE_COMPILER_MSC_WARNING_START (5063)
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wconstant-evaluated\"");
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wtautological-compare\"");
        if constexpr (is_constant_evaluated ()) {
            //Require (count == 0 or lhs != nullptr);
            //Require (count == 0 or rhs != nullptr);
            const uint8_t* li = lhs;
            const uint8_t* ri = rhs;
            for (; count--; ++li, ++ri) {
                if (int cmp = static_cast<int> (*li) - static_cast<int> (*ri)) {
                    return Common::CompareResultNormalizer (cmp);
                }
            }
            return strong_ordering::equal;
        }
        else {
            if (count == 0) {
                return strong_ordering::equal;
            }
            return Common::CompareResultNormalizer (std::memcmp (lhs, rhs, count));
        }
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wtautological-compare\"");
        DISABLE_COMPILER_MSC_WARNING_END (5063)
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wconstant-evaluated\"");
    }
    template <typename T>
    constexpr strong_ordering MemCmp (const T* lhs, const T* rhs, size_t count)
    {
        return MemCmp (reinterpret_cast<const uint8_t*> (lhs), reinterpret_cast<const uint8_t*> (rhs), count * sizeof (T));
    }
    template <typename T>
    constexpr strong_ordering MemCmp (span<const T> lhs, span<const T> rhs)
    {
        Require (lhs.size () == rhs.size ());
        return MemCmp (lhs.data (), rhs.data (), lhs.size ());
    }
    template <typename T>
    constexpr strong_ordering MemCmp (span<T> lhs, span<T> rhs)
    {
        return MemCmp (ConstSpan (lhs), ConstSpan (rhs));
    }

    namespace Private_ {
        namespace OffsetOfImpl_ {
            namespace UsingRecursiveSideStruct_ {
                // OffsetOf_ BASED ON CODE FROM - https://gist.github.com/graphitemaster/494f21190bb2c63c5516
#pragma pack(push, 1)
                template <typename MEMBER, size_t N_PAD_BYTES>
                struct Pad {
                    char   pad[N_PAD_BYTES];
                    MEMBER m;
                };
#pragma pack(pop)
                template <typename MEMBER>
                struct Pad<MEMBER, 0> {
                    MEMBER m;
                };

                DISABLE_COMPILER_MSC_WARNING_START (4324);
                template <typename BASE, typename MEMBER, size_t O>
                struct MakeUnion {
                    union U {
                        char           c;
                        BASE           base;
                        Pad<MEMBER, O> pad;
                        constexpr U () noexcept
                            : c{} {};
                        ~U () = delete;
                    };
                    //constexpr static U* u{};      // old code did this, but doesn't work if MEMBER field has type which is not allowed to be constexpr --LGP 2023-08-20
                    constexpr static U* u{nullptr}; // don't actually allocate an object (maybe use declval instead?) - cuz else U not literal type sometimes if MEMBER obj type not literal
                };
                DISABLE_COMPILER_MSC_WARNING_END (4324);

                template <typename MEMBER, typename BASE_CLASS, typename ORIG_CLASS>
                struct offset_of_impl {
                    template <size_t off, auto union_part = MakeUnion<BASE_CLASS, MEMBER, off>::u>
                    // This gets called with nullptr as 'object' for computing diff below to avoid ever building the object (cuz just computing offsets)
                    Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED static constexpr ptrdiff_t offset2 (MEMBER ORIG_CLASS::*member)
                    {
                        if constexpr (off > sizeof (BASE_CLASS)) {
                            throw 1;
                        }
                        else {
                            const auto diff1 = &((static_cast<const ORIG_CLASS*> (&union_part->base))->*member);
                            const auto diff2 = &union_part->pad.m;
                            if (diff1 > diff2) {
                                constexpr auto MIN = (sizeof (MEMBER) < alignof (ORIG_CLASS)) ? sizeof (MEMBER) : alignof (ORIG_CLASS);
                                return offset2<off + MIN> (member);
                            }
                            else {
                                return off;
                            }
                        }
                    }
                };

                template <class MEMBER, class BASE_CLASS>
                tuple<MEMBER, BASE_CLASS> get_types (MEMBER BASE_CLASS::*); // never defined, never really called, just used to extract types with decltype()

                template <class TheBase = void, class TT>
                inline constexpr size_t offset_of (TT member)
                {
                    using T      = decltype (get_types (declval<TT> ()));
                    using Member = tuple_element_t<0, T>;
                    using Orig   = tuple_element_t<1, T>;
                    using Base   = conditional_t<is_void_v<TheBase>, Orig, TheBase>;
                    return static_cast<size_t> (offset_of_impl<Member, Base, Orig>::template offset2<0> (member));
                }
            }

            namespace RequiringDefaultConstructibleObjectType_ {
                // @see https://gist.github.com/graphitemaster/494f21190bb2c63c5516 for more info on maybe how to
                template <typename T1, typename T2>
                struct offset_of {
                    static constexpr size_t offset (T1 T2::*member)
                    {
                        union X {
                            array<char, sizeof (T2)> bytes;
                            T2                       obj;
                            X (){};
                            ~X (){};
                        } objAsUnion;
                        /*
                         * &&& maybe not undefined anymore
                         *  UNDEFINED BEHAVIOR: it is undefined, but for the following reason: expr.add-5.sentence-2
                         * "If the expressions P and Q point to, respectively, elements x[i] and x[j] of 
                         * the same array object x, the expression P - Q has the value i - j; otherwise, the behavior is undefined."]
                         */
                        return size_t (&(objAsUnion.obj.*member)) - size_t (&objAsUnion.obj);
                    }
                };
            }

            namespace UsingAlignedByteArrayBuf_ {
                template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
                inline size_t offset_of (FIELD_VALUE_TYPE OWNING_OBJECT::*member)
                {
                    // Still not totally legal for non-std-layout classes, but seems to work, and I haven't found a better way
                    //      --LGP 2021-05-27
                    alignas (OWNING_OBJECT) std::byte buf[sizeof (OWNING_OBJECT)]{};
                    const OWNING_OBJECT&              o = *reinterpret_cast<const OWNING_OBJECT*> (&buf);
                    auto result = size_t (reinterpret_cast<const char*> (&(o.*member)) - reinterpret_cast<const char*> (&o));
                    // Avoid #include - Ensure (result <= sizeof (OWNING_OBJECT));
                    return result;
                }
            }

            namespace UseExplicitDefaultConstructibleStaticInstance_ {
                // @see https://gist.github.com/graphitemaster/494f21190bb2c63c5516 for more info on maybe how to
                // get this working with constexpr and without static object
                template <typename T1, typename T2>
                struct offset_of_ {
                    static inline constexpr T2 sObj_{};
                    static constexpr size_t    offset (T1 T2::*member)
                    {
                        /*
                         *  UNDEFINED BEHAVIOR: it is undefined, but for the following reason: expr.add-5.sentence-2
                         * "If the expressions P and Q point to, respectively, elements x[i] and x[j] of 
                         * the same array object x, the expression P - Q has the value i - j; otherwise, the behavior is undefined."]
                         */
                        return size_t (&(offset_of_<T1, T2>::sObj_.*member)) - size_t (&offset_of_<T1, T2>::sObj_);
                    }
                };
            }

            namespace UsingSimpleUnionToConstructActualObj_ {

                template <typename OUTER_OBJECT, typename DATA_MEMBER_TYPE>
                inline constexpr size_t offset_of (DATA_MEMBER_TYPE (OUTER_OBJECT::*dataMember))
                {
                    // NOT real assert - just tmphack to test
                    //   auto a1 = Private_::OffsetOfImpl_::UsingRecursiveSideStruct_::offset_of<OUTER_OBJECT> (dataMember);
                    //    auto a2 = PRIVATE_::OffsetOfImpl_::UsingAlignedByteArrayBuf_ (dataMember);
                    //   auto a3 = PRIVATE_::OffsetOfImpl_::RequiringDefaultConstructibleObjectType_::offset_of<DATA_MEMBER_TYPE, OUTER_OBJECT>::offset (dataMember);
                    //WeakAssert (a1 == a2);

                    union X {
                        array<char, sizeof (OUTER_OBJECT)> bytes;
                        OUTER_OBJECT                       obj;
                        X (){};
                        ~X (){};
                    } objAsUnion;

                    //     auto a4 = size_t (&(objAsUnion.obj.*dataMember)) - size_t (&objAsUnion.bytes);

                    /*
                * &&& maybe not undefined anymore
                 *  UNDEFINED BEHAVIOR: it is undefined, but for the following reason: expr.add-5.sentence-2
                 * "If the expressions P and Q point to, respectively, elements x[i] and x[j] of 
                 * the same array object x, the expression P - Q has the value i - j; otherwise, the behavior is undefined."]
                 */
                    return size_t (&(objAsUnion.obj.*dataMember)) - size_t (&objAsUnion.obj);

                    //https://stackoverflow.com/questions/12141446/offset-from-member-pointer-without-temporary-instance
                    // return Private_::OffsetOf2_::offset_of<OUTER_OBJECT> (dataMember);
                }

            }
        }
    }

    template <typename OUTER_OBJECT, typename DATA_MEMBER_TYPE>
    inline constexpr size_t OffsetOf (DATA_MEMBER_TYPE (OUTER_OBJECT::*dataMember))
    {
        //constexpr bool          kTestAllWays_ = false;
        constexpr bool          kTestAllWays_ = true;
        [[maybe_unused]] size_t r1;
        [[maybe_unused]] size_t r2;
        [[maybe_unused]] size_t r3;
        [[maybe_unused]] size_t r4;
        if constexpr (kTestAllWays_) {
            /*
             *  Setup to test/try each implementation. One with apparently best shot of working constexpr is UsingRecursiveSideStruct_
             *  HOWEVER, it requires (on unix) setting -ftemplate-depth=5000 flags, and even then, doesn't really work constexpr (complains about dereferencing nullptr).
             * 
             *  Don't give up, but no success so far. MAYBE works OK/portably without the constexpr stuff. Dunno. Probably NOT for the same deref-nullptr reason.
             */
#if defined(_MSC_VER)
            // Don't bother compiling for gcc/clang cuz fails on some compilers without -fdepth= flag, and no point since wont really work right even
            // if you provide that flag. REVISIT in the future maybe, but for now don't even bother calling
            r1 = Private_::OffsetOfImpl_::UsingRecursiveSideStruct_::offset_of<OUTER_OBJECT> (dataMember);
#endif
            r2 = Private_::OffsetOfImpl_::RequiringDefaultConstructibleObjectType_::offset_of<DATA_MEMBER_TYPE, OUTER_OBJECT>::offset (dataMember);
            r3 = Private_::OffsetOfImpl_::UsingAlignedByteArrayBuf_::offset_of<DATA_MEMBER_TYPE, OUTER_OBJECT> (dataMember);
            if constexpr (is_trivially_default_constructible_v<OUTER_OBJECT>) {
                r4 = Private_::OffsetOfImpl_::UseExplicitDefaultConstructibleStaticInstance_::offset_of_<DATA_MEMBER_TYPE, OUTER_OBJECT>::offset (dataMember);
            }
        }
        size_t r5 = Private_::OffsetOfImpl_::UsingSimpleUnionToConstructActualObj_::offset_of<OUTER_OBJECT, DATA_MEMBER_TYPE> (dataMember);
        size_t rr = r5;
        if (not is_constant_evaluated () and kTestAllWays_) {
#if defined(_MSC_VER)
            Assert (r1 == rr);
#endif
            Assert (r2 == rr);
            Assert (r3 == rr);
            if constexpr (is_trivially_default_constructible_v<OUTER_OBJECT>) {
                Assert (r4 == rr);
            }
        }
        return rr;
    }

    /*
     ********************************************************************************
     *************************** Memory::operator""_b *******************************
     ********************************************************************************
     */
    constexpr std::byte operator""_b (unsigned long long b)
    {
        Require (b <= 0xff);
        return static_cast<std::byte> (b);
    }

    ////////////////////// DEPRECATED .//////////////////////////
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
    [[deprecated ("Since Stroika v3.0d2 - just use OffsetOf")]] inline size_t constexpr OffsetOf_Constexpr (FIELD_VALUE_TYPE OWNING_OBJECT::*member)
    {
        return OffsetOf (member);
    }

}
#endif /*_Stroika_Foundation_Memory_Common_inl_*/
