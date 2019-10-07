/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_FlavorPackage_inl_
#define _Stroika_Frameworks_Led_FlavorPackage_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ***************************** FlavorPackageExternalizer ************************
     ********************************************************************************
     */
    inline FlavorPackageExternalizer::FlavorPackageExternalizer (TextStore& ts)
        : inherited ()
        , fTextStore (ts)
    {
    }

    /*
     ********************************************************************************
     ***************************** FlavorPackageInternalizer ************************
     ********************************************************************************
     */
    inline FlavorPackageInternalizer::FlavorPackageInternalizer (TextStore& ts)
        : inherited ()
        , fTextStore (ts)
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

#endif /*_Stroika_Frameworks_Led_FlavorPackage_inl_*/
