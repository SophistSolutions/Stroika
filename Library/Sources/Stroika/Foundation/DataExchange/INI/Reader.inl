/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_INI_Reader_inl_
#define _Stroika_Foundation_DataExchange_INI_Reader_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   INI {


                /*
                 ********************************************************************************
                 ************************************ INI::Reader *******************************
                 ********************************************************************************
                 */
                inline  Profile Reader::ReadProfile (const Streams::BinaryInputStream<>& in)
                {
                    return Convert (Read (in));
                }
                inline  Profile Reader::ReadProfile (const Streams::TextInputStream& in)
                {
                    return Convert (Read (in));
                }
                inline  Profile Reader::ReadProfile (istream& in)
                {
                    return Convert (Read (in));
                }
                inline  Profile Reader::ReadProfile (wistream& in)
                {
                    return Convert (Read (in));
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_INI_Reader_inl_*/
