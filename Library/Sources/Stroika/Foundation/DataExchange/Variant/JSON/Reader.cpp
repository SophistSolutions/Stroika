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

    // use StreamReader to get buffering of input data (performance tweak), and a couple helper methods
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
        inline void BackupOne ()
        {
            Seek (Streams::Whence::eFromCurrent, -1);
        }
    };

    VariantValue Reader_value_ (MyBufferedStreamReader_& in);

    /*
     */
    inline bool IsJSONSpace_ (wchar_t c)
    {
        // iswspace was pretty slow (on windoze) - showing up as significant in performance profiling.
        // According to:
        //      https://www.json.org/json-en.html
        // only spaces allowed in json source are:
        //  ws = *(
        //       %x20 /              ; Space
        //       %x09 /              ; Horizontal tab
        //       %x0A /              ; Line feed or New line
        //       %x0D                ; Carriage return
        //       )
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                return true;
            default:
                return false;
        }
    }

    inline bool IsJSONDigit_ (wchar_t c)
    {
        // iswdigit tweak (showed up as significant - 6% of full runtime)
        // According to:
        //      https://www.json.org/json-en.html
        // appears just digits 0..9
        // slightly longer way but doesn't assume ascii, and probably compiles to the same thing is '0' <= n <= '9'
        switch (c) {
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
                return true;
            default:
                return false;
        }
    }

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
        static const auto kException_{BadFormatException{L"JSON: bad hex digit after \\u"sv}};
        Execution::Throw (kException_);
    }

    // 'in' is positioned to the start of string, and we read, leaving in possitioned just after the end of the string
    VariantValue Reader_String_ (MyBufferedStreamReader_& in)
    {
        Require (not in.IsAtEOF ());
        wchar_t c = in.NextChar ();
        if (c != '\"') [[unlikely]] {
            static const auto kException_{BadFormatException{L"JSON: Expected quoted string"sv}};
            Execution::Throw (kException_);
        }
        // accumulate chars, and check for close-quote
        StringBuilder result;
        while (true) {
            if (in.IsAtEOF ()) [[unlikely]] {
                static const auto kException_{BadFormatException{L"JSON: Unexpected EOF reading string (looking for close quote)"sv}};
                Execution::Throw (kException_);
            }
            c = in.NextChar ();
            if (c == '\"') [[unlikely]] {
                return VariantValue{result.str ()};
            }
            else if (c == '\\') {
                // quoted character read...
                if (in.IsAtEOF ()) [[unlikely]] {
                    static const auto kException_{BadFormatException{L"JSON: Unexpected EOF reading string (looking for close quote)"sv}};
                    Execution::Throw (kException_);
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
                                static const auto kException_{BadFormatException{L"JSON: Unexpected EOF reading string (looking for close quote)"sv}};
                                Execution::Throw (kException_);
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
        Require (initialChar == '-' or IsJSONDigit_ (initialChar));

        bool containsDot = false;
        // ACCUMULATE STRING, and then call builtin number parsing functions...
        // This accumulation is NOT as restrictive as it could be - but should accept all valid numbers
        StringBuilder tmp;
        for (wchar_t c = initialChar; c != '\0'; c = in.Read ().value_or ('\0').As<wchar_t> ()) {
            if (IsJSONDigit_ (c) or c == '.' or c == 'e' or c == 'E' or c == '+' or c == '-') [[likely]] {
                tmp += c;
                if (c == '.') [[unlikely]] {
                    containsDot = true;
                }
            }
            else {
                // any other character signals end of number (not a syntax error)
                // but backup - don't consume next character - not part of number
                Assert (not tmp.empty ()); // at least consumed 'initialChar'
                in.BackupOne ();
                break;
            }
        }
        Assert (not tmp.empty ());
        Memory::StackBuffer<wchar_t> ignoreBuf;
        span<const wchar_t>          tmpData = tmp.GetData (&ignoreBuf);
        if (containsDot) {
            return VariantValue{Characters::FloatConversion::ToFloat<long double> (tmpData)};
        }
        else {
            // if no - use unsigned since has wider range (if no -)
            return (initialChar == kDash_)
                       ? VariantValue{Characters::String2Int<long long int> (tmpData)}
                       : VariantValue{Characters::String2Int<unsigned long long int> (tmpData)};
        }
    }

    // NOTE: THIS STARTS SEEKED JUST PAST OPENING '{'
    VariantValue Reader_Object_ (MyBufferedStreamReader_& in)
    {
        // @todo consider using other optimizations like Mapping_hashmap<>
        Containers::Concrete::Mapping_stdmap<String, VariantValue>::STDMAP<> result; // slight tweak using stl map, and move-construct Stroika map at the end

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
                static const auto kException_{BadFormatException{L"JSON: Unexpected EOF reading object (looking for '}')"sv}};
                Execution::Throw (kException_);
            }
            wchar_t nextChar = oNextChar->As<wchar_t> ();
            if (nextChar == '}') {
                if (lf == eName or lf == eComma) {
                    // skip char
                    return VariantValue{Containers::Concrete::Mapping_stdmap<String, VariantValue>{move (result)}};
                }
                else {
                    static const auto kException_{BadFormatException{L"JSON: Unexpected '}' reading object"sv}};
                    Execution::Throw (kException_);
                }
            }
            else if (nextChar == ',') {
                if (lf == eComma) [[likely]] {
                    // skip char
                    lf = eName; // next elt
                }
                else {
                    static const auto kException_{BadFormatException{L"JSON: Unexpected ',' reading object"sv}};
                    Execution::Throw (kException_);
                }
            }
            else if (nextChar == ':') {
                if (lf == eColon) [[likely]] {
                    // skip char
                    lf = eValue; // next elt
                }
                else {
                    static const auto kException_{BadFormatException{L"JSON: Unexpected ':' reading object"sv}};
                    Execution::Throw (kException_);
                }
            }
            else if (IsJSONSpace_ (nextChar)) {
                // skip char
            }
            else {
                in.BackupOne ();
                if (lf == eName) {
                    curName = Reader_String_ (in).As<String> ();
                    lf      = eColon;
                }
                else if (lf == eValue) [[likely]] {
                    result.insert ({Memory::ValueOf (curName), Reader_value_ (in)});
                    curName = nullopt;
                    lf      = eComma;
                }
                else {
                    static const auto kException_{BadFormatException{L"JSON: Unexpected character looking for colon or comma reading object"sv}};
                    Execution::Throw (kException_);
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
                static const auto kException_{BadFormatException{L"JSON: Unexpected EOF reading array (looking for ']')"sv}};
                Execution::Throw (kException_);
            }
            wchar_t peekedChar = in.Peek ()->As<wchar_t> ();
            if (peekedChar == ']') {
                if (lookingForElt) {
                    // allow ending ',' - harmless - could  be more aggressive - but if so - careful of zero-sized array special case
                }
                in.AdvanceOne ();
                return VariantValue{Containers::Concrete::Sequence_stdvector<VariantValue>{move (result)}};
            }
            else if (peekedChar == ',') {
                if (lookingForElt) [[unlikely]] {
                    static const auto kException_{BadFormatException{L"JSON: Unexpected second ',' in reading array"sv}};
                    Execution::Throw (kException_);
                }
                else {
                    lookingForElt = true;
                }
                in.AdvanceOne ();
            }
            else if (IsJSONSpace_ (peekedChar)) {
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
                    static const auto kException_{BadFormatException{L"JSON: Unexpected character (missing ',' ?) in reading array"sv}};
                    Execution::Throw (kException_);
                }
            }
        }
    }

    VariantValue Reader_SpecialToken_ (wchar_t initialChar, MyBufferedStreamReader_& in)
    {
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
        static const auto kException_{BadFormatException{L"JSON: Unrecognized token"sv}};
        Execution::Throw (kException_);
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
                    in.BackupOne ();
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
                    if (IsJSONSpace_ (oc->As<wchar_t> ())) [[likely]] {
                        // ignore
                    }
                    else {
                        static const auto kException_{BadFormatException{L"JSON: Unexpected character looking for start of value"sv}};
                        Execution::Throw (kException_);
                    }
                }
            }
        }
        // if we get here - nothing found
        static const auto kException_{BadFormatException{L"JSON: Unexpected EOF looking for value"sv}};
        Execution::Throw (kException_);
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
