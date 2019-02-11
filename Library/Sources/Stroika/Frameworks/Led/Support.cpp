/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include <cctype>
#include <cstdarg>

#include "../../Foundation/Characters/CString/Utilities.h"
#include "../../Foundation/Characters/CodePage.h"
#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Execution/Throw.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

#include "Config.h" // For qPlatform_Windows etc defines...

#if qPlatform_MacOS
#include <AppleEvents.h> // for URL support
#include <Scrap.h>
#include <TextUtils.h>
#elif qPlatform_Windows
#include <fcntl.h>
#include <io.h>
#elif qXWindows
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#endif

#include "Support.h"

#if qUseInternetConfig
#if qSDKHasInternetConfig
#include <InternetConfig.h>
#else
#include <ICAPI.h>
#include <ICTypes.h>
#endif
#endif
#if qUseActiveXToOpenURLs
#include <URLMon.h>
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
@METHOD:        ACP2WideString
@DESCRIPTION:
*/
wstring Led::ACP2WideString (const string& s)
{
    size_t                            nChars = s.length () + 1; // convert null byte, too
    Memory::SmallStackBuffer<wchar_t> result (nChars);
    CodePageConverter                 cpg (GetDefaultSDKCodePage ());
    cpg.MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return wstring (result);
}

/*
@METHOD:        Wide2ACPString
@DESCRIPTION:
*/
string Led::Wide2ACPString (const wstring& s)
{
    size_t                         nChars = s.length () * sizeof (wchar_t) + 1; // convert null byte, too
    Memory::SmallStackBuffer<char> result (nChars);
    CodePageConverter              cpg (GetDefaultSDKCodePage ());
    cpg.MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return string (result);
}

#if qSDK_UNICODE
/*
@METHOD:        Led_ANSI2SDKString
@DESCRIPTION:   <p></p>
*/
Led_SDK_String Led::Led_ANSI2SDKString (const string& s)
{
    size_t                                 nChars = s.length () + 1; // convert null byte, too
    Memory::SmallStackBuffer<Led_SDK_Char> result (nChars);
    CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return Led_SDK_String (result);
}
#else
Led_SDK_String Led::Led_Wide2SDKString (const wstring& s)
{
    size_t                         nChars = s.length () * sizeof (wchar_t) + 1; // convert null byte, too
    Memory::SmallStackBuffer<char> result (nChars);
    CodePageConverter              cpg (GetDefaultSDKCodePage ());
    cpg.MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return Led_SDK_String (result);
}
wstring Led::Led_SDKString2Wide (const Led_SDK_String& s)
{
    return ACP2WideString (s);
}
#endif

#if qSDK_UNICODE
/*
@METHOD:        Led_SDKString2ANSI
@DESCRIPTION:   <p></p>
*/
string Led::Led_SDKString2ANSI (const Led_SDK_String& s)
{
    size_t                         nChars = s.length () * sizeof (wchar_t) + 1; // convert null byte, too
    Memory::SmallStackBuffer<char> result (nChars);
    CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return string (result);
}
#endif

#if !qWideCharacters
Led_tString Led::Led_WideString2tString (const wstring& s)
{
    return Wide2ACPString (s);
}

wstring Led::Led_tString2WideString (const Led_tString& s)
{
    return ACP2WideString (s);
}
#endif

#if qWideCharacters != qSDK_UNICODE
/*
@METHOD:        Led_tString2SDKString
@DESCRIPTION:   <p></p>
*/
Led_SDK_String Led::Led_tString2SDKString (const Led_tString& s)
{
#if qWideCharacters && !qSDK_UNICODE
    size_t                                 nChars = s.length () * sizeof (wchar_t) + 1; // convert null byte, too
    Memory::SmallStackBuffer<Led_SDK_Char> result (nChars);
    CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return Led_SDK_String (result);
#elif !qWideCharacters && qSDK_UNICODE
    size_t                                 nChars = s.length () + 1; // convert null byte, too
    Memory::SmallStackBuffer<Led_SDK_Char> result (nChars);
    CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return Led_SDK_String (result);
#else
#error "Hmm"
#endif
}
#endif

#if qWideCharacters != qSDK_UNICODE
/*
@METHOD:        Led_SDKString2tString
@DESCRIPTION:   <p></p>
*/
Led_tString Led::Led_SDKString2tString (const Led_SDK_String& s)
{
#if qWideCharacters && !qSDK_UNICODE
    size_t                              nChars = s.length () + 1; // convert null byte, too
    Memory::SmallStackBuffer<Led_tChar> result (nChars);
    CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return Led_tString (result);
#elif !qWideCharacters && qSDK_UNICODE
    size_t                              nChars = s.length () * sizeof (wchar_t) + 1; // convert null byte, too
    Memory::SmallStackBuffer<Led_tChar> result (nChars);
    CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return Led_tString (result);
#else
#error "Hmm"
#endif
}
#endif

#if qWideCharacters
/*
@METHOD:        Led_ANSIString2tString
@DESCRIPTION:   <p></p>
*/
Led_tString Led::Led_ANSIString2tString (const string& s)
{
    size_t                              nChars = s.length () + 1; // convert null byte, too
    Memory::SmallStackBuffer<Led_tChar> result (nChars);
    CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return Led_tString (result);
}
#endif

#if qWideCharacters
/*
@METHOD:        Led_tString2ANSIString
@DESCRIPTION:   <p></p>
*/
string Led::Led_tString2ANSIString (const Led_tString& s)
{
    size_t                         nChars = s.length () * sizeof (wchar_t) + 1; // convert null byte, too
    Memory::SmallStackBuffer<char> result (nChars);
    CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
    return string (result);
}
#endif

/*
@METHOD:        Led_BeepNotify
@DESCRIPTION:   <p>Make an audible beep on the users terminal. Used as a simple warning
            mechanism (like for typing bad characters).</p>
                <p>NB: For X-Windows only, this function invokes the private gBeepNotifyCallBackProc
            callback procedure to handle the beeping.
            This is because with X-Windows, the beep callback requires data (XDisplay) we don't have at this level.</p>
*/
void Led::Led_BeepNotify ()
{
#if qPlatform_MacOS
    ::SysBeep (1);
#elif qPlatform_Windows
    ::MessageBeep (MB_OK);
#elif qXWindows
    if (gBeepNotifyCallBackProc != nullptr) {
        (gBeepNotifyCallBackProc) ();
    }
#endif
}

#if qXWindows
static unsigned long sLastXWindowsEventTime;
static double        sLastEventReferenceTime;
static double        Led::GetThisMachineCurTime ()
{
    struct timeval tv;
    memset (&tv, 0, sizeof (tv));
    Verify (::gettimeofday (&tv, nullptr) == 0);
    Assert (tv.tv_usec < 1000000);
    double t = static_cast<double> (tv.tv_sec) + (tv.tv_usec / 1000000.0);
    return t;
}
#endif

#if qXWindows
/*
@METHOD:        gBeepNotifyCallBackProc
@DESCRIPTION:   <p>X-Windows specific magic. See @'Led_BeepNotify'.</p>
*/
void (*Led::gBeepNotifyCallBackProc) () = nullptr;

/*
@METHOD:        SyncronizeLedXTickCount
@DESCRIPTION:   <p>X-Windows specific magic. See @'Time::GetTickCount'.</p>
*/
void Led::SyncronizeLedXTickCount (unsigned long xTickCount)
{
    sLastEventReferenceTime = GetThisMachineCurTime ();
    sLastXWindowsEventTime  = xTickCount;
}

/*
@METHOD:        LedTickCount2XTime
@DESCRIPTION:   <p>X-Windows specific. See also @'SyncronizeLedXTickCount' and @'Time::GetTickCount'. Maps Time::GetTickCount ()
            result to the sort of time value you can stick into an XEvent record.</p>
*/
unsigned long Led::LedTickCount2XTime (float ledTickCount)
{
    return static_cast<unsigned long> (ledTickCount * 1000.0f);
}
#endif

/*
@METHOD:        Led_GetDoubleClickTime
@DESCRIPTION:   <p>Returns the amount of time (in seconds) between clicks which the OS deems should be interpretted
            as a double click.</p>
*/
Time::DurationSecondsType Led::Led_GetDoubleClickTime ()
{
#if qPlatform_MacOS
    return (float(::GetDblTime ()) / 60.0f);
#elif qPlatform_Windows
    return (float(::GetDoubleClickTime ()) / 1000.0f);
#elif qXWindows
    return 0.25f; // SAME AS DOUBLE_CLICK_TIME FROM gdkevents.c
#endif
}

/*
 ********************************************************************************
 *********************** Led_ThrowOutOfMemoryException **************************
 ********************************************************************************
 */
static void (*sLedOutOfMemoryExceptionCallback) () = nullptr;

void Led::Led_ThrowOutOfMemoryException ()
{
    if (sLedOutOfMemoryExceptionCallback == nullptr) {
        throw bad_alloc ();
    }
    else {
        (sLedOutOfMemoryExceptionCallback) ();
    }
    Assert (false); // should never get here!
}

void (*Led::Led_Get_OutOfMemoryException_Handler ()) ()
{
    return sLedOutOfMemoryExceptionCallback;
}

void Led::Led_Set_OutOfMemoryException_Handler (void (*outOfMemoryHandler) ())
{
    sLedOutOfMemoryExceptionCallback = outOfMemoryHandler;
}

int Led::Led_tStrniCmp (const Led_tChar* l, const Led_tChar* r, size_t n)
{
    RequireNotNull (l);
    RequireNotNull (r);
#if qSingleByteCharacters
    return ::strnicmp (l, r, n);
#elif qMultiByteCharacters
    return ::_mbsnicmp (l, r, n);
#elif qWideCharacters
    return ::_wcsnicmp (l, r, n);
#endif
}

int Led::Led_tStriCmp (const Led_tChar* l, const Led_tChar* r)
{
    RequireNotNull (l);
    RequireNotNull (r);
#if qSingleByteCharacters
    return ::stricmp (l, r);
#elif qMultiByteCharacters
    return ::_mbsicmp (l, r);
#elif qWideCharacters
    return ::_wcsicmp (l, r);
#endif
}

/*
 ********************************************************************************
 *********************** Led_ThrowBadFormatDataException ************************
 ********************************************************************************
 */
static void (*sLedBadFormatDataExceptionCallback) () = nullptr;

void Led::Led_ThrowBadFormatDataException ()
{
    if (sLedBadFormatDataExceptionCallback == nullptr) {
        // not sure what this should throw by default?
        Execution::Throw (bad_alloc ());
    }
    else {
        (sLedBadFormatDataExceptionCallback) ();
    }
    Assert (false); // should never get here!
}

void (*Led::Led_Get_BadFormatDataException_Handler ()) ()
{
    return sLedBadFormatDataExceptionCallback;
}

void Led::Led_Set_BadFormatDataException_Handler (void (*badFormatDataExceptionCallback) ())
{
    sLedBadFormatDataExceptionCallback = badFormatDataExceptionCallback;
}

#if qPlatform_MacOS
/*
 ********************************************************************************
 ***************************** Led_ThrowOSErr ***********************************
 ********************************************************************************
 */
static void (*sLedThrowOSErrExceptionCallback) (OSErr) = nullptr;

void Led::Led_ThrowOSErr (OSErr err)
{
    if (err != noErr) {
        if (sLedThrowOSErrExceptionCallback == nullptr) {
            throw err;
        }
        else {
            (sLedThrowOSErrExceptionCallback) (err);
        }
        Assert (false); // should never get here!
    }
}

void (*Led::Led_Get_ThrowOSErrException_Handler ()) (OSErr err)
{
    return sLedThrowOSErrExceptionCallback;
}

void Led::Led_Set_ThrowOSErrException_Handler (void (*throwOSErrExceptionCallback) (OSErr err))
{
    sLedThrowOSErrExceptionCallback = throwOSErrExceptionCallback;
}
#endif

/*
@METHOD:        Led_SkrunchOutSpecialChars
@DESCRIPTION:   <p>This function is useful to remove embedded NUL-characters from a string, or
    any other such undesirable characters. Returns NEW length of string (after removals).
    Removes in place.</p>
*/
size_t Led::Led_SkrunchOutSpecialChars (Led_tChar* text, size_t textLen, Led_tChar charToRemove)
{
    size_t     charsSkipped = 0;
    Led_tChar* end          = text + textLen;
    for (Led_tChar* p = text; p + charsSkipped < end;) {
        if (*(p + charsSkipped) == charToRemove) {
            charsSkipped++;
            continue;
        }
        *p = *(p + charsSkipped);
        ++p;
    }
    return textLen - charsSkipped;
}

#if qMultiByteCharacters
bool Led::Led_IsValidMultiByteString (const Led_tChar* start, size_t len)
{
    AssertNotNull (start);
    const Led_tChar* end = &start[len];
    const Led_tChar* cur = start;
    for (; cur < end; cur = Led_NextChar (cur)) {
        if (Led_IsLeadByte (*cur)) {
            if (cur + 1 == end) {
                return false; // string ends on LedByte!!!
            }
            if (not Led_IsValidSecondByte (*(cur + 1))) {
                return false; // byte second byte after lead-byte
            }
        }
        else if (not Led_IsValidSingleByte (*cur)) {
            return false;
        }
    }
    return (cur == end); // if it were less - we'd be in the loop, and if it were
    // more - we'd have ended on a lead-byte
}
#endif

/*
 ********************************************************************************
 ************************** Led_ClipboardObjectAcquire **************************
 ********************************************************************************
 */

Led_ClipboardObjectAcquire::Led_ClipboardObjectAcquire (Led_ClipFormat clipType)
    :
#if qPlatform_MacOS || qPlatform_Windows
    fOSClipHandle (nullptr)
    ,
#endif
    fLockedData (nullptr)
{
#if qPlatform_MacOS
#if TARGET_CARBON
    ScrapRef scrap = nullptr;
    Led_ThrowIfOSStatus (::GetCurrentScrap (&scrap));
    SInt32   byteCount = 0;
    OSStatus status    = ::GetScrapFlavorSize (scrap, clipType, &byteCount);
    if (status != noTypeErr) {
        fOSClipHandle = Led_DoNewHandle (byteCount);
        Led_ThrowIfNull (fOSClipHandle);
        Assert (::GetHandleSize (fOSClipHandle) == byteCount);
        ::HLock (fOSClipHandle);
        fLockedData = *fOSClipHandle;
        Led_ThrowIfOSStatus (::GetScrapFlavorData (scrap, clipType, &byteCount, fLockedData));
    }
#else
    long scrapOffset = 0;
    fOSClipHandle    = Led_DoNewHandle (0);
    Led_ThrowIfNull (fOSClipHandle);
    long result = ::GetScrap (fOSClipHandle, clipType, &scrapOffset);
    if (result < 0) {
        ::DisposeHandle (fOSClipHandle);
        fOSClipHandle = nullptr;
        return;
    }
    Assert (::GetHandleSize (fOSClipHandle) == result);
    ::HLock (fOSClipHandle);
    fLockedData    = *fOSClipHandle;
#endif
#elif qPlatform_Windows
    // perhaps rewrite to use exceptions, but for now - when no cliptype avail - set flag so GoodClip() method can check -
    // just cuz thats what surounding code seems to expect - LGP 980617
    fOSClipHandle = ::GetClipboardData (clipType);
#if 0
    DWORD   errResult   =   ::GetLastError ();  // could be helpful for debugging if above fails... - Maybe I should do ThrowIfErrorHRESULT().... - LGP 2000/04/26
#endif
    if (fOSClipHandle != nullptr) {
        fLockedData = ::GlobalLock (fOSClipHandle);
    }
#endif
}

#if qPlatform_Windows
/*
 ********************************************************************************
 **************************** VariantArrayPacker ********************************
 ********************************************************************************
 */
VariantArrayPacker::VariantArrayPacker (VARIANT* v, VARTYPE vt, size_t nElts)
    : fSafeArrayVariant (v)
    , fPtr (nullptr)
{
    RequireNotNull (v);
    // assumes passed in variant is CONSTRUCTED (initied) - but not necesarily having the right type
    ::VariantClear (fSafeArrayVariant);
    fSafeArrayVariant->parray = ::SafeArrayCreateVector (vt, 0, static_cast<ULONG> (nElts));
    Led_ThrowIfNull (fSafeArrayVariant->parray);
    fSafeArrayVariant->vt = VT_ARRAY | vt;
    Led_ThrowIfErrorHRESULT (::SafeArrayAccessData (fSafeArrayVariant->parray, &fPtr));
}

VariantArrayPacker::~VariantArrayPacker ()
{
    AssertNotNull (fSafeArrayVariant);
    AssertNotNull (fSafeArrayVariant->parray);
    ::SafeArrayUnaccessData (fSafeArrayVariant->parray);
}

void* VariantArrayPacker::PokeAtData () const
{
    return fPtr;
}

/*
 ********************************************************************************
 ***************************** VariantArrayUnpacker *****************************
 ********************************************************************************
 */
VariantArrayUnpacker::VariantArrayUnpacker (const VARIANT& v)
    : fSafeArray (v.parray)
{
    if (not(v.vt & VT_ARRAY) or fSafeArray == nullptr) {
        throw E_INVALIDARG;
    }
    if ((v.vt & ~VT_ARRAY) != GetArrayElementType ()) {
        // I THINK this is right??? - LGP 2003-06-12 - but I'm not sure this is much reason to check/??
        throw E_INVALIDARG;
    }
    void* useP = nullptr;
    Led_ThrowIfErrorHRESULT (::SafeArrayAccessData (fSafeArray, &useP));
    fPtr = useP;
}

VariantArrayUnpacker::~VariantArrayUnpacker ()
{
    AssertNotNull (fSafeArray);
    ::SafeArrayUnaccessData (fSafeArray);
}

const void* VariantArrayUnpacker::PeekAtData () const
{
    return fPtr;
}

VARTYPE VariantArrayUnpacker::GetArrayElementType () const
{
    AssertNotNull (fSafeArray);
    VARTYPE vt = VT_EMPTY;
    Led_ThrowIfErrorHRESULT (::SafeArrayGetVartype (fSafeArray, &vt));
    return vt;
}

size_t VariantArrayUnpacker::GetLength () const
{
    AssertNotNull (fSafeArray);
    long lb = 0;
    long ub = 0;
    Led_ThrowIfErrorHRESULT (::SafeArrayGetLBound (fSafeArray, 1, &lb));
    Led_ThrowIfErrorHRESULT (::SafeArrayGetUBound (fSafeArray, 1, &ub));
    return ub - lb + 1;
}
#endif

#if qPlatform_Windows
/*
 ********************************************************************************
 **************************** CreateSafeArrayOfBSTR *****************************
 ********************************************************************************
 */

/*
@METHOD:        CreateSafeArrayOfBSTR
@DESCRIPTION:   <p>Overloaded method to create a VARIANT record containing a safe-array of BSTRs.</p>
*/
VARIANT Led::CreateSafeArrayOfBSTR (const wchar_t* const* strsStart, const wchar_t* const* strsEnd)
{
    size_t  nElts = strsEnd - strsStart;
    VARIANT result;
    ::VariantInit (&result);
    try {
        const bool kEncodeAsVariant = false; // neither seems to work with javascript test code in MSIE, but both work
        // with my C++-based unpacking code...
        if (kEncodeAsVariant) {
            VariantArrayPacker packer (&result, VT_VARIANT, nElts);
            VARIANT*           vptr = reinterpret_cast<VARIANT*> (packer.PokeAtData ());
            for (size_t i = 0; i < nElts; ++i) {
                ::VariantInit (&vptr[i]);
                Led_ThrowIfNull (vptr[i].bstrVal = ::SysAllocString (strsStart[i]));
                vptr[i].vt = VT_VARIANT;
            }
        }
        else {
            VariantArrayPacker packer (&result, VT_BSTR, nElts);
            BSTR*              bptr = reinterpret_cast<BSTR*> (packer.PokeAtData ());
            for (size_t i = 0; i < nElts; ++i) {
                Led_ThrowIfNull (bptr[i] = ::SysAllocString (strsStart[i]));
            }
        }
    }
    catch (...) {
        ::VariantClear (&result);
        throw;
    }
    return result;
}

VARIANT Led::CreateSafeArrayOfBSTR (const vector<const wchar_t*>& v)
{
    return CreateSafeArrayOfBSTR (Traversal::Iterator2Pointer (v.begin ()), Traversal::Iterator2Pointer (v.end ()));
}

VARIANT Led::CreateSafeArrayOfBSTR (const vector<wstring>& v)
{
    vector<const wchar_t*> tmp;
    for (auto i = v.begin (); i != v.end (); ++i) {
        // I hope (assure?) this doesn't create temporaries and take .c_str () of temporaries
        tmp.push_back ((*i).c_str ());
    }
    return CreateSafeArrayOfBSTR (tmp);
}

/*
 ********************************************************************************
 ***************** UnpackVectorOfStringsFromVariantArray ************************
 ********************************************************************************
 */
vector<wstring> Led::UnpackVectorOfStringsFromVariantArray (const VARIANT& v)
{
    vector<wstring>      result;
    VariantArrayUnpacker up (v);
    size_t               nElts = up.GetLength ();
    switch (up.GetArrayElementType ()) {
        case VT_BSTR: {
            const BSTR* bArray = reinterpret_cast<const BSTR*> (up.PeekAtData ());
            for (size_t i = 0; i < nElts; ++i) {
                result.push_back (bArray[i]);
            }
        } break;
        case VT_VARIANT: {
            const VARIANT* vArray = reinterpret_cast<const VARIANT*> (up.PeekAtData ());
            for (size_t i = 0; i < nElts; ++i) {
                result.push_back (vArray[i].bstrVal);
            }
        } break;
        default:
            throw (DISP_E_BADVARTYPE);
    }
    return result;
}
#endif

#if qPlatform_Windows
/*
@METHOD:        DumpSupportedInterfaces
@DESCRIPTION:   <p>@'qPlatform_Windows' only</p>
                <p>Helpful COM debugging utility which dumps to the debugger window all the interfaces
            supported by a given COM object. The arguments 'objectName' and 'levelPrefix' can be nullptr (optional).
            </p>
*/
void Led::DumpSupportedInterfaces (IUnknown* obj, const char* objectName, const char* levelPrefix)
{
    {
        string label = objectName == nullptr ? string () : Characters::CString::Format (" (named '%s')", objectName);
        string msg   = Characters::CString::Format ("Dumping interfaces for object%s at %p:\n", label.c_str (), obj);
        ::OutputDebugStringA (msg.c_str ());

        if (levelPrefix == nullptr) {
            levelPrefix = "\t";
        }
    }

    /*
     *  All interfaces are stored in the registry. Lookup each one, and do a query-interface
     *  with each on the given object to see what interfaces it contains.
     */
    HKEY      interfaceKey = nullptr;
    HKEY      iKey         = nullptr;
    BSTR      tmpStr       = nullptr;
    IUnknown* test         = nullptr;
    try {
        if (::RegOpenKeyEx (HKEY_CLASSES_ROOT, _T ("Interface"), 0, KEY_READ, &interfaceKey) != ERROR_SUCCESS) {
            throw GetLastError ();
        }
        for (DWORD i = 0;; ++i) {
            char  subKey[MAX_PATH];
            DWORD subKeySize = sizeof (subKey);
            if (::RegEnumKeyExA (interfaceKey, i, subKey, &subKeySize, 0, 0, 0, 0) == ERROR_SUCCESS) {
                try {
                    CLSID iid;
                    (void)::memset (&iid, 0, sizeof (iid));
                    tmpStr = ::SysAllocString (ACP2WideString (subKey).c_str ());
                    Led_ThrowIfErrorHRESULT (::CLSIDFromString (tmpStr, &iid));
                    if (tmpStr != nullptr) {
                        ::SysFreeString (tmpStr);
                        tmpStr = nullptr;
                    }
                    if (SUCCEEDED (obj->QueryInterface (iid, reinterpret_cast<void**> (&test)))) {
                        // dump that obj interface
                        Assert (iKey == nullptr);
                        if (::RegOpenKeyExA (interfaceKey, subKey, 0, KEY_READ, &iKey) != ERROR_SUCCESS) {
                            throw GetLastError ();
                        }
                        DWORD dwType = 0;
                        TCHAR interfaceNameBuf[1024];
                        DWORD interfaceNameBufSize = sizeof (interfaceNameBuf);
                        if (::RegQueryValueEx (iKey, _T(""), nullptr, &dwType, reinterpret_cast<LPBYTE> (interfaceNameBuf), &interfaceNameBufSize) == ERROR_SUCCESS) {
                            OutputDebugStringA (levelPrefix);
                            OutputDebugString (interfaceNameBuf);
                            OutputDebugStringA ("\n");
                        }
                        if (iKey != nullptr) {
                            ::CloseHandle (iKey);
                            iKey = nullptr;
                        }
                    }
                    if (test != nullptr) {
                        test->Release ();
                        test = nullptr;
                    }
                }
                catch (...) {
                    OutputDebugStringA (levelPrefix);
                    OutputDebugStringA ("ERROR LOOKING UP INTERFACE - IGNORING");
                    OutputDebugStringA ("\n");
                }
            }
            else {
                break; // end of list of interfaces?
            }
        }
        if (test != nullptr) {
            test->Release ();
            test = nullptr;
        }
        if (tmpStr != nullptr) {
            ::SysFreeString (tmpStr);
            tmpStr = nullptr;
        }
        if (interfaceKey != nullptr) {
            ::CloseHandle (interfaceKey);
            interfaceKey = nullptr;
        }
    }
    catch (...) {
        if (iKey != nullptr) {
            ::CloseHandle (iKey);
            iKey = nullptr;
        }
        if (interfaceKey != nullptr) {
            ::CloseHandle (interfaceKey);
            interfaceKey = nullptr;
        }
        if (tmpStr != nullptr) {
            ::SysFreeString (tmpStr);
            tmpStr = nullptr;
        }
        if (test != nullptr) {
            test->Release ();
            test = nullptr;
        }
    }
}

/*
@METHOD:        DumpObjectsInIterator
@DESCRIPTION:   <p>@'qPlatform_Windows' only</p>
                <p>Helpful COM debugging utility which dumps to the debugger window all the subobjects of a given COM object,
            along with the interfaces they supoport (see also @'DumpSupportedInterfaces'). The arguments
            'iteratorName' and 'levelPrefix' can be nullptr (optional).
            </p>
*/
void Led::DumpObjectsInIterator (IEnumUnknown* iter, const char* iteratorName, const char* levelPrefix)
{
    {
        string label = iteratorName == nullptr ? string () : Characters::CString::Format (" (named '%s')", iteratorName);
        string msg   = Characters::CString::Format ("Dumping objects (and their interface names) for iterator%s at %p\n", label.c_str (), iter);
        ::OutputDebugStringA (msg.c_str ());
        if (levelPrefix == nullptr) {
            levelPrefix = "\t";
        }
    }
    if (iter == nullptr) {
        ::OutputDebugStringA (levelPrefix);
        ::OutputDebugStringA ("<<nullptr ENUM POINTER>>\n"); // try DumpSupportedInterfaces to see what interfaces your enum interface supports!
        return;
    }
    IUnknown* nextObj = nullptr;
    for (size_t i = 0; SUCCEEDED (iter->Next (1, &nextObj, nullptr)); ++i) {
        char nameBuf[1024];
        (void)snprintf (nameBuf, NEltsOf (nameBuf), "obj#%d", static_cast<int> (i));
        char levelPrefixBuf[1024];
        Assert (::strlen (levelPrefix) < sizeof (levelPrefixBuf) / 2); // assert MUCH less
        Characters::CString::Copy (levelPrefixBuf, NEltsOf (levelPrefixBuf), levelPrefix);
        CString::Cat (levelPrefixBuf, NEltsOf (levelPrefixBuf), "\t");
        DumpSupportedInterfaces (nextObj, nameBuf, levelPrefixBuf);
        AssertNotNull (nextObj);
        nextObj->Release ();
        nextObj = nullptr;
    }
    Assert (nextObj == nullptr);
}
#endif

/*
 ********************************************************************************
 *************************************** Led_URLD *******************************
 ********************************************************************************
 */
Led_URLD::Led_URLD (const char* url, const char* title)
    : fData ()
{
    RequireNotNull (url);
    RequireNotNull (title);
    size_t urlLen   = ::strlen (url);
    size_t titleLen = ::strlen (title);

    size_t resultURLDLen = (urlLen + 1 + titleLen + 1);
    fData.resize (resultURLDLen, '\0');

    char* data = &fData.front ();
    Characters::CString::Copy (data, resultURLDLen, url);
    CString::Cat (data, fData.size () - 1, "\r");
    CString::Cat (data, fData.size () - 1, title);
}

Led_URLD::Led_URLD (const void* urlData, size_t nBytes)
    : fData ()
{
    // assure a validly formatted Led_URLD - regardless of input
    for (size_t i = 0; i < nBytes; i++) {
        char c = ((char*)urlData)[i];
        fData.push_back (c);
        if (c == '\0') {
            break;
        }
    }
    // make sure NUL-terminated...
    if (fData.size () == 0 or fData.back () != '\0') {
        fData.push_back ('\0');
    }
}

size_t Led_URLD::GetURLDLength () const
{
    // Note we DONT count the terminating NUL - cuz Netscape 2.0 doesn't appear to. And thats our reference.
    return fData.size () - 1;
}

size_t Led_URLD::GetURLLength () const
{
    size_t len = fData.size ();
    for (size_t i = 0; i < len; i++) {
        if (fData[i] == '\r') {
            return i;
        }
    }
    return len - 1;
}

size_t Led_URLD::GetTitleLength () const
{
    size_t len = fData.size ();
    for (size_t i = 0; i < len; i++) {
        if (fData[i] == '\r') {
            Assert (len >= (i + 1 + 1)); // cuz must be NUL-term & skip \r
            return len - (i + 1 + 1);    // ditto
        }
    }
    return 0;
}

char* Led_URLD::PeekAtURLD () const
{
    return (&const_cast<Led_URLD*> (this)->fData.front ());
}

char* Led_URLD::PeekAtURL () const
{
    return (&const_cast<Led_URLD*> (this)->fData.front ());
}

char* Led_URLD::PeekAtTitle () const
{
    return PeekAtURL () + GetURLLength () + 1; // skip URL and '\r'
}

string Led_URLD::GetTitle () const
{
    return string (PeekAtTitle (), GetTitleLength ());
}

string Led_URLD::GetURL () const
{
    return string (PeekAtURL (), GetURLLength ());
}

/*
 ********************************************************************************
 ********************************* Led_URLManager *******************************
 ********************************************************************************
 */
#if qUseSpyglassDDESDIToOpenURLs
DWORD Led_URLManager::sDDEMLInstance = 0; //  The DDEML instance identifier.
#endif
Led_URLManager* Led_URLManager::sThe = nullptr;

Led_URLManager::Led_URLManager ()
{
}

Led_URLManager& Led_URLManager::Get ()
{
    if (sThe == nullptr) {
        sThe = new Led_URLManager ();
    }
    return *sThe;
}

void Led_URLManager::Set (Led_URLManager* newURLMgr)
{
    if (sThe != newURLMgr) {
        delete sThe;
        sThe = newURLMgr;
    }
}

void Led_URLManager::Open (const string& url)
{
#if qUseInternetConfig
    try {
        Open_IC (url);
    }
    catch (...) {
        Open_SpyglassAppleEvent (url);
    }
#elif qPlatform_MacOS
    Open_SpyglassAppleEvent (url);
#elif qPlatform_Windows
#if qUseActiveXToOpenURLs && qUseSpyglassDDESDIToOpenURLs
    try {
        Open_ActiveX (url);
    }
    catch (...) {
        Open_SpyglassDDE (url);
    }
#else
#if qUseActiveXToOpenURLs
    Open_ActiveX (url);
#elif qUseSpyglassDDESDIToOpenURLs
    Open_SpyglassDDE (url);
#endif
#endif
#endif
#if qUseSystemNetscapeOpenURLs
    Open_SystemNetscape (url);
#endif
}

#if qPlatform_MacOS
string Led_URLManager::FileSpecToURL (const FSSpec& fsp)
{
    short  len = 0;
    Handle h   = nullptr;
    Led_ThrowOSErr (FSpGetFullPath (&fsp, &len, &h));
    AssertNotNull (h);
    const char kFilePrefix[]  = "file:///";
    size_t     kFilePrefixLen = ::strlen (kFilePrefix);
    char*      result         = new char[kFilePrefixLen + len + 1];
    (void)::memcpy (result, kFilePrefix, kFilePrefixLen);
    (void)::memcpy (&result[kFilePrefixLen], *h, len);
    ::DisposeHandle (h);
    result[kFilePrefixLen + len] = '\0';
    for (size_t i = 0; i < len; i++) {
        if (result[kFilePrefixLen + i] == ':') {
            result[kFilePrefixLen + i] = '/';
        }
    }
    string r = result;
    delete[] result;

    // Now go through and quote illegal URL characters
    string rr;
    for (size_t i = 0; i < r.length (); i++) {
        char c = r[i];
        if (isalnum (c) or c == '/' or c == '.' or c == '_' or c == ':' or c == '#') {
            rr.append (&c, 1);
        }
        else {
            rr.append ("%");
            unsigned char x = (((unsigned char)c) >> 4);
            Assert (x <= 0xf);
            x = (x > 9) ? (x - 10 + 'A') : (x + '0');
            rr.append ((char*)&x, 1);
            x = ((unsigned char)(c & 0xf));
            Assert (x <= 0xf);
            x = (x > 9) ? (x - 10 + 'A') : (x + '0');
            rr.append ((char*)&x, 1);
        }
    }
    return rr;
}
#elif qPlatform_Windows
string Led_URLManager::FileSpecToURL ([[maybe_unused]] const string& path)
{
    AssertNotImplemented (); // nyi (not needed anywhere right now)
    return "";
}
#endif

#if qUseInternetConfig
void Led_URLManager::Open_IC (const string& url)
{
    // If we compile for internet config, first try that. If that fails, fall back on Netscape.
    Str255 hint      = "\p"; // not sure what this is for. See how it works without it...
    OSType signature = '\?\?\?\?';
    {
        ProcessSerialNumber myPSN;
        memset (&myPSN, 0, sizeof (myPSN));
        Led_ThrowOSErr (::GetCurrentProcess (&myPSN));
        ProcessInfoRec info;
        memset (&info, 0, sizeof (info));
        info.processInfoLength = sizeof (info);
        Led_ThrowOSErr (::GetProcessInformation (&myPSN, &info));
        signature = info.processSignature;
    }
    ICInstance icInstance;
    Led_ThrowOSErr (::ICStart (&icInstance, signature));
#if qSDKHasInternetConfig
    //  OSErr   err =   ::ICGeneralFindConfigFile (icInstance, true, true, 0, nullptr);
    OSErr err = noErr;
#else
    ICError err    = ::ICFindConfigFile (icInstance, 0, nullptr);
#endif
    if (err == noErr) {
        long urlStart = 0;
        long urlEnd   = 0;
        // Unclear if/why url would be modified, but since they declare it as non-cost
        // better be sure...LGP 961028
        char urlBuf[1024];
        CString::Copy (urlBuf, url.c_str (), NEltsOf (urlBuf));
        err = ::ICLaunchURL (icInstance, hint, urlBuf, ::strlen (urlBuf), &urlStart, &urlEnd);
    }
    ::ICStop (icInstance);
    Led_ThrowOSErr (err);
}
#endif

#if qPlatform_MacOS
void Led_URLManager::Open_SpyglassAppleEvent (const string& url)
{
    const OSType        AE_spy_receive_suite = 'WWW!';
    const OSType        AE_spy_openURL       = 'OURL'; // typeChar OpenURL
    ProcessSerialNumber browserPSN           = FindBrowser ();
    AppleEvent          event;
    {
        AEAddressDesc progressApp;
        Led_ThrowOSErr (::AECreateDesc (typeProcessSerialNumber, &browserPSN, sizeof (browserPSN), &progressApp));
        try {
            Led_ThrowOSErr (::AECreateAppleEvent (AE_spy_receive_suite, AE_spy_openURL, &progressApp, kAutoGenerateReturnID, kAnyTransactionID, &event));
            ::AEDisposeDesc (&progressApp);
        }
        catch (...) {
            ::AEDisposeDesc (&progressApp);
            throw;
        }
    }
    try {
        Led_ThrowOSErr (::AEPutParamPtr (&event, keyDirectObject, typeChar, url.c_str (), url.length ()));
        AppleEvent reply;
        Led_ThrowOSErr (::AESend (&event, &reply, kAEWaitReply, kAENormalPriority, kAEDefaultTimeout, nil, nil));
        ::AEDisposeDesc (&event);
        ::AEDisposeDesc (&reply);
    }
    catch (...) {
        ::AEDisposeDesc (&event);
        throw;
    }
}
#endif

#if qUseActiveXToOpenURLs
void Led_URLManager::Open_ActiveX (const string& url)
{
    Memory::SmallStackBuffer<wchar_t> wideURLBuf (url.length () + 1);
    {
        int nWideChars         = ::MultiByteToWideChar (CP_ACP, 0, url.c_str (), static_cast<int> (url.length ()), wideURLBuf, static_cast<int> (url.length ()));
        wideURLBuf[nWideChars] = '\0';
    }
    IUnknown* pUnk = nullptr; // implies we are an OLE-ignorant app. But I'm not sure what else I'm supposed to pass here!

/*
     *  Use LoadLibrary/GetProcAddress instead of direct call to avoid having to link with
     *  urlmon.lib. This avoidance allows us to run on systems that don't have MSIE installed.
     *
     *  Personally, I think its pretty weak MS doesn't offer a simpler way to deal with this problem.
     *  (or if they do, their docs are weak).
     *      --LGP 961015
     */
#if 1
    static HINSTANCE urlmonLibrary = ::LoadLibrary (_T ("urlmon"));
    if (urlmonLibrary == nullptr) {
        Led_ThrowOutOfMemoryException (); // not sure what else to throw?
    }
    static HRESULT (WINAPI * hlinkNavigateString) (IUnknown*, LPCWSTR) =
        (HRESULT (WINAPI*) (IUnknown*, LPCWSTR))::GetProcAddress (urlmonLibrary, "HlinkNavigateString");
    if (hlinkNavigateString == nullptr) {
        Led_ThrowOutOfMemoryException (); // not sure what else to throw?
    }
    HRESULT result = (*hlinkNavigateString) (pUnk, wideURLBuf);
#else
    HRESULT result = ::HlinkNavigateString (pUnk, wideURLBuf);
#endif
    if (not SUCCEEDED (result)) {
        throw result;
    }
}
#endif

#if qUseSpyglassDDESDIToOpenURLs
void Led_URLManager::Open_SpyglassDDE (const string& url)
{
    /*
     *  Use DDE to talk to any browser supporting the Spyglass DDE SDI Suite (right now I only know of
     *  Netscape, ironicly) - LGP 960502
     */

    Require (sDDEMLInstance != 0); //  Must call Led_URLManager::InitDDEHandler ()
    // (maybe we should throw? put up alert? LGP 960502)

    // WWW_OPENURL
    const kDDETIMEOUT = 2 * 1000; // timeout in milliseconds
    {
        // Open Client connection
        HSZ   hszBrowser = nullptr; // connect to any browser
        HSZ   hszTopic   = ::DdeCreateStringHandle (sDDEMLInstance, _T ("WWW_OpenURL"), CP_WINANSI);
        HCONV hConv      = ::DdeConnect (sDDEMLInstance, hszBrowser, hszTopic, nullptr);
        ::DdeFreeStringHandle (sDDEMLInstance, hszTopic);

        long dwWindowID = -1;
        long dwFlags    = 0;
        // Format DDE message
        HSZ hszItem = ClientArguments ("QCS,QCS,DW,DW,QCS,QCS,CS", &url, nullptr, &dwWindowID, &dwFlags, nullptr, nullptr, nullptr);

        // Send DDE message
        //  Send along the item to the server, as an XTYP_REQUEST.
        //  Return the data received from the server.
        //  Proactively delete the hszItem string.
        HDDEDATA hRetVal = ::DdeClientTransaction (nullptr, 0, hConv, hszItem, CF_TEXT, XTYP_REQUEST, kDDETIMEOUT, nullptr);
        if (hszItem != nullptr) {
            ::DdeFreeStringHandle (sDDEMLInstance, hszItem);
        }

        ::DdeDisconnect (hConv);
    }

    // WWW_ACTIVATE(activate the window/browser application)
    {
        // Open Client connection
        HSZ   hszBrowser = nullptr; // connect to any browser
        HSZ   hszTopic   = ::DdeCreateStringHandle (sDDEMLInstance, _T ("WWW_Activate"), CP_WINANSI);
        HCONV hConv      = ::DdeConnect (sDDEMLInstance, hszBrowser, hszTopic, nullptr);
        ::DdeFreeStringHandle (sDDEMLInstance, hszTopic);

        long dwWindowID = -1;
        long dwFlags    = 0;
        // Format DDE message
        HSZ hszItem = ClientArguments ("DW,DW", &dwWindowID, &dwFlags);

        //  Send along the item to the server, as an XTYP_REQUEST.
        //  Return the data received from the server.
        //  Proactively delete the hszItem string.
        HDDEDATA hRetVal = DdeClientTransaction (nullptr, 0, hConv, hszItem, CF_TEXT, XTYP_REQUEST, kDDETIMEOUT, nullptr);
        if (hszItem != nullptr) {
            ::DdeFreeStringHandle (sDDEMLInstance, hszItem);
        }

        ::DdeDisconnect (hConv); // Close connection
    }
}
#endif

#if qUseSystemNetscapeOpenURLs
void Led_URLManager::Open_SystemNetscape (const string& url)
{
    string execString;
    /*
     *  Code lifted (based on) code from AbiWord - xap_UnixFrame.cpp
     */
    struct stat statbuf;
    memset (&statbuf, 0, sizeof (statbuf));
//
// The gnome-help-browser sucks right now. Instead open with netscape
// or kde.
// When it gets better we should restore this code.
/*
    //if (!stat("/opt/gnome/bin/gnome-help-browser", statbuf) || !stat("/usr/local/bin/gnome-help-browser", statbuf) || !stat("/usr/bin/gnome-help-browser", statbuf))
    {
        execString = g_strdup_printf("gnome-help-browser %s &", szURL);
    }
    */
#if 0
// My system has kdehelp - and I can exec it by itself - but passing args produces an error from KDE:
//      ERROR: KFM is not running
//      KFM not ready
    if (!stat("/opt/kde/bin/kdehelp", &statbuf) || !stat("/usr/local/kde/bin/kdehelp", &statbuf) || !stat("/usr/local/bin/kdehelp", &statbuf) || !stat("/usr/bin/kdehelp", &statbuf)) {
        execString = "kdehelp " + url + "&";
    }
    else
#endif
    {
        // Try to connect to a running Netscape, if not, start new one
        Memory::SmallStackBuffer<char> buf (url.length () * 2 + 1000);
        sprintf (buf, "netscape -remote openURL\\(\"%s\"\\) || netscape \"%s\" &", url.c_str (), url.c_str ());
        execString = buf;
    }
    system (execString.c_str ());
}
#endif

#if qPlatform_MacOS
pascal OSErr Led_URLManager::FSpGetFullPath (const FSSpec* spec, short* fullPathLength, Handle* fullPath)
{
    // Based on code from Apple Macintosh Developer Technical Support
    // "MoreFiles 1.4.2" example code
    OSErr      result;
    FSSpec     tempSpec;
    CInfoPBRec pb;

    /* Make a copy of the input FSSpec that can be modified */
    BlockMoveData (spec, &tempSpec, sizeof (FSSpec));

    if (tempSpec.parID == fsRtParID) {
        /* The object is a volume */

        /* Add a colon to make it a full pathname */
        ++tempSpec.name[0];
        tempSpec.name[tempSpec.name[0]] = ':';

        /* We're done */
        result = PtrToHand (&tempSpec.name[1], fullPath, tempSpec.name[0]);
    }
    else {
        /* The object isn't a volume */

        /* Put the object name in first */
        result = PtrToHand (&tempSpec.name[1], fullPath, tempSpec.name[0]);
        if (result == noErr) {
            /* Get the ancestor directory names */
            pb.dirInfo.ioNamePtr = tempSpec.name;
            pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
            pb.dirInfo.ioDrParID = tempSpec.parID;
            do { /* loop until we have an error or find the root directory */
                pb.dirInfo.ioFDirIndex = -1;
                pb.dirInfo.ioDrDirID   = pb.dirInfo.ioDrParID;
                result                 = PBGetCatInfoSync (&pb);
                if (result == noErr) {
                    /* Append colon to directory name */
                    ++tempSpec.name[0];
                    tempSpec.name[tempSpec.name[0]] = ':';

                    /* Add directory name to beginning of fullPath */
                    (void)Munger (*fullPath, 0, nullptr, 0, &tempSpec.name[1], tempSpec.name[0]);
                    result = MemError ();
                }
            } while ((result == noErr) && (pb.dirInfo.ioDrDirID != fsRtDirID));
        }
    }
    if (result == noErr) {
        /* Return the length */
        *fullPathLength = GetHandleSize (*fullPath);
    }
    else {
        /* Dispose of the handle and return nullptr and zero length */
        DisposeHandle (*fullPath);
        *fullPath       = nullptr;
        *fullPathLength = 0;
    }

    return (result);
}
#endif

#if qUseSpyglassDDESDIToOpenURLs
void Led_URLManager::InitDDEHandler ()
{
    //  Initialize DDEML
    Require (sDDEMLInstance == 0); // only call once

    // callback not normally called, but seems to sometimes be called under error conditions, and we get crashes
    // if no callback present (ie if I pass nullptr to DDEInitialize()) - not when I call DDEInitialize(), but much
    // later - apparently when DDEML tries to deliver a message.
    if (::DdeInitialize (&sDDEMLInstance, SimpleDdeCallBack, APPCMD_CLIENTONLY, 0)) {
        sDDEMLInstance = 0;
    }
}
#endif

#if qPlatform_MacOS
ProcessSerialNumber Led_URLManager::FindBrowser ()
{
    OSType              sig         = 'MOSS'; // currently we hardwire Netscape - but we could extend this list to include all known browsers...
    OSType              processType = 'APPL';
    ProcessSerialNumber psn;
    psn.highLongOfPSN = 0;
    psn.lowLongOfPSN  = kNoProcess;
    OSErr          err;
    ProcessInfoRec info;
    do {
        err = ::GetNextProcess (&psn);
        if (err == noErr) {
            info.processInfoLength = sizeof (ProcessInfoRec);
            info.processName       = nullptr;
            info.processAppSpec    = nullptr;
            err                    = ::GetProcessInformation (&psn, &info);
        }
    } while ((err == noErr) && ((info.processSignature != sig) || (info.processType != processType)));
    Led_ThrowOSErr (err);
    return info.processNumber;
}
#endif

#if qUseSpyglassDDESDIToOpenURLs
HDDEDATA CALLBACK Led_URLManager::SimpleDdeCallBack (UINT /*type*/, UINT /*fmt*/, HCONV /*hconv*/, HSZ /*hsz1*/, HSZ /*hsz2*/, HDDEDATA /*hData*/, DWORD /*dwData1*/, DWORD /*dwData2*/)
{
    return 0;
}
#endif

#if qUseSpyglassDDESDIToOpenURLs
const char* Led_URLManager::SkipToNextArgument (const char* pFormat)
{
    RequireNotNull (pFormat);
    if (*pFormat == '\0') {
        return (pFormat);
    }

    //  The next format is directly after a ','
    while (*pFormat != ',' && *pFormat != '\0') {
        pFormat++;
    }
    if (*pFormat == ',') {
        pFormat++;
    }
    return (pFormat);
}
#endif

#if qUseSpyglassDDESDIToOpenURLs
HSZ Led_URLManager::ClientArguments (const char* pFormat, ...)
{
    RequireNotNull (pFormat);

    //  Always pass in pointer values.
    //  This way, a nullptr, can be interpreted as an optional, missing parameter.
    const char* pTraverse = pFormat;

    //  Variable number of arguments.
    va_list VarList;
    va_start (VarList, pFormat);
    char   caNumpad[64];
    string csBuffer;
    string csRetval;
    while (*pTraverse != '\0') {
        //  Erase temp data from our last pass.
        caNumpad[0] = '\0';
        csBuffer    = string ();
        //  Compare our current format to the known formats
        if (0 == strncmp (pTraverse, "DW", 2)) {
            //  A DWORD.
            DWORD* pWord = va_arg (VarList, DWORD*);
            if (pWord != nullptr) {
                //  See if we need to use hex.
                snprintf (caNumpad, NEltsOf (caNumpad), "%lu", *pWord);
                csRetval += caNumpad;
            }
        }
        else if (0 == strncmp (pTraverse, "CS", 2)) {
            //  A CString, not quoted
            string* pCS = va_arg (VarList, string*);
            if (pCS != nullptr) {
                csRetval += *pCS;
            }
        }
        else if (0 == strncmp (pTraverse, "QCS", 3)) {
            //  A quoted CString
            string* pQCS = va_arg (VarList, string*);
            if (pQCS != nullptr) {
                csRetval += '\"';
                //  Need to escape any '"' to '\"', literally.
                const char* pConvert = pQCS->c_str ();
                while (*pConvert != '\0') {
                    if (*pConvert == '\"') {
                        csRetval += '\\';
                    }
                    csRetval += *pConvert;
                    pConvert++;
                }
                csRetval += '\"';
            }
        }
        else if (0 == strncmp (pTraverse, "BL", 2)) {
            //  If compiling in 32 bit mode, BOOLs change size.
            using TwoByteBool = short;
            //  A boolean
            TwoByteBool* pBool = va_arg (VarList, TwoByteBool*);
            if (pBool != nullptr) {
                csRetval += *pBool ? "TRUE" : "FALSE";
            }
        }
        else {
            //  Unhandled format, just get out of loop.
            Assert (false);
            break;
        }

        //  Go on to next type
        pTraverse = SkipToNextArgument (pTraverse);

        //  See if we need a comma
        if (*pTraverse != '\0') {
            csRetval += ',';
        }
    }

    va_end (VarList);

    //  Make sure we're atleast returning something.
    if (csRetval.empty ()) {
        return (nullptr);
    }

    //  Return our resultant HSZ, created from our string.
    HSZ Final = ::DdeCreateStringHandleA (sDDEMLInstance, csRetval.c_str (), CP_WINANSI);
    return (Final);
}

char* Led_URLManager::ExtractArgument (HSZ hszArgs, int iArg)
{
    //  Quoted strings are counted as only one argument, though
    //      the quotes are not copied into the return string.

    DWORD dwLength  = ::DdeQueryString (sDDEMLInstance, hszArgs, nullptr, 0L, CP_WINANSI) + 1;
    char* pTraverse = new char[dwLength];
    char* pRemove   = pTraverse;
    ::DdeQueryStringA (sDDEMLInstance, hszArgs, pTraverse, dwLength, CP_WINANSI);

    //  safety dance
    if (*pTraverse == '\0' || iArg < 1) {
        delete (pRemove);
        return (nullptr);
    }

    //  Need to decrement the argument we're looking for, as the very
    //      first argument has no ',' at the beginning.
    iArg--;

    //  Search through the arguments, seperated by ','.
    while (iArg) {
        //  Special handling of quoted strings.
        if (*pTraverse == '\"') {
            //  Find the ending quote.
            while (*pTraverse != '\0') {
                pTraverse++;
                if (*pTraverse == '\"') {
                    pTraverse++; //  One beyond, please
                    break;
                }
                else if (*pTraverse == '\\') {
                    //  Attempting to embed a quoted, perhaps....
                    if (*(pTraverse + 1) == '\"') {
                        pTraverse++;
                    }
                }
            }
        }
        while (*pTraverse != '\0' && *pTraverse != ',') {
            pTraverse++;
        }

        //  Go beyond a comma
        if (*pTraverse == ',') {
            pTraverse++;
        }

        iArg--;

        if (*pTraverse == '\0') {
            break;
        }
    }

    //  Handle empty arguments here.
    if (*pTraverse == ',' || *pTraverse == '\0') {
        delete (pRemove);
        return (nullptr);
    }

    int   iLength       = 1;
    char* pCounter      = pTraverse;
    using TwoByteBool   = short;
    TwoByteBool bQuoted = FALSE;

    //  specially handle quoted strings
    if (*pCounter == '\"') {
        pCounter++;
        bQuoted = TRUE;

        while (*pCounter != '\0') {
            if (*pCounter == '\"') {
                break;
            }
            else if (*pCounter == '\\') {
                if (*(pCounter + 1) == '\"') {
                    pCounter++;
                    iLength++;
                }
            }

            pCounter++;
            iLength++;
        }
    }
    while (*pCounter != '\0' && *pCounter != ',') {
        iLength++;
        pCounter++;
    }

    //  Subtrace one to ignore ending quote if we were quoted....
    if (bQuoted == TRUE) {
        iLength--;
    }

    //  Argument's of length 1 are of no interest.
    if (iLength == 1) {
        delete (pRemove);
        return (nullptr);
    }

    char* pRetVal = new char[iLength];

    if (*pTraverse == '\"') {
        pTraverse++;
    }
    CString::Copy (pRetVal, pTraverse, iLength);

    delete (pRemove);
    return (pRetVal);
}

void Led_URLManager::ServerReturned (HDDEDATA hArgs, const char* pFormat, ...)
{
    //  Of course, only pointers should be passed in as the variable number of
    //      arguments so assignment can take place.
    //  hArgs is free'd off by this function.
    char* pDataArgs = (char*)DdeAccessData (hArgs, nullptr);

    //  Initialize variable number of argumetns.
    va_list VarList;
    va_start (VarList, pFormat);

    //  It will be possible that with only one argument,
    //      that we are receiving raw data intead of string notation.
    if (strchr (pFormat, ',') == nullptr) {
        //  Only one argument.
        if (strcmp (pFormat, "DW") == 0) {
            DWORD* pWord;
            pWord  = va_arg (VarList, DWORD*);
            *pWord = *(DWORD*)pDataArgs;
            DdeUnaccessData (hArgs);
            DdeFreeDataHandle (hArgs);
            va_end (VarList);
            return;
        }
        else if (strcmp (pFormat, "BL") == 0) {
            using TwoByteBool = short;
            TwoByteBool* pBool;
            pBool  = va_arg (VarList, TwoByteBool*);
            *pBool = *(TwoByteBool*)pDataArgs;
            DdeUnaccessData (hArgs);
            DdeFreeDataHandle (hArgs);
            va_end (VarList);
            return;
        }
    }

    //  We are assuming nullptr terminated data, since there is more than one
    //      parameter expected.
    HSZ hszArgs = DdeCreateStringHandleA (sDDEMLInstance, pDataArgs, CP_WINANSI);
    DdeUnaccessData (hArgs);
    DdeFreeDataHandle (hArgs);

    int         i_ArgNum = 0;
    const char* pScan    = pFormat;
    char*       pExtract;

    //  Loop through the arguments we are going to parse.
    while (pScan && *pScan) {
        i_ArgNum++;
        pExtract = ExtractArgument (hszArgs, i_ArgNum);

        if (0 == strncmp (pScan, "DW", 2)) {
            //  DWORD.
            DWORD* pWord;
            pWord = va_arg (VarList, DWORD*);

            //  If there is nothing to scan, use a default value.
            if (pExtract == nullptr) {
                *pWord = 0x0;
            }
            else {
                sscanf (pExtract, "%lu", pWord);
            }
        }
        else if (0 == strncmp (pScan, "QCS", 3)) {
            //  A quoted CString
            string* pCS = va_arg (VarList, string*);

            if (pExtract == nullptr) {
                //pCS->Empty();
                *pCS = string ();
            }
            else {
                //  Fun thing about a qouted string, is that we need
                //      to compress and '\"' into '"'.
                //  Extractions took off the leading and ending quotes.
                char* pCopy = pExtract;
                while (*pCopy) {
                    if (*pCopy == '\\' && *(pCopy + 1) == '\"') {
                        pCopy++;
                    }

                    *pCS += *pCopy;
                    pCopy++;
                }
            }
        }
        else if (0 == strncmp (pScan, "CS", 2)) {
            //  A CString
            string* pCS = va_arg (VarList, string*);

            if (pExtract == nullptr) {
                *pCS = string ();
            }
            else {
                *pCS = pExtract;
            }
        }
        else if (0 == strncmp (pScan, "BL", 2)) {
            using TwoByteBool = short;
            //  A boolean
            TwoByteBool* pBool = va_arg (VarList, TwoByteBool*);

            if (pExtract == nullptr) {
                *pBool = FALSE;
            }
            else {
                //  Compare for a TRUE or a FALSE
                if (0 == stricmp (pExtract, "TRUE")) {
                    *pBool = TRUE;
                }
                else {
                    *pBool = FALSE;
                }
            }
        }

        //  Go on to the next argument in our format string.
        pScan = SkipToNextArgument (pScan);

        //  Free the memory that was used during extraction.
        if (pExtract != nullptr) {
            delete pExtract;
        }
    }

    //  Done with variable number of arguments
    va_end (VarList);

    //  Free off our created HSZ string.
    DdeFreeStringHandle (sDDEMLInstance, hszArgs);
}
#endif

/*
 ********************************************************************************
 ********************** Led::MakeSophistsAppNameVersionURL **********************
 ********************************************************************************
 */
string Led::MakeSophistsAppNameVersionURL (const string& relURL, const string& appName, const string& extraArgs)
{
    Require (relURL.length () > 0 and relURL[0] == '/');
    char fullVersionBuf[1024];
    (void)snprintf (fullVersionBuf, NEltsOf (fullVersionBuf), "%d", qLed_FullVersion);
    string fullURL = "http://www.sophists.com" + relURL +
                     "?AppName=" + appName +
#if qPlatform_Windows
                     string ("&Platform=Windows") +
#elif qPlatform_MacOS
                     string ("&Platform=MacOS") +
#elif qXWindows
                     string ("&Platform=XWindows") +
#endif
                     "&MajorMinorVersion=" + qLed_MajorMinorVersionString +
                     "&LedFullVersion=" + fullVersionBuf +
                     "&ShortVersionString=" + qLed_ShortVersionString +
                     extraArgs;
    return fullURL;
}

/*
 ********************************************************************************
 ******************************* Led_CasedCharsEqual ****************************
 ********************************************************************************
 */
bool Led::Led_CasedCharsEqual (char lhs, char rhs, bool ignoreCase)
{
    if (lhs == rhs) {
        return true;
    }
    if (ignoreCase and
        isascii (lhs) and isalpha (lhs) and
        isascii (rhs) and isalpha (rhs) and
        toupper (lhs) == toupper (rhs)) {
        return true;
    }
    return false;
}

/*
 ********************************************************************************
 ******************************* Led_CasedStringsEqual **************************
 ********************************************************************************
 */
bool Led::Led_CasedStringsEqual (const string& lhs, const string& rhs, bool ignoreCase)
{
    if (lhs.length () != rhs.length ()) {
        return false;
    }
    for (size_t i = 0; i < lhs.length (); i++) {
        if (not Led_CasedCharsEqual (lhs[i], rhs[i], ignoreCase)) {
            return false;
        }
    }
    return true;
}

CompileTimeFlagChecker_SOURCE (Stroika::Frameworks::Led, qSingleByteCharacters, qSingleByteCharacters);
CompileTimeFlagChecker_SOURCE (Stroika::Frameworks::Led, qMultiByteCharacters, qMultiByteCharacters);
CompileTimeFlagChecker_SOURCE (Stroika::Frameworks::Led, qWideCharacters, qWideCharacters);
CompileTimeFlagChecker_SOURCE (Stroika::Frameworks::Led, qProvideIMESupport, qProvideIMESupport);
