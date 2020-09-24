/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_SuperFastHash_inl_
#define _Stroika_Foundation_Cryptography_Digest_SuperFastHash_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Cryptography::Digest {

    /*
     ********************************************************************************
     *************** Digester<Algorithm::SuperFastHash, uint32_t> *******************
     ********************************************************************************
     */
    inline Digester<Algorithm::SuperFastHash, uint32_t>::ReturnType Digester<Algorithm::SuperFastHash, uint32_t>::operator() (const Streams::InputStream<std::byte>::Ptr& from) const
    {
        Memory::BLOB b = from.ReadAll ();
        return this->operator() (b.begin (), b.end ());
    }
    inline Digester<Algorithm::SuperFastHash, uint32_t>::ReturnType Digester<Algorithm::SuperFastHash, uint32_t>::operator() (const BLOB& from) const
    {
        return this->operator() (from.begin (), from.end ());
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_SuperFastHash_inl_*/
