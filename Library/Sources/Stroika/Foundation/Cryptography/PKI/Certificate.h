/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Certificate_h_
#define _Stroika_Foundation_Cryptography_Certificate_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Cryptography/PKI/PrivateKey.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"

namespace Stroika::Foundation::Cryptography::PKI::Certificate {

    using Characters::String;
    using Containers::Mapping;
    using Time::DateTime;
    using Traversal::Range;

    /**
     * EG Subject: C=US, ST=California, L=San Francisco, O=Wikimedia Foundation, Inc., CN=*.wikipedia.org
     */
    struct SubjectInfo {
        String fCountry;
        String fOrganization;
        String fCommonName;

        String ToString () const;
    };

    /**
     */
    class IRep {
    public:
        virtual ~IRep () = default;

        // Not Before thru Not After
        virtual Range<DateTime> GetValidDates () const = 0;
        virtual SubjectInfo     GetSubject () const    = 0;
    };

    /**
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         */
        using inherited::inherited;

        // I THINK consists of mapping of assertions (?) or sequence? key-value pairs.. - sb able to retrive and maybe
        // add to/update?
        SubjectInfo GetSubject () const
        {
            return get ()->GetSubject ();
        }
        Range<DateTime> GetValidDates () const
        {
            return get ()->GetValidDates ();
        }
        nonvirtual Characters::String ToString () const;
    };

    /**
      */
    struct SelfSignedCertParams {
        // Not Before thru Not After
        Range<DateTime>  fValidDates{Time::DateTime::Now (), Time::DateTime::Now () + Time::Duration{"PT1Y"sv}};
        SubjectInfo      fSubject;
        optional<String> fSubjectAlternativeName; // SAN
    };

    /**
     *  \brief generate a new self-signed certificate (and private key)
     *  \see https://stackoverflow.com/questions/256405/programmatically-create-x509-certificate-using-openssl
     */
    tuple<PrivateKey::Ptr, Ptr> NewSelfSigned (const SelfSignedCertParams& params);

    // and example loading PEM .CER files...
    // (regtests)
    // @todo add 'make self-signed-cert' https://stackoverflow.com/questions/256405/programmatically-create-x509-certificate-using-openssl

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_Certificate_h_*/
