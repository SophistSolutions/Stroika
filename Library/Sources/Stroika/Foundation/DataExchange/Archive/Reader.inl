/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Archive::Reader {

    /*
     ********************************************************************************
     ********************* DataExchange::Archive::Reader::Ptr ***********************
     ********************************************************************************
     */
    inline Ptr::Ptr (const shared_ptr<IRep>& rep)
        : fRep_{rep}
    {
    }
    inline Ptr::Ptr (Ptr&& src)
        : fRep_{move (src.fRep_)}
    {
    }
    inline IRep& Ptr::_GetRep ()
    {
        EnsureNotNull (fRep_);
        return *fRep_;
    }
    inline const IRep& Ptr::_ConstGetRep () const
    {
        EnsureNotNull (fRep_);
        return *fRep_;
    }
    inline Set<String> Ptr::GetContainedFiles () const
    {
        return _ConstGetRep ().GetContainedFiles ();
    }
    inline Memory::BLOB Ptr::GetData (const String& fileName) const
    {
        return _ConstGetRep ().GetData (fileName);
    }

}
