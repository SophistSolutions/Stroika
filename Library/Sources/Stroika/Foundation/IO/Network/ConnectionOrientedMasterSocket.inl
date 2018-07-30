/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_inl_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ********** Foundation::IO::Network::ConnectionOrientedMasterSocket::Ptr ********
     ********************************************************************************
     */
    inline ConnectionOrientedMasterSocket::Ptr::Ptr (nullptr_t)
        : inherited (nullptr)
    {
    }
    inline ConnectionOrientedMasterSocket::Ptr::Ptr (shared_ptr<_IRep>&& rep)
        : inherited (move (rep))
    {
    }
    inline ConnectionOrientedMasterSocket::Ptr::Ptr (const shared_ptr<_IRep>& rep)
        : inherited (rep)
    {
    }
    inline void ConnectionOrientedMasterSocket::Ptr::Listen (unsigned int backlog) const
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        _ref ().Listen (backlog);
    }
    inline ConnectionOrientedStreamSocket::Ptr ConnectionOrientedMasterSocket::Ptr::Accept () const
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return _ref ().Accept ();
    }
    inline shared_ptr<ConnectionOrientedMasterSocket::_IRep> ConnectionOrientedMasterSocket::Ptr::_GetSharedRep () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return dynamic_pointer_cast<ConnectionOrientedMasterSocket::_IRep> (inherited::_GetSharedRep ());
    }
    inline ConnectionOrientedMasterSocket::_IRep& ConnectionOrientedMasterSocket::Ptr::_ref () const
    {
        AssertMember (&inherited::_ref (), _IRep);
        return *reinterpret_cast<_IRep*> (&inherited::_ref ());
    }
    inline const ConnectionOrientedMasterSocket::_IRep& ConnectionOrientedMasterSocket::Ptr::_cref () const
    {
        AssertMember (&inherited::_cref (), _IRep);
        return *reinterpret_cast<const _IRep*> (&inherited::_cref ());
    }

}

#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_inl_*/
