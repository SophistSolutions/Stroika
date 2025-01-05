/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     **************************** Streams::EOFException *****************************
     ********************************************************************************
     */
    inline bool EOFException::GetPartialReadCompleted () const
    {
        return fPartialReadCompleted_;
    }

}
