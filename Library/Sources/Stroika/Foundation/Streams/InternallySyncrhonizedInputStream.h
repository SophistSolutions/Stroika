/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySyncrhonizedInputStream_h_
#define _Stroika_Foundation_Streams_InternallySyncrhonizedInputStream_h_ 1

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
    class InternallySyncrhonizedInputStream : public BASE_CLASS {
    private:
        using inherited = BASE_CLASS;

    public:
        /**
         *  'InternallySyncrhonizedInputStream' is a quasi-namespace: use Ptr or New () members.
         */
        InternallySyncrhonizedInputStream ()                                         = delete;
        InternallySyncrhonizedInputStream (const InternallySyncrhonizedInputStream&) = delete;

    public:
        using typename inherited::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Streams::InputStream<Byte>::Ptr syncStream = Streams::InternallySyncrhonizedInputStream<Byte>::New (otherInputStreamToBeSharedAcrossThread);
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
#include "InternallySyncrhonizedInputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedInputStream_h_*/
