/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/FloatConversion.h"
#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/String_Constant.h"
#include "../../../Streams/TextReader.h"
#include "../../BadFormatException.h"

#include "Reader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using Memory::Byte;
using Characters::String_Constant;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {

    /*
     * Parse strategy:
     *      o   Pre-pass to map all input to UNICODE - and then just handle a sequence of unicode strings
     *      o   Classic resursive decent parser.
     *      o   Inline lexical analysis (cuz very simple)
     */

    enum ReadState_ {
        eNeutral_,
        eInObject_,
        eInArray_,
        eInNumber_,
        eInString_,
    };

    /*
     * Utilities to treat string iterator/end ptr as a 'stream pointer' - and get next char
     */
    inline bool IsAtEOF_ (const Streams::InputStream<Characters::Character>& in)
    {
        Require (in != nullptr);
        return in.Peek ().IsMissing ();
    }
    inline wchar_t NextChar_ (const Streams::InputStream<Characters::Character>& in)
    {
        Require (not IsAtEOF_ (in));
        return in.Read ()->As<wchar_t> ();
    }
    inline wchar_t PeekNextChar_ (const Streams::InputStream<Characters::Character>& in)
    {
        Require (not IsAtEOF_ (in));
        return in.Peek ()->As<wchar_t> ();
    }

    VariantValue Reader_value_ (const Streams::InputStream<Characters::Character>& in);

    // throw if bad hex digit
    unsigned int HexChar2Num_ (char c)
    {
        if ('0' <= c and c <= '9') {
            return c - '0';
        }
        if ('A' <= c and c <= 'F') {
            return (c - 'A') + 10;
        }
        if ('a' <= c and c <= 'f') {
            return (c - 'a') + 10;
        }
        Execution::Throw (BadFormatException (String_Constant (L"JSON: bad hex digit after \\u")));
    }

    // 'in' is positioned to the start of string, and we read, leaving in possitioned just after end of string
    VariantValue Reader_String_ (const Streams::InputStream<Characters::Character>& in)
    {
        Require (in != nullptr);
        Require (not IsAtEOF_ (in));
        wchar_t c = NextChar_ (in);
        if (c != '\"') {
            Execution::Throw (BadFormatException (String_Constant (L"JSON: Expected quoted string")));
        }
        // accumulate chars, and check for close-quote
        wstring result;
        while (true) {
            if (IsAtEOF_ (in)) {
                Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected EOF reading string (looking for close quote)")));
            }
            c = NextChar_ (in);
            if (c == '\"') {
                return VariantValue (result);
            }
            else if (c == '\\') {
                // quoted character read...
                if (IsAtEOF_ (in)) {
                    Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected EOF reading string (looking for close quote)")));
                }
                c = NextChar_ (in);
                switch (c) {
                    case 'b':
                        c = '\b';
                        break;
                    case 'f':
                        c = '\f';
                        break;
                    case 'n':
                        c = '\n';
                        break;
                    case 'r':
                        c = '\r';
                        break;
                    case 't':
                        c = '\t';
                        break;
                    case 'u': {
                        // Not sure this is right -- But I hope so ... -- LGP 2012-11-29
                        wchar_t newC = '\0';
                        for (int n = 0; n < 4; ++n) {
                            if (IsAtEOF_ (in)) {
                                Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected EOF reading string (looking for close quote)")));
                            }
                            newC += HexChar2Num_ (static_cast<char> (NextChar_ (in)));
                            if (n != 3) {
                                newC <<= 4;
                            }
                        }
                        c = newC;
                    } break;
                    default: {
                        // if we have \N for any unrecognized N, just treat it as N
                    }
                }
            }
            Containers::ReserveSpeedTweekAdd1 (result);
            result += c; // must handle other character quoting (besides \u which was preflighted)
        }
    }

    // 'in' is positioned to the start of number, and we read, leaving in possitioned just after end of number
    VariantValue Reader_Number_ (const Streams::InputStream<Characters::Character>& in)
    {
        Require (in != nullptr);
        Require (not IsAtEOF_ (in));

        bool containsDot = false;
        // ACCUMULATE STRING, and then call builtin number parsing functions...
        // This accumulation is NOT as restrictive as it could be - but should accept all valid numbers
        wstring tmp;
        while (not IsAtEOF_ (in)) {
            wchar_t c = NextChar_ (in);
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
                    in.Seek (Streams::Whence::eFromCurrent, -1);
                }
                break;
            }
        }
        // if got not valid characters, THATS not a valid # - so error
        if (tmp.empty ()) {
            Execution::Throw (BadFormatException (String_Constant (L"JSON: no valid number found")));
        }
        if (containsDot) {
            return VariantValue (Characters::String2Float<long double> (tmp));
        }
        else {
            // if no - use unsigned since has wider range (if no -)
            return Characters::String (tmp).LTrim ().StartsWith (String_Constant (L"-")) ? VariantValue (Characters::String2Int<long long int> (tmp)) : VariantValue (Characters::String2Int<unsigned long long int> (tmp));
        }
    }

    VariantValue Reader_Object_ (const Streams::InputStream<Characters::Character>& in)
    {
        Require (in != nullptr);
        Require (not IsAtEOF_ (in));
        Mapping<String, VariantValue> result;

        if (NextChar_ (in) != '{') {
            Execution::Throw (BadFormatException (String_Constant (L"JSON: Expected '{'")));
        }
        // accumulate elements, and check for close-array
        enum LookingFor { eName,
                          eValue,
                          eColon,
                          eComma };
        LookingFor lf = eName;

        wstring curName;
        while (true) {
            if (IsAtEOF_ (in)) {
                Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected EOF reading string (looking for '}')")));
            }
            if (in.Peek () == '}') {
                if (lf == eName or lf == eComma) {
                    NextChar_ (in); // skip char
                    return VariantValue (result);
                }
                else {
                    Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected '}' reading object")));
                }
            }
            else if (iswspace (in.Peek ()->As<wchar_t> ())) {
                NextChar_ (in); // skip char
            }
            else if (in.Peek () == ',') {
                if (lf == eComma) {
                    NextChar_ (in); // skip char
                    lf = eName;     // next elt
                }
                else {
                    Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected ',' reading object")));
                }
            }
            else if (in.Peek () == ':') {
                if (lf == eColon) {
                    NextChar_ (in); // skip char
                    lf = eValue;    // next elt
                }
                else {
                    Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected ':' reading object")));
                }
            }
            else {
                if (lf == eName) {
                    curName = Reader_String_ (in).As<wstring> ();
                    lf      = eColon;
                }
                else if (lf == eValue) {
                    result.Add (curName, Reader_value_ (in));
                    curName.clear ();
                    lf = eComma;
                }
                else {
                    Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected character looking for colon or comma reading object")));
                }
            }
        }
    }

    VariantValue Reader_Array_ (const Streams::InputStream<Characters::Character>& in)
    {
        Require (in != nullptr);
        Require (not IsAtEOF_ (in));
        vector<VariantValue> result;

        if (NextChar_ (in) != '[') {
            Execution::Throw (BadFormatException (String_Constant (L"JSON: Expected '['")));
        }
        // accumulate elements, and check for close-array
        bool lookingForElt = true;
        while (true) {
            if (IsAtEOF_ (in)) {
                Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected EOF reading string (looking for ']')")));
            }
            if (in.Peek () == ']') {
                if (lookingForElt) {
                    // allow ending ',' - harmless - could  be more aggressive - but if so - careful of zero-sized array special case
                }
                NextChar_ (in); // skip char
                return VariantValue (result);
            }
            else if (in.Peek () == ',') {
                if (lookingForElt) {
                    Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected second ',' in reading array")));
                }
                else {
                    lookingForElt = true;
                }
                NextChar_ (in); // skip char
            }
            else if (iswspace (in.Peek ()->As<wchar_t> ())) {
                NextChar_ (in); // skip char
            }
            else {
                // not looking at whitespace, in midst of array, and array not terminated, so better be looking at a value
                if (lookingForElt) {
                    Containers::ReserveSpeedTweekAdd1 (result);
                    result.push_back (Reader_value_ (in));
                    lookingForElt = false;
                }
                else {
                    Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected character (missing ',' ?) in reading array")));
                }
            }
        }
    }

    VariantValue Reader_SpecialToken_ (const Streams::InputStream<Characters::Character>& in)
    {
        Require (in != nullptr);
        Require (not IsAtEOF_ (in));
        Streams::SeekOffsetType savedPos = in.GetOffset ();
        switch (in.Peek ()->As<wchar_t> ()) {
            case 'f': {
                if (
                    in.Read () == 'f' and
                    in.Read () == 'a' and
                    in.Read () == 'l' and
                    in.Read () == 's' and
                    in.Read () == 'e') {
                    return VariantValue (false);
                }
                else {
                    in.Seek (savedPos);
                }
            } break;
            case 't': {
                if (
                    in.Read () == 't' and
                    in.Read () == 'r' and
                    in.Read () == 'u' and
                    in.Read () == 'e') {
                    return VariantValue (true);
                }
                else {
                    in.Seek (savedPos);
                }
            } break;
            case 'n': {
                if (
                    in.Read () == 'n' and
                    in.Read () == 'u' and
                    in.Read () == 'l' and
                    in.Read () == 'l') {
                    return VariantValue{};
                }
                else {
                    in.Seek (savedPos);
                }
            } break;
        }
        Execution::Throw (BadFormatException (String_Constant (L"JSON: Unrecognized token")));
    }

    VariantValue Reader_value_ (const Streams::InputStream<Characters::Character>& in)
    {
        // Skip initial whitespace, and look for any value:
        //      string
        //      number
        //      object
        //      array
        //      true
        //      false
        //      null
        for (; not IsAtEOF_ (in); NextChar_ (in)) {
            switch (in.Peek ()->As<wchar_t> ()) {
                case '\"':
                    return Reader_String_ (in);

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '-':
                    return Reader_Number_ (in);

                case '{':
                    return Reader_Object_ (in);
                case '[':
                    return Reader_Array_ (in);

                case 't':
                case 'f':
                case 'n':
                    return Reader_SpecialToken_ (in);

                default: {
                    if (iswspace (in.Peek ()->As<wchar_t> ())) {
                        // ignore
                    }
                    else {
                        Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected character looking for start of value")));
                    }
                }
            }
        }
        // if we get here - nothing found
        Execution::Throw (BadFormatException (String_Constant (L"JSON: Unexpected EOF looking for value")));
    }
}

/*
 ********************************************************************************
 ************************* Variant::JSON::Reader ********************************
 ********************************************************************************
 */
class Variant::JSON::Reader::Rep_ : public Variant::Reader::_IRep {
public:
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return String_Constant (L".json");
    }
    virtual VariantValue Read (const Streams::InputStream<Byte>& in) override
    {
        constexpr bool kSeekable_{true};
        return Read (Streams::TextReader (in, kSeekable_));
    }
    virtual VariantValue Read (const Streams::InputStream<Characters::Character>& in) override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("DataExchange::JSON::Reader::Rep_::Read");
#endif
        Require (in.IsSeekable ());
        return Reader_value_ (in);
    }
};
Variant::JSON::Reader::Reader ()
    : inherited (make_shared<Rep_> ())
{
}
