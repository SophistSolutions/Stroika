/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_Reader_inl_
#define _Stroika_Foundation_DataExchange_Variant_Reader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace Variant {

                /*
                 ********************************************************************************
                 ********************* DataExchange::Reader::_Rep_Cloner ************************
                 ********************************************************************************
                 */
                inline Reader::_SharedPtrIRep Reader::_Rep_Cloner::Copy (const _IRep& t)
                {
                    return t.Clone ();
                }

                /*
                 ********************************************************************************
                 ******************************* DataExchange::Reader ***************************
                 ********************************************************************************
                 */
                inline Reader::Reader (const shared_ptr<_IRep>& rep)
                    : fRep_ (rep)
                {
                }
                inline String Reader::GetDefaultFileSuffix () const
                {
                    return fRep_->GetDefaultFileSuffix ();
                }
                inline VariantValue Reader::Read (const Streams::InputStream<Memory::Byte>::Ptr& in)
                {
                    return fRep_->Read (in);
                }
                inline VariantValue Reader::Read (const Streams::InputStream<Characters::Character>::Ptr& in)
                {
                    return fRep_->Read (in);
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
            }
        }
    }
}
#endif /*_Stroika_Foundation_DataExchange_Variant_Reader_inl_*/
