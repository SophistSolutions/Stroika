/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySyncrhonizedOutputStream_h_
#define _Stroika_Foundation_Streams_InternallySyncrhonizedOutputStream_h_ 1

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

namespace Stroika::Foundation {
    namespace Streams {

        /**
         */
        template <typename ELEMENT_TYPE, typename BASE_CLASS = OutputStream<ELEMENT_TYPE>, typename BASE_REP_TYPE = typename BASE_CLASS::_IRep>
        class InternallySyncrhonizedOutputStream : public BASE_CLASS {
        private:
            using inherited = BASE_CLASS;

        public:
            /**
             *  'InternallySyncrhonizedOutputStream' is a quasi-namespace: use Ptr or New () members.
             */
            InternallySyncrhonizedOutputStream ()                                          = delete;
            InternallySyncrhonizedOutputStream (const InternallySyncrhonizedOutputStream&) = delete;

        public:
            using typename inherited::Ptr;

        public:
            /**
             *  \par Example Usage
             *      \code
             *          Streams::OutputStream<Byte>::Ptr syncStream = Streams::InternallySyncrhonizedOutputStream<Byte>::New (otherOutputStreamToBeSharedAcrossThread);
             *      \endcode
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Letter-Internally-Synchonized">C++-Standard-Thread-Safety-Letter-Internally-Synchonized</a>
             */
            template <typename... ARGS>
            static Ptr New (ARGS&&... args);

        private:
            class Rep_;
        };
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySyncrhonizedOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedOutputStream_h_*/
