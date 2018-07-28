/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Platform_Windows_FileRegistration_h_
#define _Stroika_Frameworks_Led_Platform_Windows_FileRegistration_h_ 1

#include "../../../Foundation/StroikaPreComp.h"

/*
@MODULE:    Led_Win32_FileRegistration
@DESCRIPTION:
        <p>Functionality not directly related to Led - per-se, but often helpful in building Led-based applications.
    This code helps you to update the windows registration to say what file types you support.</p>
 */

#include "../Support.h"

namespace Stroika::Frameworks::Led::Platform {

#if qPlatform_Windows
    class Win32FileAssociationRegistrationHelper {
    public:
        Win32FileAssociationRegistrationHelper (const Led_SDK_String& fileSuffix);

        nonvirtual Led_SDK_String GetAssociatedProgID () const;
        nonvirtual Led_SDK_String GetAssociatedDefaultIcon () const;
        nonvirtual Led_SDK_String GetAssociatedEditCommand () const;
        nonvirtual Led_SDK_String GetAssociatedOpenCommand () const;

    public:
        nonvirtual void SetAssociatedProgIDAndOpenCommand (const Led_SDK_String& progID, const Led_SDK_String& progIDPrettyName, const Led_SDK_String& defaultIcon, const Led_SDK_String& editCommandLine, const Led_SDK_String& openCommandLine);

    private:
        struct KeyHolder {
            HKEY fKey;
            KeyHolder (HKEY hk);
            enum CreateIfNotThereFlag { eCreateIfNotThere };
            KeyHolder (HKEY baseKey, LPCTSTR lpSubKey);
            KeyHolder (HKEY baseKey, LPCTSTR lpSubKey, CreateIfNotThereFlag createIfNotThereFlag);
            ~KeyHolder ();
            operator HKEY ();
        };

    private:
        Led_SDK_String fFileSuffix;
    };
#endif

#if qPlatform_Windows
    class Win32UIFileAssociationInfo {
    public:
        Win32UIFileAssociationInfo (
            const Led_SDK_String& fileSuffix,
            const Led_SDK_String& fileProgID,
            const Led_SDK_String& fileProgIDPrettyName,
            const Led_SDK_String& defaultIcon,
            const Led_SDK_String& shellEditNOpenCommandLine);
        Win32UIFileAssociationInfo (
            const Led_SDK_String& fileSuffix,
            const Led_SDK_String& fileProgID,
            const Led_SDK_String& fileProgIDPrettyName,
            const Led_SDK_String& defaultIcon,
            const Led_SDK_String& shellEditCommandLine,
            const Led_SDK_String& shellOpenCommandLine);

    public:
        Led_SDK_String fFileSuffix;
        Led_SDK_String fFileProgID;
        Led_SDK_String fFileProgIDPrettyName;
        Led_SDK_String fDefaultIcon;
        Led_SDK_String fShellEditCommandLine;
        Led_SDK_String fShellOpenCommandLine;

    public:
        static Led_SDK_String kNoChange;
    };
#endif

#if qPlatform_Windows
    class Win32UIFileAssociationRegistrationHelper {
    public:
        Win32UIFileAssociationRegistrationHelper (HINSTANCE hInstance);
        Win32UIFileAssociationRegistrationHelper (HINSTANCE hInstance, const vector<Win32UIFileAssociationInfo>& infoRecs);

    public:
        nonvirtual void Add (const Win32UIFileAssociationInfo& infoRec);

    public:
        nonvirtual void DoIt () noexcept;

    public:
        nonvirtual bool RegisteredToSomeoneElse () const;
        nonvirtual void ApplyChangesSilently ();
        virtual bool    CheckUserSaysOKToUpdate () const;
        virtual void    ExpandVariables (Led_SDK_String* valWithVars) const;

    private:
        HINSTANCE                          fHINSTANCE;
        vector<Win32UIFileAssociationInfo> fInfoRecs;
    };
#endif

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */

}

#endif /*_Stroika_Frameworks_Led_Platform_Windows_FileRegistration_h_*/
