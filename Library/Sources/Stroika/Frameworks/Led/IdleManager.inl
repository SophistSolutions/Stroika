/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ****************************** NonIdleContext **********************************
     ********************************************************************************
     */
    inline IdleManager::NonIdleContext::NonIdleContext ()
    {
        IdleManager::Get ().SetInIdleMode (false);
        ++IdleManager::Get ().fNonIdleContextCount;
    }
    inline IdleManager::NonIdleContext::~NonIdleContext ()
    {
        Assert (IdleManager::Get ().fNonIdleContextCount > 0);
        --IdleManager::Get ().fNonIdleContextCount;
    }

    /*
     ********************************************************************************
     ********************************* IdleManager **********************************
     ********************************************************************************
     */
    inline IdleManager::IdleManager ()
        : fNonIdleContextCount{0}
        , fIdleManagerOSImpl{nullptr}
        , fIdlers{}
        , fNeedMgrIdleCalls{false}
        , fEnterIdlers{}
    {
    }
    inline IdleManager& IdleManager::Get ()
    {
        static IdleManager sThe_;
        return sThe_;
    }
    inline bool IdleManager::GetInIdleMode () const
    {
        return fInIdleMode_;
    }
    inline void IdleManager::SetInIdleMode (bool inIdleMode)
    {
        bool effectiveInIdleMode = inIdleMode and fNonIdleContextCount == 0;
        bool enteringIdle        = not fInIdleMode_ and effectiveInIdleMode;
        fInIdleMode_             = effectiveInIdleMode;
        if (enteringIdle) {
            CallEnterIdle ();
        }
    }

}
