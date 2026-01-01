/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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
