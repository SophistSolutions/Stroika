/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ******************** DataExchange::BadFormatException **************************
     ********************************************************************************
     */
    inline Characters::String BadFormatException::GetDetails () const
    {
        return fDetails_;
    }
    inline void BadFormatException::GetPositionInfo (optional<unsigned int>* lineNum, optional<unsigned int>* colNumber, optional<uint64_t>* fileOffset) const
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

}
