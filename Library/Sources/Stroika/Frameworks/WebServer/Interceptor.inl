/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Interceptor_inl_
#define _Stroika_Frameworks_WebServer_Interceptor_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Foundation/Debug/Assertions.h"

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ***************************** WebServer::Interceptor ***************************
     ********************************************************************************
     */
    inline Interceptor::Interceptor (const shared_ptr<_IRep>& rep)
        : fRep_{rep}
    {
        RequireNotNull (rep);
    }
    inline void Interceptor::HandleFault (Message* m, const exception_ptr& e) const noexcept
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*this};
        fRep_->HandleFault (m, e);
    }
    inline void Interceptor::HandleMessage (Message* m) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*this};
        fRep_->HandleMessage (m);
    }
    inline void Interceptor::CompleteNormally (Message* m) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*this};
        fRep_->CompleteNormally (m);
    }
    inline bool Interceptor::operator== (const Interceptor& rhs) const
    {
        return fRep_ == rhs.fRep_;
    }
    template <typename T>
    inline auto Interceptor::_GetRep () const -> const T&
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*this}; // not a fully safe check cuz returned value not checked during lifetime of this lock but pretty safe - and bug would be outside
        EnsureMember (fRep_.get (), T);
        return *dynamic_cast<const T*> (fRep_.get ());
    }

}
#endif /*_Stroika_Frameworks_WebServer_Interceptor_inl_*/
