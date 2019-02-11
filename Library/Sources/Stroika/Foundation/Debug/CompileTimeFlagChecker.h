/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_CompileTimeFlagChecker_h_
#define _Stroika_Foundation_Debug_CompileTimeFlagChecker_h_ 1

#include "../StroikaPreComp.h"

#include <cstdint>

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   This is still a pretty weak implementation, so could use some revisiting. But better now
 *              as of Stroika v2.1d18
 *
 *      @todo   Make sure not actually generating code in release builds, but still generating enough code so
 *              we get linker erorr missing qDebug=1/0.
 *
 */

namespace Stroika::Foundation::Debug {

    struct CompileTimeFlagChecker {
        using HiddenValueType = uint8_t;
    };

    /**
     *  \brief CompileTimeFlagChecker_HEADER () will generate a LINK ERROR if you ever compile a header with one value and
     *         the corresponding CompileTimeFlagChecker_SOURCE () with another value.
     *
     *          It is HOPED the linker will optimize these references out (else wise - they should be small, not super significant).
     *
     *          But still hopefully check them so that compiling with mixed CPP defines (-D defines) will cause an easily diagnosic error message.
     *
     *  If this check fails, you will see a linker message like
     *      \code
     *          unresolved external symbol "unsigned char Stroika::Foundation::Debug::CompileTimeCheck_VARIABLENAME_BADVALUE
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          error LNK2001: unresolved external symbol "unsigned char Stroika::Foundation::Debug::CompileTimeCheck_qTraceToFile_0
     *      \endcode
     *      means that the variable qTraceToFile was defined '0' in some header, but some other value (that would be 1) in the cpp file where Stroika was built.
     *
     *  \note   The CompileTimeFlagChecker_HEADER () must occur in the global namespace (in order to forward declare the shared variable in a specific namespace
     *          given by the first macro argument).
     *
     *  \par Example Usage
     *      \code
     *          CompileTimeFlagChecker_HEADER (Stroika::Foundation::Debug, qTraceToFile, qTraceToFile);
     *      \endcode
     *
     *  \note   **DEFECT**
     *          The VALUE (third) argument to CompileTimeFlagChecker_HEADER and CompileTimeFlagChecker_SOURCE must expand to zero or one, not
     *          a complex expression due to how the name macro name pasting works. UNDESIRABLE!.
     */
#define CompileTimeFlagChecker_HEADER(NS_PREFIX, NAME, VALUE) \
    CompileTimeCheck_HEADER_INTERNAL_ (NS_PREFIX, CompileTimeChecker_##NAME, CompileTimeCheck_##NAME, VALUE)
#define CompileTimeCheck_HEADER_INTERNAL_(NS_PREFIX, CHECKERNAME, NAME, VALUE)                                                      \
    namespace NS_PREFIX {                                                                                                           \
        extern Stroika::Foundation::Debug::CompileTimeFlagChecker::HiddenValueType NAME##_##VALUE;                                  \
    }                                                                                                                               \
    namespace {                                                                                                                     \
        struct tester_##CHECKERNAME##_ {                                                                                            \
            tester_##CHECKERNAME##_ ()                                                                                              \
            {                                                                                                                       \
                /* just reference given name that will be a link error if header references name not defined in CPP file */         \
                [[maybe_unused]] Stroika::Foundation::Debug::CompileTimeFlagChecker::HiddenValueType a = NS_PREFIX::NAME##_##VALUE; \
            };                                                                                                                      \
        };                                                                                                                          \
        tester_##CHECKERNAME##_ t_##CHECKERNAME##_;                                                                                 \
    }

    /**
     *  \note NS_PREFIX must be the namespace in which the CompileTimeFlagChecker_HEADER was declared.
     *
     *  Put exactly one of these in a single .cpp file that should be the definition of what everything else must be consistent with.
     *  It should take the same arguments as the corresponding CompileTimeFlagChecker_SOURCE ().
     *
     *  \par Example Usage
     *      \code
     *          CompileTimeFlagChecker_SOURCE (Stroika::Foundation::Debug, qTraceToFile, qTraceToFile);
     *      \endcode
     *
     *  \note   **DEFECT**
     *          The VALUE (third) argument to CompileTimeFlagChecker_HEADER and CompileTimeFlagChecker_SOURCE must expand to zero or one, not
     *          a complex expression due to how the name macro name pasting works. UNDESIRABLE!.
     */
#define CompileTimeFlagChecker_SOURCE(NS_PREFIX, NAME, VALUE) \
    CompileTimeCheck_SOURCE_PRIVATE_1_ (NS_PREFIX, CompileTimeCheck_##NAME, VALUE)
#define CompileTimeCheck_SOURCE_PRIVATE_1_(NS_PREFIX, NAME, VALUE) \
    Stroika::Foundation::Debug::CompileTimeFlagChecker::HiddenValueType NS_PREFIX::NAME##_##VALUE = 1;

}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "CompileTimeFlagChecker.inl"

#endif /*_Stroika_Foundation_Debug_CompileTimeFlagChecker_h_*/
