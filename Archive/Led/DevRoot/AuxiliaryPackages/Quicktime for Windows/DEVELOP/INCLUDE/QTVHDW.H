

// ---------------------------------------------------------------------
//
// QTVHDW.H    - QuickTime for Windows C and C++ Bindings
//
//             Version 2.0
//
//             (c) Copyright 1988-1994 Apple Computer, Inc. All Rights Reserved.
//
// ---------------------------------------------------------------------


#if !defined __QTVHDW_H
#define __QTVHDW_H

// Header files
#include <qtcodec.h>
#include <dciman.h>

/* VHDW function selectors
   Negative selectors are reserved by the Component Manager
*/
enum VHDW_SELECTOR { kSniffVideoHardware            = 0,
                     kVideoHardwareSetBounds        = 1
                   } ;

#if defined __cplusplus
extern "C" {
#endif

// Functions

HDWDISP QTAPI SniffVideoHardware( ComponentInstance ci, BMP_TYPE bmpt, LPSTR lpszCompID, LPSTR lpszHdwID) ;
HDWDISP QTAPI VideoHardwareSetBounds( ComponentInstance ci, LPRECT src,LPRECT dst ) ;
LPVOID QTAPI VidGetHdwType( BMP_TYPE bmpt) ;

#if defined __cplusplus
} // extern "C"
#endif

#endif                                      // __QTVHDW_H
