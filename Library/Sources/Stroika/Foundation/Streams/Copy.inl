/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     ****************************** Streams::CopyAll ********************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline void CopyAll (typename InputStream::Ptr<ELEMENT_TYPE> from, typename OutputStream::Ptr<ELEMENT_TYPE> to, size_t bufferSize)
    {
        CopyAll_Buffered<ELEMENT_TYPE> (from, to, bufferSize);
    }

    /*
     ********************************************************************************
     ************************ Streams::CopyAll_OneRead ******************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline void CopyAll_OneRead (typename InputStream::Ptr<ELEMENT_TYPE> from, typename OutputStream::Ptr<ELEMENT_TYPE> to)
    {
        to.Write (from.ReadAll ());
    }

    /*
     ********************************************************************************
     ************************** Streams::CopyAll_Buffered ***************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    void CopyAll_Buffered (typename InputStream::Ptr<ELEMENT_TYPE> from, typename OutputStream::Ptr<ELEMENT_TYPE> to, size_t bufferSize)
    {
        Memory::StackBuffer<ELEMENT_TYPE> buf;
        if constexpr (is_trivially_copyable_v<ELEMENT_TYPE>) {
            buf.resize_uninitialized (bufferSize);
        }
        else {
            buf.resize (bufferSize);
        }
        while (size_t n = from.Read (span{buf}).size ()) {
            Assert (n <= buf.size ());
            to.Write (span{buf.begin (), n});
            Execution::Thread::CheckForInterruption ();
        }
    }

}
