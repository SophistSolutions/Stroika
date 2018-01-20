/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Endian_inl_
#define _Stroika_Foundation_Configuration_Endian_inl_ 1

#include <cstdint>

#include "../Debug/Assertions.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace Configuration {

/*
             ********************************************************************************
             *************************** Configuration::GetEndianness ***********************
             ********************************************************************************
             */
#if !qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy
            namespace Private_ {
                union EndianTester_ {
                    uint32_t sdat;
                    uint8_t  cdat[4];
                };
                static constexpr EndianTester_ kMix_{0x01020304};
            }
#endif
            inline constexpr Endian GetEndianness ()
            {
#if !qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy
                return (Private_::kMix_.cdat[0] == 4) ? Endian::eLittleByte :         // aka little endian
                           (Private_::kMix_.cdat[0] == 1) ? Endian::eBigByte :        // aka big endian
                               (Private_::kMix_.cdat[0] == 2) ? Endian::eLittleWord : // aka little PDP
                                   Endian::eBigWord;
#else
#if (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN) ||             \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || \
    defined(__BIG_ENDIAN__) ||                                             \
    defined(__ARMEB__) ||                                                  \
    defined(__THUMBEB__) ||                                                \
    defined(__AARCH64EB__) ||                                              \
    defined(_MIBSEB) ||                                                    \
    defined(__MIBSEB) ||                                                   \
    defined(__MIBSEB__)
                return Endian::eBigByte;
#endif
#if (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN) ||             \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || \
    defined(__LITTLE_ENDIAN__) ||                                             \
    defined(__ARMEL__) ||                                                     \
    defined(__THUMBEL__) ||                                                   \
    defined(__AARCH64EL__) ||                                                 \
    defined(_MIPSEL) ||                                                       \
    defined(__MIPSEL) ||                                                      \
    defined(__MIPSEL__) ||                                                    \
    defined(_M_IX86) ||                                                       \
    defined(_M_X64) ||                                                        \
    defined(_M_IA64) ||                                                       \
    defined(_M_ARM)
                return Endian::eLittle;
#endif
#endif
            }

            /*
             ********************************************************************************
             ************************* Configuration::EndianConverter ***********************
             ********************************************************************************
             */
            template <>
            inline uint16_t EndianConverter (uint16_t value, Endian from, Endian to)
            {
                Require (from == Endian::eBig or from == Endian::eLittle); // just cuz thats all thats implemented
                Require (to == Endian::eBig or to == Endian::eLittle);     // just cuz thats all thats implemented
                if (from == to) {
                    return value;
                }
                else {
                    Assert (sizeof (value) == 2);
                    return ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
                }
            }
            template <>
            inline uint32_t EndianConverter (uint32_t value, Endian from, Endian to)
            {
                Require (from == Endian::eBig or from == Endian::eLittle); // just cuz thats all thats implemented
                Require (to == Endian::eBig or to == Endian::eLittle);     // just cuz thats all thats implemented
                if (from == to) {
                    return value;
                }
                else {
                    Assert (sizeof (value) == 4);
                    return ((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value >> 8) & 0xFF00) | ((value >> 24) & 0xFF);
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Configuration_Endian_inl_*/
