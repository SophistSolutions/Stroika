/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_SerializeItemToBLOB_h_
#define _Stroika_Foundation_Streams_iostream_SerializeItemToBLOB_h_ 1

#include "../../StroikaPreComp.h"

#include <iostream>

#include "../../Memory/BLOB.h"

/**
 * TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {
            namespace iostream {

                /**
                 *  Handy utility to convert an element to a string representation, and then
                 *  to a BLOB (for the purpose of encption, or digest or whatever).
                 *
                 *  @todo this is NOT well defined (utf8 or wide characters??? - define that it uses
                 *  stringstream
                 */
                template <typename T>
                Memory::BLOB SerializeItemToBLOB (const T& elt);
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SerializeItemToBLOB.inl"

#endif /*_Stroika_Foundation_Streams_iostream_SerializeItemToBLOB_h_*/
