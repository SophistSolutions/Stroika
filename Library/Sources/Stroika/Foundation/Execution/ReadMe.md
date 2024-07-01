# Stroika::[Foundation](../)::Execution

This folder contains all the Stroika Library [Foundation](../)::Execution source code.

## Overview

Execution pattern classes (external process runner, exception handling support, synchronization patterns, blocking queue, thread pools, and much more).

## Modules

- [AbortableMutex.h](AbortableMutex.h)
- [Activity.h](Activity.h)
- [BlockingQueue.h](BlockingQueue.h)
- [CommandLine.h](CommandLine.h) - parse command lines
- [Common.h](Common.h)
- [ConditionVariable.h](ConditionVariable.h) - support use of std::condition_variable
- [DLLSupport.h](DLLSupport.h)
- [Exceptions.h](Exceptions.h)
- [FeatureNotSupportedException.h](FeatureNotSupportedException.h)
- [Finally.h](Finally.h) - very handy - try/finally like support
- [Function.h](Function.h) - std::function<> but allow for \<, more usable equality etc (key to add/remove callbacks)
- [Logger.h](Logger.h)
- [Module.h](Module.h)
- [ModuleGetterSetter.h](ModuleGetterSetter.h)
- [NullMutex.h](NullMutex.h)
- [OperationNotSupportedException.h](OperationNotSupportedException.h)
- [PIDLoop.h](PIDLoop.h)
- [Platform/](Platform/)
- [Process.h](Process.h)
- [ProcessRunner.h](ProcessRunner.h) - invoke process, hookup stdin/stdout to streams
- [ProgressMonitor.h](ProgressMonitor.h)
- [RequiredComponentMissingException.h](RequiredComponentMissingException.h)
- [RequiredComponentVersionMismatchException.h](RequiredComponentVersionMismatchException.h)
- [Resources/](Resources/)
- [SharedStaticData.h](SharedStaticData.h)
- [SignalHandlers.h](SignalHandlers.h)
- [Signals.h](Signals.h)
- [SilentException.h](SilentException.h)
- [Sleep.h](Sleep.h)
- [SpinLock.h](SpinLock.h)
- [Synchronized.h](Synchronized.h) - very useful pattern - like java/c# synchronized/lock - wrap object with its lock
- [Thread.h](Thread.h) - very helpful wrapper on std::thread supporting cancelation
- [ThreadPool.h](ThreadPool.h) - queue tasks and run them on a fixed size set of threads as the threads become ready
- [TimedLockGuard.h](TimedLockGuard.h)
- [TimeOutException.h](TimeOutException.h)
- [UpdatableWaitForIOReady.h](UpdatableWaitForIOReady.h)
- [UserCanceledException.h](UserCanceledException.h)
- [Users.h](Users.h)
- [VirtualConstant.h](VirtualConstant.h)
- [WaitableEvent.h](WaitableEvent.h)
- [WaitForIOReady.h](WaitForIOReady.h)
- [WhenTimeExceeded.h](WhenTimeExceeded.h)
