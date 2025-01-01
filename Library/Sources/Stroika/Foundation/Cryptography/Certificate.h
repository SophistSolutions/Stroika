/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Certificate_h_
#define _Stroika_Foundation_Cryptography_Certificate_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Memory/BLOB.h"

namespace Stroika::Foundation::Cryptography::Certificate {

    /**
         */
    class IRep {
    public:
        virtual ~IRep () = 0;
    };

    /**
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
             *  inherit all CTORS from base
             */
        using inherited::inherited;

        // I THINK consists of mapping of assertions (?) or sequence? key-value pairs.. - sb able to retrive and maybe 
        // add to/update?
    };

    // @todo VERY rough - needs optional private key, and TYPE info
    Ptr New (Memory::BLOB pubKey, Memory::BLOB privateKey);

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
