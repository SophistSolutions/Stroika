/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_ProcFSProcesses_h_
#define _Stroika_Framework_SystemPerformance_Instruments_ProcFSProcesses_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Execution/Process.h"

#include    "../Instrument.h"


/*
 * TODO:
 *      @todo
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {


                namespace ProcFSProcesses {


                    using   Foundation::Containers::Mapping;
                    using   Foundation::Execution::pid_t;


                    /*
                     *  Based closely on http://en.wikipedia.org/wiki/Procfs
                     */
                    struct  ProcessType {
                        String  fCommandLine;
                        String  fCurrentWorkingDirectory;
                        String  fEnvironmentVariables;
                        String  fEXEPath;
                        String  fRoot;  // chroot
                        String  fStatus;            /// @todo wrong - fix details of this
                        // could add subsqeunce of 'threads' - tasks
                    };


                    using ProcessMapType = Mapping<pid_t, ProcessType>;


                    extern  const   MeasurementType kProcessMapMeasurement;


                }


//tmphack to test
#define qSupport_SystemPerformance_Instruments_ProcFSProcesses  1

#ifndef qSupport_SystemPerformance_Instruments_ProcFSProcesses
#define qSupport_SystemPerformance_Instruments_ProcFSProcesses  qPlatform_POSIX
#endif


#if     qSupport_SystemPerformance_Instruments_ProcFSProcesses
                Instrument  GetProcFSProcesses ();
#endif


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_ProcFSProcesses_h_*/
