/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_ConceptsBase_h_
#define _Stroika_Foundation_Configuration_ConceptsBase_h_ 1

#include "../StroikaPreComp.h"

#include <type_traits>

/*
 *  ********* DEPRECATED *******
 */

namespace Stroika::Foundation::Configuration {

    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    using namespace std;

    /**
     *  LIFTED -@todo add docs/reference
     *  from Stroustrup C++11 book - page 800
     */
    struct [[deprecated ("Since Stroika v3 - use concepts")]] substitution_failure{};

    /**
     *  LIFTED -@todo add docs/reference
     *  from Stroustrup C++11 book - page 800
     */
    template <typename T>
    struct [[deprecated ("Since Stroika v3 - use concepts")]] substitution_succeeded : true_type{};
    template <>
    struct substitution_succeeded<substitution_failure> : false_type {};

    /**
     * 
     *     // just needed til we have c++ 20 concepts
     *
     *  Credit to https://stackoverflow.com/users/16746390/kenash0625
     *  For his suggestion in https://stackoverflow.com/questions/70119120/how-to-fix-sfinae-check-for-operator-existing-so-that-it-works-with-stdpair/70122139#70122139
     *  See also Detection Idioms - https://segmentfault.com/a/1190000040852065/en
     */
    template <template <typename...> typename Detector, typename T, typename SFINAE = void>
    [[deprecated ("Since Stroika v3 - use concepts")]] constexpr inline bool is_detected_v = false;
    template <template <typename...> typename Detector, typename T>
    constexpr inline bool is_detected_v<Detector, T, std::void_t<Detector<T>>> = true;

    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Configuration_ConceptsBase_h_ */
