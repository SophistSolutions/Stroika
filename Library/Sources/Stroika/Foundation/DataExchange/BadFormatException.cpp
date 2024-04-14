/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"

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

/*
 ********************************************************************************
 ************************ DataExchange::BadFormatException **********************
 ********************************************************************************
 */
namespace {
    String mkMessage_OffsetInfo_ (const optional<unsigned int>& lineNumber, const optional<unsigned int>& columnNumber, const optional<uint64_t>& fileOffset)
    {
        StringBuilder result;
        if (lineNumber.has_value ()) {
            result += Format ("Line {}"_f, *lineNumber);
            if (columnNumber.has_value ()) {
                result += Format ("; Column %d"_f, *columnNumber);
            }
        }
        if (fileOffset.has_value ()) {
            if (not result.empty ()) {
                result += "; "sv;
            }
            result += Format ("; FileOffset {}"_f, *fileOffset);
        }
        return result;
    }
    inline String mkMessage_ ()
    {
        return "Badly formatted input"sv;
    }
    inline String mkMessage_ (const String& details)
    {
        return details.empty () ? mkMessage_ () : details;
    }
    String mkMessage_ (const optional<unsigned int>& lineNumber, const optional<unsigned int>& columnNumber, optional<uint64_t> fileOffset)
    {
        StringBuilder msg           = mkMessage_ ();
        String        lineInfoExtra = mkMessage_OffsetInfo_ (lineNumber, columnNumber, fileOffset);
        if (not lineInfoExtra.empty ()) {
            msg << " ("sv << lineInfoExtra << ")."sv;
        }
        return msg;
    }
    String mkMessage_ (const String& details, const optional<unsigned int>& lineNumber, const optional<unsigned int>& columnNumber,
                       const optional<uint64_t>& fileOffset)
    {
        StringBuilder msg           = mkMessage_ (details);
        String        lineInfoExtra = mkMessage_OffsetInfo_ (lineNumber, columnNumber, fileOffset);
        if (not lineInfoExtra.empty ()) {
            msg << " ("sv << lineInfoExtra << ")."sv;
        }
        return msg;
    }
}

DataExchange::BadFormatException::BadFormatException ()
    : inherited{mkMessage_ ()}
{
}

DataExchange::BadFormatException::BadFormatException (const String& details)
    : inherited{mkMessage_ (details)}
    , fDetails_{details}
{
}

DataExchange::BadFormatException::BadFormatException (const String& details, const optional<unsigned int>& lineNumber,
                                                      const optional<unsigned int>& columnNumber, const optional<uint64_t>& fileOffset)
    : inherited{mkMessage_ (details, lineNumber, columnNumber, fileOffset)}
    , fLineNumber_{lineNumber}
    , fColumnNumber_{columnNumber}
    , fFileOffset_{fileOffset}
    , fDetails_{details}
{
}
