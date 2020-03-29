/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_WaitForSocketIOReady_h_
#define _Stroika_Foundation_IO_Network_WaitForSocketIOReady_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Containers/Bijection.h"
#include "../../Execution/WaitForIOReady.h"
#include "../../Time/Duration.h"

#include "Socket.h"

/*
 * TODO:
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika::Foundation::IO::Network {
    template <typename SOCKET_SUBTYPE>
    using WaitForSocketIOReady [[deprecated ("use WaitForIOReady since 2.1a5")]] = Execution::WaitForIOReady<SOCKET_SUBTYPE>;
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_IO_Network_WaitForSocketIOReady_h_*/
