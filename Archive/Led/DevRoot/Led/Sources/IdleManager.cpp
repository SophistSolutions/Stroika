/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/IdleManager.cpp,v 2.6 2004/02/10 03:06:00 lewis Exp $
 *
 * Changes:
 *	$Log: IdleManager.cpp,v $
 *	Revision 2.6  2004/02/10 03:06:00  lewis
 *	SPR#1634: Added EnterIdler class to get notified on OnEnterIdle messages. Also - added IdleManager::NonIdleContext class to notify idle-manager of non-idle (eg executing a command) contexts (lots of these usages).
 *	
 *	Revision 2.5  2003/04/01 21:46:27  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  2003/03/28 12:58:36  lewis
 *	SPR#1388: destruction order of IdleManager and OSIdleManagerImpls is arbitrary - since
 *	done by static file scope DTORs. Could work around this other ways (with .h file with
 *	ref count based DTOR - as old days of iostream hack) - but easier to just make
 *	IdleManager::SetIdleManagerOSImpl static method and careful not to recreate IdleManager
 *	if setting impl property to NULL.
 *	
 *	Revision 2.3  2003/03/24 14:32:50  lewis
 *	Added qBCCStaticVCLDTORLibBug BCC bug workaround
 *	
 *	Revision 2.2  2003/03/21 14:57:25  lewis
 *	SPR#1368 - IdleManager had small memory leak
 *	
 *	Revision 2.1  2003/03/20 15:46:45  lewis
 *	SPR#1360- add new Idler and IdleManager classes
 *	
 *	
 *	
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<algorithm>

#include	"IdleManager.h"




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
