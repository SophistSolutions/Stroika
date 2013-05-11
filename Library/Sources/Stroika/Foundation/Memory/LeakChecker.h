/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_LeakChecker_h_
#define _Stroika_Foundation_Memory_LeakChecker_h_ 1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"



/**
 * TODO:
 *
 *      @todo   This works poorly, and so far just on windoze. Consider losing.
 *
 *      @todo   See if Valgrind works well tracking leaks. If so - even on windows - see about just losing
 *              DEBUG_NEW code! (maybe http://code.google.com/p/drmemory/
 *              - or http://www.codersnotes.com/sleepy
 *
 *              SERIOUSLY consider losing DEBUG_NEW anyhow - because it really interacts badly with overloaded
 *              operator new - such as with block allocation.
 */



/*
@CONFIGVAR:     qMSVisualStudioCRTMemoryDebug
@DESCRIPTION:   <p>The qMSVisualStudioCRTMemoryDebug macro is '1' if Stroika is compiled using the Microsoft Visual Studio crtdbg facility. If so - many
            places in Stroika will use a placement operator new - which gives the crtdbg code line number info about where something was allocated.
            This also enables some additional error checking in the C runtime memory allocator. But it has significant runtime overhead.</p>
            <p>
                This used to be #define qMSVisualStudioCRTMemoryDebug   qDebug && qPlatform_Windows, but for reasons I don't fully understand, it appears
                to interfere with MFC (at least LedIt/MFC).

            </p>
*/
#if     !defined (qMSVisualStudioCRTMemoryDebug)
#error  "qMSVisualStudioCRTMemoryDebug should normally be defined indirectly by StroikaConfig.h"
#endif




/**
 * Support for using the MSVC CRT memleak detector. If not available, then DBG_NEW simply expands
 * to 'new' so it can be safely used in places where the global new operator would have been used.
 *      -- LGP 2009-05-25
 *
 *  If DEBUG_NEW is already defined (eg because of import of <afx.h> - MFC) - don't redefine, but just allow
 *  use of MFC version.
 */
#if      !defined (DEBUG_NEW)
#if     qMSVisualStudioCRTMemoryDebug
#define     _CRTDBG_MAP_ALLOC
#include    <crtdbg.h>
#define DEBUG_NEW   new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else   /*qMSVisualStudioCRTMemoryDebug*/
#define DEBUG_NEW    new
#endif  /*qMSVisualStudioCRTMemoryDebug*/
#endif  /*!defined (DEBUG_NEW)*/



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {
            namespace   LeakChecker {


                // Note - this functionality is just for debugging, and may be disabled inside the Memory leakchecker module, in which
                // case it will link but do nothing...
                void    ForceCheckpoint ();
                void    DumpLeaksSinceLastCheckpoint ();


            }
        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "LeakChecker.inl"

#endif  /*_Stroika_Foundation_Memory_LeakChecker_h_*/
