/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_PEMFile_h_
#define _Stroika_Foundation_Cryptography_PEMFile_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Cryptography/PKI/Certificate.h"
#include "Stroika/Foundation/Cryptography/PKI/PrivateKey.h"
#include "Stroika/Foundation/Memory/BLOB.h"

/**
 *  \file
 * 
 *  \brief a PEM File is a data structure commonly used to store private keys, and or certificates (as well as several other things not currently supported here).
 * 
 *  At this level of abstraction - its just a bunch of bytes, but used in overloading, in other APIs, so they correctly
 *  know how to interpret the bytes.
 * 
 *      https://datatracker.ietf.org/doc/html/rfc7468
 * 
 */

namespace Stroika::Foundation::Cryptography::PKI::PEMFile {

    using Containers::Sequence;
    using Memory::BLOB;
    using Traversal::Iterable;

    /**
     *  @todo - can also be CRL, certificate request, etc... - message, many things can be inside
     * 
     *  \note since this is being developed to support webserver, main target support is PrivateKey/Cert --LGP 2025-01-03
     */
    using EntryType = variant<Certificate::Ptr, PrivateKey::Ptr>;

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
        template <Common::IAnyOf<Certificate::Ptr, PrivateKey::Ptr> T>
        Iterable<T> GetByType () const;

        // I THINK consists of mapping of assertions (?) or sequence? key-value pairs.. - sb able to retrive and maybe
        // add to/update?
    };

    /**
     *  \par Example Usage:
     *      \code
     *          PEMFile::Ptr myCertPem{New (IO::FileSystem::FileInputStream::New ("my-cert.pem").ReadAll ()))};
     *          Certificate::Ptr cert = pem.GetByType<Certificate::Ptr> ().FirstValue (nullptr);
     *          PrivateKey::Ptr  pkey = pem.GetByType<PrivateKey::Ptr> ().FirstValue (nullptr);
     *      \endcode
     * 
     *  \par Example Usage:     @todo need example constructed form CERT tsielf... and getting Cert...
     *      \code
     *          
     *      \endcode
     */
    Ptr New (const filesystem::path& pemFile);
    Ptr New (const Memory::BLOB& pemData);
    Ptr New (const Sequence<EntryType>& entries);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PEMFile.inl"

#endif /*_Stroika_Foundation_Cryptography_PEMFile_h_*/
