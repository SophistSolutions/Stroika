/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaTypeNotSupportedException_h_
#define _Stroika_Foundation_DataExchange_InternetMediaTypeNotSupportedException_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Execution/Exceptions.h"

namespace Stroika::Foundation::DataExchange {

    /**
     * Use when reading from a structured stream the data is ill-formed
     */
    class InternetMediaTypeNotSupportedException : public Execution::RuntimeErrorException<> {
    private:
        using inherited = Execution::RuntimeErrorException<>;

    public:
        /**
         */
        InternetMediaTypeNotSupportedException ();
        InternetMediaTypeNotSupportedException (const InternetMediaType& mediaType);

    public:
        /**
         */
        static const InternetMediaTypeNotSupportedException kThe;
    };
    inline const InternetMediaTypeNotSupportedException InternetMediaTypeNotSupportedException::kThe;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetMediaTypeNotSupportedException.inl"

#endif /*_Stroika_Foundation_DataExchange_InternetMediaTypeNotSupportedException_h_*/
