/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ProgressMonitor_inl_
#define _Stroika_Foundation_Execution_ProgressMonitor_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <atomic>

#include "../Debug/Assertions.h"
#include "../Math/Common.h"
#include "../Memory/BlockAllocated.h"
#include "../Streams/InputStreamDelegationHelper.h"

#include "Common.h"
#include "Synchronized.h"
#include "Thread.h"
#include "UserCanceledException.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ***************************** ProgressMonitor::Rep_ ****************************
     ********************************************************************************
     */
    class ProgressMonitor::Rep_ : public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    public:
        Rep_ () = default;

        Synchronized<Containers::Sequence<shared_ptr<ChangedCallbackType>>> fCallbacks_; // store shared_ptr in case mutable function - saving state - re-used across runs and not copied
        atomic<bool>                  fCanceled_{false};
        atomic<ProgressRangeType>     fCurrentProgress_{0.0};
        Synchronized<CurrentTaskInfo> fCurrentTaskInfo_;
        Thread::Ptr                   fWorkThread_; // optional - ignore if empty
    };

    /*
     ********************************************************************************
     *********************** ProgressMonitor::CurrentTaskInfo ***********************
     ********************************************************************************
     */
    inline ProgressMonitor::CurrentTaskInfo::CurrentTaskInfo (const Characters::String& taskName, const DataExchange::VariantValue& details)
        : fName{taskName}
        , fDetails{details}
    {
    }

    /*
     ********************************************************************************
     ***************************** ProgressMontitor *********************************
     ********************************************************************************
     */
    inline ProgressMonitor::ProgressMonitor (const shared_ptr<Rep_>& rep)
        : fRep_{rep}
    {
    }
    inline ProgressMonitor::ProgressRangeType ProgressMonitor::GetProgress () const
    {
        RequireNotNull (fRep_);
        Ensure (0.0 <= fRep_->fCurrentProgress_ and fRep_->fCurrentProgress_ <= 1.0);
        return fRep_->fCurrentProgress_;
    }
    inline ProgressMonitor::CurrentTaskInfo ProgressMonitor::GetCurrentTaskInfo () const
    {
        RequireNotNull (fRep_);
        return fRep_->fCurrentTaskInfo_;
    }
    inline ProgressMonitor::operator Updater ()
    {
        return Updater{fRep_};
    }

    /*
     ********************************************************************************
     *************************** ProgressMonitor::Updater ***************************
     ********************************************************************************
     */
    inline ProgressMonitor::Updater::Updater (nullptr_t)
    {
    }
    inline ProgressMonitor::Updater::Updater (const Updater& parentTask, ProgressRangeType fromProg, ProgressRangeType toProg, bool restoreTaskInfoOnDTOR)
        : fRep_{parentTask.fRep_}
        , fFromProg_{parentTask.fFromProg_ + fromProg * (parentTask.fToProg_ - parentTask.fFromProg_)}
        , fToProg_{parentTask.fFromProg_ + toProg * (parentTask.fToProg_ - parentTask.fFromProg_)}
    {
        fFromProg_ = Math::PinToSpecialPoint (fFromProg_, parentTask.fFromProg_);
        fToProg_   = Math::PinToSpecialPoint (fToProg_, parentTask.fToProg_);
        Require ((0.0f <= parentTask.fFromProg_) and (parentTask.fFromProg_ <= fFromProg_) and (fFromProg_ < fToProg_) and
                 (fToProg_ <= parentTask.fToProg_) and (parentTask.fToProg_ <= 1.0f));
        if (restoreTaskInfoOnDTOR and fRep_ != nullptr) {
            fRestoreTaskInfo_ = fRep_->fCurrentTaskInfo_.load ();
        }
    }
    inline ProgressMonitor::Updater::Updater (const Updater& parentTask, ProgressRangeType fromProg, ProgressRangeType toProg,
                                              const CurrentTaskInfo& taskInfo, bool restoreTaskInfoOnDTOR)
        : Updater{parentTask, fromProg, toProg, restoreTaskInfoOnDTOR}
    {
        SetCurrentTaskInfo (taskInfo);
    }
    inline ProgressMonitor::Updater::Updater (const shared_ptr<Rep_>& r)
        : fRep_{r}
    {
    }
    inline ProgressMonitor::Updater::~Updater ()
    {
        if (fRestoreTaskInfo_) {
            fRestoreTaskInfo_ = fRep_->fCurrentTaskInfo_.load ();
            CallNotifyProgress_ ();
        }
    }
    inline void ProgressMonitor::Updater::SetProgress (ProgressRangeType p)
    {
        ThrowIfCanceled ();
        if (fRep_ != nullptr) {
            WeakAssert (-0.001 < p and p < 1.001); // 'Weak Require' - outside this range, and its probably a caller bug
            p = Math::PinToSpecialPoint (Math::PinToSpecialPoint (p, 1.0f), 0.0f);
            Assert (0.0 <= p and p <= 1.0);
            p = fFromProg_ + p * (fToProg_ - fFromProg_);
            p = Math::PinToSpecialPoint (Math::PinToSpecialPoint (p, 1.0f), 0.0f);
            Assert (0.0 <= p and p <= 1.0);
            // pin-to-special-point to avoid floating point rounding errors triggering bogus assertions/progress changes
            p = Math::PinToSpecialPoint (p, fRep_->fCurrentProgress_.load ());
            // disallow moving progress backwards because it is nearly always a bug, and not terribly useful
            Require (p >= fRep_->fCurrentProgress_);
            if (fRep_->fCurrentProgress_.exchange (p) != p) {
                // only call if value changed - but always write so atomic update
                CallNotifyProgress_ ();
            }
        }
    }
    inline void ProgressMonitor::Updater::ThrowIfCanceled ()
    {
        if (fRep_ != nullptr and fRep_->fCanceled_) {
            if (fRep_->fWorkThread_ != nullptr) {
                fRep_->fWorkThread_.Abort ();
            }
            Throw (UserCanceledException::kThe);
        }
        Thread::CheckForInterruption ();
    }
    inline void ProgressMonitor::Updater::SetCurrentTaskInfo (const CurrentTaskInfo& taskInfo)
    {
        if (fRep_ != nullptr) {
            bool changed             = fRep_->fCurrentTaskInfo_ != taskInfo;
            fRep_->fCurrentTaskInfo_ = taskInfo;
            if (changed) {
                CallNotifyProgress_ ();
            }
        }
        ThrowIfCanceled ();
    }
    inline void ProgressMonitor::Updater::SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress)
    {
        if (fRep_ != nullptr) {
            SetProgress (currentProgress);
            ThrowIfCanceled ();
        }
    }

    /*
     ********************************************************************************
     ************************ MakeInputStreamWithProgress ***************************
     ********************************************************************************
     */
    template <typename T>
    Streams::InputStream::Ptr<T> MakeInputStreamWithProgress (const Streams::InputStream::Ptr<T>& in, ProgressMonitor::Updater progress)
    {
        struct inputStreamWithProgress : Streams::InputStreamDelegationHelper<T> {
            using inherited    = Streams::InputStreamDelegationHelper<T>;
            using ProgressType = ProgressMonitor::ProgressRangeType;
            inputStreamWithProgress (const Streams::InputStream::Ptr<T>& in, Execution::ProgressMonitor::Updater progress)
                : inherited{in}
                , fProgress_{progress}
                , fInitialSeek_{in.GetOffset ()}
                , fHighWaterMark_{fInitialSeek_}
                , fKnownEnd_{in.RemainingLength ()}
                , fEstimatedEnd_{static_cast<ProgressType> (fKnownEnd_.value_or (fInitialSeek_ + 1024))} // exponentially grow if we ever exceed
            {
            }
            // Intentionally ignore Seek, because that could be used to estimate total file size and anything contributing to progress must be an actual READ
            virtual optional<span<T>> Read (span<T> intoBuffer, Streams::NoDataAvailableHandling blockFlag) override
            {
                auto                    r      = inherited::fRealIn.Read (intoBuffer, blockFlag);
                Streams::SeekOffsetType curOff = inherited::fRealIn.GetOffset ();
                fHighWaterMark_                = max (curOff, fHighWaterMark_);
                if (not fKnownEnd_ and curOff > 0.75 * fEstimatedEnd_) {
                    fEstimatedEnd_ *= 1.5;
                }
                ProgressType progress =
                    static_cast<ProgressType> (fHighWaterMark_ - fInitialSeek_) / static_cast<ProgressType> (fEstimatedEnd_ - fInitialSeek_);
                if (progress > fLastProgressSent_) [[likely]] {
                    fProgress_.SetProgress (progress);
                    fLastProgressSent_ = progress;
                }
                return r;
            }
            ProgressMonitor::Updater          fProgress_;
            Streams::SeekOffsetType           fInitialSeek_;
            Streams::SeekOffsetType           fHighWaterMark_;
            optional<Streams::SeekOffsetType> fKnownEnd_;
            ProgressType                      fEstimatedEnd_;
            ProgressType                      fLastProgressSent_{0};
        };
        return Streams::InputStream::Ptr<T>{make_shared<inputStreamWithProgress> (in, progress)};
    }

}

#endif /*_Stroika_Foundation_Execution_ProgressMonitor_inl_*/
