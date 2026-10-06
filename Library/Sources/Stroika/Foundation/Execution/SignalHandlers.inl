/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************** SignalHandler *********************************
     ********************************************************************************
     */
    inline SignalHandler::SignalHandler (void (*signalHandler) (SignalID), Type type)
        : fType_{type}
        , fCall_{signalHandler}
    {
        Require (signalHandler == SIG_IGN or type == Type::eSafe); // otherwise use the overload taking a noexcept function
    }
    inline SignalHandler::SignalHandler (void (*signalHandler) (SignalID) noexcept, Type type)
        : fType_{type}
        , fCall_{signalHandler}
    {
    }
    inline SignalHandler::SignalHandler (const function<void (SignalID)>& signalHandler, Type type)
        : fType_{type}
        , fCall_{signalHandler}
    {
        // @todo - would LIKE to overload this CTOR like we do for regular function pointer, noexcept and noexcept(false), but
        // that doesn't appear to work wtih std::function as of C++17 (reference?) -- DISABLE this Require()
        // Require (type == Type::eSafe); // otherwise use the overload taking a noexcept function
    }
    inline SignalHandler::Type SignalHandler::GetType () const
    {
        return fType_;
    }
    inline void SignalHandler::operator() (SignalID i) const
    {
        fCall_ (i);
    }
    inline strong_ordering SignalHandler::operator<=> (const SignalHandler& rhs) const
    {
        if (strong_ordering cmp = fType_ <=> rhs.fType_; cmp != 0) {
            return cmp;
        }
        return fIdentity_ <=> rhs.fIdentity_;
    }
    inline bool SignalHandler::operator== (const SignalHandler& rhs) const
    {
        return fType_ == rhs.fType_ and fIdentity_ == rhs.fIdentity_;
    }

}

namespace Stroika::Foundation::Common {
    template <>
    constexpr EnumNames<Execution::SignalHandler::Type> DefaultNames<Execution::SignalHandler::Type>::k{{{
        {Execution::SignalHandler::Type::eDirect, L"Direct"},
        {Execution::SignalHandler::Type::eSafe, L"Safe"},
    }}};
}
