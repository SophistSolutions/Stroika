/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution::Platform::Windows {

    /*
     ********************************************************************************
     ************************ CriticalSectionRecursiveMutex *************************
     ********************************************************************************
     */
    inline CriticalSectionRecursiveMutex::CriticalSectionRecursiveMutex ()
        : fCritSec_ ()
    {
        ::InitializeCriticalSection (&fCritSec_);
    }
    inline CriticalSectionRecursiveMutex::~CriticalSectionRecursiveMutex ()
    {
        ::DeleteCriticalSection (&fCritSec_);
    }
    inline void CriticalSectionRecursiveMutex::lock ()
    {
        ::EnterCriticalSection (&fCritSec_);
    }
    inline void CriticalSectionRecursiveMutex::unlock ()
    {
        ::LeaveCriticalSection (&fCritSec_);
    }

}
