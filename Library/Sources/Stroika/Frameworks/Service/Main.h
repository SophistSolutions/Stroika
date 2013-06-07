/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Service_Main_h_
#define _Stroika_Frameworks_Service_Main_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/Execution/Process.h"
#include    "../../Foundation/Execution/Thread.h"
#include    "../../Foundation/Memory/Optional.h"

#if     qPlatform_POSIX
#include    "../../Foundation/Execution/Signals.h"
#endif



/**
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *				(note - basic functionality works on linux - start/stop/restart etc), but nothing on windoze yet.
 *
 * TODO:
 *
 *		@todo	Add ShowUsage () method to IApp... and to service (vectoring) - and use that
 *				to oimplemtn usage behvairo genericlaly. And do 'invalid-param' exAborttion
 *				and use that for usage - to trigger usage.
 *
 *      @todo   Fix majorly hacked (and unsafe anyhow cuz of malloc in signal handler)
 *              stuff. Righjt now needed to make Sercice STOP work under posix, but its
 *              NOT reliably done!!!
 *
 *      @todo   Lose (I THINK) OnStopRequest - for Ix... app - instread document
 *              that we use throwthreadexception!
 *
 *      @todo   Command-line processing is kludged and needs cleanup!!!
 *
 *      @todo   Document clearly the distinction between running service and driver
 *              to control stop/start etc. Think through relationship between windows
 *              approahc and unix approach
 *
 *      @todo   Consider adding IServceApp wrapper (taking IServcieApp sharedPtr as ctor arg) - which
 *              adds in SYSLOG calls for state changes.
 *
 *      @todo   Windoze implementation - supproting the richer set of control mechanism.
 *
 *      @todo   For UNIX
 *          (o) Get working (again) - used to work about a year ago but did major rewirte. Try to get
 *              working with demo app.
 *
 *          (o) Store in file system file with current PID.
 *
 *          (o) Start/Stop work by sending SIGNALS
 *
 *          (o) Must have generic signal handler registered (at least for TERMINATE/STOP/CONTINUE, SIGNIT (re-read conf))
 *
 *      @todo   Support Pause/Continue
 *
 *      @todo   Support Install/Uninstall () - at least on windoze!
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   Service {


            using   namespace   Stroika::Foundation;


            using   Characters::String;
            using   Containers::Sequence;
            using   Execution::pid_t;


#if     qPlatform_POSIX
            using   Execution::SignalIDType;
#endif


            /**
             *  There are two differnt dimentions along which you can subtype the behavior of a service application:
             *      (1) the functionality the service provides
             *      (2) the OS service mechanism to use
             *
             *  Users of this class will themselves address #1, by subclassing from the IApplicationRep, and providing their application
             *  behavior. But users can also select different 'service implementation' strategies by selecting the IServiceIntegrationRep subtype
             *  to pass into the constructor. What serice implementation strategies are available will depend on the OS you've built for,
             *  and often on command-line app arguments.
             *
             *  This class is sort of like a singleton, in that it can be instantiated only once, never copied, etc. But
             *  its unusual for the singleton pattern, since the user must explicitly construct it. The owner controls its lifetime.
             *
             *  Note that this class provides a convient wrapper on requesting actions on the underlying service
             *  from EITEHR the process in which the service is run, or from any other process (via IPC - specific to the process
             *  service backend implementation).
             */
            class   Main {
            public:
                struct  ServiceDescription;

            public:
                class   IApplicationRep;

            public:
                class   SimpleIApplicationRepHelper;

            public:
                class   IServiceIntegrationRep;

#if     qPlatform_POSIX
            public:
                class   BasicUNIXServiceImpl;
#endif

            public:
                class   RunTilIdleService;

            public:
                class   RunNoFrillsService;

#if     qPlatform_Windows
            public:
                class   WindowsService;
#endif

            public:
                /**
                 * The result type depends on your OS/compilation flags.
                 */
                static  shared_ptr<IServiceIntegrationRep>  mkDefaultServiceIntegrationRep ();

            public:
                /**
                 * Note - besides the obvious, the Main () function also sets signal handlers to point to this objects signal handler.
                 */
                explicit Main (shared_ptr<IApplicationRep> appRep, shared_ptr<IServiceIntegrationRep> serviceIntegrationRep = mkDefaultServiceIntegrationRep ());

            public:
                ~Main ();
            public:
                NO_COPY_CONSTRUCTOR(Main);
                NO_ASSIGNMENT_OPERATOR(Main);

            public:
                struct  CommandArgs;

            public:
                /*
                 *  These arguments are command-line arguments. They will also be indirectly passed to the IApplicationRep.
                 *  So a typical app main() might be:
                 *      TBD
                 *
                 *  When this function returns, the calling main() should exit. The caller should also be prepared for
                 *  exceptions to be thrown by Run - for example - in case of bad command line arguments.
                 *
                 *  Note also - some command line arguments are interpretted as meaning to send messages to another process
                 *  hosing the service. While others cause this process to become the process main process, and run until
                 *  told to exit.
                 */
                nonvirtual  void    Run (const CommandArgs& args);

            public:
                /**
                 *  Checks the state of the given service. NOTE - this works ACROSS PROCESSES. It can be called
                 *  to ask in a controller exe if the serviceMain EXE is running. It also - COULD give the
                 *  wrong answer - given races, so use with care.
                 */
                enum  class State : uint8_t {
                    eStopped,
                    eRunning,
                    ePaused,        // STOPPED in unix
                };
                nonvirtual  State   GetState () const;

            public:
                /**
                 *  Return 0 if no service running
                 *  ????
                 */
                nonvirtual  pid_t   GetServicePID () const;

            public:
                /**
                 *  Return non-structured, human readable summary of service status
                 */
                nonvirtual  String      GetServiceStatusMessage () const;

            public:
                /*
                 *  RunAsService () will not return until the service has terminated. It runs the service 'MainLoop'.
                 *  When that happens, the calling application should exit.
                 *
                 *  This should NOT be called directly - except when command line arguments say "RunAsService".
                 *  .... TO BE CLARIFIED .. RETHOUGHT>...
                 */
                nonvirtual  void            RunAsService ();

            public:
                /**
                 */
                nonvirtual  void            Start (Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 */
                nonvirtual  void            Stop (Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  Does a regular stop, but if that doesnt work (by the given timeout), do low-level
                 *  force stop and cleanup as best as possible.
                 */
                nonvirtual  void            ForcedStop (Time::DurationSecondsType timeout);

            public:
                /**
                */
                nonvirtual  void            Restart (Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  Does ForcedStop(timeout) - ignoring errors if was already stopped, and then starts.
                 *  This only fails if it couldn't (even forced) stop the service (ignoring timeout) or if
                 *  it fails to restart the service.
                 */
                nonvirtual  void            ForcedRestart (Time::DurationSecondsType timeout = Time::kInfinite, Time::DurationSecondsType unforcedStopTimeout = Time::kInfinite);

            public:
                /**
                */
                virtual void                ReReadConfiguration ();

            public:
                /**
                */
                virtual void                Pause ();

            public:
                /**
                */
                virtual void                Continue ();

            public:
                /**
                */
                nonvirtual  ServiceDescription  GetServiceDescription () const;

            public:
                struct  CommandNames;

            private:
                nonvirtual  const IServiceIntegrationRep&   GetServiceRep_ () const;
                nonvirtual  IServiceIntegrationRep&         GetServiceRep_ ();
                nonvirtual  const IApplicationRep&          GetAppRep_ () const;
                nonvirtual  IApplicationRep&                GetAppRep_ ();

            private:
                static  Main*   sTHIS_;

            private:
                shared_ptr<IServiceIntegrationRep>      fServiceRep_;
            };


            /**
            */
            struct  Main::ServiceDescription {
                String  fRegistrationName;      // key name used when registering service with OS
                String  fPrettyName;            // pretty printed version of fRegistrationName (can be same)
            };


            /**
            */
            struct  Main::CommandNames {
                /**
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

                /**
                 *  The kStop command tells the service to start terminate
                 */
                static  const   wchar_t kStop[];
                //DOCUMENT EACH
                //NEATLY
                // KILL termiantes (kill-9)
                //
                static  const   wchar_t kForcedStop[];
                // restart synonmy for stop (no error if not already running), and then start
                static  const   wchar_t kRestart[];
                static  const   wchar_t kForcedRestart[];
                // If service knows how to find its own config files - recheck them
                static  const   wchar_t kReloadConfiguration[];
                // SIGSTOP
                static  const   wchar_t kPause[];
                // SIGCONT
                static  const   wchar_t kContinue[];
            };


            /**
             *  This is helpful to take command-line arguments and produce a set of things todo
             *  for the service mgr. This constructor will throw if it sees something obviously wrong
             *  but will ignore unrecognized arguments.
             */
            struct  Main::CommandArgs {
                CommandArgs ();
                CommandArgs (const Sequence<String>& args);

                enum    class   MajorOperation {
                    eRunServiceMain,
                    eStart,
                    eStop,
                    eForcedStop,
                    eRestart,
                    eForcedRestart,
                    eReloadConfiguration,
                    ePause,
                    eContinue,
                };

                Memory::Optional<MajorOperation>    fMajorOperation;
                Sequence<String>                    fUnusedArguments;
            };


            /**
             * To use this class you must implement your own Rep (to represent the running service).
             *
             *  MainLoop () is automatically setup to run on its own thread. Betware, the OnXXX events maybe called on this object, but from any thread
             *  so be careful of thread safety!
             */
            class   Main::IApplicationRep {
            public:
                IApplicationRep ();
                virtual ~IApplicationRep ();
            public:
                NO_COPY_CONSTRUCTOR(IApplicationRep);
                NO_ASSIGNMENT_OPERATOR(IApplicationRep);


            public:
                /**
                 *  Each command line argument is first passed to the IApplicationRep, and it returns true if it handled it
                 *  and default for the default behavior.
                 *
                 *  Note - for multi-argument sequences, the implementer must set state so its prepared for the next
                 *  argument.
                 *
                 *  This function MAY raise an exception if it receives unrecognized argumetns, and shoudl not
                 *  print an error message in that case.
                 */
                virtual bool    HandleCommandLineArgument (const String& s);

            public:
                /**
                 * This should be overridden by each service, and should  never return until the service is done (stop request).
                 *
                 *  It might be written as:
                 *      DoServiceOneTimeInits_();
                 *      bool    stopping = false;
                 *      _SimpleGenericRunLoopHelper (&e, &stopping, [] () {
                 *          DoPeriodicallyWithoutBlocking...();
                 *      });
                 *      DoCleanupsAsServiceStops_();
                 *
                 *  @see _SimpleGenericRunLoopHelper
                 */
                virtual void                MainLoop () = 0;

            protected:
                /*
                 *  This utility function MAY be used inside MainLoop() to implement run-til-stop logic.
                 *  Just provide a lambda/callback todo the inner loop of your main loop. That loop will be called
                 *  repeatedly (unless stopping is set).
                 *      aka:
                 *          while (not *stopping) {
                 *              realMainInnerLoop ();   // must not block for long periods - or must itself check checkStopEvent
                 *              checkStopEvent->Wait();
                 *          }
                 */
                void    _SimpleGenericRunLoopHelper (Execution::Event* checkStopEvent, bool* stopping, const std::function<void()>& realMainInnerLoop);

            public:
                virtual void                OnStartRequest ();

            public:
                virtual void                OnStopRequest ();

            public:
                virtual void                OnReReadConfigurationRequest ();    //NOT USED NOW - UNCLEAR IF/HOW WE WANT TODO THIS -- LGP 2011-09-24

            public:
                //  returns a readable string about the service status. Note most of this is done by the envelope class, and this is just a way to add
                //  service specific extras
                virtual String              GetServiceStatusMessage () const;

            public:
                virtual ServiceDescription  GetServiceDescription () const = 0;
            };


            /**
             */
            class   Main::SimpleIApplicationRepHelper : public Main::IApplicationRep {
            public:
                SimpleIApplicationRepHelper ();
            };


            /**
             */
            class   Main::IServiceIntegrationRep {
            public:
                IServiceIntegrationRep ();
                NO_COPY_CONSTRUCTOR(IServiceIntegrationRep);
                NO_ASSIGNMENT_OPERATOR(IServiceIntegrationRep);
            public:
                virtual ~IServiceIntegrationRep ();

            protected:
                /**
                 *  Only legel to attach if _GetAttachedAppRep() == nullptr, and only legal to detach if _GetAttachedAppRep () != nullptr.
                 *
                 *  Must be attached before other methods can be called (at least most - document which).
                 *
                 *  Must be detatched before DTOR.
                 */
                virtual void    _Attach (shared_ptr<IApplicationRep> appRep)    =   0;

            protected:
                /**
                 */
                virtual shared_ptr<IApplicationRep>     _GetAttachedAppRep () const    =   0;

            protected:
                virtual  State               _GetState () const =   0;

            protected:
                /**
                 *  Each command line argument is first passed to the IApplicationRep, and it returns true if it handled it
                 *  and default for the default behavior.
                 *
                 *  Note - for multi-argument sequences, the implementer must set state so its prepared for the next
                 *  argument.
                 *
                 *  This function MAY raise an exception if it receives unrecognized argumetns, and shoudl not
                 *  print an error message in that case.
                 */
                virtual bool    HandleCommandLineArgument (const String& s);

            protected:
                /**
                 */
                virtual void    _RunAsAservice ()    =   0;

            protected:
                /**
                 */
                virtual     void    _Start (Time::DurationSecondsType timeout)  =   0;

            protected:
                /**
                 */
                virtual     void    _Stop (Time::DurationSecondsType timeout)   =   0;

            protected:
                /**
                 */
                virtual     void    _ForcedStop (Time::DurationSecondsType timeout)   =   0;

            protected:
                /**
                 */
                virtual  pid_t      _GetServicePID () const = 0;

            private:
                friend  class   Main;
            };


            /**
             *  Mostly for regression tests (and windoze)
             */
            class   Main::RunTilIdleService : public Main::IServiceIntegrationRep {
            public:
                RunTilIdleService ();
            protected:
                virtual void                        _Attach (shared_ptr<IApplicationRep> appRep) override;
                virtual shared_ptr<IApplicationRep> _GetAttachedAppRep () const override;
                virtual  State                      _GetState () const override;
                virtual void                        _RunAsAservice () override;
                virtual void                        _Start (Time::DurationSecondsType timeout) override;
                virtual void                        _Stop (Time::DurationSecondsType timeout) override;
                virtual void                        _ForcedStop (Time::DurationSecondsType timeout) override;
                virtual pid_t                       _GetServicePID () const override;
            private:
                Execution::Thread           fRunThread_;
                shared_ptr<IApplicationRep> fAppRep_;
            };


            /**
             *  Run with absolultely minimal OS integration support. Count on the app itself to make service calls
             *  to start/stop
             */
            class   Main::RunNoFrillsService : public Main::IServiceIntegrationRep {
            protected:
                virtual void                        _Attach (shared_ptr<IApplicationRep> appRep) override;
                virtual shared_ptr<IApplicationRep> _GetAttachedAppRep () const override;
                virtual  State                      _GetState () const override;
                virtual void                        _RunAsAservice () override;
                virtual void                        _Start (Time::DurationSecondsType timeout) override;
                virtual void                        _Stop (Time::DurationSecondsType timeout) override;
                virtual void                        _ForcedStop (Time::DurationSecondsType timeout) override;
                virtual pid_t                       _GetServicePID () const override;
            private:
                shared_ptr<IApplicationRep> fAppRep_;
            };


#if     qPlatform_POSIX
            /**
             *  Default for UNIX - responds in standard way to basic signals etc
             */
            class   Main::BasicUNIXServiceImpl : public Main::IServiceIntegrationRep {
            public:
                BasicUNIXServiceImpl ();
            protected:
                virtual void                        _Attach (shared_ptr<IApplicationRep> appRep) override;
                virtual shared_ptr<IApplicationRep> _GetAttachedAppRep () const override;
                virtual  State                      _GetState () const override;
                virtual void                        _RunAsAservice () override;
                virtual void                        _Start (Time::DurationSecondsType timeout) override;
                virtual void                        _Stop (Time::DurationSecondsType timeout) override;
                virtual void                        _ForcedStop (Time::DurationSecondsType timeout) override;
                virtual pid_t                       _GetServicePID () const override;

            protected:
                virtual String              _GetPIDFileName () const;

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

            private:
                nonvirtual  void    SetupSignalHanlders_ ();

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
                static  constexpr   SignalIDType kSIG_ReReadConfiguration    =   SIGHUP;
#else
                static  const   SignalIDType kSIG_ReReadConfiguration    =   SIGHUP;
#endif
            private:
                static  void    SignalHandler_ (SignalIDType signum);

                // MUST REDO THIS STUFF WITH EVENTS - when we have POSIX complaint event support in Stroika Foundation
            protected:
                nonvirtual  bool    _CheckShouldReReadConfig () const;
                nonvirtual  void    _DidReReadConfig ();
            private:
                bool    fMustReReadConfig;

            private:
                bool    fStopping_; // set to true externally (from other thread) and MainLoop should terminate itself cleanly

            private:
                Execution::Thread           fRunThread_;
                shared_ptr<IApplicationRep> fAppRep_;
            };
#endif


#if     qPlatform_Windows
            /**
             *  Run as a windows service - integrating with the Windows Service Mgr
             */
            class   Main::WindowsService : public Main::IServiceIntegrationRep {
            public:
                WindowsService ();
            protected:
                virtual void                        _Attach (shared_ptr<IApplicationRep> appRep) override;
                virtual shared_ptr<IApplicationRep> _GetAttachedAppRep () const override;
                virtual  State                      _GetState () const override;
                virtual void                        _RunAsAservice () override;
                virtual void                        _Start (Time::DurationSecondsType timeout) override;
                virtual void                        _Stop (Time::DurationSecondsType timeout) override;
                virtual void                        _ForcedStop (Time::DurationSecondsType timeout) override;
                virtual pid_t                       _GetServicePID () const override;
            private:
                nonvirtual  Characters::TString GetSvcName_ () const;
                nonvirtual  void                SetServiceStatus_ (DWORD dwState) noexcept;
                nonvirtual  void                ServiceMain_ (DWORD dwArgc, LPTSTR* lpszArgv) noexcept;
                static      void    WINAPI      StaticServiceMain_ (DWORD dwArgc, LPTSTR* lpszArgv) noexcept;
            private:
                SERVICE_STATUS_HANDLE       fServiceStatusHandle_;      // nullptr if invalid - not INVALID_HANDLE
                SERVICE_STATUS              fServiceStatus_;
                shared_ptr<IApplicationRep> fAppRep_;
            };
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Main.inl"

#endif  /*_Stroika_Frameworks_Service_Main_h_*/
