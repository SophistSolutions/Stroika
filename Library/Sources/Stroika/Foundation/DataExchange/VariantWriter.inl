/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_VariantWriter_inl_
#define _Stroika_Foundation_DataExchange_VariantWriter_inl_  1


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
             ****************** DataExchange::VariantWriter::_Rep_Cloner ********************
             ********************************************************************************
             */
            inline   VariantWriter::_SharedPtrIRep   VariantWriter::_Rep_Cloner::Copy (const _IRep& t)
            {
                return t.Clone ();
            }


            /*
             ********************************************************************************
             ************************ DataExchange::VariantWriter ***************************
             ********************************************************************************
             */
            inline  VariantWriter::VariantWriter (const shared_ptr<_IRep>& rep)
                : fRep_ (rep)
            {
            }
            inline  String  VariantWriter::GetDefaultFileSuffix () const
            {
                return fRep_->GetDefaultFileSuffix ();
            }
            inline  void    VariantWriter::Write (const VariantValue& v, const Streams::OutputStream<Memory::Byte>& out)
            {
                fRep_->Write (v, out);
            }
            inline  void    VariantWriter::Write (const VariantValue& v, const Streams::OutputStream<Characters::Character>& out)
            {
                fRep_->Write (v, out);
            }
            inline  VariantWriter::_IRep&   VariantWriter::_GetRep ()
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            inline  const VariantWriter::_IRep&   VariantWriter::_GetRep () const
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_VariantWriter_inl_*/
