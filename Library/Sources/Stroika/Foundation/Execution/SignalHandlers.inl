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
        , fDirectCall_{signalHandler}
    {
        Require (signalHandler == SIG_IGN or type == Type::eSafe); // otherwise use the overload taking a noexcept function
    }
    inline SignalHandler::SignalHandler (void (*signalHandler) (SignalID) noexcept, Type type)
        : fType_{type}
        , fCall_{signalHandler}
        , fDirectCall_{signalHandler}
    {
    }
    inline SignalHandler::SignalHandler (const function<void (SignalID)>& signalHandler)
        : fType_{Type::eSafe}
        , fCall_{signalHandler}
    {
    }
    inline SignalHandler::SignalHandler (const function<void (SignalID)>& signalHandler, Type type)
        : fType_{type}
        , fCall_{signalHandler}
    {
        Require (type == Type::eSafe); // a direct handler is a noexcept function pointer - see the constructors' docs
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

namespace Stroika::Foundation::Execution {
    Common::StroikaNaming EnumNamingRule (SignalHandler::Type);
}
