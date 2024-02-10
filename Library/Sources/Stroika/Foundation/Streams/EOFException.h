/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_EOFException_h_
#define _Stroika_Foundation_Streams_EOFException_h_ 1

#include "../StroikaPreComp.h"

#include "../Execution/Exceptions.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Streams {

    /**
     *  \brief - the stream ended prematurely, so that the requested operation could not be completed.
     *
     *  This is NOT thrown when the data is not just available yet. This is thrown when the data is known to
     *  never be available.
     */
    class EOFException : public Execution::RuntimeErrorException<> {
    private:
        using inherited = Execution::RuntimeErrorException<>;

    public:
        EOFException (bool partialReadCompleted = false);

    public:
        nonvirtual bool GetPartialReadCompleted () const;

    public:
        /**
         */
        static const EOFException kThe;

    private:
        bool fPartialReadCompleted_{false};
    };
    inline const EOFException EOFException::kThe;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "EOFException.inl"

#endif /*_Stroika_Foundation_Streams_EOFException_h_*/
