/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Characters/String_Constant.h"

#include    "BadFormatException.h"


/*
 * Design Note:
 *
 *      o   Chose to include lineNumber etc stuff in message by default because in apps where this is not desired -
 *          (fancy gui apps) they are more likely to be the ones to override the exception mapping to message anyhow,
 *          and tune it themselves. Its the simple apps that do little but dump the string representations of a
 *          message that are more likely to want to know the line number (a bit of a guess).
 */


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;

using   Memory::Optional;



/*
 ********************************************************************************
 ************************ DataExchange::BadFormatException **********************
 ********************************************************************************
 */
namespace   {
    String mkMessage_OffsetInfo_ (const Optional<unsigned int>& lineNumber, const Optional<unsigned int>& columnNumber, const Optional<uint64_t>& fileOffset)
    {
        String result;
        if (not lineNumber.IsMissing ()) {
            result += Format (L"Line %d", *lineNumber);
            if (not columnNumber.IsMissing ()) {
                result += Format (L"; Column %d", *columnNumber);
            }
        }
        if (not fileOffset.IsMissing ()) {
            if (not result.empty ()) {
                result += String_Constant (L"; ");
            }
            result += Format (L"; FileOffset %d", *fileOffset);
        }
        return result;
    }
    inline  String mkMessage_ ()
    {
        return String_Constant (L"Badly formatted input");
    }
    inline  String mkMessage_ (const String& details)
    {
        return details.empty () ? mkMessage_ () : details;
    }
    String mkMessage_ (const Optional<unsigned int>& lineNumber, const Optional<unsigned int>& columnNumber, Optional<uint64_t> fileOffset)
    {
        String msg             =   mkMessage_ ();
        String lineInfoExtra   =   mkMessage_OffsetInfo_ (lineNumber, columnNumber, fileOffset);
        if (not lineInfoExtra.empty ()) {
            msg += Characters::String_Constant (L" (") + lineInfoExtra + String_Constant (L").");
        }
        return msg;
    }
    String mkMessage_ (const String& details, const Optional<unsigned int>& lineNumber, const Optional<unsigned int>& columnNumber, const Optional<uint64_t>& fileOffset)
    {
        String msg             =   mkMessage_ (details);
        String lineInfoExtra   =   mkMessage_OffsetInfo_ (lineNumber, columnNumber, fileOffset);
        if (not lineInfoExtra.empty ()) {
            msg += Characters::String_Constant (L" (") + lineInfoExtra + String_Constant (L").");
        }
        return msg;
    }
}
DataExchange::BadFormatException::BadFormatException ()
    : inherited (mkMessage_ ())
    , fLineNumber_ ()
    , fColumnNumber_ ()
    , fFileOffset_ ()
    , fDetails_ ()
{
}

DataExchange::BadFormatException::BadFormatException (const String& details)
    : inherited (mkMessage_ (details))
    , fLineNumber_ ()
    , fColumnNumber_ ()
    , fFileOffset_ ()
    , fDetails_ (details)
{
}

DataExchange::BadFormatException::BadFormatException (const String& details, const Optional<unsigned int>& lineNumber, const Optional<unsigned int>& columnNumber, const Optional<uint64_t>& fileOffset)
    : inherited (mkMessage_ (details, lineNumber, columnNumber, fileOffset))
    , fLineNumber_ (lineNumber)
    , fColumnNumber_ (columnNumber)
    , fFileOffset_ (fileOffset)
    , fDetails_ (details)
{
}









template    <>
void    _NoReturn_  Execution::DoThrow (const DataExchange::BadFormatException& e2Throw)
{
#if     qDefaultTracingOn
    Memory::Optional<unsigned int>  lineNum;
    Memory::Optional<unsigned int>  colNumber;
    Memory::Optional<uint64_t>      fileOffset;
    e2Throw.GetPositionInfo (&lineNum, &colNumber, &fileOffset);
    if (lineNum.IsPresent () or colNumber.IsPresent  ()) {
        DbgTrace (L"Throwing exception: DataExchange::BadFormatException ('%s', LINE=%d, COL=%d)", e2Throw.GetDetails ().LimitLength (50).c_str (), (int)lineNum.Value (-1), (int)colNumber.Value (-1));
    }
    else {
        DbgTrace (L"Throwing exception: DataExchange::BadFormatException ('%s')", e2Throw.GetDetails ().LimitLength (50).c_str ());
    }
#endif
    throw e2Throw;
}
