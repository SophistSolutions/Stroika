/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
#endif  /*_Stroika_Foundation_IO_Network_Interface_inl_*/
