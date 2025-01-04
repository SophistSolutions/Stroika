/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_PEMFile_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_PEMFile_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Cryptography/PKI/PEMFile.h"
#include "Stroika/Foundation/Memory/BLOB.h"

/**
 *  \file
 *
 */

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::PEMFile {

    using namespace Cryptography::PKI::PEMFile;
    struct IRep : Cryptography::PKI::PEMFile::IRep {};

    /**
    */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         */
        using inherited::inherited;
    };

    /**
     *  pemData argument is raw bytes from .pem file.
     */
    Ptr New (const Memory::BLOB& pemData);
    Ptr New (const Sequence<EntryType>& entries);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_PEMFile_h_*/
