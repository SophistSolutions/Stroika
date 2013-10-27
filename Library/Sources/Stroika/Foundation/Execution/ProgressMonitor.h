/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ProgressMonitor_h_
#define _Stroika_Foundation_Execution_ProgressMonitor_h_    1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <string>

#include    "../Configuration/Common.h"
#include    "../Containers/Sequence.h"
#include    "../Memory/VariantValue.h"



/**
 * TODO:
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *      @todo   Need subclass (or builtin ability) to specify lambda to run and then have cancel do more
 *              and stop its thread.
 *
 *      @todo   If we make (document) VariantValue to be theradsafe, then we can lift critical section
 *              use here and make it simpler!
 *
 *      @todo   Consider  having ProgressMonitor be as smartptr type, instead of passing ProgressMonitor* all over
 *              the place. And break into submodules:
 *              Progress/
 *                  Monitor
 *                  Normalizer
 *                  Updater???
 *                  Listener???
 *          <<MOSTLY DONE BUT REVIEW AND CONSIDER NAMES>>>
 *
 *      @todo   This all needs tons of code review, and careful rethinking/analysis.
 *
 *      @todo   Consider carefully if progress callback SB shared_ptr<> object. Document reason for choice.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  @todo MAYBE allow copy - but just document its a smart pointer and copy just increments refcount. NOT copy by value semantics.
             *
             *  @todo REDO ALL DOCS - ABOUT NULL HANDLING AND ASSERTIONS ETC.
             *
             * This class is the basic interface used for progress tracking. Progress tracking both measures progress,
             * and supports the notion of canceling. The reason progres and cancelability are tied together is that its
             * only for 'long lived' tasks one might want to measure the progress of, that one might want to allow canceling.
             *
             * An instance of ProgressMonitor can be passed to any function which supports progress measurement (by pointer typically,
             * or reference). That function can then run off and do a long-lived computation (often in another thread), and the user
             * of this class can monitor the progress (e.g. updating a ui display), and can Cancel it at any point.
             *
             * Though this CAN all be used from a single threaded application (using some sort of co-operative multitasking) its intended
             * use is with threads: one working, and the other controlling/monitoring (e.g. ui).
             *
             * The caller (in the control thread) can call GetProgress() repeatedly - to find out whats changed, or can register a callback,
             * that will be notified on any change. But NOTE - the callback maybe called on a different thread than the ProgressMontitor was
             * originally created on!
             */
            class   ProgressMonitor final   {
            public:
                NO_COPY_CONSTRUCTOR (ProgressMonitor);
                NO_ASSIGNMENT_OPERATOR (ProgressMonitor);

            public:
                /**
                 *
                 */
                typedef float   ProgressRangeType;

            public:
                /**
                 *  This is for consumers of progress information. Consumers MAY either poll the ProgressMonitor,
                 *  or may register a callback to be notified of progress.
                 */
                typedef std::function<void (const ProgressMonitor& progressMonitor)>    ProgressChangedCallbackType;

            private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
            public:
#endif
                class IRep_;

            public:
                /**
                 */
                ProgressMonitor ();
                ProgressMonitor (const ProgressChangedCallbackType& progressChangedCallback);

            private:
                ProgressMonitor (const shared_ptr<IRep_>& rep);

            public:
                ~ProgressMonitor ();

            public:
                /**
                 *  This doesn't need to be used. You can use ProgressMonitor progress monitor just peridocially calling
                 *  GetProgress(). But you may use AddCallback () to recieve notifications of progress changes.
                 */
                nonvirtual  void    AddOnProgressCallback (const ProgressChangedCallbackType& progressChangedCallback);

            public:
                nonvirtual  ProgressRangeType   GetProgress () const;

            public:
                /**
                 *  Cancelability. Anyone can call Cancel () on this progress object. If the progress object is handed to
                 *  some long-lived task, that task may (at its discretion) - check the progress callback, and cancel its operation
                 *  by throwing a UserCanceledException.
                 */
                nonvirtual  void    Cancel ();  // causes this 'progress callback' to be marked for canceling (aborting). If already canceled, it does nothing

            public:
                class   TaskNotifier;

            public:
                nonvirtual  operator TaskNotifier ();

            public:
                struct  CurrentTaskInfo;

            public:
                /**
                 * Often in displaying progress, its useful to have a notion of what the system is doing, and thats usually displayed far away
                 *  from where the notion of progress stage resides. This API is usually called by the bit of code performing actions (to set the current task)
                 *  and by the calling GUI to Get the current task description.
                 *
                 *  Note also - for reasons of localization - its often helpful to pass back specific information about the task in progress (like file 1 of 4).
                 *  Using the 'fExtraData' field of the
                 */
                nonvirtual  CurrentTaskInfo GetCurrentTaskInfo () const;

            private:
                shared_ptr<IRep_>   fRep_;

            private:
                friend  class   TaskNotifier;
            };


            /**
             *  Often in displaying progress, its useful to have a notion of what the system is doing,
             *  and thats usually displayed far away from where the notion of progress stage resides.
             *  This API is usually called by the bit of code performing actions (to set the current task)
             *  and by the calling GUI to Get the current task description.
             *
             *  Note also - for reasons of localization - its often helpful to pass back specific information
             *  about the task in progress (like file 1 of 4). Using the 'fExtraData' field of the
             */
            struct  ProgressMonitor::CurrentTaskInfo {
                Characters::String      fName;
                Memory::VariantValue    fDetails;
            };


            /**
             *  TaskNotifier& parentTask, ProgressRangeType fromProg, ProgressRangeType toProg
             * DRAFT IDEA
             * just proxy objec you pass around for the thing that process that know their progress call to notify of changes
             */
            class   ProgressMonitor::TaskNotifier {
            public:
                /**
                 *  Use of the given TaskNotifier will generate 'setprogress' calls with appropriately scaled
                 *  progress values.
                 *
                 *  Helper used to continue reporting progress, but breaking the progress into subtasks, and doing the artithmatic of integrating the total into an overall progress total.
                 */
                TaskNotifier ();
                TaskNotifier (nullptr_t);
                TaskNotifier (const TaskNotifier& parentTask, ProgressRangeType fromProg, ProgressRangeType toProg);

            public:
                nonvirtual  void                SetProgress (ProgressRangeType p);

            public:
                /**
                 * Called from the context of a thread which has been given this progress object. This method will check
                 * if this progress object has been canceled, and in if so throw UserCanceledException.
                 *
                 * Note - this function does NOT check if the itself thread has been aborted (as that is usually taken care of automatically or via
                 * CheckForThreadAborting)
                 */
                nonvirtual  void    ThrowIfCanceled ();

            public:
                /**
                 * SetCurrentProgressAndThrowIfCanceled () overloads are handy helpers for code performing long-lived tasks to deal with being passed a
                 * null progress object (often) and do that check, and if non-null, then update the progress and check for cancelations.
                 */
                nonvirtual  void    SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress);

            public:
                /**
                 */
                nonvirtual  void    SetCurrentTaskInfo (const CurrentTaskInfo& taskInfo);

            private:
                nonvirtual  void    CallNotifyProgress_ () const;

            private:
                shared_ptr<IRep_>   fRep_;
                ProgressRangeType   fFromProg_;
                ProgressRangeType   fToProg_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ProgressMonitor.inl"

#endif  /*_Stroika_Foundation_Execution_ProgressMonitor_h_*/
