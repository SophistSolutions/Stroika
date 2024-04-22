/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Archive {

    /*
     ********************************************************************************
     ********************* DataExchange::Archive::Reader ****************************
     ********************************************************************************
     */
    inline Reader::Reader (const shared_ptr<_IRep>& rep)
        : fRep_{rep}
    {
    }
    inline Reader::Reader (Reader&& src)
        : fRep_{move (src.fRep_)}
    {
    }
    inline Reader::_IRep& Reader::_GetRep ()
    {
        EnsureNotNull (fRep_.get ());
        return *fRep_;
    }
    inline const Reader::_IRep& Reader::_GetRep () const
    {
        EnsureNotNull (fRep_.get ());
        return *fRep_;
    }
    inline Set<String> Reader::GetContainedFiles () const
    {
        return _GetRep ().GetContainedFiles ();
    }
    inline Memory::BLOB Reader::GetData (const String& fileName) const
    {
        return _GetRep ().GetData (fileName);
    }

}
