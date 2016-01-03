/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "StroikaConfig.h"

#include    "../Characters/Format.h"
#include    "../Characters/String_Constant.h"
#include    "../Execution/StringException.h"

#include    "Version.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Configuration;

using   Characters::String_Constant;


/*
 ********************************************************************************
 *********************************** Version ************************************
 ********************************************************************************
 */
Version Version::FromWin32Version4DotString (const Characters::String& win32Version4DotString)
{
    int major       =   0;
    int minor       =   0;
    int verStage    =   0;
    int verSubStage =   0;
    DISABLE_COMPILER_MSC_WARNING_START(4996)// MSVC SILLY WARNING ABOUT USING swscanf_s
    int nMatchingItems = ::swscanf (win32Version4DotString.c_str (), L"%d.%d.%d.%d", &major, &minor, &verStage, &verSubStage);
    DISABLE_COMPILER_MSC_WARNING_END(4996)
    if (nMatchingItems != 4 or not (ToInt (VersionStage::eSTART) <= verStage and verStage <= ToInt (VersionStage::eLAST))) {
        Execution::DoThrow (Execution::StringException (L"Invalid Version String"));
    }
    return Version (major, minor, static_cast<VersionStage> (verStage), verSubStage);   // in this form - no encoding of 'final build'
}

Version Version::FromPrettyVersionString (const Characters::String& prettyVersionString)
{
    uint8_t major       =   0;
    uint8_t minor       =   0;

    // Helper to throw if out of range
    auto my_wcstol_ = [] (const wchar_t* i, wchar_t** endResult) -> uint8_t {
        long l = wcstol (i, endResult, 10);
        if (l < 0 or l > numeric_limits<uint8_t>::max ())
        {
            Execution::DoThrow (Execution::StringException (L"Invalid Version String: component out of range"));
        }
        return static_cast<uint8_t> (l);
    };

    const wchar_t*  i   =   prettyVersionString.c_str ();
    wchar_t*    tokenEnd    =   nullptr;
    major = my_wcstol_ (i, &tokenEnd);  // @todo should validate, but no biggie
    if (i == tokenEnd) {
        Execution::DoThrow (Execution::StringException (L"Invalid Version String"));
    }
    Assert (static_cast<size_t> (i - prettyVersionString.c_str ()) <= prettyVersionString.length ());
    i = tokenEnd + 1;   // end plus '.' separator

    minor = my_wcstol_ (i, &tokenEnd);
    if (i == tokenEnd) {
        Execution::DoThrow (Execution::StringException (L"Invalid Version String"));    // require form 1.0a3, or at least 1.0, but no 1
    }
    Assert (static_cast<size_t> (i - prettyVersionString.c_str ()) <= prettyVersionString.length ());
    i = tokenEnd;

    VersionStage    verStage = VersionStage::Release;
    switch (*i) {
        case '\0': {
                // e.g. 1.0
                return Version (major, minor, VersionStage::Release, 0);
            }
        case 'a': {
                verStage = VersionStage::Alpha;
                i += 1;
            }
            break;
        case 'b': {
                verStage = VersionStage::Beta;
                i += 1;
            }
            break;
        case 'd': {
                verStage = VersionStage::Dev;
                i += 1;
            }
            break;
        case 'r': {
                verStage = VersionStage::ReleaseCandidate;
                i += 2; // rc
            }
            break;
        case '.': {
                // e.g. 1.0.3
                verStage = VersionStage::Release;
                i += 1;
            }
            break;
    }
    Assert (static_cast<size_t> (i - prettyVersionString.c_str ()) <= prettyVersionString.length ());
    uint8_t verSubStage = my_wcstol_ (i, &tokenEnd);
    if (i == tokenEnd) {
        Execution::DoThrow (Execution::StringException (L"Invalid Version String"));    // require form 1.0a3, or at least 1.0, but no 1
    }
    i = tokenEnd;
    bool    finalBuild = true;
    if (*i == 'x') {
        finalBuild = false;
    }
    Assert (static_cast<size_t> (i - prettyVersionString.c_str ()) <= prettyVersionString.length ());
    return Version (major, minor, verStage, verSubStage, finalBuild);
}

Characters::String      Version::AsWin32Version4DotString () const
{
    return Characters::Format (L"%d.%d.%d.%d", fMajorVer, fMinorVer, fVerStage, fVerSubStage);
}

Characters::String      Version::AsPrettyVersionString () const
{
    String  stageStr    =   String_Constant (L"?");
    switch (fVerStage) {
        case    VersionStage::Dev:
            stageStr = String_Constant (L"d");
            break;
        case    VersionStage::Alpha:
            stageStr = String_Constant (L"a");
            break;
        case    VersionStage::Beta:
            stageStr = String_Constant (L"b");
            break;
        case    VersionStage::ReleaseCandidate:
            stageStr = String_Constant (L"rc");
            break;
        case    VersionStage::Release:
            stageStr = fVerSubStage == 0 ? String () : String_Constant (L".");
            break;
    }
    String  verSubStagStr;
    if (fVerSubStage != 0) {
        verSubStagStr = Characters::Format (L"%d", fVerSubStage);
    }
    if (not fFinalBuild) {
        verSubStagStr +=  String_Constant (L"x");
    }
    return Characters::Format (L"%d.%d%s%s", fMajorVer, fMinorVer, stageStr.c_str (), verSubStagStr.c_str ());
}
