/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Optional_h_
#define _Stroika_Foundation_Containers_Optional_h_  1

#include    "../StroikaPreComp.h"

#include    "../Execution/Synchronized.h"
#include    "../Memory/Optional.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *      @todo   Review naming/design (vis-a-vis Memory::Optional and std::experimental::optional). But get a little
 *              experience with this first (--LGP 2014-06-14)
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            // @todo - DEPRECATE THIS - use Execution::Synchronized<Memory::Optional<T,TRAITS>> directly
            template    <typename T, typename TRAITS = Memory::Optional_DefaultTraits<T>>
            using   Optional = Execution::Synchronized<Memory::Optional<T, TRAITS>>;

        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "Optional.inl"

#endif  /*_Stroika_Foundation_Containers_Optional_h_*/
