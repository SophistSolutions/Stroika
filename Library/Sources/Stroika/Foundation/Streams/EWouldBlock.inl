/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_EWouldBlock_inl_
#define _Stroika_Foundation_Streams_EWouldBlock_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Streams {

    inline EWouldBlock::EWouldBlock ()
        : inherited{"Stream Read Would Block"sv}
    {
    }

}

#endif /*_Stroika_Foundation_Streams_EWouldBlock_inl_*/
