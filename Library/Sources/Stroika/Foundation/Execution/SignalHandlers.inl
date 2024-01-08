/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SignalHandlers_inl_
#define _Stroika_Foundation_Execution_SignalHandlers_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"

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
    inline SignalHandler::SignalHandler (const Function<void (SignalID)>& signalHandler, Type type)
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

}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<Execution::SignalHandler::Type> DefaultNames<Execution::SignalHandler::Type>::k{{{
        {Execution::SignalHandler::Type::eDirect, L"Direct"},
        {Execution::SignalHandler::Type::eSafe, L"Safe"},
    }}};
}

#endif /*_Stroika_Foundation_Execution_SignalHandlers_inl_*/
