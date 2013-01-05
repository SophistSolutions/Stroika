/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Memory_Common_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Memory_Common_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */


/*
 * See DOCS in Stroika/Foundation/Memory/Common.h
 */
#if     !defined (qMSVisualStudioCRTMemoryDebug)
#define qMSVisualStudioCRTMemoryDebug   0
#endif


/*
 * See DOCS in Stroika/Foundation/Memory/Common.h
 */
#if     !defined (qSupportValgrindQuirks)
#define qSupportValgrindQuirks   0
#endif



#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_Memory_Common_h_*/
