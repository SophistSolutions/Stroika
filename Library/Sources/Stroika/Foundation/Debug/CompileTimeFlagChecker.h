/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_CompileTimeFlagChecker_h_
#define _Stroika_Foundation_Debug_CompileTimeFlagChecker_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   Did super crudy implementation: try to redo with templates...
 *
 *      @todo   Make sure not actually generating code in release builds
 *
 */

namespace Stroika::Foundation::Debug {

    /**
    namespace LedCheckCompilerFlags {
    #define LedCheckCompilerFlags_(a)   _CHECK_##a
        extern  int LedCheckCompilerFlags_(qDebug);
        extern  int LedCheckCompilerFlags_(qSingleByteCharacters);
        extern  int LedCheckCompilerFlags_(qMultiByteCharacters);
        extern  int LedCheckCompilerFlags_(qWideCharacters);
        extern  int LedCheckCompilerFlags_(qProvideIMESupport);

        struct  FlagsChecker {
            FlagsChecker ()
            {
                if (LedCheckCompilerFlags_(qDebug) != qDebug)                                   {           abort ();       }
                if (LedCheckCompilerFlags_(qSingleByteCharacters) != qSingleByteCharacters)     {           abort ();       }
                if (LedCheckCompilerFlags_(qMultiByteCharacters) != qMultiByteCharacters)       {           abort ();       }
                if (LedCheckCompilerFlags_(qWideCharacters) != qWideCharacters)                 {           abort ();       }
                if (LedCheckCompilerFlags_(qProvideIMESupport) != qProvideIMESupport)           {           abort ();       }
            }
        };
        static  struct  FlagsChecker    sFlagsChecker;
    }


    namespace Private_ {
        extern  Flag2Check<>::OrigValue     Flag2Check<>::kCheck_qDebug_;
        namespace {
            constexpr   Flag2Check<>    kValidate_qDebug_ { qDebug, Flag2Check<int>::kCheck_qDebug_ };
        }
    }
    */

    struct CompileTimeFlagChecker {
        using HiddenValueType = uint8_t;
    };

// #define NAME2(a,b)         NAME2_HIDDEN(a,b)
// #define NAME2_HIDDEN(a,b)  a ## b
///#define CTC_NAME(NAME,VALUE)  NAME2_HIDDEN(##NAME,VALUE)

/**
     */
#define CompileTimeFlagChecker_HEADER(NAME, VALUE) \
    CompileTimeCheck_HEADER_INTERNAL_ (CompileTimeCheck_##NAME, VALUE)
#define CompileTimeCheck_HEADER_INTERNAL_(NAME, VALUE)                                      \
    extern Stroika::Foundation::Debug::CompileTimeFlagChecker::HiddenValueType NAME##VALUE; \
    namespace {                                                                             \
        struct tester_##NAME##_ {                                                           \
            tester_##NAME##_ ()                                                             \
            {                                                                               \
                using Stroika::Foundation::Debug::CompileTimeFlagChecker;                   \
                [[maybe_unused]] CompileTimeFlagChecker::HiddenValueType a = NAME##VALUE;   \
            };                                                                              \
        };                                                                                  \
        tester_##NAME##_ t_##NAME##_;                                                       \
    }

/**
 */
#define CompileTimeFlagChecker_SOURCE(NS_PREFIX, NAME, VALUE) \
    CompileTimeCheck_SOURCE_PRIVATE_1_ (NS_PREFIX, CompileTimeCheck_##NAME, VALUE)
#define CompileTimeCheck_SOURCE_PRIVATE_1_(NS_PREFIX, NAME, VALUE) \
    Stroika::Foundation::Debug::CompileTimeFlagChecker::HiddenValueType NS_PREFIX::NAME##VALUE = 1;
}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "CompileTimeFlagChecker.inl"

#endif /*_Stroika_Foundation_Debug_CompileTimeFlagChecker_h_*/
