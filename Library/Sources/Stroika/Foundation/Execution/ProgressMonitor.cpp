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

ProgressMonitor::ProgressMonitor (const ChangedCallbackType& progressChangedCallback)
    : fRep_ (new IRep_ ())
{
    AddOnProgressCallback (progressChangedCallback);
}

ProgressMonitor::~ProgressMonitor ()
{
}

void    ProgressMonitor::AddOnProgressCallback (const ChangedCallbackType& progressChangedCallback)
{
    RequireNotNull (fRep_);
    fRep_->fCallbacks_.Append (progressChangedCallback);
}


/*
 ********************************************************************************
 *************************** ProgressMonitor::Updater ***************************
 ********************************************************************************
 */
void    ProgressMonitor::Updater::CallNotifyProgress_ () const
{
    RequireNotNull (fRep_);
    for (ChangedCallbackType f : fRep_->fCallbacks_) {
        return f (ProgressMonitor (fRep_));
    }
}
