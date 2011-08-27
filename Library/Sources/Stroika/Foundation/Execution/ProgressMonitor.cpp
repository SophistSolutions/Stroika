/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"ProgressMonitor.h"






using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;





/*
 ********************************************************************************
 **************************** ProgressMontior ***********************************
 ********************************************************************************
 */
ProgressMontior::ProgressMontior ()
	: fCritSect_ ()
	, fCallbacks ()
	, fCanceled_ (false)
	, fCurrentProgress_ (0.0)
	, fCurrentTaskInfo_ ()
{
}

ProgressMontior::ProgressMontior (const Memory::RefCntPtr<ICallback>& callback)
	: fCritSect_ ()
	, fCallbacks ()
	, fCanceled_ (false)
	, fCurrentProgress_ (0.0)
	, fCurrentTaskInfo_ ()
{
	AddCallback (callback);
}

ProgressMontior::~ProgressMontior ()
{
}

void	ProgressMontior::AddCallback (const Memory::RefCntPtr<ICallback>& callback)
{
	AutoCriticalSection	enterCriticalSection (fCritSect_);
	fCallbacks.push_back (callback);
}

void	ProgressMontior::CallNotifyProgress_ () const
{
	AutoCriticalSection	enterCriticalSection (fCritSect_);
	for (vector<Memory::RefCntPtr<ICallback>>::const_iterator i = fCallbacks.begin (); i != fCallbacks.end (); ++i) {
		i->get ()->NotifyOfProgress (*this);
	}
}
