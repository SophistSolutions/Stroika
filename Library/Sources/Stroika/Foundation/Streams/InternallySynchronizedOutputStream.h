/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedOutputStream_h_
#define _Stroika_Foundation_Streams_InternallySynchronizedOutputStream_h_ 1

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
    template <typename ELEMENT_TYPE, typename BASE_CLASS = OutputStream<ELEMENT_TYPE>, typename BASE_REP_TYPE = typename BASE_CLASS::_IRep>
    class InternallySynchronizedOutputStream : public BASE_CLASS {
    private:
        using inherited = BASE_CLASS;

    public:
        /**
         *  'InternallySynchronizedOutputStream' is a quasi-namespace: use Ptr or New () members.
         */
        InternallySynchronizedOutputStream ()                                          = delete;
        InternallySynchronizedOutputStream (const InternallySynchronizedOutputStream&) = delete;

    public:
        using typename inherited::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Streams::OutputStream<byte>::Ptr syncStream = Streams::InternallySynchronizedOutputStream<byte>::New (otherOutputStreamToBeSharedAcrossThread);
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
#include "InternallySynchronizedOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedOutputStream_h_*/
