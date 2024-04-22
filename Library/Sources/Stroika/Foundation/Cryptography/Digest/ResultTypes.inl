/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <type_traits>

#include "../Format.h"

namespace Stroika::Foundation::Cryptography::Digest {

    namespace Private_ {
        // Try to use simple constuction of result from argument if possible
        template <typename OUT_RESULT, typename IN_RESULT>
        constexpr OUT_RESULT mkReturnType_ (IN_RESULT hashVal)
        {
            if constexpr (is_constructible_v<OUT_RESULT, IN_RESULT>) {
                return OUT_RESULT (hashVal); // intentionally allow narrowing conversions (so () not {})
            }
            else if constexpr (is_trivially_copyable_v<IN_RESULT> and is_trivially_copyable_v<OUT_RESULT>) {
                // Else if both (IN AND OUT) values trivially copyable, use memcpy (and zero fill result as needed)
                size_t     mBytes2Copy = std::min (sizeof (OUT_RESULT), sizeof (IN_RESULT));
                OUT_RESULT result{}; // zero initialize non-copied bits (@todo could just zero-fill end bits)
                DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wclass-memaccess\"") // memcpy only requires trivially_copyable, not is_trivial
                ::memcpy (&result, &hashVal, mBytes2Copy);
                DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wclass-memaccess\"")
                return result;
            }
            else if constexpr (is_same_v<OUT_RESULT, string> or is_same_v<OUT_RESULT, Characters::String> or is_same_v<OUT_RESULT, Common::GUID>) {
                return Format<OUT_RESULT> (hashVal);
            }
            // else error
        }
    }

    template <typename OUT_RESULT, typename IN_RESULT>
    constexpr OUT_RESULT ConvertResult (IN_RESULT inResult)
    {
        return Private_::mkReturnType_<OUT_RESULT> (inResult);
    }

}
