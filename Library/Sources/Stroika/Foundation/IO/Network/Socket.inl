/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ********************* Foundation::IO::Network::Socket::Ptr *********************
     ********************************************************************************
     */
    inline Socket::Ptr::Ptr (nullptr_t)
    {
    }
    inline Socket::Ptr::Ptr (const shared_ptr<_IRep>& rep)
        : fRep_{rep}
    {
    }
    inline Socket::Ptr::Ptr (shared_ptr<_IRep>&& rep)
        : fRep_{move (rep)}
    {
    }
    inline Socket::Ptr& Socket::Ptr::operator= (Ptr&& s) noexcept
    {
        fRep_ = move (s.fRep_);
        return *this;
    }
    inline Socket::Ptr& Socket::Ptr::operator= (const Ptr& s)
    {
        fRep_ = s.fRep_;
        return *this;
    }
    inline void Socket::Ptr::reset () noexcept
    {
        fRep_.reset ();
    }
    inline shared_ptr<Socket::_IRep> Socket::Ptr::_GetSharedRep () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        return fRep_;
    }
    inline Socket::_IRep& Socket::Ptr::_ref () const
    {
        RequireNotNull (fRep_);
        return *fRep_;
    }
    inline const Socket::_IRep& Socket::Ptr::_cref () const
    {
        RequireNotNull (fRep_);
        return *fRep_;
    }
    inline Socket::PlatformNativeHandle Socket::Ptr::GetNativeSocket () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        return _cref ().GetNativeSocket ();
    }
    inline optional<IO::Network::SocketAddress> Socket::Ptr::GetLocalAddress () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        return _cref ().GetLocalAddress ();
    }
    inline SocketAddress::FamilyType Socket::Ptr::GetAddressFamily () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        return _cref ().GetAddressFamily ();
    }
    inline void Socket::Ptr::Shutdown (ShutdownTarget shutdownTarget)
    {
        // not important to null-out, but may as well...
        if (fRep_ != nullptr) {
            _ref ().Shutdown (shutdownTarget);
        }
    }
    inline void Socket::Ptr::Close () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        // not important to null-out, but may as well...
        if (fRep_ != nullptr) {
            fRep_->Close ();
        }
    }
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Socket::Ptr::getsockopt (int level, int optname) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        RESULT_TYPE                                           r{};
        socklen_t                                             roptlen = sizeof (r);
        _cref ().getsockopt (level, optname, &r, &roptlen);
        return r;
    }
    template <typename ARG_TYPE>
    inline void Socket::Ptr::setsockopt (int level, int optname, ARG_TYPE arg) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        socklen_t                                             optvallen = sizeof (arg);
        _ref ().setsockopt (level, optname, &arg, optvallen);
    }
    inline bool Socket::Ptr::operator== (const Ptr& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized}; // nb: not deadlock risk cuz these aren't really mutexes, just checks
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext2{rhs._fThisAssertExternallySynchronized};
        return _GetSharedRep () == rhs._GetSharedRep ();
    }
    inline strong_ordering Socket::Ptr::operator<=> (const Ptr& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized}; // nb: not deadlock risk cuz these aren't really mutexes, just checks
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext2{rhs._fThisAssertExternallySynchronized};
#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy or qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
        return Common::compare_three_way_BWA{}(_GetSharedRep (), rhs._GetSharedRep ());
#else
        return _GetSharedRep () <=> rhs._GetSharedRep ();
#endif
    }

#if qPlatform_Windows
    /*
     ********************************************************************************
     ******************* ThrowWSASystemErrorIfNegative ******************************
     ********************************************************************************
     */
    template <typename INT_TYPE>
    inline INT_TYPE ThrowWSASystemErrorIfSOCKET_ERROR (INT_TYPE returnCode)
        requires (is_signed_v<INT_TYPE>)
    {
        if (returnCode == SOCKET_ERROR) {
            Execution::ThrowSystemErrNo (::WSAGetLastError ());
        }
        return returnCode;
    }
    // this overload is needed because the winsock type for SOCKET is UNSIGNED so < 0 test doesn't work
    inline IO::Network::Socket::PlatformNativeHandle ThrowWSASystemErrorIfSOCKET_ERROR (IO::Network::Socket::PlatformNativeHandle returnCode)
    {
        // see docs for https://docs.microsoft.com/en-us/windows/desktop/api/winsock2/nf-winsock2-accept
        if (returnCode == INVALID_SOCKET) {
            Execution::ThrowSystemErrNo (::WSAGetLastError ());
        }
        return returnCode;
    }
#endif

}
