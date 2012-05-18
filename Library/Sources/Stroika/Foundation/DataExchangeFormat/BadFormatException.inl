/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_BadFormatException_inl_
#define _Stroika_Foundation_DataExchangeFormat_BadFormatException_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {

        inline  wstring DataExchangeFormat::BadFormatException::GetDetails () const
        {
            return fDetails_;
        }


        namespace   Execution {
            template    <>
            void    _NoReturn_  DoThrow (const DataExchangeFormat::BadFormatException& e2Throw);
        }

    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_BadFormatException_inl_*/
