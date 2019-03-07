/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "StroikaConfig.h"

#include "../Characters/Format.h"
#include "../Characters/String_Constant.h"
#include "../Execution/Exceptions.h"
#include "../Memory/Bits.h"

#include "Version.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Configuration;

using Characters::String_Constant;

/*
 ********************************************************************************
 *********************************** Version ************************************
 ********************************************************************************
 */
Version Version::FromWin32Version4DotString (const Characters::String& win32Version4DotString)
{
    int major            = 0;
    int minor            = 0;
    int verStageOctet    = 0;
    int verSubStageOctet = 0;
    DISABLE_COMPILER_MSC_WARNING_START (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s
    int nMatchingItems = ::swscanf (win32Version4DotString.c_str (), L"%d.%d.%d.%d", &major, &minor, &verStageOctet, &verSubStageOctet);
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    if (major < 0 or major > 255)
        [[UNLIKELY_ATTR]]
        {
            Execution::Throw (Execution::RuntimeErrorException (L"Invalid Version String"sv));
        }
    if (minor < 0 or minor > 255)
        [[UNLIKELY_ATTR]]
        {
            Execution::Throw (Execution::RuntimeErrorException (L"Invalid Version String"sv));
        }
    int verStage = static_cast<uint16_t> (verStageOctet) >> 5;
    Assert (verStage == Memory::BitSubstring (verStageOctet, 5, 8)); // really only true cuz verStageOctet SB 8-bits - so if this fails, this answer probably better --LGP 2016-07-08
    uint16_t verSubStage = static_cast<uint16_t> ((Memory::BitSubstring (verStageOctet, 0, 5) << 7) + Memory::BitSubstring (verSubStageOctet, 1, 8));
    bool     verFinal    = verSubStageOctet & 0x1;
    if (nMatchingItems != 4 or not(ToInt (VersionStage::eSTART) <= verStage and verStage <= ToInt (VersionStage::eLAST))) {
        DbgTrace (L"win32Version4DotString=%s", win32Version4DotString.c_str ());
        Execution::Throw (Execution::Exception (L"Invalid Version String"sv));
    }
    return Version (static_cast<uint8_t> (major), static_cast<uint8_t> (minor), static_cast<VersionStage> (verStage), verSubStage, verFinal);
}

Version Version::FromPrettyVersionString (const Characters::String& prettyVersionString)
{
    uint8_t major = 0;
    uint8_t minor = 0;

    // Helper to throw if out of range
    auto my_wcstol_ = [=](const wchar_t* i, wchar_t** endResult) -> uint8_t {
        long l = wcstol (i, endResult, 10);
        if (l < 0 or l > numeric_limits<uint8_t>::max ())
            [[UNLIKELY_ATTR]]
            {
                DbgTrace (L"prettyVersionString=%s", prettyVersionString.c_str ());
                Execution::Throw (Execution::RuntimeErrorException (L"Invalid Version String: component out of range"));
            }
        return static_cast<uint8_t> (l);
    };

    const wchar_t* i        = prettyVersionString.c_str ();
    wchar_t*       tokenEnd = nullptr;
    major                   = my_wcstol_ (i, &tokenEnd); // @todo should validate, but no biggie
    if (i == tokenEnd)
        [[UNLIKELY_ATTR]]
        {
            DbgTrace (L"prettyVersionString=%s", prettyVersionString.c_str ());
            Execution::Throw (Execution::RuntimeErrorException (L"Invalid Version String"sv));
        }
    Assert (static_cast<size_t> (i - prettyVersionString.c_str ()) <= prettyVersionString.length ());
    i = tokenEnd + 1; // end plus '.' separator

    minor = my_wcstol_ (i, &tokenEnd);
    if (i == tokenEnd)
        [[UNLIKELY_ATTR]]
        {
            DbgTrace (L"prettyVersionString=%s", prettyVersionString.c_str ());
            Execution::Throw (Execution::RuntimeErrorException (L"Invalid Version String"sv)); // require form 1.0a3, or at least 1.0, but no 1
        }
    Assert (static_cast<size_t> (i - prettyVersionString.c_str ()) <= prettyVersionString.length ());
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
    Assert (static_cast<size_t> (i - prettyVersionString.c_str ()) <= prettyVersionString.length ());
    uint8_t verSubStage = my_wcstol_ (i, &tokenEnd);
    if (i == tokenEnd)
        [[UNLIKELY_ATTR]]
        {
            DbgTrace (L"prettyVersionString=%s", prettyVersionString.c_str ());
            Execution::Throw (Execution::RuntimeErrorException (L"Invalid Version String"sv)); // require form 1.0a3, or at least 1.0, but no 1
        }
    i               = tokenEnd;
    bool finalBuild = true;
    if (*i == 'x') {
        finalBuild = false;
    }
    Assert (static_cast<size_t> (i - prettyVersionString.c_str ()) <= prettyVersionString.length ());
    return Version (major, minor, verStage, verSubStage, finalBuild);
}

Characters::String Version::AsWin32Version4DotString () const
{
    return Characters::Format (L"%d.%d.%d.%d", fMajorVer, fMinorVer, (static_cast<uint8_t> (fVerStage) << 5) | (fVerSubStage >> 7), (static_cast<uint8_t> (fVerSubStage & 0x7f) << 1) | static_cast<uint8_t> (fFinalBuild));
}

Characters::String Version::AsPrettyVersionString () const
{
    String stageStr = L"?"sv;
    switch (fVerStage) {
        case VersionStage::Dev:
            stageStr = L"d"sv;
            break;
        case VersionStage::Alpha:
            stageStr = L"a"sv;
            break;
        case VersionStage::Beta:
            stageStr = L"b"sv;
            break;
        case VersionStage::ReleaseCandidate:
            stageStr = L"rc"sv;
            break;
        case VersionStage::Release:
            stageStr = fVerSubStage == 0 ? String{} : L"."sv;
            break;
    }
    String verSubStagStr;
    if (fVerSubStage != 0) {
        verSubStagStr = Characters::Format (L"%d", fVerSubStage);
    }
    if (not fFinalBuild) {
        verSubStagStr += String_Constant (L"x");
    }
    return Characters::Format (L"%d.%d%s%s", fMajorVer, fMinorVer, stageStr.c_str (), verSubStagStr.c_str ());
}

Characters::String Version::AsMajorMinorString () const
{
    return Characters::Format (L"%d.%d", fMajorVer, fMinorVer);
}