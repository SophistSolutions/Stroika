/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Optional_h_
#define _Stroika_Foundation_Containers_Optional_h_  1

#include    "../StroikaPreComp.h"

#include    "../Memory/Optional.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


#pragma message ("Warning: Obsolete as of Stroika version 2.0a41: use Execution::Synchronized<Memory::Optional<T, TRAITS>>")
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
