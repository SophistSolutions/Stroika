/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Reader_inl_
#define _Stroika_Foundation_DataExchange_Reader_inl_  1


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
             ************ DataExchange::Reader::_Rep_Cloner *********************************
             ********************************************************************************
             */
            inline   Reader::_SharedPtrIRep   Reader::_Rep_Cloner::Copy (const _IRep& t)
            {
                return t.Clone ();
            }


            /*
             ********************************************************************************
             ******************************* DataExchange::Reader ***************************
             ********************************************************************************
             */
            inline  Reader::Reader (shared_ptr<_IRep> rep)
                : fRep_ (rep)
            {
            }
            inline  String  Reader::GetDefaultFileSuffix () const
            {
                return fRep_->GetDefaultFileSuffix ();
            }
            inline  VariantValue    Reader::Read (const Streams::BinaryInputStream& in)
            {
                return fRep_->Read (in);
            }
            inline  VariantValue    Reader::Read (const Streams::TextInputStream& in)
            {
                return fRep_->Read (in);
            }
            inline  Reader::_IRep&   Reader::_GetRep ()
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            inline  const Reader::_IRep&   Reader::_GetRep () const
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchange_Reader_inl_*/
