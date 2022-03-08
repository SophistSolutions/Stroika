/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Copy_h_
#define _Stroika_Foundation_Streams_Copy_h_ 1

#include "../StroikaPreComp.h"

#include "InputStream.h"
#include "OutputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 *  TODO:
 *      @todo   Provide a variant of CopyAll which is safely always cancelable (at least of underlying src and target streams cancelable). Maybe this
 *              is already done? Or maybe this involves using ReadSome() and blocking on availabiliuty of more? CONSIDER
 *
 *      @todo   Redo API (at least overloads) with progress tracker
 */

namespace Stroika::Foundation::Streams {

    /**
     * @brief Copy the contents of stream 'from' to stream 'to'
     * 
     *  \note ***Cancelation Point*** (but may not always be cancelable - may cancel)
     */
    template <typename ELEMENT_TYPE>
    void CopyAll (typename InputStream<ELEMENT_TYPE>::Ptr from, typename OutputStream<ELEMENT_TYPE>::Ptr to, size_t bufferSize = 10 * 1024);

    /**
     * @brief Copy the contents of stream 'from' to stream 'to', by reading ALL of from into memory at once, and then writing it to 'to' (fails if not enough memory to hold entire from stream in RAM)
     * 
     *  \note ***Cancelation Point*** (but may not always be cancelable - may cancel)
     */
    template <typename ELEMENT_TYPE>
    void CopyAll_OneRead (typename InputStream<ELEMENT_TYPE>::Ptr from, typename OutputStream<ELEMENT_TYPE>::Ptr to);

    /**
     * @brief Copy the contents of stream 'from' to stream 'to', by reading a chunk at a time - tends to do multiple reads and writes - but works with arbitrarily large stream
     * 
     *  \note ***Cancelation Point*** (but may not always be cancelable - may cancel)
     */
    template <typename ELEMENT_TYPE>
    void CopyAll_Buffered (typename InputStream<ELEMENT_TYPE>::Ptr from, typename OutputStream<ELEMENT_TYPE>::Ptr to, size_t bufferSize = 10 * 1024);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Copy.inl"

#endif /*_Stroika_Foundation_Streams_Copy_h_*/
