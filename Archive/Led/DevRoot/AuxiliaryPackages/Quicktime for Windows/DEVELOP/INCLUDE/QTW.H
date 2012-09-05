

// ---------------------------------------------------------------------
//
// QTW.H - QuickTime for Windows C and C++ Bindings
//
//         Version 2.0
//
//         (c) Copyright 1988-1994 Apple Computer, Inc. All Rights Reserved.
//
// ---------------------------------------------------------------------


// Prevent multiple inclusion
// --------------------------
#ifndef __QTW_H__
#define __QTW_H__

#ifdef QTW_PASCAL
    #undef QTW_PASCAL
#endif
#ifdef _WIN32
    #define QTW_PASCAL __cdecl
#else
    #define QTW_PASCAL PASCAL
#endif




// Movie Controller MCDoAction action constants
// --------------------------------------------
typedef enum
  {mcActionIdle                   =  1, // No Param
   mcActionDraw                   =  2, // No Param
   mcActionActivate               =  3, // No param
   mcActionDeactivate             =  4, // No param
   mcActionMouseDown              =  5, // Param is pointer to EventRecord
   mcActionKey                    =  6, // Param is pointer to MSG
   mcActionPlay                   =  8, // Param is LFIXED play rate
   mcActionGoToTime               = 12, // Param is pointer to TimeRecord
   mcActionSetVolume              = 14, // Param is SFIXED volume
   mcActionGetVolume              = 15, // Param is pointer to SFIXED volume
   mcActionStep                   = 18, // Param is LONG number of steps
   mcActionSetLooping             = 21, // Param is a Boolean
   mcActionGetLooping             = 22, // Param is pointer to a Boolean
   mcActionSetLoopIsPalindrome    = 23, // Param is Boolean
   mcActionGetLoopIsPalindrome    = 24, // Param is pointer to a Boolean
   mcActionSetGrowBoxBounds       = 25, // Param is pointer to a Rect
   mcActionControllerSizeChanged  = 26, // No param
   mcActionSetSelectionBegin      = 29, // Param is pointer to TimeRecord
   mcActionSetSelectionDuration   = 30, // Param is pointer to TimeRecord
   mcActionSetKeysEnabled         = 32, // Param is Boolean
   mcActionGetKeysEnabled         = 33, // Param is pointer to Boolean
   mcActionSetPlaySelection       = 34, // Param is Boolean
   mcActionGetPlaySelection       = 35, // Param is pointer to Boolean
   mcActionSetUseBadge            = 36, // Param is Boolean
   mcActionGetUseBadge            = 37, // Param is pointer to Boolean
   mcActionSetFlags               = 38, // Param is LONG flags
   mcActionGetFlags               = 39, // Param is pointer to LONG flags
   mcActionSetPlayEveryFrame      = 40, // Param is Boolean
   mcActionGetPlayEveryFrame      = 41, // Param is pointer to Boolean
   mcActionGetPlayRate            = 42, // Param is pointer to LFIXED
   mcActionBadgeClick             = 44, // Param is pointer to Boolean
   mcActionMovieClick             = 45, // Param is pointer to event structure
   mcActionGetTimeSliderRect      = 49, // Param is pointer to RECT
   mcActionGetSelectionBegin      = 53, // Param is pointer to TimeRecord
   mcActionGetSelectionDuration   = 54  // Param is pointer to TimeRecord
  } mcActionType;


// Movie Controller MCDoAction, action McActionSetFlags parameter, movie
// controller structure parameter, lfActionFlags
// ---------------------------------------------------------------------
typedef enum
  {mcFlagSuppressStepButtons      = 1<<1,
   mcFlagSuppressSpeakerButton    = 1<<2,
   mcFlagsUseWindowPalette        = 1<<3
  } mcActionFlags;


// Toolbox function NewMovieController and Movie Controller function
// MCPositionController parameter controllerCreationFlags values,
// movie controller structure parameter, lfControllerStateFlags
// -----------------------------------------------------------------
typedef enum
  {mcTopLeftMovie                 = 1<<0,
   mcScaleMovieToFit              = 1<<1,
   mcWithBadge                    = 1<<2,
   mcNotVisible                   = 1<<3
  } mcControllerStateFlags;


// Movie Controller function MCGetControllerInfo parameter mcInfoFlags
// parameter, movie controller structure parameter, lfControllerInfoFlags
// ----------------------------------------------------------------------
typedef enum
  {mcInfoHasSound                 = 1<<5,
   mcInfoIsPlaying                = 1<<6,
   mcInfoIsLooping                = 1<<7,
   mcInfoIsInPalindrome           = 1<<8
  } mcControllerInfoFlags;


// NewMovieFromFile flags
// ----------------------
typedef enum
  {newMovieActive                 = 1<<0,
   newMovieDontResolveDataRefs    = 1<<1,
   newMovieDontAskUnresolvedDataRefs = 1<<2,
   newMovieDontAutoAlternates     = 1<<3
  } newMovieFlags;


// Flags for SetMoviePlayHints
// ---------------------------
typedef enum
  {hintsScrubMode                 = 1<<0,
   hintsLoop                                  = 1<<1,
   hintsInterpolateSound          = 1<<7,
   hintsInactive                  = 1<<11
  } moviePlayHints;


// Flags for MovieSearchText
// -------------------------

#define findTextEdgeOK                (1L << 0)
#define findTextCaseSensitive         (1L << 1)
#define findTextReverseSearch         (1L << 2)
#define findTextWrapAround            (1L << 3)
#define findTextUseOffset             (1L << 4)
#define searchTextDontGoToFoundTime   (1L << 16)
#define searchTextDontHiliteFoundText (1L << 17)
#define searchTextOneTrackOnly        (1L << 18) /*ignored*/
#define searchTextEnabledTracksOnly   (1L << 19)


// Flags for GetMovieIndTrackType
typedef enum
  {movieTrackMediaType        = 1<<0,
   movieTrackCharacteristic   = 1<<1,
   movieTrackEnabledOnly      = 1<<2
  } movieIndTrackTypeFlags;

#define VideoMediaType  QTFOURCC('v','i','d','e')
#define SoundMediaType  QTFOURCC('s','o','u','n')
#define TextMediaType   QTFOURCC('t','e','x','t')
#define MPEGMediaType   QTFOURCC('m','p','e','g')
#define MusicMediaType  QTFOURCC('m','u','s','i')

// Manifest constants for ostypes as flags for Component Manager calls
// -------------------------------------------------------------------
#define kAnyComponentType           0
#define kAnyComponentSubType        0
#define kAnyComponentManufacturer   0
#define kAnyComponentFlagsMask      0


// SetDefaultComponent Flags
// -------------------------
typedef enum
  {defaultComponentIdentical        = 0,
   defaultComponentAnyFlags         = 1,
   defaultComponentAnyManufacturer  = 2,
   defaultComponentAnySubType       = 4
  } defaultComponentFlags;


// RegisterComponent Flags
// -----------------------
typedef enum
  {registerComponentGlobal          = 1,
   registerComponentNoDuplicates    = 2,
   registerComponentAfterExisting   = 4
  } registerComponentFlags;

// putMovieIntoTypedHandle
// -----------------------
typedef enum
  {
    pmithHandleIsPointerToHandle    = 1
  } pmithFlags;


// Error codes
// -------------
enum
  {
   // Shared by QTW and QT Mac
   readErr                        = -19,
   paramErr                       = -50,
   noTypeErr                      = -102,
   insufficientMemory             = -108,
   memWZErr                       = -111,
   notLockedErr                   = -623,
   couldNotResolveDataRef         = -2000,
   badImageDescription            = -2001,
   badPublicMovieAtom             = -2002,
   cantFindHandler                = -2003,
   cantOpenHandler                = -2004,
   badComponentType               = -2005,
   noMediaHandler                 = -2006,
   noDataHandler                  = -2007,
   invalidMedia                   = -2008,
   invalidTrack                   = -2009,
   invalidMovie                   = -2010,
   invalidSampleTable             = -2011,
   invalidDataRef                 = -2012,
   invalidHandler                 = -2013,
   invalidDuration                = -2014,
   invalidTime                    = -2015,
   cantPutPublicMovieAtom         = -2016,
   badEditList                    = -2017,
   mediaTypesDontMatch            = -2018,
   progressProcAborted            = -2019,
   movieToolboxUninitialized      = -2020,
   wffileNotFound                 = -2021,
   cantCreateSingleForkFile       = -2022,
   invalidEditState               = -2023,
   nonMatchingEditState           = -2024,
   staleEditState                 = -2025,
   userDataItemNotFound           = -2026,
   maxSizeToGrowTooSmall          = -2027,
   badTrackIndex                  = -2028,
   trackIDNotFound                = -2029,
   trackNotInMovie                = -2030,
   timeNotInTrack                 = -2031,
   timeNotInMedia                 = -2032,
   badEditIndex                   = -2033,
   internalQuickTimeError         = -2034,
   cantEnableTrack                = -2035,
   invalidRect                    = -2036,
   invalidSampleNum               = -2037,
   invalidChunkNum                = -2038,
   invalidSampleDescIndex         = -2039,
   invalidChunkCache              = -2040,
   invalidSampleDescription       = -2041,
   dataNotOpenForRead             = -2042,
   dataNotOpenForWrite            = -2043,
   dataAlreadyOpenForWrite        = -2044,
   dataAlreadyClosed              = -2045,
   endOfDataReached               = -2046,
   dataNoDataRef                  = -2047,
   noMovieInDataFork              = -2048,
   invalidDataRefContainer        = -2049,
   badDataRefIndex                = -2050,
   noDefaultDataRef               = -2051,
   couldNotUseAnExistingSample    = -2052,
   featureUnsupported             = -2053,
   noVideoTrackInMovie            = -2054,
   noSoundTrackInMovie            = -2055,
   movieTextNotFound              = -2062,

   // -2150 through -2200 reserved for QTW
   soundSupportNotAvailable       = -2150,
   maxControllersExceeded         = -2152,
   unableToCreateMCWindow         = -2153,
   invalidUserDataHandle          = -2154,
   noPictureInFile                = -2155,
   invalidPictureFileHandle       = -2156,
   invalidPictureHandle           = -2157,
   badDisplayContext              = -2158,
   noMusicTrackInMovie            = -2159,
   noTextTrackInMovie             = -2160,
   noMPEGTrackInMovie             = -2161,

   // Shared by QTW and QT Mac
   invalidComponentID             = -3000,
   validInstancesExist            = -3001,
   componentNotCaptured           = -3002,
   componentDontRegister          = -3003,

   // Odds and ends used by QTW only
   noErr                          = 0,
   mcOK                           = 0,
   mcEventNotHandled              = 0,
   mcEventHandled                 = 1,
   codecConditionErr              = -8972,
   controllerBoundsNotExact       = -9996,
   editingNotAllowed              = -9995
  };


// QTInitialize error codes
// ------------------------
enum
  {QTI_OK                         = 0,
   QTI_FAIL_NOEXIST               = 1,
   QTI_FAIL_CORRUPTDLL            = 2,
   QTI_FAIL_286                   = 3,
   QTI_FAIL_WIN30                 = 4
  };


// Component Manager error codes
// -----------------------------
#define badComponentInstance    0x80008001
#define badComponentSelector    0x80008002


// Special MCI Interface
// ---------------------
#define MCI_QTW_PLAY_WINDOW     0x01000000L
#define MCI_QTW_PLAY_FULLSCREEN 0x02000000L
#define MCI_QTW_SET_USE_MC      0x00100000L
#define MCI_QTW_SET_PAL_ASSERT  0x00200000L
#define MCI_QTW_SET_GROWBOX     0x00400000L


// Declared handles
// ----------------
DECLARE_HANDLE(UserData);              // Hungarian: ud
DECLARE_HANDLE(PicHandle);             // Hungarian: ph
DECLARE_HANDLE(DIBHandle);             // Hungarian: dh
DECLARE_HANDLE(PicFile);               // Hungarian: pic
DECLARE_HANDLE(MovieFile);             // Hungarian: mov

// Typedef's
// ---------
typedef LONG ComponentResult;          // Hungarian: cr
typedef LONG OSErr;                    // Hungarian: oserr
typedef LONG OSType;                   // Hungarian: ostype
typedef void far *ProgressProcRecordPtr; // Hungarian: pprp
typedef LONG TimeScale;                // Hungarian: ts
typedef DWORD TimeValue;               // Hungarian: tv
typedef LONG LFIXED;                   // Hungarian: lfx
typedef LONG Fixed;                    // Hungarian: fx
typedef short SFIXED;                  // Hungarian: sfx
typedef BOOL Boolean;
typedef HWND WindowPtr;
typedef LPBYTE Ptr;
typedef void far *AlignmentProcRecordPtr;

#ifndef RC_INVOKED
    #if defined(_MSC_VER) || defined(__BORLANDC__)
	#pragma pack(1)
    #else
	#error check structure packing options for your compiler
	// search below for "#pragma pack" to find all the places
	// to change for your compiler's pragma.
    #endif
#endif

struct Point {
    short                       v;
    short                       h;
};
typedef struct Point Point;

typedef struct _tagRect {
   short        boxTop ;            // default text box
   short        boxLeft ;
   short        boxBottom ;
   short        boxRight ;
} Rect;
#if !defined(RC_INVOKED) && (defined(_MSC_VER) || defined(__BORLANDC__))
    #pragma pack()
#endif

enum  {
    nullEvent                   = 0,
    mouseDown                   = 1,
    mouseUp                     = 2,
    keyDown                     = 3,
    keyUp                       = 4,
    autoKey                     = 5,
    updateEvt                   = 6,
    diskEvt                     = 7,
    activateEvt                 = 8,
    osEvt                       = 15,
/* event mask equates */
    mDownMask                   = 2,
    mUpMask                     = 4,
    keyDownMask                 = 8,
    keyUpMask                   = 16,
    autoKeyMask                 = 32,
    updateMask                  = 64,
    diskMask                    = 128,
    activMask                   = 256,
    highLevelEventMask          = 1024,
    osMask                      = -32768
};

struct EventRecord {
    short                       what;
    long                        message;
    long                        when;
    Point                       where;
    short                       modifiers;
};
typedef struct EventRecord EventRecord;

typedef struct _tagInt64               // Hungarian: qw (quad word)
  {DWORD dwLo;
   LONG  dwHi;
  } Int64;

#if !defined(RC_INVOKED) && (defined(_MSC_VER) || defined(__BORLANDC__))
    #pragma pack(1)
#endif
typedef struct _tagOpenCPicParms       // Hungarian: ocp
  {Rect   rect;
   LFIXED hRes;
   LFIXED vRes;
   WORD   wVersion;
   WORD   wReserved1;
   DWORD  dwReserved2;
  } OpenCPicParams;

typedef struct _tagTimeBaseRecord      // Hungarian: tbr
  {DWORD FAR *p;
  } TimeBaseRecord;

typedef TimeBaseRecord FAR *TimeBase;  // Hungarian: tb

#define TIMEBASE_DEFAULT ((TimeBase) 0L)

typedef struct _tagTimeRecord          // Hungarian: tr
  {Int64     value;
   TimeScale scale;
   TimeBase  base;
  } TimeRecord;

typedef struct MatrixRecord {          // Hungarian: mrec
    Fixed matrix[3][3];
} MatrixRecord;

typedef struct _tagSoundDescription    // Hungarian: sd (SoundDescription)
  {LONG  descSize;                     // structure size
   DWORD dataFormat;                   // 'raw ','twos','ima4'
   DWORD resvd1;                       // always 0
   WORD  resvd2;                       // always 0
   WORD  dataRefIndex;                 // always 1
   WORD  version;                      // always 0
   WORD  revLevel;                     // always 0
   DWORD vendor;                       // always 0
   WORD  numChannels;                  // 1=mono; 2=stereo
   WORD  sampleSize;                   // 8=8bit sound; 16=16bit sound
   WORD  compressionID;                // always 0
   WORD  packetSize;                   // always 0
   LFIXED sampleRate;                  // eg: 44100.0000 per second
  } SoundDescription;

typedef struct _tagMusicDescription    // Hungarian: md (MusicDescription)
  {LONG  descSize;                     // structure size
   DWORD dataFormat;                   // always 'musi'
   DWORD resvd1;                       // always 0
   WORD  resvd2;                       // always 0
   WORD  dataRefIndex;                 // always 1
   DWORD musicFlags;                   // always 0
  } MusicDescription;

typedef struct _tagRGBColor {
    unsigned short  red;             /*magnitude of red component*/
    unsigned short  green;          /*magnitude of green component*/
    unsigned short  blue;           /*magnitude of blue component*/
} RGBColor;

typedef unsigned char Style;

typedef struct _tagScrpSTElement {
    long                        scrpStartChar;                  /*starting character position*/
    short                       scrpHeight;                     /*starting character position*/
    short                       scrpAscent;
    short                       scrpFont;
    Style                       scrpFace;                       /*unpacked byte*/
    char                        filler;                         /*scrpFace is unpacked byte*/
    short                       scrpSize;
    RGBColor                    scrpColor;
} ScrpSTElement;

typedef struct _tagTextDescription {
   long         descSize;
   long         dataFormat;             // = 'text'
   long         resvd1;
   short        resvd2;
   short        dataRefIndex;
   long         displayFlags;
   long         textJustification;
   RGBColor     bgColor ;               // red, green, blue
   Rect         defaultTextBox;
   ScrpSTElement defaultStyle;          // default style
   char         defaultFontName[ 1] ;   // pascal string ... struct is sized to fit the string
} TextDescription;

typedef struct _tagMPEGDescription {
    long descSize;
    long dataFormat;
    long resvd1;
    short resvd2;
    short dataRefIndex;
} MPEGDescription;

typedef struct _tagImageDescription    // Hungarian: id (ImageDescription)
  {LONG  idSize;                       // structure size
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
  } ImageDescription;
#if !defined(RC_INVOKED) && (defined(_MSC_VER) || defined(__BORLANDC__))
    #pragma pack()
#endif

typedef ComponentResult (FAR  CDECL *ComponentRoutine) (VOID);
typedef DWORD (FAR CDECL *ENTRYFUNC) (VOID);
typedef VOID (FAR QTW_PASCAL *FIXUPFUNC) (ENTRYFUNC);
typedef LONG Component;
typedef LONG ComponentInstance;

typedef struct tagComponentDescription { // Hungarian: cd
  OSType  ostypeComponentType;         // component identifier
  OSType  ostypeComponentSubType;      // sub-type
  OSType  ostypeComponentManufacturer; // vendor ID
  DWORD   dwComponentFlags;            // control flags
  DWORD   dwComponentFlagsMask;        // mask for control flags
  ComponentRoutine   crEntryPoint;     // pointer to entry point
  HRSRC   hrsrcName;                   // handle to name string
  HRSRC   hrsrcInfo;                   // handle to info string
  HRSRC   hrsrcIcon;                   // handle to icon
} ComponentDescription, FAR *LPCD;


// Macros
// ------
#define MAKELFIXED(integ, fract) ((LONG)(((WORD)(fract)) | (((DWORD)((WORD)(integ))) << 16)))

#define MAKESFIXED(integ, fract) ((int)(((BYTE)(fract)) | (((WORD)((BYTE)(integ))) << 8)))

#define QTFOURCC(ch0, ch1, ch2, ch3)                                \
		 ((DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |  \
		 ((DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ))


// ToolBox Callbacks and Handles
// -----------------------------
typedef const struct _tagQTHandle { int unused; } FAR *Handle;
typedef long Size;
typedef LPVOID Movie;
typedef LPVOID Media;
typedef LPVOID Track;
typedef Handle SampleDescriptionHandle;
typedef Handle PaletteHandle;
enum {
    pmAnimated  = 0x0004,   /*reserve an index on each device touched and render ciRGB.*/
};

typedef OSErr (CALLBACK *MovieRgnCoverProc) (Movie, HDC, LONG);


// Movie Controller Callbacks and Handles
// --------------------------------------
typedef ComponentInstance MovieController;
typedef BOOL (CALLBACK *MCActionFilter) (MovieController, UINT, LPVOID, LONG);

// Support C++ Function Invocations
// --------------------------------
#ifdef __cplusplus
  extern "C" {
#endif


// Function calling convention
// ---------------------------

#define QTAPI FAR CDECL

// Macintosh toolbox equivalents
// -----------------------------

Handle  QTAPI    NewHandle          ( Size byteCount);
LPVOID  QTAPI    DereferenceHandle  ( Handle theHandle);
void    QTAPI    HLock              ( Handle theHandle);
void    QTAPI    HUnlock            ( Handle theHandle);
Size    QTAPI    GetHandleSize      ( Handle theHandle);
OSErr   QTAPI    MemError           ( VOID);
void    QTAPI    SetHandleSize      ( Handle theHandle, Size newSize);
void    QTAPI    DisposeHandle      ( Handle theHandle);
signed char QTAPI  HGetState        ( Handle theHandle);
void    QTAPI    HSetState          ( Handle theHandle,signed char newState);


// Quicktime Functions
// -------------------



OSErr           QTAPI QTInitialize (LPLONG);
VOID            QTAPI QTTerminate (VOID);
VOID            QTAPI  AddTime ( TimeRecord FAR *, const TimeRecord FAR *);
VOID            QTAPI  AlignWindow ( WindowPtr wp, Boolean front, LPRECT aligmentRect,AlignmentProcRecordPtr alignmentProc);
VOID            QTAPI  ClearMoviesStickyError ( VOID);
OSErr           QTAPI  CloseMovieFile ( MovieFile);
OSErr           QTAPI  ClosePictureFile ( PicFile);
VOID            QTAPI  ConcatMatrix( const MatrixRecord FAR *srcMatrix,MatrixRecord FAR *destMatrix);
VOID            QTAPI  ConvertTimeScale ( TimeRecord FAR *, TimeScale);
LONG            QTAPI  CountUserDataType ( UserData, OSType);
OSErr           QTAPI  DeleteMovieFile ( LPCSTR);
VOID            QTAPI  DisposeMovie ( Movie);
VOID            QTAPI  DisposeMovieController ( MovieController);
VOID            QTAPI  DisposePicture ( PicHandle);
OSErr           QTAPI  DrawPicture ( HDC, PicHandle, const LPRECT, ProgressProcRecordPtr);
OSErr           QTAPI  DrawPictureFile ( HDC, PicFile, const LPRECT, ProgressProcRecordPtr);
OSErr           QTAPI  EnterMovies ( VOID);
VOID            QTAPI  ExitMovies ( VOID);
VOID            QTAPI  GetMediaHandlerDescription( Media,OSType FAR *,LPSTR,OSType FAR *);
short           QTAPI  GetMediaLanguage ( Media);
OSErr           QTAPI  GetMediaSample ( Media, Handle, long, long FAR *,
				TimeValue, TimeValue FAR *, TimeValue FAR *,
				SampleDescriptionHandle,
				long FAR *, long, long FAR *,
				short FAR *);
VOID            QTAPI  GetMediaSampleDescription( Media,LONG,SampleDescriptionHandle);
TimeScale       QTAPI  GetMediaTimeScale ( Media);
Track           QTAPI  GetMediaTrack ( Media);
Boolean         QTAPI  GetMovieActive ( Movie);
VOID            QTAPI  GetMovieActiveSegment ( Movie, TimeValue FAR *, TimeValue FAR *);
VOID            QTAPI  GetMovieBox ( Movie, LPRECT);
LONG            QTAPI  GetMovieCreationTime ( Movie);
LONG            QTAPI  GetMovieDataSize ( Movie, TimeValue, TimeValue);
TimeValue       QTAPI  GetMovieDuration ( Movie);
Track           QTAPI  GetMovieIndTrack ( Movie, LONG);
Track           QTAPI  GetMovieIndTrackType ( Movie, LONG, OSType, LONG);
VOID            QTAPI  GetMovieMatrix( Movie theMovie,MatrixRecord FAR *theMatrix);
LONG            QTAPI  GetMovieModificationTime ( Movie);
PicHandle       QTAPI  GetMoviePict ( Movie, TimeValue);
PicHandle       QTAPI  GetMoviePosterPict ( Movie);
TimeValue       QTAPI  GetMoviePosterTime ( Movie);
LFIXED          QTAPI  GetMoviePreferredRate ( Movie);
SFIXED          QTAPI  GetMoviePreferredVolume ( Movie);
VOID            QTAPI  GetMovieSelection ( Movie, TimeValue FAR *, TimeValue FAR *);
OSErr           QTAPI  GetMoviesError ( VOID);
OSErr           QTAPI  GetMoviesStickyError ( VOID);
ComponentResult QTAPI  GetMovieStatus ( Movie, LPVOID);
TimeValue       QTAPI  GetMovieTime ( Movie, TimeRecord FAR *);
TimeScale       QTAPI  GetMovieTimeScale ( Movie);
LONG            QTAPI  GetMovieTrackCount ( Movie);
UserData        QTAPI  GetMovieUserData ( Movie);
OSType          QTAPI  GetNextUserDataType ( UserData, OSType);
OSErr           QTAPI  GetPictureFileHeader ( PicFile, LPRECT, OpenCPicParams FAR *);
OSErr           QTAPI  GetPictureFileInfo ( PicFile, ImageDescription FAR *);
PicHandle       QTAPI  GetPictureFromFile ( PicFile);
OSErr           QTAPI  GetPictureInfo ( PicHandle, ImageDescription FAR *);
HPALETTE        QTAPI  GetPicturePalette ( PicHandle);
OSErr           QTAPI  GetSoundInfo ( Movie, SoundDescription FAR *);
VOID            QTAPI  GetTrackDimensions( Track theTrack,Fixed FAR *width,Fixed FAR *height);
Boolean         QTAPI  GetTrackEnabled ( Track th);
Media           QTAPI  GetTrackMedia ( Track);
VOID            QTAPI  GetTrackMatrix( Track theTrack,MatrixRecord FAR *theMatrix);
Movie           QTAPI  GetTrackMovie ( Track);
PaletteHandle   QTAPI  GetTrackPalette( Track);
PicHandle       QTAPI  GetTrackPict ( Track, TimeValue);
OSErr           QTAPI  GetUserData ( UserData, LPHANDLE, OSType, LONG, LPLONG);
OSErr           QTAPI  GetUserDataText ( UserData, LPHANDLE, OSType, LONG, UINT, LPLONG);
OSErr           QTAPI  GetVideoInfo ( Movie, ImageDescription FAR *);
VOID            QTAPI  KillPicture ( PicHandle);
OSErr           QTAPI  MovieSearchText ( Movie, Ptr, long, long, Track FAR *, TimeValue FAR *, long FAR *);
MovieController QTAPI  NewMovieController ( Movie, const LPRECT, LONG, HWND);
#ifndef _WIN32
 OSErr           QTAPI  NewMovieFromDataFork ( Movie FAR *, HFILE, LONG, UINT);
#else
 OSErr           QTAPI  NewMovieFromDataFork ( Movie FAR *, HANDLE, LONG, UINT);
#endif
OSErr           QTAPI  NewMovieFromFile ( Movie FAR *, MovieFile, LPINT, LPSTR, UINT, Boolean FAR *);
VOID            QTAPI  NormalizeRect ( LPRECT);
OSErr           QTAPI  OpenMovieFile ( LPCSTR, MovieFile FAR *, UINT);
OSErr           QTAPI  OpenPictureFile ( LPCSTR, PicFile FAR *, UINT);
DIBHandle       QTAPI  PictureToDIB ( PicHandle);
OSErr           QTAPI  PrerollMovie ( Movie, TimeValue, LFIXED);
Boolean         QTAPI  PtInMovie ( Movie, POINT);
Boolean         QTAPI  PtInTrack( Track theTrack,POINT thePoint);
OSErr           QTAPI  PutMovieIntoTypedHandle( Movie, Track,
				OSType, Handle,TimeValue,TimeValue,LONG,ComponentInstance);
VOID            QTAPI  SetEntryUsage( PaletteHandle dstPalette,short srcEntry, short srcUsage,short srcTolerance);                                
VOID            QTAPI  SetMovieActive ( Movie, Boolean);
VOID            QTAPI  SetMovieBox ( Movie, const LPRECT);
VOID            QTAPI  SetMovieCoverProcs ( Movie, MovieRgnCoverProc, MovieRgnCoverProc, LONG);
VOID            QTAPI  SetMoviePlayHints ( Movie, LONG, LONG);
OSErr           QTAPI  SetTrackEnabled ( Track th, Boolean);
VOID            QTAPI  SetTrackFollowCLUT ( Track th, Boolean);
VOID            QTAPI  SubtractTime ( TimeRecord FAR *, const TimeRecord FAR *);
TimeValue       QTAPI  TrackTimeToMediaTime ( TimeValue, Track);
VOID            QTAPI  TransformRect( const MatrixRecord FAR *theMatrix,LPRECT theRect,LPVOID);
OSErr           QTAPI  UpdateMovie ( Movie);
ComponentResult QTAPI  MCActivate ( MovieController, HWND, Boolean);
ComponentResult QTAPI  MCClick ( MovieController, HWND, Point, long, long);
ComponentResult QTAPI  MCDoAction ( MovieController, UINT, LPVOID);
ComponentResult QTAPI  MCDraw ( MovieController, HWND);
ComponentResult QTAPI  MCDrawBadge ( MovieController, HRGN, HRGN FAR *);
ComponentResult QTAPI  MCGetControllerBoundsRect ( MovieController, LPRECT);
ComponentResult QTAPI  MCGetControllerInfo ( MovieController, LPLONG);
TimeValue       QTAPI  MCGetCurrentTime ( MovieController, TimeScale FAR *);
Movie           QTAPI  MCGetMovie ( MovieController);
ComponentResult QTAPI  MCGetVisible ( MovieController);
ComponentResult QTAPI  MCIdle ( MovieController);
ComponentResult QTAPI  MCIsControllerAttached ( MovieController);
ComponentResult QTAPI  MCIsPlayerMessage ( MovieController, HWND, UINT, WPARAM, LPARAM);
ComponentResult QTAPI  MCKey ( MovieController, WPARAM, LPARAM);
ComponentResult QTAPI  MCMovieChanged ( MovieController, Movie);
OSErr           QTAPI  MCNewMovieController ( MovieController, ComponentInstance, Movie, const LPRECT, LONG, HWND);
ComponentResult QTAPI  MCNewAttachedController ( MovieController, Movie, HWND, POINT);
ComponentResult QTAPI  MCPositionController ( MovieController, LPRECT, LPRECT, LONG);
ComponentResult QTAPI  MCRemoveMovie ( MovieController);
ComponentResult QTAPI  MCSetActionFilter ( MovieController, MCActionFilter, LONG);
ComponentResult QTAPI  MCSetControllerAttached ( MovieController, Boolean);
ComponentResult QTAPI  MCSetControllerBoundsRect ( MovieController, const LPRECT);
ComponentResult QTAPI  MCSetMovie ( MovieController, Movie, HWND, POINT);
ComponentResult QTAPI  MCSetVisible ( MovieController, Boolean);
Component       QTAPI  CaptureComponent ( Component cCaptured, Component cCapturing);
OSErr           QTAPI  CloseComponent ( ComponentInstance ci);
OSErr           QTAPI  CloseComponentResFile ( short sFileNum);
LONG            QTAPI  ComponentFunctionImplemented ( ComponentInstance ci, short sFunctionNumber);
LONG            QTAPI  CountComponentInstances ( Component c);
LONG            QTAPI  CountComponents ( ComponentDescription FAR *lpcdLooking);
Component       QTAPI  FindNextComponent ( Component c, ComponentDescription FAR *lpcdLooking);
HINSTANCE       QTAPI  GetComponentResFileInstance ( short sFileNum);
OSErr           QTAPI  GetComponentInfo ( Component c, ComponentDescription FAR *lpcd);
OSErr           QTAPI  GetComponentInstanceError ( ComponentInstance ci);
LPVOID          QTAPI  GetComponentInstanceStorage ( ComponentInstance ci);
LONG            QTAPI  GetComponentListModSeed ( VOID);
LONG            QTAPI  GetComponentRefcon ( Component c);
LONG            QTAPI  GetComponentVersion ( ComponentInstance ci);
ComponentInstance QTAPI  OpenComponent ( Component c);
short           QTAPI  OpenComponentResFile ( Component c);
ComponentInstance QTAPI  OpenDefaultComponent ( OSType ostypeComponentType, OSType ostypeComponentSubType);
Component       QTAPI  RegisterComponent ( LPCD lpcd, WORD wGlobal, FIXUPFUNC lpfnTBFixup, FIXUPFUNC lpfnCMFixup);
OSErr           QTAPI  RegisterComponentResourceFile ( LPCSTR lpszDLL, WORD wGlobal);
VOID            QTAPI  SetComponentInstanceError ( ComponentInstance ci, OSErr oserr);
VOID            QTAPI  SetComponentInstanceStorage ( ComponentInstance ci, LPVOID lpvStorage);
VOID            QTAPI  SetComponentRefcon ( Component c, LONG lRefcon);
OSErr           QTAPI  SetDefaultComponent ( Component c, short sFlags);
OSErr           QTAPI  UncaptureComponent ( Component c);
OSErr           QTAPI  UnregisterComponent ( Component c);
OSErr           QTAPI  UnregisterComponentResourceFile ( LPCSTR lpszDLL);

// End of Functions
// ----------------
#ifdef __cplusplus
  }
#endif


// End of QTW.H
// ------------
#endif // __QTW_H__
