/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
    : fRep_ (make_shared<Rep_> ())
{
}

ProgressMonitor::ProgressMonitor (Thread::Ptr workThread)
    : fRep_ (make_shared<Rep_> ())
{
}

ProgressMonitor::~ProgressMonitor ()
{
}

void ProgressMonitor::AddOnProgressCallback (const ChangedCallbackType& progressChangedCallback)
{
    RequireNotNull (fRep_);
    fRep_->fCallbacks_.Append (progressChangedCallback);
}

void ProgressMonitor::Cancel ()
{
    RequireNotNull (fRep_);
    fRep_->fCanceled_ = true;
    fRep_->fWorkThread_.Abort ();
}

/*
 ********************************************************************************
 *************************** ProgressMonitor::Updater ***************************
 ********************************************************************************
 */
void ProgressMonitor::Updater::CallNotifyProgress_ () const
{
    RequireNotNull (fRep_);
    for (ChangedCallbackType f : fRep_->fCallbacks_) {
#if qFoundation_Execution_Function_OperatorForwardNeedsRefBug
        ProgressMonitor p (fRep_);
        f (ref (p));
#else
        f (ProgressMonitor (fRep_));
#endif
    }
}
