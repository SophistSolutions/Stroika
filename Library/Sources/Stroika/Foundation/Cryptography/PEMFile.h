/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_PEMFile_h_
#define _Stroika_Foundation_Cryptography_PEMFile_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Memory/BLOB.h"

/**
 *  \file
 *
 */

namespace Stroika::Foundation::Cryptography {

    /**
     *  \brief a PEM File is a data structure commonly used to store private keys, and or certificates (as well as other things).
     * 
     *  At this level of abstraction - its just a bunch of bytes, but used in overloading, in other APIs, so they correctly
     *  know how to interpret the bytes.
     * 
     * https://en.wikipedia.org/wiki/Privacy-Enhanced_Mail#:~:text=Privacy%2DEnhanced%20Mail%20(PEM),MIME%2C%20the%20textual%20encoding%20they
     *      https://datatracker.ietf.org/doc/html/rfc7468
     * 
     * 
     *  \par Example Usage:
     *      \code
     *          PEMFile myCertPem{IO::FileSystem::InputFileStream::New ("my-cert.pem").ReadAll ())};
     *      \endcode
     */
    struct PEMFile {
        /**
         *  Future versions of this API might validate the argument to assure its a valid pem file and throw if not.
         */
        PEMFile (const Memory::BLOB& b);

        const Memory::BLOB fData;

        nonvirtual Characters::String ToString () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_PEMFile_h_*/
