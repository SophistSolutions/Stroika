/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_h_
#define _Stroika_Foundation_Characters_String_h_    1

/**
 *  \file
 *
 * Description:
 *
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
 *              o   String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly     (aka String_Constant)
 *              o   String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite
 *              o   String_ExternalMemoryOwnership_StackLifetime_ReadOnly
 *              o   String_ExternalMemoryOwnership_StackLifetime_ReadWrite
 *              o   String_Common
 */

#include    "../StroikaPreComp.h"

#include    <string>

#include    "../Execution/ModuleInit.h"
#include    "../Memory/SharedByValue.h"
#include    "TString.h"

#include    "Character.h"



/**
 * TODO:
 *
 *      @todo   Think out and document the whole choice about 'readonly' strings and all modifying member
 *              functions returning a new string. Has performance implications, but also usability.
 *              Not sure what way to go (probably do it), but clearly document!!! And docment reasons.
 *              Maybe can be done transparently with envelope (maybe already done?)
 *
 *      @todo   FromTString() overload like FromUTF8(), and docs for BOTH
 *
 *      @todo   RFind() API should be embellished to include startAt etc, like regular Find () - but not 100%
 *              sure - think through...
 *
 *      @todo   Redo SetLength() API, so caller must specify fill-character.
 *
 *      @todo   Consider changing API for RemoveAt() - so second argument is ENDPOINT - to be more consistent
 *               with other APIs.
 *
 *      @todo   Document better what APIs CHANGE the string, and what APIs have no effect. Verbs like
 *              "ToLowerCase" are AMBIGUOUS.
 *
 *      @todo   CLEANUP Tokenize API, AND VERY IMPROTANMTLY - either DOCUMENT or indirect to Tokenize() API.
 *              MANY people will expect a String.Split() function to exist. This is what the existing TOKENIZE() API does but thats
 *              not obvious!!!! MUST BE SUPER CLEARLY DOCUMENTED.
 *              Be sure docs for TOKENIZE are clear this is not a FLEX replacement - but just a very simple 'split' like functionaliuty.
 *              not totally clear what name is best (split or tokenize()).
 *
 *      @todo   MAYBE also add ReplaceOne() function (we have ReplaceAll() now).
 *
 *      @todo   Make another pass over String_ExternalMemoryOwnership_StackLifetime_ReadOnly/ReadWrite
 *              documentation, and make clearer, and document the tricky bits loosely
 *              alluded to in the appropriate place if the API is truely DOABLE.
 *
 *      @todo   At this stage - for our one trivial test - performance is now about 5% faster than
 *              visual studio.net 2010, but
 *              about a factor of 2 SLOWER than GCC (as of 2011-12-04).
 *
 *              I SUSPECT the next big change to address this will be && MOVE OPERATOR support.
 *
 *      @todo   EITHER embed data as buffer in BufferdString - so small strings fit without malloc,
 *              or use separate buffer. Good reasons for both ways. Not sure whats best.
 *
 *              o   At least one StringRep variant (maybe the stanrdard/common string-buffer rep
 *                  which has a fixed-size buffer, and uses that INLINE, and allocates POINTER if that isn't big enuf?
 *
 *              o   PROBABLY best to just DO direct blockallocated() calls for data < fixed size
 *
 *      @todo   Fix const   Memory::SharedByValue<String::String::Rep>  String::kEmptyStringRep_ (new String_CharArray::MyRep_ (nullptr, 0), &String::Clone_);
 *              to properly handle cross-module startup (not safe as is - probably use ModuleInit<> stuff. OR use static intit PTR and assure its fixed
 *              just in CPP file
 *
 *      @todo   Move DOCS in the top of this file down to the appropriate major classes - and then review the implemantion and make sure
 *              it is all correct for each (especially SetStorage () sutff looks quesitonable)
 *
 *      @todo   Use new CopyTo() method to get rid of MOST of the casts/memcpy code in the implementation
 *
 *      @todo   Try and get rid of the Peek () API
 *
 *      @todo   WRITEUP THREAD SAFETY:
 *              Writeup in docs STRINGS THREADING SAFETY setioN (intenral hidden stuff fully threadsafe,
 *              but externally, envelope cannot be read/write or write/write at the same time). – document examples.
 *
 *      @todo   Add Ranged insert public envelope API, and add APPEND (not just operaotr+) API
 *
 *      @todo   Migrate most of the StringUtils stuff here like:
 *              > Contains- with CI optin
 *overload so can be string arg OR lambda!
 *              > Compare () - returns < less > more =0 for equal- with CI optin
 *              > Equals() - with CI optin
 *
 *
 *      @todo   Add Left()/Right()/Mid() funtions - like basic (simple, vaguely useful - especially 'Right'()).
 *
 *      @todo   Compare
 *          template    <typename TCHAR>
 *              basic_string<TCHAR> RTrim (const basic_string<TCHAR>& text)
 *                  {
 *                      std::locale loc1;   // default locale
 *                      const ctype<TCHAR>& ct = use_facet<ctype<TCHAR> >(loc1);
 *                      typename basic_string<TCHAR>::const_iterator i = text.end ();
 *                      for (; i != text.begin () and ct.is (ctype<TCHAR>::space, *(i-1)); --i)
 *                          ;
 *                      return basic_string<TCHAR> (text.begin (), i);
 *                  }
 *          with the TRIM() implementation I wrote here - in String. Not sure we want to use the local stuff? Maybe?
 *
 *      @todo   when we get Sequence<> ported (after) - we MUST add sequence-iterator to String class
 *              (will work beatifulyl with new stdc++ foreach() stuff).
 *
 *              (OR PERHAPS create new class Iterable<T> and make String subclass from that instead of Sequence?)?
 *
 *              (OR - perhaps BEST - make STRING a readonly class, and have it able to
 *                  Sequence<Character> AsCharacters() const;
 *                  and CTOR
 *                  String (Sequence<Character>);
 *              THEN the class can be fully readonly and use sequence stuff to modify?
 *              UNSURE - maybe just too conveinet to be able to modify (but still do sequnce converters?)
 *
 *      @todo   Redo implementation of String_StackLifetime - using high-performance algorithm described in the documentation.
 *
 *      @todo   Do String_stdwstring() – as impl optimized to return std::wstring() a lot – saving that impl internally.
 *              Do make this efficient, must have pur virtual method of String:::Rep which fills in a wstring* arg
 *              (what about ‘into no-malloc semantics – I guess taken care of perhaps by this? Maybe not… THINKOUT –
 *              but pretty sure we want some sort of String_stdwstring().
 *
 *      @todo   Handle Turkish toupper('i') problem. Maybe use ICU. Maybe add optional LOCALE parameter to routines where this matters.
 *              Maybe use per-thread global LOCALE settings. Discuss with KDJ.
 *              KDJ's BASIC SUGGESTION is - USE ICU and 'stand on their shoulders'.
 *
 *      @todo   Consider adding a new subtype of string - OPTIMIZAITON - which takes an ASCII argument (so can do less checking
 *              and be more compact??? Perhaps similarly for REP storing stuff as UTF8?
 *
 */






namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {

            constexpr size_t    kBadStringIndex   = wstring::npos;

            class   RegularExpression;

            /**
             *  The Stroika String class is an alternatve for the std::wstring class, which should be largely
             *  interoperable with code using wstring (there is wstring constructor and As<wstring>()
             *  methods).
             *
             *  The Stroika String class is conceptually a sequence of (UNICODE) Characters, and so there is
             *  no obvious way to map the Stroika String to a std::string. However, if you specify a codepage
             *  for conversion, or are converting to/from TString/TChar, there is builtin support for that.
             *
             *
             *  EOS Handling:
             *      The Stroika String class does support having embedded NUL-characters. It also supports
             *      returning wchar_t* strings which are NUL-terminated. But - Stroika generally does NOT
             *      maintain strings internally as NUL-terminated (generally). It may add a performance
             *      overhead when you call the c_str() method to force a NUL-character termination.
             *      See String::c_str ().
             *
             *      Also note that some subclasses of String (e.g. future String_stdwstring) may not support
             *      internal NUL-characters, if their underling implementation doesn't allow for that.
             */
            class   String {
            public:
                /**
                 * All the constructors are obvious, except to note that NUL-character ARE allowed in strings,
                 * except for the case of single char* argument constructors - which find the length based on
                 * the terminating NUL-character.
                 */
                String ();
                String (const char16_t* cString);
                String (const wchar_t* cString);
                String (const wchar_t* from, const wchar_t* to);
                String (const Character* from, const Character* to);
                String (const std::wstring& r);
                String (const String& from);
                String (const String && from);
                ~String ();

                nonvirtual  String& operator= (const String& newString);

            public:
                static  String  FromUTF8 (const char* from);
                static  String  FromUTF8 (const char* from, const char* to);
                static  String  FromUTF8 (const std::string& from);

            public:
                static  String  FromTString (const TChar* from);
                static  String  FromTString (const TString& from);

            public:
                nonvirtual  String& operator+= (Character appendage);
                nonvirtual  String& operator+= (const String& appendage);

            public:
                /**
                 *  Returns the number of characters in the String. Note that this may not be the same as bytes,
                 *  does not include NUL termination, and doesn't in any way respect NUL termination (meaning
                 *  a nul-character is allowed in a Stroika string.
                 */
                nonvirtual  size_t  GetLength () const;

            public:
                /**
                 * NOTE - when you increase the size of a string with SetLength() - the extra characters
                 * added are not initialized, and will have random values.
                 */
                nonvirtual  void    SetLength (size_t newLength);

            public:
                nonvirtual  bool    empty () const;

            public:
                nonvirtual  void    clear ();

            public:
                nonvirtual  Character   operator[] (size_t i) const;
                nonvirtual  void        SetCharAt (Character c, size_t i);

            public:
                // Note that it is legal, but pointless to pass in an empty string to insert
                nonvirtual  void        InsertAt (Character c, size_t at);
                nonvirtual  void        InsertAt (const String& s, size_t at);
                nonvirtual  void        InsertAt (const wchar_t* from, const wchar_t* to, size_t at);
                nonvirtual  void        InsertAt (const Character* from, const Character* to, size_t at);

            public:
                // Note that it is legal, but pointless to pass in an empty string to insert
                nonvirtual  void        Append (Character c);
                nonvirtual  void        Append (const String& s);
                nonvirtual  void        Append (const wchar_t* from, const wchar_t* to);
                nonvirtual  void        Append (const Character* from, const Character* to);

            public:
                /*
                 * Remove the characters start at 'index' - removing nCharsToRemove (defaults to 1).
                 * It is an error if this implies removing characters off the end of the string.
                 */
                nonvirtual  void        RemoveAt (size_t index, size_t nCharsToRemove = 1);

            public:
                /**
                 *  Remove the first occurence of Character 'c' from the string. Not an error if none
                 *  found.
                 *
                 *      @todo   CRAPPY API - probably delete or redo. See if ever used!!! RemoveAll or RemoveFirst() would be
                 *              better names (thogh all woudl be a change in functionality) - and maybe return a bool so
                 *              you can tell if it did anyhting?
                 */
                nonvirtual  void        Remove (Character c);

            public:
                /**
                 *  Produce a substring of this string, starting at from, and up to to
                 *  (require from <= to unless to == kBadStingIndex). If to is kBadStringIndex (default)
                 *  then return all the way to the end of the string.
                 *
                 *  *NB* This function treats the second argument differntly than String::substr () -
                 *  which respects the STL basic_string API. This function treats the second argument
                 *  as a 'to', STL substr() treats it as a count. This amounts to the same thing for the
                 *  very common cases of substr(N) - because second argument is defaulted, and,
                 *  substr (0, N) - because then the count and end are the same.
                 *
                 *  \req  ((from <= to) or (to == kBadStringIndex));
                 *  \req  ((to <= GetLength ()) or (to == kBadStringIndex));
                 *
                 *  @see substr
                 */
                nonvirtual  String      SubString (size_t from, size_t to = kBadStringIndex) const;

            public:
                /**
                 *  Returns true if the argument character or string is found anywhere inside this string.
                 *  This is equivilent to
                 *      return Match (".*" + X + L".*");    // If X had no characters which look like they are part of
                 *                                          // a regular expression
                 *
                 *  @see Match
                 */
                nonvirtual  bool    Contains (Character c, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  bool    Contains (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;

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
                nonvirtual  bool    StartsWith (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;

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
                nonvirtual  bool    EndsWith (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                 *  Apply the given regular expression return true if it matches this string. This only
                 *  returns true if the expression matches the ENTIRE string - all the way to the end.
                 *  @see FindEach() or @see Find - to find a set of things which match.
                 *
                 *  For example:
                 *      Assert (String (L"abc").Match (L"abc"));
                 *      Assert (not (String (L"abc").Match (L"bc")));
                 *      Assert (String (L"abc").Match (L".*bc"));
                 *      Assert (not String (L"abc").Match (L"b.*c"));
                 *
                 *  Details on the regular expression language/format can be found at:
                 *      http://en.wikipedia.org/wiki/C%2B%2B11#Regular_expressions
                 *
                 *  @see Contains
                 *  @see StartsWith
                 *  @see EndsWith
                 *  @see Find
                 *  @see FindEach
                 */
                nonvirtual  bool    Match (const RegularExpression& regEx) const;

            public:
                /**
                 *  Find returns the index of the first occurance of the given Character/substring argument in
                 *  this string. Find () always returns a valid string index, which is followed by the
                 *  given substring, or kBadStringIndex otherwise.
                 *
                 *  Find () can optionally be provided a 'startAt' offset to begin the search at.
                 *
                 *  And the overload taking a RegularExpression - returns BOTH the location where the match
                 *  is found, but the length of the match.
                 *
                 *  \req (startAt <= GetLength ());
                 *
                 *  @see FindEach ()
                 *  @see FindEachString ()
                 */
                nonvirtual  size_t                  Find (Character c, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  size_t                  Find (Character c, size_t startAt, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  size_t                  Find (const String& subString, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  size_t                  Find (const String& subString, size_t startAt, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  pair<size_t, size_t>    Find (const RegularExpression& regEx, size_t startAt = 0) const;

            public:
                /*
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
                 *  @see Find ()
                 *  @see FindEachString ()
                 */
                nonvirtual  vector<pair<size_t, size_t>>    FindEach (const RegularExpression& regEx) const;
                nonvirtual  vector<size_t>                  FindEach (const String& string2SearchFor, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                 *  Run Find (), starting at the beginning of the string and iterating through, capturing all the
                 *  occurrances of the given argument substring. Accumulate those results and return the actual
                 *  substrings.
                 *
                 *  *Design Note*:
                 *
                 *  There is no overload for FindEachString() taking a Character or SubString, because the results
                 *  wouldn't be useful. Their count might be, but the results would each be identical to the argument.
                 */
                nonvirtual  vector<String>  FindEachString (const RegularExpression& regEx) const;

            public:
                /**
                 * RFind (substring) returns the index of the last occurance of the given substring in
                 * this string. This function always returns a valid string index, which is followed by the
                 * given substring, or kBadStringIndex otherwise.
                 */
                nonvirtual  size_t  RFind (Character c) const;
                nonvirtual  size_t  RFind (const String& subString) const;

            public:
                /*
                 * Apply the given regular expression, with 'with' and replace each match. This doesn't
                 * modify this string, but returns the replacement string.

                 * CHECK - BUT HI HTINK WE DEFINE TO REPLACE ALL? OR MAKE PARAM?
                * See regex_replace () for definition of the regEx language
                *       TODO: GIVE EXAMPLES
                 */
                nonvirtual  String  ReplaceAll (const RegularExpression& regEx, const String& with, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  String  ReplaceAll (const String& string2SearchFor, const String& with, CompareOptions co = CompareOptions::eWithCase) const;

            public:
#if     !qCompilerAndStdLib_Supports_lambda_default_argument_WITH_closureCvtToFunctionPtrSupported
                inline  static  bool    DefaultTrimArg_ (Character c)               {       return c.IsWhitespace ();       }
#endif

#if     qCompilerAndStdLib_Supports_lambda_default_argument_WITH_closureCvtToFunctionPtrSupported
                /*
                 * String LTrim () scans the characters form the left to right, and applies the given
                 * 'shouldBeTrimmed' function (defaults to IsWhitespace). All such characters are removed,
                 * and the resulting string is returned. This does not modify the current string its
                 * applied to - just returns the trimmed string.
                 */
                nonvirtual  String  LTrim (bool (*shouldBeTrimmmed) (Character) = [](const Character& c) -> bool { return c.IsWhitespace (); }) const;
                /*
                 * String RTrim () scans the characters form the right to left, and applies the given
                 * 'shouldBeTrimmed' function (defaults to IsWhitespace). All such characters are removed,
                 * and the resulting string is returned. This does not modify the current string its
                 * applied to - just returns the trimmed string.
                 */
                nonvirtual  String  RTrim (bool (*shouldBeTrimmmed) (Character) = [](Character c) -> bool { return c.IsWhitespace (); }) const;
                /*
                 * String Trim () is locally equivilent to RTrim (shouldBeTrimmed).LTrim (shouldBeTrimmed).
                 */
                nonvirtual  String  Trim (bool (*shouldBeTrimmmed) (Character) = [](Character c) -> bool { return c.IsWhitespace (); }) const;
#else
                nonvirtual  String  LTrim (bool (*shouldBeTrimmmed) (Character) = DefaultTrimArg_) const;
                nonvirtual  String  RTrim (bool (*shouldBeTrimmmed) (Character) = DefaultTrimArg_) const;
                nonvirtual  String  Trim (bool (*shouldBeTrimmmed) (Character) = DefaultTrimArg_) const;
#endif

            public:
                /*
                 * Walk the entire string, and produce a new string consisting of all characters for which
                 * the predicate 'removeCharIf' returned false.
                 */
                nonvirtual  String  StripAll (bool (*removeCharIf) (Character)) const;

            public:
                /*
                 * Return a new string based on this string where each lower case characer is replaced by its
                 * upper case equivilent.
                 */
                nonvirtual  String  ToLowerCase () const;

                /*
                 * Return a new string based on this string where each lower case characer is replaced by its
                 * upper case equivilent.
                 */
                nonvirtual  String  ToUpperCase () const;

                /*
                 * Return true if the string contains zero non-whitespace characters.
                 */
                nonvirtual  bool    IsWhitespace () const;


            public:
                /*
                 *  CopyTo () copies the contents of this string to the target buffer.
                 *  CopyTo () does NOT nul-terminate the target buffer, but DOES assert that (bufTo-bufFrom)
                 *  is >= this->GetLength ()
                 */
                nonvirtual  void    CopyTo (Character* bufFrom, Character* bufTo) const;
                nonvirtual  void    CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const;


            public:
                /*
                 * Convert String losslessly into a standard C++ type (right now just <wstring>,
                 * <const wchar_t*>,<const Character*> supported)
                 *
                 * For the special cases of <T=const wchar_t*>, and <T=const Character*>, the returned
                 * result is NOT NUL-terminated.
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
                template    <typename   T>
                nonvirtual  void    As (T* into) const;

            public:
                /*
                 * Convert String losslessly into a standard C++ type (right now just <string> supported).
                 * Note - template param is optional.
                 */
                template    <typename   T>
                nonvirtual  T   AsUTF8 () const;
                template    <typename   T>
                nonvirtual  void    AsUTF8 (T* into) const;
                nonvirtual  std::string AsUTF8 () const;
                nonvirtual  void        AsUTF8 (std::string* into) const;

            public:
                /*
                 */
                nonvirtual  TString AsTString () const;
                nonvirtual  void    AsTString (TString* into) const;

            public:
                /*
                 * Convert String losslessly into a standard C++ type (right now just <string> supported).
                 * The source string MUST be valid ascii characters (asserted)
                 */
                template    <typename   T>
                nonvirtual  T   AsASCII () const;
                template    <typename   T>
                nonvirtual  void    AsASCII (T* into) const;
                nonvirtual  std::string AsASCII () const;
                nonvirtual  void        AsASCII (std::string* into) const;


            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 */
                nonvirtual  int Compare (const String& rhs, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  int Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  int Compare (const wchar_t* rhsStart, const wchar_t* rhsEnd, CompareOptions co = CompareOptions::eWithCase) const;


                // StdC++ wstring aliases [there maybe a namespace trick in new c++ to do this without
                // inlines - like new '=' guy???
            public:
                nonvirtual  size_t          size () const;
                nonvirtual  size_t          length () const;

                // As with STL, the return value of the data () function should NOT be assumed to be
                // NUL-terminated
                //
                // The lifetime of the pointer returned is gauranteed until the next call to this String
                // envelope class (that is if other reps change, or are acceessed this data will not
                // be modified)
                nonvirtual  const wchar_t*  data () const;

                /**
                 * This will always return a value which is NUL-terminated. Note that Stroika generally
                 *  does NOT keep strings in NUL-terminated form, so this could be a costly function,
                 * requiring a copy of the data.
                 *
                 * The lifetime of the pointer returned is gauranteed until the next call to this String
                 * envelope class (that is if other reps change, or are acceessed this data will not be modified)
                 * Note also that Stroika strings ALLOW internal nul bytes, so though the Stroika string
                 * class NUL-terminates, it does noting to prevent already existng NUL bytes from causing
                 * confusion elsewhere.
                 */
                nonvirtual  const wchar_t*  c_str () const;

                // need more overloads
                nonvirtual  size_t find (wchar_t c) const;
                // need more overloads
                nonvirtual  size_t rfind (wchar_t c) const;

                // mimic (much of - need more overloads) STL variant
                nonvirtual  void erase (size_t from = 0, size_t count = kBadStringIndex);

                nonvirtual  void    push_back (wchar_t c);
                nonvirtual  void    push_back (Character c);

                /**
                 *  Compatable with STL::basic_string::subtr() - which interprets second argument as count. Not the same
                 *  as Stroika::String::SubString (where the second argument is a 'to')
                 *
                 *  @see SubString
                 */
                nonvirtual  String      substr (size_t from, size_t count = kBadStringIndex) const;

            protected:
                class   _IRep;
                typedef     shared_ptr<_IRep>   _SharedPtrIRep;
            protected:
                static  _SharedPtrIRep   _Clone (const _IRep& rep);

            protected:
                struct  _Rep_Cloner {
                    inline  static  _SharedPtrIRep   Copy (const _IRep& t) {
                        return String::_Clone (t);
                    }
                };

            protected:
                typedef Memory::SharedByValue<Memory::SharedByValue_Traits<_IRep, _SharedPtrIRep, _Rep_Cloner>>  _SharedRepByValuePtr;
                _SharedRepByValuePtr _fRep;

            protected:
                /**
                 * rep MUST be not-null
                 */
                String (const _SharedRepByValuePtr::shared_ptr_type& rep);
                String (const _SharedRepByValuePtr::shared_ptr_type && rep);

            private:
                /*
                 * These are made friends so they can peek at the shared part, as an optimization.
                 */
                friend  bool    operator== (const String& lhs, const String& rhs);
                friend  bool    operator!= (const String& lhs, const String& rhs);
                friend  bool    operator< (const String& lhs, const String& rhs);
                friend  bool    operator<= (const String& lhs, const String& rhs);
                friend  bool    operator> (const String& lhs, const String& rhs);
                friend  bool    operator>= (const String& lhs, const String& rhs);

                // constructs a StringRep_Catenate
                friend  String  operator+ (const String& lhs, const String& rhs);
            };

            template    <>
            void    String::As (wstring* into) const;
            template    <>
            wstring String::As () const;
            template    <>
            const wchar_t*  String::As () const;
            template    <>
            const Character*    String::As () const;

            template    <>
            void    String::AsUTF8 (string* into) const;
            template    <>
            string  String::AsUTF8 () const;

            template    <>
            void    String::AsASCII (string* into) const;
            template    <>
            string  String::AsASCII () const;



            /**
             * Protected helper Rep class.
             */
            class   String::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                typedef     String::_SharedPtrIRep  _SharedPtrIRep;

            public:

                virtual _SharedPtrIRep      Clone () const                          = 0;

                virtual size_t              GetLength () const                      = 0;
                virtual void                RemoveAll ()                            = 0;

                virtual Character           GetAt (size_t index) const              = 0;
                virtual void                SetAt (Character item, size_t index)    = 0;
                // This rep is NEVER called with nullptr src/end nor start==end
                virtual void                InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) = 0;
                virtual void                RemoveAt (size_t index, size_t nCharsToRemove)  = 0;

                virtual void                SetLength (size_t newLength)            = 0;

                // return nullptr if its not already NUL-terminated
                virtual const wchar_t*      c_str_peek () const noexcept            = 0;

                // change rep so its NUL-termainted
                virtual const wchar_t*      c_str_change ()                         = 0;

                virtual const Character*    Peek () const                           = 0;

                virtual pair<const Character*, const Character*> GetData () const   = 0;

                virtual int                 Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co) const   =   0;

            public:
                /*
                 *  CopyTo () copies the contents of this string to the target buffer.
                 *  CopyTo () does NOT nul-terminate the target buffer, but DOES assert that (bufTo-bufFrom) is >= this->GetLength ()
                 */
                nonvirtual  void    CopyTo (Character* bufFrom, Character* bufTo) const;
                nonvirtual  void    CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const;
            };


            String  operator+ (const String& lhs, const String& rhs);

            bool    operator== (const String& lhs, const String& rhs);
            bool    operator== (const wchar_t* lhs, const String& rhs);
            bool    operator== (const String& lhs, const wchar_t* rhs);
            bool    operator!= (const String& lhs, const String& rhs);
            bool    operator!= (const wchar_t* lhs, const String& rhs);
            bool    operator!= (const String& lhs, const wchar_t* rhs);
            bool    operator< (const String& lhs, const String& rhs);
            bool    operator< (const wchar_t* lhs, const String& rhs);
            bool    operator< (const String& lhs, const wchar_t* rhs);
            bool    operator<= (const String& lhs, const String& rhs);
            bool    operator<= (const wchar_t* lhs, const String& rhs);
            bool    operator<= (const String& lhs, const wchar_t* rhs);
            bool    operator> (const String& lhs, const String& rhs);
            bool    operator> (const wchar_t* lhs, const String& rhs);
            bool    operator> (const String& lhs, const wchar_t* rhs);
            bool    operator>= (const String& lhs, const String& rhs);
            bool    operator>= (const wchar_t* lhs, const String& rhs);
            bool    operator>= (const String& lhs, const wchar_t* rhs);



            /**
             * @brief   Return true of the two argument strings are equal. This is equivilent to
             *              lhs.compare (rhs, co);
             */
            bool Equals (const String& lhs, const String& rhs, CompareOptions co = CompareOptions::eWithCase);





            /**
             *  String_BufferedArray is a kind of string which maintains extra buffer space, and
             *  is more efficient if you are going to resize your string.
             */
            class   String_BufferedArray  : public String {
            public:
                /*
                 * Note for the meaning of the reserve parameter, see String_BufferedArray::reserve()
                 */
                String_BufferedArray ();
                String_BufferedArray (size_t reserve);
                explicit String_BufferedArray (const wchar_t* cString);
                explicit String_BufferedArray (const wchar_t* cString, size_t reserve);
                explicit String_BufferedArray (const wstring& str);
                explicit String_BufferedArray (const wstring& str, size_t reserve);
                explicit String_BufferedArray (const String& from);
                explicit String_BufferedArray (const String& from, size_t reserve);
                String_BufferedArray (const String_BufferedArray& s);

                String_BufferedArray& operator= (const String_BufferedArray& s);

            public:
                // This returns the number of characters of space available in the buffer
                // (without doing memory allocations)
                nonvirtual  size_t  capacity () const;

            public:
                // Reserve the given number of characters of space. N can be any size, and is only a hint.
                // A value of n < GetLength () will be ignored.
                nonvirtual  void    reserve (size_t n);
            };







            /**
             *      String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly is a subtype of string you can
             * use to construct a String object, so long as the memory pointed to in the argument has a
             *      o   FULL APPLICATION LIFETIME,
             *      o   the member referenced never changes - is READONLY.
             *
             *      String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly will NOT change the memory
             * referenced in the CTOR.
             *
             *      Strings constructed with this String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly
             * maybe treated like normal strings - passed anywhere, and even modified via the String APIs.
             *
             *  For example
             *      String  tmp1    =   L"FRED";
             *      String  tmp2    =   String (L"FRED");
             *      String  tmp3    =   String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (L"FRED");
             *
             *      extern String saved;
             *      inline  String  F(String x)         { saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
             *      F(tmp1);
             *      F(tmp2);
             *      F(tmp3);
             *
             *      These ALL do essentially the same thing, and are all equally safe. The 'tmp3' implementation
             * maybe slightly more efficent, but all are equally safe.
             *
             */
            class   String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly : public String {
            public:
                explicit String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const wchar_t* cString);
                String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s);

                String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s);

            private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
            public:
#endif
                class   MyRep_;
            };


            /**
             *      String_Constant can safely be used to initilaize constant C-strings as Stroika strings,
             * with a minimum of cost.
             */
            typedef String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly String_Constant;





            /**
             *      String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite is a subtype of string you can
             * use to construct a String object, so long as the memory pointed to in the argument has a
             *      o   FULL APPLICATION LIFETIME,
             *      o   and never changes value through that pointer
             *
             *      Note that the memory passed in MUST BE MODIFIABLE (READ/WRITE) - as
             * String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite MAY modify the data in the
             * pointer during the objects lifetime.
             *
             *      Strings constructed with this String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite
             * maybe treated like normal strings - passed anywhere, and even modified via the String APIs.
             * However, the underlying implemenation may cache the argument const wchar_t* cString
             * indefinitely, and re-use it as needed, so only call this String constructor with a block
             * of read-only, never changing memory, and then - only as a performance optimization.
             *
             *  For example
             *      String  tmp1    =   L"FRED";
             *      String  tmp2    =   String (L"FRED");
             *      static  wchar_t buf[1024] = { L"FRED" };
             *      String  tmp3    =   String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (buf);
             *
             *      extern String saved;
             *      inline  String  F(String x)         { saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
             *      F(tmp1);
             *      F(tmp2);
             *      F(tmp3);
             *
             *      These ALL do essentially the same thing, and are all equally safe. The 'tmp3' implementation
             * maybe slightly more efficent, but all are equally safe.
             *
             */
            class   String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite : public String {
            public:
                explicit String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (wchar_t* cString);
                String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s);

                String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s);

            private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
            public:
#endif
                class   MyRep_;
            };





            /*
            *
            *   Design Overview:
            *
            *       This class looks and acts like a regular String object, but with the performance advantage
            *   that it requires no (significant) free-store allocation. It allocates a 'rep' object from
            *   block-allocation, and re-uses its argument pointers for actual string character storage.
            *
            *       Also important, it avoids having todo any copying of the original string.
            *
            *       It can avoid these costs under MANY - but not ALL circumstances. This underlying String
            *   object may remain highly efficeint (working off stack memory) only so long as its original
            *   String_ExternalMemoryOwnership_StackLifetime_ReadOnly exsts. Once that goes out of scope
            *   the underlying StringRep must be 'morphed' effectively into a regular string-rep (if there
            *   remain any references.
            *
            *       Also - SOME APIS (still TBD, but perhaps including c_str()) - will force that morphing
            *   /copying.
            *
            *       This can STILL be a HUGE performance benefit. Consider a shim API between Xerces and
            *   Stroika - for SAX - where lots of strings are efficiently passed by Xerces - and are forwarded
            *   to Stroika 'SAX' APIs. But consider a usage, where several of the strings are never used.
            *
            *       The shim would be copying/converting these strings, all for no purpose, since they never
            *   got used. This cost can be almost totally eliminated.
            *
            *       Plus - many - perhaps even most String API methods can be applied to these ligher cost
            *   strings in that 'SAX Callback' scenario without ever constructing high-cost String objects.
            *
            *       But - if anyone ever does allocate one of those objects - no biggie. It just gets morphed
            *   and the cost paid then.
            *
            *       THATS THE PLAN ANYHOW....
            *

            **
            ** SERIOUS - NOT sure what todo about stuff like c_str() - as doc says below - no obvious limitation on lifetime! I GUESS we must simply
            ** force such APIs to 'breakreferences' - as the note below says. THAT does appear to fix the problem - but at a cost - maybe too huge cost?
            *
            *
            *     o   Seriously reconsider design / semantics of String_ExternalMemoryOwnership_StackLifetime_ReadOnly/String_ExternalMemoryOwnership_StackLifetime_ReadWrite classes.
            *         There maybe a serious / hopeless bug having todo with threads and c_str(). Suppose in one thread we do:
            *             void    f()
            *                 {
            *                     wchar_t buf[1024] = L"fred";
            *                     String_ExternalMemoryOwnership_StackLifetime_ReadOnly tmp (buf);
            *
            *                     // Then call some function F() with tmp - where F() passes the value (as a String) to another thread, and that other thread runs for a very long time with the string - just READING
            *                     // the string - never MODIFYING it.
            *                     //
            *                     // THEN - because of second thread owning that string - the refCount is bumped up. And suppose that other string calls s.c_str() - and is ever so briefly using the s.c_str() results - say
            *                     // in a printf() call.
            *                     //
            *                     // Now when String_ExternalMemoryOwnership_StackLifetime_ReadOnly goes out of scope - and it notices its refCount != 0, it must somehow MORPH the underlying representation into
            *                     // something SAFE (copy all the strings). But thats tricky - ESPECIALLY in light of threads and someone else doing a c_str().
            *                     //
            *                     // I guess its not IMPOSSIBLE to fix all this. You could
            *                     //          (o)     On c_str() calls - at least for this class rep - maybe always - BREAKREFERENCES() - so the lifetime of your ptr is garuanteed friendly
            *                     //          (o)     Put in enuf flags and CriticalSection code - so on String_ExternalMemoryOwnership_StackLifetime_ReadOnly () - we block until we can
            *                     //                  Copy all the data safely and set a flag saying to free it at the end - as a regular string - not a String_ExternalMemoryOwnership_StackLifetime_ReadOnly::Rep (would be nice
            *                     //                  to transform the rep object but thats trickier).
            *                 }

            *
             */

            /*
             * In case of trouble in stack lifetime string make bundle copy object. Store it in rep. Then next call from envelope triggers exception which forces type morph call from envelope!

            Think that fixes most trouble except that aLl enveoes methods now need to handle exceotion
            */



            /*
             *  String_ExternalMemoryOwnership_StackLifetime_ReadOnly is a subtype of String you can use to construct a String object, so long as the memory pointed to
             * in the argument has a
             *      o   Greater lifetime than the String_ExternalMemoryOwnership_StackLifetime_ReadOnly envelope class
             *      o   and buffer data never changes value externally to this String represenation
             *
            ///REVIEW - PRETTY SURE THIS IS WRONG!!!! - UNSAFE - READONLY SHOULD mean pointer passed in is CONST - so memory may NOT be modified in this case -- LGP 2012-03-28
            /// DOBLE CHECK NO ASSIMPTIONS BELOW - WRONG - LINE NOT ASSUMED ANYWHERE
             *  Note that the memory passed in must be READ/WRITE - and may be modified by the String_ExternalMemoryOwnership_StackLifetime_ReadOnly ()!
             *
             *  Strings constructed with this String_ExternalMemoryOwnership_StackLifetime_ReadOnly maybe treated like normal strings - passed anywhere, and even modified via the
             *  String APIs. However, the underlying implemenation may cache the argument const wchar_t* cString for as long as the lifetime of the envelope class,
             *  and re-use it as needed during this time, so only call this String constructor with great care, and then - only as a performance optimization.
             *
             *  This particular form of String wrapper CAN be a great performance optimization when a C-string buffer is presented and one must
             *  call a 'String' based API. The argument C-string will be used to handle all the Stroika-String operations, and never modified, and the
             *  association will be broken when the String_ExternalMemoryOwnership_StackLifetime_ReadOnly goes out of scope.
             *
             *  This means its EVEN safe to use in cases where the String object might get assigned to long-lived String variables (the internal data will be
             *  copied in that case).
             *
             *  For example
             *
             *      extern String saved;
             *      inline  String  F(String x)         { saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
             *
             *
             *      void f (const wchar_t* cs)
             *          {
             *              F(L"FRED";);
             *              F(String (L"FRED"));
             *              F(String_ExternalMemoryOwnership_StackLifetime_ReadOnly (cs));
             *          }
             *
             *  These ALL do essentially the same thing, and are all equally safe. The third call to F () with String_ExternalMemoryOwnership_StackLifetime_ReadOnly()
             *  based memory maybe more efficient than the previous two, because the string pointed to be 'cs' never needs to be copied (now malloc/copy needed).
             *
             *      <<TODO: not sure we have all the CTOR/op= stuff done correctly for this class - must rethink - but only needed to rethink when we do
             *          real optimized implemenation >>
             */
            class   String_ExternalMemoryOwnership_StackLifetime_ReadOnly : public String {
            public:
                explicit String_ExternalMemoryOwnership_StackLifetime_ReadOnly (const wchar_t* cString);
// DOCUMENT THESE NEW EXTRA CTORS!!! NYI
                explicit String_ExternalMemoryOwnership_StackLifetime_ReadOnly (const wchar_t* start, const wchar_t* end);
            };




            /**
             *  String_ExternalMemoryOwnership_StackLifetime_ReadWrite is a subtype of string you can use to
             *  construct a String object, so long as the memory pointed to min the argument has a
             *      o   Greater lifetime than the String_ExternalMemoryOwnership_StackLifetime_ReadWrite envelope class
             *      o   and buffer data never changes value externally to this String represenation (but maybe changed by
             *          the String_ExternalMemoryOwnership_StackLifetime_ReadWrite implementation)
             *
             *  Note that the memory passed in must be READ/WRITE - and may be modified by the
             *  String_ExternalMemoryOwnership_StackLifetime_ReadWrite ()!
             *
             *  Strings constructed with this String_ExternalMemoryOwnership_StackLifetime_ReadWrite maybe treated
             *  like normal strings - passed anywhere, and even modified via the
             *  String APIs. However, the underlying implemenation may cache the argument 'wchar_t* cString' for as long as the lifetime of the envelope class,
             *  and re-use it as needed during this time, so only call this String constructor with great care, and then - only as a performance optimization.
             *
             *  This particular form of String wrapper CAN be a great performance optimization when a C-string buffer is presented and one must
             *  call a 'String' based API. The argument C-string will be used to handle all the Stroika-String operations, and never modified, and the
             *  association will be broken when the String_ExternalMemoryOwnership_StackLifetime_ReadWrite goes out of scope.
             *
             *  This means its EVEN safe to use in cases where the String object might get assigned to long-lived String variables (the internal data will be
             *  copied in that case).
             *
             *  For example
             *
             *      extern String saved;
             *      inline  String  F(String x)         { saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
             *
             *
             *      void f ()
             *          {
             *              char    cs[1024] = L"FRED";
             *              F(L"FRED";);
             *              F(String (L"FRED"));
             *              F(String_ExternalMemoryOwnership_StackLifetime_ReadWrite (cs));
             *          }
             *
             *  These ALL do essentially the same thing, and are all equally safe. The third call to F () with String_ExternalMemoryOwnership_StackLifetime_ReadWrite()
             *  based memory maybe more efficient than the previous two, because the string pointed to be 'cs' never needs to be copied (until its changed inside F()).
             *
             *      <<TODO: not sure we have all the CTOR/op= stuff done correctly for this class - must rethink - but only needed to rethink when we do
             *          real optimized implemenation >>
             */
            class   String_ExternalMemoryOwnership_StackLifetime_ReadWrite : public String {
            public:
                explicit String_ExternalMemoryOwnership_StackLifetime_ReadWrite (wchar_t* cString);
// DOCUMENT THESE NEW EXTRA CTORS!!! NYI
                explicit String_ExternalMemoryOwnership_StackLifetime_ReadWrite (wchar_t* start, wchar_t* end);
//TODO: start/end defines range of string, and bufend if 'extra bytes' usable after end - enen though not orignally part of string
                explicit String_ExternalMemoryOwnership_StackLifetime_ReadWrite (wchar_t* start, wchar_t* end, wchar_t* bufEnd);
            };


#if     0
            /**
             *
             *  NOT YET IMPLEMETNED - EVEN IN FAKE FORM - BECAUSE I"M NOT SURE OF SEMANTICS YET!
             *
             *  String_Common is a subtype of string you can use to construct a String object freely. It has no semantics requirements. However, it SHOULD only
             *  be used for strings which are commonly used, and where you wish to save space. The implementation will keep the memory for String_Common strings
             *  allocated permanently - for the lifetime of the application, and will take potentially extra time looking for the given string.
             *
             *  We MAY handle this like the HealthFrame RFLLib ATOM class - where we store the string in a hashtable (or map), and do quick lookup of associated index, and
             *  also store in a table (intead of vector of strings, use a big buffer we APPEND to, and whose index is the value of the stored rep. Then doing a PEEK()
             *  is trivial and efficient.
             *
             *      <<TODO: OPTIMIZATION not really implemented yet. But it can still freeely be used safely.>>
             */
            class   String_Common : public String {
            public:
                explicit String_Common (const String& from);
            };
#endif


#if     0
            /**
             *  NOT YET IMPLEMETNED
             *
             *  String_stdwstring is completely compatible with any other String implementation, except that it represents things
             *  internally using the stdC++ wstring class. The principle advantage of this is that converting TO wstrings
             *  is much more efficient.
             *
             *      (AS OF YET UNCLEAR IF/HOW WE CAN SUPPORT MANIPULATIONS OF A wstring* or wstring& alias to the String rep's owned copy.
             *      probably won't be allowed, but it would be helpful to some applicaitons if we could)
             */
            class   String_stdwstring : public String {
            public:
                explicit String_stdwstring (const String& from);
            };
#endif



            /**
             *  This can be referenced in your ModuleInitializer<> to force correct inter-module construction order.
             */
            Execution::ModuleDependency MakeModuleDependency_String ();

        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "String.inl"


#endif  /*_Stroika_Foundation_Characters_String_h_*/
