/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_VersionDefs_h_
#define _Stroika_Foundation_Configuration_VersionDefs_h_ 1

#include "../StroikaPreComp.h"

#if defined(__cplusplus)
#include <cstdint>
#endif

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  This file defines the part of the version stuff that can be safely included anywhere - even in a
 *  non-C++ file (e.g. resource compiler).
 */

#if defined(__cplusplus)
namespace Stroika::Foundation::Configuration {
#endif

#if defined(__cplusplus)
    /**
     *  Format
     *      |MAJOR|MINOR|STAGE|SubStage|Final|
     *      32    24    16    13       1     0
     * width    8     8     3      12     1
     *
     *  \note   When looking at a Binary32BitFullVersionType as bytes,
     *          high order byte = MAJOR
     *          next byte = MINOR
     *          next nibble (1/2 byte):
     *              kStroika_Version_Stage_Dev              (kStroika_Version_Stage_Dev<<1)                     =>  0x2
     *              kStroika_Version_Stage_Alpha            (kStroika_Version_Stage_Alpha<<1)                   =>  0x4
     *              kStroika_Version_Stage_Beta             (kStroika_Version_Stage_Beta<<1)                    =>  0x6
     *              kStroika_Version_Stage_ReleaseCandidate (kStroika_Version_Stage_ReleaseCandidate<<1)        =>  0x8
     *              kStroika_Version_Stage_Release          (kStroika_Version_Stage_Release<<1)                 =>  0xa
     *              low order bit part of substage but generally zero and ignoreable
     *          next nibble - part of substage but generally zero and ignorable
     *          next byte - low order 8 bits of substage << 1, so take that # and shift right  (divide by 2) to get substage
     *          low order bit boolean for 'final build' versus dev-builds
     *
     *  So Release 1.2b4 would be (in decimal place separated octets):
     *      1.2.96.9 (in hex 0x1.0x2.0x60.0x9)
     *
     *  So Release 3.0 would be (in decimal place separated octets):
     *      3.0.160.1  (in hex 0x3.0x0.0xa0.0x1)
     *
     *  So Release 3.0.1 would be (in decimal place separated octets):
     *      3.0.160.3  (in hex 0x3.0x0.0xa0.0x3)
     *
     *  \note
     *      Format UNTIL Stroika v2.0a151
     *          |MAJOR|MINOR|STAGE|SubStage|Final|
     *          32    25    17    9        1     0
     *     width    7     8     8      8      1
     */
    using Binary32BitFullVersionType = uint32_t;
#endif

    /**
     *  We use a MACRO here so we can use in languages other than C++, and so we can use these in #if macro pre-processor commands.
     */
#define kStroika_Version_Stage_Dev 0x1
#define kStroika_Version_Stage_Alpha 0x2
#define kStroika_Version_Stage_Beta 0x3
#define kStroika_Version_Stage_ReleaseCandidate 0x4
#define kStroika_Version_Stage_Release 0x5

    /**
     *  We use a MACRO here so we can use in languages other than C++, and so we can use these in #if macro pre-processor commands.
     *      @see Binary32BitFullVersionType for bit layout
     *
     *  \par Example Usage
     *      \code
     *          // be sure to #include "Stroika/Foundation/Configuration/StroikaVersion.h" for kStroika_Version_FullVersion else kStroika_Version_FullVersion acts as if 0, and always use old code
     *          #if     kStroika_Version_FullVersion >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 156, 0)
     *              use new function;
     *          #else
     *              use older API
     *          #endif
     *      \endcode
     */
#define Stroika_Make_FULL_VERSION(_Major_, _Minor_, _Stage_, _SubStage_, _FinalBuild_) \
    ((_Major_ << 24) |                                                                 \
     (_Minor_ << 16) |                                                                 \
     (_Stage_ << 13) |                                                                 \
     (_SubStage_ << 1) |                                                               \
     (_FinalBuild_))

#if defined(__cplusplus)
}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "VersionDefs.inl"

#endif /*_Stroika_Foundation_Configuration_VersionDefs_h_*/
