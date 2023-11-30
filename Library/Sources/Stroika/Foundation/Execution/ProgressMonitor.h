/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ProgressMonitor_h_
#define _Stroika_Foundation_Execution_ProgressMonitor_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <string>

#include "../Configuration/Common.h"
#include "../Containers/Sequence.h"
#include "../DataExchange/VariantValue.h"
#include "../Execution/Function.h"
#include "Thread.h"

/**
 * TODO:
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *      @todo   MAYBE allow copy - but just document its a smart pointer and copy just increments refcount.
 *              NOT copy by value semantics.
 *
 *      @todo   Look at stuff I did I HeatthFrame progress code to automatically increase displayed progress values
 *              to monotonically when i have guesses - like for network activities. That should somehow integrate
 *              with this - maybe as an optional 'updater' module/adapter?
 *
 *      @todo   Not sure if we need to decompose into more component classes and use subtyping for stuff like
 *              thread support. It seems it mgiht work like this, but I'm sure it's not yet elegant.
 */

namespace Stroika::Foundation::Execution {

    /**
     *  ProgressMonitor is the basic interface used for progress tracking. Progress tracking both
     *  measures progress, and supports the notion of canceling. The reason progres and cancelability
     *  are tied together is that its only for 'long lived' tasks one might want to measure the progress of,
     *  that one might want to allow canceling.
     *
     *  A progress Monitor owns a list of ChangedCallbackType that can be associated with the ProgressMonitor.
     *  These callbacks are each called whenever the progress is known to have changed. Note - this callback
     *  uses std::function<>, and the callback runs on an arbitrary thread, not necessarily the one used
     *  by the creator of the ProgressMonitor.
     *
     *  A ProgressMonitor also has associated with it an arbitrary number of Updater objects. These are the things
     *  that one hands to processes (not OS processes, but long lived procedures or threads) whcih they then
     *  callback to to notify of thier progress.
     *
     *  An updater is retrieved from the root ProgressMonitor, and it has 'scope' of 0..1. You can construct
     *  sub-updaters by passing a base Updater to the Updater constructor along with a subrange (inside 0..1).
     *  That way - if you have sub-procedures, they can report on thier progress (0..1) and that is mapped to a subrange
     *  of the overall progress.
     *
     *  Note - in order to help debug the progress values, ProgressMonitor strictly enforces some rules. Progress
     *  starts at zero, and successive values are non-degreasing. This means the progress bar grows monotonically (though
     *  not necessarily smoothly). In order to avoid common floating point bugs with roudning errors, ProgressMonitor
     *  employs Math::PinToSpecialPoint ().
     *
     *  Users of ProgressMonitor can call "Cancel" on the ProgressMonitor at any point. This records a cancelation
     *  in the Updater object, so that when it calls Updater::SetCurrentProgressAndThrowIfCanceled () - and
     *  perhaps in other situations (see thread support below) - the progress will terminate immediately.
     *
     *  ProgressMontitor supports having the underlying long-lived-task happen EITHER in the current thread, or in
     *  another thread (the ProgressMonitor and related code is fully threadsafe).
     *
     *  If ProgressMontitor is constructed with an argument Thread (optional) - then attempts to Cancel the ProgressMonitor
     *  will also send an Abort() command to the associated thread. This can accelerate - depending less on co-operative
     *  checking - to cancel the long-lived progress-monitored process.
     * 
     *  \par Example Usage
     *      \code
     *          void CompileData (const filesystem::path& sourceFile, ProgressMonitor::Updater progress) 
     *          {
     *              ...
     *              progress.SetCurrentTaskInfo ("Compiling Strings"_k);
     *              progress.SetCurrentProgressAndThrowIfCanceled (0.3f);
     *              SubTask_ (ProgressMonitor::Updater{progress, 0.50f, 0.60f});    // also may do progress calls (0..1 in subtask mapped into .5 to .6 range here)
     *              ...
     *          }
     *          ProgressMonitor  progMon;
     *          prog.AddOnProgressCallback ([lastProg = 0u,  lastDesc = String{}] (const ProgressMonitor& progressMonitor) mutable {
     *                  unsigned int curProgPct = static_cast<unsigned int> (progressMonitor.GetProgress () * 100);
     *                  Assert (0 <= curProgPct and curProgPct <= 100);
     *                  if (lastDesc != progressMonitor.GetCurrentTaskInfo ().fName) {
     *                      cout << "\r\n";
     *                      lastDesc = progressMonitor.GetCurrentTaskInfo ().fName;
     *                      cout << "\t" << lastDesc.AsNarrowSDKString () << "\r\n";
     *                  }
     *                  if (lastProg != curProgPct) {
     *                      cout << "\r";
     *                      cout << Characters::CString::Format ("\t\t%d%% complete            ", curProgPct); /// spaces to wipe-out rest of line
     *                      lastProg = curProgPct;
     *                  }
     *              }); 
     *          CompileData (file, progMon);    // CompileData() takes updater, but ProgressMonitor has conversion op to create Updater...
     *      \endcode
     */
    class ProgressMonitor final {
    public:
        /**
         *
         */
        using ProgressRangeType = float;

    public:
        /**
         *  This is for consumers of progress information. Consumers MAY either poll the ProgressMonitor,
         *  or may register a callback to be notified of progress.
         */
        using ChangedCallbackType = Execution::Function<void (const ProgressMonitor& progressMonitor)>;

    private:
        class Rep_;

    public:
        /**
         *  If work thread is specified (optional) - then thread cancelation will work more efficiently.
         *  But this is not required.
         * 
         *  \todo Consider if we should take ChangedCallbackType CTOR arg; not sure of the workerThread usecase. CONSIDER!
         */
        ProgressMonitor ();
        ProgressMonitor (const ProgressMonitor&) = delete;
        ProgressMonitor (Thread::Ptr workThread);

    private:
        ProgressMonitor (const shared_ptr<Rep_>& rep);

    public:
        ~ProgressMonitor () = default;

    public:
        nonvirtual ProgressMonitor& operator= (const ProgressMonitor&) = delete;

    public:
        /**
         *  This doesn't need to be used. You can use ProgressMonitor progress monitor just peridocially calling
         *  GetProgress(). But you may use AddCallback () to recieve notifications of progress changes.
         */
        nonvirtual void AddOnProgressCallback (const ChangedCallbackType& progressChangedCallback);

    public:
        /**
         *  Return the progress value (between 0..1). This values starts at zero, and increases
         *  monotonically to 1.0
         */
        nonvirtual ProgressRangeType GetProgress () const;

    public:
        /**
         *  Cancelability. Anyone can call Cancel () on this progress object. If the progress
         *  object is handed to some long-lived task, that task may (at its discretion) - check
         *  the progress callback, and cancel its operation by throwing a UserCanceledException.
         *
         *  It is safe to call multiple times (and just may have no additional effect).
         *
         *  If a work thread is associated with the ProgressMonitor, it will be automatically
         *  aborted.
         */
        nonvirtual void Cancel ();

    public:
        class Updater;

    public:
        nonvirtual operator Updater ();

    public:
        struct CurrentTaskInfo;

    public:
        /**
         *  Often in displaying progress, its useful to have a notion of what the system is doing,
         *  and thats usually displayed far away from where the notion of progress stage resides.
         *  This API is usually called by the bit of code performing actions (to set the current task)
         *  and by the calling GUI to Get the current task description.
         *
         *  Note also - for reasons of localization - its often helpful to pass back specific
         *  information about the task in progress (like file 1 of 4).
         *
         *  Use the 'fExtraData' field of the CurrentTaskInfo.
         */
        nonvirtual CurrentTaskInfo GetCurrentTaskInfo () const;

    private:
        shared_ptr<Rep_> fRep_;

    private:
        friend class Updater;
    };

    /**
     *  Often in displaying progress, its useful to have a notion of what the system is doing,
     *  and thats usually displayed far away from where the notion of progress stage resides.
     *  This API is usually called by the bit of code performing actions (to set the current task)
     *  and by the calling GUI to Get the current task description.
     *
     *  Note also - for reasons of localization - its often helpful to pass back specific information
     *  about the task in progress (like file 1 of 4).
     *
     *  Use the 'fExtraData' field of the CurrentTaskInfo.
     */
    struct ProgressMonitor::CurrentTaskInfo {
        Characters::String         fName;
        DataExchange::VariantValue fDetails;

        CurrentTaskInfo (const Characters::String& taskName = {}, const DataExchange::VariantValue& details = {});
        CurrentTaskInfo (const CurrentTaskInfo&) = default;
    };

    /**
     *  The Updater is the API passed to code which knows about its progress through a long-lived task and makes callbacks
     *  to indicate phase, and percent progress (# 0..1).
     * 
     *      \note in old RFLLib code - this was called ProgressSubTask
     * 
     *      \todo Consider if DTOR should REMOVE added 'curTaskInfo' (stack behavior will require restructuring) - save old info on entry/construction perhaps
     */
    class ProgressMonitor::Updater {
    public:
        /**
         *  Use of the given Updater will generate 'setprogress' calls with appropriately scaled
         *  progress values.
         *
         *  Helper used to continue reporting progress, but breaking the progress into subtasks,
         *  and doing the artithmatic of integrating the total into an overall progress total.
         * 
         *  \note - initial updater generated via ProgressMontior::operator Updater (); null-updater
         *        maybe used if there are no progress updates to display;
         * 
         *  \note - all 'Updater' methods (besides the constructor and destructor) - are expected to be called from the context
         *        of the worker task (especially ThrowIfCanceled).
         */
        Updater () = delete;
        Updater (nullptr_t);
        Updater (const Updater& parentTask, ProgressRangeType fromProg, ProgressRangeType toProg);
        Updater (const Updater& parentTask, ProgressRangeType fromProg, ProgressRangeType toProg, const CurrentTaskInfo& taskInfo);

    private:
        Updater (const shared_ptr<Rep_>& r);

    public:
        /**
         *  Progress is a number 0..1. However, if outside that range, it will be silently pinned to be in that range (so
         *  caller need not check/be careful).
         * 
         *  However, we do 'weak assert' that its close to that range, since being more than a float-point round-off away, probably
         *  indicates a bug.
         */
        nonvirtual void SetProgress (ProgressRangeType p);

    public:
        /**
         *  Called from the context of a thread which has been given this progress object. This method will check
         *  if this progress object has been canceled, and in if so throw UserCanceledException.
         *
         *  Note - this function does NOT check if the itself thread has been aborted (as that is usually
         *  taken care of automatically or via CheckForInterruption)
         */
        nonvirtual void ThrowIfCanceled ();

    public:
        /**
         *  SetCurrentProgressAndThrowIfCanceled () overloads are handy helpers for code performing
         *  long-lived tasks to deal with being passed a null progress object (often) and do that check,
         *  and if non-null, then update the progress and check for cancelations.
         */
        nonvirtual void SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress);

    public:
        /**
         */
        nonvirtual void SetCurrentTaskInfo (const CurrentTaskInfo& taskInfo);

    private:
        friend class ProgressMonitor;

    private:
        nonvirtual void CallNotifyProgress_ () const;

    private:
        shared_ptr<Rep_>  fRep_;
        ProgressRangeType fFromProg_{0.0};
        ProgressRangeType fToProg_{1.0};
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ProgressMonitor.inl"

#endif /*_Stroika_Foundation_Execution_ProgressMonitor_h_*/
