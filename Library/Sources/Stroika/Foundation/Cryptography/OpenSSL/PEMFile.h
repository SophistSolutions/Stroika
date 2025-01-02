/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_PEMFile_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_PEMFile_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/Certificate.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/PrivateKey.h"
#include "Stroika/Foundation/Cryptography/PEMFile.h"
#include "Stroika/Foundation/Memory/BLOB.h"

/**
 *  \file
 *
 */

namespace Stroika::Foundation::Cryptography::OpenSSL::PEMFile {

    struct IRep : Cryptography::PEMFile::IRep {};

    /**
    */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         */
        using inherited::inherited;
    };

    // @todo VERY rough - needs optional private key, and TYPE info
    Ptr New (const Memory::BLOB& pemData);

/**
     *  @todo CerticateRequest
     */
#if 0
    struct PEMFile : public Cryptography::PEMFile {
        using inherited = Cryptography::PEMFile;
        using inherited::inherited;

        /**
       // can be null if missing - an optional itme in PEM file
        */
        Certificate::Ptr GetCertificate () const;

        /**
       // can be null if missing - an optional itme in PEM file
        */
        PrivateKey::Ptr GetPrivateKey () const;

        nonvirtual Characters::String ToString () const;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_PEMFile_h_*/
