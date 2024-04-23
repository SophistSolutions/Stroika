/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Exception_h_
#define _Stroika_Foundation_IO_Network_HTTP_Exception_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/IO/Network/HTTP/Status.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Foundation::Characters::String;

    /**
     *  See http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1.1
     *
     *  @see ClientErrorException
     */
    class Exception : public Execution::RuntimeErrorException<> {
    public:
        // If no reason is given, a default is generated based on the status code
        Exception (Status status, const String& reason = {});

    public:
        // throw Exception () type iff the status indicates a real HTTP exception code (i.e. ignore 2xx values etc)
        static void ThrowIfError (Status status, const String& reason = {});
        static void ThrowIfError (const String& status, const String& reason = {});

        static bool IsHTTPStatusOK (Status status);

    public:
        /*
         * Stroika contains a table mapping some HTTP status values to English text. This can be used to access
         * that mapping. Its thin - but it contains most of the biggies. If forceAlwaysFound is true, it will
         * always generate some response (sprintf the # with some verbiage) and if false it will return an empty string
         * for Status values not in our table.
         */
        static String GetStandardTextForStatus (Status s, bool forceAlwaysFound = false);

    public:
        nonvirtual Status GetStatus () const;

    public:
        /**
         * Returns true if Status code indicates a 4xx error
         */
        nonvirtual bool IsClientError () const;

    public:
        /**
         * Returns true if Status code indicates a 5xx error
         */
        nonvirtual bool IsServerError () const;

    public:
        // If this was constructed with a specific reason - it will be returned here. Otherwise an autogenerated one based
        // on the status will be returned.
        //
        // This is effectively a human readable, and perhaps more detailed - explanation of the exception
        nonvirtual String GetReason () const;

    private:
        Status fStatus_;
        String fReason_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exception.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Exception_h_*/
