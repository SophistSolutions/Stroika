/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_Writer_inl_
#define _Stroika_Foundation_DataExchangeFormat_Writer_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            /*
             ********************************************************************************
             ************************* DataExchangeFormat::Writer ***************************
             ********************************************************************************
             */
            inline  Writer::Writer (shared_ptr<_IRep> rep)
                : fRep_ (rep)
            {
            }
            inline  void    Writer::Write (const Memory::VariantValue& v, const Streams::BinaryOutputStream& out)
            {
                fRep_->Write (v, out);
            }
            inline  void    Writer::Write (const Memory::VariantValue& v, const Streams::TextOutputStream& out)
            {
                fRep_->Write (v, out);
            }
            inline  shared_ptr<Writer::_IRep>   Writer::_GetRep () const
            {
                return fRep_;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_Writer_inl_*/
