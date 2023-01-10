/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Copy_inl_
#define _Stroika_Foundation_Streams_Copy_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Execution/Thread.h"
#include "../Memory/StackBuffer.h"

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     ****************************** Streams::CopyAll ********************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline void CopyAll (typename InputStream<ELEMENT_TYPE>::Ptr from, typename OutputStream<ELEMENT_TYPE>::Ptr to, size_t bufferSize)
    {
        CopyAll_Buffered<ELEMENT_TYPE> (from, to, bufferSize);
    }

    /*
     ********************************************************************************
     ************************ Streams::CopyAll_OneRead ******************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline void CopyAll_OneRead (typename InputStream<ELEMENT_TYPE>::Ptr from, typename OutputStream<ELEMENT_TYPE>::Ptr to)
    {
        to.Write (from.ReadAll ());
    }

    /*
     ********************************************************************************
     ************************** Streams::CopyAll_Buffered ***************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    void CopyAll_Buffered (typename InputStream<ELEMENT_TYPE>::Ptr from, typename OutputStream<ELEMENT_TYPE>::Ptr to, size_t bufferSize)
    {
        Memory::StackBuffer<ELEMENT_TYPE> buf;
        if constexpr (is_trivially_copyable_v<ELEMENT_TYPE>) {
            buf.resize_uninitialized (bufferSize);
        }
        else {
            buf.resize (bufferSize);
        }
        while (size_t n = from.Read (buf.begin (), buf.end ())) {
            Assert (n <= buf.size ());
            to.Write (buf.begin (), buf.begin () + n);
            Execution::Thread::CheckForInterruption ();
        }
    }

}

#endif /*_Stroika_Foundation_Streams_Copyinl_*/
