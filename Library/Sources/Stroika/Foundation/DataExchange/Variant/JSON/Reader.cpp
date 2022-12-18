/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <optional>

#include "../../../Characters/FloatConversion.h"
#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Containers/Support/ReserveTweaks.h"
#include "../../../Memory/InlineBuffer.h"
#include "../../../Streams/StreamReader.h"
#include "../../../Streams/TextReader.h"
#include "../../../Traversal/Range.h"
#include "../../BadFormatException.h"

#include "Reader.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Traversal;

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

    // @todo generalize this so templated and can use in header file
    // or merge this buffering logic into Ptr class? No - issue with that is we mess up other access to Ptr,
    // so must be clear this an adopt (or have detach method)
    // Maybe have "Syncrhonize" method, so this forces THIS seek ptr to same position as
    // underlying stream?
    struct MyBufferedStreamReader_ : Streams::StreamReader<Character> {
        MyBufferedStreamReader_ (const Streams::InputStream<ElementType>::Ptr& underlyingReadFromStreamAdopted)
            : StreamReader<Character>{underlyingReadFromStreamAdopted}
        {
        }
        [[nodiscard]] inline wchar_t NextChar ()
        {
            Require (not IsAtEOF ());
            return Read ()->As<wchar_t> ();
        }
        inline void AdvanceOne ()
        {
            Require (not IsAtEOF ());
            Seek (Streams::Whence::eFromCurrent, 1);
        }
    };

    VariantValue Reader_value_ (MyBufferedStreamReader_& in);

    // throw if bad hex digit
    uint8_t HexChar2Num_ (char c)
    {
        if ('0' <= c and c <= '9') {
            return static_cast<uint8_t> (c - '0');
        }
        if ('A' <= c and c <= 'F') {
            return static_cast<uint8_t> ((c - 'A') + 10);
        }
        if ('a' <= c and c <= 'f') {
            return static_cast<uint8_t> ((c - 'a') + 10);
        }
        Execution::Throw (BadFormatException{L"JSON: bad hex digit after \\u"sv});
    }

    // 'in' is positioned to the start of string, and we read, leaving in possitioned just after end of string
    VariantValue Reader_String_ (MyBufferedStreamReader_& in)
    {
        Require (not in.IsAtEOF ());
        wchar_t c = in.NextChar ();
        if (c != '\"') [[unlikely]] {
            Execution::Throw (BadFormatException{L"JSON: Expected quoted string"sv});
        }
        // accumulate chars, and check for close-quote
        StringBuilder result;
        while (true) {
            if (in.IsAtEOF ()) [[unlikely]] {
                Execution::Throw (BadFormatException{L"JSON: Unexpected EOF reading string (looking for close quote)"sv});
            }
            c = in.NextChar ();
            if (c == '\"') [[unlikely]] {
                return VariantValue{result.str ()};
            }
            else if (c == '\\') {
                // quoted character read...
                if (in.IsAtEOF ()) [[unlikely]] {
                    Execution::Throw (BadFormatException{L"JSON: Unexpected EOF reading string (looking for close quote)"sv});
                }
                c = in.NextChar ();
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
                            if (in.IsAtEOF ()) [[unlikely]] {
                                Execution::Throw (BadFormatException{L"JSON: Unexpected EOF reading string (looking for close quote)"sv});
                            }
                            newC += HexChar2Num_ (static_cast<char> (in.NextChar ()));
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
            result += c; // must handle other character quoting (besides \u which was preflighted)
        }
    }

    // 'in' is positioned to the second character of number (first passed as arg), and we read, leaving in positioned just after end of number
    static constexpr Character kDash_{'-'};
    VariantValue               Reader_Number_ (wchar_t initialChar, MyBufferedStreamReader_& in)
    {
        Require (initialChar == '-' or iswdigit (initialChar));

        bool containsDot = false;
        // ACCUMULATE STRING, and then call builtin number parsing functions...
        // This accumulation is NOT as restrictive as it could be - but should accept all valid numbers
        StringBuilder tmp;
        for (wchar_t c = initialChar; c != '\0'; c = in.Read ().value_or ('\0').As<wchar_t> ()) {
            if (iswdigit (c) or c == '.' or c == 'e' or c == 'E' or c == '+' or c == '-') [[likely]] {
                tmp += c;
                if (c == '.') [[unlikely]] {
                    containsDot = true;
                }
            }
            else {
                // any other character signals end of number (not a syntax error)
                // but backup - don't consume next character - not part of number
                Assert (not tmp.empty ()); // at least consumed 'initialChar'
                in.Seek (Streams::Whence::eFromCurrent, -1);
                break;
            }
        }
        Assert (not tmp.empty ());
        if (containsDot) {
            return VariantValue{Characters::FloatConversion::ToFloat<long double> (tmp.begin (), tmp.end ())};
        }
        else {
            // if no - use unsigned since has wider range (if no -)
            return (initialChar == kDash_)
                       ? VariantValue{Characters::String2Int<long long int> (tmp.begin (), tmp.end ())}
                       : VariantValue{Characters::String2Int<unsigned long long int> (tmp.begin (), tmp.end ())};
        }
    }

    // NOTE: THIS STARTS SEEKED JUST PAST OPENING '{'
    VariantValue Reader_Object_ (MyBufferedStreamReader_& in)
    {
        Mapping<String, VariantValue> result;

        // accumulate elements, and check for close-array
        enum LookingFor { eName,
                          eValue,
                          eColon,
                          eComma };
        LookingFor lf = eName;

        optional<String> curName;
        while (true) {
            optional<Character> oNextChar = in.Read ();
            if (not oNextChar.has_value ()) [[unlikely]] {
                in.Seek (Streams::Whence::eFromCurrent, -1);
                Execution::Throw (BadFormatException{L"JSON: Unexpected EOF reading string (looking for '}')"sv});
            }
            wchar_t nextChar = oNextChar->As<wchar_t> ();
            if (nextChar == '}') {
                if (lf == eName or lf == eComma) {
                    // skip char
                    return VariantValue{result};
                }
                else {
                    in.Seek (Streams::Whence::eFromCurrent, -1);
                    Execution::Throw (BadFormatException{L"JSON: Unexpected '}' reading object"sv});
                }
            }
            else if (nextChar == ',') {
                if (lf == eComma) [[likely]] {
                    // skip char
                    lf = eName; // next elt
                }
                else {
                    in.Seek (Streams::Whence::eFromCurrent, -1);
                    Execution::Throw (BadFormatException{L"JSON: Unexpected ',' reading object"sv});
                }
            }
            else if (nextChar == ':') {
                if (lf == eColon) [[likely]] {
                    // skip char
                    lf = eValue; // next elt
                }
                else {
                    in.Seek (Streams::Whence::eFromCurrent, -1);
                    Execution::Throw (BadFormatException{L"JSON: Unexpected ':' reading object"sv});
                }
            }
            else if (iswspace (nextChar)) {
                // skip char
            }
            else {
                in.Seek (Streams::Whence::eFromCurrent, -1);
                if (lf == eName) {
                    curName = Reader_String_ (in).As<wstring> ();
                    lf      = eColon;
                }
                else if (lf == eValue) [[likely]] {
                    result.Add (Memory::ValueOf (curName), Reader_value_ (in));
                    curName = nullopt;
                    lf      = eComma;
                }
                else {
                    Execution::Throw (BadFormatException{L"JSON: Unexpected character looking for colon or comma reading object"sv});
                }
            }
        }
    }

    // NOTE - called with OPENING '[' already read
    VariantValue Reader_Array_ (MyBufferedStreamReader_& in)
    {
        vector<VariantValue> result;

        // accumulate elements, and check for close-array
        bool lookingForElt = true;
        while (true) {
            if (in.IsAtEOF ()) [[unlikely]] {
                Execution::Throw (BadFormatException{L"JSON: Unexpected EOF reading string (looking for ']')"sv});
            }
            if (in.Peek () == ']') {
                if (lookingForElt) {
                    // allow ending ',' - harmless - could  be more aggressive - but if so - careful of zero-sized array special case
                }
                in.AdvanceOne ();
                return VariantValue{result};
            }
            else if (in.Peek () == ',') {
                if (lookingForElt) [[unlikely]] {
                    Execution::Throw (BadFormatException{L"JSON: Unexpected second ',' in reading array"sv});
                }
                else {
                    lookingForElt = true;
                }
                in.AdvanceOne ();
            }
            else if (iswspace (in.Peek ()->As<wchar_t> ())) {
                in.AdvanceOne ();
            }
            else {
                // not looking at whitespace, in midst of array, and array not terminated, so better be looking at a value
                if (lookingForElt) [[likely]] {
                    Containers::Support::ReserveTweaks::Reserve4Add1 (result);
                    result.push_back (Reader_value_ (in));
                    lookingForElt = false;
                }
                else {
                    Execution::Throw (BadFormatException{L"JSON: Unexpected character (missing ',' ?) in reading array"sv});
                }
            }
        }
    }

    VariantValue Reader_SpecialToken_ (wchar_t initialChar, MyBufferedStreamReader_& in)
    {
        Streams::SeekOffsetType savedPos = in.GetOffset ();
        switch (initialChar) {
            case 'f': {
                Character buf[4];
                if (in.ReadAll (begin (buf), end (buf)) == 4 and
                    buf[0] == 'a' and
                    buf[1] == 'l' and
                    buf[2] == 's' and
                    buf[3] == 'e') {
                    return VariantValue{false};
                }
            } break;
            case 't': {
                Character buf[3];
                if (in.ReadAll (begin (buf), end (buf)) == 3 and
                    buf[0] == 'r' and
                    buf[1] == 'u' and
                    buf[2] == 'e') {
                    return VariantValue{true};
                }
            } break;
            case 'n': {
                Character buf[3];
                if (in.ReadAll (begin (buf), end (buf)) == 3 and
                    buf[0] == 'u' and
                    buf[1] == 'l' and
                    buf[2] == 'l') {
                    return VariantValue{};
                }
            } break;
        }
        in.Seek (savedPos - 1); // because handed initial char, and seeked past it
        Execution::Throw (BadFormatException{L"JSON: Unrecognized token"sv});
    }

    VariantValue Reader_value_ (MyBufferedStreamReader_& in)
    {
        // Skip initial whitespace, and look for any value:
        //      string
        //      number
        //      object
        //      array
        //      true
        //      false
        //      null
        for (optional<Character> oc = in.Read (); oc; oc = in.Read ()) {
            switch (oc->As<wchar_t> ()) {
                case '\"':
                    in.Seek (Streams::Whence::eFromCurrent, -1);
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
                    return Reader_Number_ (oc->As<wchar_t> (), in);

                case '{':
                    return Reader_Object_ (in);
                case '[':
                    return Reader_Array_ (in);

                case 't':
                case 'f':
                case 'n':
                    return Reader_SpecialToken_ (oc->As<wchar_t> (), in);

                default: {
                    if (iswspace (oc->As<wchar_t> ())) [[likely]] {
                        // ignore
                    }
                    else {
                        Execution::Throw (BadFormatException{L"JSON: Unexpected character looking for start of value"sv});
                    }
                }
            }
        }
        // if we get here - nothing found
        Execution::Throw (BadFormatException{L"JSON: Unexpected EOF looking for value"sv});
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
        static const String kResult_ = L".json"sv;
        return kResult_;
    }
    virtual VariantValue Read (const Streams::InputStream<byte>::Ptr& in) override
    {
        using namespace Streams;
        return Read (TextReader::New (in, SeekableFlag::eSeekable));
    }
    virtual VariantValue Read (const Streams::InputStream<Characters::Character>::Ptr& in) override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::JSON::Reader::Rep_::Read"};
#endif
        Require (in.IsSeekable ());
        MyBufferedStreamReader_ reader{in};
        return Reader_value_ (reader);
    }
};
Variant::JSON::Reader::Reader ()
    : inherited{make_shared<Rep_> ()}
{
}
