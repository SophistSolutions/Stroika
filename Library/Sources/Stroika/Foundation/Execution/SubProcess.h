/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SubProcess_h_
#define _Stroika_Foundation_Execution_SubProcess_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Characters/String.h"

#include    "Exceptions.h"


/*
 *  TODO:
 *      o   Consider rename this module to 'RunExternalProcess'
 *          The term subprocess COULD mean other things. Its shorter, but that matters little unless you use it alot.
 *          MAYBE "ExternalProcessRunner". Or "SubProcessRunner".
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            // IDEA HERE IS FROM KDJ - Do something like python/perl stuff for managing subprocesses easily.

            // Look input stream, output stream(or streams - stdout/stderr) - and some kind of extenral process control
            // so can say WIAT or Termiante.
            //
            // Simple portable wrapper.
            //
            // Could use simple singly threaded approach used in TypeNValue ReportDefinition::RunExternalProcess_ (const TString& cmdLine, const TString& currentDir, const BLOBs::BLOB& stdinBLOB, const ContentType& resultFormat, float timeout)
            // except that code has the defect that when the input pipe is full, and there is nothing in the output piples
            // it busy waits. We COULD fix this by doing a select.
            //
            // OR - as KDJ suggests - create 3 threads - one that just reads on stdout, one that just reads on stderr, and one that
            // spits into stdin.
            //
            // The caller of 'subprocess' then would just wait on each of the 3 subprocesses (or would implement the aforementioned
            // looping over reads/writes/selects etc).
            //


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SubProcess_h_*/
