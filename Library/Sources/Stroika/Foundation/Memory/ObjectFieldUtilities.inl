/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_ObjectFieldUtilities_inl_
#define _Stroika_Foundation_Memory_ObjectFieldUtilities_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/Assertions.h"
#include "../Debug/Sanitizer.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     *********************** ConvertPointerToDataMemberToOffset *********************
     ********************************************************************************
     */
    namespace Private_ {
        namespace OffsetOf_ {
            // OffsetOf_ BASED ON CODE FROM - https://gist.github.com/graphitemaster/494f21190bb2c63c5516
#pragma pack(push, 1)
            template <typename MEMBER, size_t O>
            struct Pad {
                char   pad[O];
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
                constexpr static U* u{nullptr}; // don't actually allocate an object (maybe use declval instead?)
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
                            constexpr auto MIN = sizeof (MEMBER) < alignof (ORIG_CLASS) ? sizeof (MEMBER) : alignof (ORIG_CLASS);
                            return offset2<off + MIN> (member);
                        }
                        else {
                            return off;
                        }
                    }
                }
            };

            template <class MEMBER, class BASE_CLASS>
            tuple<MEMBER, BASE_CLASS> get_types (MEMBER BASE_CLASS::*); // never defined, never really called, just used to extrac types with decltype()

            template <class TheBase = void, class TT>
            inline constexpr ptrdiff_t offset_of (TT member)
            {
                using T      = decltype (get_types (declval<TT> ()));
                using Member = tuple_element_t<0, T>;
                using Orig   = tuple_element_t<1, T>;
                using Base   = conditional_t<is_void_v<TheBase>, Orig, TheBase>;
                return offset_of_impl<Member, Base, Orig>::template offset2<0> (member);
            }
        }
    }
    template <typename OUTER_OBJECT, typename BASE_OBJECT, typename DATA_MEMBER_TYPE>
    inline constexpr size_t ConvertPointerToDataMemberToOffset (DATA_MEMBER_TYPE (BASE_OBJECT::*dataMember))
    {
        return Private_::OffsetOf_::offset_of<OUTER_OBJECT> (dataMember);
    }
    template <typename OUTER_OBJECT, typename DATA_MEMBER_TYPE>
    inline constexpr size_t ConvertPointerToDataMemberToOffset (DATA_MEMBER_TYPE (OUTER_OBJECT::*dataMember))
    {
        //https://stackoverflow.com/questions/12141446/offset-from-member-pointer-without-temporary-instance
        return Private_::OffsetOf_::offset_of<OUTER_OBJECT> (dataMember);
    }

    /*
     ********************************************************************************
     *************************** GetObjectOwningField *******************************
     ********************************************************************************
     */
    template <typename APPARENT_MEMBER_TYPE, typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    inline OUTER_OBJECT* GetObjectOwningField (APPARENT_MEMBER_TYPE* aggregatedMember, AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember))
    {
        RequireNotNull (aggregatedMember);
        RequireNotNull (aggregatedPtrToMember);
        byte*     adjustedAggregatedMember = reinterpret_cast<byte*> (static_cast<AGGREGATED_OBJECT_TYPE*> (aggregatedMember));
        ptrdiff_t adjustment               = static_cast<ptrdiff_t> (ConvertPointerToDataMemberToOffset (aggregatedPtrToMember));
        return reinterpret_cast<OUTER_OBJECT*> (adjustedAggregatedMember - adjustment);
    }
    template <typename APPARENT_MEMBER_TYPE, typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    inline const OUTER_OBJECT* GetObjectOwningField (const APPARENT_MEMBER_TYPE* aggregatedMember,
                                                     AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember))
    {
        RequireNotNull (aggregatedMember);
        RequireNotNull (aggregatedPtrToMember);
        const byte* adjustedAggregatedMember = reinterpret_cast<const byte*> (static_cast<const AGGREGATED_OBJECT_TYPE*> (aggregatedMember));
        ptrdiff_t adjustment = static_cast<ptrdiff_t> (ConvertPointerToDataMemberToOffset (aggregatedPtrToMember));
        return reinterpret_cast<const OUTER_OBJECT*> (adjustedAggregatedMember - adjustment);
    }

}
#endif /*_Stroika_Foundation_Memory_ObjectFieldUtilities_inl_*/
