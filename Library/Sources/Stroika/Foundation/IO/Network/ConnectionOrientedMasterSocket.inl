/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_inl_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_inl_ 1

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
                 ********** Foundation::IO::Network::ConnectionOrientedMasterSocket::Ptr ********
                 ********************************************************************************
                 */
                inline ConnectionOrientedMasterSocket::Ptr::Ptr (nullptr_t)
                    : inherited (shared_ptr<inherited::_IRep>{})
                {
                }
                inline ConnectionOrientedMasterSocket::Ptr::Ptr (const ConnectionOrientedMasterSocket& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionOrientedMasterSocket::Ptr::Ptr (const Ptr& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionOrientedMasterSocket::Ptr::Ptr (Ptr&& src)
                    : inherited (src._GetSharedRep ())
                {
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_inl_*/
