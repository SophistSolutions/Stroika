/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Memory_BlockAllocated_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Memory_BlockAllocated_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

//#define   qStroika_Foundation_Memory_PreferBlockAllocation                       0
//#define   qStroika_Foundation_Memory_PreferBlockAllocation                       1
#ifndef qStroika_Foundation_Memory_PreferBlockAllocation
#define qStroika_Foundation_Memory_PreferBlockAllocation 1
#endif

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_Memory_BlockAllocated_h_*/
