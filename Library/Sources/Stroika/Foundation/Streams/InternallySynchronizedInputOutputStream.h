/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_h_
#define _Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Memory/BLOB.h"

#include "InputOutputStream.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late</a>
 *
 *
 */

namespace Stroika::Foundation::Streams {

    /**
     *  Helper to wrap an existing stream in synchronized wrapper - like @see Execution::Synchronized - except this wrape
     *  the underlying pointed to stream, not the external smart-pointer wrapper.
     */
    template <typename ELEMENT_TYPE, typename BASE_CLASS = InputOutputStream<ELEMENT_TYPE>, typename BASE_REP_TYPE = typename BASE_CLASS::_IRep>
    class InternallySynchronizedInputOutputStream : public BASE_CLASS {
    private:
        using inherited = BASE_CLASS;

    public:
        /**
         *  'InternallySynchronizedInputOutputStream' is a quasi-namespace: use Ptr or New () members.
         */
        InternallySynchronizedInputOutputStream ()                                               = delete;
        InternallySynchronizedInputOutputStream (const InternallySynchronizedInputOutputStream&) = delete;

    public:
        using typename inherited::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Streams::InputOutputStream<byte>::Ptr syncStream = Streams::InternallySynchronizedInputOutputStream<byte>::New (otherInputOutputStreamToBeSharedAcrossThread);
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          using InternalSyncRep_ = Streams::InternallySynchronizedInputOutputStream<byte, SocketStream, SocketStream::Rep_>;
         *          Ptr unsyncStream;// = ... get from someplace - maybe make_shared<Rep_> ()...;
         *          Ptr syncStream = InternalSyncRep_::New (unsyncStream);
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-Letter-Internally-Synchronized</a>
         */
        template <typename... ARGS>
        static Ptr New (ARGS&&... args);

    private:
        class Rep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySynchronizedInputOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_h_*/
