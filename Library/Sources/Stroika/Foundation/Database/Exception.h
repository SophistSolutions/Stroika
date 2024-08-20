/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Exception_h_
#define _Stroika_Foundation_Database_Exception_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Execution/Exceptions.h"

/*
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Database {

    using namespace std;
    using namespace Stroika::Foundation;

    using Characters::String;

    /**
     *  An exception reported by the (typically remote) database.
     */
    class Exception : public Execution::RuntimeErrorException<> {
    private:
        using inherited = Execution::RuntimeErrorException<>;

    public:
        Exception (const String& message);
    };

    /**
     *  The database unexpectedly reported no data for the request.
     */
    class NoDataException : public Exception {
    public:
        NoDataException ();
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Database_Exception_h_*/
