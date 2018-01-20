/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_INI_Reader_inl_
#define _Stroika_Foundation_DataExchange_Variant_INI_Reader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace Variant {
                namespace INI {

                    /*
                     ********************************************************************************
                     ************************************ INI::Reader *******************************
                     ********************************************************************************
                     */
                    inline Profile Reader::ReadProfile (const Streams::InputStream<Memory::Byte>::Ptr& in)
                    {
                        return Convert (Read (in));
                    }
                    inline Profile Reader::ReadProfile (const Streams::InputStream<Characters::Character>::Ptr& in)
                    {
                        return Convert (Read (in));
                    }
                    inline Profile Reader::ReadProfile (istream& in)
                    {
                        return Convert (Read (in));
                    }
                    inline Profile Reader::ReadProfile (wistream& in)
                    {
                        return Convert (Read (in));
                    }
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_DataExchange_Variant_INI_Reader_inl_*/
