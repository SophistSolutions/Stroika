/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"

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
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock{fThisAssertExternallySynchronized_};
        fRep_->HandleFault (m, e);
    }
    inline void Interceptor::HandleMessage (Message* m) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock{fThisAssertExternallySynchronized_};
        fRep_->HandleMessage (m);
    }
    inline void Interceptor::CompleteNormally (Message* m) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock{fThisAssertExternallySynchronized_};
        fRep_->CompleteNormally (m);
    }
    inline bool Interceptor::operator== (const Interceptor& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock{fThisAssertExternallySynchronized_};
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock2{rhs.fThisAssertExternallySynchronized_};
        return fRep_ == rhs.fRep_;
    }
    template <typename T>
    inline auto Interceptor::_GetRep () const -> const T&
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock{fThisAssertExternallySynchronized_}; // not a fully sufficient check cuz returned value not checked during lifetime of this lock but pretty safe - and bug would be outside
        EnsureMember (fRep_.get (), T);
        return *dynamic_cast<const T*> (fRep_.get ());
    }

}
