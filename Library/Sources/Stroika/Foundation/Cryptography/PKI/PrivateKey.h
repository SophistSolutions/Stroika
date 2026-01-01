/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_PrivateKey_h_
#define _Stroika_Foundation_Cryptography_PrivateKey_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Memory/BLOB.h"

namespace Stroika::Foundation::Cryptography::PKI::PrivateKey {

    using Characters::String;
    using Memory::BLOB;

    /**
     */
    class IRep {
    public:
        virtual ~IRep () = default;

        // @todo type notion not super useful like this
        virtual int          GetType () const         = 0;
        virtual unsigned int GetBits () const         = 0;
        virtual String       GetPrintSummary () const = 0;
    };

    /**
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         */
        using inherited::inherited;

        Characters::String ToString () const;

        int GetType () const
        {
            return get ()->GetType ();
        }
        // the cryptographic length of the cryptosystem to which the key in pkey belongs, in bits
        unsigned int GetBits () const
        {
            return get ()->GetBits ();
        }
        String GetPrintSummary () const
        {
            return get ()->GetPrintSummary ();
        }
    };

#if 0
    // Probably no useful CTOR of this type - must have 'type' and possibly extra params for that type and construct subtypes.
    /**
     *  \brief Create a PrivateKey (for now just from a PEM file)
     */
    //Ptr New (const PEMFile& pem);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_PrivateKey_h_*/
