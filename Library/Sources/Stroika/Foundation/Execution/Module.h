/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Module_h_
#define _Stroika_Foundation_Execution_Module_h_ 1

#include "../StroikaPreComp.h"

#if qPlatform_POSIX
#include <unistd.h>
#endif

#include "../Characters/String.h"
#include "../Configuration/Common.h"

#if !defined(qHas_pid_t)
#error "qHas_pid_t must  be defined in StroikaConfig.h"
#endif

namespace Stroika::Foundation::Execution {

    /// TODO - maybe move this to configuraiotn module???

#if qHas_pid_t
    using pid_t = ::pid_t;
#else
#if qPlatform_Windows
    using pid_t = DWORD;
#else
    using pid_t = int;
#endif
#endif

    using Characters::SDKString;
    using Characters::String;

    /**
     *  The directory where the executable that is running this code is located. If this code is compiled into a DLL,
     *  this returns the executable directory for the underlying process/executable (not the DLL/so file).
     */
    String GetEXEDir ();

    /**
     *  The path where the executable that is running this code is located. If this code is compiled into a DLL,
     *  this returns the executable for the underlying process/executable (not the DLL/so file).
     */
    String GetEXEPath ();

    /**
     *  @see GetEXEDir
     *
     *  The variant returning TString is useful especially when you need to avoid other Stroika
     *  dependencies, such as low level coding and avoiding deadly embraces with tracelog code.
     */
    SDKString GetEXEDirT ();

    /**
     *  @see GetEXEPath
     *
     *  The variant returning SDKString is useful especially when you need to avoid other Stroika
     *  dependencies, such as low level coding and avoiding deadly embraces with tracelog code.
     */
    SDKString GetEXEPathT ();

    /**
     *  Return the full path to the given process
     */
    String GetEXEPath (pid_t processID);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Execution_Module_h_*/
