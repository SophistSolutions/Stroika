/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_IdleManager_h_
#define _Stroika_Frameworks_Led_IdleManager_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    IdleManager
@DESCRIPTION:   <p>Note that this implementation is for SYNCRONOUS, and not THREADED idle time
            processing.
            </p>

 */

#include <map>
#include <vector>

#include "Support.h"

namespace Stroika::Frameworks::Led {

    /*
    @CLASS:         Idler
    @DESCRIPTION:   <p>Simple mixin interface you inherit from if you're object would like to be
                ticked (notified periodically) at idle time.
                </p>
    */
    class Idler {
    protected:
        Idler () = default;

    public:
        virtual void SpendIdleTime ();
    };

    /*
    @CLASS:         EnterIdler
    @DESCRIPTION:   <p></p>
    */
    class EnterIdler {
    protected:
        EnterIdler () = default;

    public:
        virtual void OnEnterIdle ();
    };

    /*
    @CLASS:         IdleManager
    @DESCRIPTION:   <p>
                </p>
    */
    class IdleManager {
    private:
        IdleManager ();

    public:
        static IdleManager& Get ();

    public:
        virtual void AddIdler (Idler* idler);
        virtual void RemoveIdler (Idler* idler);

    public:
        virtual void AddEnterIdler (EnterIdler* enterIdler);
        virtual void RemoveEnterIdler (EnterIdler* enterIdler);

    public:
        nonvirtual bool GetInIdleMode () const;
        nonvirtual void SetInIdleMode (bool inIdleMode);

    private:
        bool fInIdleMode_{false};

    public:
        static inline Foundation::Time::DurationSeconds kNeverCallIdler{100s}; // @todo crazy never value - switch to using optional<>
        virtual Foundation::Time::DurationSeconds       GetIdlerFrequncy (Idler* idler);
        virtual void                                    SetIdlerFrequncy (Idler* idler, Foundation::Time::DurationSeconds idlerFrequency);

    public:
        class NonIdleContext;

    protected:
        unsigned int fNonIdleContextCount;

    private:
        friend class NonIdleContext;

    public:
        class IdleManagerOSImpl;

    public:
        static void SetIdleManagerOSImpl (IdleManagerOSImpl* impl);

    private:
        IdleManagerOSImpl* fIdleManagerOSImpl;

    private:
        friend class IdleManagerOSImpl;

    private:
        nonvirtual void UpdateIdleMgrImplState ();

    protected:
        virtual void CallSpendTime ();
        virtual void CallEnterIdle ();

    private:
        struct IdlerInfo {
            IdlerInfo () = default;
            Foundation::Time::DurationSeconds  fIdlerFrequency{IdleManager::kNeverCallIdler};
            Foundation::Time::TimePointSeconds fLastCalledAt{};
        };
        map<Idler*, IdlerInfo> fIdlers;
        bool                   fNeedMgrIdleCalls;

    private:
        vector<EnterIdler*> fEnterIdlers;
    };

    /*
    @CLASS:         IdleManager::NonIdleContext
    @DESCRIPTION:   <p></p>
    */
    class IdleManager::NonIdleContext {
    public:
        NonIdleContext ();
        ~NonIdleContext ();
    };

    /*
    @CLASS:         IdleManager::IdleManagerOSImpl
    @ACCESS:        public
    @DESCRIPTION:   <p>
                </p>
    */
    class IdleManager::IdleManagerOSImpl {
    protected:
        IdleManagerOSImpl () = default;

    public:
        virtual void                              StartSpendTimeCalls ()                                                       = 0;
        virtual void                              TerminateSpendTimeCalls ()                                                   = 0;
        virtual Foundation::Time::DurationSeconds GetSuggestedFrequency () const                                               = 0;
        virtual void                              SetSuggestedFrequency (Foundation::Time::DurationSeconds suggestedFrequency) = 0;

    protected:
        virtual void CallSpendTime ();
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IdleManager.inl"

#endif /*_Stroika_Frameworks_Led_IdleManager_h_*/
