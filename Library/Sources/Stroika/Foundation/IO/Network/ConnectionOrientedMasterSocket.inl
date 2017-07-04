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
                 ********** Foundation::IO::Network::ConnectionOrientedMasterSocket *************
                 ********************************************************************************
                 */
                inline shared_ptr<ConnectionOrientedMasterSocket::Ptr::_IRep> ConnectionOrientedMasterSocket::_GetSharedRep () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return dynamic_pointer_cast<ConnectionOrientedMasterSocket::_IRep> (inherited::_GetSharedRep ());
                }
                inline ConnectionOrientedMasterSocket::_IRep& ConnectionOrientedMasterSocket::_ref () const
                {
                    AssertMember (&inherited::_ref (), _IRep);
                    return *reinterpret_cast<_IRep*> (&inherited::_ref ());
                }
                inline const ConnectionOrientedMasterSocket::_IRep& ConnectionOrientedMasterSocket::_cref () const
                {
                    AssertMember (&inherited::_cref (), _IRep);
                    return *reinterpret_cast<const _IRep*> (&inherited::_cref ());
                }
                inline void ConnectionOrientedMasterSocket::Listen (unsigned int backlog) const
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().Listen (backlog);
                }
                inline ConnectionOrientedSocket::Ptr ConnectionOrientedMasterSocket::Accept () const
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    return _ref ().Accept ();
                }

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
                inline ConnectionOrientedMasterSocket::Ptr& ConnectionOrientedMasterSocket::Ptr::operator= (const Ptr& rhs)
                {
                    Socket::Ptr::operator= (rhs);
                    return *this;
                }
                inline ConnectionOrientedMasterSocket::Ptr& ConnectionOrientedMasterSocket::Ptr::operator= (Ptr&& rhs)
                {
                    Socket::Ptr::operator= (move (rhs));
                    return *this;
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_inl_*/
