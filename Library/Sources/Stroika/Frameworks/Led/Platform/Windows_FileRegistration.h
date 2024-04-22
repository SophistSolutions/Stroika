/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Platform_Windows_FileRegistration_h_
#define _Stroika_Frameworks_Led_Platform_Windows_FileRegistration_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

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
        Win32FileAssociationRegistrationHelper (const SDKString& fileSuffix);

        nonvirtual SDKString GetAssociatedProgID () const;
        nonvirtual SDKString GetAssociatedDefaultIcon () const;
        nonvirtual SDKString GetAssociatedEditCommand () const;
        nonvirtual SDKString GetAssociatedOpenCommand () const;

    public:
        nonvirtual void SetAssociatedProgIDAndOpenCommand (const SDKString& progID, const SDKString& progIDPrettyName, const SDKString& defaultIcon,
                                                           const SDKString& editCommandLine, const SDKString& openCommandLine);

    private:
        struct KeyHolder {
            HKEY fKey;
            KeyHolder (HKEY hk);
            enum CreateIfNotThereFlag {
                eCreateIfNotThere
            };
            KeyHolder (HKEY baseKey, LPCTSTR lpSubKey);
            KeyHolder (HKEY baseKey, LPCTSTR lpSubKey, CreateIfNotThereFlag createIfNotThereFlag);
            ~KeyHolder ();
            operator HKEY ();
        };

    private:
        SDKString fFileSuffix;
    };
#endif

#if qPlatform_Windows
    class Win32UIFileAssociationInfo {
    public:
        Win32UIFileAssociationInfo (const SDKString& fileSuffix, const SDKString& fileProgID, const SDKString& fileProgIDPrettyName,
                                    const SDKString& defaultIcon, const SDKString& shellEditNOpenCommandLine);
        Win32UIFileAssociationInfo (const SDKString& fileSuffix, const SDKString& fileProgID, const SDKString& fileProgIDPrettyName,
                                    const SDKString& defaultIcon, const SDKString& shellEditCommandLine, const SDKString& shellOpenCommandLine);

    public:
        SDKString fFileSuffix;
        SDKString fFileProgID;
        SDKString fFileProgIDPrettyName;
        SDKString fDefaultIcon;
        SDKString fShellEditCommandLine;
        SDKString fShellOpenCommandLine;

    public:
        static SDKString kNoChange;
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
        virtual void    ExpandVariables (SDKString* valWithVars) const;

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
