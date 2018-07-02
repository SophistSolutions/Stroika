/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Modbus_Server_h_
#define _Stroika_Frameworks_Modbus_Server_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Execution/Logger.h"
#include "../../Foundation/Execution/Thread.h"
#include "../../Foundation/Execution/ThreadPool.h"
#include "../../Foundation/IO/Network/Socket.h"
#include "../../Foundation/Memory/Optional.h"

#include "IModbusService.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  \note Testing Note
 *        This code was tested and used by a client as of around Stroika v2.0a204, but may not have been used since, and
 *        probably has not undergone testing since. It could have small breaks, but I dont have stuff with which to test, so
 *        as-is - sorry
 *
 * TODO:
 *
 *      @todo
 */

namespace Stroika {
    namespace Frameworks {
        namespace Modbus {

            using namespace Stroika::Foundation;

            /**
             */
            struct ServerOptions {
                /**
                 *  By spec, defaults to 502
                 */
                optional<uint16_t> fListenPort;

                /**
                 *  Logger to write interesting messages to.
                 */
                optional<Execution::Logger*> fLogger;

                /**
                 *  Often helpful to specify reUseAddr = true, to avoid trouble restarting service
                 */
                optional<IO::Network::Socket::BindFlags> fBindFlags;

                /**
                 *  To specify size, provide your own threadpool
                 */
                shared_ptr<Execution::ThreadPool> fThreadPool;

                /**
                 *  defaults to true iff argument fThreadPool null.
                 *
                 *  \note   Either let this class or caller must shutdown threadpool before exiting app.
                 */
                optional<bool> fShutdownThreadPool;
            };

            /**
             *  Construct a Modbus TCP Listener which will listen for Modbus connections, run them using
             *  the optionally provided thread pool (and other configuration options) and send actual handler
             *  requests to the argument IModbusService handler.
             *
             *  Supported Function Codes:
             *      o   kReadCoils              (#1)
             *      o   ReadDiscreteInputs      (#2)
             *      o   ReadHoldingResisters    (#3)
             *      o   ReadInputRegister       (#4)
             *      o   WriteSingleCoil         (#5)
             *
             *  \req serviceHandler != nullptr
             */
            Execution::Thread::Ptr MakeModbusTCPServerThread (const shared_ptr<IModbusService>& serviceHandler, const ServerOptions& options = ServerOptions{});

            template <typename MODBUS_REGISTER_DESCRIPTOR, typename SRC_TYPE>
            void SplitSrcAcrossOutputs (const SRC_TYPE& s, typename MODBUS_REGISTER_DESCRIPTOR::NameType baseRegister, Containers::Mapping<typename MODBUS_REGISTER_DESCRIPTOR::NameType, typename MODBUS_REGISTER_DESCRIPTOR::ValueType>* update);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Server.inl"

#endif /*_Stroika_Frameworks_Modbus_Server_h_*/
