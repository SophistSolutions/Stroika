/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/StringUtils.h"
#include    "../../Characters/Format.h"
#include    "../../Streams/TextInputStreamBinaryAdapter.h"
#include    "../BadFormatException.h"

#include    "Reader.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;





namespace   {

    /*
     * Language Docs:
     *      http://json.org/
     */

    /*
     * Parse strategy:
     *      o   Pre-pass to map all input to UNICODE - and then just handle a sequence of unicode strings
     *      o   Classic resursive decent parser.
     *      o   Inline lexical analysis (cuz very simple)
     */

    enum    ReadState_ {
        eNeutral_,
        eInObject_,
        eInArray_,
        eInNumber_,
        eInString_,
    };






    /*
     * Utilities to treat string iterator/end ptr as a 'stream pointer' - and get next char
     */
    bool    IsAtEOF_ (wstring::const_iterator* i, wstring::const_iterator end)
    {
        Require (i != nullptr);
        Require (*i <= end);
        return *i == end;
    }
    wchar_t NextChar_ (wstring::const_iterator* i, wstring::const_iterator end)
    {
        Require (not IsAtEOF_ (i, end));
        wchar_t c   =   *(*i);
        (*i)++;
        return c;
    }

    Memory::VariantValue    Reader_value_ (wstring::const_iterator* i, wstring::const_iterator end);


    // throw if bad hex digit
    unsigned int    HexChar2Num_ (char c)
    {
        if ('0' <= c and c <= '9') {
            return c - '0';
        }
        if ('A' <= c and c <= 'F') {
            return (c - 'A') + 10;
        }
        if ('a' <= c and c <= 'a') {
            return (c - 'a') + 10;
        }
        Execution::DoThrow (BadFormatException (L"JSON: bad hex digit after \\u"));
    }

    // 'in' is positioned to the start of string, and we read, leaving in possitioned just after end of string
    Memory::VariantValue    Reader_String_ (wstring::const_iterator* i, wstring::const_iterator end)
    {
        Require (i != nullptr);
        Require (*i < end);
        wchar_t c   =   NextChar_ (i, end);
        if (c != '\"') {
            Execution::DoThrow (BadFormatException (L"JSON: Expected quoted string"));
        }
        // accumulate chars, and check for close-quote
        wstring result;
        while (true) {
            if (IsAtEOF_ (i, end)) {
                Execution::DoThrow (BadFormatException (L"JSON: Unexpected EOF reading string (looking for close quote)"));
            }
            c = NextChar_ (i, end);
            if (c == '\"') {
                return Memory::VariantValue (result);
            }
            else if (c == '\\') {
                // quoted character read...
                if (IsAtEOF_ (i, end)) {
                    Execution::DoThrow (BadFormatException (L"JSON: Unexpected EOF reading string (looking for close quote)"));
                }
                c = NextChar_ (i, end);
                switch (c) {
                    case    'b':
                        c = '\b';
                        break;
                    case    'f':
                        c = '\f';
                        break;
                    case    'n':
                        c = '\n';
                        break;
                    case    'r':
                        c = '\r';
                        break;
                    case    't':
                        c = '\t';
                        break;
                    case    'u':    {
                            // Not sure this is right -- But I hope so ... -- LGP 2012-11-29
                            wchar_t newC    =   '\0';
                            for (int n = 0; n < 4; ++n) {
                                if (IsAtEOF_ (i, end)) {
                                    Execution::DoThrow (BadFormatException (L"JSON: Unexpected EOF reading string (looking for close quote)"));
                                }
                                newC += HexChar2Num_ (static_cast<char> (NextChar_ (i, end)));
                                if (n != 3) {
                                    newC <<= 4;
                                }
                            }
                            c = newC;
                        }
                        break;
                    default: {
                            // if we have \N for any unrecognized N, just treat it as N
                        }
                }
            }
            Containers::ReserveSpeedTweekAdd1 (result);
            result += c;    // must handle other character quoting (besides \u which was preflighted)
        }
    }



    // 'in' is positioned to the start of number, and we read, leaving in possitioned just after end of number
    Memory::VariantValue    Reader_Number_ (wstring::const_iterator* i, wstring::const_iterator end)
    {
        Require (i != nullptr);
        Require (*i < end);

        bool    containsDot =   false;
        // ACCUMULATE STRING, and then call builtin number parsing functions...
        // This accumulation is NOT as restrictive as it could be - but should accept all valid numbers
        wstring tmp;
        while (not IsAtEOF_ (i, end)) {
            wchar_t c = NextChar_ (i, end);
            if (iswdigit (c) or c == '.' or c == 'e' or c == 'E' or c == '+' or c == '-') {
                Containers::ReserveSpeedTweekAdd1 (tmp);
                tmp += c;
                if (c == '.') {
                    containsDot = true;
                }
            }
            else {
                // any other character signals end of number (not a syntax error)
                // but if we read anything at all, backup - don't consume next character - not part of number
                if (!tmp.empty ()) {
                    (*i)--;
                }
                break;
            }
        }
        // if got not valid characters, THATS not a valid # - so error
        if (tmp.empty ()) {
            Execution::DoThrow (BadFormatException (L"JSON: no valid number found"));
        }
        // COULD do better about types (int32 versus int64 etc)
        if (containsDot) {
            return Memory::VariantValue (Characters::String2Float (tmp));
        }
        else {
            return Memory::VariantValue (Characters::String2Int (tmp));
        }
    }

    Memory::VariantValue    Reader_Object_ (wstring::const_iterator* i, wstring::const_iterator end)
    {
        Require (i != nullptr);
        Require (*i < end);
        map<wstring, Memory::VariantValue>   result;

        if (NextChar_ (i, end) != '{') {
            Execution::DoThrow (BadFormatException (L"JSON: Expected '{'"));
        }
        // accumulate elements, and check for close-array
        enum LookingFor { eName, eValue, eColon, eComma };
        LookingFor  lf = eName;

        wstring curName;
        while (true) {
            if (IsAtEOF_ (i, end)) {
                Execution::DoThrow (BadFormatException (L"JSON: Unexpected EOF reading string (looking for '}')"));
            }
            if (**i == '}') {
                if (lf == eName or lf == eComma) {
                    NextChar_ (i, end);     // skip char
                    return Memory::VariantValue (result);
                }
                else {
                    Execution::DoThrow (BadFormatException (L"JSON: Unexpected '}' reading object"));
                }
            }
            else if (iswspace (**i)) {
                NextChar_ (i, end);     // skip char
            }
            else if (**i == ',') {
                if (lf == eComma) {
                    NextChar_ (i, end);     // skip char
                    lf = eName;             // next elt
                }
                else {
                    Execution::DoThrow (BadFormatException (L"JSON: Unexpected ',' reading object"));
                }
            }
            else if (**i == ':') {
                if (lf == eColon) {
                    NextChar_ (i, end);     // skip char
                    lf = eValue;            // next elt
                }
                else {
                    Execution::DoThrow (BadFormatException (L"JSON: Unexpected ':' reading object"));
                }
            }
            else {
                if (lf == eName) {
                    curName = Reader_String_ (i, end).As<wstring> ();
                    lf = eColon;
                }
                else if (lf == eValue) {
                    Memory::VariantValue    v    = Reader_value_ (i, end);
                    result.insert (map<wstring, Memory::VariantValue>::value_type (curName, v));
                    curName.clear ();
                    lf = eComma;
                }
                else {
                    Execution::DoThrow (BadFormatException (L"JSON: Unexpected character looking for colon or comma reading object"));
                }
            }
        }
    }

    Memory::VariantValue    Reader_Array_ (wstring::const_iterator* i, wstring::const_iterator end)
    {
        Require (i != nullptr);
        Require (*i < end);
        vector<Memory::VariantValue>    result;

        if (NextChar_ (i, end) != '[') {
            Execution::DoThrow (BadFormatException (L"JSON: Expected '['"));
        }
        // accumulate elements, and check for close-array
        bool    lookingForElt   =   true;
        while (true) {
            if (IsAtEOF_ (i, end)) {
                Execution::DoThrow (BadFormatException (L"JSON: Unexpected EOF reading string (looking for ']')"));
            }
            if (**i == ']') {
                if (lookingForElt) {
                    // allow ending ',' - harmless - could  be more aggressive - but if so - careful of zero-sized array special case
                }
                NextChar_ (i, end);     // skip char
                return Memory::VariantValue (result);
            }
            else if (**i == ',') {
                if (lookingForElt) {
                    Execution::DoThrow (BadFormatException (L"JSON: Unexpected second ',' in reading array"));
                }
                else {
                    lookingForElt = true;
                }
                NextChar_ (i, end);     // skip char
            }
            else if (iswspace (**i)) {
                NextChar_ (i, end);     // skip char
            }
            else {
                // not looking at whitespace, in midst of array, and array not terminated, so better be looking at a value
                if (lookingForElt) {
                    Containers::ReserveSpeedTweekAdd1 (result);
                    result.push_back (Reader_value_ (i, end));
                    lookingForElt = false;
                }
                else {
                    Execution::DoThrow (BadFormatException (L"JSON: Unexpected character (missing ',' ?) in reading array"));
                }
            }
        }
    }

    Memory::VariantValue    Reader_SpecialToken_ (wstring::const_iterator* i, wstring::const_iterator end)
    {
        Require (i != nullptr);
        Require (*i < end);
        switch (**i) {
            case    'f': {
                    if (5 <= (end - *i) and
                            * ((*i) + 1) == 'a' and
                            * ((*i) + 2) == 'l' and
                            * ((*i) + 3) == 's' and
                            * ((*i) + 4) == 'e'
                       ) {
                        (*i) += 5;
                        return Memory::VariantValue (false);
                    }
                }
                break;
            case    't': {
                    if (4 <= (end - *i) and
                            * ((*i) + 1) == 'r' and
                            * ((*i) + 2) == 'u' and
                            * ((*i) + 3) == 'e'
                       ) {
                        (*i) += 4;
                        return Memory::VariantValue (true);
                    }
                }
                break;
            case    'n': {
                    if (4 <= (end - *i) and
                            * ((*i) + 1) == 'u' and
                            * ((*i) + 2) == 'l' and
                            * ((*i) + 3) == 'l'
                       ) {
                        (*i) += 4;
                        return Memory::VariantValue ();
                    }
                }
                break;
        }
        Execution::DoThrow (BadFormatException (L"JSON: Unrecognized token"));
    }

    Memory::VariantValue    Reader_value_ (wstring::const_iterator* i, wstring::const_iterator end)
    {
        // Skip initial whitespace, and look for any value:
        //      string
        //      number
        //      object
        //      array
        //      true
        //      false
        //      null
        for (; *i < end; ++i) {
            switch (**i) {
                case    '\"':
                    return Reader_String_ (i, end);

                case    '0':
                case    '1':
                case    '2':
                case    '3':
                case    '4':
                case    '5':
                case    '6':
                case    '7':
                case    '8':
                case    '9':
                case    '-':
                    return Reader_Number_ (i, end);

                case    '{':
                    return Reader_Object_ (i, end);
                case    '[':
                    return Reader_Array_ (i, end);

                case    't':
                case    'f':
                case    'n':
                    return Reader_SpecialToken_ (i, end);

                default: {
                        if (iswspace (**i)) {
                            // ignore
                        }
                        else {
                            Execution::DoThrow (BadFormatException (L"JSON: Unexpected character looking for start of value"));
                        }
                    }
            }

        }
        // if we get here - nothing found
        Execution::DoThrow (BadFormatException (L"JSON: Unexpected EOF looking for value"));
    }
}










/*
 ********************************************************************************
 ******************* DataExchangeFormat::JSON::Reader ***************************
 ********************************************************************************
 */
class   DataExchangeFormat::JSON::Reader::Rep_ : public DataExchangeFormat::Reader::_IRep {
public:
    virtual Memory::VariantValue    Read (const Streams::BinaryInputStream& in) override {
		return Read (Streams::TextInputStreamBinaryAdapter (in));
    }
    virtual Memory::VariantValue    Read (const Streams::TextInputStream& in) override {
		wstring     tmp =   in.ReadAll ().As<wstring> ();
		wstring::const_iterator i = tmp.begin ();
		return Reader_value_ (&i, tmp.end ());
    }
};
DataExchangeFormat::JSON::Reader::Reader ()
    : inherited (shared_ptr<_IRep> (new Rep_ ()))
{
}
