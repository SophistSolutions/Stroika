/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Format_h_
#define _Stroika_Foundation_Characters_Format_h_    1

#include    "../StroikaPreComp.h"

#include    <ios>
#include    <locale>

#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"

#include    "String.h"



/**
 * TODO:
 *
 *      @todo   DOCUMENT BEHAVIOR OF STRING2INT() for bad strings. What does it do?
 *              AND SIMILARPT FOR hexString2Int. And for btoh – probably rewrite to use strtoul/strtol etc
 *
 *      @todo   Same changes to HexString2Int() as we did with String2Int() - template on return value.
 *
 *      @todo   Consdier if we should have variants of these funtions taking a locale, or
 *              always using C/currnet locale. For the most part - I find it best to use the C locale.
 *              But DOCUMENT in all cases!!! And maybe implement variants...
 */



/**
@CONFIGVAR:     qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat
@DESCRIPTION:   Very surprising difference between Microsoft of Linux interpretations of sprintf(). It appears the Linux/gcc approach was
                blessed by ISO - and so is now 'correct'. But no matter, because its so fundementally different, they will have a difficult
                time changing.

                The issue is - for sprintf - and in our case vswprintf - what is the interpretation of '%s'. Does this match a char* or
                wchar_t* string in the argument list?

                According to Microsoft, it would appear to match wchar_t*.
                    From http://msdn.microsoft.com/en-us/library/ybk95axf(v=vs.71).aspx:
                    int main( void )
                    {
                       wchar_t buf[100];
                       int len = swprintf( buf, L"%s", L"Hello world" );
                       printf( "wrote %d characters\n", len );
                       len = swprintf( buf, L"%s", L"Hello\xffff world" );
                       // swprintf fails because string contains WEOF (\xffff)
                       printf( "wrote %d characters\n", len );
                    }

                But according to http://www.tin.org/bin/man.cgi?section=3&topic=vsnprintf:
                      s      If  no  l  modifier  is  present:  The  const char * argument is
                              expected to be a pointer to an array of character type  (pointer
                              to  a string).  Characters from the array are written up to (but
                              not including) a terminating null byte ('\0'); if a precision is
                              specified,  no more than the number specified are written.  If a
                              precision is given, no null byte need be present; if the  preci-
                              sion is not specified, or is greater than the size of the array,
                              the array must contain a terminating null byte.

                              If an l modifier is present: The const  wchar_t  *  argument  is
                              expected  to  be a pointer to an array of wide characters.  Wide
                              characters from the array are converted to multibyte  characters
                              (each  by  a  call  to the wcrtomb() function, with a conversion
                              state starting in the initial state before the first wide  char-
                              acter),  up  to and including a terminating null wide character.
                              The resulting multibyte characters are written up  to  (but  not
                              including)  the  terminating null byte. If a precision is speci-
                              fied, no more bytes than the number specified are  written,  but
                              no  partial multibyte characters are written. Note that the pre-
                              cision determines the number of bytes written, not the number of
                              wide  characters  or screen positions.  The array must contain a
                              terminating null wide character, unless a precision is given and
                              it  is  so  small  that  the  number of bytes written exceeds it
                              before the end of the array is reached.
    */
#ifndef qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat
#error "qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat should normally be defined indirectly by StroikaConfig.h"
#endif



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /*
             * Format is the Stroika wrapper on sprintf().
             * The main differences between sprintf () and Format are:
             *      (1)     No need for buffer management. ANY sized buffer will be automatically allocated internally and returned as a
             *              string (eventually probably using Stroika::String - but for now std::string).
             *
             *      (2)     This version of format has a SUBTLE - but important difference from std::c++ / sprintf() in the interpretation of
             *              %s format strings in Format(const wchar_t*).  The interpretation of Format (const char*,...) is the same
             *              but for Format (const wchar_t* format, ...) - %s is assumed to match a const wchar_t* string in the variable
             *              argument list.
             *
             *              This deviation from the c++ standard (technically not a deviation because Stroika::Foundation::Characters::Format() isn't
             *              covered by the stdc++ ;-)) - is because of two reasons:
             *                  a)  Microsoft has this interpretation.
             *                  b)  Its a BETTER interpretation (allowing for stuff like (Format (SDKSTR("time%s"), count==1? SDKSTR (""): SDKSTR ("s"));
             *
             *      See also qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat
             */
            String FormatV (const wchar_t* format, va_list argsList);
            String Format (const wchar_t* format, ...);


            /**
             *  Convert the given hex-format string to an unsigned integer.
             *  String2Int will return 0 if no valid parse, and UINT_MAX on overflow.
             *
             *  @see strtoul(), or @see wcstol (). This is a simple wrapper on strtoul() or wcstoul().
             *  strtoul() etc are more flexible. This is merely meant to be an often convenient wrapper.
             *  Use strtoul etc directly to see if the string parsed properly.
             */
            unsigned int     HexString2Int (const String& s);


            /**
             *  Convert the given decimal-format integral string to any integer type
             *  ( e.g. signed char, unsigned short int, long long int, uint32_t etc).
             *
             *  String2Int will return 0 if no valid parse, and numeric_limits<T>::min on underflow,
             *  numeric_limits<T>::max on overflow.
             *      @todo consider if this is unwise - it seems we ought to throw? Or have a variant
             *              perhaps that does no throw?
             *
             *              CONSIDER!
             *
             *  @see strtoll(), or @see wcstoll (). This is a simple wrapper on strtoll() / wcstoll ().
             *  strtoll() is more flexible. This is merely meant to be an often convenient wrapper.
             *  Use strtoll etc directly to see if the string parsed properly.
             */
            template    <typename T>
            T     String2Int (const String& s);


            /**
             *  Convert the given decimal-format floating point string to an double.
             *  String2Float will return nan () if no valid parse.
             *
             *  @see strtod(), or @see wcstod (). This is a simple wrapper on strtod() / wcstod ()
             *  except that it returns nan() on invalid data, instead of zero.
             *
             *  strtod() /etc are more flexible. This is merely meant to be an often convenient wrapper.
             *  Use strtod etc directly to see if the string parsed properly.
             */
            double  String2Float (const String& s);


            /**
             *  Note - this routine ignores the current locale settings, and always uses the 'C' locale.
             *
             *  Precision (here) is defined to be the number of digits after the decimal point. This prints
             *  in fixed point format (not scientific notation), and trims any trailing zeros (after the
             *  decimal point).
             *
             *  map nan to empty string, and use limited precision, and strip trialing .0.
             */
            struct  Float2StringOptions {
                enum UseCLocale { eUseCLocale };
                enum UseCurrentLocale { eUseCurrentLocale };
                Float2StringOptions ();
                Float2StringOptions (UseCLocale);   // same as default
                Float2StringOptions (UseCurrentLocale);
                Float2StringOptions (const std::locale& l);
                Float2StringOptions (std::ios_base::fmtflags fmtFlags);
                Float2StringOptions (unsigned int precision);

                Memory::Optional<unsigned int>              fPrecision;
                Memory::Optional<std::ios_base::fmtflags>   fFmtFlags;
                Memory::Optional<std::locale>               fUseLocale; // if empty, use C-locale
            };
            String Float2String (float f, const Float2StringOptions& options = Float2StringOptions ());
            String Float2String (double f, const Float2StringOptions& options = Float2StringOptions ());
            String Float2String (long double f, const Float2StringOptions& options = Float2StringOptions ());


            /**
             *  This could be generalized to accomodate TRIM/merge with TRIM, but it sometimes used to trim other
             *  characters (like trailing CRLF, or trailing '.').
             *
             *  @todo   Should this be part of String class? Or deleted?
             */
            String StripTrailingCharIfAny (const String& s, wchar_t c);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Format.inl"

#endif  /*_Stroika_Foundation_Characters_Format_h_*/
