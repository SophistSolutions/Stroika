/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Modbus_Server_h_
#define _Stroika_Frameworks_Modbus_Server_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Execution/Logger.h"
#include    "../../Foundation/Execution/Thread.h"
#include    "../../Foundation/Memory/Optional.h"

#include    "IModbusService.h"



/**
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   Modbus {


            using   namespace   Stroika::Foundation;

            struct ServerOptions {
                Memory::Optional<uint16_t>              fListenPort;        // By spec, defaults to 502
                Memory::Optional<Execution::Logger*>    fLogger;
            };

            /*
             *  DRAFT API
             */
            Execution::Thread   MakeModbusTCPServerThread (shared_ptr<IModbusService> serviceHandler, const ServerOptions& options = ServerOptions {});


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Server.inl"

#endif  /*_Stroika_Frameworks_Modbus_Server_h_*/
