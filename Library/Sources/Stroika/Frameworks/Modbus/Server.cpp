/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/Format.h"
#include    "../../Foundation/Characters/String_Constant.h"
#include    "../../Foundation/Characters/StringBuilder.h"
#include    "../../Foundation/Configuration/Endian.h"
#include    "../../Foundation/Debug/Trace.h"
#include    "../../Foundation/IO/Network/Listener.h"
#include    "../../Foundation/IO/Network/Socket.h"
#include    "../../Foundation/IO/Network/SocketStream.h"
#include    "../../Foundation/Memory/BLOB.h"
#include    "../../Foundation/Streams/BufferedOutputStream.h"
#include    "../../Foundation/Traversal/DiscreteRange.h"

#include    "Server.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Debug;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Traversal;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::Modbus;



/*
 *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf page 11
 */
namespace {
    enum    FunctionCodeType_   :   uint8_t {
        kReadCoils_                 =   1,
        kReadHoldingResisters_      =   3,
        kReadInputResister_         =   4,
        kWriteSingleCoil            =   5,
    };
}




namespace {

    /*
     *  Roughly from http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf - but they describe
     *  only first 7 bytes as header. Trouble is - hard to do in C++ - only 7 type struct.  Subclass combing
     *  longer.
     *
     *  Plus always seems used with following function code. So go with the flow, and add the word ish to the name.
     */
    struct MBAPHeaderIsh_ {
        alignas(2)  uint16_t    fTransactionID;
        alignas(2)  uint16_t    fProtocolID;
        alignas(2)  uint16_t    fLength;
        alignas(1)  uint8_t     fUnitID;
        alignas(1)  uint8_t     fFunctionCode;

        size_t  GetPayloadLength () const
        {
            // Funky - but from http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf -
            //       The length field is a byte count of the following fields, including the Unit Identifier and data fields.
            Require (fLength >= 2);
            return fLength - 2;
        }
    };

    static_assert (sizeof (MBAPHeaderIsh_) == 8, "");
    static_assert (offsetof (MBAPHeaderIsh_, fTransactionID) == 0, "");
    static_assert (offsetof (MBAPHeaderIsh_, fProtocolID) == 2, "");
    static_assert (offsetof (MBAPHeaderIsh_, fLength) == 4, "");
    static_assert (offsetof (MBAPHeaderIsh_, fUnitID) == 6, "");
    static_assert (offsetof (MBAPHeaderIsh_, fFunctionCode) == 7, "");

}



namespace {
    /*
     *  All transmissions in big endian (network byte order)
     */
    uint16_t    FromNetwork_ (uint16_t v)
    {
        return Configuration::EndianConverter (v, Configuration::GetEndianness (), Configuration::Endian::eBig);
    }
    MBAPHeaderIsh_  FromNetwork_ (const MBAPHeaderIsh_& v)
    {
        return MBAPHeaderIsh_ {
            FromNetwork_ (v.fTransactionID),
            FromNetwork_ (v.fProtocolID),
            FromNetwork_ (v.fLength),
            v.fUnitID,
            v.fFunctionCode,
        };
    }
    uint16_t    ToNetwork_ (uint16_t v)
    {
        return Configuration::EndianConverter (v, Configuration::Endian::eBig, Configuration::GetEndianness ());
    }
    MBAPHeaderIsh_  ToNetwork_ (const MBAPHeaderIsh_& v)
    {
        return MBAPHeaderIsh_ {
            ToNetwork_ (v.fTransactionID),
            ToNetwork_ (v.fProtocolID),
            ToNetwork_ (v.fLength),
            v.fUnitID,
            v.fFunctionCode,
        };
    }
}




// For internal debugging
namespace Stroika {
    namespace Foundation {
        namespace Characters {
            template    <>
            String  ToString (const FunctionCodeType_& f)
            {
                switch (f) {
                    case FunctionCodeType_::kReadCoils_:
                        return L"ReadCoils_";
                    case FunctionCodeType_::kReadHoldingResisters_:
                        return L"ReadHoldingResisters_";
                    case FunctionCodeType_::kReadInputResister_:
                        return L"ReadInputResister_";
                    case FunctionCodeType_::kWriteSingleCoil:
                        return L"WriteSingleCoil";
                }
                return Format (L"%d", f);
            }
            template    <>
            String  ToString (const MBAPHeaderIsh_& mh)
            {
                StringBuilder   sb;
                sb += L"{";
                sb += L"TransactionID: " + Characters::ToString (mh.fTransactionID) + L", ";
                sb += L"ProtocolID: " + Characters::ToString (mh.fProtocolID) + L", ";
                sb += L"Length: " + Characters::ToString (mh.fLength) + L", ";
                sb += L"UnitID: " + Characters::ToString (mh.fUnitID) + L", ";
                sb += L"FunctionCode: " + Characters::ToString (mh.fFunctionCode) + L", ";
                sb += L"}";
                return sb.str ();
            }
        }
    }
}





namespace {
    // @todo fix logging/reporting, etc
    void    ConnectionHandler_ (const Socket& connectionSocket, shared_ptr<IModbusService> serviceHandler, const ServerOptions& options)
    {
        TraceContextBumper ctx ("Modbus-Connection");
        SocketStream socketStream { connectionSocket };
        InputStream<Byte>   in  =   socketStream;
        OutputStream<Byte>  out =   BufferedOutputStream<Byte> { socketStream };    // critical so we dont write multiple packets - at least some apps assume whole thing comes in one packet

        auto    checkedReadHelper = [] (const Memory::BLOB & requestPayload, uint16_t maxSecondValue) ->pair<uint16_t, uint16_t> {
            /*
             *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 16 (etc)
             */
            if (requestPayload.size () != 4)
            {
                Throw (StringException (L"bad params")); //  should log error - and maybe throw/return /send to remote side error code?
            }
            uint16_t    startingAddress =   FromNetwork_ (*reinterpret_cast<const uint16_t*> (requestPayload.begin () + 0));
            uint16_t    quantity        =   FromNetwork_ (*reinterpret_cast<const uint16_t*> (requestPayload.begin () + 2));    // allowed 1..0x7d
            if (not (0 < quantity and quantity <= maxSecondValue))
            {
                Throw (StringException (L"bad params")); //  should log error - and maybe throw/return /send to remote side error code?
            }
            return pair<uint16_t, uint16_t> { startingAddress,  quantity };
        };

        /*
         *  I believe (must re-read docs more carefully) - we can recieve multiple requests on a single connection, and just close the connection
         *  on our first bad packet.
         */
        while (true) {
            MBAPHeaderIsh_  requestHeader {};
            size_t  n   =   in.ReadAll (reinterpret_cast<Byte*> (&requestHeader), reinterpret_cast<Byte*> (&requestHeader + 1));
            if (n != sizeof (requestHeader)) {
                return; //  if not zero should log error - and maybe throw/return /send to remote side error code?
            }
            requestHeader = FromNetwork_ (requestHeader);

            /*
             * Perform minimal validation and - for now - abandon conneciton - but soon bettter error handling (see above)
             */
            if (requestHeader.fProtocolID != 0) {
                Throw (StringException (L"bad protocol")); //  should log error - and maybe throw/return /send to remote side error code?
            }
            if (requestHeader.fLength < 2) {
                // PUT LOGGER INTO OPTIONS OBJET
                // Error cuz we full ehader I know of requires 2 bytes at least
                Throw (StringException (L"bad request length")); //  should log error - and maybe throw/return /send to remote side error code?
            }

            Memory::BLOB    requestPayload = in.ReadAll (requestHeader.GetPayloadLength ());
            switch (requestHeader.fFunctionCode) {
                case FunctionCodeType_::kReadInputResister_: {
                        /*
                         *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 16
                         */
                        uint16_t    startingAddress =   checkedReadHelper (requestPayload, 0x7d).first;
                        uint16_t    quantity        =   checkedReadHelper (requestPayload, 0x7d).second;
                        uint16_t    endAddress      =   startingAddress + quantity;
                        SmallStackBuffer<uint16_t>  results { quantity };
                        (void)::memset (results.begin (), 0, quantity * sizeof(uint16_t));  // for now - fill zeros for values not returned by backend
                        for (auto i : serviceHandler->ReadInputRegisters (DiscreteRange<uint16_t> { startingAddress, endAddress } .Elements ().As <IModbusService::SetRegisterNames<InputRegisterDescriptorType>> ())) {
                            Require (startingAddress <= i.fKey and i.fKey < endAddress);        // IModbusService must respect this!
                            results[i.fKey - startingAddress] = ToNetwork_ (i.fValue);
                        }
                        {
                            // Response ready - format, toNetwork, and write
                            uint8_t responseLen =   static_cast<uint8_t> (quantity);    // OK cuz validated in checkedReadHelper
                            out.Write (reinterpret_cast<const Byte*> (&responseLen), reinterpret_cast<const Byte*> (&responseLen + 1));
                            out.Write (reinterpret_cast<const Byte*> (results.begin ()), reinterpret_cast<const Byte*> (results.begin ()) + responseLen * 2);
                        }
                    }
                    break;
                default: {
                        DbgTrace (L"UNREGONIZED PACKET SO CLOSING CONNECTION");
                        return; //  should log error - and maybe throw/return /send to remote side error code?
                    }
            }
            out.Flush ();   // since buffering output, be sure to flush after each response!
        }
    }
}







Execution::Thread   Modbus::MakeModbusTCPServerThread (shared_ptr<IModbusService> serviceHandler, const ServerOptions& options)
{
    // Note - we return thread not started, so caller must explicitly start, but internal threads start immediately
    auto onModbusConnection = [serviceHandler, options] (const Socket & s) {
        // @todo - use ThreadPools - and run connection in threadpool
        static  atomic<uint32_t>        sConnectionNumber_;
        Thread ([serviceHandler, options, s]  () { ConnectionHandler_ (s, serviceHandler, options); }, Thread::eAutoStart, Format (L"Modbus Connection %d", sConnectionNumber_++));
    };
    return Thread {
        [onModbusConnection, options] ()
        {
            TraceContextBumper ctx ("Modbus-Listener");
            Listener l { SocketAddress { Network::V4::kAddrAny, options.fListenPort.Value (502) }, onModbusConnection };
            WaitableEvent { WaitableEvent::eAutoReset } .Wait ();   // forever (til thread abort)
        },
        String_Constant { L"Modbus-Listener"}
    };
}
