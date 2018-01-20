/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Interface_inl_
#define _Stroika_Foundation_IO_Network_Interface_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

/*
                 ********************************************************************************
                 ************************************* Interface ********************************
                 ********************************************************************************
                 */
#if qPlatform_POSIX
                /**
                 *  On unix, its the interface name, e.g. eth0, eth1, etc.
                 *  On Windows, this is concept doesn't really exist.
                 */
                inline String Interface::GetInterfaceName () const
                {
                    return fInternalInterfaceID;
                }
#endif
            }
        }
    }
}
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            template <>
            struct DefaultNames<IO::Network::Interface::Status> : EnumNames<IO::Network::Interface::Status> {
                static constexpr EnumNames<IO::Network::Interface::Status> k{
                    EnumNames<IO::Network::Interface::Status>::BasicArrayInitializer{
                        {
                            {IO::Network::Interface::Status::eConnected, L"Connected"},
                            {IO::Network::Interface::Status::eRunning, L"Running"},
                        }}};
                DefaultNames ()
                    : EnumNames<IO::Network::Interface::Status> (k)
                {
                }
            };
            template <>
            struct DefaultNames<IO::Network::Interface::Type> : EnumNames<IO::Network::Interface::Type> {
                static constexpr EnumNames<IO::Network::Interface::Type> k{
                    EnumNames<IO::Network::Interface::Type>::BasicArrayInitializer{
                        {
                            {IO::Network::Interface::Type::eLoopback, L"Loopback"},
                            {IO::Network::Interface::Type::eWiredEthernet, L"WiredEthernet"},
                            {IO::Network::Interface::Type::eWIFI, L"WIFI"},
                            {IO::Network::Interface::Type::eTunnel, L"Tunnel"},
                            {IO::Network::Interface::Type::eOther, L"Other"},
                        }}};
                DefaultNames ()
                    : EnumNames<IO::Network::Interface::Type> (k)
                {
                }
            };
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_Interface_inl_*/
