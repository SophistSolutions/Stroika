/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "ProgressMonitor.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 **************************** ProgressMonitor ***********************************
 ********************************************************************************
 */
ProgressMonitor::ProgressMonitor ()
    : fRep_{make_shared<Rep_> ()}
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
    fRep_->fCallbacks_.rwget ().rwref ().Append (make_shared<ChangedCallbackType> (progressChangedCallback));
}

void ProgressMonitor::Cancel ()
{
    RequireNotNull (fRep_);
    fRep_->fCanceled_ = true;
    fRep_->fWorkThread_.Abort ();
}

ProgressMonitor::operator Updater ()
{

    return Updater{fRep_};
}

/*
 ********************************************************************************
 *************************** ProgressMonitor::Updater ***************************
 ********************************************************************************
 */
void ProgressMonitor::Updater::CallNotifyProgress_ () const
{
    RequireNotNull (fRep_);
    for (shared_ptr<ChangedCallbackType> f : fRep_->fCallbacks_.load ()) {
        (*f) (ProgressMonitor{fRep_});
    }
}
