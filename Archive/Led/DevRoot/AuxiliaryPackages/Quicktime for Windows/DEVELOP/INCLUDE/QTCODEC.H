

// ---------------------------------------------------------------------
//
// QTCODEC.H - QuickTime for Windows C and C++ Bindings
//
//             Version 2.0
//
//             (c) Copyright 1988-1994 Apple Computer, Inc. All Rights Reserved.
//
// ---------------------------------------------------------------------


#if !defined __QTCODEC_H
#define __QTCODEC_H

// Header files
#include <qtw.h>
#include <compmgr.h>

// Macros
#ifdef _WIN32
	#define FIELDOFFSET(type, field)    ((int)(&((type NEAR*)1)->field)-1)
#endif
#define ADDOFFSET(a,b) (LPBYTE)((char HUGE *)(a)+(b))
#define SUBOFFSET(a,b) (LPBYTE)((char HUGE *)(a)-(b))
#define HUGESUM(a,b,c) ADDOFFSET(a,FIELDOFFSET(b,c))
#define ROUNDUP(a,b) (((a)+(b-1))&(~(b-1)))
#define ROUNDDOWN(a,b) ((a)&(~(b-1)))
#define ROUNDDOWNSLOW(a,b) ((a)-((a)%(b)))
#define LOCAL static NEAR
#define GlobalDeref(a) GlobalLock(a)

// 16-bit macro to determine if we are running on Windows 95
#ifndef _WIN32
    #define OS_WINNT 0x04
    #define bThisIsWin95() \
        (((GetWinFlags() >> 12) != OS_WINNT) \
        && (GetProcAddress(GetModuleHandle("KRNL386.EXE"), "REGSETVALUEEX") != NULL))
#endif


// Data types
typedef short * PSHORT ;
typedef short FAR * LPSHORT ;
typedef BOOL * PBOOL ;
typedef BOOL FAR * LPBOOL ;
#ifndef _WIN32
    #define HUGE _huge
#else
    #define HUGE
#endif
typedef char HUGE * HPSTR ;
typedef unsigned char HUGE * HPBYTE ;
typedef void HUGE * HPVOID ;
typedef LONG HUGE * HPLONG ;
typedef WORD ( FAR CDECL * HDWDISP)( ...) ;

// Data
enum VID_DISPATCH { VDSP_SETBANK      =  1
		  , VDSP_SLIDEWINDOW  =  2
		  , VDSP_SAVECONTEXT  =  3
		  , VDSP_RESTCONTEXT  =  4
		  , VDSP_SETTARGET    =  5
		  , VDSP_IDENTIFY     = 21
		  , VDSP_VERSION      = 22
		  , VDSP_BANKTABLE    = 23
		  , VDSP_BITBLTTYPE   = 24
		  , VDSP_SCANWIDTH    = 25
          , VDSP_LINEAR_BUF   = 26
		  , VDSP_READWRITE    = 41
		  , VDSP_TERMINATE    = 86
		  } ;
enum OPT_TYPE { OPT_UNINIT = 0              // field uninitialized
	      , OPT_RAW    = 1              // decompress to raw buffer
	      , OPT_BMP    = 2              // decompress to BMP
	      , OPT_HDW    = 3              // decompress to video adapter
	      } ;
enum BMP_TYPE { BMP_NONE      =  0          // unknown type
	      , BMP_DIB       =  1          // DIB
	      , BMP_MONO      =  2          // monochrome
	      , BMP_PACKED_4  =  3          // packed 4 bit, e.g., Fahrenheit
	      , BMP_PLANAR_4  =  4          // VGA or SVGA
	      , BMP_INDEX_8   =  5          // palettized driver
	      , BMP_5_5_5     =  6          // 32,768 colors
	      , BMP_5_6_5     =  7          // XGA, Intel order
	      , BMP_PLANAR_16 =  8          // two planes of one byte each
	      , BMP_8_8_8_RGB =  9          // true color RGB
	      , BMP_MEMERR    = 10          // insufficient memory for buffers
	      , BMP_8_8_8_BGR = 11          // true color BGR
	      , BMP_5_6_5_M   = 12          // XGA, Motorola order
	      , BMP_aRGB32    = 13          // 32-bit with alpha channel
	      , BMP_RGBa32    = 14          // 32-bit with alpha channel
	      , BMP_aBGR32    = 15          // 32-bit with alpha channel
	      , BMP_BGRa32    = 16          // 32-bit with alpha channel
	      
					    // note below-16:32 selectors are not the base of a
					    // selectorArray, therefore huge ptr arithmetic is not valid

	      , BMP_IF09            = 17    // YVU9 with difference bits (16:32 selector,linear frame buffer)
	      , BMP_YVU9_PLANAR     = 18    // YVU9 (16:32 selector,linear frame buffer)
	      , BMP_YVU12_PLANAR    = 19    // YVU12 (16:32 selector,linear frame buffer)
	      , BMP_YVU9_PACKED     = 20    // YVU9 (16:32 selector,linear frame buffer)
	      , BMP_YVU12_PACKED    = 21    // YVU12 (16:32 selector,linear frame buffer)
	      , BMP_UYVY            = 22    // YUV 4:2:2 (16:32 selector,linear frame buffer) 
	      , BMP_YUV_411         = 23    // YUV_411 (16:32 selector,linear frame buffer) 
	      } ;
typedef LONG ImageSequence ;
// We regard as a limiting case a 1,600 x 1,200 screen with 3 bytes
// per pixel and a pixel pitch that is a power of 2, thus 8,192.
// This gives a bank break every 8 lines, for a total of 150 bank
// breaks.
#define MAXBANKBREAKS 150
#if !defined _MATRIX
#define _MATRIX
typedef LONG LFRACT ;
#define MAKELFRACT(value, fract) ((((DWORD)(fract))>>2) | (((DWORD)((WORD)(value))) << 30))
typedef struct {                            // Hungarian: mtrx
  struct {
    LFIXED lfxCol1 ;
    LFIXED lfxCol2 ;
    LFRACT lfrCol3 ;
  } Row[ 3] ;
} MATRIX, FAR * LPMATRIX ;
#endif                                      // !defined _MATRIX
#define ostypeAPPL  QTFOURCC( 'a', 'p', 'p', 'l')
#define ostypeCVID  QTFOURCC( 'c', 'v', 'i', 'd')
#define ostypeDCMP  QTFOURCC( 'd', 'c', 'm', 'p')
#define ostypeJPEG  QTFOURCC( 'j', 'p', 'e', 'g')
#define ostypeRAW   QTFOURCC( 'r', 'a', 'w', ' ')
#define ostypeRLE   QTFOURCC( 'r', 'l', 'e', ' ')
#define ostypeRPZA  QTFOURCC( 'r', 'p', 'z', 'a')
#define ostypeRT21  QTFOURCC( 'r', 't', '2', '1')
#define ostypeIV31  QTFOURCC( 'i', 'v', '3', '1')
#define ostypeIV32  QTFOURCC( 'i', 'v', '3', '2')
#define ostypeSMC   QTFOURCC( 's', 'm', 'c', ' ')

/* Codec function selectors
   Codec selectors 0-127 are reserved by Apple
   Codec selectors 128-191 are subtype specific
   Codec selectors 192-255 are vendor specific
   Codec selectors 256-32767 are reserved by Apple
   Negative selectors are reserved by the Component Manager
*/
enum CODEC_SELECTOR { codecGetCodecInfo           =  0
		    , codecGetCompressionTime     =  1
		    , codecGetMaxCompressionSize  =  2
		    , codecPreCompress            =  3
		    , codecBandCompress           =  4
		    , codecPreDecompress          =  5
		    , codecBandDecompress         =  6
		    , codecCDSequenceBusy         =  7
		    , codecGetCompressedImageSize =  8
		    , codecGetSimilarity          =  9
		    , codecTrimImage              = 10
		    } ;


typedef struct tagCODECCAPABILITIES {
  LONG     lFlags ;
  WORD     wWantedPixelSize ;
  WORD     wExtendWidth ;                   // extra bytes after last row
  WORD     wExtendHeight ;                  // number of rows per compress unit
  WORD     wBandMin ;
  WORD     wBandInc ;
  WORD     wPad ;
  WORD     wTime ;
					    // new fields for Windows version
  OPT_TYPE optt ;                           // the supported optimization
} CODECCAPABILITIES, FAR * LPCODECCAPABILITIES ;

typedef struct tagMASKBITS  {
  LPVOID lpBits ;                           // pointer to the bits
  WORD wRowBytes ;                          // pixel pitch or BMP pitch
  RECT rectBounds ;                         // clip rectangle
} MASKBITS, FAR * LPMASKBITS ;

// flags for lFlags in CODECCAPABILITIES
#define codecCanScale             (1L <<  0)
#define codecCanMask              (1L <<  1)
#define codecCanMatte             (1L <<  2)
#define codecCanTransform         (1L <<  3)
#define codecCanTransferMode      (1L <<  4)
#define codecCanCopyPrev          (1L <<  5)
#define codecCanSpool             (1L <<  6)
#define codecCanClipVertical      (1L <<  7)
#define codecCanClipRectangular   (1L <<  8)
#define codecCanRemapColor        (1L <<  9)
#define codecCanFastDither        (1L << 10)
#define codecCanSrcExtract        (1L << 11)
#define codecCanCopyPrevComp      (1L << 12)
#define codecCanAsync             (1L << 13)
#define codecCanMakeMask          (1L << 14)
#define codecCanShift             (1L << 15)

// flags for lConditionFlags in CODECDECOMPRESSPARAMETERS
#define codecConditionFirstBand         (1L <<  0)
#define codecConditionLastBand          (1L <<  1)
#define codecConditionCodecChangedMask  (1L << 31)

#if defined(_MSC_VER)
    #pragma pack(1)
#else
    #error check structure packing
#endif

typedef struct tagIMCLUTDATA {
  short     sIndex ;
  short     sRed ;
  short     sGreen ;
  short     sBlue ;
} IMCLUTDATA, FAR * LPIMCLUTDATA ;

typedef struct tagIMAGECLUT {
  short       clutID ;
					  // remaining fields are optional
  LONG        clutSeed ;
  short       clutFlags ;
  short       clutSize ;
  IMCLUTDATA    clutData[ 1] ;
} IMAGECLUT, FAR * LPIMAGECLUT ;

#pragma pack()

typedef struct tagFULLIMAGEDESCRIPTION {              
    // These first fields must match IMAGEDESCRIPTION exactly (see qtw.h).
    LONG  idSize;                       // structure size
    DWORD CodecType;                    // 'rpza', 'jpeg', 'rle ', 'raw ', 'smc', 'cvid'
    DWORD resvd1;                       //
    WORD  resvd2;                       // always 0
    WORD  dataRefIndex;                 // always 1
    WORD  version;                      //
    WORD  revLevel;                     //
    DWORD vendor;                       // 'appl' or other vendor
    DWORD temporalQuality;              //
    DWORD spatialQuality;               //
    WORD  width;                        // Source image width in pixels
    WORD  height;                       // Source image height in pixels
    LFIXED hRes;                        // Horizontal resolution (eg: 72.0)
    LFIXED vRes;                        // Vertical resolution (eg: 72.0)
    DWORD dataSize;                     // Memory required for image data
    WORD  frameCount;                   // always 0
    char  name[32];                     // Compression algorithm (eg: Animation)
    WORD  depth;                        // Pixel depth of source image
    WORD  clutID;                       // Macintosh ROM Color table ID
  
    // These last fields are additional data from the sample description.
    // This is a variable length addition!
    IMAGECLUT sampleClut;               // imageClut
  
} FULLIMAGEDESCRIPTION, FAR * LPFULLIMAGEDESCRIPTION ;

typedef struct tagCODECDECOMPRESSPARAMS {
  LONG lSequenceID ;                        // number of first frame
  ImageDescription FAR * lpim ;             // pointer to image description
  LPVOID lpCompressed ;                     // pointer to compressed data
  LONG lBufferSize ;                        // no image buffer size
  LONG lFrameNumber ;                       // within the sequence
  LONG lStartLine ;                         // starting line of band
  LONG lStopLine ;                          // ending line of band
  LONG lConditionFlags ;                    // how component has been called
  LONG lCallerFlags ;                       // further control information
  LPCODECCAPABILITIES lpCapabilities ;      // filled out by PreDecompress
  LPVOID lpvProgressProc ;                  // no progress proc
  LPVOID lpvCompletionProc ;                // no completion proc
  LPVOID lpvDataProc ;                      // no data loading proc
  HDC hdc ;                                 // instead of CGrafPtr
  LPVOID lpUncompressed ;                   // pointer to offscreen buffer
  LPMASKBITS lpMaskBits ;                   // mask bits structure
  LPVOID lpMattePixMap ;                    // blend matte (unused)
  RECT rectSample ;                         // subset of sample to decompress
  LPMATRIX lpMatrix ;                       // target transformation matrix
  BYTE bAccuracy ;                          // requested accuracy
  BYTE bAlign ;                             // for alignment only
  WORD wRop2 ;                              // usually R2_COPYPEN
  WORD wMatrixType ;                        // identify 0.5X, 2X, 4X
  RECT rectDest ;                           // destination rectangle
  // new fields for Windows version
  LONG lCompressedSize ;                    // size of compressed data
  BMP_TYPE bmpt ;                           // BMP type
  LPVOID lpDitherTable ;                    // pointer to dither table
  WORD wClutSeed ;                          // track CLUT changes
  OPT_TYPE optt ;                           // optimization type
  HDWDISP lpvHardware ;                     // addr of adapter dispatch routine
  LPWORD lpwBankBoundaries ;                // adapter's bank boundary table
  LPWORD lpwBitSmear ;                      // bit smearing table
  WORD wRowsPerFullSegment ;                // banding will obsolete this
  LPVOID lpOffScreen ;                      // offscreen handling of bank splits
  DWORD dwReserved[ 1] ;                    // room for expansion
  OSType sourceDataType;
  Handle sourceDataDesc;
  LPVOID sourceDataPtr;    
}  CODECDECOMPRESSPARAMS, FAR * LPCODECDECOMPRESSPARAMS ;

typedef struct tagCODECINFO {
  char szTypeName[ 32] ;                    // name of codec type
  WORD wVersion ;
  WORD wRevLevel ;
  DWORD dwVendor ;
  DWORD dwDecompressFlags ;
  DWORD dwCompressFlags ;
  DWORD dwFormatFlags ;
  BYTE bDecompressionAccuracy ;
  WORD wCompressionSpeed ;
  WORD wDecompressionSpeed ;
  BYTE bCompressionLevel ;
  BYTE bReserved ;
  WORD wMinimumHeight ;
  WORD wMinimumWidth ;
  WORD wDecompressPipelineLatency ;
  WORD wCompressPipelineLatency ;
  DWORD dwPrivateData ;
}  CODECINFO, FAR * LPCODECINFO ;

// flags for dwComponentFlags in ComponentDescription structure
// flags for dwDecompressFlags and dwCompressFlags in CODECINFO
// The depths refer to target depths supported.
#define codecInfoDoes1           (1L <<  0)
#define codecInfoDoes4           (1L <<  1)
#define codecInfoDoes8           (1L <<  2)
#define codecInfoDoes16          (1L <<  3)
#define codecInfoDoes24          (1L <<  4)
#define codecInfoDoes32          (1L <<  5)
#define codecInfoDoesDither      (1L <<  6)
#define codecInfoDoesStretch     (1L <<  7)
#define codecInfoDoesShrink      (1L <<  8)
#define codecInfoDoesMask        (1L <<  9)
#define codecInfoDoesTemporal    (1L << 10)
#define codecInfoDoesDouble      (1L << 11)
#define codecInfoDoesQuad        (1L << 12)
#define codecInfoDoesHalf        (1L << 13)
#define codecInfoDoesQuarter     (1L << 14)
#define codecInfoDoesRotate      (1L << 15)
#define codecInfoDoesHorizFlip   (1L << 16)
#define codecInfoDoesVertFlip    (1L << 17)
#define codecInfoDoesSkew        (1L << 18)
#define codecInfoDoesBlend       (1L << 19)
#define codecInfoDoesWarp        (1L << 20)
#define codecInfoDoesRecompress  (1L << 21)
#define codecInfoDoesSpool       (1L << 22)

// flags for dwFormatFlags in CODECINFO
// The depths refer to source depths supported.
#define codecInfoDepth1          (1L <<  0)
#define codecInfoDepth2          (1L <<  1)
#define codecInfoDepth4          (1L <<  2)
#define codecInfoDepth8          (1L <<  3)
#define codecInfoDepth16         (1L <<  4)
#define codecInfoDepth24         (1L <<  5)
#define codecInfoDepth32         (1L <<  6)
#define codecInfoDepth33         (1L <<  7)
#define codecInfoDepth34         (1L <<  8)
#define codecInfoDepth36         (1L <<  9)
#define codecInfoDepth40         (1L << 10)
#define codecInfoStoresClut      (1L << 11)
#define codecInfoDoesLossless    (1L << 12)
#define codecInfoSeqSensitive    (1L << 13)

// Functions
#if defined __cplusplus
extern "C" {
#endif
ComponentResult QTAPI cfBandDecompress( STKOFF_CMP so
				      , LPVOID lpvStorage
				      , LPCODECDECOMPRESSPARAMS lpdecomp
				      ) ;
ComponentResult QTAPI cfCanDoSelect( STKOFF_CMP so, LONG lFunctionSelector) ;
ComponentResult QTAPI cfCloseSelect( STKOFF_CMP so, ComponentInstance ci) ;
ComponentResult QTAPI cfGetCodecInfo( STKOFF_CMP so
				    , LPVOID lpvStorage
				    , LPCODECINFO lpinfo
				    ) ;
ComponentResult QTAPI cfOpenSelect( STKOFF_CMP so, ComponentInstance ci) ;
ComponentResult QTAPI cfPreDecompress( STKOFF_CMP so
				     , LPVOID lpvStorage
				     , LPCODECDECOMPRESSPARAMS lpdecomp
				     ) ;
ComponentResult QTAPI cfRegisterSelect( STKOFF_CMP so, ComponentInstance ci) ;
ComponentResult QTAPI cfSequenceBusy( STKOFF_CMP so
				    , LPVOID lpvStorage
				    , ImageSequence seq
				    ) ;
ComponentResult QTAPI cfTargetSelect( STKOFF_CMP so, ComponentInstance ci) ;
ComponentResult QTAPI cfVersionSelect( STKOFF_CMP so, ComponentInstance ci) ;

DWORD QTAPI CodecEntry( VOID) ;
WORD QTAPI Flip16( WORD wValue) ;
VOID QTAPI Flip16Many( LPVOID lpSource, DWORD dwNbrWords) ;
LONG QTAPI Flip32( LONG lValue) ;
VOID QTAPI Flip32Many( LPVOID lpSource, DWORD dwNbrDwords) ;
BOOL QTAPI FreeMemory( LPVOID lpvMem) ;
LPVOID QTAPI GetMemory( LONG lSize) ;
LPVOID QTAPI ReallocateMemory( LPVOID lpvMem, LONG lSize) ;
#ifndef _WIN32
    VOID QTAPI CopyMemory( HPVOID hpvDest, HPVOID hpvSource, LONG cbCopy) ;
#endif
OSType FAR PASCAL THNGIDENTIFY( LPCID FAR *lplpcid) ;
VOID QTAPI VidComputeBankBoundaries1( LPWORD lpSourceTable
				    , LPWORD lpTargetTable
				    , WORD wLeft
				    , WORD wTop
				    , WORD wRight
				    , WORD wBottom
				    ) ;
VOID QTAPI VidComputeBankBoundaries2( LPWORD lpSourceTable
				    , LPWORD lpTargetTable
				    , WORD wLeft
				    , WORD wTop
				    , WORD wRight
				    , WORD wBottom
				    ) ;
VOID QTAPI VidComputeBankBoundaries4( LPWORD lpSourceTable
				    , LPWORD lpTargetTable
				    , WORD wLeft
				    , WORD wTop
				    , WORD wRight
				    , WORD wBottom
				    ) ;
VOID QTAPI VidComputeBankBoundaries81( LPWORD lpSourceTable
				     , LPWORD lpTargetTable
				     , WORD wLeft
				     , WORD wTop
				     , WORD wRight
				     , WORD wBottom
				     ) ;
VOID QTAPI VidComputeBankBoundaries82( LPWORD lpSourceTable
				     , LPWORD lpTargetTable
				     , WORD wLeft
				     , WORD wTop
				     , WORD wRight
				     , WORD wBottom
				     ) ;
#if defined __cplusplus
}                                           // defined __cplusplus
#endif

#endif                                      // !defined __QTCODEC_H
