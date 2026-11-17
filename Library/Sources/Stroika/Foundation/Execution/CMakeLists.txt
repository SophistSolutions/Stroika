target_sources (stroika-foundation PRIVATE
    AbortableMutex.cpp
    Activity.cpp
    CommandLine.cpp
    CPUAffinity.cpp
    DLLSupport.cpp
    Exceptions.cpp
    FeatureNotSupportedException.cpp
    IntervalTimer.cpp
    Logger.cpp
    Module.cpp
    OperationNotSupportedException.cpp
    Process.cpp
    ProcessRunner.cpp
    ProgressMonitor.cpp
    RequiredComponentMissingException.cpp
    RequiredComponentVersionMismatchException.cpp
    SharedStaticData.cpp
    SilentException.cpp
    Signals.cpp
    SignalHandlers.cpp
    Sleep.cpp
    Synchronized.cpp
    Thread.cpp
    ThreadPool.cpp
    Throw.cpp
    Users.cpp
    WaitableEvent.cpp
    WaitForIOReady.cpp
)

add_subdirectory (Platform)
add_subdirectory (Resources)
