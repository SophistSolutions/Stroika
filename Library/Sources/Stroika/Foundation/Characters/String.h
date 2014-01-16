/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_h_
#define _Stroika_Foundation_Characters_String_h_    1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "../Execution/ModuleInit.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"
#include    "SDKString.h"

#include    "Character.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
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
 *              o   String_wstring
 *
 *      Possible future additions
 *              o   String_Common
 *              o   String_AsciiOnlyOptimized
 *
 * TODO:
 *
 *      @todo   The string REPS are (I think) fully threadsafe, but I dont think the String envelope class is.
 *              We need to be much more careful about the Envelope class thread safety.
 *
 *              For example,
 *
 *              String  String::StripAll (bool (*removeCharIf) (Character)) const
 *              {
 *                  ...
 *                  size_t  n   =   GetLength ();
 *                  for (size_t i = 0; i < n; ++i) {
 *                      Character   c   =   operator[] (i);
 *              ...
 *              What if someone changed the length in the middle of the loop!
 *
 *      @todo   Annotate basic string aliases as (std::basic_string alias - as below). At least try and think
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
 *      @todo   Add FromISOLatin1()
 *
 *      @todo   Performance optimize FromUTF8, and AsUtf8() functions to not go through intermediate string
 *              objects!
 *
 *      @todo   Redo SetLength() API, so caller must specify fill-character.
 *              Or maybe better yet - replace with two APIs
 *                  GrowToLength(n, fillChar)
 *                  ShriunkToLength(n) - AKA Qt::QString::truncate().
 *              Not an error if nothing todo (even if already longer/shorter).
 *
 *      @todo   Add #include of Comparer and template specialize Comparer for String??? Maybe
 *              Maybe not needed. Or maybe can avoid the #include and just do template specailizaiton?
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
 *                  Consider adding a new subtype of string - OPTIMIZAITON - which takes an ASCII argument (so can do less checking
 *                  and be more compact??? Perhaps similarly for REP storing stuff as UTF8?
 *
 *                  CLOSELY RELATED - MAYBE DO part of above (no check CTOR except assert).
 *
 *      @todo   We want String : public Iterable<Character>. Verify easy and has no overhead!
 *              Document we chose NOT to have String : Sequence<Character> - because of issue of assignemtn to
 *              members? Maybe not good enough reason? But would muck up API more. Maybe revisit that?
 *
 *              OBSOLETE RELATED TODO
 *                  Somehow make it easy to convert String to an Iterable<Character> - and perhaps even Sequence<Character>
 *                  Should be able to do for (Character c : s) {...}.
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
 *      @todo   CLEANUP Tokenize API, AND VERY IMPROTANMTLY - either DOCUMENT or indirect to Tokenize() API.
 *              MANY people will expect a String.Split() function to exist. This is what the existing TOKENIZE() API does but thats
 *              not obvious!!!! MUST BE SUPER CLEARLY DOCUMENTED.
 *              Be sure docs for TOKENIZE are clear this is not a FLEX replacement - but just a very simple 'split' like functionaliuty.
 *              not totally clear what name is best (split or tokenize()).
 *
 *              Closely related to my existing FIND () API. Maybe this is just a comment?
 *
 *              But also review:
 *                  http://qt-project.org/doc/qt-5.0/qtcore/qstring.html#split
 *
 *                  especially:
 *                      QString line = "forename\tmiddlename  surname \t \t phone";
 *                      QRegularExpression sep("\\s+");
 *                      str = line.section(sep, 2, 2); // str == "surname"
 *                      str = line.section(sep, -3, -2); // str == "middlename  surname"
 *              Make sure our FIND is at least this simple, and maybe diff between find and split is FIND the regular expression names the things
 *              looked for and SPLIT() uses regexp to name the separators?
 *
 *              Add something like the above to the String String demo app (when it exists)
 *
 *      @todo   MAYBE also add ReplaceOne() function (we have ReplaceAll() now).
 *
 *      @todo   At this stage - for our one trivial test - performance is now about 5% faster than
 *              visual studio.net 2010, but
 *              about a factor of 2 SLOWER than GCC (as of 2011-12-04).
 *
 *              I SUSPECT the next big change to address this will be && MOVE OPERATOR support.
 *
 *      @todo   Fix const   Memory::SharedByValue<String::String::Rep>  String::kEmptyStringRep_ (new String_CharArray::MyRep_ (nullptr, 0), &String::Clone_);
 *              to properly handle cross-module startup (not safe as is - probably use ModuleInit<> stuff. OR use static intit PTR and assure its fixed
 *              just in CPP file
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
 *      @todo   Try and get rid of the Peek () API
 *
 *              OR better document why its OK (unexpected throw stuff may make it OK).
 *
 *              Maybe also add GetRepSupportedFeatures() API? Maybe not worth it. If no - dumcument why someplace!
 *
 *      @todo   WRITEUP THREAD SAFETY:
 *              Writeup in docs STRINGS THREADING SAFETY setioN (intenral hidden stuff fully threadsafe,
 *              but externally, envelope cannot be read/write or write/write at the same time). � document examples.
 *
 *      @todo   Add Ranged insert public envelope API, and add APPEND (not just operaotr+) API. See/maybe use new
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
 *          with the TRIM() implementation I wrote here - in String. Not sure we want to use the local stuff? Maybe?
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
 *              We MAY handle this like the HealthFrame RFLLib ATOM class - where we store the string in
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
 *              [RETHINK - See String_AsciiOnlyOptimized - and probably dont do this]
 *
 *      @todo   Handle Turkish toupper('i') problem. Maybe use ICU. Maybe add optional LOCALE parameter to routines where this matters.
 *              Maybe use per-thread global LOCALE settings. Discuss with KDJ.
 *              KDJ's BASIC SUGGESTION is - USE ICU and 'stand on their shoulders'.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            class   RegularExpression;


            /**
             *  The Stroika String class is an alternatve for the std::wstring class, which should be largely
             *  interoperable with code using wstring (there is wstring constructor and As<wstring>()
             *  methods).
             *
             *  The Stroika String class is conceptually a sequence of (UNICODE) Characters, and so there is
             *  no obvious way to map the Stroika String to a std::string. However, if you specify a codepage
             *  for conversion, or are converting to/from SDKString/SDKChar, there is builtin support for that.
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
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *      @see   Concrete::String_BufferedArray
             *      @see   Concrete::String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly     (aka String_Constant)
             *      @see   Concrete::String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite
             *      @see   Concrete::String_ExternalMemoryOwnership_StackLifetime_ReadOnly
             *      @see   Concrete::String_ExternalMemoryOwnership_StackLifetime_ReadWrite
             *      @see   Concrete::String_Common
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
             *      enable_shared_from_this() would add costs (at least size) even when we dont use String iteration
             *      (which we didnt even impelement for a couple years, so may not be that critical).
             *
             *      For now - stick to simple impl - of just copy on start of iteration.
             *          -- LGP 2013-12-17
             *
             */
            class   String : public Traversal::Iterable<Character> {
            private:
                using   inherited   =   Iterable<Character>;

            public:
#if     !qCompilerAndStdLib_constexpr_StaticDataMember_Buggy
                static  constexpr size_t    kBadIndex   = wstring::npos;
#else
                DEFINE_CONSTEXPR_CONSTANT (size_t, kBadIndex, -1);
#endif

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
                String (String&& from);
                ~String ();

            protected:
                class   _IRep;

            protected:
                typedef Iterable<Character>::_SharedPtrIRep _IterableSharedPtrIRep;

            protected:
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            protected:
                /**
                 * rep MUST be not-null
                 */
                String (const _SharedPtrIRep& rep);
                String (_SharedPtrIRep&& rep);

            public:
                nonvirtual  String& operator= (const String& newString);

            public:
                /**
                 *  Create a String object from a 'char-based' utf-8 encoded string.
                 */
                static  String  FromUTF8 (const char* from);
                static  String  FromUTF8 (const char* from, const char* to);
                static  String  FromUTF8 (const std::string& from);

            public:
                /**
                 *  Create a String object from a 'SDKChar' (os-setting - current code page) encoded string.
                 *  See @SDKChar
                 *  See @SDKString
                 */
                static  String  FromSDKString (const SDKChar* from);
                static  String  FromSDKString (const SDKChar* from, const SDKChar* to);
                static  String  FromSDKString (const SDKString& from);

            public:
                /**
                 *  Create a String object from a 'char-based' (os-setting - current code page) encoded string.
                 */
                static  String  FromNarrowSDKString (const char* from);
                static  String  FromNarrowSDKString (const string& from);

            public:
                /**
                 *  Create a String object from ascii text. This function requires that its arguments all ascii (no high-bit set)
                 */
                static  String  FromAscii (const char* from);
                static  String  FromAscii (const string& from);

            public:
                nonvirtual  String& operator+= (Character appendage);
                nonvirtual  String& operator+= (const String& appendage);

            public:
                nonvirtual  String  operator+ (const String& rhs) const;

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
                /**
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 */
                nonvirtual  Character   operator[] (size_t i) const;
                nonvirtual  void        SetCharAt (Character c, size_t i);

            public:
                /**
                 *  Note that it is legal, but pointless to pass in an empty string to insert
                 */
                nonvirtual  void        InsertAt (Character c, size_t at);
                nonvirtual  void        InsertAt (const String& s, size_t at);
                nonvirtual  void        InsertAt (const wchar_t* from, const wchar_t* to, size_t at);
                nonvirtual  void        InsertAt (const Character* from, const Character* to, size_t at);

            public:
                /**
                 *  Note that it is legal, but pointless to pass in an empty string to insert
                 */
                nonvirtual  void        Append (Character c);
                nonvirtual  void        Append (const String& s);
                nonvirtual  void        Append (const wchar_t* from, const wchar_t* to);
                nonvirtual  void        Append (const Character* from, const Character* to);

            public:
                /**
                 * Remove the characters at 'charAt' (RemoveAt/1) or between 'from' and 'to'.
                 *
                 * It is an error if this implies removing characters off the end of the string.
                 *
                 *  \req (charAt < GetLength ())
                 *  \req (from <= to)
                 *  \req (to <= GetLength ())
                 */
                nonvirtual  void        RemoveAt (size_t charAt);
                nonvirtual  void        RemoveAt (size_t from, size_t to);

            public:
                /**
                 *  Remove the first occurence of Character 'c' from the string. Not an error if none
                 *  found.
                 */
                nonvirtual  void        Remove (Character c);

            public:
                /**
                 *  Produce a substring of this string, starting at from, and up to to
                 *  (require from <= to unless to == kBadStingIndex). If to is kBadIndex (default)
                 *  then return all the way to the end of the string.
                 *
                 *  *NB* This function treats the second argument differntly than String::substr () -
                 *  which respects the STL basic_string API. This function treats the second argument
                 *  as a 'to', STL substr() treats it as a count. This amounts to the same thing for the
                 *  very common cases of substr(N) - because second argument is defaulted, and,
                 *  substr (0, N) - because then the count and end are the same.
                 *
                 *  \req  ((from <= to) or (to == kBadIndex));
                 *  \req  ((to <= GetLength ()) or (to == kBadIndex));
                 *
                 *  @see substr
                 */
                nonvirtual  String      SubString (size_t from, size_t to = kBadIndex) const;

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
                 *  given substring, or kBadIndex otherwise.
                 *
                 *  Find () can optionally be provided a 'startAt' offset to begin the search at.
                 *
                 *  And the overload taking a RegularExpression - returns BOTH the location where the match
                 *  is found, but the length of the match.
                 *
                 *  Note - for the special case of Find(empty-string) - the return value is 0 if this string
                 *  is non-empty, and kBadIndex if this string was empty.
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
                 * given substring, or kBadIndex otherwise.
                 */
                nonvirtual  size_t  RFind (Character c) const;
                nonvirtual  size_t  RFind (const String& subString) const;

            public:
                /*
                 * Apply the given regular expression, with 'with' and replace each match. This doesn't
                 * modify this string, but returns the replacement string.

                 * CHECK - BUT HI HTINK WE DEFINE TO REPLACE ALL? OR MAKE PARAM?
                * See regex_replace () for definition of the regEx language
                *
                *   Require (not string2SearchFor.empty ());
                *       TODO: GIVE EXAMPLES
                *
                * Note - it IS legal to have with contain the original search for string, or even to have it 'created' as part of where it gets
                * inserted. The implementation will only replace those that pre-existed.
                 */
                nonvirtual  String  ReplaceAll (const RegularExpression& regEx, const String& with, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  String  ReplaceAll (const String& string2SearchFor, const String& with, CompareOptions co = CompareOptions::eWithCase) const;

            public:
#if     qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
                inline  static  bool    DefaultTrimArg_ (Character c)               {       return c.IsWhitespace ();       }
#endif

#if     !qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
                /*
                 * String LTrim () scans the characters form the left to right, and applies the given
                 * 'shouldBeTrimmed' function (defaults to IsWhitespace). All such characters are removed,
                 * and the resulting string is returned. This does not modify the current string its
                 * applied to - just returns the trimmed string.
                 */
                nonvirtual  String  LTrim (bool (*shouldBeTrimmmed) (Character) = [](Character c) -> bool { return c.IsWhitespace (); }) const;
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
                /**
                 * Return a new string based on this string where each lower case characer is replaced by its
                 * upper case equivilent. Note that non-lower-case characters (such as punctuation) un unchanged.
                 */
                nonvirtual  String  ToLowerCase () const;

            public:
                /**
                 * Return a new string based on this string where each lower case characer is replaced by its
                 * upper case equivilent. Note that non-upper-case characters (such as punctuation) un unchanged.
                 */
                nonvirtual  String  ToUpperCase () const;

            public:
                /**
                 * Return true if the string contains zero non-whitespace characters.
                 */
                nonvirtual  bool    IsWhitespace () const;

            public:
                /**
                 *  This function is for GUI/display purposes. It returns the given string, trimmed down
                 *  to at most maxLen characters, and removes surrounding whitespace.
                 */
                nonvirtual  String  LimitLength (size_t maxLen, bool keepLeft = true) const;

            public:
                /**
                 *  CopyTo () copies the contents of this string to the target buffer.
                 *  CopyTo () does NOT nul-terminate the target buffer, but DOES assert that (bufTo-bufFrom)
                 *  is >= this->GetLength ()
                 */
                nonvirtual  void    CopyTo (Character* bufFrom, Character* bufTo) const;
                nonvirtual  void    CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const;


            public:
                /**
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
                /**
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
                /**
                 */
                nonvirtual  SDKString   AsSDKString () const;
                nonvirtual  void        AsSDKString (SDKString* into) const;

            public:
                /**
                 */
                nonvirtual  string  AsNarrowSDKString () const;
                nonvirtual  void    AsNarrowSDKString (string* into) const;

            public:
                /**
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
                nonvirtual  int Compare (const Character* rhs, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  int Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  int Compare (const wchar_t* rhs, CompareOptions co = CompareOptions::eWithCase) const;
                nonvirtual  int Compare (const wchar_t* rhsStart, const wchar_t* rhsEnd, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                * @brief   Return true of the two argument strings are equal. This is equivilent to
                *              lhs.Compare (rhs, co);
                */
                nonvirtual  bool    Equals (const String& rhs, CompareOptions co = CompareOptions::eWithCase) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const String& rhs)
                 */
                nonvirtual  bool operator< (const String& rhs) const;
                nonvirtual  bool operator<= (const String& rhs) const;
                nonvirtual  bool operator> (const String& rhs) const;
                nonvirtual  bool operator>= (const String& rhs) const;
                nonvirtual  bool operator== (const String& rhs) const;
                nonvirtual  bool operator!= (const String& rhs) const;
                nonvirtual  bool operator< (const Character* rhs) const;
                nonvirtual  bool operator<= (const Character* rhs) const;
                nonvirtual  bool operator> (const Character* rhs) const;
                nonvirtual  bool operator>= (const Character* rhs) const;
                nonvirtual  bool operator== (const Character* rhs) const;
                nonvirtual  bool operator!= (const Character* rhs) const;
                nonvirtual  bool operator< (const wchar_t* rhs) const;
                nonvirtual  bool operator<= (const wchar_t* rhs) const;
                nonvirtual  bool operator> (const wchar_t* rhs) const;
                nonvirtual  bool operator>= (const wchar_t* rhs) const;
                nonvirtual  bool operator== (const wchar_t* rhs) const;
                nonvirtual  bool operator!= (const wchar_t* rhs) const;

            public:
                /**
                 *  std::basic_string alias for npos = kBadIndex
                 */
#if     !qCompilerAndStdLib_constexpr_StaticDataMember_Buggy
                static  constexpr size_t    npos   = kBadIndex;
#else
                DEFINE_CONSTEXPR_CONSTANT (size_t, npos, kBadIndex);
#endif

            public:
                /**
                 *  std::basic_string alias: size = GetLength
                 */
                nonvirtual  size_t          size () const;

            public:
                /**
                 *  std::basic_string alias: length = GetLength
                 */
                nonvirtual  size_t          length () const;

            public:
                // As with STL, the return value of the data () function should NOT be assumed to be
                // NUL-terminated
                //
                // The lifetime of the pointer returned is gauranteed until the next call to this String
                // envelope class (that is if other reps change, or are acceessed this data will not
                // be modified)
                nonvirtual  const wchar_t*  data () const;

            public:
                /**
                 *  This will always return a value which is NUL-terminated. Note that Stroika generally
                 *  does NOT keep strings in NUL-terminated form, so this could be a costly function,
                 *  requiring a copy of the data.
                 *
                 *  The lifetime of the pointer returned is gauranteed until the next call to this String
                 *  envelope class (that is if other reps change, or are acceessed this data will not be modified)
                 *  Note also that Stroika strings ALLOW internal nul bytes, so though the Stroika string
                 *  class NUL-terminates, it does nothing to prevent already existng NUL bytes from causing
                 *  confusion elsewhere.
                 */
                nonvirtual  const wchar_t*  c_str () const;

            public:
                // need more overloads
                nonvirtual  size_t find (wchar_t c, size_t startAt = 0) const;

            public:
                // need more overloads
                nonvirtual  size_t rfind (wchar_t c) const;

            public:
                // mimic (much of - need more overloads) STL variant
                nonvirtual  void erase (size_t from = 0, size_t count = kBadIndex);

            public:
                nonvirtual  void    push_back (wchar_t c);
                nonvirtual  void    push_back (Character c);

            public:
                /**
                 *  Compatable with STL::basic_string::subtr() - which interprets second argument as count. Not the same
                 *  as Stroika::String::SubString (where the second argument is a 'to')
                 *
                 *  @see SubString
                 */
                nonvirtual  String      substr (size_t from, size_t count = kBadIndex) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
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
            class   String::_IRep : public Iterable<Character>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            protected:
                using   _IterableSharedPtrIRep  =   String::_IterableSharedPtrIRep;

            protected:
                using   _SharedPtrIRep      =   String::_SharedPtrIRep;

            public:
                /*
                 *  UnsupportedFeatureException
                 *
                 *          Some 'reps' - don't support some features. For exmaple - a READONLY char* rep won't support
                 *      any operaiton that modifies the string. Its up to the CONTAINER change the rep to a generic one
                 *      that supports everything todo that.
                 *
                 *          We COULD have done this by having a SUPPORTSX() predicate method called on each rep before
                 *      each all, or have an extra return value about if it succeeded. But - that would be alot of
                 *      overhead for something likely quite rate. In other words, it will be EXCEPTIONAL that one tries
                 *      to change a string that happened to come from a 'readonly' source. We can handle that internally,
                 *      and transparently by thorwing an excpetion that never makes it out of the String module/cpp file.
                 */
                class   UnsupportedFeatureException {};

            public:
                virtual void                RemoveAll ()                            = 0;

                virtual Character           GetAt (size_t index) const              = 0;
                virtual void                SetAt (Character item, size_t index)    = 0;
                // This rep is NEVER called with nullptr src/end nor start==end
                virtual void                InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) = 0;
                virtual void                RemoveAt (size_t from, size_t to)       = 0;

                virtual void                SetLength (size_t newLength)            = 0;

                // return nullptr if its not already NUL-terminated
                virtual const wchar_t*      c_str_peek () const noexcept            = 0;

                // change rep so its NUL-termainted
                virtual const wchar_t*      c_str_change ()                         = 0;

                virtual const Character*    Peek () const                           = 0;

                virtual pair<const Character*, const Character*> GetData () const   = 0;

            public:
                /*
                 *  CopyTo () copies the contents of this string to the target buffer.
                 *  CopyTo () does NOT nul-terminate the target buffer, but DOES assert that (bufTo-bufFrom) is >= this->GetLength ()
                 */
                nonvirtual  void    CopyTo (Character* bufFrom, Character* bufTo) const;
                nonvirtual  void    CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const;
            };


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
