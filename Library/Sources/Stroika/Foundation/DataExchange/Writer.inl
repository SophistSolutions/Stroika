/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Writer_inl_
#define _Stroika_Foundation_DataExchange_Writer_inl_  1


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
             ****************** DataExchange::Writer::_Rep_Cloner ***************************
             ********************************************************************************
             */
            inline   Writer::_SharedPtrIRep   Writer::_Rep_Cloner::Copy (const _IRep& t)
            {
                return t.Clone ();
            }


            /*
             ********************************************************************************
             ******************************* DataExchange::Writer ***************************
             ********************************************************************************
             */
            inline  Writer::Writer (shared_ptr<_IRep> rep)
                : fRep_ (rep)
            {
            }
            inline  String  Writer::GetDefaultFileSuffix () const
            {
                return fRep_->GetDefaultFileSuffix ();
            }
            inline  void    Writer::Write (const VariantValue& v, const Streams::BinaryOutputStream& out)
            {
                fRep_->Write (v, out);
            }
            inline  void    Writer::Write (const VariantValue& v, const Streams::TextOutputStream& out)
            {
                fRep_->Write (v, out);
            }
            inline  Writer::_IRep&   Writer::_GetRep ()
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            inline  const Writer::_IRep&   Writer::_GetRep () const
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_Writer_inl_*/
