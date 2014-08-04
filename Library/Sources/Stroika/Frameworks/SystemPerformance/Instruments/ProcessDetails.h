/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_ProcessDetails_h_
#define _Stroika_Framework_SystemPerformance_Instruments_ProcessDetails_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../../Foundation/Execution/Process.h"
#include    "../../../Foundation/Memory/Optional.h"

#include    "../Instrument.h"


/*
 * TODO:
 *      @todo   Eventually allow configuration of whether to use procfs or ps process grep, but default to
 *              procfs is available.
 *
 *      @todo   Do Windoze impl using windows APIs for process enumeration etc
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {
                namespace ProcessDetails {


                    using   Foundation::Containers::Mapping;
                    using   Foundation::DataExchange::ObjectVariantMapper;
                    using   Foundation::Execution::pid_t;
                    using   Foundation::Memory::Optional;


                    /*
                     *  Based closely on http://en.wikipedia.org/wiki/Procfs
                     */
                    struct  ProcessType {
                        Optional<String>  fCommandLine;
                        Optional<String>  fCurrentWorkingDirectory;
                        Optional<Mapping<String, String>>  fEnvironmentVariables;
                        Optional<String>  fEXEPath;
                        Optional<String>  fRoot;  // chroot
                        enum class RunStatus {
                            eRun,
                            eSuspended,
                            eIdle,
                            // See linux docs - these above are guesses
                        };
                        Time::DurationSecondsType   fTotalTimeUsed = 0;     // ps time field - in seconds - combines system and user time
                        //String  fStatus;            /// @todo wrong - fix details of this
                        // could add subsqeunce of 'threads' - tasks
                    };


                    enum    class   Fields2Capture {
                        eCommandLine,
                        /// many more TBD
                    };

                    using ProcessMapType = Mapping<pid_t, ProcessType>;


                    extern  const   MeasurementType kProcessMapMeasurement;


                    /**
                     *  For ProcessType and ProcessMapType types.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    enum class CachePolicy {
                        eOmitUnchangedValues,
                        eIncludeAllRequestedValues,
                    };


                    /**
                     *  Instrument returning ProcessMapType measurements.
                     */
                    Instrument          GetInstrument (
                        const Optional<Set<Fields2Capture>>& onlyCaptureFields = Optional<Set<Fields2Capture>> (),
                        const Optional<Set<pid_t>>& restrictToPIDs = Optional<Set<pid_t>> (),
                        const Optional<Set<pid_t>>& omitPIDs = Optional<Set<pid_t>> (),
                        CachePolicy cachePolicy = CachePolicy::eIncludeAllRequestedValues
                    );


                }

            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_ProcessDetails_h_*/
