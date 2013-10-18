/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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


        /*
         ********************************************************************************
         ******************** DataExchangeFormat::BadFormatException ********************
         ********************************************************************************
         */
        inline  Characters::String DataExchangeFormat::BadFormatException::GetDetails () const
        {
            return fDetails_;
        }
        inline  void    DataExchangeFormat::BadFormatException::GetPositionInfo (Memory::Optional<unsigned int>* lineNum, Memory::Optional<unsigned int>* colNumber, Memory::Optional<uint64_t>* fileOffset) const
        {
            if (lineNum != nullptr) {
                *lineNum = fLineNumber_;
            }
            if (colNumber != nullptr) {
                *colNumber = fColumnNumber_;
            }
            if (fileOffset != nullptr) {
                *fileOffset = fFileOffset_;
            }
        }


        namespace   Execution {
            template    <>
            void    _NoReturn_  DoThrow (const DataExchangeFormat::BadFormatException& e2Throw);
        }


    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_BadFormatException_inl_*/
