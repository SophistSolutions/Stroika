/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Interface_inl_
#define _Stroika_Foundation_IO_Network_Interface_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                /*
                 ********************************************************************************
                 ************************************* Interface ********************************
                 ********************************************************************************
                 */
#if     qPlatform_POSIX
                /**
                 *  On unix, its the interface name, e.g. eth0, eth1, etc.
                 *  On Windows, this is concept doesn't really exist.
                 */
                inline  String      Interface::GetInterfaceName () const
                {
                    return fInternalInterfaceID;
                }
#endif


            }
        }
    }
}

namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {
#if     qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy
            template    <>
            const EnumNames<IO::Network::Interface::Status> DefaultNames<IO::Network::Interface::Status>::k = EnumNames<IO::Network::Interface::Status>::BasicArrayInitializer  {
                {
                    { IO::Network::Interface::Status::eConnected, L"Connected" },
                    { IO::Network::Interface::Status::eRunning, L"Running" },
                }
            };
            template    <>
            const EnumNames<IO::Network::Interface::Type>   DefaultNames<IO::Network::Interface::Type>::k = EnumNames<IO::Network::Interface::Type>::BasicArrayInitializer  {
                {
                    { IO::Network::Interface::Type::eLoopback, L"Loopback" },
                    { IO::Network::Interface::Type::eWiredEthernet, L"WiredEthernet" },
                    { IO::Network::Interface::Type::eWIFI, L"WIFI" },
                    { IO::Network::Interface::Type::eTunnel, L"Tunnel" },
                    { IO::Network::Interface::Type::eOther, L"Other" },
                }
            };
#else
            template    <>
            constexpr   EnumNames<IO::Network::Interface::Status> DefaultNames<IO::Network::Interface::Status>::k {
                EnumNames<IO::Network::Interface::Status>::BasicArrayInitializer  {
                    {
                        { IO::Network::Interface::Status::eConnected, L"Connected" },
                        { IO::Network::Interface::Status::eRunning, L"Running" },
                    }
                }
            };
            template    <>
            constexpr   EnumNames<IO::Network::Interface::Type>   DefaultNames<IO::Network::Interface::Type>::k {
                EnumNames<IO::Network::Interface::Type>::BasicArrayInitializer  {
                    {
                        { IO::Network::Interface::Type::eLoopback, L"Loopback" },
                        { IO::Network::Interface::Type::eWiredEthernet, L"WiredEthernet" },
                        { IO::Network::Interface::Type::eWIFI, L"WIFI" },
                        { IO::Network::Interface::Type::eTunnel, L"Tunnel" },
                        { IO::Network::Interface::Type::eOther, L"Other" },
                    }
                }
            };
#endif
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_Interface_inl_*/
