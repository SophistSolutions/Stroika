/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Jenkins_inl_
#define _Stroika_Foundation_Cryptography_Digest_Jenkins_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Cryptography::Digest {

    /*
     ********************************************************************************
     ******************* Digester<Algorithm::Jenkins, uint32_t> *********************
     ********************************************************************************
     */
    inline Digester<Algorithm::Jenkins, uint32_t>::ReturnType Digester<Algorithm::Jenkins, uint32_t>::operator() (const BLOB& from) const
    {
        return this->operator() (from.begin (), from.end ());
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_Jenkins_inl_*/
