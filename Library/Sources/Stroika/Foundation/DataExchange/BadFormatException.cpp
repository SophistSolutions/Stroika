/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Characters/String_Constant.h"

#include "BadFormatException.h"

/*
 * Design Note:
 *
 *      o   Chose to include lineNumber etc stuff in message by default because in apps where this is not desired -
 *          (fancy gui apps) they are more likely to be the ones to override the exception mapping to message anyhow,
 *          and tune it themselves. Its the simple apps that do little but dump the string representations of a
 *          message that are more likely to want to know the line number (a bit of a guess).
 */

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using Memory::Optional;

/*
 ********************************************************************************
 ************************ DataExchange::BadFormatException **********************
 ********************************************************************************
 */
namespace {
    String mkMessage_OffsetInfo_ (const optional<unsigned int>& lineNumber, const optional<unsigned int>& columnNumber, const optional<uint64_t>& fileOffset)
    {
        String result;
        if (lineNumber.has_value ()) {
            result += Format (L"Line %d", *lineNumber);
            if (columnNumber.has_value ()) {
                result += Format (L"; Column %d", *columnNumber);
            }
        }
        if (fileOffset.has_value ()) {
            if (not result.empty ()) {
                result += String_Constant (L"; ");
            }
            result += Format (L"; FileOffset %d", *fileOffset);
        }
        return result;
    }
    inline String mkMessage_ ()
    {
        return String_Constant (L"Badly formatted input");
    }
    inline String mkMessage_ (const String& details)
    {
        return details.empty () ? mkMessage_ () : details;
    }
    String mkMessage_ (const optional<unsigned int>& lineNumber, const optional<unsigned int>& columnNumber, optional<uint64_t> fileOffset)
    {
        String msg           = mkMessage_ ();
        String lineInfoExtra = mkMessage_OffsetInfo_ (lineNumber, columnNumber, fileOffset);
        if (not lineInfoExtra.empty ()) {
            msg += Characters::String_Constant (L" (") + lineInfoExtra + String_Constant (L").");
        }
        return msg;
    }
    String mkMessage_ (const String& details, const optional<unsigned int>& lineNumber, const optional<unsigned int>& columnNumber, const optional<uint64_t>& fileOffset)
    {
        String msg           = mkMessage_ (details);
        String lineInfoExtra = mkMessage_OffsetInfo_ (lineNumber, columnNumber, fileOffset);
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

DataExchange::BadFormatException::BadFormatException (const String& details, const optional<unsigned int>& lineNumber, const optional<unsigned int>& columnNumber, const optional<uint64_t>& fileOffset)
    : inherited (mkMessage_ (details, lineNumber, columnNumber, fileOffset))
    , fLineNumber_ (lineNumber)
    , fColumnNumber_ (columnNumber)
    , fFileOffset_ (fileOffset)
    , fDetails_ (details)
{
}

const DataExchange::BadFormatException DataExchange::BadFormatException::kThe;
