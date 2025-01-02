/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_PEMFile_h_
#define _Stroika_Foundation_Cryptography_PEMFile_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Cryptography/Certificate.h"
#include "Stroika/Foundation/Cryptography/PrivateKey.h"
#include "Stroika/Foundation/Memory/BLOB.h"

/**
 *  \file
 *
 */

namespace Stroika::Foundation::Cryptography::PEMFile {

    using Containers::Sequence;
    using Memory::BLOB;

    /**
     *  @todo - can also be CRL, cetificate request, etc... - message, many things can be inside
     */
    using EntryType = variant<PrivateKey::Ptr, Certificate::Ptr>;

    /**
     */
    class IRep {
    public:
        virtual ~IRep () = default;

    public:
        virtual BLOB                GetData () const    = 0;
        virtual Sequence<EntryType> GetEntries () const = 0;
    };

    /**
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         */
        using inherited::inherited;

        Characters::String ToString () const;

        BLOB                GetData () const;
        Sequence<EntryType> GetEntries () const;

        // I THINK consists of mapping of assertions (?) or sequence? key-value pairs.. - sb able to retrive and maybe
        // add to/update?
    };
    Ptr New (const Memory::BLOB& pemData);

#if 0
    /**
     *  \brief a PEM File is a data structure commonly used to store private keys, and or certificates (as well as several other things not currently supported here).
     * 
     *  At this level of abstraction - its just a bunch of bytes, but used in overloading, in other APIs, so they correctly
     *  know how to interpret the bytes.
     * 
     *      https://datatracker.ietf.org/doc/html/rfc7468
     * 
     *  \par Example Usage:
     *      \code
     *          PEMFile myCertPem{IO::FileSystem::FileInputStream::New ("my-cert.pem").ReadAll ())};
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
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PEMFile.inl"

#endif /*_Stroika_Foundation_Cryptography_PEMFile_h_*/
