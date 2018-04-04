/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_inl_
#define _Stroika_Foundation_Common_Compare_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Common {

            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated\"");
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
            DISABLE_COMPILER_MSC_WARNING_START (4996)

            namespace Private_ {
                template <typename T>
                inline constexpr int Compare_Helper_ (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2, typename enable_if<Has_Compare<T>::value>::type* = nullptr)
                {
                    return v1.Compare (v2);
                }
                template <typename T>
                inline constexpr int Compare_Helper_ (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2, typename enable_if<not Has_Compare<T>::value>::type* = nullptr)
                {
#if qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    return (v1 < v2) ? -1 : ((v2 < v1) ? 1 : 0);
#else
                    if (v1 < v2) {
                        return -1;
                    }
                    else if (v2 < v1) {
                        return 1;
                    }
                    else {
                        return 0;
                    }
#endif
                }
            }

            template <typename T>
            inline constexpr int ComparerWithWellOrder<T>::Compare (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2)
            {
#if qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                return Private_::Compare_Helper_<T> (v1, v2);
#else
                int result{Private_::Compare_Helper_<T> (v1, v2)};
                Assert ((result == 0) == Equals (v1, v2));
                return result;
#endif
            }
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated\"");
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
            DISABLE_COMPILER_MSC_WARNING_END (4996)
        }
    }
}
#endif /*_Stroika_Foundation_Common_Compare_inl_*/
