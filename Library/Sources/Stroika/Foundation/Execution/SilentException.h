/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_SilentException_h_
#define _Stroia_Foundation_Execution_SilentException_h_ 1

#include "../StroikaPreComp.h"

#include <exception>

#include "Exceptions.h"

namespace Stroika::Foundation::Execution {

    /**
     * Throw this when an error has already been reported - so that it isn't reported again
     */
    class SilentException : public exception {
    public:
        /**
         *  \note not inline because in the SilentException class, this type is incomplete (sad - as of C++17 limitation).
         */
        static const SilentException kThe;

    public:
        /**
         *  Provide a 'c string' variant of the exception message. Convert the UNICODE
         *  string argument to a narrow-string (multibyte) in the SDK code page.
         *  @see GetDefaultSDKCodePage()
         */
        virtual const char* what () const noexcept override;
    };

}

#endif /*_Stroia_Foundation_Execution_SilentException_h_*/
