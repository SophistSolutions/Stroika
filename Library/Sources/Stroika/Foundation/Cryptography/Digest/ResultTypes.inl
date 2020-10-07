/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_ResultTypes_inl_
#define _Stroika_Foundation_Cryptography_Digest_ResultTypes_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <type_traits>

#include "../Format.h"

namespace Stroika::Foundation::Cryptography::Digest {

    namespace Private_ {
        // Try to use simple constuction of result from argument if possible
        template <typename OUT_RESULT, typename IN_RESULT>
        constexpr OUT_RESULT mkReturnType_ (IN_RESULT hashVal, enable_if_t<is_constructible_v<OUT_RESULT, IN_RESULT>, void>* = nullptr)
        {
            return OUT_RESULT (hashVal);
        }
        // Else if both (IN AND OUT) values trivially copyable, use memcpy (and zero fill result as needed)
        template <typename OUT_RESULT, typename IN_RESULT>
        OUT_RESULT mkReturnType_ (IN_RESULT hashVal,
                                  enable_if_t<
                                      not is_constructible_v<OUT_RESULT, IN_RESULT> and (is_trivially_copyable_v<IN_RESULT> and is_trivially_copyable_v<OUT_RESULT>), char>* = nullptr)
        {
            size_t     mBytes2Copy = std::min (sizeof (OUT_RESULT), sizeof (IN_RESULT));
            OUT_RESULT result{}; // zero initialize non-copied bits (@todo could just zero-fill end bit)
            ::memcpy (&result, &hashVal, mBytes2Copy);
            return result;
        }
        // NOTE - mkReturnType1_<string,XXX> () here uses enable_if and is_same, since C++ doesn't currently allow partial function template
        // specialization -- LGP 2020-10-02
        template <typename OUT_RESULT, typename IN_RESULT>
        inline OUT_RESULT mkReturnType_ (IN_RESULT hashVal, enable_if_t<not is_constructible_v<OUT_RESULT, IN_RESULT> && is_same_v<OUT_RESULT, string>, short>* = nullptr)
        {
            return Format (hashVal);
        }
    }

    template <typename OUT_RESULT, typename IN_RESULT>
    constexpr OUT_RESULT ConvertResult (IN_RESULT inResult)
    {
        return Private_::mkReturnType_<OUT_RESULT> (inResult);
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_ResultTypes_inl_*/
