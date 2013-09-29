/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_SDKChar_h_
#define _Stroika_Foundation_Characters_SDKChar_h_ 1

#include    "../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <tchar.h>
#endif

#include    "../Configuration/Common.h"



/**
 *  Each platform SDK has its own policy for representing characters. Some use narrow characters, and a predefined code page,
 *  and others use wide-characters (unicode).
 *
 *  SDKChar is the underlying represenation of the SDK's characters.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
            @CONFIGVAR:     qTargetPlatformSDKUseswchar_t
            @DESCRIPTION:   <p>defines if we use wchar_t or char for most platform interfaces (mostly applicable/useful for windows)</p>
             */
#if     !defined (qTargetPlatformSDKUseswchar_t)
#error "qTargetPlatformSDKUseswchar_t should normally be defined indirectly by StroikaConfig.h"
#endif


#if     qTargetPlatformSDKUseswchar_t
            typedef wchar_t SDKChar;
#else
            typedef char    SDKChar;
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "SDKChar.inl"

#endif  /*_Stroika_Foundation_Characters_SDKChar_h_*/
