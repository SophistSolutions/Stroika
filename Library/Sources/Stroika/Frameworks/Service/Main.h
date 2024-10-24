/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Service_Main_h_
#define _Stroika_Frameworks_Service_Main_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <filesystem>
#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/Execution/Process.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Streams/OutputStream.h"
#include "Stroika/Foundation/Time/Duration.h"

#if qPlatform_POSIX
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Signals.h"
#endif

/**
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *
 *      @todo   Do a better job handling default/reasonable timeout time for service to start/stop/restart.
 *              Hacked something mostly for UNIX.
 *
 *      @todo   Window service not fully working - see why when you start/stop we don't see
 *              Logger::Get ().Log (Logger::eNotice, L"User-service code is shut down");
 *              message printed out. Basic startup/shutdown seems roughly OK though.
 *
 *      @todo   Fix Main::BasicUNIXServiceImpl::SignalHandler_() use of static variable. Race conditions
 *              accessing static var. Use instance var and proper locking.
 *
 *      @todo   Fix Main::BasicUNIXServiceImpl::SignalHandler_ on kSIG_ReReadConfiguration to somehow q message
 *              to re-read, and not invoke re-read from thread (avoid possible deadlock with allocating mem in signal handler).
 *
 *      @todo   Fix majorly hacked (and unsafe anyhow cuz of malloc in signal handler)
 *              stuff. Righjt now needed to make Sercice STOP work under posix, but its
 *              NOT reliably done!!!
 *
 *              SEE "ADD SAFE SIGNAL HANDLER (DEADLOCK FREE) SUPPORT" in Execution::Signal module
 *
 *      @todo   Carefully review Windows service code for thread locking and races. Complete missing cases, and
 *              bullet proof.
 *
 *      @todo   Support passing extra args to subprocess. For example, if we do --start --logfile=xxx,
 *              We need the invocation of EXE --Run-As-Service to ALSO take --logfile=xxx param. NYI
 *
 *      @todo   Add "Starting" and "Stopping" status values to possible states.
 *              Probably add extra states, and then callback parameter to IApplicationRep::MainLoop()
 *              so it can callback and say status moved from starting to running. Easier to automatically
 *              do the stopping/stopped change (cuz we have the hooks).
 *
 *              Added the startedCB method - just so fewer incompatible API-breaking changes needed in the
 *              future.
 *
 *      @todo   Implement/test (it maybe implemented) --ReloadConfiguration operation.
 *
 *      @todo - Main::CommandNames - consider making these STRING params, and then this could be end-user configurable (good idea!)
 *
 *      @todo   Main::CommandNames - provide for command aliases.
 *
 *      @todo   Document clearly the distinction between running service and driver
 *              to control stop/start etc. Think through relationship between windows
 *              approahc and unix approach
 *
 *      @todo   Windoze implementation - supporting the richer set of control mechanism.
 *              AT LEAST support --stop from command-line (currently broken - but you can use net stop or net start servicename;
 *
 *      @todo   Support Pause/Continue
 *
 * Long-Term TODO:
 *      @todo   Consider adding a 'heartbeat' mechanism - so callback in IApplicationRep
 *              called periodically to see if all OK, and otherwise have recovery strategies.
 */

namespace Stroika::Frameworks::Service {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Sequence;
    using Execution::pid_t;

#if qPlatform_POSIX
    using Execution::SignalID;
#endif

    /**
     *  A service is a program that runs in the background on your computer, and has no user interface.
     *
     *  This is often very handy - and often useful to mix together with a related non-service (UI)
     *  program component.
     *
     *  This concept exists on both UNIX and Windows, but is implemented differently.
     *
     *  There are two different dimensions along which you can subtype the behavior of a service application:
     *      (1) the functionality the service provides
     *      (2) the OS service mechanism to use
     *
     *  Users of this class will themselves address #1, by subclassing from the IApplicationRep, and providing their application
     *  behavior. But users can also select different 'service implementation' strategies by selecting the IServiceIntegrationRep subtype
     *  to pass into the constructor. What service implementation strategies are available will depend on the OS you've built for,
     *  and often on command-line app arguments.
     *
     *  This class is sort of like a singleton, in that it can be instantiated only once, never copied, etc. But
     *  its unusual for the singleton pattern, since the user must explicitly construct it. The owner controls its lifetime.
     *
     *  Note that this class provides a convenient wrapper on requesting actions on the underlying service
     *  from EITEHR the process in which the service is run, or from any other process (via IPC - specific to the process
     *  service backend implementation).
     *
     *  This class is 'final' - since the intended use is to provide polymorphic behavior via the REP classes passed
     *  in to the CTOR.
     *
     *  Miscellaneous Notes:
     *      o   No need for a --daemonize option for --Run-As-Service, because that's essentially
     *          what the --start command does
     *
     *  \par Example Usage
     *      @see Samples/SimpleService project
     *      \code
     *      struct   AppRep_ : Main::IApplicationRep {
     *          AppRep_ () = default;
     *          ...
     *          virtual void  MainLoop (const std::function<void()>& startedCB) override
     *          {
     *              .... do your service and wait forever for thread abort
     *          }
     *
     *      };
     *      ...
     *      Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
     *      Main    m (make_shared<AppRep_> ());
     *      m.Run (args);
     *      \endcode
     */
    class Main final {
    public:
        struct ServiceDescription;

    public:
        class IApplicationRep;

    public:
        class SimpleIApplicationRepHelper;

    public:
        class IServiceIntegrationRep;

#if qPlatform_POSIX
    public:
        class BasicUNIXServiceImpl;
#endif

    public:
        class LoggerServiceWrapper;

    public:
        class RunNoFrillsService;

#if qPlatform_Windows
    public:
        class WindowsService;
#endif

    public:
        /**
         * The result type depends on your OS/compilation flags.
         */
        static shared_ptr<IServiceIntegrationRep> mkDefaultServiceIntegrationRep ();

    public:
        /**
         * Note - besides the obvious, the Main () function also sets signal handlers to point to this objects signal handler.
         */
        explicit Main (const shared_ptr<IApplicationRep>&        appRep,
                       const shared_ptr<IServiceIntegrationRep>& serviceIntegrationRep = mkDefaultServiceIntegrationRep ());
        Main ()            = delete;
        Main (const Main&) = delete;

    public:
        ~Main ();

    public:
        nonvirtual const Main& operator= (const Main&) = delete;

    public:
        struct CommandArgs;

    public:
        /**
         *  The caller COULD just call these operations and see if they fail, but the operations have side-effects.
         *  This lets the caller detect the features without the side-effects.
         */
        enum class ServiceIntegrationFeatures {
            eInstall,
            eGetServicePID,
        };

    public:
        /**
         *  The caller COULD just call these operations and see if they fail, but the operations have side-effects.
         *  This lets the caller detect the features without the side-effects.
         */
        nonvirtual Containers::Set<ServiceIntegrationFeatures> GetServiceIntegrationFeatures () const;

    public:
        /**
         *  These arguments are command-line arguments. They will also be indirectly passed to the IApplicationRep.
         *  So a typical app main() might be:
         *      TBD
         *
         *  When this function returns, the calling main() should exit. The caller should also be prepared for
         *  exceptions to be thrown by Run - for example - in case of bad command line arguments.
         *
         *  Note also - some command line arguments are interpreted as meaning to send messages to another process
         *  hosing the service. While others cause this process to become the process main process, and run until
         *  told to exit.
         */
        nonvirtual void Run (const CommandArgs& args, const Streams::OutputStream::Ptr<Characters::Character>& out = nullptr);

    public:
        /**
         *  Checks the state of the given service. NOTE - this works ACROSS PROCESSES. It can be called
         *  to ask in a controller exe if the serviceMain EXE is running. It also - COULD give the
         *  wrong answer - given races, so use with care.
         */
        enum class State : uint8_t {
            eStopped,
            eRunning,
            ePaused, // STOPPED in unix
        };
        nonvirtual State GetState () const;

    public:
        /**
         *  Return 0 if no service running
         *  ????
         */
        nonvirtual pid_t GetServicePID () const;

    public:
        /**
         *  Return non-structured, human readable summary of service status
         */
        nonvirtual String GetServiceStatusMessage () const;

    public:
        /**
         */
        nonvirtual void Install ();

    public:
        /**
         */
        nonvirtual void UnInstall ();

    public:
        /**
         *  RunAsService () will not return until the service has terminated. It runs the service 'MainLoop'.
         *  When that happens, the calling application should exit.
         *
         *  This should NOT be called directly - except when command line arguments say "Run-As-Service".
         *
         *  This function / operation WILL FAIL if the service is already running (so this automatically implements
         *  protection against running multiple instances of the same service - singleton pattern.
         *
         *  If the startup of the service fails due to exception, it will be propagated back to the caller.
         *
         *  @see RunDirectly
         */
        nonvirtual void RunAsService ();

    public:
        /**
         *  RunDirectly () is mostly a debug-handy/debug-friendly variant of RunAsService().
         *
         *  RunDirectly () will not return until the program has terminated. It runs the service 'MainLoop'.
         *  When that happens, the calling application should exit.
         *
         *  When using RunDirectly - other operations like 'status' and 'kill' etc - will NOT function.
         *  This bypasses the backend service mechanism - and just runs the application-specific code (typically
         *  so that can be debugged, but possibly also for testing or other purposes).
         *
         *  If the startup of the service fails due to exception, it will be propagated back to the caller.
         *
         *  @see RunAsService
         */
        nonvirtual void RunDirectly (const optional<Time::Duration>& runFor = {});

    public:
        /**
         */
        nonvirtual void Start (Time::DurationSeconds timeout = Time::kInfinity);

    public:
        /**
         */
        nonvirtual void Stop (Time::DurationSeconds timeout = Time::kInfinity);

    public:
        /**
         *  Does a regular stop, but if that doesn't work (by the given timeout), do low-level
         *  force stop and cleanup as best as possible.
         */
        nonvirtual void ForcedStop (Time::DurationSeconds timeout);

    public:
        /**
        */
        nonvirtual void Restart (Time::DurationSeconds timeout = Time::kInfinity);

    public:
        /**
         *  Does ForcedStop(timeout) - ignoring errors if was already stopped, and then starts.
         *  This only fails if it couldn't (even forced) stop the service (ignoring timeout) or if
         *  it fails to restart the service.
         */
        nonvirtual void ForcedRestart (Time::DurationSeconds timeout = Time::kInfinity, Time::DurationSeconds unforcedStopTimeout = Time::kInfinity);

    public:
        /**
        */
        virtual void ReReadConfiguration ();

    public:
        /**
        */
        virtual void Pause ();

    public:
        /**
        */
        virtual void Continue ();

    public:
        /**
        */
        nonvirtual ServiceDescription GetServiceDescription () const;

    public:
        struct CommandNames;

    public:
        struct CommandOptions;

    private:
        nonvirtual const IServiceIntegrationRep& GetServiceRep_ () const;
        nonvirtual IServiceIntegrationRep&       GetServiceRep_ ();
        nonvirtual const IApplicationRep&        GetAppRep_ () const;
        nonvirtual IApplicationRep&              GetAppRep_ ();

    private:
        shared_ptr<IServiceIntegrationRep> fServiceRep_; // no need to synchronize because all access to shared_ptr R/O after initialization
    };

    /**
    */
    struct Main::ServiceDescription {
        String fRegistrationName; // key name used when registering service with OS
        String fPrettyName;       // pretty printed version of fRegistrationName (can be same)
    };

    /**
     */
    struct Main::CommandNames {

        /**
         */
        static inline constexpr string_view kInstall = "Install"sv;

        /**
         */
        static inline constexpr string_view kUnInstall = "UnInstall"sv;

        /**
         *  The kRunAsService command is about the only command that tends to NOT be called by users on the command line.
         *  it tells the code to run indefinitely, (until told to stop) - running the service loop.
         *
         *  This is typically called INDRECTLY via a special fork/exec as a result of a kStart command, or its called from
         *  init as part of system startup.
         */
        static inline constexpr string_view kRunAsService = "Run-As-Service"sv;

        /**
         *  kRunDirectly is mostly a debug-handy/debug-friendly variant of RunAsService().
         *
         *  kRunDirectly will not return until the program has terminated. It runs the service 'MainLoop'.
         *  When that happens, the calling application should exit.
         *
         *  When using kRunDirectly - other operations like 'status' and 'kill' etc - will NOT function.
         *  This bypasses the backend service mechanism - and just runs the application-specific code (typically
         *  so that can be debugged, but possibly also for testing or other purposes).
         */
        static inline constexpr string_view kRunDirectly = "Run-Directly"sv;

        //static inline constexpr string_view kRunDirectlyFor = "Run-Directly-For"sv;

        /*
         *  The kStart command tells the service to start running. It returns an error
         *  if the service is already started.
         */
        static inline constexpr string_view kStart = "Start"sv;

        /**
         *  The kStop command tells the service to start terminate
         */
        static inline constexpr string_view kStop = "Stop"sv;

        //DOCUMENT EACH
        //NEATLY
        // KILL termiantes (kill-9)
        //
        static inline constexpr string_view kForcedStop = "ForcedStop"sv;

        // restart synonym for stop (no error if not already running), and then start
        static inline constexpr string_view kRestart = "Restart"sv;

        static inline constexpr string_view kForcedRestart = "ForcedRestart"sv;

        // If service knows how to find its own config files - recheck them
        static inline constexpr string_view kReloadConfiguration = "Reload-Configuration"sv;

        // SIGSTOP
        static inline constexpr string_view kPause = "Pause"sv;

        // SIGCONT
        static inline constexpr string_view kContinue = "Continue"sv;

        static inline constexpr string_view kRunFor = "runFor"sv;
        static inline constexpr string_view kStatus = "Status"sv;
    };

    struct Main::CommandOptions {
        static inline const Execution::CommandLine::Option kInstall{.fLongName       = CommandNames::kInstall,
                                                                    .fHelpOptionText = "Register the service as installed"sv};
        static inline const Execution::CommandLine::Option kUnInstall{.fLongName       = CommandNames::kUnInstall,
                                                                      .fHelpOptionText = "Unregister the service installation"sv};
        static inline const Execution::CommandLine::Option kRunAsService{
            .fLongName = CommandNames::kRunAsService, .fHelpOptionText = "Run as a service daemon - as of run by OS service manager"sv};
        static inline const Execution::CommandLine::Option kRunDirectly{.fLongName = CommandNames::kRunDirectly};
        // static inline const Execution::CommandLine::Option kRunDirectlyFor{.fLongName = CommandNames::kRunDirectlyFor, .fSupportsArgument = true};
        static inline const Execution::CommandLine::Option kStart{.fLongName       = CommandNames::kStart,
                                                                  .fHelpOptionText = "Request OS service manager start the service."sv};
        static inline const Execution::CommandLine::Option kStop{.fLongName       = CommandNames::kStop,
                                                                 .fHelpOptionText = "Request service manager stop the running service."sv};
        static inline const Execution::CommandLine::Option kForcedStop{
            .fLongName       = CommandNames::kForcedStop,
            .fHelpOptionText = "Request service manager stop the running service (aggressively if it doesn't respond)."sv};
        static inline const Execution::CommandLine::Option kRestart{.fLongName = CommandNames::kRestart, .fHelpOptionText = "Stop followed by Start."s};
        static inline const Execution::CommandLine::Option kForcedRestart{.fLongName       = CommandNames::kForcedRestart,
                                                                          .fHelpOptionText = "ForcedStop followed by Start."s};
        static inline const Execution::CommandLine::Option kReloadConfiguration{.fLongName = CommandNames::kReloadConfiguration,
                                                                                .fHelpOptionText = "Request service reload its configuration."sv};
        static inline const Execution::CommandLine::Option kPause{.fLongName       = CommandNames::kPause,
                                                                  .fHelpOptionText = "Request OS service manager pause a service."sv};
        static inline const Execution::CommandLine::Option kContinue{.fLongName = CommandNames::kContinue,
                                                                     .fHelpOptionText = "Request OS service manager continue a paused service."sv};

        static inline const Execution::CommandLine::Option kRunFor{.fLongName         = CommandNames::kRunFor,
                                                                   .fSupportsArgument = true,
                                                                   .fHelpArgName      = "NSECONDS"sv,
                                                                   .fHelpOptionText = "Run for the argument number of seconds, and quietly quit."sv};
        static inline const Execution::CommandLine::Option kStatus{.fLongName       = CommandNames::kStatus,
                                                                   .fHelpOptionText = "Check the status of the running service"sv};

        static inline const initializer_list<Execution::CommandLine::Option> kAll{
            kInstall, kUnInstall,     kRunAsService,        kRunDirectly, kStart,    kStop,   kForcedStop,
            kRestart, kForcedRestart, kReloadConfiguration, kPause,       kContinue, kRunFor, kStatus};
    };

    /**
     *  This is helpful to take command-line arguments and produce a set of things todo
     *  for the service mgr. This constructor will throw if it sees something obviously wrong
     *  but will ignore unrecognized arguments.
     */
    struct Main::CommandArgs {
        /**
         */
        CommandArgs (const Execution::CommandLine& cmdLine);

        enum class MajorOperation {
            eInstall,
            eUnInstall,
            eRunServiceMain,
            eRunDirectly, // uses fRunFor if provided
            eStart,
            eStop,
            eForcedStop,
            eRestart,
            eForcedRestart,
            eReloadConfiguration,
            ePause,
            eContinue,
        };

        /**
         */
        optional<MajorOperation> fMajorOperation;

        /**
         *  If provided, how long to run for in RunDirectly commands, else treat as run infinitely
         */
        optional<Time::Duration> fRunFor;

        /**
         */
        Sequence<String> fUnusedArguments;
    };

    /**
     * To use this interface users of the Framework::Service module must implement (to represent the running service).
     *
     *  MainLoop () is automatically setup to run on its own thread. Beware, the OnXXX
     *  events maybe called on this object, but from any thread so be careful of thread safety!
     */
    class Main::IApplicationRep {
    public:
        IApplicationRep ()                       = default;
        IApplicationRep (const IApplicationRep&) = delete;
        virtual ~IApplicationRep ()              = default;

    public:
        nonvirtual const IApplicationRep& operator= (const IApplicationRep&) = delete;

    public:
        /**
         *  Each command line argument is first passed to the IApplicationRep, and it returns true if it handled it
         *  and default for the default behavior.
         *
         *  Note - for multi-argument sequences, the implementer must set state so its prepared for the next
         *  argument.
         *
         *  This function MAY raise an exception if it receives unrecognized arguments, and should not
         *  print an error message in that case.
         */
        virtual bool HandleCommandLineArgument (const String& s);

    public:
        /**
         *  This should be overridden by each service, and should  never return until the service is done
         *  (probably because of a stop request).
         *
         *  Each MainLoop() runs on a thread, and it is terminated by being sent a Thread::AbortException.
         *
         *  This is handy, because its mostly treated by Stroika classes automatically, and mostly automatically
         *  cleans up any ongoing operations.
         *
         *  It might be written as:
         *      ...
         *      [[maybe_unused]] auto&& cleanup  =   Execution::Finally ([this] () {
         *      // do your cleanup here
         *      });
         *      // INITIALIZE_SOMETHING(); - maybe firing off other threads
         *      startedCallback (); // notify the service subsystem you've successfully started your service
         *      Execution::Event w;
         *      w.Wait ();  // wait til your told your done. This could be replaced with real work, but need
         *                  // not be - if you do you real work on another thread
         *      return;
         *
         *  See the Samples/SimpleService example code for a functional example.
         */
        virtual void MainLoop (const function<void ()>& startedCB) = 0;

    public:
        virtual void OnReReadConfigurationRequest ();

    public:
        //  returns a readable string about the service status. Note most of this is done by the envelope class, and this is just a way to add
        //  service specific extras
        virtual String GetServiceStatusMessage () const;

    public:
        virtual ServiceDescription GetServiceDescription () const = 0;
    };

    /**
     */
    class Main::SimpleIApplicationRepHelper : public Main::IApplicationRep {
    public:
        SimpleIApplicationRepHelper () = default;
    };

    /**
     *  \note   These CAN be accessed from multiple threads, and each subclass respecting this API
     *          must be internally synchronized.
     */
    class Main::IServiceIntegrationRep {
    public:
        IServiceIntegrationRep ()                              = default;
        IServiceIntegrationRep (const IServiceIntegrationRep&) = delete;

    public:
        virtual ~IServiceIntegrationRep () = default;

    public:
        nonvirtual const IServiceIntegrationRep& operator= (const IServiceIntegrationRep&) = delete;

    protected:
        /**
         *  Only legal to attach if _GetAttachedAppRep() == nullptr, and only legal to detach if _GetAttachedAppRep () != nullptr.
         *
         *  Must be attached before other methods can be called (at least most - document which).
         *
         *  Must be detached before DTOR.
         */
        virtual void _Attach (const shared_ptr<IApplicationRep>& appRep) = 0;

    protected:
        /**
         */
        virtual shared_ptr<IApplicationRep> _GetAttachedAppRep () const = 0;

    protected:
        virtual Containers::Set<ServiceIntegrationFeatures> _GetSupportedFeatures () const = 0;

    protected:
        virtual State _GetState () const = 0;

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
        virtual bool HandleCommandLineArgument (const String& s);

    protected:
        /**
         *  (only supported if (need service supports install-feature)
         */
        virtual void _Install () = 0;

    protected:
        /**
         *  (only supported if (need service supports uninstall-feature)
         */
        virtual void _UnInstall () = 0;

    protected:
        /**
         */
        virtual void _RunAsService () = 0;

    protected:
        /**
         */
        virtual void _RunDirectly (const optional<Time::Duration>& runFor) = 0;

    protected:
        /**
         */
        virtual void _Start (Time::DurationSeconds timeout) = 0;

    protected:
        /**
         */
        virtual void _Stop (Time::DurationSeconds timeout) = 0;

    protected:
        /**
         */
        virtual void _ForcedStop (Time::DurationSeconds timeout) = 0;

    protected:
        /**
         */
        virtual pid_t _GetServicePID () const = 0;

    private:
        friend class Main;
    };

    /**
     *  Wrap this around any IServiceIntegrationRep, to get Logging to work.
     *
     *      \note - LoggerServiceWrapper doesn't work with WindowsService to get logging on _RunAsService cuz we don't get a hook
     *              http://stroika-bugs.sophists.com/browse/STK-476
     */
    class Main::LoggerServiceWrapper : public Main::IServiceIntegrationRep {
    public:
        LoggerServiceWrapper (const shared_ptr<Main::IServiceIntegrationRep>& delegateTo);

    protected:
        virtual void                                        _Attach (const shared_ptr<IApplicationRep>& appRep) override;
        virtual shared_ptr<IApplicationRep>                 _GetAttachedAppRep () const override;
        virtual Containers::Set<ServiceIntegrationFeatures> _GetSupportedFeatures () const override;
        virtual State                                       _GetState () const override;
        virtual void                                        _Install () override;
        virtual void                                        _UnInstall () override;
        virtual void                                        _RunAsService () override;
        virtual void                                        _RunDirectly (const optional<Time::Duration>& runFor) override;
        virtual void                                        _Start (Time::DurationSeconds timeout) override;
        virtual void                                        _Stop (Time::DurationSeconds timeout) override;
        virtual void                                        _ForcedStop (Time::DurationSeconds timeout) override;
        virtual pid_t                                       _GetServicePID () const override;

    private:
        shared_ptr<Main::IServiceIntegrationRep> fDelegateTo_; // no need to synchronize because all access to shared_ptr R/O after initialization
    };

    /**
     *  Run with absolutely minimal OS integration support. Count on the app itself to make service calls
     *  to start/stop
     */
    class Main::RunNoFrillsService : public Main::IServiceIntegrationRep {
    protected:
        virtual void                                        _Attach (const shared_ptr<IApplicationRep>& appRep) override;
        virtual shared_ptr<IApplicationRep>                 _GetAttachedAppRep () const override;
        virtual Containers::Set<ServiceIntegrationFeatures> _GetSupportedFeatures () const override;
        virtual State                                       _GetState () const override;
        virtual void                                        _Install () override;
        virtual void                                        _UnInstall () override;
        virtual void                                        _RunAsService () override;
        virtual void                                        _RunDirectly (const optional<Time::Duration>& runFor) override;
        virtual void                                        _Start (Time::DurationSeconds timeout) override;
        virtual void                                        _Stop (Time::DurationSeconds timeout) override;
        virtual void                                        _ForcedStop (Time::DurationSeconds timeout) override;
        virtual pid_t                                       _GetServicePID () const override;

    private:
        shared_ptr<IApplicationRep> fAppRep_;
    };

#if qPlatform_POSIX
    /**
     *  Default for UNIX - responds in standard way to basic signals etc
     */
    class Main::BasicUNIXServiceImpl : public Main::IServiceIntegrationRep {
    public:
        BasicUNIXServiceImpl ();
        ~BasicUNIXServiceImpl ();

    protected:
        virtual void                                        _Attach (const shared_ptr<IApplicationRep>& appRep) override;
        virtual shared_ptr<IApplicationRep>                 _GetAttachedAppRep () const override;
        virtual Containers::Set<ServiceIntegrationFeatures> _GetSupportedFeatures () const override;
        virtual State                                       _GetState () const override;
        virtual void                                        _Install () override;
        virtual void                                        _UnInstall () override;
        virtual void                                        _RunAsService () override;
        virtual void                                        _RunDirectly (const optional<Time::Duration>& runFor) override;
        virtual void                                        _Start (Time::DurationSeconds timeout) override;
        virtual void                                        _Stop (Time::DurationSeconds timeout) override;
        virtual void                                        _ForcedStop (Time::DurationSeconds timeout) override;
        virtual pid_t                                       _GetServicePID () const override;

    protected:
        virtual filesystem::path _GetPIDFileName () const;

    protected:
        // Called internally when - for example - asked to start and we find there are already lock files etc from
        // a previous run of the service, but its actually dead
        virtual void _CleanupDeadService ();

    private:
        nonvirtual void SetupSignalHanlders_ (bool install);

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
        static constexpr SignalID kSIG_ReReadConfiguration = SIGHUP;

    private:
        Execution::SignalHandler fOurSignalHandler_; // only initialized and then read as consant, so no need to synchronize
        nonvirtual void          SignalHandler_ (SignalID signum);

        // MUST REDO THIS STUFF WITH EVENTS - when we have POSIX complaint event support in Stroika Foundation
    protected:
        nonvirtual bool _CheckShouldReReadConfig () const;
        nonvirtual void _DidReReadConfig ();

    private:
        bool fMustReReadConfig{false};

    private:
        Execution::Synchronized<shared_ptr<IApplicationRep>> fAppRep_;
        Execution::Synchronized<Execution::Thread::Ptr>      fRunThread_;
    };
#endif

#if qPlatform_Windows
    /**
     *  Run as a windows service - integrating with the Windows Service Mgr
     *
     *      \note - LoggerServiceWrapper doesn't work with WindowsService to get logging on _RunAsService cuz we don't get a hook
     *              http://stroika-bugs.sophists.com/browse/STK-476
     */
    class Main::WindowsService : public Main::IServiceIntegrationRep {
    public:
        WindowsService ();

    protected:
        virtual void                                        _Attach (const shared_ptr<IApplicationRep>& appRep) override;
        virtual shared_ptr<IApplicationRep>                 _GetAttachedAppRep () const override;
        virtual Containers::Set<ServiceIntegrationFeatures> _GetSupportedFeatures () const override;
        virtual State                                       _GetState () const override;
        virtual void                                        _Install () override;
        virtual void                                        _UnInstall () override;
        virtual void                                        _RunAsService () override;
        virtual void                                        _RunDirectly (const optional<Time::Duration>& runFor) override;
        virtual void                                        _Start (Time::DurationSeconds timeout) override;
        virtual void                                        _Stop (Time::DurationSeconds timeout) override;
        virtual void                                        _ForcedStop (Time::DurationSeconds timeout) override;
        virtual pid_t                                       _GetServicePID () const override;

    private:
        nonvirtual Characters::SDKString GetSvcName_ () const;
        nonvirtual bool                  IsInstalled_ () const noexcept;
        nonvirtual void                  SetServiceStatus_ (DWORD dwState) noexcept;
        nonvirtual void                  ServiceMain_ (DWORD dwArgc, LPTSTR* lpszArgv) noexcept;
        static void WINAPI               StaticServiceMain_ (DWORD dwArgc, LPTSTR* lpszArgv) noexcept;
        nonvirtual void                  Handler_ (DWORD dwOpcode) noexcept;
        static void WINAPI               StaticHandler_ (DWORD dwOpcode) noexcept;
        nonvirtual void                  OnStopRequest_ () noexcept;

    private:
        static WindowsService*      s_SvcRunningTHIS_;
        Execution::Thread::Ptr      fRunThread_;
        SERVICE_STATUS_HANDLE       fServiceStatusHandle_{nullptr}; // nullptr if invalid - not INVALID_HANDLE
        SERVICE_STATUS              fServiceStatus_{};
        shared_ptr<IApplicationRep> fAppRep_;
    };
#endif
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Main.inl"

#endif /*_Stroika_Frameworks_Service_Main_h_*/
