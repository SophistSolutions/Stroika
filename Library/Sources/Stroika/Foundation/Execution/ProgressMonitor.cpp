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
    for (ProgressChangedCallbackType f : fRep_->fCallbacks_) {
        return f (ProgressMonitor (fRep_));
    }
}
