/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySyncrhonizedInputOutputStream_h_
#define _Stroika_Foundation_Streams_InternallySyncrhonizedInputOutputStream_h_ 1

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
    class InternallySyncrhonizedInputOutputStream : public BASE_CLASS {
    private:
        using inherited = BASE_CLASS;

    public:
        /**
         *  'InternallySyncrhonizedInputOutputStream' is a quasi-namespace: use Ptr or New () members.
         */
        InternallySyncrhonizedInputOutputStream ()                                               = delete;
        InternallySyncrhonizedInputOutputStream (const InternallySyncrhonizedInputOutputStream&) = delete;

    public:
        using typename inherited::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Streams::InputOutputStream<Byte>::Ptr syncStream = Streams::InternallySyncrhonizedInputOutputStream<Byte>::New (otherInputOutputStreamToBeSharedAcrossThread);
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          using InternalSyncRep_ = Streams::InternallySyncrhonizedInputOutputStream<Memory::Byte, SocketStream, SocketStream::Rep_>;
         *          Ptr unsyncStream;// = ... get from someplace - maybe make_shared<Rep_> ()...;
         *          Ptr syncStream = InternalSyncRep_::New (unsyncStream);
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-Letter-Internally-Synchronized</a>
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
#include "InternallySyncrhonizedInputOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedInputOutputStream_h_*/
