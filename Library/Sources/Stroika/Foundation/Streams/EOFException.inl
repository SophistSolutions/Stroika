/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
