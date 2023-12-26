/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextReader_inl_
#define _Stroika_Foundation_Streams_TextReader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Streams::TextReader {

    /*
     ********************************************************************************
     ***************************** TextReader::New **********************************
     ********************************************************************************
     */
    inline auto New (const InputStream::Ptr<Character>& src) -> Ptr
    {
        return src;
    }

}

#endif /*_Stroika_Foundation_Streams_TextReader_inl_*/
