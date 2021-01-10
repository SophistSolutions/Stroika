/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_MD5_inl_
#define _Stroika_Foundation_Cryptography_Digest_MD5_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Cryptography::Digest {

    /*
     ********************************************************************************
     ******************* Digester<Algorithm::MD5, uint32_t> *************************
     ********************************************************************************
     */
    inline Digester<Algorithm::MD5, Result128BitType>::ReturnType Digester<Algorithm::MD5, Result128BitType>::operator() (const Streams::InputStream<std::byte>::Ptr& from) const
    {
        // @todo - REIMPLMENET CALLING MD5 Update directly, on each read, as in CRC32 impl...
        Memory::BLOB b = from.ReadAll ();
        return this->operator() (b.begin (), b.end ());
    }
    inline Digester<Algorithm::MD5, Result128BitType>::ReturnType Digester<Algorithm::MD5, Result128BitType>::operator() (const BLOB& from) const
    {
        return this->operator() (from.begin (), from.end ());
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_MD5_inl_*/
