/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_VariantReader_inl_
#define _Stroika_Foundation_DataExchange_VariantReader_inl_  1


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
             *************** DataExchange::VariantReader::_Rep_Cloner ***********************
             ********************************************************************************
             */
            inline   VariantReader::_SharedPtrIRep   VariantReader::_Rep_Cloner::Copy (const _IRep& t)
            {
                return t.Clone ();
            }


            /*
             ********************************************************************************
             *************************** DataExchange::VariantReader ************************
             ********************************************************************************
             */
            inline  VariantReader::VariantReader (const shared_ptr<_IRep>& rep)
                : fRep_ (rep)
            {
            }
            inline  String  VariantReader::GetDefaultFileSuffix () const
            {
                return fRep_->GetDefaultFileSuffix ();
            }
            inline  VariantValue    VariantReader::Read (const Streams::InputStream<Memory::Byte>& in)
            {
                return fRep_->Read (in);
            }
            inline  VariantValue    VariantReader::Read (const Streams::InputStream<Characters::Character>& in)
            {
                return fRep_->Read (in);
            }
            inline  VariantReader::_IRep&   VariantReader::_GetRep ()
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            inline  const VariantReader::_IRep&   VariantReader::_GetRep () const
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchange_VariantReader_inl_*/
