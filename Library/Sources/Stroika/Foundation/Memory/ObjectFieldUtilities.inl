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

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     *********************** ConvertPointerToDataMemberToOffset *********************
     ********************************************************************************
     */
    namespace Private_ {


        /// BASED ON CODE FROM - https://gist.github.com/graphitemaster/494f21190bb2c63c5516 ---

#pragma pack(push, 1)
        template <typename Member, std::size_t O>
        struct Pad {
            char   pad[O];
            Member m;
        };
#pragma pack(pop)
        template <typename Member>
        struct Pad<Member, 0> {
            Member m;
        };

        template <typename Base, typename Member, std::size_t O>
        struct MakeUnion {
            union U {
                char           c;
                Base           base;
                Pad<Member, O> pad;
                constexpr U () noexcept
                    : c{} {};
                ~U () = delete;
            };
            constexpr static U* u{};
        };

        template <typename Member, typename Base, typename Orig>
        struct offset_of_impl {
            template <std::size_t off, auto union_part = MakeUnion<Base, Member, off>::u>
            static constexpr std::ptrdiff_t offset2 (Member Orig::*member)
            {
                if constexpr (off > sizeof (Base)) {
                    throw 1;
                }
                else {
                    const auto diff1 = &((static_cast<const Orig*> (&union_part->base))->*member);
                    const auto diff2 = &union_part->pad.m;
                    if (diff1 > diff2) {
                        constexpr auto MIN = sizeof (Member) < alignof (Orig) ? sizeof (Member) : alignof (Orig);
                        return offset2<off + MIN> (member);
                    }
                    else {
                        return off;
                    }
                }
            }
        };

        template <class Member, class Base>
        std::tuple<Member, Base> get_types (Member Base::*);

        template <class TheBase = void, class TT>
        inline constexpr std::ptrdiff_t offset_of (TT member)
        {
            using T      = decltype (get_types (std::declval<TT> ()));
            using Member = std::tuple_element_t<0, T>;
            using Orig   = std::tuple_element_t<1, T>;
            using Base   = std::conditional_t<std::is_void_v<TheBase>, Orig, TheBase>;
            return offset_of_impl<Member, Base, Orig>::template offset2<0> (member);
        }

        template <auto member, class TheBase = void>
        inline constexpr std::ptrdiff_t offset_of ()
        {
            return offset_of<TheBase> (member);
        }

    }
    template <typename OUTER_OBJECT, typename DATA_MEMBER_TYPE>
    inline constexpr size_t ConvertPointerToDataMemberToOffset (DATA_MEMBER_TYPE (OUTER_OBJECT::*dataMember))
    {
        //https://stackoverflow.com/questions/12141446/offset-from-member-pointer-without-temporary-instance
        // return reinterpret_cast<char*> (&(((OUTER_OBJECT*)0)->*dataMember)) - reinterpret_cast<char*> (0);
      //  return Private_::offset_of_impl<DATA_MEMBER_TYPE, OUTER_OBJECT, OUTER_OBJECT>::offset2 (dataMember);
        return Private_::offset_of (dataMember);
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
        std::byte* adjustedAggregatedMember = reinterpret_cast<std::byte*> (static_cast<AGGREGATED_OBJECT_TYPE*> (aggregatedMember));
        ptrdiff_t  adjustment               = static_cast<ptrdiff_t> (ConvertPointerToDataMemberToOffset (aggregatedPtrToMember));
        return reinterpret_cast<OUTER_OBJECT*> (adjustedAggregatedMember - adjustment);
    }
    template <typename APPARENT_MEMBER_TYPE, typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    inline const OUTER_OBJECT* GetObjectOwningField (const APPARENT_MEMBER_TYPE* aggregatedMember,
                                                     AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember))
    {
        RequireNotNull (aggregatedMember);
        RequireNotNull (aggregatedPtrToMember);
        const std::byte* adjustedAggregatedMember =
            reinterpret_cast<const std::byte*> (static_cast<const AGGREGATED_OBJECT_TYPE*> (aggregatedMember));
        ptrdiff_t adjustment = static_cast<ptrdiff_t> (ConvertPointerToDataMemberToOffset (aggregatedPtrToMember));
        return reinterpret_cast<const OUTER_OBJECT*> (adjustedAggregatedMember - adjustment);
    }

}
#endif /*_Stroika_Foundation_Memory_ObjectFieldUtilities_inl_*/
