/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_IO_Network_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_IO_Network_h_   1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

#if     qPlatform_Windows
// allows selection of WinSock2 in files that depend on it, but use <Windows.h>
#define _WINSOCKAPI_
#endif // qPlatform_Windows


#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_IO_Network_h_*/
