/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_ArchiveReader_inl_
#define _Stroika_Foundation_DataExchange_ArchiveReader_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /*
             ********************************************************************************
             ************************ DataExchange::ArchiveReader ***************************
             ********************************************************************************
             */
            inline  ArchiveReader::ArchiveReader (const shared_ptr<_IRep>& rep)
                : fRep_ (rep)
            {
            }
            inline  ArchiveReader::_IRep&   ArchiveReader::_GetRep ()
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            inline  const ArchiveReader::_IRep&   ArchiveReader::_GetRep () const
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            inline  Set<String>     ArchiveReader::GetContainedFiles () const
            {
                return _GetRep ().GetContainedFiles ();
            }
            inline  Memory::BLOB    ArchiveReader::GetData (const String& fileName) const
            {
                return _GetRep ().GetData (fileName);
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchange_ArchiveReader_inl_*/
