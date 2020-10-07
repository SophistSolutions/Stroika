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
        // Adapt the digest return type to the return type declared by the Hasher (often the same)
        template <typename ADAPTER_RETURN_TYPE, typename HASHER_RETURN_TYPE>
        constexpr ADAPTER_RETURN_TYPE mkReturnType_ (HASHER_RETURN_TYPE hashVal, enable_if_t<is_constructible_v<ADAPTER_RETURN_TYPE, HASHER_RETURN_TYPE>, void>* = nullptr)
        {
            return ADAPTER_RETURN_TYPE (hashVal);
        }
        // NOTE - mkReturnType1_<string,XXX> () here uses enable_if and is_same, since C++ doesn't currently allow partial function template
        // specialization -- LGP 2020-10-02
        template <typename ADAPTER_RETURN_TYPE, typename HASHER_RETURN_TYPE>
        inline ADAPTER_RETURN_TYPE mkReturnType_ (HASHER_RETURN_TYPE hashVal, enable_if_t<is_same_v<ADAPTER_RETURN_TYPE, string>>* = nullptr)
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
