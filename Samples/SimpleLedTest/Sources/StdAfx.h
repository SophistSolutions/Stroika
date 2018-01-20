/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef __StdAfx__h__
#define __StdAfx__h__ 1

//#define WIN32_LEAN_AND_MEAN       // Exclude rarely-used stuff from Windows headers

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE // UNICODE is used by Windows headers
#endif
#endif

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE // _UNICODE is used by C-runtime/MFC headers
#endif
#endif

#include "Stroika/Foundation/StroikaPreComp.h"

#include <windows.h>

#endif
