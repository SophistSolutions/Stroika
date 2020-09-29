/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_h_
#define _Stroika_Foundation_Cryptography_Hash_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Memory/BLOB.h"

#include "Digest/Digester.h"

// THIS FILE IS OBSOLETE - USE Digest/Hash.h
#pragma message("Cryptography/Hash.h deprecated since Stroika v2.1b6 - use Cryptography/Digest/Hash.h")

namespace Stroika::Foundation::Cryptography {

    template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE = typename DIGESTER::ReturnType>
    [[deprecated ("Cryptography deprecated since Stroika 2.1b6: use Cryptography::Digest::Hash instead")]] HASH_RETURN_TYPE Hash (TYPE_TO_COMPUTE_HASH_OF data2Hash);
    template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE = typename DIGESTER::ReturnType>
    [[deprecated ("Cryptography deprecated since Stroika 2.1b6: use Cryptography::Digest::Hash instead")]] HASH_RETURN_TYPE Hash (TYPE_TO_COMPUTE_HASH_OF data2Hash, const Memory::BLOB& salt);
    template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE = typename DIGESTER::ReturnType>
    [[deprecated ("Cryptography deprecated since Stroika 2.1b6: use Cryptography::Digest::Hash instead")]] HASH_RETURN_TYPE Hash (TYPE_TO_COMPUTE_HASH_OF data2Hash, TYPE_TO_COMPUTE_HASH_OF salt);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Hash.inl"

#endif /*_Stroika_Foundation_Cryptography_Hash_h_*/
