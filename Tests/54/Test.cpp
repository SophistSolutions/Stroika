/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::Modbus
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <vector>

#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedMasterSocket.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedStreamSocket.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "Stroika/Frameworks/Modbus/IModbusService.h"
#include "Stroika/Frameworks/Modbus/Server.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Modbus;

#if qStroika_HasComponent_googletest
namespace {
    // Each value is a function of its (1-based) register or input number; coils start with 1, 3 and 10 on, and take writes
    struct TestService_ : IModbusService {
        Execution::Synchronized<RegisterValuesMapping<CoilsDescriptorType>> fCoils{
            RegisterValuesMapping<CoilsDescriptorType>{{1, true}, {3, true}, {10, true}}};

        virtual RegisterValuesMapping<CoilsDescriptorType> ReadCoils (const SetRegisterNames<CoilsDescriptorType>& names) override
        {
            RegisterValuesMapping<CoilsDescriptorType> result;
            auto                                       coils = fCoils.cget ();
            for (uint16_t n : names) {
                if (optional<bool> v = coils->Lookup (n)) {
                    result.Add (n, *v);
                }
            }
            return result;
        }
        virtual void WriteCoils (const RegisterValuesMapping<CoilsDescriptorType>& values) override
        {
            auto coils = fCoils.rwget ();
            for (const auto& kv : values) {
                coils->Add (kv.fKey, kv.fValue);
            }
        }
        virtual RegisterValuesMapping<DiscreteInputDescriptorType> ReadDiscreteInput (const SetRegisterNames<DiscreteInputDescriptorType>& names) override
        {
            RegisterValuesMapping<DiscreteInputDescriptorType> result;
            for (uint16_t n : names) {
                result.Add (n, n % 2 == 1); // the odd ones on
            }
            return result;
        }
        virtual RegisterValuesMapping<InputRegisterDescriptorType> ReadInputRegisters (const SetRegisterNames<InputRegisterDescriptorType>& names) override
        {
            RegisterValuesMapping<InputRegisterDescriptorType> result;
            for (uint16_t n : names) {
                result.Add (n, static_cast<uint16_t> (n * 10));
            }
            return result;
        }
        virtual RegisterValuesMapping<HoldingRegisterDescriptorType> ReadHoldingRegisters (const SetRegisterNames<HoldingRegisterDescriptorType>& names) override
        {
            RegisterValuesMapping<HoldingRegisterDescriptorType> result;
            for (uint16_t n : names) {
                result.Add (n, static_cast<uint16_t> (1000 + n));
            }
            return result;
        }
        virtual void WriteHoldingRegisters (const RegisterValuesMapping<HoldingRegisterDescriptorType>&) override
        {
        }
    };

    // a port free now, for the server to listen on
    uint16_t FreshPort_ ()
    {
        ConnectionOrientedMasterSocket::Ptr s = ConnectionOrientedMasterSocket::New (SocketAddress::INET, Socket::STREAM);
        s.Bind (SocketAddress{V4::kLocalhost, 0});
        return s.GetLocalAddress ()->GetPort ();
    }

    // the server's thread binds once started, so retry until it listens
    ConnectionOrientedStreamSocket::Ptr ConnectToServer_ (uint16_t port)
    {
        Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s;
        while (true) {
            try {
                ConnectionOrientedStreamSocket::Ptr s = ConnectionOrientedStreamSocket::New (SocketAddress::INET, Socket::STREAM);
                s.Connect (SocketAddress{V4::kLocalhost, port});
                return s;
            }
            catch (const system_error&) {
                if (Time::GetTickCount () > giveUpAt) {
                    throw;
                }
                Execution::Sleep (50ms);
            }
        }
    }

    // n bytes from s - fewer if 5 seconds pass first, so a reply shorter than expected fails the test rather than hangs it
    vector<uint8_t> Read_ (const ConnectionOrientedStreamSocket::Ptr& s, size_t n)
    {
        vector<uint8_t>                                                result;
        Time::TimePointSeconds                                         giveUpAt = Time::GetTickCount () + 5s;
        Execution::WaitForIOReady<ConnectionOrientedStreamSocket::Ptr> ready{s};
        while (result.size () < n and not ready.WaitQuietlyUntil (giveUpAt).empty ()) {
            byte       buf[512];
            span<byte> got = s.Read (span{buf}.first (min (n - result.size (), size (buf))));
            if (got.empty ()) {
                break; // the server closed the connection
            }
            for (byte b : got) {
                result.push_back (static_cast<uint8_t> (b));
            }
        }
        return result;
    }

    // One Modbus TCP exchange: send pdu (function code and data) after an MBAP header - transaction 0x1234, protocol 0, unit 7 -
    // check the reply's header echoes it, with the length of a replyPDUSize reply, and return the reply's pdu
    vector<uint8_t> Exchange_ (const ConnectionOrientedStreamSocket::Ptr& s, const vector<uint8_t>& pdu, size_t replyPDUSize)
    {
        auto mbapHeader = [] (size_t pduSize) {
            size_t length = pduSize + 1; // MBAP's length counts the unit id too
            return vector<uint8_t>{0x12, 0x34, 0, 0, static_cast<uint8_t> (length >> 8), static_cast<uint8_t> (length & 0xff), 7};
        };
        vector<uint8_t> request = mbapHeader (pdu.size ());
        request.insert (request.end (), pdu.begin (), pdu.end ());
        s.Write (as_bytes (span{request}));
        EXPECT_EQ (Read_ (s, 7), mbapHeader (replyPDUSize));
        return Read_ (s, replyPDUSize);
    }
}

namespace {
    // Each function code the server supports, then one it does not - all on one connection
    GTEST_TEST (Frameworks_Modbus, Server_)
    {
        Debug::TraceContextBumper     ctx{"Frameworks_Modbus::Server_"};
        auto                          service = Memory::MakeSharedPtr<TestService_> ();
        uint16_t                      port    = FreshPort_ ();
        Execution::Thread::CleanupPtr server{Execution::Thread::CleanupPtr::eAbortBeforeWaiting,
                                             MakeModbusTCPServerThread (service, ServerOptions{.fListenPort = port})};
        server.Start ();
        ConnectionOrientedStreamSocket::Ptr s = ConnectToServer_ (port);

        // Read Holding Registers (3): wire addresses 0-2 are registers 1-3
        EXPECT_EQ (Exchange_ (s, {3, 0, 0, 0, 3}, 8), (vector<uint8_t>{3, 6, 0x03, 0xE9, 0x03, 0xEA, 0x03, 0xEB}));
        // Read Input Registers (4): registers 10 and 11
        EXPECT_EQ (Exchange_ (s, {4, 0, 9, 0, 2}, 6), (vector<uint8_t>{4, 4, 0, 100, 0, 110}));
        // Read Coils (1): coils 1-10, of which 1, 3 and 10 (the last asked for) are on
        EXPECT_EQ (Exchange_ (s, {1, 0, 0, 0, 10}, 4), (vector<uint8_t>{1, 2, 0x05, 0x02}));
        // Read Discrete Inputs (2): inputs 1-8, the odd ones on
        EXPECT_EQ (Exchange_ (s, {2, 0, 0, 0, 8}, 3), (vector<uint8_t>{2, 1, 0x55}));
        // Write Single Coil (5): coil 5 on - the reply echoes the request
        EXPECT_EQ (Exchange_ (s, {5, 0, 4, 0xFF, 0}, 5), (vector<uint8_t>{5, 0, 4, 0xFF, 0}));
        EXPECT_EQ (service->fCoils.cget ()->Lookup (5), true);
        // Write Multiple Registers (16), not supported: exception ILLEGAL_FUNCTION
        EXPECT_EQ (Exchange_ (s, {16, 0, 0, 0, 1, 2, 0, 7}, 2), (vector<uint8_t>{16 | 0x80, 1}));
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
