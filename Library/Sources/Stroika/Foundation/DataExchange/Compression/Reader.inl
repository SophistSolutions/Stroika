/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Compression_Reader_inl_
#define _Stroika_Foundation_DataExchange_Compression_Reader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::DataExchange::Compression {

    /*
     ********************************************************************************
     ********************* DataExchange::Compression::Reader ************************
     ********************************************************************************
     */
    inline Reader::Reader (const shared_ptr<_IRep>& rep)
        : fRep_ (rep)
    {
    }
    inline Reader::Reader (Reader&& src)
        : fRep_ (move (src.fRep_))
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
    inline InputStream<std::byte>::Ptr Reader::Compress (const InputStream<std::byte>::Ptr& src) const
    {
        return _GetRep ().Compress (src);
    }
    inline BLOB Reader::Compress (const BLOB& src) const
    {
        return _GetRep ().Compress (src.As<InputStream<std::byte>::Ptr> ()).ReadAll ();
    }
    inline InputStream<std::byte>::Ptr Reader::Decompress (const InputStream<std::byte>::Ptr& src) const
    {
        return _GetRep ().Decompress (src);
    }
    inline BLOB Reader::Decompress (const BLOB& src) const
    {
        return _GetRep ().Decompress (src.As<InputStream<std::byte>::Ptr> ()).ReadAll ();
    }

}

#endif /*_Stroika_Foundation_DataExchange_Compression_Reader_inl_*/
