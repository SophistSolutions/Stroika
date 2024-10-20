/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Configuration/StroikaConfig.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Memory/Bits.h"

#include "Version.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Configuration;

/*
 ********************************************************************************
 *********************************** Version ************************************
 ********************************************************************************
 */
Version Version::FromWin32Version4DotString (const Characters::String& win32Version4DotString)
{
    int    major               = 0;
    int    minor               = 0;
    int    verStageOctet       = 0;
    int    verSubStageOctet    = 0;
    String win32Version4DotStr = win32Version4DotString; // copy so can call c_str() on copy
    DISABLE_COMPILER_MSC_WARNING_START (4996)            // MSVC SILLY WARNING ABOUT USING swscanf_s
    int nMatchingItems = ::swscanf (win32Version4DotStr.c_str (), L"%d.%d.%d.%d", &major, &minor, &verStageOctet, &verSubStageOctet);
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    if (major < 0 or major > 255) [[unlikely]] {
        static const Execution::RuntimeErrorException kException_{"Invalid Version String"sv};
        Execution::Throw (kException_);
    }
    if (minor < 0 or minor > 255) [[unlikely]] {
        Execution::Throw (Execution::RuntimeErrorException{"Invalid Version String"sv});
    }
    int verStage = static_cast<uint16_t> (verStageOctet) >> 5;
    Assert (verStage == Memory::BitSubstring (verStageOctet, 5, 8)); // really only true cuz verStageOctet SB 8-bits - so if this fails, this answer probably better --LGP 2016-07-08
    uint16_t verSubStage =
        static_cast<uint16_t> ((Memory::BitSubstring (verStageOctet, 0, 5) << 7) + Memory::BitSubstring (verSubStageOctet, 1, 8));
    bool verFinal = verSubStageOctet & 0x1;
    if (nMatchingItems != 4 or not(ToInt (VersionStage::eSTART) <= verStage and verStage <= ToInt (VersionStage::eLAST))) {
        DbgTrace ("win32Version4DotString={}"_f, win32Version4DotStr);
        static const Execution::Exception kException_{"Invalid Version String"sv};
        Execution::Throw (kException_);
    }
    return Version{static_cast<uint8_t> (major), static_cast<uint8_t> (minor), static_cast<VersionStage> (verStage), verSubStage, verFinal};
}

Version Version::FromPrettyVersionString (const Characters::String& prettyVersionString)
{
    uint8_t major = 0;
    uint8_t minor = 0;

    wstring ppv = prettyVersionString.As<wstring> (); // copy so can c_str()

    // Helper to throw if out of range
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wunused-lambda-capture\"");
    auto my_wcstol_ = [&ppv] (const wchar_t* i, wchar_t** endResult) -> uint8_t {
        long l = wcstol (i, endResult, 10);
        if (l < 0 or l > numeric_limits<uint8_t>::max ()) [[unlikely]] {
            DbgTrace (L"prettyVersionString={}"_f, ppv);
            static const Execution::RuntimeErrorException kException_{"Invalid Version String: component out of range"_k};
            Execution::Throw (kException_);
        }
        return static_cast<uint8_t> (l);
    };
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wunused-lambda-capture\"");

    const wchar_t* i        = ppv.c_str ();
    wchar_t*       tokenEnd = nullptr;
    major                   = my_wcstol_ (i, &tokenEnd); // @todo should validate, but no biggie
    if (i == tokenEnd) [[unlikely]] {
        DbgTrace (L"prettyVersionString={}"_f, ppv);
        static const Execution::RuntimeErrorException kException_{"Invalid Version String"sv};
        Execution::Throw (kException_);
    }
    Assert (static_cast<size_t> (i - ppv.c_str ()) <= prettyVersionString.length ());
    i = tokenEnd + 1; // end plus '.' separator

    minor = my_wcstol_ (i, &tokenEnd);
    if (i == tokenEnd) [[unlikely]] {
        DbgTrace (L"prettyVersionString={}"_f, ppv);
        Execution::Throw (Execution::RuntimeErrorException{"Invalid Version String"sv}); // require form 1.0a3, or at least 1.0, but no 1
    }
    Assert (static_cast<size_t> (i - ppv.c_str ()) <= ppv.length ());
    i = tokenEnd;

    VersionStage verStage = VersionStage::Release;
    switch (*i) {
        case '\0': {
            // e.g. 1.0
            return Version (major, minor, VersionStage::Release, 0);
        }
        case 'a': {
            verStage = VersionStage::Alpha;
            i += 1;
        } break;
        case 'b': {
            verStage = VersionStage::Beta;
            i += 1;
        } break;
        case 'd': {
            verStage = VersionStage::Dev;
            i += 1;
        } break;
        case 'r': {
            verStage = VersionStage::ReleaseCandidate;
            i += 2; // rc
        } break;
        case '.': {
            // e.g. 1.0.3
            verStage = VersionStage::Release;
            i += 1;
        } break;
    }
    Assert (static_cast<size_t> (i - ppv.c_str ()) <= ppv.size ());
    uint8_t verSubStage = my_wcstol_ (i, &tokenEnd);
    if (i == tokenEnd) [[unlikely]] {
        DbgTrace (L"prettyVersionString={}"_f, ppv);
        static const Execution::RuntimeErrorException kException_{"Invalid Version String"sv};
        Execution::Throw (kException_); // require form 1.0a3, or at least 1.0, but no 1
    }
    i               = tokenEnd;
    bool finalBuild = true;
    if (*i == 'x') {
        finalBuild = false;
    }
    Assert (static_cast<size_t> (i - ppv.c_str ()) <= ppv.size ());
    return Version{major, minor, verStage, verSubStage, finalBuild};
}

Characters::String Version::AsWin32Version4DotString () const
{
    return Characters::Format ("{}.{}.{}.{}"_f, fMajorVer, fMinorVer, (static_cast<uint8_t> (fVerStage) << 5) | (fVerSubStage >> 7),
                               (static_cast<uint8_t> (fVerSubStage & 0x7f) << 1) | static_cast<uint8_t> (fFinalBuild));
}

Characters::String Version::AsPrettyVersionString () const
{
    String stageStr = "?"sv;
    switch (fVerStage) {
        case VersionStage::Dev:
            stageStr = "d"sv;
            break;
        case VersionStage::Alpha:
            stageStr = "a"sv;
            break;
        case VersionStage::Beta:
            stageStr = "b"sv;
            break;
        case VersionStage::ReleaseCandidate:
            stageStr = "rc"sv;
            break;
        case VersionStage::Release:
            stageStr = fVerSubStage == 0 ? String{} : "."sv;
            break;
    }
    String verSubStagStr;
    if (fVerSubStage != 0) {
        verSubStagStr = Characters::Format ("{}"_f, fVerSubStage);
    }
    if (not fFinalBuild) {
        verSubStagStr += "x"sv;
    }
    return Characters::Format ("{}.{}{}{}"_f, fMajorVer, fMinorVer, stageStr, verSubStagStr);
}

Characters::String Version::AsMajorMinorString () const
{
    return Characters::Format ("{}.{}"_f, fMajorVer, fMinorVer);
}