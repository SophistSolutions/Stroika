/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_IdleManager_inl_
#define _Stroika_Framework_Led_IdleManager_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

   /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */

    //  class   Idler
    inline Idler::Idler ()
    {
    }

    //  class   EnterIdler
    inline EnterIdler::EnterIdler ()
    {
    }

    //  class   IdleManager::NonIdleContext
    inline IdleManager::NonIdleContext::NonIdleContext ()
    {
        IdleManager::Get ().SetInIdleMode (false);
        IdleManager::Get ().fNonIdleContextCount++;
    }
    inline IdleManager::NonIdleContext::~NonIdleContext ()
    {
        Assert (IdleManager::Get ().fNonIdleContextCount > 0);
        IdleManager::Get ().fNonIdleContextCount--;
    }

    //  class   IdleManager
    inline IdleManager::IdleManager ()
        : fInIdleMode (false)
        , fNonIdleContextCount (0)
        , fIdleManagerOSImpl (nullptr)
        , fIdlers ()
        , fNeedMgrIdleCalls (false)
        , fEnterIdlers ()
    {
    }
    inline IdleManager& IdleManager::Get ()
    {
        if (sThe == nullptr) {
            sThe = new IdleManager ();
        }
        return *sThe;
    }
    inline bool IdleManager::GetInIdleMode () const
    {
        return fInIdleMode;
    }
    inline void IdleManager::SetInIdleMode (bool inIdleMode)
    {
        bool effectiveInIdleMode = inIdleMode and fNonIdleContextCount == 0;
        bool enteringIdle        = not fInIdleMode and effectiveInIdleMode;
        fInIdleMode              = effectiveInIdleMode;
        if (enteringIdle) {
            CallEnterIdle ();
        }
    }

    //  class   IdleManager::IdleManagerOSImpl
    inline IdleManager::IdleManagerOSImpl::IdleManagerOSImpl ()
    {
    }

}

#endif /*_Stroika_Framework_Led_IdleManager_inl_*/
