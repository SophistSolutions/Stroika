/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_BadFormatException_inl_
#define _Stroika_Foundation_DataExchange_BadFormatException_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {


        /*
         ********************************************************************************
         ******************** DataExchange::BadFormatException ********************
         ********************************************************************************
         */
        inline  Characters::String DataExchange::BadFormatException::GetDetails () const
        {
            return fDetails_;
        }
        inline  void    DataExchange::BadFormatException::GetPositionInfo (Memory::Optional<unsigned int>* lineNum, Memory::Optional<unsigned int>* colNumber, Memory::Optional<uint64_t>* fileOffset) const
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
            void    _NoReturn_  DoThrow (const DataExchange::BadFormatException& e2Throw);
        }


    }
}
#endif  /*_Stroika_Foundation_DataExchange_BadFormatException_inl_*/
