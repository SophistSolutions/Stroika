/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::WebService::OpenAPI {

    inline Specification::Specification (const VariantValue& v)
        : fValue_{v}
    {
    }
    template <typename T>
    inline T Specification::As () const
        requires (same_as<T, VariantValue>)
    {
        if constexpr (same_as<T, VariantValue>) {
            return fValue_;
        }
    }

}
