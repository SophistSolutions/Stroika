/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_h_
#define _Stroika_Foundation_Characters_String_h_ 1

#include "../StroikaPreComp.h"

#include <locale>
#include <string>
#include <string_view>

#include "../Common/Compare.h"
#include "../Containers/Sequence.h"
#include "../Containers/Set.h"
#include "../Execution/ModuleInit.h"
#include "../Execution/Synchronized.h"
#include "../Memory/SharedByValue.h"
#include "../Traversal/Iterable.h"
#include "SDKString.h"

#include "Character.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * Description:
 *      This family of classes using something akin to the Letter/Envelope paradigm
 *      described in Copliens "Advanced C++ Programming Styles and Idioms").
 *
 *      The class String () is the main thing that users of this class will see. In
 *      fact, it would be EXTREMELY unusual to interact with the class String in any
 *      other way. However, as part of the internal implementation, and in some cases
 *      for optimizations in places where an alternate text representation is preferred,
 *      there may be some interest in the "letters" inside of Strings.
 *
 *      The class that String's own is a reference counted pointer to String::Rep.
 *      This class is abstract, and defines a protocol for getting at the string data
 *      (Character*) and for getting/setting the string length. All the rest of the
 *      String class is built in this interface, and subclasses of String::Rep only
 *      needed follow that prootcol to be used interchangably with the rest of Strings.
 *
 *      This is which NEED be used. However, Stroika provides several String SUBTYPES
 *      which act EXACTLY like a String, but offer different performance behaviors.
 *
 *              o   String_BufferedArray
 *              o   String_ExternalMemoryOwnership_ApplicationLifetime     (aka String_Constant)
 *              o   String_ExternalMemoryOwnership_StackLifetime
 *              o   String_wstring
 *
 *      Possible future additions
 *              o   String_Common
 *              o   String_AsciiOnlyOptimized
 *
 * TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-684 - 
 *              Change internal format / rep for String class to UTF8
 *              so strings generally represented more compactly. Adds cost for computing length (maybe cache). And for
 *              indexing (must be index in terms of characters but that doesnt work right now on windows due to
 *              using utf16 and not handling surrogates).
 *
 *      @todo   Consider if AsUTF8 () should start returning u8string instead of string, after Stroika v2.2, when we directly
 *              support C++20.
 *
 *      @todo   PROBALY get rid of
 *                                      nonvirtual  void        SetCharAt (Character c, size_t i);
 *              and fofrce people to use StringBUilder. I think that maybe only mutator except operator+= which is
 *              (or can be) safe, and is just wildly helpful, so proabby keep that. But document rationale!
 *              (well - .clear() is also mutaotr and operator= obvously).
 *
 *      @todo   EXPLAIN why InsertAt () returns string and Append() doesn't! Or - change it!
 *              Basic idea is that append is SO convnentit (+=) - that we just must support that
 *              and it can be done safely.
 *
 *              But InsertAt () is less common, and so we can just encourage the more safe strategy of
 *              returning a new string.
 *
 *              Weak arguemnt! BUt best I can come up with...
 *
 *      @todo   Handle few remaining cases of '// @todo - NOT ENVELOPE THREADSAFE' in implementaiton - mostly on
 *              Appends and Removes.
 *
 *              NO WORSE THAN THAT - MUCH IS NOT ENVELOPE THREADSAFE. Issue is that even with no sharing (sharecount=1)
 *              if I have one thread reading and one writing - that's intrinsically not safe!
 *
 *              I THINK the only ways out are:
 *                  o   lock every operation (makes reads too costly)
 *                  o   Do all multistep operations in reps (so they can manage locking (I think that
 *                      still means read/write locks like we have for Containers).
 *                  o   OR every changing operation makes a new string rep. THAT - as it turns out- maybe
 *                      the way to go (lisp/functional programming).
 *
 *              That may mean the CopyOnWrite stuff is useless here?
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
 *      @todo   Annotate basic string aliases as (basic_string alias - as below). At least try and think
 *              through if this seems ugly/pointless.
 *
 *      @todo   Add AsSDKString(TCHAR BUF) overload - as performance tweek. Not sure can be easily done safely so
 *              not sure of this, but could be a significant performance advantage. Test more...
 *
 *              Maybe better - AsTChar* - which takes BUFFER arg. And OPTIONALLY uses that buffer arg (depending
 *              on sizeof TCHAR???
 *
 *              Or maybe better - just change code that uses TSTring version of APIs to use W version of APIS!!!
 *              I THINK THAT IS BEST!
 *
 *      @todo   Add PadLeft/PadRight or FillLeft/FilLRight() - not sure which name is better. But idea is to
 *              produce a string which is indentical to the orig except that IF start len < n, then expand it with
 *              the given arg char repeated on the left or right.
 *
 *      @todo   Performance optimize FromUTF8, and AsUtf8() functions to not go through intermediate string
 *              objects!
 *
 *      @todo   Add #include of Comparer and template specialize Comparer for String??? Maybe
 *              Maybe not needed. Or maybe can avoid the #include and just do template specializaiton?
 *
 *      @todo   WHEN we have String_stdwstring implemented  - we can do String (wstring&& ctor that
 *              does std::move of wstring - and builds String_stdwstring rep! That could be notably
 *              faster for constructing Strings from wstring - in some cases...
 *
 *      @todo   Add a String_AsciiOnlyOptimized or String_isolatin1Optimized class (instead of String_UTF8).
 *              Idea is THIS can be very efficient and we can detect automatically when to create in
 *              normal string CTOR (i think - if strlen > 3? - check if all ascii and do).
 *
 *              This will automatically morph to other class (like buffered as we do for readonly). Maybe that
 *              means we need a backend, but no front-end?
 *
 *              OBSOLETE TODO
 *                  Consider adding a new subtype of string - OPTIMIZATION - which takes an ASCII argument (so can do less checking
 *                  and be more compact??? Perhaps similarly for REP storing stuff as UTF8?
 *
 *                  CLOSELY RELATED - MAYBE DO part of above (no check CTOR except assert).
 *
 *      @todo   Think out and document the whole choice about 'readonly' strings and all modifying member
 *              functions returning a new string. Has performance implications, but also usability.
 *              Not sure what way to go (probably do it), but clearly document!!! And docment reasons.
 *              Maybe can be done transparently with envelope (maybe already done?)
 *
 *      @todo   RFind() API should be embellished to include startAt etc, like regular Find () - but not 100%
 *              sure - think through...
 *
 *      @todo   Document better what APIs CHANGE the string, and what APIs have no effect. Verbs like
 *              "ToLowerCase" are AMBIGUOUS.
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
 *      @todo   Migrate most of the StringUtils stuff here like:
 *              > Contains- with CI optin
 *overload so can be string arg OR lambda!
 *              > Compare () - returns < less > more =0 for equal- with CI optin
 *              > Equals() - with CI optin
 *
 *      @todo   Add Left()/Right()/Mid() funtions - like basic (simple, vaguely useful - especially 'Right'()).
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
 *
 *      @todo   Implement String_Common
 *              NOT YET IMPLEMETNED - EVEN IN FAKE FORM - BECAUSE I"M NOT SURE OF SEMANTICS YET!
 *
 *              String_Common is a subtype of string you can use to construct a String object freely.
 *              It has no semantics requirements. However, it SHOULD only be used for strings which are
 *              commonly used, and where you wish to save space. The implementation will keep the memory for String_Common strings
 *              allocated permanently - for the lifetime of the application, and will take potentially
 *              extra time looking for the given string.
 *
 *              We MAY handle this like the DataExchange::Atom<> class - where we store the string in
 *              a hashtable (or map), and do quick lookup of associated index, and also store in a table
 *              (intead of vector of strings, use a big buffer we APPEND to, and whose index is the value
 *              of the stored rep. Then doing a PEEK() is trivial and efficient.
 *              class   String_Common : public String {
 *                  public:
 *                      explicit String_Common (const String& from);
 *              };
 *
 *      @todo   Add a String_UTF8 backend - (maybe a number of variants). Key is that these can be more compact
 *              and for many operaitons, just fine, but for insert, and a[i] = quite sucky.
 *              [RETHINK - See String_AsciiOnlyOptimized - and probably don't do this]
 *
 *      @todo   Handle Turkish toupper('i') problem. Maybe use ICU. Maybe add optional LOCALE parameter to routines where this matters.
 *              Maybe use per-thread global LOCALE settings. Discuss with KDJ.
 *              KDJ's BASIC SUGGESTION is - USE ICU and 'stand on their shoulders'.
 */
namespace Stroika::Foundation::Containers {
    template <typename T>
    class Set;
}

namespace Stroika::Foundation::Characters {

    class RegularExpression;
    class RegularExpressionMatch;

    /**
     *  The Stroika String class is an alternatve for the wstring class, which should be largely
     *  interoperable with code using wstring (there is wstring constructor and As<wstring>()
     *  methods).
     *
     *  The Stroika String class is conceptually a sequence of (UNICODE) Characters, and so there is
     *  no obvious way to map the Stroika String to a string. However, if you specify a codepage
     *  for conversion, or are converting to/from SDKString/SDKChar, there is builtin support for that.
     *
     *
     *  EOS Handling:
     *      The Stroika String class does support having embedded NUL-characters. It also supports
     *      returning wchar_t* strings which are NUL-terminated. In order to provide NUL-terminated
     *      strings through the cost c_str () API, Stroika may maintain strings internally as NUL-terminted
     *      wchar_t* (or at least retains the ability to as needed).
     *
     *      See String::c_str ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *                              with caveats!
     *
     *                              const wchar_t* c_str ()
     *                              const wchar_t* As<const wchar_t*> ()
     *                              const Character* As<const Character*> ()
     *
     *                              all return 'internal' pointers whose lifetime extends until the next modification of the String object.
     *                              But if another thread modifies this String object, that might not be usefully long.
     *
     *                              You can always copy a 'shared' String to a copy no other thread is modifying, and call c_str() on that.
     *
     *      @see   Concrete::String_BufferedArray
     *      @see   Concrete::String_ExternalMemoryOwnership_ApplicationLifetime     (aka String_Constant)
     *      @see   Concrete::String_ExternalMemoryOwnership_StackLifetime
     *      @see   Concrete::String_Common
     *
     *          Uses immutable string rep pattern.
     *
     *  \note   Design Choice - Iterable<T> / Iterator<T> behavior
     *      We have two basic choices of how to define the behavior of iterators:
     *      o   Live Update (like we do for Containers) - where changes to the
     *          String appear in iteration.
     *
     *      o   Snapshot at the start of iteration
     *
     *      The advantages of 'live update' are that its probably better / clearer semantics. We may
     *      want to switch to that. But to implement, we need to keep the update and iteration code
     *      in sync.
     *
     *      'Snapshot at the start of iteration' can be more efficient, and easier to implement.
     *      So - we do that for now.
     *
     *      Among 'snapshot' impl choices - we COULD do lazy copy snapshot (COW). That would perform best.
     *      But to do so - the way the code is currently structured, we would need to use enable_shared_from_this
     *      so we can recover a shared_ptr in ReadOnlyRep::_Rep::MakeIterator.
     *
     *      enable_shared_from_this() would add costs (at least size) even when we don't use String iteration
     *      (which we didnt even impelement for a couple years, so may not be that critical).
     *
     *      For now - stick to simple impl - of just copy on start of iteration.
     *          -- LGP 2013-12-17
     *
     *  \note   Design Note: operator overloads
     *      See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     *
     *      We use non-member function operator overloads for comparisons and +, so that
     *      things like L"fred" != String(L"banry") works.
     *
     *      Since the enclosing namespace of both arguments is considered in lookups this should generally work
     *      pretty well, I think. At least we are testing this as of 2014-12-20.
     *
     *  \note   Static Initialization, file scope variables, application lifetime variables.
     *          It \em IS safe to use the String class at file scope. The constructors are carefully crafted to
     *          operate properly, even if used at file scope, and to initialize other strings or objects.
     *          @see "Test of STATIC FILE SCOPE INITIALIZATION"
     *
     *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     */
    class String : public Traversal::Iterable<Character> {
    private:
        using inherited = Iterable<Character>;

    public:
        /**
         * All the constructors are obvious, except to note that NUL-character ARE allowed in strings,
         * except for the case of single char* argument constructors - which find the length based on
         * the terminating NUL-character.
         *
         *  \req for String (const basic_string_view<wchar_t>& str) - str[str.length()]=='\0';   
         *       c-string nul-terminated (which happens automatically with L"xxx"sv)
         */
        String ();
#if __cpp_char8_t >= 201811L
        String (const char8_t* cString);
#endif
        String (const char16_t* cString);
        String (const char32_t* cString);
        String (const wchar_t* cString);
#if __cpp_char8_t >= 201811L
        String (const char8_t* from, const char8_t* to);
#endif
        String (const char16_t* from, const char16_t* to);
        String (const char32_t* from, const char32_t* to);
        String (const wchar_t* from, const wchar_t* to);
        String (const Character* from, const Character* to);
        String (const basic_string_view<wchar_t>& str);
        String (const wstring& r);
        String (const u16string& r);
        String (const u32string& r);
        String (const Iterable<Character>& src);
        String (const String& from) noexcept;
        String (String&& from) noexcept;
        explicit String (const Character& c);

    public:
        ~String () = default;

    protected:
        class _IRep;

    protected:
        using _IterableRepSharedPtr = Iterable<Character>::_IterableRepSharedPtr;

    protected:
        using _SharedPtrIRep = PtrImplementationTemplate<_IRep>;

    protected:
        /**
         */
        using _SafeReadRepAccessor = Iterable<Character>::_SafeReadRepAccessor<_IRep>;

    protected:
        /**
         * \req rep MUST be not-null
         *  However, with move constructor, it maybe null on exit.
         */
        String (const _SharedPtrIRep& rep) noexcept;
        String (_SharedPtrIRep&& rep) noexcept;

    public:
        nonvirtual String& operator= (const String& rhs) = default;
        nonvirtual String& operator= (String&& newString) noexcept = default;

    public:
        /**
         *  Create a String object from a 'char-based' utf-8 encoded string.
         *
         *  \par Example Usage
         *      \code
         *          VerifyTestResult (string (u8"שלום") == String::FromUTF8 (u8"שלום").AsUTF8 ());
         *      \endcode
         *
         *  \note   Reading improperly encoded text may result in a RuntimeException indicating inproperly encoded characters.
         *
         */
        static String FromUTF8 (const char* from);
        static String FromUTF8 (const char* from, const char* to);
        static String FromUTF8 (const string& from);
#if __cpp_char8_t >= 201811L
        static String FromUTF8 (const char8_t* from);
        static String FromUTF8 (const char8_t* from, const char8_t* to);
        static String FromUTF8 (const u8string& from);
#endif

    public:
        /**
         *  Create a String object from a 'SDKChar' (os-setting - current code page) encoded string.
         *  See @SDKChar
         *  See @SDKString
         *
         *  \note   Reading improperly encoded text may result in a RuntimeException indicating inproperly encoded characters.
         *
         */
        static String FromSDKString (const SDKChar* from);
        static String FromSDKString (const SDKChar* from, const SDKChar* to);
        static String FromSDKString (const SDKString& from);

    public:
        /**
         *  Create a String object from a 'char-based' (os-setting - current code page) encoded string.
         *
         *  \note   Reading improperly encoded text may result in a RuntimeException indicating inproperly encoded characters.
         *
         */
        static String FromNarrowSDKString (const char* from);
        static String FromNarrowSDKString (const char* from, const char* to);
        static String FromNarrowSDKString (const string& from);

    public:
        /**
         *  Create a String object from a 'char-based' on the encoding from the argument locale.
         *  This throws an exception if there is an error performing the conversion.
         *
         *  \note   Reading improperly encoded text may result in a RuntimeException indicating inproperly encoded characters.
         *
         */
        static String FromNarrowString (const char* from, const locale& l);
        static String FromNarrowString (const char* from, const char* to, const locale& l);
        static String FromNarrowString (const string& from, const locale& l);

    public:
        /**
         *  Create a String object from ascii text. 
         *
         *  \note   Reading non-ascii text will result in a RuntimeException indicating inproperly encoded characters.
         *
         *  \note - before Stroika v2.1d23 this was a requires check, and an assertion error to pass in non-ascii text
         */
        static String FromASCII (const char* from);
        static String FromASCII (const char* from, const char* to);
        static String FromASCII (const string& from);
        static String FromASCII (const wchar_t* from);
        static String FromASCII (const wchar_t* from, const wchar_t* to);
        static String FromASCII (const wstring& from);

    public:
        /**
         *  Create a String object from ISO-Latin-1 (https://en.wikipedia.org/wiki/ISO/IEC_8859-1)
         *
         *  \note Though ISO-Latin-1 is is defined to only have 192 valid characters (source), UNICODE
         *        is defined so that the first 256 code points match ISO-Latin-1.
         *
         *        Because of this, This function allows ANY 8-bit) characters at all - to be passed in, and those characters
         *        will be mapped to UNICODE characters (of the same code point) in the resulting String.
         *
         *  \note Alias From8bitASCII () or FromExtendedASCII () or FromAnyCharset (), FromAnyCharacterSet ()
         */
        static String FromISOLatin1 (const char* from);
        static String FromISOLatin1 (const char* start, const char* end);
        static String FromISOLatin1 (const string& from);

    private:
        static _SharedPtrIRep mkEmpty_ ();
        static _SharedPtrIRep mk_ (const wchar_t* start, const wchar_t* end);
        static _SharedPtrIRep mk_ (const wchar_t* start1, const wchar_t* end1, const wchar_t* start2, const wchar_t* end2);
        static _SharedPtrIRep mk_ (const char16_t* start, const char16_t* end);
        static _SharedPtrIRep mk_ (const char32_t* start, const char32_t* end);

    public:
        nonvirtual String& operator+= (Character appendage);
        nonvirtual String& operator+= (const String& appendage);
        nonvirtual String& operator+= (const wchar_t* appendageCStr);

    public:
        /**
         *  @see    Append() for a similar function that modifies 'this'
         */
        nonvirtual String Concatenate (const String& rhs) const;
        nonvirtual String Concatenate (const wchar_t* appendageCStr) const;

    public:
        /**
         *  Returns the number of characters in the String. Note that this may not be the same as bytes,
         *  does not include NUL termination, and doesn't in any way respect NUL termination (meaning
         *  a nul-character is allowed in a Stroika string.
         */
        nonvirtual size_t GetLength () const;

    public:
        /**
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  Alias for *this = String ();
         */
        nonvirtual void clear ();

    public:
        /**
         */
        nonvirtual const Character GetCharAt (size_t i) const;

    public:
        /**
         */
        nonvirtual void SetCharAt (Character c, size_t i);

    public:
        /**
         *  \brief return (read-only) Character object
         *
         *  Alias for GetCharAt (size_t i) const;
         */
        nonvirtual const Character operator[] (size_t i) const;

    public:
        /**
         *  InsertAt() constructs a new string by taking this string, and inserting the argument
         *  characters.
         *
         *  \em Note that for repeated insertions, this is much less efficient than just
         *      using StringBuilder.
         */
        nonvirtual String InsertAt (Character c, size_t at) const;
        nonvirtual String InsertAt (const String& s, size_t at) const;
        nonvirtual String InsertAt (const wchar_t* from, const wchar_t* to, size_t at) const;
        nonvirtual String InsertAt (const Character* from, const Character* to, size_t at) const;

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
         */
        nonvirtual void Append (Character c);
        nonvirtual void Append (const String& s);
        nonvirtual void Append (const wchar_t* s);
        nonvirtual void Append (const wchar_t* from, const wchar_t* to);
        nonvirtual void Append (const Character* from, const Character* to);

    public:
        /**
         * Remove the characters at 'charAt' (RemoveAt/1) or between 'from' and 'to'.
         *
         * It is an error if this implies removing characters off the end of the string.
         *
         *  \par Example Usage
         *      \code
         *          String mungedData = L"04 July 2014;
         *          if (optional<pair<size_t, size_t>> i = mungedData.Find (RegularExpression {L"0[^\b]"})) {
         *              mungedData = mungedData.RemoveAt (*i);
         *          }
         *      \endcode
         *
         *  \req (charAt < GetLength ())
         *  \req (from <= to)
         *  \req (to <= GetLength ())
         *
         *  \em Note that this is quite inefficent: consider using StringBuilder
         */
        nonvirtual String RemoveAt (size_t charAt) const;
        nonvirtual String RemoveAt (size_t from, size_t to) const;
        nonvirtual String RemoveAt (pair<size_t, size_t> fromTo) const;

    public:
        /**
         *  Remove the first occurence of Character 'c' from the string. Not an error if none
         *  found.
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
         *  \req  (to <= GetLength ());     // for 2-arg variant
         *
         *  \par Example Usage
         *      \code
         *          String tmp { L"This is good" };
         *          Assert (tmp.SubString (5) == L"is good");
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          const String_Constant kTest_ { L"a=b" };
         *          const String_Constant kLbl2LookFor_ { L"a=" };
         *          if (resultLine.Find (kLbl2LookFor_)) {
         *              String  tmp { resultLine.SubString (kLbl2LookFor_.length ()) };
         *          }
         *          Assert (tmp == L"b");
         *      \endcode
         *
         *  OVERLOADS WITH ptrdiff_t:
         *
         *  This is like SubString() except that if from/to are negative, they are treated as relative to the end
         *  of the String.
         *
         *  So for example, SubString (0, -1) is equivalent to SubString (0, GetLength () - 1) - and so is an
         *  error if the string is empty.
         *
         *  Similarly, SubString (-5) is equivalent to SubString (GetLength ()-5, GetLength ()) - so can be used
         *  to grab the end of a string.
         *
         *  \req  (adjustedFrom <= adjustedTo);
         *  \req  (adjustedTo <= GetLength ());     // for 2-arg variant
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

    private:
        nonvirtual size_t SubString_adjust_ (unsigned int fromOrTo, size_t myLength) const;
        nonvirtual size_t SubString_adjust_ (unsigned long fromOrTo, size_t myLength) const;
        nonvirtual size_t SubString_adjust_ (unsigned long long fromOrTo, size_t myLength) const;
        nonvirtual size_t SubString_adjust_ (int fromOrTo, size_t myLength) const;
        nonvirtual size_t SubString_adjust_ (long fromOrTo, size_t myLength) const;
        nonvirtual size_t SubString_adjust_ (long long fromOrTo, size_t myLength) const;

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

    private:
        static String SubString_ (const _SafeReadRepAccessor& thisAccessor, size_t thisLen, size_t from, size_t to);

    public:
        /**
         *  Return 'count' copies of this String (concatenated after one another).
         */
        nonvirtual String Repeat (unsigned int count) const;

    public:
        /**
         *  Returns true if the argument character or string is found anywhere inside this string.
         *  This is equivalent to
         *      return Match (".*" + X + L".*");    // If X had no characters which look like they are part of
         *                                          // a regular expression
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
         *      return Match (X + L".*");
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
         *      return Match (X + L".*");
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
         *          Assert (String (L"abc").Match (L"abc"));
         *          Assert (not (String (L"abc").Match (L"bc")));
         *          Assert (String (L"abc").Match (L".*bc"));
         *          Assert (not String (L"abc").Match (L"b.*c"));
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          static const RegularExpression kSonosRE_{L"([0-9.:]*)( - .*)"_RegEx};
         *          static const String            kTestStr_{L"192.168.244.104 - Sonos Play:5"};
         *          optional<String> match1;
         *          optional<String> match2;
         *          VerifyTestResult (kTestStr_.Match (kSonosRE_, &match1, &match2) and match1 == L"192.168.244.104" and match2 == L" - Sonos Play:5");
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          // https://tools.ietf.org/html/rfc3986#appendix-B
         *          static const RegularExpression kParseURLRegExp_{L"^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?"_RegEx};
         *          optional<String>               scheme;
         *          optional<String>               authority;
         *          optional<String>               path;
         *          optional<String>               query;
         *          optional<String>               fragment;
         *          if (rawURL.Match (kParseURLRegExp_, nullptr, &scheme, nullptr, &authority, &path, nullptr, &query, nullptr, &fragment)) {
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
        nonvirtual bool Match (const RegularExpression& regEx) const;
        nonvirtual bool Match (const RegularExpression& regEx, Containers::Sequence<String>* matches) const;
        template <typename... OPTIONAL_STRINGS>
        nonvirtual bool Match (const RegularExpression& regEx, OPTIONAL_STRINGS&&... subMatches) const;

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
         *  \req (startAt <= GetLength ());
         *
         *  \par Example Usage
         *      \code
         *          const String kTest_ { L"a=b" };
         *          const String kLbl2LookFor_ { L"a=" };
         *          if (kTest_.Find (kLbl2LookFor_)) {
         *              String  tmp { kTest_.SubString (kLbl2LookFor_.length ()) };
         *          }
         *          Assert (tmp == L"b");
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
         *  search String ("AAAA").FindEach ("AA"), you will get 2 answers ({0, 2}).
         *
         *  @see Find ()
         *  @see FindEachString ()
         *  @see Match ()
         */
        nonvirtual vector<pair<size_t, size_t>> FindEach (const RegularExpression& regEx) const;
        nonvirtual vector<size_t> FindEach (const String& string2SearchFor, CompareOptions co = CompareOptions::eWithCase) const;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          const String_Constant kTest_    { L"a=b," };
         *          const RegularExpression kRE_    { L"a=(.*)[, ]" };
         *          Sequence<String>      tmp1      { kTest_.FindEachString (kRE_) };
         *          Assert (tmp1.size () == 1 and tmp1[0] == L"a=b,");
         *          Sequence<RegularExpressionMatch>      tmp2 { kTest_.FindEachMatch (kRE_) };
         *          Assert (tmp2.size () == 1 and tmp2[0].GetFullMatch () == L"a=b," and tmp2[0].GetSubMatches () == Sequence<String> {L"b"});
         *      \endcode
         *
         *  @see Find ()
         *  @see FindEachString ()
         *  @see Match ()
         */
        nonvirtual vector<RegularExpressionMatch> FindEachMatch (const RegularExpression& regEx) const;

        /**
         *  \par Example Usage
         *      \code
         *          const String_Constant kTest_ { L"a=b, c=d" };
         *          const RegularExpression kRE_ { L"(.)=(.)" };
         *          Assert ((kTest_.FindEachString (kRE_) ==  vector<String> {L"a=b", L"c=d"}));
         *      \endcode
         *
         *  @see Find ()
         *  @see FindEachMatch ()
         *  @see Match ()
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
        nonvirtual optional<size_t> RFind (Character c) const;
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
         *          mungedData = mungedData.ReplaceAll (RegularExpression{ L"\\b0+" }, L"");    // strip all leading zeros
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
         *  Utility to squash out bad characters from a string, and replace them with the argument 'safe' character. Handly
         *  for things like file names, etc...
         *
         *  If replacement missing (the default) - the badCharacters are just removed, and not replaced.
         *
         *  \note FilteredString could have been called 'SafeString'
         */
        [[deprecated ("Use ReplaceAll instead - as of v2.1d11 - replacement char betcomes string")]] nonvirtual String FilteredString (const Iterable<Character>& badCharacters, optional<Character> replacement = optional<Character>{}) const;
        [[deprecated ("Use ReplaceAll instead - as of v2.1d11 - replacement char betcomes string")]] nonvirtual String FilteredString (const RegularExpression& badCharacters, optional<Character> replacement = optional<Character>{}) const;
        [[deprecated ("Use ReplaceAll instead - as of v2.1d11 - replacement char betcomes string")]] nonvirtual String FilteredString (const function<bool (Character)>& badCharacterP, optional<Character> replacement = optional<Character>{}) const;

    public:
        /**
         *  Break this String into constituent parts. This is a simplistic API but at least handy as is.
         *
         *  The caller can specify the token seperators by set, by lambda. This defaults to the lambda "isWhitespace".
         *
         *  This is often called 'Split' in other APIs. This is NOT (as is now) a replacement for flex, but just for
         *  simple, but common string splitting needs (though if I had a regexp param, it may approach the power of flex).
         *
         *  \par Example Usage
         *      \code
         *          String  t { L"ABC DEF G" };
         *          Assert (t.Tokenize ().length () == 3);
         *          Assert (t.Tokenize ()[1] == L"DEF");
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          String  t { L"foo=   7" };
         *          auto    tt = t.Tokenize (Set<Character> { '=' });
         *          Assert (t.length () == 2);
         *          Assert (t[1] == L"7");
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
         *       String name = origName.RTrim ([] (Character c) { return c == '\\';});        // Trim a trailing backslash(s), if present
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
         *  CopyTo () does NOT nul-terminate the target buffer, but DOES assert that (bufTo-bufFrom)
         *  is >= this->GetLength ()
         */
        nonvirtual void CopyTo (Character* bufFrom, Character* bufTo) const;
        nonvirtual void CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const;

    public:
        /**
         * Convert String losslessly into a standard C++ type.
         *
         *  Only specifically specialized variants are supported. Supported type 'T' values include:
         *      o   wstring
         *      o   const wchar_t*
         *      o   const Character*
         *      o   u16string
         *      o   u32string
         *
         *  \note
         *      o   As<u16string> () equivilent to AsUTF16 () calll
         *      o   As<u32string> () equivilent to AsUTF32 () calll
         *
         *  \note   We tried to also have template<typename T> explicit operator T () const; - conversion operator - but
         *          We got too frequent confusion in complex combinations of templates, like with:
         *          Set<String> x ( *optional<String> {String ()) );       // fails cuz calls operator Set<String> ()!
         *          Set<String> x { *optional<String> {String ()) };       // works as expected
         */
        template <typename T>
        nonvirtual T As () const;
        template <typename T>
        nonvirtual void As (T* into) const;

    public:
        /**
         *  Create a narrow string object from this, based on the encoding from the argument locale.
         *  This throws an exception if there is an error performing the conversion, and the 'into' overload
         *  leaves 'into' in an undefined (but safe) state.
         */
        nonvirtual string AsNarrowString (const locale& l) const;
        nonvirtual void   AsNarrowString (const locale& l, string* into) const;

    public:
        /**
         * Convert String losslessly into a standard C++ type.
         * Only specifically specialized variants are supported (right now just <string> supported).
         * Note - template param is optional.
         *
         *  SUPPORTED result type "T": values are:
         *      string
         *      u8string        // C++2a or higher
         */
        template <typename T = string>
        nonvirtual T AsUTF8 () const;
        template <typename T = string>
        nonvirtual void AsUTF8 (T* into) const;

    public:
        /**
         * Convert String losslessly into a standard C++ type u16string.
         *
         *  \note - the resulting string may have a different length than this->GetLength() due to surrogates
         */
        nonvirtual u16string AsUTF16 () const;
        nonvirtual void      AsUTF16 (u16string* into) const;

    public:
        /**
         * Convert String losslessly into a standard C++ type u32string.
         *
         *  \note - As of Stroika 2.1d23 - the resulting string may have a different length than this->GetLength() due to surrogates,
         *          but eventually the intent is to fix Stroika's string class so this is not true, and it returns the length of the string
         *          in GetLength () with surrogates removed (in other words uses ucs32 represenation). But not there yet.
         */
        nonvirtual u32string AsUTF32 () const;
        nonvirtual void      AsUTF32 (u32string* into) const;

    public:
        /**
         */
        nonvirtual SDKString AsSDKString () const;
        nonvirtual void      AsSDKString (SDKString* into) const;

    public:
        /**
         */
        nonvirtual string AsNarrowSDKString () const;
        nonvirtual void   AsNarrowSDKString (string* into) const;

    public:
        /**
         * Convert String losslessly into a standard C++ type.
         * Only specifically specialized variants are supported (right now just <string> supported).
         * The source string MUST be valid ascii characters - throw RuntimeErrorException<>
         *
         *  \note - this is a (compatible) change of behavior: before Stroika v2.1d23, this would assert out on invalid ASCII.
         */
        template <typename T = string>
        nonvirtual T AsASCII () const;
        template <typename T = string>
        nonvirtual void AsASCII (T* into) const;

    public:
        /**
         *  Only defined for CHAR_TYPE=Character or wchar_t (for now).
         *
         *  Lifetime is ONLY up until next method access to String.
         *
         *  @see c_str()
         *
         *  \note Design Note:
         *      This is eqivilent to returning pair { c_str (), c_str () + length () } - only a bit faster since all in one call.
         *      So long as we support c_str () returning an internal wchar_t* pointer, this adds no extra cost/constraints.
         */
        template <typename CHAR_TYPE>
        nonvirtual pair<const CHAR_TYPE*, const CHAR_TYPE*> GetData () const;

    public:
        /**
         *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
         */
        [[deprecated ("in Stroika v2.1d24 - use ThreeWayComparer{co} () instead")]] int Compare (const String& rhs, CompareOptions co = CompareOptions::eWithCase) const;
        [[deprecated ("in Stroika v2.1d24 - use ThreeWayComparer{co} () instead")]] int Compare (const Character* rhs, CompareOptions co = CompareOptions::eWithCase) const;
        [[deprecated ("in Stroika v2.1d24 - use ThreeWayComparer{co} () instead")]] int Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co = CompareOptions::eWithCase) const;
        [[deprecated ("in Stroika v2.1d24 - use ThreeWayComparer{co} () instead")]] int Compare (const wchar_t* rhs, CompareOptions co = CompareOptions::eWithCase) const;
        [[deprecated ("in Stroika v2.1d24 - use ThreeWayComparer{co} () instead")]] int Compare (const wchar_t* rhsStart, const wchar_t* rhsEnd, CompareOptions co = CompareOptions::eWithCase) const;

    public:
        /**
         * @brief   Return true of the two argument strings are equal. This is equivalent to
         *              lhs.Compare (rhs, co);
         */
        nonvirtual bool Equals (const String& rhs, CompareOptions co = CompareOptions::eWithCase) const;
        nonvirtual bool Equals (const wchar_t* rhs, CompareOptions co = CompareOptions::eWithCase) const;

    public:
        struct EqualToCI;

    public:
        struct LessCI;

    public:
        struct ThreeWayComparer;

    public:
        /**
         *  Alias for basic_string>char>::npos - except this is constexpr.
         *
         *  This is only used for 'STL-compatabiliity' apis, like substr (), find, rfind (), etc.
         */
        static constexpr size_t npos = static_cast<size_t> (-1);

    public:
        /**
         *  basic_string alias: size = GetLength
         */
        nonvirtual size_t size () const;

    public:
        /**
         *  basic_string alias: length = GetLength
         */
        nonvirtual size_t length () const;

    public:
        /**
         *  As with STL, the return value of the data () function should NOT be assumed to be
         *  NUL-terminated
         *
         *  The lifetime of the pointer returned is guaranteed until the next non-const call to this String
         *  envelope class (that is if other reps change, or are acceessed this data will not
         *  be modified)
         *
         *  \note THREAD-SAFETY - small risk - be sure reference to returned pointer cleaered before String modified
         */
        nonvirtual const wchar_t* data () const;

    public:
        /**
         *  This will always return a value which is NUL-terminated.
         *
         *  The lifetime of the pointer returned is guaranteed until the next non-const call to this String
         *  envelope class (that is if other reps change, or are acceessed this data will not be modified)
         *  Note also that Stroika strings ALLOW internal nul bytes, so though the Stroika string
         *  class NUL-terminates, it does nothing to prevent already existng NUL bytes from causing
         *  confusion elsewhere.
         *
         *  Lifetime is ONLY up until next non-const method access to String, so this API is intrinsically not threadsafe.
         *  That is - if you call this on a String, you better not be updating the string at the same time!
         *
         *  \note THREAD-SAFETY - small risk - be sure reference to returned pointer cleaered before String modified
         *
         *  @see GetData ()
         */
        nonvirtual const wchar_t* c_str () const noexcept;

    public:
        /**
         *  need more overloads.
         *
         *  Returns String::npos if not found, else the zero based index.
         */
        nonvirtual size_t find (wchar_t c, size_t startAt = 0) const;

    public:
        /**
         *   need more overloads.
         *
         *   Returns String::npos if not found, else the zero based index.
         */
        nonvirtual size_t rfind (wchar_t c) const;

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

    protected:
        nonvirtual void _AssertRepValidType () const;

    public:
        friend wostream& operator<< (wostream& out, const String& s);
        friend String    operator+ (const wchar_t* lhs, const String& rhs);
    };

    template <>
    void String::As (wstring* into) const;
    template <>
    wstring String::As () const;
    template <>
    const wchar_t* String::As () const;
    template <>
    const Character* String::As () const;
    template <>
    u16string String::As () const;
    template <>
    u32string String::As () const;

    template <>
    void String::AsUTF8 (string* into) const;
    template <>
    string String::AsUTF8 () const;
#if __cpp_char8_t >= 201811L
    template <>
    void String::AsUTF8 (u8string* into) const;
    template <>
    u8string String::AsUTF8 () const;
#endif

    template <>
    void String::AsASCII (string* into) const;
    template <>
    string String::AsASCII () const;

    template <>
    pair<const Character*, const Character*> String::GetData () const;
    template <>
    pair<const wchar_t*, const wchar_t*> String::GetData () const;

    /*
     *  Equivilent to std::equal_to<String>, except doing case insensitive compares
     */
    struct String::EqualToCI : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        nonvirtual bool operator() (const String& lhs, const String& rhs) const;
    };

    /*
     *  Equivilent to std::less<String>, except doing case insensitive compares
     */
    struct String::LessCI : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder> {
        nonvirtual bool operator() (const String& lhs, const String& rhs) const;
    };

    /**
     */
    struct String::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        constexpr ThreeWayComparer (CompareOptions co = CompareOptions::eWithCase);
        // Extra overloads a slight performance improvement
        nonvirtual int operator() (const String& lhs, const String& rhs) const;
        nonvirtual int operator() (const Character* lhs, const String& rhs) const;
        nonvirtual int operator() (const String& lhs, const Character* rhs) const;
        nonvirtual int operator() (const Character* lhs, const Character* rhs) const;
        nonvirtual int operator() (const wchar_t* lhs, const String& rhs) const;
        nonvirtual int operator() (const String& lhs, const wchar_t* rhs) const;
        nonvirtual int operator() (const wchar_t* lhs, const wchar_t* rhs) const;

        CompareOptions fCompareOptions;
    };

    /**
     * Protected helper Rep class.
     */
    class String::_IRep
        : public Iterable<Character>::_IRep
#if !qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        ,
          public Traversal::IterableBase::enable_shared_from_this_PtrImplementationTemplate<String::_IRep>
#endif
    {
    protected:
        using _IterableRepSharedPtr = String::_IterableRepSharedPtr;

    protected:
        using _SharedPtrIRep = String::_SharedPtrIRep;

    protected:
        _IRep () = default;

    protected:
        _IRep (const wchar_t* start, const wchar_t* end);

    public:
        virtual ~_IRep () = default;

    protected:
        /**
         *  PROTECTED INLINE UTILITY
         *  \req *end == '\0'
         *
         *  So allocated storage MUST always contain space for the terminating NUL-character.
         */
        nonvirtual void _SetData (const wchar_t* start, const wchar_t* end);

    protected:
        /**
         *  PROTECTED INLINE UTILITY
         */
        nonvirtual size_t _GetLength () const;

    protected:
        /**
         *  PROTECTED INLINE UTILITY
         */
        nonvirtual Character _GetAt (size_t index) const;

    protected:
        /**
         *  PROTECTED INLINE UTILITY
         */
        nonvirtual const Character* _Peek () const;

        // Overrides for Iterable<Character>
    public:
        virtual Traversal::Iterator<Character> MakeIterator (Traversal::IteratorOwnerID suggestedOwner) const override;
        virtual size_t                         GetLength () const override;
        virtual bool                           IsEmpty () const override;
        virtual void                           Apply (_APPLY_ARGTYPE doToElement) const override;
        virtual Traversal::Iterator<Character> FindFirstThat (_APPLYUNTIL_ARGTYPE, Traversal::IteratorOwnerID suggestedOwner) const override;

    public:
        nonvirtual Character GetAt (size_t index) const;

    public:
        nonvirtual pair<const Character*, const Character*> GetData () const;

    public:
        /*
         *  Return a pointer to mostly standard (wide, nul-terminated) C string,
         *  whose lifetime extends to the next non-const call on this rep.
         *
         *  It is only 'mostly' standard because it is allowed to have nul-chars embedded in it. But it will
         *  always have str[len] == 0;
         *
         *  \ensure returnResult[len] == '\0';
         */
        virtual const wchar_t* c_str_peek () const noexcept = 0;

    public:
        /*
         *  CopyTo () copies the contents of this string to the target buffer.
         *  CopyTo () does NOT nul-terminate the target buffer, but DOES assert that (bufTo-bufFrom) is >= this->GetLength ()
         */
        nonvirtual void CopyTo (Character* bufFrom, Character* bufTo) const;
        nonvirtual void CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const;

    protected:
        const wchar_t* _fStart;
        const wchar_t* _fEnd; // \note - _fEnd must always point to a 'NUL' character, so the underlying array extends one or more beyond

    private:
        friend class String;
    };

    /**
     *  \brief user defined literal for ASCII string. Equivilent to String::FromASCII () - except this REQUIRES its argument
     *         be a forever-lived nul-terminated C string.
     *
     *  \req each character isascii()
     */
    String operator"" _ASCII (const char* str, size_t len);

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

    /**
     *  Basic operator overload with the obivous meaning, and simply indirect to @String::Compare (const String& rhs)
     */
    bool operator< (const String& lhs, const String& rhs);
    bool operator< (const String& lhs, const wchar_t* rhs);
    bool operator< (const wchar_t* lhs, const String& rhs);

    /**
     *  Basic operator overload with the obivous meaning, and simply indirect to @String::Compare (const String& rhs)
     */
    bool operator<= (const String& lhs, const String& rhs);
    bool operator<= (const String& lhs, const wchar_t* rhs);
    bool operator<= (const wchar_t* lhs, const String& rhs);

    /**
     *  Basic operator overload with the obivous meaning, and simply indirect to @String::Equals (const String& rhs)
     */
    bool operator== (const String& lhs, const String& rhs);
    bool operator== (const String& lhs, const wchar_t* rhs);
    bool operator== (const wchar_t* lhs, const String& rhs);

    /**
     *  Basic operator overload with the obivous meaning, and simply indirect to @String::Equals (const String& rhs)
     */
    bool operator!= (const String& lhs, const String& rhs);
    bool operator!= (const String& lhs, const wchar_t* rhs);
    bool operator!= (const wchar_t* lhs, const String& rhs);

    /**
     *  Basic operator overload with the obivous meaning, and simply indirect to @String::Compare (const String& rhs)
     */
    bool operator>= (const String& lhs, const String& rhs);
    bool operator>= (const String& lhs, const wchar_t* rhs);
    bool operator>= (const wchar_t* lhs, const String& rhs);

    /**
     *  Basic operator overload with the obivous meaning, and simply indirect to @String::Compare (const String& rhs)
     */
    bool operator> (const String& lhs, const String& rhs);
    bool operator> (const String& lhs, const wchar_t* rhs);
    bool operator> (const wchar_t* lhs, const String& rhs);

    /**
     *  Basic operator overload with the obivous meaning, and simply indirect to @String::Concatenate (const String& rhs)
     */
    String operator+ (const String& lhs, const wchar_t* rhs);
    String operator+ (const String& lhs, const String& rhs);
    String operator+ (const wchar_t* lhs, const String& rhs);

    /**
     *  This can be referenced in your ModuleInitializer<> to force correct inter-module construction order.
     */
    Execution::ModuleDependency MakeModuleDependency_String ();
}

namespace std {
    template <>
    class hash<Stroika::Foundation::Characters::String> {
    public:
        size_t operator() (const Stroika::Foundation::Characters::String& arg) const
        {
            return hash<wstring> () (arg.As<wstring> ());
        }
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "String.inl"

#endif /*_Stroika_Foundation_Characters_String_h_*/
