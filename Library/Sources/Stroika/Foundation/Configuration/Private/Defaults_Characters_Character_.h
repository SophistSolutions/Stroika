/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Characters_Character_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Characters_Character_h_  1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */


// First DEFINE qNBytesPerWCharT
#ifndef qNBytesPerWCharT
#if     defined(_WINDOWS) || defined(_WIN64) || defined(_WIN32)
#define qNBytesPerWCharT    2
#else
#define qNBytesPerWCharT    4
#endif
#endif


#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_Characters_Character_h_*/
