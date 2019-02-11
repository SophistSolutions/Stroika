/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_ErrNoException_h_
#define _Stroia_Foundation_Execution_ErrNoException_h_ 1

#include "../StroikaPreComp.h"

#include <cerrno>
#include <exception>

#include "Exceptions.h"

//_DeprecatedFile_ ("DEPRECATED in v2.1d18 - #include Exceptions.h instead of ErrNoException.h");

namespace Stroika::Foundation::Execution {

    using Characters::SDKString;

    /**
     */
    class [[deprecated ("Since v2.1d18, use Execution::ThrowPOSIXErrNo")]] errno_ErrorException : public Execution::Exception<>
    {
    public:
        explicit errno_ErrorException (errno_t e);

    public:
        operator errno_t () const;

    public:
        static SDKString LookupMessage (errno_t e);
        nonvirtual SDKString LookupMessage () const;

    public:
        // throw errno_ErrorException () - unless I can find another Win32Exception, or bad_alloc() or some such which is
        // as good a fit.
        [[noreturn]] static void Throw (errno_t error);

    private:
        errno_t fError{0};
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ErrNoException.inl"

#endif /*_Stroia_Foundation_Execution_ErrNoException_h_*/
