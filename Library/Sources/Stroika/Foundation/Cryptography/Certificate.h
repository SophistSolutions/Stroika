/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Certificate_h_
#define _Stroika_Foundation_Cryptography_Certificate_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Cryptography/PrivateKey.h"
#include "Stroika/Foundation/Memory/BLOB.h"

namespace Stroika::Foundation::Cryptography::Certificate {

    using Containers::Mapping;

    using Characters::String;

    /**
     */
    class IRep {
    public:
        virtual ~IRep () = default;

        virtual String                  GetSubjectName () const = 0;
        virtual Mapping<String, String> GetCommonNames () const = 0;
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
        String GetSubjectName () const
        {
            return get ()->GetSubjectName ();
        }
        Mapping<String, String> GetCommonNames () const
        {
            return get ()->GetCommonNames ();
        }
        nonvirtual Characters::String ToString () const;
    };

    // Ptr New (const PEMFile& pemFile);

    /**
     *  \brief generate a new self-signed certificate (and private key)
     *  \see https://stackoverflow.com/questions/256405/programmatically-create-x509-certificate-using-openssl
     */
    tuple<PrivateKey::Ptr, Ptr> NewSelfSigned ();

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
