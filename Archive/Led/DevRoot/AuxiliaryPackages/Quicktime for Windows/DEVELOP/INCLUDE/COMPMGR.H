
// ---------------------------------------------------------------------
//
// COMPMGR.H - QuickTime for Windows Component Header File
//
//             Version 1.0
//
//             (c) Copyright 1988-1994 Apple Computer, Inc. All Rights Reserved.
//
// ---------------------------------------------------------------------


// Prevent multiple inclusion
// --------------------------
#ifndef __COMPMGR_H__
#define __COMPMGR_H__

// thngIdentifify CID interface constants
// --------------------------------------
#define CID_VERSION  0x00010000           // version of CID interface
#define THING  QTFOURCC('t','h','n','g')  // ostype to identify components


// Component Flags
// ---------------
#define cmpWantsRegisterMessage     0x80000000


// Component Manager/Component interface structure through thngIdentify
// --------------------------------------------------------------------
typedef struct tagCID {                   // Hungarian: cid
  LONG      lVersion;                     // version of thngIdentify interface
  short     sComponentCount;              // number of components within file
  LPCD      lpcdTable;                    // pointer to ComponentDescription records
  FIXUPFUNC lpfnTBFixup;                  // pointer to Toolbox entry point fixup
  FIXUPFUNC lpfnCMFixup;                  // pointer to CM entry point fixup
} CID, FAR *LPCID;


// Component Selectors
// -------------------
#define kComponentOpenSelect        -1    // mandatory
#define kComponentCloseSelect       -2    // mandatory
#define kComponentCanDoSelect       -3    // mandatory
#define kComponentVersionSelect     -4    // mandatory
#define kComponentRegisterSelect    -5    // optional
#define kComponentTargetSelect      -6    // optional


// Stack offset for component functions
// ------------------------------------
typedef struct _tagSTKOFF_CMP {
#ifndef _WIN32
  WORD  words[3];                         // words on stack from glue code
#else
  DWORD  dword[2];                        // dword on stack from glue code
#endif
} STKOFF_CMP;

#endif // __COMPMGR_H__
