/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution::Resources {

    /*
     ********************************************************************************
     ************************************ Manager ***********************************
     ********************************************************************************
     */
    inline Manager::Manager (const shared_ptr<_IRep>& rep)
        : fRep_ (rep)
    {
    }
    inline Accessor Manager::ReadResource (const Name& name) const
    {
        return fRep_->ReadResource (name);
    }
    inline Accessor Manager::_mkAccessor (span<const byte> s)
    {
        return Accessor{s};
    }

}
