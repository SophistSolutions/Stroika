/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Configuration/Endian.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/IO/Network/Listener.h"
#include "Stroika/Foundation/IO/Network/Socket.h"
#include "Stroika/Foundation/IO/Network/SocketStream.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/Bits.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/BufferedInputStream.h"
#include "Stroika/Foundation/Streams/BufferedOutputStream.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"

#include "Server.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Traversal;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Modbus;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf page 11
 */
namespace {
    enum FunctionCodeType_ : uint8_t {
        kReadCoils_            = 1,
        kReadDiscreteInputs_   = 2,
        kReadHoldingResisters_ = 3,
        kReadInputRegister_    = 4,
        kWriteSingleCoil_      = 5,
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
        alignas (2) uint16_t fTransactionID;
        alignas (2) uint16_t fProtocolID;
        alignas (2) uint16_t fLength;
        alignas (1) uint8_t fUnitID;
        alignas (1) FunctionCodeType_ fFunctionCode;

        static constexpr size_t kExtraLengthFromThisHeaderAccountedInPayloadLength{2};

        size_t GetPayloadLength () const
        {
            // Funky - but from http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf -
            //       The length field is a byte count of the following fields, including the Unit Identifier and data fields.
            Require (fLength >= kExtraLengthFromThisHeaderAccountedInPayloadLength);
            return fLength - kExtraLengthFromThisHeaderAccountedInPayloadLength;
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
    uint16_t FromNetwork_ (uint16_t v)
    {
        return Configuration::EndianConverter (v, Configuration::Endian::eBig, Configuration::GetEndianness ());
    }
    MBAPHeaderIsh_ FromNetwork_ (const MBAPHeaderIsh_& v)
    {
        return MBAPHeaderIsh_{FromNetwork_ (v.fTransactionID), FromNetwork_ (v.fProtocolID), FromNetwork_ (v.fLength), v.fUnitID, v.fFunctionCode};
    }
    uint16_t ToNetwork_ (uint16_t v)
    {
        return Configuration::EndianConverter (v, Configuration::GetEndianness (), Configuration::Endian::eBig);
    }
    MBAPHeaderIsh_ ToNetwork_ (const MBAPHeaderIsh_& v)
    {
        return MBAPHeaderIsh_{ToNetwork_ (v.fTransactionID), ToNetwork_ (v.fProtocolID), ToNetwork_ (v.fLength), v.fUnitID, v.fFunctionCode};
    }
}

// For internal debugging
namespace Stroika::Foundation::Characters {
    template <>
    String ToString (const FunctionCodeType_& f)
    {
        switch (f) {
            case FunctionCodeType_::kReadCoils_:
                return "Read-Coils"_k;
            case FunctionCodeType_::kReadDiscreteInputs_:
                return "Read-Discrete-Inputs"_k;
            case FunctionCodeType_::kReadHoldingResisters_:
                return "Read-Holding-Resisters_"_k;
            case FunctionCodeType_::kReadInputRegister_:
                return "Read-Input-Register_"_k;
            case FunctionCodeType_::kWriteSingleCoil_:
                return "WriteSingleCoil"_k;
        }
        return Format ("{}"_f, f);
    }
    template <>
    String ToString (const MBAPHeaderIsh_& mh)
    {
        StringBuilder sb;
        sb << "{"sv;
        sb << "TransactionID: "sv << mh.fTransactionID << ", "sv;
        sb << "ProtocolID: "sv << mh.fProtocolID << ", "sv;
        sb << "Length: "sv << mh.fLength << ", "sv;
        sb << "UnitID: "sv << mh.fUnitID << ", "sv;
        sb << "FunctionCode: "sv << mh.fFunctionCode;
        sb << "}"sv;
        return sb;
    }
}

namespace {
    void ConnectionHandler_ (const ConnectionOrientedStreamSocket::Ptr& connectionSocket, shared_ptr<IModbusService> serviceHandler,
                             const ServerOptions& options)
    {
        TraceContextBumper ctx{"Modbus-Connection"};
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        static atomic<uint32_t> sConnectionNumber_;
        uint32_t                thisModbusConnectionNumber = ++sConnectionNumber_;
        DbgTrace ("Starting Modbus connection {}"_f, thisModbusConnectionNumber);
        [[maybe_unused]] auto&& cleanup = Execution::Finally (
            [thisModbusConnectionNumber] () { DbgTrace ("Finishing Modbus connection {}"_f, thisModbusConnectionNumber); });
#endif
        if constexpr (qDebug) {
            if (auto p = connectionSocket.GetPeerAddress ()) {
                DbgTrace ("Starting connection from peer: {}"_f, *p);
            }
        }

        SocketStream::Ptr socketStream = SocketStream::New (connectionSocket);
        InputStream::Ptr<byte> in = BufferedInputStream::New<byte> (socketStream); // not important, but a good idea, to avoid excessive kernel calls
        OutputStream::Ptr<byte> out = BufferedOutputStream::New<byte> (socketStream); // critical so we don't write multiple packets - at least some apps assume whole thing comes in one packet

        auto checkedReadHelperPayload2Shorts = [] (const Memory::BLOB& requestPayload, uint16_t minSecondValue,
                                                   uint16_t maxSecondValue) -> pair<uint16_t, uint16_t> {
            /*
             *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 16 (etc)
             */
            if (requestPayload.size () != 4) {
                DbgTrace ("requestPayload={}"_f, requestPayload);
                Throw (Execution::Exception{Characters::Format ("Invalid payload length (got {}, expected 4)"_f, requestPayload.size ())});
            }
            uint16_t startingAddress = FromNetwork_ (*reinterpret_cast<const uint16_t*> (requestPayload.begin () + 0));
            uint16_t quantity = FromNetwork_ (*reinterpret_cast<const uint16_t*> (requestPayload.begin () + 2)); // allowed 1..maxSecondValue
            if (not(minSecondValue <= quantity and quantity <= maxSecondValue)) {
                Throw (Execution::Exception{Characters::Format ("Invalid quantity parameter ({}): expected value from {}..{}"_f, quantity,
                                                                minSecondValue, maxSecondValue)});
            }
            return pair<uint16_t, uint16_t>{startingAddress, quantity};
        };

        try {
            /*
             *  I believe (must re-read docs more carefully) - we can receive multiple requests on a single connection, and just close the connection
             *  on our first bad packet.
             */
            while (true) {
                MBAPHeaderIsh_ requestHeader; // intentionally don't initialize since either all read, or we throw
                size_t         n = in.ReadAll (as_writable_bytes (span{&requestHeader, 1})).size ();
                if (n != sizeof (requestHeader)) {
                    if (n == 0) {
                        break; // just EOF - so quietly end/close connection
                    }
                    else {
                        Throw (Execution::Exception{"Incomplete MBAP header"sv}); // Bad packet - incomplete header - so closing connection
                    }
                }
                requestHeader = FromNetwork_ (requestHeader);

                /*
                 * Perform minimal validation and - for now - abandon conneciton - but soon bettter error handling (see above)
                 */
                if (requestHeader.fProtocolID != 0) {
                    Throw (Execution::Exception{"bad protocol"sv});
                }
                if (requestHeader.fLength < 2) {
                    // Error cuz each full header I know of requires 2 bytes at least
                    Throw (Execution::Exception{"Illegal short MBAP request length"sv});
                }

                Memory::BLOB requestPayload      = in.ReadAll (requestHeader.GetPayloadLength ());
                auto         zeroToOneBased      = [] (uint16_t i) -> uint16_t { return i + 1; };
                auto         oneBasedToZeroBased = [] (uint16_t i) -> uint16_t { return i - 1; };
                switch (requestHeader.fFunctionCode) {
                    case FunctionCodeType_::kReadCoils_: {
                        /*
                         *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 12
                         */
                        uint16_t startingAddress = checkedReadHelperPayload2Shorts (requestPayload, 1, 0x7d0).first;
                        uint16_t quantity        = checkedReadHelperPayload2Shorts (requestPayload, 1, 0x7d0).second;
                        Assert (quantity >= 1);
                        uint16_t             endInclusiveAddress = startingAddress + quantity - 1u;
                        size_t               quantityBytes       = (quantity + 7) / 8;
                        StackBuffer<uint8_t> results{quantityBytes};
                        (void)::memset (results.begin (), 0, quantityBytes); // for now - fill zeros for values not returned by backend
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("Processing kReadCoils_ (starting0Address: {}, quantity: {}) message with request-header={}",
                                  startingAddress, quantity, requestHeader);
#endif
                        for (const auto& i :
                             serviceHandler->ReadCoils (DiscreteRange<uint16_t>{zeroToOneBased (startingAddress), zeroToOneBased (endInclusiveAddress)}
                                                            .Elements ()
                                                            .As<IModbusService::SetRegisterNames<CoilsDescriptorType>> ())) {
                            if (startingAddress <= oneBasedToZeroBased (i.fKey) and oneBasedToZeroBased (i.fKey) < endInclusiveAddress and i.fValue) {
                                results[(oneBasedToZeroBased (i.fKey) - startingAddress) / 8] |=
                                    Memory::Bit ((oneBasedToZeroBased (i.fKey) - startingAddress) % 8);
                            }
                        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"results bitmask bytes={}", Memory::BLOB{reinterpret_cast<const byte*> (results.begin ()),
                                                                            reinterpret_cast<const byte*> (results.end ())});
#endif
                        {
                            // Response ready - format, toNetwork, and write
                            uint8_t responseLen = static_cast<uint8_t> (quantityBytes); // OK cuz validated in checkedReadHelperPayload2Shorts (and converted to bytes)
                            MBAPHeaderIsh_ responseHeader =
                                MBAPHeaderIsh_{requestHeader.fTransactionID, requestHeader.fProtocolID,
                                               static_cast<uint16_t> (MBAPHeaderIsh_::kExtraLengthFromThisHeaderAccountedInPayloadLength +
                                                                      sizeof (responseLen) + responseLen),
                                               requestHeader.fUnitID, requestHeader.fFunctionCode};
                            out.Write (Memory::AsBytes (ToNetwork_ (responseHeader)));
                            out.Write (Memory::AsBytes (responseLen));
                            out.Write (span{results.begin (), responseLen});
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace ("Sent response: header={}, responseLen={}, responsePayload={}"_f, responseHeader, responseLen,
                                      Memory::BLOB{results.begin (), results.begin () + responseLen});
#endif
                        }
                    } break;
                    case FunctionCodeType_::kReadDiscreteInputs_: {
                        /*
                         *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 13
                         */
                        uint16_t startingAddress = checkedReadHelperPayload2Shorts (requestPayload, 1, 0x7d0).first;
                        uint16_t quantity        = checkedReadHelperPayload2Shorts (requestPayload, 1, 0x7d0).second;
                        Assert (quantity >= 1);
                        uint16_t             endInclusiveAddress = startingAddress + quantity - 1u;
                        size_t               quantityBytes       = (quantity + 7) / 8;
                        StackBuffer<uint8_t> results{quantityBytes}; // for now - fill zeros for values not returned by backend
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("Processing kReadDiscreteInputs_ (starting0Address: {}, quantity: {}) message with request-header={}"_f,
                                  startingAddress, quantity, requestHeader);
#endif
                        for (const auto& i : serviceHandler->ReadDiscreteInput (
                                 DiscreteRange<uint16_t>{zeroToOneBased (startingAddress), zeroToOneBased (endInclusiveAddress)}
                                     .Elements ()
                                     .As<IModbusService::SetRegisterNames<DiscreteInputDescriptorType>> ())) {
                            if (startingAddress <= oneBasedToZeroBased (i.fKey) and oneBasedToZeroBased (i.fKey) <= endInclusiveAddress) {
                                if (i.fValue) {
                                    results[(oneBasedToZeroBased (i.fKey) - startingAddress) / 8] |=
                                        Memory::Bit ((oneBasedToZeroBased (i.fKey) - startingAddress) % 8);
                                }
                            }
                        }
                        {
                            // Response ready - format, toNetwork, and write
                            uint8_t responseLen = static_cast<uint8_t> (quantityBytes); // OK cuz validated in checkedReadHelperPayload2Shorts (and converted to bytes)
                            MBAPHeaderIsh_ responseHeader =
                                MBAPHeaderIsh_{requestHeader.fTransactionID, requestHeader.fProtocolID,
                                               static_cast<uint16_t> (MBAPHeaderIsh_::kExtraLengthFromThisHeaderAccountedInPayloadLength +
                                                                      sizeof (responseLen) + responseLen),
                                               requestHeader.fUnitID, requestHeader.fFunctionCode};
                            out.Write (Memory::AsBytes (ToNetwork_ (responseHeader)));
                            out.Write (Memory::AsBytes (responseLen));
                            out.Write (span{results.begin (), responseLen});
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace ("Sent response: header={}, responseLen={}"_f, responseHeader, responseLen);
#endif
                        }
                    } break;
                    case FunctionCodeType_::kReadHoldingResisters_: {
                        /*
                         *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 15
                         */
                        uint16_t startingAddress = checkedReadHelperPayload2Shorts (requestPayload, 1, 0x7d).first;
                        uint16_t quantity        = checkedReadHelperPayload2Shorts (requestPayload, 1, 0x7d).second;
                        Assert (quantity >= 1);
                        uint16_t              endInclusiveAddress = startingAddress + quantity - 1u;
                        StackBuffer<uint16_t> results{quantity}; // for now - fill zeros for values not returned by backend
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"Processing kReadHoldingResisters_ (starting0Address: {}, quantity: {}) message with request-header={}"_f,
                                  startingAddress, quantity, requestHeader);
#endif
                        for (const auto& i : serviceHandler->ReadHoldingRegisters (
                                 DiscreteRange<uint16_t>{zeroToOneBased (startingAddress), zeroToOneBased (endInclusiveAddress)}
                                     .Elements ()
                                     .As<IModbusService::SetRegisterNames<HoldingRegisterDescriptorType>> ())) {
                            if (startingAddress <= oneBasedToZeroBased (i.fKey) and oneBasedToZeroBased (i.fKey) <= endInclusiveAddress) {
                                results[oneBasedToZeroBased (i.fKey) - startingAddress] = ToNetwork_ (i.fValue);
                            }
                        }
                        {
                            // Response ready - format, toNetwork, and write
                            uint8_t responseLen = static_cast<uint8_t> (quantity * sizeof (results[0])); // (responseLen in bytes) OK cuz validated in checkedReadHelperPayload2Shorts
                            MBAPHeaderIsh_ responseHeader =
                                MBAPHeaderIsh_{requestHeader.fTransactionID, requestHeader.fProtocolID,
                                               static_cast<uint16_t> (MBAPHeaderIsh_::kExtraLengthFromThisHeaderAccountedInPayloadLength +
                                                                      sizeof (responseLen) + responseLen),
                                               requestHeader.fUnitID, requestHeader.fFunctionCode};
                            out.Write (Memory::AsBytes (ToNetwork_ (responseHeader)));
                            out.Write (Memory::AsBytes (responseLen));
                            out.Write (span{reinterpret_cast<const byte*> (results.begin ()), responseLen});
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace ("Sent response: header={}, responseLen={}"_f, responseHeader, responseLen);
#endif
                        }
                    } break;
                    case FunctionCodeType_::kReadInputRegister_: {
                        /*
                         *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 16
                         */
                        uint16_t startingAddress = checkedReadHelperPayload2Shorts (requestPayload, 1, 0x7d).first;
                        uint16_t quantity        = checkedReadHelperPayload2Shorts (requestPayload, 1, 0x7d).second;
                        Assert (quantity >= 1);
                        uint16_t              endInclusiveAddress = startingAddress + quantity - 1u;
                        StackBuffer<uint16_t> results{quantity}; // for now - fill zeros for values not returned by backend
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("Processing kReadInputRegister_ (starting0Address: {}, quantity: {}) message with request-header={}"_f,
                                  startingAddress, quantity, requestHeader);
#endif
                        for (const auto& i : serviceHandler->ReadInputRegisters (
                                 DiscreteRange<uint16_t>{zeroToOneBased (startingAddress), zeroToOneBased (endInclusiveAddress)}
                                     .Elements ()
                                     .As<IModbusService::SetRegisterNames<InputRegisterDescriptorType>> ())) {
                            if (startingAddress <= oneBasedToZeroBased (i.fKey) and oneBasedToZeroBased (i.fKey) <= endInclusiveAddress) {
                                results[oneBasedToZeroBased (i.fKey) - startingAddress] = ToNetwork_ (i.fValue);
                            }
                        }
                        {
                            // Response ready - format, toNetwork, and write
                            uint8_t responseLen = static_cast<uint8_t> (quantity * sizeof (results[0])); // (responseLen in bytes) OK cuz validated in checkedReadHelperPayload2Shorts
                            MBAPHeaderIsh_ responseHeader =
                                MBAPHeaderIsh_{requestHeader.fTransactionID, requestHeader.fProtocolID,
                                               static_cast<uint16_t> (MBAPHeaderIsh_::kExtraLengthFromThisHeaderAccountedInPayloadLength +
                                                                      sizeof (responseLen) + responseLen),
                                               requestHeader.fUnitID, requestHeader.fFunctionCode};
                            out.Write (Memory::AsBytes (ToNetwork_ (responseHeader)));
                            out.Write (Memory::AsBytes (responseLen));
                            out.Write (span{reinterpret_cast<const byte*> (results.begin ()), responseLen});
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace ("Sent response: header={}, responseLen={}"_f, responseHeader, responseLen);
#endif
                        }
                    } break;
                    case FunctionCodeType_::kWriteSingleCoil_: {
                        /*
                         *  From http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf - page 17
                         */
                        uint16_t outputAddress = checkedReadHelperPayload2Shorts (requestPayload, 0, 0xff00).first;
                        uint16_t value         = checkedReadHelperPayload2Shorts (requestPayload, 0, 0xff00).second;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("Processing kWriteSingleCoil_ (outputAddress: {}, value: {}) message with request-header={}"_f,
                                  outputAddress, value, requestHeader);
#endif
                        serviceHandler->WriteCoils ({{zeroToOneBased (outputAddress), value == 0 ? false : true}});
                        {
                            // Response ready - format, toNetwork, and write
                            out.Write (Memory::AsBytes (requestHeader));
                            out.Write (Memory::AsBytes (ToNetwork_ (outputAddress)));
                            out.Write (Memory::AsBytes (ToNetwork_ (value)));
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace ("Sent response: header={}"_f, requestHeader);
#endif
                        }
                    } break;
                    default: {
                        DbgTrace ("UNREGONIZED FunctionCode (NYI probably) - {} - so echo ILLEGAL_FUNCTION code"_f, requestHeader.fFunctionCode);
                        if (options.fLogger) {
                            options.fLogger.value ()->Log (Logger::eWarning, "ModbusTCP unrecognized function code '{}'- rejected as ILLEGAL_FUNCTION"_f,
                                                           requestHeader.fFunctionCode);
                        }
                        MBAPHeaderIsh_ responseHeader = requestHeader;
                        responseHeader.fFunctionCode = static_cast<FunctionCodeType_> (responseHeader.fFunctionCode | 0x80); // set high bit
                        out.Write (Memory::AsBytes (ToNetwork_ (responseHeader)));
                        out.Write (Memory::AsBytes (static_cast<uint8_t> (ExceptionCode::ILLEGAL_FUNCTION)));
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("Sent UNREGONIZED_FUNCTION response: header={}, and exceptionCode={}"_f, responseHeader, exceptionCode);
#endif
                    }
                }
                out.Flush (); // since buffering output, be sure to flush after each response!
            }
        }
        catch (const Execution::Thread::AbortException&) {
            ReThrow (); // no warning needed
        }
        catch (...) {
            // Anytime we leave the loop due to an exception, thats worth a log note
            if (options.fLogger) {
                options.fLogger.value ()->Log (Logger::eWarning, "ModbusTCP connection ended abnormally: {}"_f, current_exception ());
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
Execution::Thread::Ptr Modbus::MakeModbusTCPServerThread (const shared_ptr<IModbusService>& serviceHandler, const ServerOptions& options)
{
    shared_ptr<Execution::ThreadPool> usingThreadPool = options.fThreadPool;
    if (usingThreadPool == nullptr) {
        usingThreadPool = make_shared<Execution::ThreadPool> (ThreadPool::Options{.fThreadCount = 1});
    }

    // Note - we return thread not started, so caller must explicitly start, but internal threads start immediately
    auto onModbusConnection = [serviceHandler, options, usingThreadPool] (const ConnectionOrientedStreamSocket::Ptr& s) {
        usingThreadPool->AddTask ([serviceHandler, options, s] () { ConnectionHandler_ (s, serviceHandler, options); });
    };
    return Thread::New (
        [onModbusConnection, options] () {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            TraceContextBumper ctx{"Modbus-Listener"};
#endif
            uint16_t usingPortNumber = options.fListenPort.value_or (502);
            if (options.fLogger) {
                options.fLogger.value ()->Log (Logger::eInfo, "Listening for ModbusTCP requests on port {}"_f, usingPortNumber);
            }
            WaitableEvent{}.Wait (); // forever (til thread abort)
        },
        "Modbus-Listener"_k);
}
