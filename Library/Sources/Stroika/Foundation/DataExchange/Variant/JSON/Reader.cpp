/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>
#include <stack>

#if __has_include("boost/json.hpp")
#include <boost/json.hpp>
#include <boost/json/basic_parser_impl.hpp>
#endif

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_stdhashmap.h"
#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Memory/InlineBuffer.h"
#include "Stroika/Foundation/Streams/StreamReader.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#include "Stroika/Foundation/Traversal/Range.h"

#include "Reader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Traversal;

using Containers::Concrete::Mapping_stdhashmap;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {

    /*
     * Parse strategy:
     *      o   Pre-pass to map all input to UNICODE - and then just handle a sequence of unicode strings
     *      o   Classic recursive decent parser.
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
        MyBufferedStreamReader_ (const Streams::InputStream::Ptr<ElementType>& underlyingReadFromStreamAdopted)
            : StreamReader<Character>{underlyingReadFromStreamAdopted}
        {
        }
        [[nodiscard]] inline char32_t NextChar ()
        {
            Require (not IsAtEOF ());
            return Read ()->As<char32_t> ();
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
    inline bool IsJSONSpace_ (char32_t c)
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

    inline bool IsJSONDigit_ (char32_t c)
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
        if ('0' <= c and c <= '9') [[likely]] {
            return static_cast<uint8_t> (c - '0');
        }
        if ('a' <= c and c <= 'f') [[likely]] {
            return static_cast<uint8_t> ((c - 'a') + 10);
        }
        if ('A' <= c and c <= 'F') [[likely]] {
            return static_cast<uint8_t> ((c - 'A') + 10);
        }
        static const auto kException_{BadFormatException{"JSON: bad hex digit after \\u"sv}};
        Execution::Throw (kException_);
    }

    // 'in' is positioned to the start of string, and we read, leaving in positioned just after the end of the string
    String Reader_String_ (MyBufferedStreamReader_& in)
    {
        Require (not in.IsAtEOF ());
        char32_t c = in.NextChar ();
        if (c != '\"') [[unlikely]] {
            static const auto kException_{BadFormatException{"JSON: Expected quoted string"sv}};
            Execution::Throw (kException_);
        }
        /*
         * accumulate chars, and check for close-quote
         */
        StringBuilder<StringBuilder_Options<char8_t, 64>> result; // StringBuilder_Options: text often ascii, empirically a bit faster with on windows on jsonparser regtest
        while (true) {
            if (in.IsAtEOF ()) [[unlikely]] {
                static const auto kException_{BadFormatException{"JSON: Unexpected EOF reading string (looking for close quote)"sv}};
                Execution::Throw (kException_);
            }
            c = in.NextChar ();
            if (c == '\"') [[unlikely]] {
                return result.str ();
            }
            else if (c == '\\') [[unlikely]] {
                // quoted character read...
                if (in.IsAtEOF ()) [[unlikely]] {
                    static const auto kException_{BadFormatException{"JSON: Unexpected EOF reading string (looking for close quote)"sv}};
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
                        char32_t newC = '\0';
                        for (int n = 0; n < 4; ++n) {
                            if (in.IsAtEOF ()) [[unlikely]] {
                                static const auto kException_{BadFormatException{"JSON: Unexpected EOF reading string (looking for close quote)"sv}};
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
    VariantValue               Reader_Number_ (char32_t initialChar, MyBufferedStreamReader_& in)
    {
        Require (initialChar == '-' or IsJSONDigit_ (initialChar));

        bool containsDot = false;
        // ACCUMULATE STRING, and then call builtin number parsing functions...
        // This accumulation is NOT as restrictive as it could be - but should accept all valid numbers
        StringBuilder tmp;
        for (char32_t c = initialChar; c != '\0'; c = in.Read ().value_or ('\0').As<char32_t> ()) {
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
        Memory::StackBuffer<char32_t> ignoreBuf;
        span<const char32_t>          tmpData = tmp.GetData (&ignoreBuf);
        if (containsDot) {
            return VariantValue{Characters::FloatConversion::ToFloat<long double> (tmpData)};
        }
        else {
            // if no - use unsigned since has wider range (if no -)
            return (initialChar == kDash_) ? VariantValue{Characters::String2Int<long long int> (tmpData)}
                                           : VariantValue{Characters::String2Int<unsigned long long int> (tmpData)};
        }
    }

    // NOTE: THIS STARTS SEEKED JUST PAST OPENING '{'
    VariantValue Reader_Object_ (MyBufferedStreamReader_& in)
    {
        Mapping_stdhashmap<String, VariantValue>::STDHASHMAP<> result; // slight tweak using stl map, and move-construct Stroika map at the end

        // accumulate elements, and check for close-array
        enum LookingFor {
            eName, // this means looking for start of next field/member of the object
            eValue,
            eColon,
            eComma // when looking for comma could either find it, or } marking end of object
        };
        LookingFor       lf = eName;
        optional<String> curName;
        while (true) {
            optional<Character> oNextChar = in.Peek ();
            if (not oNextChar.has_value ()) [[unlikely]] {
                static const auto kException_{BadFormatException{"JSON: Unexpected EOF reading object (looking for '}')"sv}};
                Execution::Throw (kException_);
            }
            char32_t nextChar = oNextChar->As<char32_t> ();
            if (IsJSONSpace_ (nextChar)) [[likely]] {
                in.AdvanceOne (); // skip char
                continue;
            }
            switch (lf) {
                case eName: {
                    Assert (curName == nullopt);
                    if (nextChar == '}') {
                        in.AdvanceOne (); // finished object
                        return VariantValue{Mapping_stdhashmap<String, VariantValue>{move (result)}};
                    }
                    else if (nextChar == '\"') [[likely]] {
                        curName = Reader_String_ (in); // starting a new data member (with a string)
                        lf      = eColon;
                    }
                    else {
                        static const auto kException_{
                            BadFormatException{"JSON: Reading object, looking for a name, didn't find a close brace or open quote"sv}};
                        Execution::Throw (kException_);
                    }
                } break;
                case eComma: {
                    Assert (curName == nullopt);
                    if (nextChar == '}') {
                        in.AdvanceOne (); // finished object
                        return VariantValue{Mapping_stdhashmap<String, VariantValue>{move (result)}};
                    }
                    else if (nextChar == ',') [[likely]] {
                        in.AdvanceOne (); // consume it, and look for name next (start of next object member)
                        lf = eName;
                    }
                    else {
                        static const auto kException_{BadFormatException{"JSON: Reading object, looking for a comma, but found something else"sv}};
                        Execution::Throw (kException_);
                    }
                } break;
                case eColon: {
                    Assert (curName);
                    if (nextChar == ':') [[likely]] {
                        in.AdvanceOne (); // consume the separator, and look the the value
                        lf = eValue;
                    }
                    else {
                        static const auto kException_{BadFormatException{"JSON: Reading object, looking for a colon, but found something else"sv}};
                        Execution::Throw (kException_);
                    }
                } break;
                case eValue: {
                    Assert (curName);
                    // dont care what the character is, read a new value
                    result.insert ({Memory::ValueOf (curName), Reader_value_ (in)});
                    curName = nullopt;
                    lf      = eComma; // and look for another field/data member
                } break;
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
                static const auto kException_{BadFormatException{"JSON: Unexpected EOF reading array (looking for ']')"sv}};
                Execution::Throw (kException_);
            }
            char32_t peekedChar = in.Peek ()->As<char32_t> ();
            if (peekedChar == ']') {
                if (lookingForElt) {
                    // allow ending ',' - harmless - could  be more aggressive - but if so - careful of zero-sized array special case
                }
                in.AdvanceOne ();
                return VariantValue{Containers::Concrete::Sequence_stdvector<VariantValue>{move (result)}};
            }
            else if (peekedChar == ',') {
                if (lookingForElt) [[unlikely]] {
                    static const auto kException_{BadFormatException{"JSON: Unexpected second ',' in reading array"sv}};
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
                    static const auto kException_{BadFormatException{"JSON: Unexpected character (missing ',' ?) in reading array"sv}};
                    Execution::Throw (kException_);
                }
            }
        }
    }

    VariantValue Reader_SpecialToken_ (char32_t initialChar, MyBufferedStreamReader_& in)
    {
        switch (initialChar) {
            case 'f': {
                Character buf[4];
                if (in.ReadAll (begin (buf), end (buf)) == 4 and buf[0] == 'a' and buf[1] == 'l' and buf[2] == 's' and buf[3] == 'e') {
                    return VariantValue{false};
                }
            } break;
            case 't': {
                Character buf[3];
                if (in.ReadAll (begin (buf), end (buf)) == 3 and buf[0] == 'r' and buf[1] == 'u' and buf[2] == 'e') {
                    return VariantValue{true};
                }
            } break;
            case 'n': {
                Character buf[3];
                if (in.ReadAll (begin (buf), end (buf)) == 3 and buf[0] == 'u' and buf[1] == 'l' and buf[2] == 'l') {
                    return VariantValue{};
                }
            } break;
        }
        static const auto kException_{BadFormatException{"JSON: Unrecognized token"sv}};
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
            switch (oc->As<char32_t> ()) {
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
                    return Reader_Number_ (oc->As<char32_t> (), in);

                case '{':
                    return Reader_Object_ (in);
                case '[':
                    return Reader_Array_ (in);

                case 't':
                case 'f':
                case 'n':
                    return Reader_SpecialToken_ (oc->As<char32_t> (), in);

                default: {
                    if (IsJSONSpace_ (oc->As<char32_t> ())) [[likely]] {
                        // ignore
                    }
                    else {
                        static const auto kException_{BadFormatException{"JSON: Unexpected character looking for start of value"sv}};
                        Execution::Throw (kException_);
                    }
                }
            }
        }
        // if we get here - nothing found
        static const auto kException_{BadFormatException{"JSON: Unexpected EOF looking for value"sv}};
        Execution::Throw (kException_);
    }
}

/*
 ********************************************************************************
 ************************* Variant::JSON::Reader ********************************
 ********************************************************************************
 */
class Variant::JSON::Reader::NativeRep_ : public Variant::Reader::_IRep {
public:
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<NativeRep_> (); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        static const String kResult_ = ".json"sv;
        return kResult_;
    }
    virtual VariantValue Read (const Streams::InputStream::Ptr<byte>& in) override
    {
        using namespace Streams;
        return Read (TextReader::New (in, nullopt, SeekableFlag::eSeekable));
    }
    virtual VariantValue Read (const Streams::InputStream::Ptr<Characters::Character>& in) override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::JSON::Reader::NativeRep_::Read"};
#endif
        Require (in.IsSeekable ());
        MyBufferedStreamReader_ reader{in};
        return Reader_value_ (reader);
    }
};
#if __has_include("boost/json.hpp")

namespace {
    // Based on example: https://www.boost.org/doc/libs/1_83_0/libs/json/doc/html/json/examples.html#json.examples.validate
    struct BoostSAXHandler_ {
        /// The maximum number of elements allowed in an array
        static constexpr std::size_t max_array_size = static_cast<size_t> (-1);

        /// The maximum number of elements allowed in an object
        static constexpr std::size_t max_object_size = static_cast<size_t> (-1);

        /// The maximum number of characters allowed in a string
        static constexpr std::size_t max_string_size = static_cast<size_t> (-1);

        /// The maximum number of characters allowed in a key
        static constexpr std::size_t max_key_size = static_cast<size_t> (-1);

        BoostSAXHandler_ ()                        = default;
        BoostSAXHandler_ (const BoostSAXHandler_&) = delete;

        bool on_document_begin ([[maybe_unused]] error_code& ec)
        {
            Assert (fStack_.empty ());
            fStack_.emplace (Context_::eSimple);
            return true;
        }
        bool on_document_end ([[maybe_unused]] error_code& ec)
        {
            Assert (fStack_.size () == 1);
            this->PopContext_ ();
            Assert (fStack_.empty ());
            return true;
        }

        bool on_array_begin ([[maybe_unused]] error_code& ec)
        {
            fStack_.emplace (Context_::eArray);
            return true;
        }
        bool on_array_end ([[maybe_unused]] std::size_t n, [[maybe_unused]] error_code& ec)
        {
            Assert (fStack_.top ().GetContextType () == Context_::eArray);
            Assert (fStack_.top ().PeekAccumVector_ ().size () == n);
            PopContext_ ();
            return true;
        }

        bool on_object_begin ([[maybe_unused]] error_code& ec)
        {
            fStack_.emplace (Context_::eMap);
            return true;
        }
        bool on_object_end ([[maybe_unused]] std::size_t n, [[maybe_unused]] error_code& ec)
        {
            Assert (fStack_.top ().GetContextType () == Context_::eMap);
            Assert (fStack_.top ().PeekAccumObj_ ().size () == n);
            PopContext_ ();
            return true;
        }

        bool on_string_part (string_view s, [[maybe_unused]] std::size_t n, [[maybe_unused]] error_code& ec)
        {
            fPartSaver_.push_back (span<const char>{s});
            return true;
        }
        bool on_string (string_view s, [[maybe_unused]] std::size_t n, [[maybe_unused]] error_code& ec)
        {
            // Tricky - not really documented (I can find) - but seems if n != s.size() - we must use fPartSaver from on_key_part
            if (s.size () == n) {
                AddCompleteValue_ (VariantValue{toStroikaString_ (s)});
            }
            else {
                fPartSaver_.push_back (span<const char>{s});
                String res = toStroikaString_ (fPartSaver_);
                fPartSaver_.clear ();
                AddCompleteValue_ (VariantValue{res});
            }
            return true;
        }

        bool on_key_part (string_view s, [[maybe_unused]] std::size_t n, [[maybe_unused]] error_code& ec)
        {
            // tricky - save text in buffer, for use on subsequent onKey
            Assert (fStack_.top ().GetContextType () == Context_::eMap);
            fPartSaver_.push_back (span<const char>{s});
            return true;
        }
        bool on_key (string_view s, [[maybe_unused]] std::size_t n, [[maybe_unused]] error_code& ec)
        {
            Assert (fStack_.top ().GetContextType () == Context_::eMap);
            Assert (s.size () <= n);
            // Tricky - not really documented (I can find) - but seems if n != s.size() - we must use fPartSaver from on_key_part
            if (s.size () == n) {
                fStack_.top ().fKey = toStroikaString_ (s);
            }
            else {
                fPartSaver_.push_back (span<const char>{s});
                fStack_.top ().fKey = toStroikaString_ (fPartSaver_);
                fPartSaver_.clear ();
            }
            return true;
        }

        bool on_number_part ([[maybe_unused]] string_view s, [[maybe_unused]] error_code& ec)
        {
            // No need to track anything for numbers, as boost appears to incrementally parse and keep its state internally
            return true;
        }
        bool on_int64 (int64_t i, [[maybe_unused]] string_view s, [[maybe_unused]] error_code& ec)
        {
            AddCompleteValue_ (VariantValue{i});
            return true;
        }
        bool on_uint64 (uint64_t u, [[maybe_unused]] string_view s, [[maybe_unused]] error_code& ec)
        {
            AddCompleteValue_ (VariantValue{u});
            return true;
        }
        bool on_double (double d, [[maybe_unused]] string_view s, [[maybe_unused]] error_code& ec)
        {
            AddCompleteValue_ (VariantValue{d});
            return true;
        }

        bool on_bool (bool b, [[maybe_unused]] error_code& ec)
        {
            AddCompleteValue_ (VariantValue{b});
            return true;
        }

        bool on_null ([[maybe_unused]] error_code& ec)
        {
            AddCompleteValue_ (VariantValue{});
            return true;
        }

        bool on_comment_part ([[maybe_unused]] string_view s, [[maybe_unused]] error_code& ec)
        {
            // ignore comments
            return true;
        }
        bool on_comment ([[maybe_unused]] string_view s, [[maybe_unused]] error_code& ec)
        {
            // ignore comments
            return true;
        }

        // Careful not to use string_view directly cuz Stroika assumes we can keep that pointer (String_Constant) - so map to span, and also character to use char8_t
        // to signal character set, and not ASCII
        template <typename CONTAINER_OF_CHAR_BUT_REALLY_UTF8>
        static String toStroikaString_ (CONTAINER_OF_CHAR_BUT_REALLY_UTF8 sv)
            requires requires (CONTAINER_OF_CHAR_BUT_REALLY_UTF8 t) {
                {
                    span<const char>{t}
                };
            }
        {
            return String{Memory::SpanReInterpretCast<const char8_t> (span<const char>{sv})};
        }

        VariantValue GetConstructedValue () const
        {
            return fCompletedFinalValue_;
        }
        void AddCompleteValue_ (VariantValue v)
        {
            Assert (not fStack_.empty ());
            Context_& t = fStack_.top ();
            switch (t.GetContextType ()) {
                case Context_::eArray:
                    t.PeekAccumVector_ ().push_back (v);
                    break;
                case Context_::eMap:
                    t.PeekAccumObj_ ().insert ({t.fKey, v});
                    break;
                case Context_::eSimple:
                    t.PeekSimpleValue_ () = v;
                    break;
                default:
                    AssertNotReached ();
            }
        }
        void PopContext_ ()
        {
            Assert (not fStack_.empty ());
            // complete what is at the top of the stack and do AddCompleteValue_ to the new top of the stack
            Context_&    t  = fStack_.top ();
            VariantValue vv = [&t] () {
                switch (t.GetContextType ()) {
                    case Context_::eArray:
                        return VariantValue{move (t.PeekAccumVector_ ())};
                    case Context_::eMap:
                        return VariantValue{Mapping_stdhashmap<String, VariantValue>{move (t.PeekAccumObj_ ())}};
                    case Context_::eSimple:
                        return t.PeekSimpleValue_ ();
                    default:
                        AssertNotReached ();
                        return VariantValue{};
                }
            }();
            fStack_.pop ();
            if (fStack_.empty ()) {
                fCompletedFinalValue_ = vv;
            }
            else {
                AddCompleteValue_ (vv);
            }
        }

        /*
         *  We have a stack of context objects for in progress parses. This cheaply maintains the data at each point of the stack.
         */
        struct Context_ {

            // NOTE - critical these enums correspond to index values of std::variant<> below
            enum ContextType_ {
                eSimple,
                eArray,
                eMap
            };

            Context_ () = delete;
            Context_ (ContextType_ ct)
            {
                switch (ct) {
                    case eSimple:
                        fVV_ = VariantValue{};
                        break;
                    case eArray:
                        fVV_ = vector<VariantValue>{};
                        break;
                    case eMap:
                        fVV_ = Mapping_stdhashmap<String, VariantValue>::STDHASHMAP<>{};
                        break;
                }
                Ensure (ct == GetContextType ()); // ensure ContextType_ enum in same order as variant<> arguments
            }
            ~Context_ ()               = default;
            Context_ (const Context_&) = delete;
            Context_ (Context_&&)      = default;

            ContextType_ GetContextType () const
            {
                return static_cast<ContextType_> (fVV_.index ());
            }

            vector<VariantValue>& PeekAccumVector_ ()
            {
                Require (GetContextType () == eArray);
                return get<vector<VariantValue>> (fVV_);
            }
            Mapping_stdhashmap<String, VariantValue>::STDHASHMAP<>& PeekAccumObj_ ()
            {
                Require (GetContextType () == eMap);
                return get<Mapping_stdhashmap<String, VariantValue>::STDHASHMAP<>> (fVV_);
            }
            VariantValue& PeekSimpleValue_ ()
            {
                Require (GetContextType () == eSimple);
                return get<VariantValue> (fVV_);
            }

            // use variant to save construct/destruct of uneeded parts
            variant<VariantValue, vector<VariantValue>, Mapping_stdhashmap<String, VariantValue>::STDHASHMAP<>> fVV_;

            String fKey; // only allowed of context type = eMap (so COULD embed in above variant, but KISS) - also could use optional<String> which would help some things and make others worse...
        };

        std::stack<Context_> fStack_;
        VariantValue         fCompletedFinalValue_; // only filled in when stack is empty

        // doesn't need to be in stack context cuz cannot fill partial string/key/etc with intervening pop/push
        // Not using StringBuilder here cuz could contain partial strings
        Memory::InlineBuffer<char, 512> fPartSaver_;
    };
}
class Variant::JSON::Reader::BoostRep_ : public Variant::Reader::_IRep {
public:
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<BoostRep_> (); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        static const String kResult_ = ".json"sv;
        return kResult_;
    }
    virtual VariantValue Read (const Streams::InputStream::Ptr<byte>& in) override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::JSON::Reader::BoostRep_::Read"};
#endif
        using namespace Streams;
        using namespace boost;
        bool           inSeekable = in.IsSeekable ();
        constexpr bool kUseSAX_   = true; // experimentally, on windows, sax about 10% faster than stream_parser/convert way
        try {
            if constexpr (kUseSAX_) {
                json::basic_parser<BoostSAXHandler_> p{json::parse_options{}};
                byte                                 buf[8 * 1024]; // experimentally - larger buffers didn't help speed
                const size_t                         targetChunkSize = inSeekable ? Memory::NEltsOf (buf) : 1;
                size_t                               actualChunkSize;
                boost::system::error_code            ec;
                while ((actualChunkSize = in.Read (span{buf, targetChunkSize}).size ()) != 0) {
                    ec.clear ();
                    size_t nParsed = p.write_some (true, reinterpret_cast<const char*> (begin (buf)), actualChunkSize, ec);
                    Assert (nParsed <= actualChunkSize);
                    if (nParsed < actualChunkSize) {
                        in.Seek (Whence::eFromCurrent, static_cast<SignedSeekOffsetType> (nParsed) - static_cast<SignedSeekOffsetType> (actualChunkSize));
                        break;
                    }
                    if (p.done ()) {
                        break; // good parse
                    }
                    else if (ec) {
                        Execution::Throw (DataExchange::BadFormatException{String::FromNarrowSDKString (ec.what ())});
                    }
                }
                if (not p.done ()) {
                    p.write_some (false, nullptr, 0, ec);
                    if (ec and ec != boost::json::error::extra_data /*and ec != boost::json::error::incomplete*/) {
                        Execution::Throw (DataExchange::BadFormatException{String::FromNarrowSDKString (ec.what ())});
                    }
                }
                return p.handler ().GetConstructedValue ();
            }
            else {
                json::stream_parser p;
                byte                buf[8 * 1024];
                const size_t        targetChunkSize = inSeekable ? Memory::NEltsOf (buf) : 1;
                size_t              actualChunkSize;
                while ((actualChunkSize = in.Read (span{buf, targetChunkSize}).size ()) != 0) {
                    boost::system::error_code ec;
                    size_t                    nParsed = p.write_some (reinterpret_cast<const char*> (begin (buf)), actualChunkSize, ec);
                    Assert (nParsed <= actualChunkSize);
                    if (nParsed < actualChunkSize) {
                        in.Seek (Whence::eFromCurrent, static_cast<SignedSeekOffsetType> (nParsed) - static_cast<SignedSeekOffsetType> (actualChunkSize));
                        break;
                    }
                    if (p.done ()) {
                        break; // good parse
                    }
                    else if (ec) {
                        Execution::Throw (DataExchange::BadFormatException{String::FromNarrowSDKString (ec.what ())});
                    }
                }
                if (not p.done ()) {
                    p.finish (); // in case wrote text like '3' to buffer, ambiguous if done
                }
                Assert (p.done ());
                return DataExchange::VariantValue{p.release ()}; // Transform boost objects to Stroika objects
            }
        }
        catch (...) {
            Execution::Throw (DataExchange::BadFormatException{Characters::ToString (current_exception ())});
        }
    }
    virtual VariantValue Read (const Streams::InputStream::Ptr<Characters::Character>& in) override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::JSON::Reader::BoostRep_::Read"};
#endif
        Require (in.IsSeekable ());
        using namespace Streams;
        return Read (_ToByteReader (in));
    }
};
#endif

inline auto Variant::JSON::Reader::mk_ (const ReaderOptions& options) -> shared_ptr<_IRep>
{
    switch (options.fPreferredAlgorithm.value_or (ReaderOptions::Algorithm::eDEFAULT)) {
        case ReaderOptions::Algorithm::eStroikaNative:
            return make_shared<NativeRep_> ();
#if __has_include("boost/json.hpp")
        case ReaderOptions::Algorithm::eBoost:
            return make_shared<BoostRep_> ();
#endif
        default:
            AssertNotReached ();
            return nullptr;
    }
}

Variant::JSON::Reader::Reader (const ReaderOptions& options)
    : inherited{mk_ (options)}
{
}
