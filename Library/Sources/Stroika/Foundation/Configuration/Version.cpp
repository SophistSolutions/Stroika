/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "StroikaConfig.h"

#include    "../Characters/Format.h"
#include    "../Characters/String_Constant.h"

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
Version::Version (const Characters::String& win32Version4DoTString)
{
    int major       =   0;
    int minor       =   0;
    int verStage    =   0;
    int verSubStage =   0;
    DISABLE_COMPILER_MSC_WARNING_START(4996)// MSVC SILLY WARNING ABOUT USING swscanf_s
    (void)::swscanf (win32Version4DoTString.c_str (), L"%d.%d.%d.%d", &major, &minor, &verStage, &verSubStage);
    DISABLE_COMPILER_MSC_WARNING_END(4996)
    fMajorVer = major;
    fMinorVer = minor;
    fVerStage = static_cast<VersionStage> (verStage);   // hack - should throw if out of range
    fVerSubStage = verSubStage;
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
    String  verSubStagStr   =   L"";
    if (fVerSubStage != 0) {
        verSubStagStr = Characters::Format (L"%d", fVerSubStage);
    }
    if (not fFinalBuild) {
        verSubStagStr += L"x";
    }
    return Characters::Format (L"%d.%d%s%s", fMajorVer, fMinorVer, stageStr.c_str (), verSubStagStr.c_str ());
}
