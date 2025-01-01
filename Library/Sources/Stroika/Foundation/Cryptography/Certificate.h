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
    };

    // @todo VERY rough - needs optional private key, and TYPE info
    Ptr New (Memory::BLOB pubKey, Memory::BLOB privateKey);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_Certificate_h_*/
