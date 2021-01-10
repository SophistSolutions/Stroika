/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Digester_inl_
#define _Stroika_Foundation_Cryptography_Digest_Digester_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "ResultTypes.h"

namespace Stroika::Foundation::Cryptography::Digest {

    /*
     ********************************************************************************
     *********************** Digester<ALGORITHM, RETURN_TYPE> ***********************
     ********************************************************************************
     */
    template <typename ALGORITHM, typename RETURN_TYPE>
    auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const Streams::InputStream<std::byte>::Ptr& from) const -> ReturnType
    {
        // default algorithm for any RETURN_TYPE is to use Digester<ALGORITHM> which is specailized for
        // its primary return type, and use ConvertResult () to map to selected return type (if possible)
        auto defaultDigester = Digester<ALGORITHM>{};
        return ConvertResult<RETURN_TYPE> (defaultDigester (from));
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const std::byte* from, const std::byte* to) const -> ReturnType
    {
        // see above
        auto defaultDigester = Digester<ALGORITHM>{};
        return ConvertResult<RETURN_TYPE> (defaultDigester (from, to));
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const BLOB& from) const -> ReturnType
    {
        // see above
        auto defaultDigester = Digester<ALGORITHM>{};
        return ConvertResult<RETURN_TYPE> (defaultDigester (from));
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_Digester_inl_*/
