/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Memory/BLOB.h"

namespace Stroika::Foundation::DataExchange::Compression {

    /*
     ********************************************************************************
     ********************* DataExchange::Compression::Reader ************************
     ********************************************************************************
     */
    inline Reader::Reader (const shared_ptr<IRep>& rep)
        : fRep_{rep}
    {
    }
    inline Reader::Reader (Reader&& src)
        : fRep_{move (src.fRep_)}
    {
    }
    inline Reader::IRep& Reader::_GetRep ()
    {
        EnsureNotNull (fRep_.get ());
        return *fRep_;
    }
    inline const Reader::IRep& Reader::_GetRep () const
    {
        EnsureNotNull (fRep_.get ());
        return *fRep_;
    }
    inline InputStream::Ptr<byte> Reader::Compress (const InputStream::Ptr<byte>& src) const
    {
        return _GetRep ().Compress (src);
    }
    inline BLOB Reader::Compress (const BLOB& src) const
    {
        return _GetRep ().Compress (src.As<InputStream::Ptr<byte>> ()).ReadAll ();
    }
    inline InputStream::Ptr<byte> Reader::Decompress (const InputStream::Ptr<byte>& src) const
    {
        return _GetRep ().Decompress (src);
    }
    inline BLOB Reader::Decompress (const BLOB& src) const
    {
        return _GetRep ().Decompress (src.As<InputStream::Ptr<byte>> ()).ReadAll ();
    }

}
