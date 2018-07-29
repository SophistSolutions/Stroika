/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_EOFException_h_
#define _Stroika_Foundation_Streams_EOFExceptionm_h_ 1

#include "../StroikaPreComp.h"

#include "../Execution/StringException.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 */

namespace Stroika::Foundation::Streams {

    /**
     */
    class EOFException : public Execution::StringException {
    public:
        EOFException ();
        EOFException (bool partialReadCompleted);

    public:
        nonvirtual bool GetPartialReadCompleted () const;

    public:
        /**
         */
        static const EOFException kThe;

    private:
        bool fPartialReadCompleted_{false};
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "EOFException.inl"

#endif /*_Stroika_Foundation_Streams_EOFException_h_*/
