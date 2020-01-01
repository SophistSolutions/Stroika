/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include <algorithm>

#include "../../Foundation/Time/Realtime.h"

#include "IdleManager.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ************************************ Idler *************************************
 ********************************************************************************
 */
void Idler::SpendIdleTime ()
{
}

/*
 ********************************************************************************
 ******************************* EnterIdler *************************************
 ********************************************************************************
 */
void EnterIdler::OnEnterIdle ()
{
}

/*
 ********************************************************************************
 **************************** IdleManager::IdlerInfo ****************************
 ********************************************************************************
 */
IdleManager::IdlerInfo::IdlerInfo ()
    : fIdlerFrequency (IdleManager::kNeverCallIdler)
    , fLastCalledAt (0.0f)
{
}

/*
 ********************************************************************************
 **************************** IdleManager::Cleanup ******************************
 ********************************************************************************
 */
IdleManager::Cleanup sCleanup;

IdleManager::Cleanup::~Cleanup ()
{
    delete IdleManager::sThe;
    IdleManager::sThe = nullptr;
}

/*
 ********************************************************************************
 ********************************** IdleManager *********************************
 ********************************************************************************
 */
IdleManager*              IdleManager::sThe            = nullptr;
Time::DurationSecondsType IdleManager::kNeverCallIdler = 100.0f;

void IdleManager::AddIdler (Idler* idler)
{
    RequireNotNull (idler);
    Require (fIdlers.find (idler) == fIdlers.end ());
    IdlerInfo idlerInfo;
    fIdlers.insert (map<Idler*, IdlerInfo>::value_type (idler, idlerInfo));
}

void IdleManager::RemoveIdler (Idler* idler)
{
    RequireNotNull (idler);
    map<Idler*, IdlerInfo>::iterator i = fIdlers.find (idler);
#if qBCCStaticVCLDTORLibBug
    {
        if (i == fIdlers.end ()) {
            return;
        }
    }
#endif
    Require (i != fIdlers.end ());
    Assert (i->first == idler);
    fIdlers.erase (i);
}

void IdleManager::AddEnterIdler (EnterIdler* enterIdler)
{
    RequireNotNull (enterIdler);
    Require (find (fEnterIdlers.begin (), fEnterIdlers.end (), enterIdler) == fEnterIdlers.end ());
    fEnterIdlers.push_back (enterIdler);
    UpdateIdleMgrImplState ();
}

void IdleManager::RemoveEnterIdler (EnterIdler* enterIdler)
{
    RequireNotNull (enterIdler);
    vector<EnterIdler*>::iterator i = find (fEnterIdlers.begin (), fEnterIdlers.end (), enterIdler);
    Require (i != fEnterIdlers.end ());
    Assert (*i == enterIdler);
    fEnterIdlers.erase (i);
    UpdateIdleMgrImplState ();
}

Time::DurationSecondsType IdleManager::GetIdlerFrequncy (Idler* idler)
{
    RequireNotNull (idler);
    map<Idler*, IdlerInfo>::iterator i = fIdlers.find (idler);
    Require (i != fIdlers.end ());
    Assert (i->first == idler);
    return i->second.fIdlerFrequency;
}

void IdleManager::SetIdlerFrequncy (Idler* idler, Time::DurationSecondsType idlerFrequency)
{
    RequireNotNull (idler);
    map<Idler*, IdlerInfo>::iterator i = fIdlers.find (idler);
    Require (i != fIdlers.end ());
    Assert (i->first == idler);
    if (i->second.fIdlerFrequency != idlerFrequency) {
        i->second.fIdlerFrequency = idlerFrequency;
        UpdateIdleMgrImplState ();
    }
}

void IdleManager::UpdateIdleMgrImplState ()
{
    if (fIdleManagerOSImpl != nullptr) {
        Time::DurationSecondsType idleFreq = kNeverCallIdler;
        for (auto i = fIdlers.begin (); i != fIdlers.end (); ++i) {
            idleFreq = min (idleFreq, i->second.fIdlerFrequency);
        }
        if (not fEnterIdlers.empty ()) {
            const Time::DurationSecondsType kMinEnterIdleFreqCheck = 0.5f;
            idleFreq                                               = min (idleFreq, kMinEnterIdleFreqCheck);
        }
        bool shouldNeedIdleMgr = (idleFreq != kNeverCallIdler);
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

void IdleManager::CallSpendTime ()
{
    SetInIdleMode (true); // not SURE this is the best place to call this - maybe SB called from OSREP only???
    Foundation::Time::DurationSecondsType now = Time::GetTickCount ();
    for (auto i = fIdlers.begin (); i != fIdlers.end (); ++i) {
        // only call SpendTime if its been requested
        if (i->second.fLastCalledAt + i->second.fIdlerFrequency <= now) {
            Idler* idler = i->first;
            idler->SpendIdleTime ();
            now                     = Time::GetTickCount (); // update 'now' since we could have spent alot of time in 'SpendIdleTime'
            i->second.fLastCalledAt = now;
        }
    }
}

void IdleManager::CallEnterIdle ()
{
    for (auto i = fEnterIdlers.begin (); i != fEnterIdlers.end (); ++i) {
        EnterIdler* enterIdler = *i;
        AssertNotNull (enterIdler);
        enterIdler->OnEnterIdle ();
    }
}

void IdleManager::SetIdleManagerOSImpl (IdleManagerOSImpl* impl)
{
    // If they are just setting the IMPL to nullptr and sThe is nullptr, then don't bother
    // creating it (cuz at destruction time - these destructors could get done in any order,
    // and forcing a create here could create an artificail memory block left over that looks
    // to the memory leak detector - like a memory leak.
    if (impl == nullptr) {
        if (sThe != nullptr) {
            sThe->fIdleManagerOSImpl = nullptr;
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
void IdleManager::IdleManagerOSImpl::CallSpendTime ()
{
    IdleManager::Get ().CallSpendTime ();
}
