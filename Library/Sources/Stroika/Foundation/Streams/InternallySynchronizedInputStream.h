/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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

namespace Stroika::Foundation::Streams::InternallySynchronizedInputStream {

    /**
     */
    struct DefaultOptions {
        /**
         * Default to using non-recursive simple mutex (so beware of one rep method calling another - if you do, override this argument to New).
         */
        using MutexType = mutex;
    };

    /**
     *  \par Example Usage
     *      \code
     *          Streams::InputStream::Ptr<byte> syncStream = Streams::InternallySynchronizedInputStream::New<byte> (otherInputStreamToBeSharedAcrossThread);
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     */
    template <typename BASE_REP_TYPE, typename OPTIONS = DefaultOptions, typename... ARGS>
    typename InputStream::Ptr<typename BASE_REP_TYPE::ElementType> New (OPTIONS o = {}, ARGS&&... args);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySynchronizedInputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputStream_h_*/
