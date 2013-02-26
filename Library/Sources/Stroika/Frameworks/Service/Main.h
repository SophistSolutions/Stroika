/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Service_Main_h_
#define _Stroika_Frameworks_Service_Main_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>
#include    <vector>

#if     qPlatform_POSIX
#include    <csignal>
#endif

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"


/*
 * TODO:
 *      (o) For UNIX
 *
 *          (o) Store in file system file with current PID.
 *
 *          (o) Start/Stop work by sending SIGNALS
 *
 *          (o) Must have generic signal handler registered (at least for TERMINATE/STOP/CONTINUE, SIGNIT (re-read conf))
 *
 *      (o) Windoze implementation - supproting the richer set of control mechanism.
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   Service {

            using   namespace   Stroika::Foundation;
            using   Characters::String;

            // very very rough draft -
            class   Main {
            public:
                struct  ServiceDescription {
                    String  fName;
                };
            public:
                /*
                 * To use this class you must implement your own Rep (to represent the running service).
                 *
                 *  MainLoop () is automatically setup to run on its own thread. Betware, the OnXXX events maybe called on this object, but from any thread
                 *  so be careful of thread safety!
                 */
                class   IRep {
                public:
                    IRep ();
                    virtual ~IRep ();

                public:
                    // This should be overridden by each service, and should  never return until the service is done (stop request).
                    virtual void                MainLoop () = 0;

                public:
                    virtual void                OnStartRequest ();
                    virtual void                OnStopRequest ();
                    virtual void                OnReReadConfigurationRequest ();    //NOT USED NOW - UNCLEAR IF/HOW WE WANT TODO THIS -- LGP 2011-09-24

                    //  returns a readable string about the service status. Note most of this is done by the envelope class, and this is just a way to add
                    //  service specific extras
                    virtual String              GetServiceStatusMessage () const;
                    virtual ServiceDescription  GetServiceDescription () const = 0;

#if     qPlatform_POSIX
                public:
                    virtual String              GetPIDFileName () const;
#endif

#if     qPlatform_POSIX
                public:
                    virtual void                SignalHandler (int signum);
#endif


                    // MUST REDO THIS STUFF WITH EVENTS - when we have POSIX complaint event support in Stroika Foundation
                protected:
                    nonvirtual  bool    _CheckShouldReReadConfig () const;
                    nonvirtual  void    _DidReReadConfig ();
                private:
                    bool    fMustReReadConfig;

                protected:
                    // Called periodically in subclasses of MainLoop to abort processing when the service is being shut down. Triggers a
                    // ThreadAborted exception when its time...
                    nonvirtual  void    _CheckAndAbortThread () const;

                private:
                    bool    fStopping_; // set to true externally (from other thread) and MainLoop should terminate itself cleanly

                };
            public:
                /*
                 * Note - besides the obvios, the Main () function also sets signal handlers to point to this objects signal handler.
                 */
                Main (shared_ptr<IRep> rep);

            protected:
                static  shared_ptr<IRep> _sRep;


#if     qPlatform_POSIX
            private:
                nonvirtual  void    SetupSignalHanlders_ ();
#endif


#if     qPlatform_POSIX
                /*
                 * By default, ServiceMain sets up its own signal handlers for
                 *
                 *      SIGTERM
                 *      SIGHUP
                 *      ....
                 *<<should  add more - like TSTP and CONT - but not high priorities since the default UNIX behavior of these is pretty reasonable
                 *>>>--LGP 2011-09-24
                 *
                 *  If the user of this class needs there own signal handlers, but still wnats to leverage the default handling in this
                 *  class, there are two easy ways:
                 *      (1)     overide the 'rep' method Signalhandler and delegate t your own handlers.
                 *      (2)     or, replace the signal hanlder yourself (with the signal system call), and call
                 *              SignalHandler () directly on this class.
                 */
            public:
#if     qCompilerAndStdLib_Supports_constexpr
                static  constexpr   int kSIG_ReReadConfiguration    =   SIGHUP;
#else
                static  const   int kSIG_ReReadConfiguration    =   SIGHUP;
#endif
            public:
                static  void    SignalHandler (int signum);
#endif

            public:
                /*
                 *  Checks the state of the given service. NOTE - this works ACROSS PROCESSES. It can be called to ask in a controller exe
                 *  if the serviceMain EXE is running. It also - COULD give the wrong answer - given races, so use with care.
                 */
                enum  class State : uint8_t {
                    eStopped,
                    eRunning,
                    ePaused,        // STOPPED in unix
                };
                nonvirtual  State               GetState () const;

#if     qPlatform_POSIX
            public:
                // Return 0 if no service running
                nonvirtual  pid_t   GetServicePID () const;
#endif


            public:
                // Return non-structured, human readable summary of service status
                nonvirtual  String      GetServiceStatusMessage () const;

            public:
                /*
                 *  RunAsService () will not return - until the service has terminated. It runs the service 'MainLoop'.
                 */
                virtual void                RunAsService ();

            public:
                /*
                 */
                nonvirtual  void            Start (Time::DurationSecondsType timeout = Time::kInfinite);
            protected:
                virtual void                _Start (Time::DurationSecondsType timeout);

            public:
                /*
                 */
                nonvirtual  void            Stop (Time::DurationSecondsType timeout = Time::kInfinite);
            protected:
                virtual     void            _Stop (Time::DurationSecondsType timeout);

            public:
                /*
                 */
                virtual void                Kill ();

            public:
                /*
                 */
                nonvirtual  void            Restart (Time::DurationSecondsType timeout = Time::kInfinite);
            protected:
                virtual void                _Restart (Time::DurationSecondsType timeout);

            public:
                /*
                 */
                virtual void                ReReadConfiguration ();

            public:
                /*
                 */
                virtual void                Pause ();

            public:
                /*
                 */
                virtual void                Continue ();

            protected:
                // Call to check if the service appears to be NOT RUNNING, but have some remnants of a previous run that
                // need to be cleaned up via _CleanupDeadService ()
                virtual     bool            _IsServiceFailed ();

            protected:
                // Called internally when - for example - asked to start and we find there are already lock files etc from
                // a previous run of the service, but its actually dead
                virtual     void            _CleanupDeadService ();

            protected:
                // Checks if the service process is actually running - not just if it is supposed to be. This can be used to
                // wait for a service to startup, or to shut down
                virtual     bool    _IsServiceActuallyRunning ();

            public:
                /*
                 */
                nonvirtual  ServiceDescription  GetServiceDescription () const;

            public:
                struct  CommandNames {
                    /*
                     *  The kRunAsService command is about the only command that tends to NOT be called by users on the command line.
                     *  it tells the code to run indefinitely, (until told to stop) - running the service loop.
                     *
                     *  This is typically called INDRECTLY via a specail fork/exec as a result of a kStart command, or its called from
                     *  init as part of system startup.
                     */
                    static  const   wchar_t kRunAsService[];
                    /*
                     *  The kStart command tells the service to start running. It returns an error
                     *  if the service is already started.
                     */
                    static  const   wchar_t kStart[];
                    /*
                     *  The kStop command tells the service to start terminate
                     */
                    static  const   wchar_t kStop[];
                    //DOCUMENT EACH
                    //NEATLY
                    // KILL termiantes (kill-9)
                    //
                    static  const   wchar_t kKill[];
                    // restart synonmy for stop (no error if not already running), and then start
                    static  const   wchar_t kRestart[];
                    // If service knows how to find its own config files - recheck them
                    static  const   wchar_t kReloadConfiguration[];
                    // SIGSTOP
                    static  const   wchar_t kPause[];
                    // SIGCONT
                    static  const   wchar_t kContinue[];
                };

            protected:
                /*
                 * Will handle the given command line argument, and return true if it recognized it, and handled it. It will return false otherwise.
                 */
                nonvirtual  bool    _HandleStandardCommandLineArgument (const String& arg);
            };

        }
    }
}
#endif  /*_Stroika_Frameworks_Service_Main_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Main.inl"
