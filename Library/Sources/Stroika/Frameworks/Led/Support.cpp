/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cctype>
#include <cstdarg>

#include "../../Foundation/Characters/CString/Utilities.h"
#include "../../Foundation/Characters/CodeCvt.h"
#include "../../Foundation/Characters/CodePage.h"
#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Execution/Throw.h"
#include "../../Foundation/Memory/StackBuffer.h"

#include "Config.h" // For qPlatform_Windows etc defines...

#if qPlatform_Windows
#include <fcntl.h>
#include <io.h>
#elif qStroika_FeatureSupported_XWindows
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#endif

#include "Support.h"

#if qUseActiveXToOpenURLs
#include <URLMon.h>
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

#if !qTargetPlatformSDKUseswchar_t
SDKString Led::Led_tString2SDKString (const Led_tString& s)
{
    return CodeCvt<wchar_t>{locale{}}.String2Bytes<SDKString> (span<const wchar_t>{s});
}
#endif

/*
@METHOD:        Led_ANSIString2tString
@DESCRIPTION:   <p></p>
*/
Led_tString Led::Led_ANSIString2tString (const string& s)
{
    // Up until Stroika v3.0d2 (and maybe for a while after) - this converted from CP_ACP, despite being called 'ANSI' - I htink because I was
    // once confused about the difference between these two --LGP 2023-07-27
    return CodeCvt<Led_tChar>{locale{}}.Bytes2String<Led_tString> (as_bytes (span{s}));
}

/*
@METHOD:        Led_tString2ANSIString
@DESCRIPTION:   <p></p>
*/
string Led::Led_tString2ANSIString (const Led_tString& s)
{
    // Up until Stroika v3.0d2 (and maybe for a while after) - this converted from CP_ACP, despite being called 'ANSI' - I htink because I was
    // once confused about the difference between these two --LGP 2023-07-27
    return CodeCvt<Led_tChar>{locale{}}.String2Bytes<string> (span{s});
}

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
#if qPlatform_Windows
    ::MessageBeep (MB_OK);
#elif qStroika_FeatureSupported_XWindows
    if (gBeepNotifyCallBackProc != nullptr) {
        (gBeepNotifyCallBackProc) ();
    }
#endif
}

#if qStroika_FeatureSupported_XWindows
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

#if qStroika_FeatureSupported_XWindows
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
Time::DurationSeconds Led::Led_GetDoubleClickTime ()
{
#if qPlatform_Windows
    return Time::DurationSeconds{float (::GetDoubleClickTime ()) / 1000.0f};
#else
    return 0.25s; // SAME AS DOUBLE_CLICK_TIME FROM gdkevents.c
#endif
}

int Led::Led_tStrniCmp (const Led_tChar* l, const Led_tChar* r, size_t n)
{
    using Characters::String;
    RequireNotNull (l);
    RequireNotNull (r);
    auto result =
        String::ThreeWayComparer{eCaseInsensitive}(Characters::String{l}.SafeSubString (0, n), Characters::String{r}.SafeSubString (0, n));
    if (result == strong_ordering::equal)
        return 0;
    if (result == strong_ordering::less)
        return -1;
    if (result == strong_ordering::greater)
        return 1;
    AssertNotReached ();
    return 0;
}

int Led::Led_tStriCmp (const Led_tChar* l, const Led_tChar* r)
{
    RequireNotNull (l);
    RequireNotNull (r);
    auto result = String::ThreeWayComparer{eCaseInsensitive}(Characters::String{l}, Characters::String{r});
    if (result == strong_ordering::equal)
        return 0;
    if (result == strong_ordering::less)
        return -1;
    if (result == strong_ordering::greater)
        return 1;
    AssertNotReached ();
    return 0;
}

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
            ++charsSkipped;
            continue;
        }
        *p = *(p + charsSkipped);
        ++p;
    }
    return textLen - charsSkipped;
}

#if qStroika_Frameworks_Led_SupportClipboard
/*
 ********************************************************************************
 ************************** Led_ClipboardObjectAcquire **************************
 ********************************************************************************
 */

Led_ClipboardObjectAcquire::Led_ClipboardObjectAcquire (Led_ClipFormat clipType)
    :
#if qPlatform_Windows
    fOSClipHandle (nullptr)
    ,
#endif
    fLockedData (nullptr)
{
#if qPlatform_Windows
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
#endif

#if qPlatform_Windows
/*
 ********************************************************************************
 **************************** VariantArrayPacker ********************************
 ********************************************************************************
 */
VariantArrayPacker::VariantArrayPacker (VARIANT* v, VARTYPE vt, size_t nElts)
    : fSafeArrayVariant{v}
{
    RequireNotNull (v);
    // assumes passed in variant is CONSTRUCTED (initied) - but not necesarily having the right type
    ::VariantClear (fSafeArrayVariant);
    fSafeArrayVariant->parray = ::SafeArrayCreateVector (vt, 0, static_cast<ULONG> (nElts));
    Execution::ThrowIfNull (fSafeArrayVariant->parray);
    fSafeArrayVariant->vt = VT_ARRAY | vt;
    ThrowIfErrorHRESULT (::SafeArrayAccessData (fSafeArrayVariant->parray, &fPtr));
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
    : fSafeArray{v.parray}
{
    if (not(v.vt & VT_ARRAY) or fSafeArray == nullptr) {
        throw E_INVALIDARG;
    }
    if ((v.vt & ~VT_ARRAY) != GetArrayElementType ()) {
        // I THINK this is right??? - LGP 2003-06-12 - but I'm not sure this is much reason to check/??
        throw E_INVALIDARG;
    }
    void* useP = nullptr;
    ThrowIfErrorHRESULT (::SafeArrayAccessData (fSafeArray, &useP));
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
    ThrowIfErrorHRESULT (::SafeArrayGetVartype (fSafeArray, &vt));
    return vt;
}

size_t VariantArrayUnpacker::GetLength () const
{
    AssertNotNull (fSafeArray);
    long lb = 0;
    long ub = 0;
    ThrowIfErrorHRESULT (::SafeArrayGetLBound (fSafeArray, 1, &lb));
    ThrowIfErrorHRESULT (::SafeArrayGetUBound (fSafeArray, 1, &ub));
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
                Execution::ThrowIfNull (vptr[i].bstrVal = ::SysAllocString (strsStart[i]));
                vptr[i].vt = VT_VARIANT;
            }
        }
        else {
            VariantArrayPacker packer (&result, VT_BSTR, nElts);
            BSTR*              bptr = reinterpret_cast<BSTR*> (packer.PokeAtData ());
            for (size_t i = 0; i < nElts; ++i) {
                Execution::ThrowIfNull (bptr[i] = ::SysAllocString (strsStart[i]));
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
        string label = objectName == nullptr ? string{} : Characters::CString::Format (" (named '%s')", objectName);
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
                    tmpStr = ::SysAllocString (String::FromNarrowSDKString (subKey).As<wstring> ().c_str ());
                    ThrowIfErrorHRESULT (::CLSIDFromString (tmpStr, &iid));
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
                        if (::RegQueryValueEx (iKey, _T(""), nullptr, &dwType, reinterpret_cast<LPBYTE> (interfaceNameBuf),
                                               &interfaceNameBufSize) == ERROR_SUCCESS) {
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
        string label = iteratorName == nullptr ? string{} : Characters::CString::Format (" (named '%s')", iteratorName);
        string msg = Characters::CString::Format ("Dumping objects (and their interface names) for iterator%s at %p\n", label.c_str (), iter);
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
        (void)snprintf (nameBuf, Memory::NEltsOf (nameBuf), "obj#%d", static_cast<int> (i));
        char levelPrefixBuf[1024];
        Assert (::strlen (levelPrefix) < sizeof (levelPrefixBuf) / 2); // assert MUCH less
        Characters::CString::Copy (levelPrefixBuf, Memory::NEltsOf (levelPrefixBuf), levelPrefix);
        CString::Cat (levelPrefixBuf, Memory::NEltsOf (levelPrefixBuf), "\t");
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
    return string{PeekAtTitle (), GetTitleLength ()};
}

string Led_URLD::GetURL () const
{
    return string{PeekAtURL (), GetURLLength ()};
}

/*
 ********************************************************************************
 ********************************* Led_URLManager *******************************
 ********************************************************************************
 */
namespace {
    Led_URLManager* sTheURLMgr_;
}
Led_URLManager& Led_URLManager::Get ()
{
    if (sTheURLMgr_ == nullptr) {
        sTheURLMgr_ = new Led_URLManager ();
    }
    return *sTheURLMgr_;
}

void Led_URLManager::Set (Led_URLManager* newURLMgr)
{
    if (sTheURLMgr_ != newURLMgr) {
        delete sTheURLMgr_;
        sTheURLMgr_ = newURLMgr;
    }
}

void Led_URLManager::Open (const string& url)
{
#if qPlatform_Windows
#if qUseActiveXToOpenURLs
    Open_ActiveX (url);
#endif
#endif
}

string Led_URLManager::FileSpecToURL ([[maybe_unused]] const filesystem::path& p)
{
    AssertNotImplemented (); // nyi (not needed anywhere right now)
    return "";
}

#if qUseActiveXToOpenURLs
void Led_URLManager::Open_ActiveX (const string& url)
{
    Memory::StackBuffer<wchar_t> wideURLBuf{Memory::eUninitialized, url.length () + 1};
    {
        int nWideChars         = ::MultiByteToWideChar (CP_ACP, 0, url.c_str (), static_cast<int> (url.length ()), wideURLBuf.data (),
                                                        static_cast<int> (url.length ()));
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
        Execution::Throw (bad_alloc{});
    }
    static HRESULT (WINAPI * hlinkNavigateString) (IUnknown*, LPCWSTR) =
        (HRESULT (WINAPI*) (IUnknown*, LPCWSTR))::GetProcAddress (urlmonLibrary, "HlinkNavigateString");
    if (hlinkNavigateString == nullptr) {
        Execution::Throw (bad_alloc{});
    }
    HRESULT result = (*hlinkNavigateString) (pUnk, wideURLBuf.data ());
#else
    HRESULT result = ::HlinkNavigateString (pUnk, wideURLBuf.data ());
#endif
    if (not SUCCEEDED (result)) {
        throw result;
    }
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
    (void)snprintf (fullVersionBuf, Memory::NEltsOf (fullVersionBuf), "%d", qLed_FullVersion);
    string fullURL = "http://www.sophists.com" + relURL + "?AppName=" + appName +
#if qPlatform_Windows
                     string{"&Platform=Windows"} +
#elif qStroika_FeatureSupported_XWindows
                     string{"&Platform=XWindows"} +
#endif
                     "&MajorMinorVersion=" + qLed_MajorMinorVersionString + "&LedFullVersion=" + fullVersionBuf +
                     "&ShortVersionString=" + qLed_ShortVersionString + extraArgs;
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
    if (ignoreCase and isascii (lhs) and isalpha (lhs) and isascii (rhs) and isalpha (rhs) and toupper (lhs) == toupper (rhs)) {
        return true;
    }
    return false;
}

CompileTimeFlagChecker_SOURCE (Stroika::Frameworks::Led, qStroika_Frameworks_Led_ProvideIMESupport, qStroika_Frameworks_Led_ProvideIMESupport);
