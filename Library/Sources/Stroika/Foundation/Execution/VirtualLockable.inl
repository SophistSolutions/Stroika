/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ****************************** VirtualLockable *********************************
     ********************************************************************************
     */
    template <Common::StdCompat::Lockable REAL_MUTEX>
    VirtualLockable VirtualLockable::Make ()
    {
        struct MyRep_ : IRep_ {
            MyRep_ ()
                : fMutex_{}
            {
            }
            virtual void lock () override
            {
                fMutex_.lock ();
            }
            virtual bool try_lock () override
            {
                return fMutex_.try_lock ();
            }
            virtual void unlock () override
            {
                fMutex_.unlock ();
            }
            REAL_MUTEX fMutex_;
        };
        return VirtualLockable{make_unique<MyRep_> ()};
    }
    inline VirtualLockable::VirtualLockable (unique_ptr<IRep_>&& rep)
        : fRep_{move (rep)}
    {
        RequireNotNull (fRep_);
    }
    inline void VirtualLockable::lock ()
    {
        fRep_->lock ();
    }
    inline bool VirtualLockable::try_lock ()
    {
        return fRep_->try_lock ();
    }
    inline void VirtualLockable::unlock ()
    {
        fRep_->unlock ();
    }

}
