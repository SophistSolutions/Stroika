/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_h_
#define _Stroika_Foundation_Characters_String_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <locale>
#include <string>
#include <string_view>

#include "../Common/Compare.h"
#include "../Containers/Sequence.h"
#include "../Containers/Set.h"
#include "../Execution/Synchronized.h"
#include "../Memory/SharedByValue.h"
#include "../Memory/StackBuffer.h"
#include "../Traversal/Iterable.h"
#include "SDKString.h"

#include "Character.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   EXPLAIN why InsertAt () returns string and Append() doesn't! Or - change it!
 *              Basic idea is that append is SO convnentit (+=) - that we just must support that
 *              and it can be done safely.
 *
 *              But InsertAt () is less common, and so we can just encourage the more safe strategy of
 *              returning a new string.
 *
 *              Weak arguemnt! BUt best I can come up with...
 *
 *      @todo   Cleanup SubString (), and String::SubString_ use of SharedByValue<TRAITS>::ReadOnlyReference for
 *              performance. At some level - in String::SubString_ - we have a (hidden) sharedPtr and it would
 *              be safe and performant in that case to re-use that shared_ptr to make a new String envelope.
 *
 *              However, I'm not sure its safe in general to have SharedByValue<TRAITS>::ReadOnlyReference expose
 *              its shared_ptr, which appears needed to make this happen.
 *
 *              Not a biggie opportunity, so we can delay this -- LGP 2014-04-10
 *
 *      @todo   Change APIs that return vector to return Iterable<> using CreateGenerator (). Tried once and worked
 *              very nicely.
 *
 *      @todo   Add PadLeft/PadRight or FillLeft/FilLRight() - not sure which name is better. But idea is to
 *              produce a string which is indentical to the orig except that IF start len < n, then expand it with
 *              the given arg char repeated on the left or right.
 *
 *      @todo   RFind() API should be embellished to include startAt etc, like regular Find () - but not 100%
 *              sure - think through...
 *
 *      @todo   MAYBE also add ReplaceOne() function (we have ReplaceAll() now).
 *
 *      @todo   Add overload for ReplaceAll() where first arg is a lambda on a character, so easier to generalzie to replace all whitespace, etc.
 *
 *      @todo   Move DOCS in the top of this file down to the appropriate major classes - and then review the implemantion and make sure
 *              it is all correct for each (especially SetStorage () sutff looks quesitonable)
 *
 *      @todo   String::Remove()
 *              CRAPPY API - probably delete or redo. See if ever used!!! RemoveAll or RemoveFirst() would be
 *              better names (thogh all woudl be a change in functionality) - and maybe return a bool so
 *              you can tell if it did anyhting? Also - make case match an optional param.
 *
 *      @todo   Use new CopyTo() method to get rid of MOST of the casts/memcpy code in the implementation
 *
 *      @todo   Add Ranged insert public envelope API, and add APPEND (not just operator+) API. See/maybe use new
 *              Stroika Range type?
 *
 *      @todo   Add NormalizeSpace (Character useSpaceCharacter = ' ');
 *              see Qt 'QString::simplify()'. Idea is Trim() (right and left) - plus replace contiguous substrings with
 *              Character::IsSpace() with a single (given) space character.
 *
 *      @todo   Compare
 *          template    <typename TCHAR>
 *              basic_string<TCHAR> RTrim (const basic_string<TCHAR>& text)
 *                  {
 *                      std::locale loc1;   // default locale
 *                      const ctype<TCHAR>& ct = use_facet<ctype<TCHAR>>(loc1);
 *                      typename basic_string<TCHAR>::const_iterator i = text.end ();
 *                      for (; i != text.begin () and ct.is (ctype<TCHAR>::space, *(i-1)); --i)
 *                          ;
 *                      return basic_string<TCHAR> (text.begin (), i);
 *                  }
 *          with the TRIM() implementation I wrote here - in String. Not sure we want to use the locale stuff? Maybe?
 */

namespace Stroika::Foundation::Containers {
    template <typename T>
    class Set;
}

namespace Stroika::Foundation::Characters {

    class RegularExpression;
    class RegularExpressionMatch;

    /**
     *  \brief returns true iff T == u8string, u16string, u32string, or wstring
     */
    template <typename T>
    concept IBasicUNICODEStdString = is_same_v<T, u8string> or is_same_v<T, u16string> or is_same_v<T, u32string> or is_same_v<T, wstring>;

    /**
     *  \brief String is like std::u32string, except it is much easier to use, often much more space efficient, and more easily interoperates with other string types
     * 
     *  The Stroika String class is conceptually a sequence of (UNICODE) Characters, and so there is
     *  no obvious way to map the Stroika String to a std::string (in general). However, if you specify a codepage
     *  for conversion, or are converting to/from SDKString/SDKChar, or u8string, etc, there is builtin support for that.
     *
     *  EOS Handling:
     *      The Stroika String class supports having embedded NUL-characters. It also supports
     *      easy construction from NUL-terminated character strings.
     * 
     *      There is an API - c_str () - to return a NUL-terminated wchar_t C-String (**SOMETIMES - AS OF v3 - TBD**).
     *
     *      About spans, and the \0 NUL-termination - generally do NOT include
     *      the NUL-character in your span! Stroika strings will allow this, and treat
     *      it as just another character, but its probably not what you meant.
     * 
     *  \note Narrow String handling
     *      Becuase the character set of strings of type 'char' is ambiguous, if you construct a String
     *      with char (char* etc) - it is somehow 'required' that the characters be ASCII. If using the FromConstantString () API
     *      , or operator"" _k, it is checked with Require () - so assertion failure. If you construct
     *      with String::CTOR, it will generate a runtime exception (so more costly runtime checking).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   Design note - mutability vs. immutability
     *          https://stroika.atlassian.net/browse/STK-968
     *          String objects are MUTABLE
     *          String reps are IMMUTABLE.
     *          Changes to string like +=, create new string reps (and so WORK but are quite costly). Consider
     *          Use StringBuilder for that purpose in performance sensative code.
     *
     *          TODO WRITEUP AND MAYBE MAKE IMMUTABLE
     * 
     *          Current Mutating methods (as of v3.0d1x)
     *          o   SetCharAt
     *          o   c_str()
     *          o   operator=
     *          o   clear()
     *          o   Append
     *          o   operator+=
     *          o   c_str ()  -- non-const
     *          o   erase()
     *
     *          SOMEWHAT ironically, the only of these methods hard to replace is the non-const c_str () - and maybe there
     *          not bad cuz I deprecated? COULD just deprecate ALL of these, and then the class is fully immutable. Probably
     *          easier to udnerstand/reason about.
     * 
     *          @todo CONSIDER LOSIING THESE METHODS ABOVE (or deprecating at leats)
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *      o   String::EqualsComparer, String::ThreeWayComparer and String::LessComparer provided with construction parameters to allow case insensitive compares 
     */
    class [[nodiscard]] String : public Traversal::Iterable<Character> {
    private:
        using inherited = Iterable<Character>;

    public:
        /**
         * All the constructors are obvious, except
         *      o   NUL-character ARE allowed in strings, except for the case of single
         *          charX* argument constructors - which find the length based on
         *          the terminating NUL-character.
         *
         *  \note about lifetime of argument data (basic_string_view<CHAR_T> constructors)
         *        All data is copied out / saved by the end of the constructor for all constructors EXCEPT
         *        the basic_string_view<CHAR_T> constructors - where it is REQUIRED the data last 'forever'.
         * 
         *  \req for String (const basic_string_view<wchar_t>& str) - str[str.length()]=='\0';   
         *       c-string nul-terminated (which happens automatically with L"xxx"sv)
         * 
         *  \note 'char' (using ASCII = char) constructors:
         *        Becuase the characterset of strings of type 'char' is ambiguous, if you construct a String
         *        with char (char* etc) - it runtime checked that the characters are ASCII (except for the basic_string_view
         *        constructors where we check but with assertions).
         * 
         *        This mimics the behavior in Stroika v2.1 with String::FromASCII ()
         * 
         *  \note the basic_string move Constructors MAY move or copy the underlying std string, but they still maintain
         *        the same requirements on their arguments as the copy basic_string constructors (eg. char must be ascii)
         * 
         *  \see also - FromUTF8, FromSDKString, FromNarrowSDKString, FromStringConstant, FromLatin1, which are all like constructors
         *       but with special names to avoid confusion and make clear their arguments, and not participate in overloading. Note, chose
         *       this path instead of FLAG argument and explicit on CTOR, cuz more terse.
         *
         *  TODO:
         *      @todo   basic_string_view<> UNICODE constructors - like char8_t etc, should also use STRINGCONSTANT code and avoid copying.
         */
        String ();
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        String (const CHAR_T* cString);
        template <Memory::ISpanT SPAN_OF_CHAR_T>
        String (SPAN_OF_CHAR_T s)
            requires (IUNICODECanUnambiguouslyConvertFrom<typename SPAN_OF_CHAR_T::value_type>);
        template <IStdBasicStringCompatibleCharacter CHAR_T>
        String (const basic_string<CHAR_T>& s);
        template <IStdBasicStringCompatibleCharacter CHAR_T>
        String (const basic_string_view<CHAR_T>& s);
        template <IStdBasicStringCompatibleCharacter CHAR_T>
        explicit String (basic_string<CHAR_T>&& s);
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        String (const Iterable<CHAR_T>& src)
            requires (not Memory::ISpanT<CHAR_T>);
        explicit String (const Character& c);
        String (String&& from) noexcept      = default;
        String (const String& from) noexcept = default;

    public:
#if qCompilerAndStdLib_templateConstructorSpecialization_Buggy
        String (const basic_string_view<char>& str); // char==ASCII
        String (const basic_string_view<char8_t>& str);
        String (const basic_string_view<char16_t>& str);
        String (const basic_string_view<char32_t>& str);
        String (const basic_string_view<wchar_t>& str);
#else
        template <>
        String (const basic_string_view<char>& str); // char==ASCII
        template <>
        String (const basic_string_view<char8_t>& str);
        template <>
        String (const basic_string_view<char16_t>& str);
        template <>
        String (const basic_string_view<char32_t>& str);
        template <>
        String (const basic_string_view<wchar_t>& str);
#endif

    public:
        ~String () = default;

    protected:
        class _IRep;

    protected:
        /**
         */
        using _SafeReadRepAccessor = Iterable<Character>::_SafeReadRepAccessor<_IRep>;

    protected:
        /**
         * \req rep MUST be not-null
         *  However, with move constructor, it maybe null on exit.
         */
        String (const shared_ptr<_IRep>& rep) noexcept;
        String (shared_ptr<_IRep>&& rep) noexcept;

    public:
        nonvirtual String& operator= (String&& rhs) noexcept      = default;
        nonvirtual String& operator= (const String& rhs) noexcept = default;

    public:
        /**
         *  Create a String object from a 'char-based' utf-8 encoded string.
         *
         *  \par Example Usage
         *      \code
         *          VerifyTestResult (string{u8"שלום"} == String::FromUTF8 (u8"שלום").AsUTF8 ());
         *      \endcode
         *
         *  \note   Reading improperly encoded text may result in a RuntimeException indicating improperly encoded characters.
         */
        template <typename CHAR_T>
        static String FromUTF8 (span<CHAR_T> from)
            requires (is_same_v<remove_cv_t<CHAR_T>, char8_t> or is_same_v<remove_cv_t<CHAR_T>, char>);
        template <typename CHAR_T>
        static String FromUTF8 (basic_string<CHAR_T> from)
            requires (is_same_v<remove_cv_t<CHAR_T>, char8_t> or is_same_v<remove_cv_t<CHAR_T>, char>);
        template <typename CHAR_T>
        static String FromUTF8 (const CHAR_T* from)
            requires (is_same_v<remove_cv_t<CHAR_T>, char8_t> or is_same_v<remove_cv_t<CHAR_T>, char>);

    public:
        /**
         *  Create a String object from a 'SDKChar' (os-setting - current code page) encoded string.
         *  See @SDKChar
         *  See @SDKString
         *
         *  \note   Reading improperly encoded text may result in a RuntimeException indicating improperly encoded characters.
         */
        static String FromSDKString (const SDKChar* from);
        static String FromSDKString (span<const SDKChar> s);
        static String FromSDKString (const SDKString& from);

    public:
        /**
         *  Create a String object from a 'char-based' (os-setting - current code page) encoded string.
         *
         *  \note   Reading improperly encoded text may result in a RuntimeException indicating improperly encoded characters.
         */
        static String FromNarrowSDKString (const char* from);
        static String FromNarrowSDKString (span<const char> s);
        static String FromNarrowSDKString (const string& from);

    public:
        /**
         *  Create a String object from a char based on the encoding from the argument locale.
         *  This throws an exception if there is an error performing the conversion.
         *
         *  \note   Reading improperly encoded text may result in a RuntimeException indicating improperly encoded characters.
         */
        static String FromNarrowString (const char* from, const locale& l);
        static String FromNarrowString (span<const char> s, const locale& l);
        static String FromNarrowString (const string& from, const locale& l);

    public:
        /**
         *   \brief Take the given argument data (constant span) - which must remain unchanged - constant - for the application lifetime - and treat it as a Stroika String object
         * 
         * This allows creation of String objects with fewer memory allocations, and more efficient storage, but only in constrained situations
         *
         *  The resulting String is a perfectly compliant Stroika String (somewhat akin to std::string_view vs std::string).
         *
         *  \par Example:
         *      \code
         *          String  tmp1    =   "FRED";
         *          String  tmp2    =   String{"FRED"};
         *          String  tmp3    =   String::FromStringConstant ("FRED");    // same as 2 above, but faster
         *          String  tmp4    =   "FRED"sv;                               // equivilent to FromStringConstant
         *          String  tmp5    =   "FRED"_k;                               // equivilent to FromStringConstant
         *      \endcode
         *
         *  \em WARNING - BE VERY CAREFUL - be sure arguments have application lifetime.
         * 
         *  \req argument string must be nul-terminated (but CAN contain additional embedded nul characters)
         * 
         *  \note In Stroika v2.1 this was called class String_ExternalMemoryOwnership_ApplicationLifetime.
         *  \note In Stroika v2.1 this was called class String_Constant.
         * 
         *  The constructor requires an application lifetime NUL-terminated array of characters - such as one
         *  created with "sample" (but allows embedded NUL-characters).
         *
         *  \req ((str.data () + str.size ()) == '\0'); // crazy weird requirement, but done cuz L"x"sv already does NUL-terminate and we can
         *                                              // take advantage of that fact - re-using the NUL-terminator for our own c_str() implementation
         * 
         *  \note FromStringConstant with 'char' - REQUIRES that the char elements are ASCII (someday this maybe lifted and iterpret as Latin1)
         *        For the case of char, we also do not check/require the nul-termination bit.
         * 
         *  \note for overloads with wchar_t, if sizeof (wchar_t) == 2
         *        \req Require (UTFConvert::AllFitsInTwoByteEncoding (s));
         */
        template <size_t SIZE>
        static String FromStringConstant (const ASCII (&cString)[SIZE]);
        template <size_t SIZE>
        static String FromStringConstant (const wchar_t (&cString)[SIZE]);
        static String FromStringConstant (const basic_string_view<ASCII>& str);
        static String FromStringConstant (const basic_string_view<wchar_t>& str);
        static String FromStringConstant (const basic_string_view<char32_t>& str);
        static String FromStringConstant (span<const ASCII> s);
        static String FromStringConstant (span<const wchar_t> s);
        static String FromStringConstant (span<const char32_t> s);

    public:
        /**
         *  Create a String object from UNICODE Latin-1 Supplement (https://en.wikipedia.org/wiki/Latin-1_Supplement)
         *
         *  This is roughly, but not exactly, the same as the ISO-Latin-1 single-byte characterset (https://en.wikipedia.org/wiki/ISO/IEC_8859-1)
         *
         *  \note if character code point >= 256, this will throw an exception - not defined for that range (only checked if sizeof (CHAR_T) > 1)
         *
         *  \note Alias From8bitASCII () or FromExtendedASCII ()
         */
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static String FromLatin1 (const CHAR_T* cString);
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static String FromLatin1 (span<const CHAR_T> s);
        template <IStdBasicStringCompatibleCharacter CHAR_T>
        static String FromLatin1 (const basic_string<CHAR_T>& s);

    public:
        /**
         *  \brief appends 'rhs' string to this string (without modifying this string) and returns the combined string
         *
         *  @see    Append() for a similar function that modifies 'this'
         */
        template <typename T>
        nonvirtual String Concatenate (T&& rhs) const
            requires (is_convertible_v<T, String>);

    private:
        nonvirtual String Concatenate_ (const String& rhs) const;

    public:
        /**
         *  Returns the number of characters in the String. Note that this may not be the same as bytes,
         *  does not include NUL termination, and doesn't in any way respect NUL termination (meaning
         *  a nul-character is allowed in a Stroika string.
         * 
         * \note Alias GetLength ()
         */
        nonvirtual size_t size () const noexcept;

    public:
        /**
         */
        nonvirtual bool empty () const noexcept;

    public:
        /**
         *  Alias for *this = String{};
         */
        nonvirtual void clear ();

    public:
        /**
         */
        nonvirtual const Character GetCharAt (size_t i) const noexcept;

    public:
        /**
         *  \brief - PREFER USING StringBuilder if you are using this. This is very slow;
         *  \todo Consider losing this method...
         */
        nonvirtual void SetCharAt (Character c, size_t i);

    public:
        /**
         *  \brief return (read-only) Character object
         *
         *  Alias for GetCharAt (size_t i) const;
         */
        nonvirtual const Character operator[] (size_t i) const noexcept;

    public:
        /**
         *  InsertAt() constructs a new string by taking this string, and inserting the argument
         *  characters.
         *
         *  \em Note that for repeated insertions, this is much less efficient than just
         *      using StringBuilder.
         * 
         *  \note that if at == this->size (), you are appending.
         */
        nonvirtual String InsertAt (Character c, size_t at) const;
        nonvirtual String InsertAt (const String& s, size_t at) const;
        nonvirtual String InsertAt (span<const Character> s, size_t at) const;
        nonvirtual String InsertAt (span<Character> s, size_t at) const;

    public:
        /**
         *  Append() adds the given argument characters to the end of this string object.
         *
         *  Unlike InsertAt() - this modifies 'this' string, rather than returning a new one.
         *
         *  \em Note that for repeated insertions, this is much less efficient than just
         *      using StringBuilder.
         *
         *  \em Note that it is legal, but pointless to pass in an empty string to insert
         *
         *  @see    Concatenate() for a similar function that doesn't modify the source
         * 
         *  \todo CONSIDER DEPRECATING/LOSING THIS API - right now its NOT performant, but could easily be tweaked. Just better to use StringBuilder... Also API could
         *        be generalized like CTOR - with appenbding any 'convertible to string' type, like char16_t, etc...
         */
        nonvirtual void Append (Character c);
        nonvirtual void Append (const String& s);
        nonvirtual void Append (const wchar_t* s);
        nonvirtual void Append (const wchar_t* from, const wchar_t* to);
        nonvirtual void Append (const Character* from, const Character* to);
        template <typename CHAR_T>
        nonvirtual void Append (span<const CHAR_T> s)
            requires (is_same_v<CHAR_T, Character> or is_same_v<CHAR_T, char32_t>);

    public:
        nonvirtual String& operator+= (Character appendage);
        nonvirtual String& operator+= (const String& appendage);
        nonvirtual String& operator+= (const wchar_t* appendageCStr);

    public:
        /**
         * Remove the characters at 'charAt' (RemoveAt/1) or between 'from' and 'to' (const method - doesn't modify this)
         *
         * It is an error if this implies removing characters off the end of the string.
         *
         *  \par Example Usage
         *      \code
         *          String mungedData = "04 July 2014";
         *          if (optional<pair<size_t, size_t>> i = mungedData.Find (RegularExpression{"0[^\b]"})) {
         *              mungedData = mungedData.RemoveAt (*i);
         *          }
         *      \endcode
         *
         *  \req (charAt < size ())
         *  \req (from <= to)
         *  \req (to <= size ())
         *
         *  \em Note that this is quite inefficent: consider using StringBuilder
         */
        nonvirtual String RemoveAt (size_t charAt) const;
        nonvirtual String RemoveAt (size_t from, size_t to) const;
        nonvirtual String RemoveAt (pair<size_t, size_t> fromTo) const;

    public:
        /**
         *  Remove the first occurence of Character 'c' from the string. Not an error if none
         *  found. Doesn't modify this (const method) - returns resulting string.
         *
         *  \em Note that this is quite inefficent: consider using StringBuffer (@todo is that the right name)???
         */
        nonvirtual String Remove (Character c) const;

    public:
        /**
         *  OVERLOADS WITH size_t:
         *
         *  Produce a substring of this string, starting at 'from', and up to 'to' (or end of string
         *  for one-arg overload).
         *
         *  *NB* This function treats the second argument differently than String::substr () -
         *  which respects the STL basic_string API. This function treats the second argument
         *  as a 'to', STL substr() treats it as a count. This amounts to the same thing for the
         *  very common cases of substr(N) - because second argument is defaulted, and,
         *  substr (0, N) - because then the count and end are the same.
         *
         *  \req  (from <= to);
         *  \req  (to <= size ());     // for 2-arg variant
         *
         *  \par Example Usage
         *      \code
         *          String tmp { "This is good" };
         *          Assert (tmp.SubString (5) == "is good");
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          const String kTest_ { "a=b"sv };
         *          const String kLbl2LookFor_ { "a="_k };
         *          if (resultLine.Find (kLbl2LookFor_)) {
         *              String  tmp { resultLine.SubString (kLbl2LookFor_.length ()) };
         *          }
         *          Assert (tmp == "b");
         *      \endcode
         *
         *  OVERLOADS WITH ptrdiff_t:
         *
         *  This is like SubString() except that if from/to are negative, they are treated as relative to the end
         *  of the String.
         *
         *  So for example, SubString (0, -1) is equivalent to SubString (0, size () - 1) - and so is an
         *  error if the string is empty.
         *
         *  Similarly, SubString (-5) is equivalent to SubString (size ()-5, size ()) - so can be used
         *  to grab the end of a string.
         *
         *  \req  (adjustedFrom <= adjustedTo);
         *  \req  (adjustedTo <= size ());     // for 2-arg variant
         *
         *  \note \em Design Note
         *      We chose not to overload SubString() with this functionality because it would have been to easy
         *      to mask bugs.
         *
         *  \note \em Design Note
         *      This was originally inspired by Python arrays. From https://docs.python.org/2/tutorial/introduction.html:
         *          Indices may also be negative numbers, to start counting from the right
         *
         *  \note \em Alias
         *      This API is identical to the javascript String.slice () method/behavior
         *      @see http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-262.pdf
         *           15.5.4.13 String.prototype.slice (start, end)
         *
         *  \note \em Alias
         *      This API - when called with negative indexes - used to be called CircularSubString ().
         *
         *  @see substr
         *  @see SafeSubString
         */
        template <typename SZ>
        nonvirtual String SubString (SZ from) const;
        template <typename SZ1, typename SZ2>
        nonvirtual String SubString (SZ1 from, SZ2 to) const;

    public:
        /**
         *  Like SubString(), but no requirements on from/to. These are just adjusted to the edge of the string
         *  if the exceed those endpoints. And if arguments are <0, they are interpretted as end-relative.
         *
         *  \note \em Alias
         *      This API - when called with negative indexes - used to be called SafeCircularSubString ().
         *
         *  @see substr
         *  @see SubString
         */
        template <typename SZ>
        nonvirtual String SafeSubString (SZ from) const;
        template <typename SZ1, typename SZ2>
        nonvirtual String SafeSubString (SZ1 from, SZ2 to) const;

    public:
        /**
         *  Return 'count' copies of this String (concatenated after one another).
         */
        nonvirtual String Repeat (unsigned int count) const;

    public:
        /**
         *  Returns true if the argument character or string is found anywhere inside this string.
         *  This is equivalent to
         *      return Matches (".*" + X + ".*");    // If X had no characters which look like they are part of
         *                                            // a regular expression
         *
         *  @see Match
         */
        nonvirtual bool Contains (Character c, CompareOptions co = CompareOptions::eWithCase) const;
        nonvirtual bool Contains (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;

    public:
        /**
         *  Returns true iff the given substring is contained in this string.
         *
         *  Similar to:
         *      return Matches (X + ".*");
         *  except for the fact that with StartsWith() doesn't interpet 'X' as a regular expression
         *
         *  @see Match
         *  @see EndsWith
         */
        nonvirtual bool StartsWith (const Character& c, CompareOptions co = CompareOptions::eWithCase) const;
        nonvirtual bool StartsWith (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;

    public:
        /**
         *  Returns true iff the given substring is contained in this string.
         *
         *  Similar to:
         *      return Matches (X + ".*");
         *  except for the fact that with StartsWith() doesn't interpet 'X' as a regular expression
         *
         *  @see Match
         *  @see StartsWith
         */
        nonvirtual bool EndsWith (const Character& c, CompareOptions co = CompareOptions::eWithCase) const;
        nonvirtual bool EndsWith (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;

    public:
        /**
         *  Apply the given regular expression return true if it matches this string. This only
         *  returns true if the expression matches the ENTIRE string - all the way to the end.
         *  @see FindEach() or @see Find - to find a set of things which match.
         *
         *  \par Example Usage
         *      \code
         *          Assert (String{"abc"}.Matches ("abc"));
         *          Assert (not (String{"abc"}.Matches ("bc")));
         *          Assert (String{"abc"}.Matches (".*bc"));
         *          Assert (not String{"abc"}.Matches ("b.*c"));
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          static const RegularExpression kSonosRE_{"([0-9.:]*)( - .*)"_RegEx};
         *          static const String            kTestStr_{"192.168.244.104 - Sonos Play:5"};
         *          optional<String> match1;
         *          optional<String> match2;
         *          VerifyTestResult (kTestStr_.Matches (kSonosRE_, &match1, &match2) and match1 == "192.168.244.104" and match2 == " - Sonos Play:5");
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          // https://tools.ietf.org/html/rfc3986#appendix-B
         *          static const RegularExpression kParseURLRegExp_{"^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?"_RegEx};
         *          optional<String>               scheme;
         *          optional<String>               authority;
         *          optional<String>               path;
         *          optional<String>               query;
         *          optional<String>               fragment;
         *          if (rawURL.Matches (kParseURLRegExp_, nullptr, &scheme, nullptr, &authority, &path, nullptr, &query, nullptr, &fragment)) {
         *              DbgTrace (L"***good - scehme=%s", Characters::ToString (scheme).c_str ());
         *              DbgTrace (L"***good - authority=%s", Characters::ToString (authority).c_str ());
         *              DbgTrace (L"***good - path=%s", Characters::ToString (path).c_str ());
         *              DbgTrace (L"***good - query=%s", Characters::ToString (query).c_str ());
         *              DbgTrace (L"***good - fragment=%s", Characters::ToString (fragment).c_str ());
         *          }
         *      \endcode
         *
         *  Details on the regular expression language/format can be found at:
         *      http://en.wikipedia.org/wiki/C%2B%2B11#Regular_expressions
         *
         *  \note If any 'submatch' arguments are passed to Match, they MUST be of type optional<String>* or nullptr.
         *        Passing nullptr allows matched parameters to not be returned, but still identified positionally (by index).
         *
         *  @see Contains
         *  @see StartsWith
         *  @see EndsWith
         *  @see Find
         *  @see FindEach
         */
        nonvirtual bool Matches (const RegularExpression& regEx) const;
        nonvirtual bool Matches (const RegularExpression& regEx, Containers::Sequence<String>* matches) const;
        template <typename... OPTIONAL_STRINGS>
        nonvirtual bool Matches (const RegularExpression& regEx, OPTIONAL_STRINGS&&... subMatches) const;

    public:
        /**
         *  Find returns the index of the first occurance of the given Character/substring argument in
         *  this string. Find () always returns a valid string index, which is followed by the
         *  given substring, or nullopt otherwise.
         *
         *  Find () can optionally be provided a 'startAt' offset to begin the search at.
         *
         *  And the overload taking a RegularExpression - returns BOTH the location where the match
         *  is found, but the length of the match.
         *
         *  Note - for the special case of Find(empty-string) - the return value is 0 if this string
         *  is non-empty, and nullopt if this string was empty.
         *
         *  \note   Alias - could have been called IndexOf ()
         *
         *  \req (startAt <= size ());
         *
         *  \par Example Usage
         *      \code
         *          const String kTest_{ "a=b" };
         *          const String kLbl2LookFor_ { "a=" };
         *          if (kTest_.Find (kLbl2LookFor_)) {
         *              String  tmp { kTest_.SubString (kLbl2LookFor_.length ()) };
         *          }
         *          Assert (tmp == "b");
         *      \endcode
         *
         *  @see FindEach ()
         *  @see FindEachString ()
         *  @see Tokenize
         */
        nonvirtual optional<size_t> Find (Character c, CompareOptions co = CompareOptions::eWithCase) const;
        nonvirtual optional<size_t> Find (Character c, size_t startAt, CompareOptions co = CompareOptions::eWithCase) const;
        nonvirtual optional<size_t> Find (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;
        nonvirtual optional<size_t> Find (const String& subString, size_t startAt, CompareOptions co = CompareOptions::eWithCase) const;
        nonvirtual optional<pair<size_t, size_t>> Find (const RegularExpression& regEx, size_t startAt = 0) const;
        nonvirtual Traversal::Iterator<Character> Find (const function<bool (Character item)>& that) const;

    public:
        /**
         *  This is just like Find, but captures all the matching results in an iterable result.
         *  The reason the overload for RegularExpression's returns a list of pair<size_t,size_t> is because
         *  the endpoint of the match is ambiguous. For fixed string Find, the end of match is computable
         *  from the arguments.
         *
         *  FindEach () can be more handy to use than directly using Find () in scenarios where you want
         *  to iterate over each match:
         *      e.g.:
         *          for (auto i : s.FindEach ("xxx")) {....}
         *
         *  Also, to count matches, you can use:
         *      size_t nMatches = FindEach (matchexp).size ();
         *
         *  Note: FindEach handles the special case of an empty match as ignored, so FindEach(empty-str-or-regexp)
         *  always returns an empty list. Also - for the String case, it returns distinct matches, so if you
         *  search String{"AAAA"}.FindEach ("AA"), you will get 2 answers ({0, 2}).
         *
         *  @see Find ()
         *  @see FindEachString ()
         *  @see Matches ()
         */
        nonvirtual vector<pair<size_t, size_t>> FindEach (const RegularExpression& regEx) const;
        nonvirtual vector<size_t> FindEach (const String& string2SearchFor, CompareOptions co = CompareOptions::eWithCase) const;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          const String                        kTest_{ "a=b,"sv };
         *          const RegularExpression             kRE_{ "a=(.*)[, ]" };
         *          Sequence<String>                    tmp1{ kTest_.FindEachString (kRE_) };
         *          Assert (tmp1.size () == 1 and tmp1[0] == "a=b,");
         *          Sequence<RegularExpressionMatch>    tmp2 { kTest_.FindEachMatch (kRE_) };
         *          Assert (tmp2.size () == 1 and tmp2[0].GetFullMatch () == "a=b," and tmp2[0].GetSubMatches () == Sequence<String> {"b"});
         *      \endcode
         *
         *  @see Find ()
         *  @see FindEachString ()
         *  @see Matches ()
         */
        nonvirtual vector<RegularExpressionMatch> FindEachMatch (const RegularExpression& regEx) const;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          const String            kTest_ { "a=b, c=d"_k };
         *          const RegularExpression kRE_ { "(.)=(.)" };
         *          Assert ((kTest_.FindEachString (kRE_) ==  vector<String> {"a=b", "c=d"}));
         *      \endcode
         *
         *  @see Find ()
         *  @see FindEachMatch ()
         *  @see Matches ()
         */
        nonvirtual vector<String> FindEachString (const RegularExpression& regEx) const;

    public:
        /**
         * RFind (substring) returns the index of the last occurance of the given substring in
         * this string. This function always returns a valid string index, which is followed by the
         * given substring, or optional<size_t> {} otherwise.
         *
         *  \note   Alias - could have been called RIndexOf ()
         */
        nonvirtual optional<size_t> RFind (Character c) const noexcept;
        nonvirtual optional<size_t> RFind (const String& subString) const;

    public:
        /**
         *  Replace the range of this string with the given replacement. Const method: just creates new string as described.
         */
        nonvirtual String Replace (size_t from, size_t to, const String& replacement) const;
        nonvirtual String Replace (pair<size_t, size_t> fromTo, const String& replacement) const;

    public:
        /**
         * Apply the given regular expression, with 'with' and replace each match. This doesn't
         * modify this string, but returns the replacement string.
         *
         * CHECK - BUT HI HTINK WE DEFINE TO REPLACE ALL? OR MAKE PARAM?
         * See regex_replace () for definition of the regEx language
         *
         *   Require (not string2SearchFor.empty ());
         *
         *  \par Example Usage
         *      \code
         *          mungedData = mungedData.ReplaceAll (RegularExpression{ "\\b0+" }, "");    // strip all leading zeros
         *      \endcode
         *
         *  Note - it IS legal to have with contain the original search for string, or even
         *  to have it 'created' as part of where it gets
         *  inserted. The implementation will only replace those that pre-existed.
         *
         *  \note To perform a regular expression replace-all, which is case insensitive, create the regular expression with CompareOptions::eCaseInsensitive
         *
         *  \note ReplaceAll could have been called 'SafeString' or 'FilteredString' (was at one point - replaces that functionality)
         */
        nonvirtual String ReplaceAll (const RegularExpression& regEx, const String& with) const;
        nonvirtual String ReplaceAll (const String& string2SearchFor, const String& with, CompareOptions co = CompareOptions::eWithCase) const;
        nonvirtual String ReplaceAll (const function<bool (Character)>& replaceCharP, const String& with) const;
        nonvirtual String ReplaceAll (const Containers::Set<Character>& charSet, const String& with) const;

    public:
        /**
         *  Replace any CR or LF or CRLF seqeunces with plan NL-terminated text.
         */
        nonvirtual String NoramlizeTextToNL () const;

    public:
        /**
         *  Break this String into constituent parts. This is a simplistic API but at least handy as is.
         *
         *  The caller can specify the token seperators by set, by lambda. This defaults to the lambda "isWhitespace".
         *
         *  This is often called 'Split' in other APIs. This is NOT (as is now) a replacement for flex, but just for
         *  simple, but common string splitting needs (though if I had a regexp param, it may approach the power of flex).
         *
         *  \note If this->length () == 0, this method returns a list of length 0;
         *  \note Its fine for the split character/characters to be missing, in which case this
         *        returns a list of length 1
         *
         *  \par Example Usage
         *      \code
         *          String  t { "ABC DEF G" };
         *          Assert (t.Tokenize ().length () == 3);
         *          Assert (t.Tokenize ()[1] == "DEF");
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          String  t { "foo=   7" };
         *          auto    tt = t.Tokenize ({ '=' });
         *          Assert (t.length () == 2);
         *          Assert (t[1] == "7");
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          String  t { "foo=   7" };
         *          auto    tt = t.Tokenize ({ '=' });
         *          Assert (t.length () == 2);
         *          Assert (t[1] == "7");
         *      \endcode
         *
         *  @see Find
         *
         *  TODO:
         *      @todo   Consider adding overload that uses RegularExpression to define tokens.
         *
         *      @todo   Consider adding overload where trim is replaced with lambda saying 'isTrimmedCharacter')
         *
         *      @todo   Review:
         *                  http://qt-project.org/doc/qt-5.0/qtcore/qstring.html#split
         *              especially:
         *                  QString line = "forename\tmiddlename  surname \t \t phone";
         *                  QRegularExpression sep("\\s+");
         *                  str = line.section(sep, 2, 2); // str == "surname"
         *                  str = line.section(sep, -3, -2); // str == "middlename  surname"
         *              Make sure our Find/Tokenize is at least this simple, and maybe diff between find and split
         *              is FIND the regular expression names the things looked for and SPLIT() uses regexp to name the separators?
         *              Add something like the above to the String String demo app (when it exists)
         */
        nonvirtual Containers::Sequence<String> Tokenize (
            const function<bool (Character)>& isTokenSeperator = [] (Character c) -> bool { return c.IsWhitespace (); }, bool trim = true) const;
        nonvirtual Containers::Sequence<String> Tokenize (const Containers::Set<Character>& delimiters, bool trim = true) const;

    public:
        /**
         * String LTrim () scans the characters form the left to right, and applies the given
         * 'shouldBeTrimmed' function (defaults to IsWhitespace). All such characters are removed,
         * and the resulting string is returned. This does not modify the current string its
         * applied to - just returns the trimmed string.
         */
        nonvirtual String LTrim (bool (*shouldBeTrimmmed) (Character) = [] (Character c) -> bool { return c.IsWhitespace (); }) const;

    public:
        /**
         * String RTrim () scans the characters form the right to left, and applies the given
         * 'shouldBeTrimmed' function (defaults to IsWhitespace). All such characters are removed,
         * and the resulting string is returned. This does not modify the current string its
         * applied to - just returns the trimmed string.
         *
         *  \par Example Usage
         *      \code
         *          String name = origName.RTrim ([] (Character c) { return c == '\\';});        // Trim a trailing backslash(s), if present
         *      \endcode
         */
        nonvirtual String RTrim (bool (*shouldBeTrimmmed) (Character) = [] (Character c) -> bool { return c.IsWhitespace (); }) const;

    public:
        /**
         * String Trim () is locally equivalent to RTrim (shouldBeTrimmed).LTrim (shouldBeTrimmed).
         */
        nonvirtual String Trim (bool (*shouldBeTrimmmed) (Character) = [] (Character c) -> bool { return c.IsWhitespace (); }) const;

    public:
        /**
         * Walk the entire string, and produce a new string consisting of all characters for which
         * the predicate 'removeCharIf' returned false.
         */
        nonvirtual String StripAll (bool (*removeCharIf) (Character)) const;

    public:
        /**
         *  Combine the given array into a single string (typically comma space) separated.
         *  If given a list of length n, this adds n-1 seperators.
         * 
         *  \note .Net version - https://docs.microsoft.com/en-us/dotnet/api/system.string.join?redirectedfrom=MSDN&view=net-6.0#System_String_Join_System_String_System_String___
         *  \note Java version - https://docs.oracle.com/javase/8/docs/api/java/lang/String.html#join-java.lang.CharSequence-java.lang.CharSequence...-
         *  \note Javascript   - https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/join
         */
        static String Join (const Iterable<String>& list, const String& separator = ", "sv);

    public:
        /**
         * Return a new string based on this string where each lower case characer is replaced by its
         * upper case equivalent. Note that non-lower-case characters (such as punctuation) un unchanged.
         */
        nonvirtual String ToLowerCase () const;

    public:
        /**
         * Return a new string based on this string where each lower case characer is replaced by its
         * upper case equivalent. Note that non-upper-case characters (such as punctuation) un unchanged.
         */
        nonvirtual String ToUpperCase () const;

    public:
        /**
         * Return true if the string contains zero non-whitespace characters.
         */
        nonvirtual bool IsWhitespace () const;

    public:
        /**
         *  \brief return the first maxLen (or fewer if string shorter) characters of this string (adding ellipsis if truncated)
         *
         *  This function is for GUI/display purposes. It returns the given string, trimmed down
         *  to at most maxLen characters, and removes whitespace (on 'to trim' side - given by keepLeft flag -
         *  if needed to get under maxLen).
         *
         *  Note in the 3-arg overload, the elipsis string MAY be the empty string.
         */
        nonvirtual String LimitLength (size_t maxLen, bool keepLeft = true) const;
        nonvirtual String LimitLength (size_t maxLen, bool keepLeft, const String& ellipsis) const;

    public:
        /**
         *  CopyTo () copies the contents of this string to the target buffer.
         *  CopyTo () does NOT nul-terminate the target buffer
         * 
         *  Returns span of CHAR_T objects written - a subspan of the argument span
         * 
         *  \req s.size () >= UTFConvert::ComputeTargetBufferSize<CHAR_T> (...this-string-data...);
         * 
         *  \see See also GetData<CHAR_T> (buf) - similar functionality - except caller doesn't need to know size of buffer to allocate
         */
        template <IUNICODECanAlwaysConvertTo CHAR_T>
        nonvirtual span<CHAR_T> CopyTo (span<CHAR_T> s) const
            requires (not is_const_v<CHAR_T>);

    public:
        /**
         * Convert String losslessly into a standard C++ type.
         *
         *  Only specifically specialized variants supported: IBasicUNICODEStdString<T> or is_same_v<T,String>
         *      o   wstring
         *      o   u8string
         *      o   u16string
         *      o   u32string
         *      o   String    (return *this; handy sometimes in templated usage; harmless)
         *
         *  DEPRECATED AS OF v3.0d1 because As is const method - could do non-const As<> overload for these, but that would be confusing
         *      o   const wchar_t*
         *      o   const Character*
         *
         *  \note
         *      o   As<u8string> () equivilent to AsUTF8 () call
         *      o   As<u16string> () equivilent to AsUTF16 () call
         *      o   As<u32string> () equivilent to AsUTF32 () call
         *
         *  \note   We tried to also have template<typename T> explicit operator T () const; - conversion operator - but
         *          We got too frequent confusion in complex combinations of templates, like with:
         *          Set<String> x ( *optional<String> {String{}) );       // fails cuz calls operator Set<String> ()!
         *          Set<String> x { *optional<String> {String{}) };       // works as expected
         */
        template <typename T>
        nonvirtual T As () const
            requires (IBasicUNICODEStdString<T> or is_same_v<T, String>);
        template <typename T>
        [[deprecated ("Since Stroika v3.0d2, just use 0 arg version)")]] void As (T* into) const
            requires (IBasicUNICODEStdString<T> or is_same_v<T, String>)
        {
            *into = this->As<T> ();
        }

    public:
        /**
         *  Create a narrow string object from this, based on the encoding from the argument locale.
         *  This throws an exception if there is an error performing the conversion, and the 'into' overload
         *  leaves 'into' in an undefined (but safe) state.
         */
        nonvirtual string AsNarrowString (const locale& l) const;
        nonvirtual string AsNarrowString (const locale& l, AllowMissingCharacterErrorsFlag) const;
        [[deprecated ("Since Stroika v3.0d2, just use 1 arg version)")]] void AsNarrowString (const locale& l, string* into) const
        {
            *into = this->AsNarrowString (l);
        }

    public:
        /**
         * Convert String losslessly into a standard C++ type.
         * Only specifically specialized variants are supported.
         *
         *  SUPPORTED result type "T": values are:
         *      string
         *      u8string
         */
        template <typename T = u8string>
        nonvirtual T AsUTF8 () const
            requires (is_same_v<T, string> or is_same_v<T, u8string>);
        template <typename T = u8string>
        [[deprecated ("Since Stroika v3.0d2 - use AsUTF8/0")]] void AsUTF8 (T* into) const
            requires (is_same_v<T, string> or is_same_v<T, u8string>)
        {
            *into = this->AsUTF8 ();
        }

    public:
        /**
         * Convert String losslessly into a standard C++ type u16string.
         *
         *  \par Example Usage:
         *      \code
         *          String s = u"hi mom";
         *          u16string su    =   AsUTF16 ();
         *      \endcode
         * 
         *  \note - the resulting string may have a different length than this->size() due to surrogates
         * 
         *  @todo allow wchar_t if sizeof(wchar_t) == 2
         */
        template <typename T = u16string>
        nonvirtual T AsUTF16 () const
            requires (is_same_v<T, u16string> or (sizeof (wchar_t) == sizeof (char16_t) and is_same_v<T, wstring>));
        template <typename T = u16string>
        [[deprecated ("Since Stroika v3.0d2 - use AsUTF16/0")]] void AsUTF16 (T* into) const
            requires (is_same_v<T, u16string> or (sizeof (wchar_t) == sizeof (char16_t) and is_same_v<T, wstring>))
        {
            *into = AsUTF16 ();
        }

    public:
        /**
         * Convert String losslessly into a standard C++ type u32string.
         * 
         *  \par Example Usage:
         *      \code
         *          String s = u"hi mom";
         *          u32string su    =   AsUTF32 ();
         *      \endcode
         *
         *  \note - As of Stroika 2.1d23 - the resulting string may have a different length than this->size() due to surrogates,
         *          but eventually the intent is to fix Stroika's string class so this is not true, and it returns the length of the string
         *          in size () with surrogates removed (in other words uses ucs32 represenation). But not there yet.
         */
        template <typename T = u32string>
        nonvirtual T AsUTF32 () const
            requires (is_same_v<T, u32string> or (sizeof (wchar_t) == sizeof (char32_t) and is_same_v<T, wstring>));
        template <typename T = u32string>
        [[deprecated ("Since Stroika v3.0d2 - use AsUTF32/0")]] void AsUTF32 (T* into) const
            requires (is_same_v<T, u32string> or (sizeof (wchar_t) == sizeof (char32_t) and is_same_v<T, wstring>))
        {
            *into = AsUTF32 ();
        }

    public:
        /**
         *  See docs on SDKChar for meaning (charactet set).
         * 
         *  Note - many UNICODE Strings cannot be represented in the SDKString character set (especially if narrow - depends alot).
         *  But in that case, AsNarrowSDKString () will throw, unless AllowMissingCharacterErrorsFlag is specified.
         */
        nonvirtual SDKString                                       AsSDKString () const;
        nonvirtual SDKString                                       AsSDKString (AllowMissingCharacterErrorsFlag) const;
        [[deprecated ("Since Stroika v3.0d2 - just use /0")]] void AsSDKString (SDKString* into) const
        {
            *into = AsSDKString ();
        }

    public:
        /**
         *  See docs on SDKChar for meaning (charactet set). If SDKChar is a wide character, there is probably still a
         *  default 'code page' to interpret narrow characters (Windows CP_ACP). This is a string in that characterset.
         * 
         *  Note - many UNICODE Strings cannot be represented in the SDKString character set (especially if narrow - depends alot).
         *  But in that case, AsNarrowSDKString () will throw, unless AllowMissingCharacterErrorsFlag is specified.
         */
        nonvirtual string                                          AsNarrowSDKString () const;
        nonvirtual string                                          AsNarrowSDKString (AllowMissingCharacterErrorsFlag) const;
        [[deprecated ("Since Stroika v3.0d2 - just use /0")]] void AsNarrowSDKString (string* into) const
        {
            *into = SDK2Narrow (AsSDKString ());
        }

    public:
        /**
         * Convert String losslessly into a standard C++ type.
         * Only specifically specialized variants are supported (right now just <string> supported).
         * The source string MUST be valid ascii characters - throw RuntimeErrorException<>
         *
         *  \par Example Usage:
         *      \code
         *          string a1    =  String{"hi mom"}.AsASCII ();    // OK
         *          string a2    =  String{u"שלום"}.AsASCII ();      // throws
         *      \endcode
         * 
         *  \note - this is a (compatible) change of behavior: before Stroika v2.1d23, this would assert out on invalid ASCII.
         * 
         *  Supported Types:
         *      o   Memory::StackBuffer<char>
         *      o   string
         *      o   u8string        (note any ASCII string is also legit utf-8)
         */
        template <typename T = string>
        nonvirtual T AsASCII () const
            requires requires (T* into) {
                {
                    into->empty ()
                } -> same_as<bool>;
                {
                    into->push_back (ASCII{0})
                };
            };
        ;
        template <typename T = string>
        [[deprecated ("Since v3.0d2 use /0")]] void AsASCII (T* into) const
            requires (is_same_v<T, string> or is_same_v<T, Memory::StackBuffer<char>>)
        {
            if (not AsASCIIQuietly (into)) {
                ThrowInvalidAsciiException_ ();
            }
        }

    public:
        /**
         * Convert String losslessly into a standard C++ type.
         * Only specifically specialized variants are supported (right now just <string> supported).
         * If this source contains any invalid ASCII characters, this returns nullopt, and else a valid engaged string.
         * 
         *  Supported Types(T):
         *      o   Memory::StackBuffer<char>
         *      o   string
         *      o   u8string (note any valid ASCII string is also valid utf-8)
         */
        template <typename T = string>
        nonvirtual optional<T> AsASCIIQuietly () const
            requires requires (T* into) {
                {
                    into->empty ()
                } -> same_as<bool>;
                {
                    into->push_back (ASCII{0})
                };
            };
        ;
        template <typename T = string>
        [[deprecated ("Since v3.0d2 use /0 overload")]] bool AsASCIIQuietly (T* into) const
            requires (is_same_v<T, string> or is_same_v<T, Memory::StackBuffer<char>>)
        {
            auto r = this->AsASCIIQuietly ();
            if (r) {
                *into = *r;
                return true;
            }
            else {
                return false;
            }
        }

    public:
        /**
         *  \brief Summary data for raw contents of rep - each rep will support at least one of these span forms
         *
         *  Each rep will support a span of at least one code-point type (ascii, utf8, utf16, or utf32)
         *
         *  This API is guaranteed to support a span of at least one of these types (maybe more). The caller may
         *  specify the code-point type preferred.
         * 
         *  \note eAscii is a subset of eSingleByteLatin1, so when the type eAscii is returned, EITHER fSingleByteLatin1 or fAscii maybe
         *        maybe used.
         * 
         *  This API is public, but best to avoid depending on internals of String API - like PeekSpanData - since
         *  this reasonably likely to change in future versions.
         */
        struct PeekSpanData {
            enum StorageCodePointType {
                /**
                 *  ASCII is useful to track in storage (though same size as eSingleByteLatin1) - because requests
                 *  to convert to UTF-8 are free - ASCII is legit UTF8 (not true for eSingleByteLatin1)
                 */
                eAscii,
                /**
                 *  Latin1 - 8 bit representation of characters. But 256 of them - more than plain ascii.
                 *  And cheap/easy to convert to UNICODE (since code points of wider characters exactly the same values).
                 */
                eSingleByteLatin1,
                eChar16,
                eChar32
            };
            StorageCodePointType fInCP;
            union {
                span<const ASCII>    fAscii;
                span<const Latin1>   fSingleByteLatin1;
                span<const char16_t> fChar16;
                span<const char32_t> fChar32;
            };
        };

    public:
        /**
         *  \brief return the constant character data inside the string in the form of a case variant union of different span types (at least one will be there)
         *         templated type arg just used to pick a preferred type.
         * 
         *  \note CHAR_TYPE == char implies eAscii
         * 
         *  \note Reason for the two step API - getting the PeekSpanData, and then using - is because getting
         *        the data is most expensive part (virtual function), and the packaged PeekSpanData gives enuf
         *        info to do the next steps (quickly inline usually)
         * 
         *  This API is public, but best to avoid depending on internals of String API - like PeekSpanData - since
         *  this reasonably likely to change in future versions.
         */
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_TYPE = ASCII>
        nonvirtual PeekSpanData GetPeekSpanData () const;

    public:
        /**
         *  \brief return the constant character data inside the string in the form of a span or nullopt if not available for that CHAR_TYPE
         * 
         *  This API is public, but best to avoid depending on internals of String API - like PeekSpanData - since
         *  this reasonably likely to change in future versions.
         */
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_TYPE>
        static optional<span<const CHAR_TYPE>> PeekData (const PeekSpanData& pds);
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_TYPE>
        nonvirtual optional<span<const CHAR_TYPE>> PeekData () const;

    public:
        /**
         *  \brief return the constant character data inside the string (rep) in the form of a span, possibly quickly and direclty, and possibly copied into possiblyUsedBuffer
         * 
         *  This API will typically return a span of data which is internal pointers into the data of the rep (and so its invalidated on the
         *  next change to the string).
         * 
         *  BUT - it maybe a span of data stored into the argument possiblyUsedBuffer (which is why it must be provided - cannot be nullptr).
         *  If you want the freedom to not pass in this buffer, see the PeekData API.
         * 
         *  \note - CHAR_T must satisfy the concept IUNICODECanAlwaysConvertTo - SAFELY - because the string MIGHT contain characters not in any 
         *          unsafe char class (like ASCII or Latin1), and so there might not be a way to do the conversion. Use 
         *          PeekData () to do that - where it can return nullopt if no conversion possible.
         * 
         *  \par Example Usage
         *      \code
         *          Memory::StackBuffer<char8_t> maybeIgnoreBuf1;
         *          span<const char8_t>          thisData = GetData (&maybeIgnoreBuf1);
         *      \endcode
         * 
         *  \note Prior to Stroika v3.0d1, GetData() took no arguments, and returned pair<const CHAR_TYPE*, const CHAR_TYPE*>
         *        which is pretty similar, but not quite the same. To adapt any existing code calling that older version of the API
         *        just add a Memory::StackBuffer<T> b; and pass &b to GetData(); And the return span is not the same as pair<> but
         *        easily convertible.
         */
        template <IUNICODECanAlwaysConvertTo CHAR_TYPE, size_t STACK_BUFFER_SZ>
        static span<const CHAR_TYPE> GetData (const PeekSpanData& pds, Memory::StackBuffer<CHAR_TYPE, STACK_BUFFER_SZ>* possiblyUsedBuffer);
        template <IUNICODECanAlwaysConvertTo CHAR_TYPE, size_t STACK_BUFFER_SZ>
        nonvirtual span<const CHAR_TYPE> GetData (Memory::StackBuffer<CHAR_TYPE, STACK_BUFFER_SZ>* possiblyUsedBuffer) const;

    public:
        struct EqualsComparer;

    public:
        struct LessComparer;

    public:
        struct ThreeWayComparer;

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const String& rhs) const;
        nonvirtual strong_ordering operator<=> (const wchar_t* rhs) const;
        nonvirtual strong_ordering operator<=> (const wstring& rhs) const;
        nonvirtual strong_ordering operator<=> (const wstring_view& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const String& rhs) const;
        nonvirtual bool operator== (const wchar_t* rhs) const;
        nonvirtual bool operator== (const wstring& rhs) const;
        nonvirtual bool operator== (const wstring_view& rhs) const;

    public:
        /**
         *  Alias for basic_string>char>::npos - except this is constexpr.
         *
         *  This is only used for 'STL-compatabiliity' apis, like substr (), find, rfind (), etc.
         */
        static constexpr size_t npos = static_cast<size_t> (-1);

    public:
        /**
         *  basic_string alias: length = size
         */
        nonvirtual size_t length () const noexcept;

    public:
        /**
         *  \note BREAKING change between Stroika 2.1 and v3 - const c_str/0 no longer guaraneed to return non-null
         * 
         *        Mitigating this, the non-const c_str() still will return non-null, and the const overload taking
         *        StackBuffer<wchar_t> will also guarantee returning non-null.
         * 
         *        In the case of the overloads taking no arguments, the lifetime of the returned pointer is until the
         *        next change to this string.  In the case of the StackBuffer overload, the guarantee extends for the lifetime
         *        of the argument buffer (typically just the next few lines of code).
         * 
         *  This will always return a value which is NUL-terminated.
         * 
         *  Note also - the c_str () function CAN now be somewhat EXPENSIVE, causing a mutation of the String object, so use
         *  one of the const overloads where possible (or where performance matters).
         * 
         *  \note Why does c_str (StackBuffer) return a tuple?
         *        Sometimes you just want a plain const wchar_t* you can use with an old C pointer based API. But that
         *        fails/asserts out if you happen to have an empty string and try to get the pointer. Sometimes - you just need
         *        the pointer!
         * 
         *        And why the string-view part? Because sometimes you want the LENGTH. Sure - you can just compute it again. But
         *        that is costly. Sure you can just use the original string length. BUT THAT WOULD BE A BUG once I support
         *        surrogates properly (at least on windows where wchar_t isn't char32_t).
         */
        nonvirtual const wchar_t* c_str ();
        nonvirtual tuple<const wchar_t*, wstring_view> c_str (Memory::StackBuffer<wchar_t>* possibleBackingStore) const;

    public:
        /**
         *  Follow the basic_string<>::find () API
         * 
         *  need more overloads.
         *
         *  Returns String::npos if not found, else the zero based index.
         */
        nonvirtual size_t find (Character c, size_t startAt = 0) const;
        nonvirtual size_t find (const String& s, size_t startAt = 0) const;

    public:
        /**
         *  Follow the basic_string<>::rfind () API
         * 
         *   need more overloads.
         *
         *   Returns String::npos if not found, else the zero based index.
         */
        nonvirtual size_t rfind (Character c) const;

    public:
        /**
         *  mimic (much of - need more overloads) STL variant
         */
        nonvirtual void erase (size_t from = 0);
        nonvirtual void erase (size_t from, size_t count);

    public:
        /**
         */
        nonvirtual void push_back (wchar_t c);
        nonvirtual void push_back (Character c);

    public:
        /**
         *  mimic https://en.cppreference.com/w/cpp/string/basic_string/front
         * 
         *  \req not empty ()
         */
        nonvirtual Character front () const;

    public:
        /**
         *  mimic https://en.cppreference.com/w/cpp/string/basic_string/back
         * 
         *  \req not empty ()
         */
        nonvirtual Character back () const;

    public:
        /**
         *  Compatable with STL::basic_string::subtr() - which interprets second argument as count. Not the same
         *  as Stroika::String::SubString (where the second argument is a 'to')
         *
         *  @see SubString
         *
         *  From http://en.cppreference.com/w/cpp/string/basic_string/substr
         *      Returns a substring [pos, pos+count). If the requested substring extends
         *      past the end of the string, or if count == npos, the returned substring is [pos, size()).
         *      std::out_of_range if pos > size()
         */
        nonvirtual String substr (size_t from, size_t count = npos) const;

    public:
        template <typename CHAR_T>
        [[deprecated ("Since Stroika v3.0d1, String{}")]] static String FromASCII (span<const CHAR_T> s)
        {
            return String{s};
        }
        template <typename CHAR_T>
        [[deprecated ("Since Stroika v3.0d1, String{}")]] static String FromASCII (const CHAR_T* cString)
        {
            return String{cString};
        }
        template <IStdBasicStringCompatibleCharacter CHAR_T>
        [[deprecated ("Since Stroika v3.0d1, String{}")]] static String FromASCII (const basic_string<CHAR_T>& str)
        {
            return String{str};
        }
        [[deprecated ("Since Stroika v3.0d1, use span{} overload for this")]] static String FromASCII (const char* from, const char* to)
        {
            return String{span{from, to}};
        }
        [[deprecated ("Since Stroika v3.0d1, use span{} overload for this")]] static String FromASCII (const wchar_t* from, const wchar_t* to)
        {
            return String{span{from, to}};
        }
        [[deprecated ("Since Stroika v3.0d1, use span overloads")]] String InsertAt (const wchar_t* from, const wchar_t* to, size_t at) const
        {
            Memory::StackBuffer<Character> buf{Memory::eUninitialized, UTFConvert::ComputeTargetBufferSize<Character> (span{from, to})};
            return InsertAt (UTFConvert::kThe.ConvertSpan (span{from, to}, span{buf}), at);
        }
        [[deprecated ("Since Stroika v3.0d1, use span overloads")]] String InsertAt (const Character* from, const Character* to, size_t at) const
        {
            return InsertAt (span{from, to}, at);
        }
        [[deprecated ("Since Stroika v3.0d1, use span{} overload for this")]] static String FromLatin1 (const char* start, const char* end)
        {
            return FromLatin1 (span{start, end});
        }
        [[deprecated ("Since Stroika v3.0d1, use span{} constructor for this")]] static String FromNarrowString (const char* from,
                                                                                                                 const char* to, const locale& l)
        {
            return FromNarrowString (span{from, to}, l);
        }
        [[deprecated ("Since Stroika v3.0d1, use span{} constructor for this")]] static String FromNarrowSDKString (const char* from, const char* to)
        {
            return FromNarrowSDKString (span{from, to});
        }
        template <IUNICODECanAlwaysConvertTo CHAR_T>
        [[deprecated ("Since Stroika v3.0d1, use span{} constructor for this")]] String (const CHAR_T* from, const CHAR_T* to)
            : String{span<const CHAR_T>{from, to}}
        {
        }
        [[deprecated ("Since Stroika v3.0d1 - use As<wstring> ().c_str () or other c_str() overload (*UNSAFE TO USE*)")]] nonvirtual const wchar_t*
        c_str () const noexcept;
        [[deprecated ("Since Stroika v3.0 - use span{} overloads")]] inline static String FromSDKString (const SDKChar* from, const SDKChar* to)
        {
            return FromSDKString (span{from, to});
        }
        [[deprecated ("Since Stroika v3.0 - use span{} overloads")]] static String FromUTF8 (const char* from, const char* to)
        {
            return FromUTF8 (span{from, to});
        }
        [[deprecated ("Since Stroika v3.0 - use span{} overloads")]] static String FromUTF8 (const char8_t* from, const char8_t* to)
        {
            return FromUTF8 (span{from, to});
        }
        template <typename T = string>
        [[deprecated ("Since Stroika v3.0d1 - use Character::AsAsciiQuietly")]] static bool AsASCIIQuietly (const wchar_t* fromStart,
                                                                                                            const wchar_t* fromEnd, T* into)
        {
            return Character::AsASCIIQuietly (span<const wchar_t>{fromStart, fromEnd}, into);
        }
        [[deprecated ("Since Stroika v3.0d1 due to https://stroika.atlassian.net/browse/STK-965 - NOT IMPLEMENTED")]] nonvirtual const wchar_t* data () const;

    private:
        static shared_ptr<_IRep> mkEmpty_ ();

    private:
        /**
         *  If the argument CHAR_T is restrictive (such as ASCII/char) - this CHECKS and THROWS (Character::CheckASCII).
         *  This function also reads the data, and sees if it can downshift 'CHAR_T' to something more restrictive, and produces
         *  a possibly smaller rep.
         * 
         *  For some overloads (e..g && move) - the data is 'stolen/moved'.
         * 
         *  See mk_nocheck_ for a simpler - DO WHAT I SAID - operation.
         */
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static shared_ptr<_IRep> mk_ (span<const CHAR_T> s);
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static shared_ptr<_IRep> mk_ (Iterable<CHAR_T> it);
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static shared_ptr<_IRep> mk_ (span<CHAR_T> s);
        template <IStdBasicStringCompatibleCharacter CHAR_T>
        static shared_ptr<_IRep> mk_ (basic_string<CHAR_T>&& s);

    private:
        /*
         *  Note the mk_nocheck_ - just does the mk of the buffer, but assuming the arguments are legit and will fit (though it may
         *  assert in DEBUG builds this is true).
         * 
         *  This just blindly allocates the buffer of the given size/type for the given arguments.
         */
        template <typename CHAR_T>
        static shared_ptr<_IRep> mk_nocheck_ (span<const CHAR_T> s)
            requires (is_same_v<CHAR_T, ASCII> or is_same_v<CHAR_T, Latin1> or is_same_v<CHAR_T, char16_t> or is_same_v<CHAR_T, char32_t>);

    private:
        template <unsigned_integral T>
        nonvirtual size_t SubString_adjust_ (T fromOrTo, size_t myLength) const;
        template <signed_integral T>
        nonvirtual size_t SubString_adjust_ (T fromOrTo, size_t myLength) const;

    private:
        nonvirtual String SubString_ (const _SafeReadRepAccessor& thisAccessor, size_t from, size_t to) const;

    protected:
        nonvirtual void _AssertRepValidType () const;

    private:
        [[noreturn]] static void ThrowInvalidAsciiException_ (); // avoid include

    public:
        friend wostream& operator<< (wostream& out, const String& s);
    };

    /**
     * Protected helper Rep class.
     * 
     *  \note   Important design note - String reps are IMMUTABLE. Changes to string like +=, create new string reps (so costly).
     *          Use StringBuilder for that purpose in performance sensative code.
     */
    class String::_IRep : public Iterable<Character>::_IRep {
    public:
        /**
         *  Return the ith character in the string.
         */
        virtual Character GetAt (size_t index) const noexcept = 0;

    public:
        /**
         *  Each rep will support a span of at least one code-point type (ascii, utf8, utf16, or utf32)
         *
         *  This API is guaranteed to support a span of at least one of these types (maybe more). The caller may
         *  specify the code-point type preferred.
         */
        virtual PeekSpanData PeekData ([[maybe_unused]] optional<PeekSpanData::StorageCodePointType> preferred) const noexcept = 0;

    public:
        /*
         *  Return a pointer to mostly standard (wide, nul-terminated) C string,
         *  whose lifetime extends to the next non-const call on this rep, or nullptr.
         *
         *  It is only 'mostly' standard because it is allowed to have nul-chars embedded in it. But it will
         *  always have str[len] == 0;
         *
         *  \note Since Stroika v3.0d1, this can return nullptr (in which case the String library will allocate a new backend)
         * 
         *  \ensure returnResult == nullptr or returnResult[len] == '\0';
         */
        virtual const wchar_t* c_str_peek () const noexcept = 0;

    private:
        friend class String;
    };

    /**
     *  The concept IConvertibleToString is satisfied iff the argument type can be used to construct a (Stroika) String.
     *  Note subtly, const char* is treated (as of Stroika v3) as convertible to String, but the characters must be ASCII, or
     *  an exception will be generated in the constructor.
     */
    template <typename T>
    concept IConvertibleToString = convertible_to<T, String>;

    // Some some docs/testing...
    static_assert (not IConvertibleToString<int>);
    static_assert (not IConvertibleToString<char>);     // would have been sensible to allow, but easily generates confusing results: cuz that means String x = 3 would work; confusing with ovarloads)
    static_assert (IConvertibleToString<string>);
    static_assert (IConvertibleToString<wstring>);
    static_assert (IConvertibleToString<u8string>);
    static_assert (IConvertibleToString<u16string>);
    static_assert (IConvertibleToString<u32string>);

    /**
     *  Use Stroika String more easily with std::ostream.
     *
     *  \note   EXPERIMENTAL API (added /as of 2014-02-15 - Stroika 2.0a21)
     *
     *  \note   Note sure how well this works being in a namespace!
     *
     *  \note   Intentionally don't do operator>> because not so well defined for strings (could do as wtith STL I guess?)
     *
     *  \note   tried to use templates to avoid the need to create a dependency of this module on iostream,
     *          but that failed (maybe doable but overloading was trickier).
     */
    wostream& operator<< (wostream& out, const String& s);

    namespace Private_ {
        // This is just anything that can be treated as a 'span<const Character>'
        // clang-format off
        template <typename T>
        concept ICanBeTreatedAsSpanOfCharacter_ =
            derived_from<remove_cvref_t<T>, String>
            or is_same_v<remove_cvref_t<T>, u8string> 
            or is_same_v<remove_cvref_t<T>, u8string_view> 
            or is_same_v<remove_cvref_t<T>, u16string> 
            or is_same_v<remove_cvref_t<T>, u16string_view> 
            or is_same_v<remove_cvref_t<T>, u32string> 
            or is_same_v<remove_cvref_t<T>, u32string_view> 
            or is_same_v<remove_cvref_t<T>, wstring> 
            or is_same_v<remove_cvref_t<T>, wstring_view> 
            or is_same_v<remove_cvref_t<T>, const Character*> 
            or is_same_v<remove_cvref_t<T>, const char8_t*>
            or is_same_v<remove_cvref_t<T>, const char16_t*>
            or is_same_v<remove_cvref_t<T>, const char32_t*>
            or is_same_v<remove_cvref_t<T>, const wchar_t*> 
            ;
        // clang-format on

        template <ICanBeTreatedAsSpanOfCharacter_ USTRING, size_t STACK_BUFFER_SZ>
        span<const Character> AsSpanOfCharacters_ (USTRING&& s, Memory::StackBuffer<Character, STACK_BUFFER_SZ>* mostlyIgnoredBuf);
    }

    /**
     */
    struct String::EqualsComparer : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
        /**
         *  optional CompareOptions to CTOR allows for case insensative compares
         */
        constexpr EqualsComparer (CompareOptions co = CompareOptions::eWithCase);

        /**
         * Extra overloads a slight performance improvement
         */
        template <IConvertibleToString LT, IConvertibleToString RT>
        nonvirtual bool operator() (LT&& lhs, RT&& rhs) const;

        CompareOptions fCompareOptions;

    private:
        template <Private_::ICanBeTreatedAsSpanOfCharacter_ LT, Private_::ICanBeTreatedAsSpanOfCharacter_ RT>
        bool Cmp_ (LT&& lhs, RT&& rhs) const;
        template <Private_::ICanBeTreatedAsSpanOfCharacter_ LT, Private_::ICanBeTreatedAsSpanOfCharacter_ RT>
        bool Cmp_Generic_ (LT&& lhs, RT&& rhs) const;
    };

    /**
     */
    struct String::ThreeWayComparer : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eThreeWayCompare> {
        /**
         *  optional CompareOptions to CTOR allows for case insensative compares
         */
        constexpr ThreeWayComparer (CompareOptions co = CompareOptions::eWithCase);

        /**
         * Extra overloads a slight performance improvement
         */
        template <IConvertibleToString LT, IConvertibleToString RT>
        nonvirtual strong_ordering operator() (LT&& lhs, RT&& rhs) const;

        CompareOptions fCompareOptions;

    private:
        template <Private_::ICanBeTreatedAsSpanOfCharacter_ LT, Private_::ICanBeTreatedAsSpanOfCharacter_ RT>
        strong_ordering Cmp_ (LT&& lhs, RT&& rhs) const;
        template <Private_::ICanBeTreatedAsSpanOfCharacter_ LT, Private_::ICanBeTreatedAsSpanOfCharacter_ RT>
        strong_ordering Cmp_Generic_ (LT&& lhs, RT&& rhs) const;
    };

    /**
     *  \brief very similar to ThreeWayComparer but returns true if less
     */
    struct String::LessComparer : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eStrictInOrder> {
        constexpr LessComparer (CompareOptions co = CompareOptions::eWithCase);

        template <typename T1, typename T2>
        nonvirtual bool operator() (T1 lhs, T2 rhs) const;

    private:
        ThreeWayComparer fComparer_;
    };
    static_assert (Common::IInOrderComparer<String::LessComparer, String>);

    /**
     *  \brief shorthand for String::FromStringConstant { ARGUMENT }
     *
     *  \par Example:
     *      \code
     *          String s1 = "some-string"_k;
     *          String s2 = String::FromStringConstant ("some-string");
     *          String s3 = "some-string"sv;           // in most cases this will also work fine, and is preferable (since sv is part of C++ standard)
     *      \endcode
     *
     *  \note _k is STILL sometimes useful and better than sv, since the TYPE returned by _k is a String_Constant which IS a String
     *        so it will work in some overload contexts where sv would fail.
     * 
     *  \note operator"" _k with char*, requires that the argument string MUST BE ASCII (someday maybe lifted to allow Latin1)
     */
    String operator"" _k (const char* s, size_t len);
    String operator"" _k (const wchar_t* s, size_t len);

    /**
     *  Basic operator overload with the obvious meaning, and simply indirect to @String::Concatenate (const String& rhs)
     *
     *  \note Design Note
     *      Don't use member function so "x" + String{u"x"} works.
     *      Insist that EITHER LHS or RHS is a string (else operator applies too widely).
     * 
     *  Both arguments must be convertible to a String, and at least must be String or derived from String
     */
    template <IConvertibleToString LHS_T, IConvertibleToString RHS_T>
    String operator+ (LHS_T&& lhs, RHS_T&& rhs)
        requires (derived_from<remove_cvref_t<LHS_T>, String> or derived_from<remove_cvref_t<RHS_T>, String>);

}

namespace std {
    template <>
    struct hash<Stroika::Foundation::Characters::String> {
        size_t operator() (const Stroika::Foundation::Characters::String& arg) const;
    };
}

namespace Stroika::Foundation::Memory {
    class BLOB; // Forward declare to avoid mutual include issues
}

namespace Stroika::Foundation::DataExchange {
    template <typename T>
    struct DefaultSerializer; // Forward declare to avoid mutual include issues
    template <>
    struct DefaultSerializer<Stroika::Foundation::Characters::String> {
        Memory::BLOB operator() (const Stroika::Foundation::Characters::String& arg) const;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "String.inl"

#endif /*_Stroika_Foundation_Characters_String_h_*/
