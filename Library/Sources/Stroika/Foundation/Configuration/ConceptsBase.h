/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_ConceptsBase_h_
#define _Stroika_Foundation_Configuration_ConceptsBase_h_ 1

#include "../StroikaPreComp.h"

#include <type_traits>

/*
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * Description:
 *
 *
 *
 * TODO:
 *      @todo   Maybe put concepts in sub-namespace Concept {} - and then list them out here? Instead of the _ stuff?
 *
 *      @todo   Think out if/when qCheckConceptRequirements SB defined? See if it takes up any runtime space/time? And decide
 *              according to that!
 *
 *
 * Notes:
 *
 *
 *
 */

namespace Stroika::Foundation::Configuration {

    using namespace std;

    /**
     *  LIFTED -@todo add docs/reference
     *  from Stroustrup C++11 book - page 800
     */
    struct substitution_failure {
    };

    /**
     *  LIFTED -@todo add docs/reference
     *  from Stroustrup C++11 book - page 800
     */
    template <typename T>
    struct substitution_succeeded : true_type {
    };
    template <>
    struct substitution_succeeded<substitution_failure> : false_type {
    };

    /**
     *  \brief  Define has_XXX (where you specify XXX) class which has a 'value' field saying
     *          if the XXX property is present.
     *
     *  Since I cannot (so far figure out how to) do in a single simple statement/template,
     *  at least do this magic in a macro...
     */
#define STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(NAME, XTEST)                                                    \
    namespace Private_ {                                                                                            \
        template <typename T>                                                                                       \
        struct NAME##_result_impl {                                                                                 \
            template <typename X>                                                                                   \
            static auto                                                     check (const X& x) -> decltype (XTEST); \
            static Stroika::Foundation::Configuration::substitution_failure check (...);                            \
            using type = decltype (check (declval<T> ()));                                                          \
        };                                                                                                          \
    }                                                                                                               \
    template <typename T>                                                                                           \
    using NAME##_result = typename Private_::NAME##_result_impl<T>::type;                                           \
    template <typename T>                                                                                           \
    struct [[deprecated ("Since Stroika 2.1b14 - use templates in Concepts.h and is_detected_v")]] has_##NAME : integral_constant<bool, not is_same<NAME##_result<T>, Stroika::Foundation::Configuration::substitution_failure>::value>{};

    /**
     * 
     *     // just needed til we have c++ 20 concepts
     *
     *  Credit to https://stackoverflow.com/users/16746390/kenash0625
     *  For his suggestion in https://stackoverflow.com/questions/70119120/how-to-fix-sfinae-check-for-operator-existing-so-that-it-works-with-stdpair/70122139#70122139
     *  See also Detection Idioms - https://segmentfault.com/a/1190000040852065/en
     */
    template <
        template <typename...> typename Detector,
        typename T,
        typename SFINAE = void>
    constexpr inline bool is_detected_v = false;
    template <
        template <typename...> typename Detector,
        typename T>
    constexpr inline bool is_detected_v<
        Detector, T, std::void_t<Detector<T>>> = true;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Configuration_ConceptsBase_h_ */
