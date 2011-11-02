/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<algorithm>

#include	"Led_IdleManager.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
namespace	Led {
#endif



/*
 ********************************************************************************
 ************************************ Idler *************************************
 ********************************************************************************
 */
void	Idler::SpendIdleTime ()
{
}





/*
 ********************************************************************************
 ******************************* EnterIdler *************************************
 ********************************************************************************
 */
void	EnterIdler::OnEnterIdle ()
{
}





/*
 ********************************************************************************
 **************************** IdleManager::IdlerInfo ****************************
 ********************************************************************************
 */
IdleManager::IdlerInfo::IdlerInfo ():
	fIdlerFrequency (IdleManager::kNeverCallIdler),
	fLastCalledAt (0.0f)
{
}





/*
 ********************************************************************************
 **************************** IdleManager::Cleanup ******************************
 ********************************************************************************
 */
IdleManager::Cleanup	sCleanup;

IdleManager::Cleanup::~Cleanup ()
{
	delete IdleManager::sThe;
	IdleManager::sThe = NULL;
}





/*
 ********************************************************************************
 ********************************** IdleManager *********************************
 ********************************************************************************
 */
IdleManager*	IdleManager::sThe				=	NULL;
float			IdleManager::kNeverCallIdler	=	100.0f;


void	IdleManager::AddIdler (Idler* idler)
{
	Led_RequireNotNil (idler);
	Led_Require (fIdlers.find (idler) == fIdlers.end ());
	IdlerInfo	idlerInfo;
	fIdlers.insert (map<Idler*, IdlerInfo>::value_type (idler, idlerInfo));
}

void	IdleManager::RemoveIdler (Idler* idler)
{
	Led_RequireNotNil (idler);
	map<Idler*, IdlerInfo>::iterator i	=	fIdlers.find (idler);
	#if		qBCCStaticVCLDTORLibBug
		{
			if (i == fIdlers.end ()) {
        			return;
			}
  		}
	#endif
	Led_Require (i != fIdlers.end ());
	Led_Assert (i->first == idler);
	fIdlers.erase (i);
}

void	IdleManager::AddEnterIdler (EnterIdler* enterIdler)
{
	Led_RequireNotNil (enterIdler);
	Led_Require (std::find (fEnterIdlers.begin (), fEnterIdlers.end (), enterIdler) == fEnterIdlers.end ());
	fEnterIdlers.push_back (enterIdler);
	UpdateIdleMgrImplState ();
}

void	IdleManager::RemoveEnterIdler (EnterIdler* enterIdler)
{
	Led_RequireNotNil (enterIdler);
	vector<EnterIdler*>::iterator i	=	std::find (fEnterIdlers.begin (), fEnterIdlers.end (), enterIdler);
	Led_Require (i != fEnterIdlers.end ());
	Led_Assert (*i == enterIdler);
	fEnterIdlers.erase (i);
	UpdateIdleMgrImplState ();
}

float	IdleManager::GetIdlerFrequncy (Idler* idler)
{
	Led_RequireNotNil (idler);
	map<Idler*, IdlerInfo>::iterator i	=	fIdlers.find (idler);
	Led_Require (i != fIdlers.end ());
	Led_Assert (i->first == idler);
	return i->second.fIdlerFrequency;
}

void	IdleManager::SetIdlerFrequncy (Idler* idler, float idlerFrequency)
{
	Led_RequireNotNil (idler);
	map<Idler*, IdlerInfo>::iterator i	=	fIdlers.find (idler);
	Led_Require (i != fIdlers.end ());
	Led_Assert (i->first == idler);
	if (i->second.fIdlerFrequency != idlerFrequency) {
		i->second.fIdlerFrequency = idlerFrequency;
		UpdateIdleMgrImplState ();
	}
}

void	IdleManager::UpdateIdleMgrImplState ()
{
	if (fIdleManagerOSImpl != NULL) {
		float	idleFreq			=	kNeverCallIdler;
		for (map<Idler*, IdlerInfo>::iterator i = fIdlers.begin (); i != fIdlers.end (); ++i) {
			idleFreq = min (idleFreq, i->second.fIdlerFrequency);
		}
		if (not fEnterIdlers.empty ()) {
			const float	kMinEnterIdleFreqCheck	=	0.5f;
			idleFreq = min (idleFreq, kMinEnterIdleFreqCheck);
		}
		bool	shouldNeedIdleMgr	=	(idleFreq != kNeverCallIdler);
		if (shouldNeedIdleMgr != fNeedMgrIdleCalls or (shouldNeedIdleMgr and idleFreq != fIdleManagerOSImpl->GetSuggestedFrequency ())) {
			fNeedMgrIdleCalls = shouldNeedIdleMgr;
			if (fNeedMgrIdleCalls) {
				fIdleManagerOSImpl->SetSuggestedFrequency (idleFreq);
				fIdleManagerOSImpl->StartSpendTimeCalls ();
			}
			else {
				fIdleManagerOSImpl->TerminateSpendTimeCalls ();
			}
		}
	}
}

void	IdleManager::CallSpendTime ()
{
	SetInIdleMode (true);	// not SURE this is the best place to call this - maybe SB called from OSREP only???
	float	now	=	Led_GetTickCount ();
	for (map<Idler*, IdlerInfo>::iterator i = fIdlers.begin (); i != fIdlers.end (); ++i) {
		// only call SpendTime if its been requested
		if (i->second.fLastCalledAt + i->second.fIdlerFrequency <= now) {
			Idler*	idler	=	i->first;
			idler->SpendIdleTime ();
			now = Led_GetTickCount ();		// update 'now' since we could have spent alot of time in 'SpendIdleTime'
			i->second.fLastCalledAt = now;
		}
	}
}

void	IdleManager::CallEnterIdle ()
{
	for (vector<EnterIdler*>::iterator i = fEnterIdlers.begin (); i != fEnterIdlers.end (); ++i) {
		EnterIdler*	enterIdler	=	*i;
		Led_AssertNotNil (enterIdler);
		enterIdler->OnEnterIdle ();
	}
}

void	IdleManager::SetIdleManagerOSImpl (IdleManagerOSImpl* impl)
{
	// If they are just setting the IMPL to NULL and sThe is NULL, then don't bother
	// creating it (cuz at destruction time - these destructors could get done in any order, 
	// and forcing a create here could create an artificail memory block left over that looks
	// to the memory leak detector - like a memory leak.
	if (impl == NULL) {
		if (sThe != NULL) {
			sThe->fIdleManagerOSImpl = NULL;
		}
	}
	else {
		Get ().fIdleManagerOSImpl = impl;
	}
}







/*
 ********************************************************************************
 ************************** IdleManager::IdleManagerOSImpl **********************
 ********************************************************************************
 */
void	IdleManager::IdleManagerOSImpl::CallSpendTime ()
{
	IdleManager::Get ().CallSpendTime ();
}




#if		qLedUsesNamespaces
}
#endif






// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
