/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"

#include    "BadFormatException.h"


/*
 * Design Note:
 *
 *      o   Chose to include lineNumber etc stuff in message by default because in apps where this is not desired - (fancy gui apps) they are
 *          more likely to be the ones to override the exception mapping to message anyhow, and tune it themselves. Its the simple apps
 *          that do little but dump the string representations of a message that are more likely to want to know the line number (a bit of a guess).
 */


using   namespace   Stroika::Foundation;


/*
 ********************************************************************************
 ****************** DataExchangeFormat::BadFormatException **********************
 ********************************************************************************
 */
namespace   {
    wstring mkMessage_OffsetInfo_ (Memory::Optional<unsigned int> lineNumber, Memory::Optional<unsigned int> columnNumber, Memory::Optional<uint64_t> fileOffset)
    {
        wstring result;
        if (not lineNumber.empty ()) {
            result += Characters::Format (L"Line %d", *lineNumber);
            if (not columnNumber.empty ()) {
                result += Characters::Format (L"; Column %d", *columnNumber);
            }
        }
        if (not fileOffset.empty ()) {
            if (not result.empty ()) {
                result += L"; ";
            }
            result += Characters::Format (L"; FileOffset %d", *fileOffset);
        }
        return result;
    }
    wstring mkMessage_ ()
    {
        return L"Badly formatted input";
    }
    wstring mkMessage_ (const wstring& details)
    {
        return details.empty () ? mkMessage_ () : details;
    }
    wstring mkMessage_ (Memory::Optional<unsigned int> lineNumber, Memory::Optional<unsigned int> columnNumber, Memory::Optional<uint64_t> fileOffset)
    {
        wstring msg             =   mkMessage_ ();
        wstring lineInfoExtra   =   mkMessage_OffsetInfo_ (lineNumber, columnNumber, fileOffset);
        if (not lineInfoExtra.empty ()) {
            msg += L" (" + lineInfoExtra + L").";
        }
        return msg;
    }
    wstring mkMessage_ (const wstring& details, Memory::Optional<unsigned int> lineNumber, Memory::Optional<unsigned int> columnNumber, Memory::Optional<uint64_t> fileOffset)
    {
        wstring msg             =   mkMessage_ (details);
        wstring lineInfoExtra   =   mkMessage_OffsetInfo_ (lineNumber, columnNumber, fileOffset);
        if (not lineInfoExtra.empty ()) {
            msg += L" (" + lineInfoExtra + L").";
        }
        return msg;
    }
}
DataExchangeFormat::BadFormatException::BadFormatException ()
    : StringException (mkMessage_ ())
    , fDetails_ ()
    , fLineNumber ()
    , fColumnNumber ()
    , fFileOffset ()
{
}

DataExchangeFormat::BadFormatException::BadFormatException (const wstring& details)
    : StringException (mkMessage_ (details))
    , fDetails_ (details)
    , fLineNumber ()
    , fColumnNumber ()
    , fFileOffset ()
{
}

DataExchangeFormat::BadFormatException::BadFormatException (const wstring& details, Memory::Optional<unsigned int> lineNumber, Memory::Optional<unsigned int> columnNumber, Memory::Optional<uint64_t> fileOffset)
    : StringException (mkMessage_ (details, lineNumber, columnNumber, fileOffset))
    , fDetails_ (details)
    , fLineNumber (lineNumber)
    , fColumnNumber (columnNumber)
    , fFileOffset (fileOffset)
{
}

void    DataExchangeFormat::BadFormatException::GetPositionInfo (Memory::Optional<unsigned int>* lineNum, Memory::Optional<unsigned int>* colNumber, Memory::Optional<uint64_t>* fileOffset) const
{
    if (lineNum != nullptr) {
        *lineNum = fLineNumber;
    }
    if (colNumber != nullptr) {
        *colNumber = fColumnNumber;
    }
    if (fileOffset != nullptr) {
        *fileOffset = fFileOffset;
    }
}









template    <>
void    _NoReturn_  Execution::DoThrow (const DataExchangeFormat::BadFormatException& e2Throw)
{
#if     qDefaultTracingOn
    Memory::Optional<unsigned int>  lineNum;
    Memory::Optional<unsigned int>  colNumber;
    Memory::Optional<uint64_t>      fileOffset;
    e2Throw.GetPositionInfo (&lineNum, &colNumber, &fileOffset);
    int useLineNum  =   lineNum.empty () ? -1 : *lineNum;
    int useColNum   =   colNumber.empty () ? -1 : *colNumber;
    DbgTrace (L"Throwing exception: DataExchangeFormat::BadFormatException ('%s', LINE=%d, COL=%d)", e2Throw.GetDetails ().c_str (), useLineNum, useColNum);
#endif
    throw e2Throw;
}
