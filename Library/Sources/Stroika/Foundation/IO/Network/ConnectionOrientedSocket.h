/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedSocket_h_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedSocket_h_ 1

#include "../../StroikaPreComp.h"

#include "ConnectionOrientedStreamSocket.h"

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {
                typedef ConnectionOrientedStreamSocket ConnectionOrientedSocket[[deprecated ("use ConnectionOrientedStreamSocket in version 2.0a224")]];
            }
        }
    }
}

#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedSocket_h_*/
