/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Endian_inl_
#define _Stroika_Foundation_Configuration_Endian_inl_   1


#include    <cstdint>


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /*
             ********************************************************************************
             *************************** Configuration::GetEndianness ***********************
             ********************************************************************************
             */
#if     qCompilerAndStdLib_constexpr_Buggy || !qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy
            namespace   Private_ {
                union   EndianTester_ {
                    uint32_t    sdat;
                    uint8_t     cdat[4];
                };
                static constexpr EndianTester_ kMix_ { 0x01020304 };
            }
#endif
            inline  constexpr   Endian  GetEndianness ()
            {
#if     qCompilerAndStdLib_constexpr_Buggy || !qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy
                return
                    (Private_::kMix_.cdat[0] == 4) ?
                    Endian::eLittleByte :                   // aka little endian
                    (Private_::kMix_.cdat[0] == 1) ?
                    Endian::eBigByte :                      // aka big endian
                    (Private_::kMix_.cdat[0] == 2) ?
                    Endian::eLittleWord :                   // aka little PDP
                    Endian::eBigWord
                    ;
#else
#if     defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
                return Endian::eBigByte;
#endif
#if     defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) || \
    defined(_M_IX86) || defined(_M_X64) || defined(_M_IA64) || defined(_M_ARM)
                return Endian::eLittle;
#endif
#endif
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_Endian_inl_*/
