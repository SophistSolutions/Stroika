/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_PrivateKey_h_
#define _Stroika_Foundation_Cryptography_PrivateKey_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
//#include "Stroika/Foundation/Cryptography/PEMFile.h"
#include "Stroika/Foundation/Memory/BLOB.h"

namespace Stroika::Foundation::Cryptography::PrivateKey {

    /**
     */
    class IRep {
    public:
        virtual ~IRep () = default;
    };

    /**
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         */
        using inherited::inherited;

        Characters::String ToString () const;

        // I THINK consists of mapping of assertions (?) or sequence? key-value pairs.. - sb able to retrive and maybe
        // add to/update?
    };

    /**
     *  \brief Create a PrivateKey (for now just from a PEM file)
     */
    //Ptr New (const PEMFile& pem);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_PrivateKey_h_*/
