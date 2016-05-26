/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/Format.h"
#include    "../../Foundation/Characters/String_Constant.h"
#include    "../../Foundation/Characters/StringBuilder.h"
#include    "../../Foundation/Characters/ToString.h"
#include    "../../Foundation/Configuration/Endian.h"
#include    "../../Foundation/Debug/Trace.h"
#include    "../../Foundation/IO/Network/Listener.h"
#include    "../../Foundation/IO/Network/Socket.h"
#include    "../../Foundation/IO/Network/SocketStream.h"
#include    "../../Foundation/Memory/Bits.h"
#include    "../../Foundation/Memory/BLOB.h"
#include    "../../Foundation/Streams/BufferedInputStream.h"
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






// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1






/*
 *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf page 11
 */
namespace {
    enum    FunctionCodeType_   :   uint8_t {
        kReadCoils_                 =   1,
        kReadDiscreteInputs_        =   2,
        kReadHoldingResisters_      =   3,
        kReadInputRegister_         =   4,
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
        return Configuration::EndianConverter (v, Configuration::Endian::eBig, Configuration::GetEndianness ());
    }
    MBAPHeaderIsh_  FromNetwork_ (const MBAPHeaderIsh_& v)
    {
        return MBAPHeaderIsh_ { FromNetwork_ (v.fTransactionID), FromNetwork_ (v.fProtocolID), FromNetwork_ (v.fLength), v.fUnitID, v.fFunctionCode };
    }
    uint16_t    ToNetwork_ (uint16_t v)
    {
        return Configuration::EndianConverter (v, Configuration::GetEndianness (), Configuration::Endian::eBig);
    }
    MBAPHeaderIsh_  ToNetwork_ (const MBAPHeaderIsh_& v)
    {
        return MBAPHeaderIsh_ { ToNetwork_ (v.fTransactionID), ToNetwork_ (v.fProtocolID), ToNetwork_ (v.fLength), v.fUnitID, v.fFunctionCode };
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
                        return L"Read-Coils";
                    case FunctionCodeType_::kReadDiscreteInputs_:
                        return L"Read-Discrete-Inputs";
                    case FunctionCodeType_::kReadHoldingResisters_:
                        return L"Read-Holding-Resisters_";
                    case FunctionCodeType_::kReadInputRegister_:
                        return L"Read-Input-Register_";
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
    void    ConnectionHandler_ (const Socket& connectionSocket, shared_ptr<IModbusService> serviceHandler, const ServerOptions& options)
    {
        TraceContextBumper ctx ("Modbus-Connection");
#if     qDefaultTracingOn
        static  atomic<uint32_t>        sConnectionNumber_;
        uint32_t    thisModbusConnectionNumber = ++sConnectionNumber_;
        DbgTrace ("Starting modbus connection %d", thisModbusConnectionNumber);
        auto&& cleanup  =   Execution::Finally ([thisModbusConnectionNumber] () {
            DbgTrace ("Finishing modbus connection %d", thisModbusConnectionNumber);
        });
#endif
#if     qDebug
        if (auto p = connectionSocket.GetPeerAddress ()) {
            DbgTrace (L"Starting connection from peer: %s", Characters::ToString (*p).c_str ());
        }
#endif

        SocketStream        socketStream { connectionSocket };
        InputStream<Byte>   in  =   BufferedInputStream<Byte> { socketStream };     // not important, but a good idea, to avoid excessive kernel calls
        OutputStream<Byte>  out =   BufferedOutputStream<Byte> { socketStream };    // critical so we dont write multiple packets - at least some apps assume whole thing comes in one packet

        auto    checkedReadHelper = [] (const Memory::BLOB & requestPayload, uint16_t maxSecondValue) ->pair<uint16_t, uint16_t> {
            /*
             *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 16 (etc)
             */
            if (requestPayload.size () != 4)
            {
                Throw (StringException (L"Invalid payload length (expected 4)"));
            }
            uint16_t    startingAddress =   FromNetwork_ (*reinterpret_cast<const uint16_t*> (requestPayload.begin () + 0));
            uint16_t    quantity        =   FromNetwork_ (*reinterpret_cast<const uint16_t*> (requestPayload.begin () + 2));    // allowed 1..maxSecondValue
            if (not (0 < quantity and quantity <= maxSecondValue))
            {
                Throw (StringException (Characters::Format (L"Invalid quantity parameter (%d): expected value from 1..%d", quantity, maxSecondValue)));
            }
            return pair<uint16_t, uint16_t> { startingAddress,  quantity };
        };

        try {
            /*
             *  I believe (must re-read docs more carefully) - we can recieve multiple requests on a single connection, and just close the connection
             *  on our first bad packet.
             */
            while (true) {
                MBAPHeaderIsh_  requestHeader {};
                size_t  n   =   in.ReadAll (reinterpret_cast<Byte*> (&requestHeader), reinterpret_cast<Byte*> (&requestHeader + 1));
                if (n != sizeof (requestHeader)) {
                    if (n == 0) {
                        break;  // just EOF - so quietly end/close connection
                    }
                    else {
                        Throw (StringException (L"Incomplete MBAP header"));    // Bad packet - incomplete header - so closing connection
                    }
                }
                requestHeader = FromNetwork_ (requestHeader);

                /*
                 * Perform minimal validation and - for now - abandon conneciton - but soon bettter error handling (see above)
                 */
                if (requestHeader.fProtocolID != 0) {
                    Throw (StringException (L"bad protocol"));
                }
                if (requestHeader.fLength < 2) {
                    // Error cuz each full header I know of requires 2 bytes at least
                    Throw (StringException (L"Illegal short MBAP request length"));
                }

                Memory::BLOB    requestPayload = in.ReadAll (requestHeader.GetPayloadLength ());
                switch (requestHeader.fFunctionCode) {
                    case FunctionCodeType_::kReadCoils_: {
                            /*
                             *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 12
                             */
                            uint16_t    startingAddress =   checkedReadHelper (requestPayload, 0x7d0).first;
                            uint16_t    quantity        =   checkedReadHelper (requestPayload, 0x7d0).second;
                            uint16_t    endAddress      =   startingAddress + quantity;
                            size_t      quantityBytes   =   (quantity + 7) / 8;
                            SmallStackBuffer<uint16_t>  results { quantityBytes };
                            (void)::memset (results.begin (), 0, quantityBytes);  // for now - fill zeros for values not returned by backend
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace (L"Processing kReadCoils_ (%d,%d) message with request-header=%s", startingAddress, quantity, Characters::ToString (requestHeader).c_str ());
#endif
                            for (auto i : serviceHandler->ReadCoils (DiscreteRange<uint16_t> { startingAddress + 1, endAddress + 1 } .Elements ().As <IModbusService::SetRegisterNames<CoilsDescriptorType>> ())) {
                                Require (startingAddress <= i.fKey - 1 and i.fKey - 1 < endAddress);    // IModbusService must respect this!
                                if (i.fValue) {
                                    results[(i.fKey - 1 - startingAddress) / 8] |= Memory::Bit ((i.fKey - 1 - startingAddress) % 8);
                                }
                            }
                            {
                                // Response ready - format, toNetwork, and write
                                uint8_t responseLen =   static_cast<uint8_t> (quantityBytes);    // OK cuz validated in checkedReadHelper (and converted to bytes)
                                MBAPHeaderIsh_  responseHeader   = MBAPHeaderIsh_ { requestHeader.fTransactionID, requestHeader.fProtocolID, static_cast<uint16_t> (2 + responseLen), requestHeader.fUnitID, requestHeader.fFunctionCode };
                                out.WritePOD (ToNetwork_ (responseHeader));
                                out.WritePOD (responseLen);
                                out.Write (reinterpret_cast<const Byte*> (results.begin ()), reinterpret_cast<const Byte*> (results.begin ()) + responseLen);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                                DbgTrace (L"Sent response: header=%s, responseLen=%d", Characters::ToString (networkResponseHeader).c_str (), responseLen);
#endif
                            }
                        }
                        break;
                    case FunctionCodeType_::kReadDiscreteInputs_: {
                            /*
                             *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 13
                             */
                            uint16_t    startingAddress =   checkedReadHelper (requestPayload, 0x7d0).first;
                            uint16_t    quantity        =   checkedReadHelper (requestPayload, 0x7d0).second;
                            uint16_t    endAddress      =   startingAddress + quantity;
                            size_t      quantityBytes   =   (quantity + 7) / 8;
                            SmallStackBuffer<uint16_t>  results { quantityBytes };
                            (void)::memset (results.begin (), 0, quantityBytes);  // for now - fill zeros for values not returned by backend
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace (L"Processing kReadDiscreteInputs_ (%d,%d) message with request-header=%s", startingAddress, quantity, Characters::ToString (requestHeader).c_str ());
#endif
                            for (auto i : serviceHandler->ReadDiscreteInput (DiscreteRange<uint16_t> { startingAddress + 1, endAddress + 1 } .Elements ().As <IModbusService::SetRegisterNames<DiscreteInputDescriptorType>> ())) {
                                Require (startingAddress <= i.fKey - 1 and i.fKey - 1 < endAddress);        // IModbusService must respect this!
                                if (i.fValue) {
                                    results[(i.fKey - 1 - startingAddress) / 8] |= Memory::Bit ((i.fKey - 1 - startingAddress) % 8);
                                }
                            }
                            {
                                // Response ready - format, toNetwork, and write
                                uint8_t responseLen =   static_cast<uint8_t> (quantityBytes);    // OK cuz validated in checkedReadHelper (and converted to bytes)
                                MBAPHeaderIsh_  responseHeader   = MBAPHeaderIsh_ { requestHeader.fTransactionID, requestHeader.fProtocolID, static_cast<uint16_t> (2 + responseLen), requestHeader.fUnitID, requestHeader.fFunctionCode };
                                out.WritePOD (ToNetwork_ (responseHeader));
                                out.WritePOD (responseLen);
                                out.Write (reinterpret_cast<const Byte*> (results.begin ()), reinterpret_cast<const Byte*> (results.begin ()) + responseLen);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                                DbgTrace (L"Sent response: header=%s, responseLen=%d", Characters::ToString (responseHeader).c_str (), responseLen);
#endif
                            }
                        }
                        break;
                    case FunctionCodeType_::kReadHoldingResisters_: {
                            /*
                             *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 15
                             */
                            uint16_t    startingAddress =   checkedReadHelper (requestPayload, 0x7d).first;
                            uint16_t    quantity        =   checkedReadHelper (requestPayload, 0x7d).second;
                            uint16_t    endAddress      =   startingAddress + quantity;
                            SmallStackBuffer<uint16_t>  results { quantity };
                            (void)::memset (results.begin (), 0, quantity * sizeof(uint16_t));  // for now - fill zeros for values not returned by backend
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace (L"Processing kReadHoldingResisters_ (%d,%d) message with request-header=%s", startingAddress, quantity, Characters::ToString (requestHeader).c_str ());
#endif
                            for (auto i : serviceHandler->ReadHoldingRegisters (DiscreteRange<uint16_t> { startingAddress + 1, endAddress + 1 } .Elements ().As <IModbusService::SetRegisterNames<HoldingRegisterDescriptorType>> ())) {
                                Require (startingAddress <= i.fKey - 1 and i.fKey - 1 < endAddress);        // IModbusService must respect this!
                                results[i.fKey - 1 - startingAddress] = ToNetwork_ (i.fValue);
                            }
                            {
                                // Response ready - format, toNetwork, and write
                                uint8_t responseLen =   static_cast<uint8_t> (quantity);    // OK cuz validated in checkedReadHelper
                                MBAPHeaderIsh_  responseHeader   = MBAPHeaderIsh_ { requestHeader.fTransactionID, requestHeader.fProtocolID, static_cast<uint16_t> (2 + 2 * responseLen), requestHeader.fUnitID, requestHeader.fFunctionCode };
                                out.WritePOD (ToNetwork_ (responseHeader));
                                out.WritePOD (responseLen);
                                out.Write (reinterpret_cast<const Byte*> (results.begin ()), reinterpret_cast<const Byte*> (results.begin ()) + responseLen * 2);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                                DbgTrace (L"Sent response: header=%s, responseLen=%d", Characters::ToString (responseHeader).c_str (), responseLen);
#endif
                            }
                        }
                        break;
                    case FunctionCodeType_::kReadInputRegister_: {
                            /*
                             *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 16
                             */
                            uint16_t    startingAddress =   checkedReadHelper (requestPayload, 0x7d).first;
                            uint16_t    quantity        =   checkedReadHelper (requestPayload, 0x7d).second;
                            uint16_t    endAddress      =   startingAddress + quantity;
                            SmallStackBuffer<uint16_t>  results { quantity };
                            (void)::memset (results.begin (), 0, quantity * sizeof(uint16_t));  // for now - fill zeros for values not returned by backend
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace (L"Processing kReadInputRegister_ (%d,%d) message with request-header=%s", startingAddress, quantity, Characters::ToString (requestHeader).c_str ());
#endif
                            for (auto i : serviceHandler->ReadInputRegisters (DiscreteRange<uint16_t> { startingAddress + 1, endAddress + 1 } .Elements ().As <IModbusService::SetRegisterNames<InputRegisterDescriptorType>> ())) {
                                Require (startingAddress <= i.fKey - 1 and i.fKey - 1 < endAddress);        // IModbusService must respect this!
                                results[i.fKey - 1 - startingAddress] = ToNetwork_ (i.fValue);
                            }
                            {
                                // Response ready - format, toNetwork, and write
                                uint8_t responseLen =   static_cast<uint8_t> (quantity);    // OK cuz validated in checkedReadHelper
                                MBAPHeaderIsh_  responseHeader   = MBAPHeaderIsh_ { requestHeader.fTransactionID, requestHeader.fProtocolID, static_cast<uint16_t> (2 + 2 * responseLen), requestHeader.fUnitID, requestHeader.fFunctionCode };
                                out.WritePOD (ToNetwork_ (responseHeader));
                                out.WritePOD (responseLen);
                                out.Write (reinterpret_cast<const Byte*> (results.begin ()), reinterpret_cast<const Byte*> (results.begin ()) + responseLen * 2);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                                DbgTrace (L"Sent response: header=%s, responseLen=%d", Characters::ToString (responseHeader).c_str (), responseLen);
#endif
                            }
                        }
                        break;
                    case FunctionCodeType_::kWriteSingleCoil: {
                            /*
                             *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 17
                             */
                            uint16_t    outputAddress   =   checkedReadHelper (requestPayload, 0xff00).first;
                            uint16_t    value           =   checkedReadHelper (requestPayload, 0xff00).second;
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace (L"Processing kWriteSingleCoil (%d,%d) message with request-header=%s", outputAddress, value, Characters::ToString (requestHeader).c_str ());
#endif
                            serviceHandler->WriteCoils (initializer_list<KeyValuePair<CoilsDescriptorType::NameType, CoilsDescriptorType::ValueType>> { {outputAddress + 1, value == 0 ? false : true } });
                            {
                                // Response ready - format, toNetwork, and write
                                out.WritePOD (requestHeader);
                                out.WritePOD (ToNetwork_ (outputAddress));
                                out.WritePOD (ToNetwork_ (value));
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                                DbgTrace (L"Sent response: header=%s", Characters::ToString (requestHeader).c_str ());
#endif
                            }
                        }
                        break;
                    default: {
                            DbgTrace (L"UNREGONIZED FunctionCode (nyi probably) - so echo ILLEGAL_FUNCTION code");
                            MBAPHeaderIsh_ responseHeader   =   requestHeader;
                            responseHeader.fFunctionCode |= 0x80;   // set high bit
                            out.WritePOD (ToNetwork_ (responseHeader));
                            uint8_t    exceptionCode    =   static_cast<uint8_t> (ExceptionCode::ILLEGAL_FUNCTION);
                            out.Write (reinterpret_cast<const Byte*> (&exceptionCode), reinterpret_cast<const Byte*> (&exceptionCode + 1));
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace (L"Sent UNREGONIZED_FUNCTION response: header=%s, and exceptionCode=%d", Characters::ToString (responseHeader).c_str (), exceptionCode);
#endif
                        }
                }
                out.Flush ();   // since buffering output, be sure to flush after each response!
            }
        }
        catch (...) {
            // Anytime we leave the loop due to an exception, thats worth a log note
            if (options.fLogger) {
                options.fLogger.value ()->Log (Logger::Priority::eWarning, L"ModbusTCP connection ended abnormally: %s", Characters::ToString (current_exception ()).c_str ());
            }
            ReThrow ();
        }
    }
}






/*
 ********************************************************************************
 *********** Frameworks::Modbus::MakeModbusTCPServerThread **********************
 ********************************************************************************
 */
Execution::Thread   Modbus::MakeModbusTCPServerThread (const shared_ptr<IModbusService>& serviceHandler, const ServerOptions& options)
{
    shared_ptr<Execution::ThreadPool>       usingThreadPool = options.fThreadPool;
    bool                                    weOwnPool { false };
    if (usingThreadPool == nullptr) {
        usingThreadPool = make_shared<Execution::ThreadPool> (1);
        weOwnPool = true;
    }

    // Note - we return thread not started, so caller must explicitly start, but internal threads start immediately
    auto onModbusConnection = [serviceHandler, options, usingThreadPool] (const Socket & s) {
        usingThreadPool->AddTask ([serviceHandler, options, s]  () { ConnectionHandler_ (s, serviceHandler, options); });
    };
    return Thread {
        [onModbusConnection, options, usingThreadPool, weOwnPool] ()
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            TraceContextBumper ctx ("Modbus-Listener");
#endif
            uint16_t    usingPortNumber =   options.fListenPort.Value (502);
            if (options.fLogger) {
                options.fLogger.value ()->Log (Logger::Priority::eInfo, L"Listening for ModbusTCP requests on port %d", usingPortNumber);
            }

            auto&& cleanup  =   Execution::Finally ([usingThreadPool, weOwnPool] () {
                if (weOwnPool) {
                    // we should only do this if WE constructed the threadpool.
                    // If someone else did and passed it in - their job to cleanup (it could be running other tasks!)
                    Thread::SuppressInterruptionInContext suppress; // so subsidiary threads cleanup
                    usingThreadPool->AbortAndWaitForDone ();
                }
            });

            Listener l { SocketAddress { Network::V4::kAddrAny, usingPortNumber }, options.fBindFlags.Value (), onModbusConnection };
            WaitableEvent { WaitableEvent::eAutoReset } .Wait ();   // forever (til thread abort)
        },
        String_Constant { L"Modbus-Listener"}
    };
}
