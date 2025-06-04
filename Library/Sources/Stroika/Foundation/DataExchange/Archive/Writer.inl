/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Archive::Writer {

    /*
     ********************************************************************************
     ******************** DataExchange::Archive::Writer::Ptr ************************
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
        EnsureNotNull (fRep_.get ());
        return *fRep_;
    }
    inline const IRep& Ptr::_GetRep () const
    {
        EnsureNotNull (fRep_.get ());
        return *fRep_;
    }
    inline void Ptr::Add (const String& fileName, const Memory::BLOB& data)
    {
        #if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
        return _GetRep ().Add (fileName, span<const byte>{data});
        #else
        return _GetRep ().Add (fileName, span{data});
        #endif
    }

}
