/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ***************************** FlavorPackageExternalizer ************************
     ********************************************************************************
     */
    inline FlavorPackageExternalizer::FlavorPackageExternalizer (TextStore& ts)
        : fTextStore{ts}
    {
    }

    /*
     ********************************************************************************
     ***************************** FlavorPackageInternalizer ************************
     ********************************************************************************
     */
    inline FlavorPackageInternalizer::FlavorPackageInternalizer (TextStore& ts)
        : fTextStore{ts}
    {
    }

    /*
     ********************************************************************************
     ***************************** ReaderFlavorPackage ******************************
     ********************************************************************************
     */
    inline bool ReaderFlavorPackage::GetFlavorAvailable_TEXT () const
    {
        if (GetFlavorAvailable (kTEXTClipFormat)) {
            return true;
        }
        return false;
    }

}
