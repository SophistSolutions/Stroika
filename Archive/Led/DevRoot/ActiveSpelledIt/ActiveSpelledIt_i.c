

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Wed Apr 09 23:22:00 2008
 */
/* Compiler settings for .\Sources\ActiveSpelledIt.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IActiveSpelledItCtl,0xD3241A98,0x5959,0x4414,0x99,0x8F,0xEF,0x1C,0xFB,0xED,0xF1,0xA5);


MIDL_DEFINE_GUID(IID, IID_IActiveSpelledIt_WordInfo,0xB86F5832,0x5874,0x4952,0xB8,0x8C,0xA2,0x7C,0xC8,0x20,0xB9,0x76);


MIDL_DEFINE_GUID(IID, IID_IActiveSpelledIt_ScanContext,0x8CBD536B,0x9816,0x4A2E,0x98,0xBD,0x57,0x61,0xDA,0xE6,0xD3,0x79);


MIDL_DEFINE_GUID(IID, LIBID_ActiveSpelledItLib,0x2C7971C0,0x67BB,0x4CA9,0x90,0x98,0x25,0xD4,0x68,0xDF,0x74,0x4D);


MIDL_DEFINE_GUID(CLSID, CLSID_ActiveSpelledItCtl,0x2D8F976C,0x2869,0x49D4,0xBC,0xC7,0xB1,0x55,0x5E,0xA7,0x6C,0x2C);


MIDL_DEFINE_GUID(CLSID, CLSID_BOGUS_TO_GET_INTERFACES_EXPORTED,0x93890BC2,0xF754,0x404E,0xA1,0xDA,0x12,0xAA,0x7E,0x3A,0xB8,0x9A);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



