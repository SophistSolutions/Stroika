/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

#include "Stroika/Foundation/Characters/String2Int.h"

namespace Stroika::Foundation::DataExchange::JSON {

    /*
     ********************************************************************************
     ********************************** JSON::JWT ***********************************
     ********************************************************************************
     */
    inline Mapping<String, String> JWT::GetHeaderClaims () const
    {
        return fHeaderClaims_;
    }
    inline Mapping<String, String> JWT::GetPayloadClaims () const
    {
        return fPayloadClaims_;
    }
    inline optional<String> JWT::GetIssuer () const
    {
        return fPayloadClaims_.Lookup (JWTRegisteredClaims::kIssuer);
    }
    inline optional<String> JWT::GetSubject () const
    {
        return fPayloadClaims_.Lookup (JWTRegisteredClaims::kSubject);
    }
    inline optional<String> JWT::GetAudience () const
    {
        return fPayloadClaims_.Lookup (JWTRegisteredClaims::kAudience);
    }
    inline optional<DateTime> JWT::GetExpirationTime () const
    {
        if (auto i = fPayloadClaims_.Lookup (JWTRegisteredClaims::kExpirationTime)) {
            return DateTime{Characters::String2Int<time_t> (*i)};
        }
        return nullopt;
    }
    inline optional<DateTime> JWT::GetNotBefore () const
    {
        if (auto i = fPayloadClaims_.Lookup (JWTRegisteredClaims::kNotBefore)) {
            return DateTime{Characters::String2Int<time_t> (*i)};
        }
        return nullopt;
    }
    inline optional<Range<DateTime>> JWT::GetValidFor () const
    {
        return Range<DateTime>{GetNotBefore ().value_or (DateTime::kMin), GetExpirationTime ().value_or (DateTime::kMax)};
    }
    inline optional<DateTime> JWT::GetIssuedAt () const
    {
        if (auto i = fPayloadClaims_.Lookup (JWTRegisteredClaims::kIssuedAt)) {
            return DateTime{Characters::String2Int<time_t> (*i)};
        }
        return nullopt;
    }
    inline optional<String> JWT::GetJWTID () const
    {
        return fPayloadClaims_.Lookup (JWTRegisteredClaims::kJWTID);
    }

}
