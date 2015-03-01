/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cmath>
#include    <cstdarg>
#include    <cstdio>
#include    <fstream>
#include    <map>
#include    <mutex>

#include    "../Characters/Format.h"
#include    "../Characters/LineEndings.h"
#include    "../Execution/Common.h"
#include    "../Execution/Module.h"
#include    "../Execution/Process.h"
#include    "../Execution/Thread.h"
#include    "../IO/FileSystem/PathName.h"
#include    "../Memory/Common.h"
#include    "../Time/Realtime.h"

#if     qTraceToFile
#include    "../IO/FileSystem/WellKnownLocations.h"
#include    "../Time/DateTime.h"
#endif

#include    "Trace.h"



using   namespace   Stroika::Foundation;

using   namespace   Characters;
using   namespace   Debug;
using   namespace   Execution;


using   Execution::make_unique_lock;



/*
 * TODO:
 *
 *      (o)     The buffering code here maybe now correct, but isn't simple/clear, so rewrite/improve...
 *              -- LGP 2011-10-03
 */



namespace   {
    // This is MOSTLY to remove NEWLINES from the MIDDLE of a message - replace with kBadChar.
    const   char    kBadChar_   =   ' ';
    void    SquishBadCharacters_ (string* s)
    {
        RequireNotNull (s);
        size_t  end =   s->length ();
        // ignore last 2 in case crlf
        if (end > 2) {
            end -= 2;
        }
        for (size_t i = 0; i < end; ++i) {
            if ((*s)[i] == '\n' or (*s)[i] == '\r') {
                (*s)[i] = kBadChar_;
            }
        }
    }
    void    SquishBadCharacters_ (wstring* s)
    {
        RequireNotNull (s);
        size_t  end =   s->length ();
        // ignore last 2 in case crlf
        if (end > 2) {
            end -= 2;
        }
        for (size_t i = 0; i < end; ++i) {
            if ((*s)[i] == '\n' or (*s)[i] == '\r') {
                (*s)[i] = kBadChar_;
            }
        }
    }
}



#if     qCompilerAndStdLib_HasFirstTimeUsePerTranslationUnitFloatingPoint_Buggy
FILE_SCOPE_TOP_OF_TRANSLATION_UNIT_MSVC_FLOATING_POINT_BWA();
#endif



/*
 ********************************************************************************
 ************************ Private_::TraceModuleData_ ****************************
 ********************************************************************************
 */
namespace   {
    recursive_mutex*    sEmitTraceCritSec_   =   nullptr;
#if     qTraceToFile
    ofstream*   sTraceFile  =   nullptr;
#endif
#if     qDefaultTracingOn
    thread_local    unsigned int    sTraceContextDepth_ { 0 };  // no need for atomic access because thread_local
#endif

    // Declared HERE instead of the template so they get shared across TYPE values for CHARTYPE
    Thread::IDType  sMainThread_                =   Execution::GetCurrentThreadID ();

    string  mkPrintDashAdornment_ ()
    {
        size_t threadPrintWidth = FormatThreadID (sMainThread_).length () - 4;
        string result;
        result.reserve (threadPrintWidth / 2);
        for (size_t i = 0; i < threadPrintWidth / 2; ++i) {
            result.append ("-");
        }
        return result;
    }
    char        sThreadPrintDashAdornment_[32]; // use static array to avoid putting the string object into TraceModuleData_, and otherwise having to worry about use after main when calls DbgTrace()
    bool        sDidOneTimePrimaryThreadMessage_    =   false;
}





#if     qTraceToFile
namespace   {
    SDKString mkTraceFileName_ ()
    {
        // Use TempDir instead of EXEDir because on vista, installation permissions prevent us from (easily) writing in EXEDir.
        // (could fix of course, but I'm not sure desirable - reasonable defaults)
        //
        // Don't want to use TempFileLibrarian cuz we dont want these deleted on app exit
        SDKString mfname;
        {
            mfname = Execution::GetEXEPathT ();
            size_t i = mfname.rfind (IO::FileSystem::kPathComponentSeperator);
            if (i != SDKString::npos) {
                mfname = mfname.substr (i + 1);
            }
            i = mfname.rfind ('.');
            if (i != SDKString::npos) {
                mfname.erase (i);
            }
            for (auto i = mfname.begin (); i != mfname.end (); ++i) {
                if (*i == ' ') {
                    *i = '-';
                }
            }
        }
        SDKString nowstr  =   Time::DateTime::Now ().Format (Time::DateTime::PrintFormat::eXML).AsSDKString ();
        for (auto i = nowstr.begin (); i != nowstr.end (); ++i) {
            if (*i == ':') {
                *i = '-';
            }
        }
        return IO::FileSystem::WellKnownLocations::GetTemporaryT () + CString::Format (SDKSTR ("TraceLog_%s_PID#%d-%s.txt"), mfname.c_str (), (int)Execution::GetCurrentProcessID (), nowstr.c_str ());
    }
}
#endif

Debug::Private_::TraceModuleData_::TraceModuleData_ ()
    : fEmitter ()
    , fStringDependency (Characters::MakeModuleDependency_String ())
#if     qTraceToFile
    , fTraceFileName (mkTraceFileName_ ())
#endif
{
    CString::Copy (sThreadPrintDashAdornment_, NEltsOf (sThreadPrintDashAdornment_), mkPrintDashAdornment_ ().c_str ());
    Assert (sEmitTraceCritSec_ == nullptr);
    sEmitTraceCritSec_ = new recursive_mutex ();
#if     qTraceToFile
    Assert (sTraceFile == nullptr);
    sTraceFile = new ofstream ();
    sTraceFile->open (Emitter::Get ().GetTraceFileName ().c_str (), ios::out | ios::binary);
#endif
}

Debug::Private_::TraceModuleData_::~TraceModuleData_ ()
{
    delete sEmitTraceCritSec_;
    sEmitTraceCritSec_ = nullptr;
#if     qTraceToFile
    AssertNotNull (sTraceFile);
    sTraceFile->close ();
    delete sTraceFile;
    sTraceFile = nullptr;
#endif
}








namespace   {
    inline  recursive_mutex&    GetCritSection_ ()
    {
        // obsolete comment because as of 2014-01-31 (or earlier) we are acutallly deleting this. But we do have
        // some race where we sometimes - rarely - trigger this error. So we may need to re-instate that leak!!!
        // and this comment!!!
        //  --LGP 2014-02-01
#if 0
        // this is a 'false' or 'apparent' memory leak, but we allocate the object this way because in C++ things
        // can be destroyed in any order, (across OBJs), and though this gets destroyed late, its still possible
        // someone might do a trace message.
        //      -- LGP 2008-12-21
#endif
        EnsureNotNull (sEmitTraceCritSec_);
        return *sEmitTraceCritSec_;
    }
}



#if     qTraceToFile
SDKString Emitter::GetTraceFileName () const
{
    return Execution::ModuleInitializer<Private_::TraceModuleData_>::Actual ().fTraceFileName;
}
#endif


#if     qTraceToFile
namespace   {
    void    Emit2File_ (const char* text)
    {
        RequireNotNull (text);
        RequireNotNull (sTraceFile);
        try {
            if (sTraceFile->is_open ()) {
                (*sTraceFile) << text;
                sTraceFile->flush ();
            }
        }
        catch (...) {
            Assert (false);
        }
    }
    void    Emit2File_ (const wchar_t* text)
    {
        RequireNotNull (text);
        Emit2File_ (WideStringToUTF8 (text).c_str ());
    }
}
#endif







/*
 ********************************************************************************
 ******************** Debug::MakeModuleDependency_Trace *************************
 ********************************************************************************
 */
Execution::ModuleDependency Debug::MakeModuleDependency_Trace ()
{
    return Execution::ModuleInitializer<Debug::Private_::TraceModuleData_>::GetDependency ();
}






/*
 ********************************************************************************
 ************************************ Emitter ***********************************
 ********************************************************************************
 */
Emitter::Emitter ()
    : fLastNCharBufCharCount_ (0)
//  , fLastNCharBuf_CHAR_ ()
//  , fLastNCharBuf_WCHAR_ ()
    , fLastNCharBuf_WCHARFlag_ (false)
    , fLastNCharBuf_Token_ (0)
    , fLastNCharBuf_WriteTickcount_ (0.0f)
{
}

/*
@DESCRIPTION:   <p>This function takes a 'format' argument and then any number of additional arguments - exactly
            like std::printf (). It calls std::vsprintf () internally. This can be called directly - regardless of the
             @'qDefaultTracingOn' flag - but is typically just called indirectly by calling
             @'DbgTrace'.</p>
*/
void    Emitter::EmitTraceMessage (const char* format, ...)
{
    try {
        va_list     argsList;
        va_start (argsList, format);
        string  tmp =   Characters::CString::FormatV (format, argsList);
        va_end (argsList);
        SquishBadCharacters_ (&tmp);
        AssureHasLineTermination (&tmp);
        DoEmitMessage_ (0, Containers::Start (tmp), Containers::End (tmp));
    }
    catch (...) {
        Assert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
    }
}

void    Emitter::EmitTraceMessage (const wchar_t* format, ...)
{
    try {
        va_list     argsList;
        va_start (argsList, format);
        wstring tmp =   Characters::CString::FormatV (format, argsList);
        va_end (argsList);
        SquishBadCharacters_ (&tmp);
        AssureHasLineTermination (&tmp);
        DoEmitMessage_ (0, Containers::Start (tmp), Containers::End (tmp));
    }
    catch (...) {
        Assert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
    }
}

Emitter::TraceLastBufferedWriteTokenType    Emitter::EmitTraceMessage (size_t bufferLastNChars, const char* format, ...)
{
    try {
        va_list     argsList;
        va_start (argsList, format);
        string  tmp =   Characters::CString::FormatV (format, argsList);
        va_end (argsList);
        SquishBadCharacters_ (&tmp);
        AssureHasLineTermination (&tmp);
        return DoEmitMessage_ (bufferLastNChars, Containers::Start (tmp), Containers::End (tmp));
    }
    catch (...) {
        Assert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
        return 0;
    }
}

Emitter::TraceLastBufferedWriteTokenType    Emitter::EmitTraceMessage (size_t bufferLastNChars, const wchar_t* format, ...)
{
    try {
        va_list     argsList;
        va_start (argsList, format);
        wstring tmp =   Characters::CString::FormatV (format, argsList);
        va_end (argsList);
        SquishBadCharacters_ (&tmp);
        AssureHasLineTermination (&tmp);
        return DoEmitMessage_ (bufferLastNChars, Containers::Start (tmp), Containers::End (tmp));
    }
    catch (...) {
        Assert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
        return 0;
    }
}

template    <typename   CHARTYPE>
Emitter::TraceLastBufferedWriteTokenType    Emitter::DoEmitMessage_ (size_t bufferLastNChars, const CHARTYPE* p, const CHARTYPE* e)
{
    auto    critSec { make_unique_lock (GetCritSection_ ()) };
    FlushBufferedCharacters_ ();
    static  Time::DurationSecondsType   sStartOfTime    =   0.0;
    if (sStartOfTime == 0.0) {
        sStartOfTime = Time::GetTickCount ();
    }
    Time::DurationSecondsType   curRelativeTime =   Time::GetTickCount () - sStartOfTime;
    {
        char    buf[1024];
        Thread::IDType  threadID    =   Execution::GetCurrentThreadID ();
        string  threadIDStr =   WideStringToNarrowSDKString (FormatThreadID (threadID));
        if (sMainThread_ == threadID) {
            Verify (::snprintf  (buf, NEltsOf (buf), "[%sMAIN%s][%08.3f]\t", sThreadPrintDashAdornment_, sThreadPrintDashAdornment_, curRelativeTime) > 0);
            if (not sDidOneTimePrimaryThreadMessage_) {
                sDidOneTimePrimaryThreadMessage_ = true;
                char buf2[1024];
                Verify ( ::snprintf  (buf2, NEltsOf (buf2), "(REAL THREADID=%s)\t", threadIDStr.c_str ()) > 0);
#if     __STDC_WANT_SECURE_LIB__
                strcat_s (buf, buf2);
#else
                strcat (buf, buf2);
#endif
#if     qPlatform_POSIX
                Verify (::snprintf  (buf2, NEltsOf (buf2), "(pthread_self=0x%lx)\t", (unsigned long)pthread_self ()) > 0);
#if     __STDC_WANT_SECURE_LIB__
                strcat_s (buf, buf2);
#else
                strcat (buf, buf2);
#endif
#endif
            }
        }
        else {
            ::snprintf  (buf, NEltsOf (buf), "[%s][%08.3f]\t", threadIDStr.c_str (), curRelativeTime);
        }
        DoEmit_ (buf);
    }
#if     qDefaultTracingOn
    unsigned int    contextDepth    =   TraceContextBumper::GetCount ();
    for (unsigned int i = 0; i < contextDepth; ++i) {
        DoEmit_ (L"\t");
    }
#endif
    if (bufferLastNChars == 0) {
        DoEmit_ (p, e);
        fLastNCharBuf_Token_++; // even if not buffering, increment, so other buffers known to be invalid
    }
    else {
        Assert ((e - p) > static_cast<ptrdiff_t> (bufferLastNChars));
        BufferNChars_ (bufferLastNChars, e - bufferLastNChars);
        DoEmit_ (p, e - bufferLastNChars);
        fLastNCharBuf_WriteTickcount_ = curRelativeTime + sStartOfTime;
        fLastNCharBuf_Token_++; // even if not buffering, increment, so other buffers known to be invalid
    }
    return fLastNCharBuf_Token_;
}

void    Emitter::BufferNChars_ (size_t bufferLastNChars, const char* p)
{
    Assert (bufferLastNChars < NEltsOf (fLastNCharBuf_CHAR_));
    fLastNCharBufCharCount_ = bufferLastNChars;
#if     __STDC_WANT_SECURE_LIB__
    strcpy_s (fLastNCharBuf_CHAR_, p);
#else
    strcpy (fLastNCharBuf_CHAR_, p);
#endif
    fLastNCharBuf_WCHARFlag_ = false;
}

void    Emitter::BufferNChars_ (size_t bufferLastNChars, const wchar_t* p)
{
    Assert (bufferLastNChars < NEltsOf (fLastNCharBuf_WCHAR_));
    fLastNCharBufCharCount_ = bufferLastNChars;
#if     __STDC_WANT_SECURE_LIB__
    ::wcscpy_s (fLastNCharBuf_WCHAR_, p);
#else
    ::wcscpy (fLastNCharBuf_WCHAR_, p);
#endif
    fLastNCharBuf_WCHARFlag_ = true;
}

void    Emitter::FlushBufferedCharacters_ ()
{
    if (fLastNCharBufCharCount_ != 0) {
        if (fLastNCharBuf_WCHARFlag_) {
            DoEmit_ (fLastNCharBuf_WCHAR_);
        }
        else {
            DoEmit_ (fLastNCharBuf_CHAR_);
        }
        fLastNCharBufCharCount_ = 0;
    }
}

bool    Emitter::UnputBufferedCharactersForMatchingToken (TraceLastBufferedWriteTokenType token)
{
    auto    critSec { make_unique_lock (GetCritSection_ ()) };
    // If the fLastNCharBuf_Token_ matches (no new tokens written since the saved one) and the time
    // hasn't been too long (we currently write 1/100th second timestamp resolution).
    // then blank unput (ignore) buffered characters, and return true so caller knows to write
    // funky replacement for those characters.
    if (fLastNCharBuf_Token_ == token and (Time::GetTickCount () - fLastNCharBuf_WriteTickcount_ < 0.02f)) {
        fLastNCharBufCharCount_ = 0;
        return true;
    }
    return false;   // assume old behavior for now
}

void    Emitter::DoEmit_ (const char* p)
{
#if     qPlatform_Windows
    ::OutputDebugStringA (p);
#endif
#if     qTraceToFile
    Emit2File_ (p);
#endif
}

void    Emitter::DoEmit_ (const wchar_t* p)
{
#if     qPlatform_Windows
    ::OutputDebugStringW (p);
#endif
#if     qTraceToFile
    Emit2File_ (p);
#endif
}

void    Emitter::DoEmit_ (const char* p, const char* e)
{
    size_t  len =   e - p;
    Memory::SmallStackBuffer<char>  buf (len + 1);
    (void)::memcpy (buf.begin (), p, len);
    buf.begin () [len] = '\0';
    DoEmit_ (buf.begin ());
}

void    Emitter::DoEmit_ (const wchar_t* p, const wchar_t* e)
{
    size_t  len =   e - p;
    Memory::SmallStackBuffer<wchar_t>   buf (len + 1);
    (void)::memcpy (buf.begin (), p, len * sizeof (wchar_t));
    buf.begin () [len] = '\0';
    DoEmit_ (buf.begin ());
}









/*
 ********************************************************************************
 ****************************** TraceContextBumper ******************************
 ********************************************************************************
 */
#if     qDefaultTracingOn
TraceContextBumper::TraceContextBumper (const SDKChar* contextName)
    : fDoEndMarker (true)
    //,fSavedContextName_ ()
{
    fLastWriteToken_ = Emitter::Get ().EmitTraceMessage (3 + strlen (GetEOL<char> ()), SDKSTR ("<%s> {"), contextName);
    size_t  len =   min (NEltsOf (fSavedContextName_), char_traits<SDKChar>::length (contextName));
    char_traits<SDKChar>::copy (fSavedContextName_, contextName, len);
    *(std::end (fSavedContextName_) - 1) = '\0';
    fSavedContextName_[len] = '\0';
    IncCount_ ();
}

unsigned int    TraceContextBumper::GetCount ()
{
    return sTraceContextDepth_;
}

void    TraceContextBumper::IncCount_ ()
{
    sTraceContextDepth_++;
}

void    TraceContextBumper::DecrCount_ ()
{
    --sTraceContextDepth_;
}

TraceContextBumper::~TraceContextBumper ()
{
    DecrCount_ ();
    if (fDoEndMarker) {
        auto    critSec { make_unique_lock (GetCritSection_ ()) };
        if (Emitter::Get ().UnputBufferedCharactersForMatchingToken (fLastWriteToken_)) {
            Emitter::Get ().EmitUnadornedText ("/>");
            Emitter::Get ().EmitUnadornedText (GetEOL<char> ());
        }
        else {
            Emitter::Get ().EmitTraceMessage (SDKSTR ("} </%s>"), fSavedContextName_);
        }
    }
}
#endif
