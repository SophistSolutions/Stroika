/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "ProgressMonitor.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;



/*
 ********************************************************************************
 **************************** ProgressMonitor ***********************************
 ********************************************************************************
 */
ProgressMonitor::ProgressMonitor ()
    : fRep_ (new IRep_ ())
{
}

ProgressMonitor::ProgressMonitor (const ProgressChangedCallbackType& progressChangedCallback)
    : fRep_ (new IRep_ ())
{
    AddOnProgressCallback (progressChangedCallback);
}

ProgressMonitor::~ProgressMonitor ()
{
}

void    ProgressMonitor::AddOnProgressCallback (const ProgressChangedCallbackType& progressChangedCallback)
{
    RequireNotNull (fRep_);
    lock_guard<mutex> enterCriticalSection (fRep_->fCritSect_);
    fRep_->fCallbacks_.Append (progressChangedCallback);
}


/*
 ********************************************************************************
 ********************** ProgressMonitor::TaskNotifier ***************************
 ********************************************************************************
 */
void    ProgressMonitor::TaskNotifier::CallNotifyProgress_ () const
{
    RequireNotNull (fRep_);
    lock_guard<mutex> enterCriticalSection (fRep_->fCritSect_);
    for (ProgressChangedCallbackType f : fRep_->fCallbacks_) {
        // @todo - must fix arg to callback or find way to get back owning ProgressMonitor or make temporary one - thats really good enuf...
        //f (*this);
    }
}
