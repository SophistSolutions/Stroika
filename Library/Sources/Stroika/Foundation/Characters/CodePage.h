/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CodePage_h_
#define _Stroika_Foundation_Characters_CodePage_h_ 1

#include "../StroikaPreComp.h"

#include <exception>
#include <string>
#include <vector>

#include "../Configuration/Common.h"
#include "../Memory/Common.h"

/**
 *  \file
 *      This module is designed to provide mappings between wide UNICODE and various other code pages
 *      and UNICODE encodings.</p>
 *
 */

namespace Stroika::Foundation::Characters {

    using std::byte;

    class String;

    /**
     *       A codePage is a Win32 (really DOS) concept which describes a particular single or
     *    multibyte (narrow) character set encoding. 
     *  
     *  \note   Maybe someday add  a layer to map to/from Mac 'ScriptIDs' - which are basicly analagous, just not
     *          as widely used.
     * 
     *  \note   UINT in windows SDK;
     */
    using CodePage = uint32_t;

    /**
     *  Predefined well known code pages (generally not used/useful except on Windows)
     * 
     *  NOTE - not #if qPlatform_Windows or in windows namespace cuz can be used for Windows interoperability on other platforms - much supported portably.
     */
    namespace WellKnownCodePages {
        constexpr CodePage kANSI = 1252;

        constexpr CodePage kMAC             = 2;
        constexpr CodePage kPC              = 437; // IBM PC code page 437
        constexpr CodePage kPCA             = 850; // IBM PC code page 850, used by IBM Personal System/2
        constexpr CodePage kThai            = 874; // From uniscribe sample code (LGP 2003-01-13)
        constexpr CodePage kSJIS            = 932;
        constexpr CodePage kGB2312          = 936; // Chinese (Simplified)
        constexpr CodePage kKorean          = 949;
        constexpr CodePage kBIG5            = 950; // Chinese (Traditional)
        constexpr CodePage kEasternEuropean = 1250;
        constexpr CodePage kCyrilic         = 1251; // Russian (Cyrilic)
        constexpr CodePage kGreek           = 1253;
        constexpr CodePage kTurkish         = 1254;
        constexpr CodePage kHebrew          = 1255;
        constexpr CodePage kArabic          = 1256;
        constexpr CodePage kBaltic          = 1257;
        constexpr CodePage kVietnamese      = 1258;

        constexpr CodePage kUNICODE_WIDE           = 1200; // Standard UNICODE for MS-Windows
        constexpr CodePage kUNICODE_WIDE_BIGENDIAN = 1201;

        constexpr CodePage kUTF8 = 65001;
    }

    /**
     * \brief Returns a character encoding name registered by the IANA - for the given CodePage
     * 
     *  See https://www.w3.org/International/articles/http-charset/index#charset
     * 
     *  This works poorly, but is used in the HTTP Response generation, so cannot be removed for now.
     */
    wstring GetCharsetString (CodePage cp);

    /**
    * &&& redo as RuntimeException... - but todo move to separate file
     */
    class CodePageNotSupportedException : public exception {
    public:
        CodePageNotSupportedException (CodePage codePage);

    public:
        /**
         *  Provide a 'c string' variant of the exception message. Convert the UNICODE
         *  string argument to a narrow-string (multibyte) in the current locale multibyte encoding.
         */
        virtual const char* what () const noexcept override;

    public:
        /**
         */
        nonvirtual CodePage GetCodePage () const;

    private:
        string   fMsg_;
        CodePage fCodePage_;
    };

    /*
    * * @todo for v3 - make much clearer/portable!!! -  and use Stroika containers
    * 
    @CLASS:         CodePagesInstalled
    @DESCRIPTION:
                <p>Helper class to check what code pages are installed on a given machine.</p>
    */
    class CodePagesInstalled {
    public:
        CodePagesInstalled ();

    public:
        /*
        @METHOD:        CodePagesInstalled::GetAll
        @DESCRIPTION:   <p>Returns a list of all code pages installed on the system.
                    This list is returned in sorted order.</p>
        */
        vector<CodePage> GetAll ();

        /*
        @METHOD:        CodePagesInstalled::IsCodePageAvailable
        @DESCRIPTION:   <p>Checks if the given code page is installed.</p>
        */
        bool IsCodePageAvailable (CodePage cp);

    private:
        vector<CodePage> fCodePages_;
    };

    /*
    @CLASS:         CodePagePrettyNameMapper
    @DESCRIPTION:   <p>Code to map numeric code pages to symbolic user-interface appropriate names.</p>
    */
    class CodePagePrettyNameMapper {
    public:
        static wstring GetName (CodePage cp);

    public:
        struct CodePageNames;

        // This class builds commands with command names. The UI may wish to change these
        // names (eg. to customize for particular languages, etc)
        // Just patch these strings here, and commands will be created with these names.
        // (These names appear in text of undo menu item)
    public:
        static CodePageNames GetCodePageNames ();
        static void          SetCodePageNames (const CodePageNames& cmdNames);
        static CodePageNames MakeDefaultCodePageNames ();

    private:
        static CodePageNames sCodePageNames_;
    };
    struct CodePagePrettyNameMapper::CodePageNames {
        wstring fUNICODE_WIDE;
        wstring fUNICODE_WIDE_BIGENDIAN;
        wstring fANSI;
        wstring fMAC;
        wstring fPC;
        wstring fSJIS;
        wstring fUTF8;
        wstring f850;
        wstring f851;
        wstring f866;
        wstring f936;
        wstring f949;
        wstring f950;
        wstring f1250;
        wstring f1251;
        wstring f10000;
        wstring f10001;
        wstring f50220;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CodePage.inl"

#endif /*_Stroika_Foundation_Characters_CodePage_h_*/
