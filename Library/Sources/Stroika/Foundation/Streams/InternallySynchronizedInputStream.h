/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedInputStream_h_
#define _Stroika_Foundation_Streams_InternallySynchronizedInputStream_h_ 1

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
     */
    template <typename ELEMENT_TYPE, typename BASE_CLASS = InputStream<ELEMENT_TYPE>, typename BASE_REP_TYPE = typename BASE_CLASS::_IRep>
    class InternallySynchronizedInputStream : public BASE_CLASS {
    private:
        using inherited = BASE_CLASS;

    public:
        /**
         *  'InternallySynchronizedInputStream' is a quasi-namespace: use Ptr or New () members.
         */
        InternallySynchronizedInputStream ()                                         = delete;
        InternallySynchronizedInputStream (const InternallySynchronizedInputStream&) = delete;

    public:
        using typename inherited::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Streams::InputStream<byte>::Ptr syncStream = Streams::InternallySynchronizedInputStream<byte>::New (otherInputStreamToBeSharedAcrossThread);
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
#include "InternallySynchronizedInputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputStream_h_*/
