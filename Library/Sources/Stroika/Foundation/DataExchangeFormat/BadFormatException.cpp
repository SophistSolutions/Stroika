/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
using   namespace   Stroika::Foundation::Characters;


/*
 ********************************************************************************
 ****************** DataExchangeFormat::BadFormatException **********************
 ********************************************************************************
 */
namespace   {
    String mkMessage_OffsetInfo_ (Memory::Optional<unsigned int> lineNumber, Memory::Optional<unsigned int> columnNumber, Memory::Optional<uint64_t> fileOffset)
    {
        String result;
        if (not lineNumber.empty ()) {
            result += Format (L"Line %d", *lineNumber);
            if (not columnNumber.empty ()) {
                result += Format (L"; Column %d", *columnNumber);
            }
        }
        if (not fileOffset.empty ()) {
            if (not result.empty ()) {
                result += L"; ";
            }
            result += Format (L"; FileOffset %d", *fileOffset);
        }
        return result;
    }
    inline  String mkMessage_ ()
    {
        return L"Badly formatted input";
    }
    inline  String mkMessage_ (const String& details)
    {
        return details.empty () ? mkMessage_ () : details;
    }
    String mkMessage_ (Memory::Optional<unsigned int> lineNumber, Memory::Optional<unsigned int> columnNumber, Memory::Optional<uint64_t> fileOffset)
    {
        String msg             =   mkMessage_ ();
        String lineInfoExtra   =   mkMessage_OffsetInfo_ (lineNumber, columnNumber, fileOffset);
        if (not lineInfoExtra.empty ()) {
            msg += L" (" + lineInfoExtra + L").";
        }
        return msg;
    }
    String mkMessage_ (const String& details, Memory::Optional<unsigned int> lineNumber, Memory::Optional<unsigned int> columnNumber, Memory::Optional<uint64_t> fileOffset)
    {
        String msg             =   mkMessage_ (details);
        String lineInfoExtra   =   mkMessage_OffsetInfo_ (lineNumber, columnNumber, fileOffset);
        if (not lineInfoExtra.empty ()) {
            msg += L" (" + lineInfoExtra + L").";
        }
        return msg;
    }
}
DataExchangeFormat::BadFormatException::BadFormatException ()
    : inherited (mkMessage_ ())
    , fLineNumber_ ()
    , fColumnNumber_ ()
    , fFileOffset_ ()
    , fDetails_ ()
{
}

DataExchangeFormat::BadFormatException::BadFormatException (const String& details)
    : inherited (mkMessage_ (details))
    , fLineNumber_ ()
    , fColumnNumber_ ()
    , fFileOffset_ ()
    , fDetails_ (details)
{
}

DataExchangeFormat::BadFormatException::BadFormatException (const String& details, Memory::Optional<unsigned int> lineNumber, Memory::Optional<unsigned int> columnNumber, Memory::Optional<uint64_t> fileOffset)
    : inherited (mkMessage_ (details, lineNumber, columnNumber, fileOffset))
    , fLineNumber_ (lineNumber)
    , fColumnNumber_ (columnNumber)
    , fFileOffset_ (fileOffset)
    , fDetails_ (details)
{
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
    DbgTrace (L"Throwing exception: DataExchangeFormat::BadFormatException ('%s', LINE=%d, COL=%d)", e2Throw.GetDetails ().LimitLength (50).c_str (), useLineNum, useColNum);
#endif
    throw e2Throw;
}
