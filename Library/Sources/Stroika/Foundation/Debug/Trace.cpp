/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <map>
#include <mutex>

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"
#include "../Characters/LineEndings.h"
#include "../Characters/ToString.h"
#include "../Debug/Sanitizer.h"
#include "../Debug/Valgrind.h"
#include "../Execution/Common.h"
#include "../Execution/Module.h"
#include "../Execution/Process.h"
#include "../Execution/Thread.h"
#include "../IO/FileSystem/PathName.h"
#include "../Memory/Common.h"
#include "../Time/Realtime.h"

#if qTraceToFile
#include "../IO/FileSystem/WellKnownLocations.h"
#include "../Time/DateTime.h"
#endif

#include "Trace.h"

using namespace Stroika::Foundation;

using namespace Characters;
using namespace Debug;
using namespace Execution;

using Memory::SmallStackBuffer;
using Memory::SmallStackBufferCommon;

/*
 * TODO:
 *
 *      @todo   The buffering code here maybe now correct, but isn't simple/clear, so rewrite/improve...
 *              -- LGP 2011-10-03
 */

CompileTimeFlagChecker_SOURCE (Stroika::Foundation::Debug, qTraceToFile, qTraceToFile);
CompileTimeFlagChecker_SOURCE (Stroika::Foundation::Debug, qDefaultTracingOn, qDefaultTracingOn);

namespace {
    // This is MOSTLY to remove NEWLINES from the MIDDLE of a message - replace with kBadChar.
    const char kBadChar_ = ' ';
    void       SquishBadCharacters_ (string* s)
    {
        RequireNotNull (s);
        size_t end = s->length ();
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
    void SquishBadCharacters_ (wstring* s)
    {
        RequireNotNull (s);
        size_t end = s->length ();
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

/*
 ********************************************************************************
 ************************ Private_::TraceModuleData_ ****************************
 ********************************************************************************
 */
namespace {
    recursive_mutex* sEmitTraceCritSec_ = nullptr;
#if qTraceToFile
    ofstream* sTraceFile = nullptr;
#endif
#if qDefaultTracingOn
    thread_local unsigned int tTraceContextDepth_{0}; // no need for atomic access because thread_local
#endif

    // Declared HERE instead of the template so they get shared across TYPE values for CHARTYPE
    Thread::IDType sMainThread_ = Execution::GetCurrentThreadID ();
}

#if qTraceToFile
namespace {
    filesystem::path mkTraceFileName_ ()
    {
        // Use TempDir instead of EXEDir because on vista, installation permissions prevent us from (easily) writing in EXEDir.
        // (could fix of course, but I'm not sure desirable - reasonable defaults)
        //
        // Don't want to use TempFileLibrarian cuz we don't want these deleted on app exit
        SDKString mfname;
        {
            try {
                mfname = Execution::GetEXEPath ().native ();
            }
            catch (...) {
                mfname = SDKSTR ("{unknown}");
            }
            {
                size_t i = mfname.rfind (IO::FileSystem::kPathComponentSeperator);
                if (i != SDKString::npos) {
                    mfname = mfname.substr (i + 1);
                }
                i = mfname.rfind ('.');
                if (i != SDKString::npos) {
                    mfname.erase (i);
                }
            }
            for (auto i = mfname.begin (); i != mfname.end (); ++i) {
                if (*i == ' ') {
                    *i = '-';
                }
            }
        }
        SDKString nowstr = Time::DateTime::Now ().Format (Time::DateTime::PrintFormat::eISO8601).AsSDKString (); // use eISO8601 instead of eCurrentLocale cuz locale CTOR not safe to construct before main
        for (auto i = nowstr.begin (); i != nowstr.end (); ++i) {
            if (*i == ':') {
                *i = '-';
            }
            if (*i == '/' or *i == ' ') {
                *i = '_';
            }
        }
        return IO::FileSystem::WellKnownLocations::GetTemporary () / CString::Format (SDKSTR ("TraceLog_%s_PID#%d-%s.txt"), mfname.c_str (), (int)Execution::GetCurrentProcessID (), nowstr.c_str ());
    }
}
#endif

Debug::Private_::TraceModuleData_::TraceModuleData_ ()
    : fEmitter ()
#if qTraceToFile
    , fTraceFileName (mkTraceFileName_ ())
#endif
{
    Assert (sEmitTraceCritSec_ == nullptr);
    sEmitTraceCritSec_ = new recursive_mutex ();
#if qTraceToFile
    Assert (sTraceFile == nullptr);
    sTraceFile = new ofstream ();
    sTraceFile->open (Emitter::Get ().GetTraceFileName ().c_str (), ios::out | ios::binary);
#endif
    DbgTrace (L"***Starting TraceLog***");
    TraceContextBumper ctx{L"debug-state"};
    DbgTrace (L"Debug::kBuiltWithAddressSanitizer = %s", Characters::ToString (Debug::kBuiltWithAddressSanitizer).c_str ());
    DbgTrace (L"Debug::IsRunningUnderValgrind () = %s", Characters::ToString (Debug::IsRunningUnderValgrind ()).c_str ());
}

Debug::Private_::TraceModuleData_::~TraceModuleData_ ()
{
    delete sEmitTraceCritSec_;
    sEmitTraceCritSec_ = nullptr;
#if qTraceToFile
    AssertNotNull (sTraceFile);
    sTraceFile->close ();
    delete sTraceFile;
    sTraceFile = nullptr;
#endif
}

namespace {
    inline recursive_mutex& GetEmitCritSection_ ()
    {
        EnsureNotNull (sEmitTraceCritSec_);
        return *sEmitTraceCritSec_;
    }
}

#if qTraceToFile
SDKString Emitter::GetTraceFileName () const
{
    return Execution::ModuleInitializer<Private_::TraceModuleData_>::Actual ().fTraceFileName;
}
#endif

#if qTraceToFile
namespace {
    void Emit2File_ (const char* text) noexcept
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
            AssertNotReached ();
        }
    }
    void Emit2File_ (const wchar_t* text) noexcept
    {
        RequireNotNull (text);
        try {
            Emit2File_ (WideStringToUTF8 (text).c_str ());
        }
        catch (...) {
            AssertNotReached ();
        }
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
/*
@DESCRIPTION:   <p>This function takes a 'format' argument and then any number of additional arguments - exactly
            like std::printf (). It calls std::vsprintf () internally. This can be called directly - regardless of the
             @'qDefaultTracingOn' flag - but is typically just called indirectly by calling
             @'DbgTrace'.</p>
*/
void Emitter::EmitTraceMessage (const char* format, ...) noexcept
{
    Thread::SuppressInterruptionInContext suppressAborts;
    try {
        va_list argsList;
        va_start (argsList, format);
        string tmp = Characters::CString::FormatV (format, argsList);
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

void Emitter::EmitTraceMessage (const wchar_t* format, ...) noexcept
{
    Thread::SuppressInterruptionInContext suppressAborts;
    try {
        va_list argsList;
        va_start (argsList, format);
        wstring tmp = Characters::CString::FormatV (format, argsList);
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

Emitter::TraceLastBufferedWriteTokenType Emitter::EmitTraceMessage (size_t bufferLastNChars, const char* format, ...) noexcept
{
    Thread::SuppressInterruptionInContext suppressAborts;
    try {
        va_list argsList;
        va_start (argsList, format);
        string tmp = Characters::CString::FormatV (format, argsList);
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

Emitter::TraceLastBufferedWriteTokenType Emitter::EmitTraceMessage (size_t bufferLastNChars, const wchar_t* format, ...) noexcept
{
    Thread::SuppressInterruptionInContext suppressAborts;
    try {
        va_list argsList;
        va_start (argsList, format);
        wstring tmp = Characters::CString::FormatV (format, argsList);
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

namespace {
    // .first is true iff added, and false if already present
    // .second is the threadid to display
    pair<bool, string> mkThreadLabelForThreadID_ (const Thread::IDType& threadID)
    {
        constexpr bool kEmitThreadIDsByIndex_ = true;   // if true, emit a much shorter thread ID, making - I suspect (testing)
                                                        // for terser and clearer tracelogs. Only downside is that you must find
                                                        // first occurence of that index to find real threadId, and use that in waits, etc.
                                                        // @todo MAYBE include this mapping in the THREAD class, so it can be used in printing
                                                        // info about threads
        if (kEmitThreadIDsByIndex_) {
            [[maybe_unused]] auto&& critSec = lock_guard{GetEmitCritSection_ ()};
            char                    buf[1024];
            static map<Thread::IDType, unsigned int> sShownThreadIDs_;
            static int                               sMinWidth_       = 4; // for MAIN
            auto                                     i                = sShownThreadIDs_.find (threadID);
            unsigned int                             threadIndex2Show = 0;
            if (i == sShownThreadIDs_.end ()) {
                threadIndex2Show = sShownThreadIDs_.size ();
                sShownThreadIDs_.insert (pair<Thread::IDType, unsigned int>{threadID, threadIndex2Show}).second;
                if (threadIndex2Show >= 10000) {
                    sMinWidth_ = 5;
                }
            }
            else {
                threadIndex2Show = i->second;
            }
            if (threadID == sMainThread_) {
                static string kMAIN_{"main"sv};
                return pair<bool, string>{i == sShownThreadIDs_.end (), kMAIN_};
            }
            (void)snprintf (buf, NEltsOf (buf), "%.*d", sMinWidth_, threadIndex2Show);
            return pair<bool, string>{i == sShownThreadIDs_.end (), buf};
        }
        else {
            // If this is deemed useful, then re-instate the mapping of threadID == sMainThread_ to "MAIN" with appropriate -- around it
            return pair<bool, string>{false, FormatThreadID_A (threadID)};
        }
    }
}
template <typename CHARTYPE>
Emitter::TraceLastBufferedWriteTokenType Emitter::DoEmitMessage_ (size_t bufferLastNChars, const CHARTYPE* s, const CHARTYPE* e)
{
    [[maybe_unused]] auto&& critSec = lock_guard{GetEmitCritSection_ ()};
    FlushBufferedCharacters_ ();
    Time::DurationSecondsType curRelativeTime = Time::GetTickCount ();
    {
        char           buf[1024];
        Thread::IDType threadID    = Execution::GetCurrentThreadID ();
        pair<bool, string> threadIDInfo = mkThreadLabelForThreadID_ (threadID);
        Verify (snprintf (buf, NEltsOf (buf), "[%s][%08.3f]\t", threadIDInfo.second.c_str (), static_cast<double> (curRelativeTime)) > 0);
        if (threadIDInfo.first) {
                char buf2[1024];
            Verify (snprintf (buf2, NEltsOf (buf2), "(NEW THREAD, index=%s Real Thread ID=%s)\t", threadIDInfo.second.c_str (), FormatThreadID_A (threadID).c_str ()) > 0);
#if __STDC_WANT_SECURE_LIB__
                strcat_s (buf, buf2);
#else
                strcat (buf, buf2);
#endif
#if qPlatform_POSIX
                Verify (snprintf (buf2, NEltsOf (buf2), "(pthread_self=0x%lx)\t", (unsigned long)pthread_self ()) > 0);
#if __STDC_WANT_SECURE_LIB__
                strcat_s (buf, buf2);
#else
                strcat (buf, buf2);
#endif
#endif
        }
        DoEmit_ (buf);
    }
#if qDefaultTracingOn
    unsigned int contextDepth = TraceContextBumper::GetCount ();
    for (unsigned int i = 0; i < contextDepth; ++i) {
        DoEmit_ (L"\t");
    }
#endif
    if (bufferLastNChars == 0) {
        DoEmit_ (s, e);
        fLastNCharBuf_Token_++; // even if not buffering, increment, so other buffers known to be invalid
    }
    else {
        Assert ((e - s) > static_cast<ptrdiff_t> (bufferLastNChars));
        BufferNChars_ (bufferLastNChars, e - bufferLastNChars);
        DoEmit_ (s, e - bufferLastNChars);
        fLastNCharBuf_WriteTickcount_ = curRelativeTime;
        fLastNCharBuf_Token_++; // even if not buffering, increment, so other buffers known to be invalid
    }
    return fLastNCharBuf_Token_;
}

void Emitter::BufferNChars_ (size_t bufferLastNChars, const char* p)
{
    Assert (bufferLastNChars < NEltsOf (fLastNCharBuf_CHAR_));
    fLastNCharBufCharCount_ = bufferLastNChars;
    memcpy (fLastNCharBuf_CHAR_, p, bufferLastNChars); // no need to nul-terminate because fLastNCharBufCharCount_ stores length
    fLastNCharBuf_WCHARFlag_ = false;
}

void Emitter::BufferNChars_ (size_t bufferLastNChars, const wchar_t* p)
{
    Assert (bufferLastNChars < NEltsOf (fLastNCharBuf_WCHAR_));
    fLastNCharBufCharCount_ = bufferLastNChars;
    memcpy (fLastNCharBuf_WCHAR_, p, bufferLastNChars * sizeof (wchar_t)); // no need to nul-terminate because fLastNCharBufCharCount_ stores length
    fLastNCharBuf_WCHARFlag_ = true;
}

void Emitter::FlushBufferedCharacters_ ()
{
    if (fLastNCharBufCharCount_ != 0) {
        if (fLastNCharBuf_WCHARFlag_) {
            DoEmit_ (fLastNCharBuf_WCHAR_, fLastNCharBuf_WCHAR_ + fLastNCharBufCharCount_);
        }
        else {
            DoEmit_ (fLastNCharBuf_CHAR_, fLastNCharBuf_CHAR_ + fLastNCharBufCharCount_);
        }
        fLastNCharBufCharCount_ = 0;
    }
}

bool Emitter::UnputBufferedCharactersForMatchingToken (TraceLastBufferedWriteTokenType token)
{
    [[maybe_unused]] auto&& critSec = lock_guard{GetEmitCritSection_ ()};
    // If the fLastNCharBuf_Token_ matches (no new tokens written since the saved one) and the time
    // hasn't been too long (we currently write 1/100th second timestamp resolution).
    // then blank unput (ignore) buffered characters, and return true so caller knows to write
    // funky replacement for those characters.
    if (fLastNCharBuf_Token_ == token and (Time::GetTickCount () - fLastNCharBuf_WriteTickcount_ < 0.02f)) {
        fLastNCharBufCharCount_ = 0;
        return true;
    }
    return false; // assume old behavior for now
}

void Emitter::DoEmit_ (const char* p) noexcept
{
#if qPlatform_Windows
    constexpr size_t kMaxLen_ = 1023; // no docs on limit, but various hints the limit is somewhere between 1k and 4k. Empirically - just chops off after a point...
    if (::strlen (p) < kMaxLen_) {
        ::OutputDebugStringA (p);
    }
    else {
        char buf[1024]; // @todo if/when we always support constexpr can use that here!
        (void)::memcpy (buf, p, sizeof (buf));
        buf[NEltsOf (buf) - 1] = 0;
        ::OutputDebugStringA (buf);
        ::OutputDebugStringA ("...");
        ::OutputDebugStringA (GetEOL<char> ());
    }
#endif
#if qTraceToFile
    Emit2File_ (p);
#endif
}

void Emitter::DoEmit_ (const wchar_t* p) noexcept
{
#if qPlatform_Windows
    constexpr size_t kMaxLen_ = 1023; // no docs on limit, but various hints the limit is somewhere between 1k and 4k. Empirically - just chops off after a point...
    if (::wcslen (p) < kMaxLen_) {
        ::OutputDebugStringW (p);
    }
    else {
        wchar_t buf[1024]; // @todo if/when we always support constexpr can use that here!
        (void)::memcpy (buf, p, sizeof (buf));
        buf[NEltsOf (buf) - 1] = 0;
        ::OutputDebugStringW (buf);
        ::OutputDebugStringW (L"...");
        ::OutputDebugStringW (GetEOL<wchar_t> ());
    }
#endif
#if qTraceToFile
    Emit2File_ (p);
#endif
}

void Emitter::DoEmit_ (const char* p, const char* e) noexcept
{
    try {
        size_t                 len = e - p;
        SmallStackBuffer<char> buf (SmallStackBufferCommon::eUninitialized, len + 1);
        (void)::memcpy (buf.begin (), p, len);
        buf.begin ()[len] = '\0';
        DoEmit_ (buf.begin ());
    }
    catch (...) {
        AssertNotReached ();
    }
}

void Emitter::DoEmit_ (const wchar_t* p, const wchar_t* e) noexcept
{
    try {
        size_t                    len = e - p;
        SmallStackBuffer<wchar_t> buf (SmallStackBufferCommon::eUninitialized, len + 1);
        (void)::memcpy (buf.begin (), p, len * sizeof (wchar_t));
        buf.begin ()[len] = '\0';
        DoEmit_ (buf.begin ());
    }
    catch (...) {
        AssertNotReached ();
    }
}

/*
 ********************************************************************************
 ****************************** TraceContextBumper ******************************
 ********************************************************************************
 */
#if qDefaultTracingOn
TraceContextBumper::TraceContextBumper (const wchar_t* contextName) noexcept
    : fDoEndMarker (true)
{
    fLastWriteToken_ = Emitter::Get ().EmitTraceMessage (3 + ::wcslen (GetEOL<wchar_t> ()), L"<%s> {", contextName);
    size_t len       = min (NEltsOf (fSavedContextName_) - 1, char_traits<wchar_t>::length (contextName));
    char_traits<wchar_t>::copy (fSavedContextName_, contextName, len);
    *(std::end (fSavedContextName_) - 1) = '\0';
    fSavedContextName_[len]              = '\0';
    IncCount_ ();
}

TraceContextBumper::TraceContextBumper (const wchar_t* contextName, const wchar_t* extraFmt, ...) noexcept
    : fDoEndMarker (true)
{
    try {
        va_list argsList;
        va_start (argsList, extraFmt);
        fLastWriteToken_ = Emitter::Get ().EmitTraceMessage (3 + ::wcslen (GetEOL<wchar_t> ()), L"<%s (%s)> {", contextName, Characters::CString::FormatV (extraFmt, argsList).c_str ());
        va_end (argsList);
        size_t len = min (NEltsOf (fSavedContextName_) - 1, char_traits<wchar_t>::length (contextName));
        char_traits<wchar_t>::copy (fSavedContextName_, contextName, len);
        *(std::end (fSavedContextName_) - 1) = '\0';
        fSavedContextName_[len]              = '\0';
        IncCount_ ();
    }
    catch (...) {
    }
}

TraceContextBumper::TraceContextBumper (const char* contextName) noexcept
    : TraceContextBumper (mkwtrfromascii_ (contextName).data ())
{
}

unsigned int TraceContextBumper::GetCount ()
{
    return tTraceContextDepth_;
}

void TraceContextBumper::IncCount_ () noexcept
{
    tTraceContextDepth_++;
}

void TraceContextBumper::DecrCount_ () noexcept
{
    --tTraceContextDepth_;
}

TraceContextBumper::~TraceContextBumper ()
{
    DecrCount_ ();
    if (fDoEndMarker) {
        [[maybe_unused]] auto&& critSec = lock_guard{GetEmitCritSection_ ()};
        if (Emitter::Get ().UnputBufferedCharactersForMatchingToken (fLastWriteToken_)) {
            Emitter::Get ().EmitUnadornedText ("/>");
            Emitter::Get ().EmitUnadornedText (GetEOL<char> ());
        }
        else {
            Emitter::Get ().EmitTraceMessage (L"} </%s>", fSavedContextName_);
        }
    }
}

auto TraceContextBumper::mkwtrfromascii_ (const char* contextName) -> array<wchar_t, kMaxContextNameLen_>
{
    array<wchar_t, kMaxContextNameLen_> r;
    auto                                ci = contextName;
    for (; *ci != '\0'; ++ci) {
        Require (isascii (*ci));
        size_t i = ci - contextName;
        if (i < kMaxContextNameLen_ - 1) {
            r[i] = *ci;
        }
        else {
            break;
        }
    }
    Assert (ci - contextName < kMaxContextNameLen_);
    r[ci - contextName] = '\0';
    return r;
}
#endif
