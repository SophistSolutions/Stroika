/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "ProgressMonitor.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Memory::MakeSharedPtr;

/*
 ********************************************************************************
 ******************************* ProgressMonitor ********************************
 ********************************************************************************
 */
ProgressMonitor::ProgressMonitor ()
    : fRep_{MakeSharedPtr<Rep_> ()}
{
}

ProgressMonitor::ProgressMonitor (Thread::Ptr workThread)
    : ProgressMonitor{}
{
    fRep_->fWorkThread_ = workThread;
}

ProgressMonitor::ProgressMonitor (ChangedCallbackType callback, Thread::Ptr workThread)
    : ProgressMonitor{workThread}
{
    AddOnProgressCallback (callback);
}

ProgressMonitor::ProgressMonitor (Traversal::Iterable<ChangedCallbackType> callbacks, Thread::Ptr workThread)
    : ProgressMonitor{workThread}
{
    for (auto i : callbacks) {
        AddOnProgressCallback (i);
    }
}

void ProgressMonitor::AddOnProgressCallback (const ChangedCallbackType& progressChangedCallback)
{
    RequireNotNull (fRep_);
    fRep_->fCallbacks_.rwget ().rwref ().Append (MakeSharedPtr<ChangedCallbackType> (progressChangedCallback));
}

void ProgressMonitor::Cancel ()
{
    RequireNotNull (fRep_);
    fRep_->fCanceled_ = true;
    if (fRep_->fWorkThread_ != nullptr) {
        fRep_->fWorkThread_.Abort ();
    }
}

/*
 ********************************************************************************
 *************************** ProgressMonitor::Updater ***************************
 ********************************************************************************
 */
void ProgressMonitor::Updater::CallNotifyProgress_ () const noexcept
{
    RequireNotNull (fRep_);
    for (shared_ptr<ChangedCallbackType> f : fRep_->fCallbacks_.load ()) {
        // NB: a throw from ChangedCallbackType is DOCUMENTED to be illegal and will result in 'unexpected' - cuz of noexcept on this function (even threadabort)
        (*f) (ProgressMonitor{fRep_}); // callbacks are noexcept, and should never hold a lock (or at least always guaraneed very short lived)
    }
}
