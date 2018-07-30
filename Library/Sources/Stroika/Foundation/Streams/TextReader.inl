/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextReader_inl_
#define _Stroika_Foundation_Streams_TextReader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     *********************************** TextReader::Ptr ****************************
     ********************************************************************************
     */
    inline TextReader::Ptr::Ptr (const shared_ptr<InputStream<Character>::_IRep>& from)
        : inherited (from)
    {
    }
    inline TextReader::Ptr::Ptr (const InputStream<Character>::Ptr& from)
        : inherited (from)
    {
    }

    /*
     ********************************************************************************
     *********************************** TextReader *********************************
     ********************************************************************************
     */
    inline auto TextReader::New (const InputStream<Character>::Ptr& src) -> Ptr
    {
        return src;
    }

}
#endif /*_Stroika_Foundation_Streams_TextReader_inl_*/
