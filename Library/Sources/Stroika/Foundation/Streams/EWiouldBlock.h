/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_EWouldBlock_h_
#define _Stroika_Foundation_Streams_EWouldBlock_h_ 1

#include "../StroikaPreComp.h"

#include "../Execution/Exceptions.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::Streams {

    /**
     *  \brief - a read (typically) or write operation would have blocked, and the XXX flag was passed to a Stream read
     * 
     *  \see NoDataAvailableHandling
     */
    class EWouldBlock : public Execution::RuntimeErrorException<> {
    private:
        using inherited = Execution::RuntimeErrorException<>;

    public:
        EWouldBlock ();

    public:
        /**
         */
        static const EWouldBlock kThe;
    };
    inline const EWouldBlock EWouldBlock::kThe;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "EWouldBlock.inl"

#endif /*_Stroika_Foundation_Streams_EWouldBlock_h_*/
