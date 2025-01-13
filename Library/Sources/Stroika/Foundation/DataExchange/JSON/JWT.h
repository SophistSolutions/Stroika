/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_JSON_JWT_h_
#define _Stroika_Foundation_DataExchange_JSON_JWT_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Traversal/Range.h"

/**
 */
namespace Stroika::Foundation::DataExchange::JSON {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Mapping;
    using Time::DateTime;
    using Traversal::Range;

    /**
     *  Claim names from https://datatracker.ietf.org/doc/html/rfc7519#section-4.1
     */
    namespace JWTRegisteredClaims {
        /**
         * \brief The "iss" (issuer) claim identifies the principal that issued the JWT.
         */
        constexpr string_view kIssuer = "iss"sv;

        /**
         * \brief The "sub" (subject) claim identifies the principal that is the subject of the JWT.
         */
        constexpr string_view kSubject = "sub"sv;

        /**
         * \brief The "aud" (audience) claim identifies the recipients that the JWT is intended for.
         */
        constexpr string_view kAudience = "aud"sv;

        /**
         * \brief The "exp" (expiration time) claim identifies the expiration time on or after which
         *        the JWT MUST NOT be accepted for processing
         */
        constexpr string_view kExpirationTime = "exp"sv;

        /**
         * \brief The "nbf" (not before) claim identifies the time before which the JWT
         *        MUST NOT be accepted for processing.
         */
        constexpr string_view kNotBefore = "nbf"sv;

        /**
         * \brief  The "iat" (issued at) claim identifies the time at which the JWT was issued
         */
        constexpr string_view kIssuedAt = "iat"sv;

        /**
         * \brief  The "jti" (JWT ID) claim provides a unique identifier for the JWT
         */
        constexpr string_view kJWTID = "jti"sv;
    }

    /**
     *  \brief JSON Web Token - a cryptographically signed set of claims - see https://datatracker.ietf.org/doc/html/rfc7519
     * 
     *  \todo consider using/wrapping https://github.com/Thalhammer/jwt-cpp
     *        seems much more complete than what I need right now. What I need for now is quite simple (I THINK) - though more complex to do the validation stuff
     * 
     *  \par Example Usage:
     *      \code
     *          auto encodedJWT = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJpc3MiOiJhdXRoMCIsInNhbXBsZSI6InRlc3QifQ.lQm3N2bVlqt2-1L-FsOjtR6uE-L4E9zJutMWKIe1v1M";
     *          JWT jwt{encodedJWT};
     *          if (auto audience = jwt.GetPayloadClaims ().Lookup (JWTRegisteredClaims::kAudience)) {
     *              cout << "Audience is {}"_f (*audience) << endl;
     *          }
     *          if (auto validFor = jwt.GetValidFor ()) {
     *              cout << "Valid-For is {}"_f (*validFor) << endl;
     *          }
     *          for (auto& claim : jwt.GetPayloadClaims ()) {
     *              DbgTrace ("claim: {}"_f, claim);
     *          }
     *      \endcode
     * 
     *      \todo new CTOR taking set of claims , and producing signature etc..
     */
    class JWT {
    public:
        /**
         *  \brief construct a JWT from an encoded string. If fullyValidate true, also validate the signature (NYI)
         */
        JWT (const JWT&) = default;
        JWT (const String& encodedString, bool fullyValidate = true);

    public:
        /**
         */
        nonvirtual Mapping<String, String> GetHeaderClaims () const;

    public:
        /**
         *  \brief this is probably what you want to look at
         */
        nonvirtual Mapping<String, String> GetPayloadClaims () const;

    public:
        /**
         */
        nonvirtual optional<String> GetIssuer () const;

    public:
        /**
         */
        nonvirtual optional<String> GetSubject () const;

    public:
        /**
         */
        nonvirtual optional<String> GetAudience () const;

    public:
        /**
         */
        nonvirtual optional<DateTime> GetExpirationTime () const;

    public:
        /**
         */
        nonvirtual optional<DateTime> GetNotBefore () const;

    public:
        /**
         *  \brief combined exp and nbf, to produce a datetime range the token is to be considered valid
         */
        nonvirtual optional<Range<DateTime>> GetValidFor () const;

    public:
        /**
         */
        nonvirtual optional<DateTime> GetIssuedAt () const;

    public:
        /**
         */
        nonvirtual optional<String> GetJWTID () const;

    public:
        /**
         */
        nonvirtual String ToString () const;

    private:
        Mapping<String, String> fHeaderClaims_;
        Mapping<String, String> fPayloadClaims_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "JWT.inl"

#endif /*_Stroika_Foundation_DataExchange_JSON_JWT_h_*/
