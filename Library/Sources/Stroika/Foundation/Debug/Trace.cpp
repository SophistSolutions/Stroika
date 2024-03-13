/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
#include "../Memory/StackBuffer.h"
#include "../Time/DateTime.h"
#include "../Time/Realtime.h"

#if qStroika_Foundation_Debug_Trace_TraceToFile
#include "../IO/FileSystem/WellKnownLocations.h"
#include "../Time/DateTime.h"
#endif

#include "Trace.h"

using namespace Stroika::Foundation;

using namespace Characters;
using namespace Debug;
using namespace Execution;

using Memory::StackBuffer;

using Debug::Private_::Emitter;

/*
 * TODO:
 *
 *      @todo   The buffering code here maybe now correct, but isn't simple/clear, so rewrite/improve...
 *              -- LGP 2011-10-03
 */

CompileTimeFlagChecker_SOURCE (Stroika::Foundation::Debug, qTraceToFile, qStroika_Foundation_Debug_Trace_TraceToFile);
CompileTimeFlagChecker_SOURCE (Stroika::Foundation::Debug, qDefaultTracingOn, qStroika_Foundation_Debug_Trace_DefaultTracingOn);

namespace {
    // This is MOSTLY to remove NEWLINES from the MIDDLE of a message - replace with kBadChar.
    const char kBadChar_ = ' ';
    void       SquishBadCharacters_ (string* s) noexcept
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
    void SquishBadCharacters_ (wstring* s) noexcept
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

namespace {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
    thread_local unsigned int tTraceContextDepth_{0}; // no need for atomic access because thread_local
#endif

    // Declared HERE instead of the template so they get shared across TYPE values for CHARTYPE
    Thread::IDType sMainThread_ = Execution::Thread::GetCurrentThreadID ();
}

/*
     ********************************************************************************
     ******************************* Debug::Private_::Emitter ***********************
     ********************************************************************************
     */

template <typename CHARTYPE>
inline void Debug::Private_::Emitter::EmitUnadornedText (const CHARTYPE* p)
{
    DoEmit_ (p);
}

/*
 ********************************************************************************
 ************************* Debug::Private_::ModuleInit_ *************************
 ********************************************************************************
 */
namespace {
    struct PrivateModuleData_ {
        recursive_mutex fModuleMutex; // see GetEmitCritSection_
        Emitter         fEmitter;

#if qStroika_Foundation_Debug_Trace_TraceToFile
        ofstream fTraceFile;
#endif

#if qStroika_Foundation_Debug_Trace_TraceToFile
        PrivateModuleData_ ()
        {
            fTraceFile.open (Debug::Private_::Emitter::GetTraceFileName ().native ().c_str (), ios::out | ios::binary);
        }
#endif
    };
    PrivateModuleData_* sModuleData_{nullptr};
}

Debug::Private_::ModuleInit_::ModuleInit_ () noexcept
{
    Assert (sModuleData_ == nullptr);
    sModuleData_ = new PrivateModuleData_ ();
}
Debug::Private_::ModuleInit_::~ModuleInit_ ()
{
    Assert (sModuleData_ != nullptr);
    delete sModuleData_;
#if qDebug
    sModuleData_ = nullptr;
#endif
}

/*
 ********************************************************************************
 ************************** Private_::Emitter ***********************************
 ********************************************************************************
 */
auto Debug::Private_::Emitter::Get () noexcept -> Emitter&
{
    auto emitFirstTime = [] () {
        // Cannot call DbgTrace or TraceContextBumper in this code (else hang cuz calls back to Emitter::Get ())
        // which is why this function takes Emitter as argument!
        sModuleData_->fEmitter.EmitTraceMessage (L"***Starting TraceLog***");
        sModuleData_->fEmitter.EmitTraceMessage (L"Starting at %s", Time::DateTime::Now ().Format ().c_str ());
#if qStroika_Foundation_Debug_Trace_TraceToFile
        sModuleData_->fEmitter.EmitTraceMessage (L"TraceFileName: %s", Characters::ToString (Emitter::GetTraceFileName ()).c_str ());
#endif
        sModuleData_->fEmitter.EmitTraceMessage (L"EXEPath=%s", Characters::ToString (Execution::GetEXEPath ()).c_str ());
        sModuleData_->fEmitter.EmitTraceMessage (L"<debug-state {>");
        sModuleData_->fEmitter.EmitTraceMessage (L"  Debug::kBuiltWithAddressSanitizer = %s",
                                                 Characters::ToString (Debug::kBuiltWithAddressSanitizer).c_str ());
        sModuleData_->fEmitter.EmitTraceMessage (L"  Debug::kBuiltWithThreadSanitizer = %s",
                                                 Characters::ToString (Debug::kBuiltWithThreadSanitizer).c_str ());
        sModuleData_->fEmitter.EmitTraceMessage (L"  Debug::kBuiltWithUndefinedBehaviorSanitizer = %s(?)",
                                                 Characters::ToString (Debug::kBuiltWithUndefinedBehaviorSanitizer).c_str ()); // warning maybe falsely reported as false on gcc
        sModuleData_->fEmitter.EmitTraceMessage (L"  Debug::IsRunningUnderValgrind () = %s",
                                                 Characters::ToString (Debug::IsRunningUnderValgrind ()).c_str ());
        sModuleData_->fEmitter.EmitTraceMessage (L"</debug-state>");
    };
    static once_flag sOnceFlag_;
    call_once (sOnceFlag_, [=] () { emitFirstTime (); });
    return sModuleData_->fEmitter;
}

#if qStroika_Foundation_Debug_Trace_TraceToFile
filesystem::path Debug::Private_::Emitter::GetTraceFileName ()
{
    auto mkTraceFileName_ = [] () -> filesystem::path {
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
        SDKString nowstr = Time::DateTime::Now ().Format (Time::DateTime::kISO8601Format).AsSDKString (); // use eISO8601 instead of eCurrentLocale cuz locale CTOR not safe to construct before main
        for (auto i = nowstr.begin (); i != nowstr.end (); ++i) {
            if (*i == ':') {
                *i = '-';
            }
            if (*i == '/' or *i == ' ') {
                *i = '_';
            }
        }
        return IO::FileSystem::WellKnownLocations::GetTemporary () /
               CString::Format (SDKSTR ("TraceLog_%s_PID#%d-%s.txt"), mfname.c_str (), (int)Execution::GetCurrentProcessID (), nowstr.c_str ());
    };
    static filesystem::path sTraceFileName_ = mkTraceFileName_ ();
    return sTraceFileName_;
}
#endif

#if qStroika_Foundation_Debug_Trace_TraceToFile
namespace {
    void Emit2File_ (const char* text) noexcept
    {
        RequireNotNull (text);
        AssertNotNull (sModuleData_);
        try {
            if (sModuleData_->fTraceFile.is_open ()) {
                sModuleData_->fTraceFile << text;
                sModuleData_->fTraceFile.flush ();
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
            Emit2File_ (String{text}.AsNarrowSDKString (eIgnoreErrors).c_str ());
        }
        catch (...) {
            AssertNotReached ();
        }
    }
}
#endif

/*
@DESCRIPTION:   <p>This function takes a 'format' argument and then any number of additional arguments - exactly
            like std::printf (). It calls std::vsprintf () internally. This can be called directly - regardless of the
             @'qStroika_Foundation_Debug_Trace_DefaultTracingOn' flag - but is typically just called indirectly by calling
             @'DbgTrace'.</p>
*/
void Debug::Private_::Emitter::EmitTraceMessage (const char* format, ...) noexcept
{
    if (TraceContextSuppressor::GetSuppressTraceInThisThread ()) {
        return;
    }
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
        WeakAssert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
    }
}

void Debug::Private_::Emitter::EmitTraceMessage (const wchar_t* format, ...) noexcept
{
    if (TraceContextSuppressor::GetSuppressTraceInThisThread ()) {
        return;
    }
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
        WeakAssert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
    }
}

auto Debug::Private_::Emitter::EmitTraceMessage (size_t bufferLastNChars, const char* format, ...) noexcept -> TraceLastBufferedWriteTokenType
{
    if (TraceContextSuppressor::GetSuppressTraceInThisThread ()) {
        return 0;
    }
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
        WeakAssert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
        return 0;
    }
}

auto Debug::Private_::Emitter::EmitTraceMessage (size_t bufferLastNChars, const wchar_t* format, ...) noexcept -> TraceLastBufferedWriteTokenType
{
    if (TraceContextSuppressor::GetSuppressTraceInThisThread ()) {
        return 0;
    }
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
        WeakAssert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
        return 0;
    }
}

//#if __cpp_lib_format >= 201907
void Debug::Private_::Emitter::EmitTraceMessageRaw2 (wstring_view users_fmt, qStroika_Foundation_Characters_FMT_PREFIX_::wformat_args&& args) noexcept
{
    try {
        EmitTraceMessageRaw (vformat (qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{users_fmt}, args));
    }
    catch (...) {
    }
}
void Debug::Private_::Emitter::EmitTraceMessageRaw2 (string_view users_fmt, qStroika_Foundation_Characters_FMT_PREFIX_::format_args&& args) noexcept
{
    try {
        Debug::Private_::Emitter::EmitTraceMessageRaw (Characters::String::FromNarrowSDKString (vformat (qStroika_Foundation_Characters_FMT_PREFIX_::string_view{users_fmt}, args)).As<wstring> ());
    }
    catch (...) {
    }
}
//#endif

void Debug::Private_::Emitter::EmitTraceMessageRaw (const wstring& raw) noexcept
{
    if (TraceContextSuppressor::GetSuppressTraceInThisThread ()) {
        return;
    }
    Thread::SuppressInterruptionInContext suppressAborts;
    try {
        wstring tmp = raw;
        SquishBadCharacters_ (&tmp);
        AssureHasLineTermination (&tmp);
        DoEmitMessage_ (0, Containers::Start (tmp), Containers::End (tmp));
    }
    catch (...) {
        WeakAssert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
    }
}

namespace {
    // .first is true iff added, and false if already present
    // .second is the threadid to display
    pair<bool, string> mkThreadLabelForThreadID_ (const Thread::IDType& threadID)
    {
        if (qStroika_Foundation_Debug_Trace_ShowThreadIndex) {
            static atomic<int> sMinWidth_       = 4; // for MAIN
            bool               wasNew           = false;
            unsigned int       threadIndex2Show = Thread::IndexRegistrar::sThe.GetIndex (threadID, &wasNew);
            if (wasNew) {
                if (threadIndex2Show >= 10000) {
                    sMinWidth_.store (5); // could enhance if we anticipate more threads
                }
            }
            if (threadID == sMainThread_) {
                static const string kMAIN_{"MAIN"sv};
#if qCompilerAndStdLib_Debug32Codegen_make_pair_string_Buggy
                auto r   = make_pair (wasNew, kMAIN_);
                r.second = "MAIN";
                return r;
#else
                return make_pair (wasNew, kMAIN_);
#endif
            }
            else {
                char buf[1024];
                (void)::snprintf (buf, Memory::NEltsOf (buf), "%.*d", sMinWidth_.load (), threadIndex2Show);
                return make_pair (wasNew, buf);
            }
        }
        else {
            // If this is deemed useful, then re-instate the mapping of threadID == sMainThread_ to "MAIN" with appropriate -- around it
            return make_pair (false, Thread::FormatThreadID_A (threadID));
        }
    }
}
template <typename CHARTYPE>
auto Debug::Private_::Emitter::DoEmitMessage_ (size_t bufferLastNChars, const CHARTYPE* s, const CHARTYPE* e) -> TraceLastBufferedWriteTokenType
{
    [[maybe_unused]] lock_guard critSec{sModuleData_->fModuleMutex};
    FlushBufferedCharacters_ ();

    auto curRelativeTime = Time::DisplayedRealtimeClock::now (); // same as Time::clock_cast<Time::DisplayedRealtimeClock> (Time::GetTickCount ())

    {
        char               buf[1024];
        Thread::IDType     threadID     = Execution::Thread::GetCurrentThreadID ();
        pair<bool, string> threadIDInfo = mkThreadLabelForThreadID_ (threadID);
        Verify (::snprintf (buf, Memory::NEltsOf (buf), "[%s][%08.3f]\t", threadIDInfo.second.c_str (),
                            static_cast<double> (curRelativeTime.time_since_epoch ().count ())) > 0);
        if (threadIDInfo.first) {
            char buf2[1024]; // intentionally un-initialized
            Verify (snprintf (buf2, Memory::NEltsOf (buf2), "(NEW THREAD, index=%s Real Thread ID=%s)\t", threadIDInfo.second.c_str (),
                              Thread::FormatThreadID_A (threadID).c_str ()) > 0);
#if __STDC_WANT_SECURE_LIB__
            (void)::strcat_s (buf, buf2);
#else
            (void)::strcat (buf, buf2);
#endif
#if qPlatform_POSIX
            Verify (::snprintf (buf2, Memory::NEltsOf (buf2), "(pthread_self=0x%lx)\t", (unsigned long)pthread_self ()) > 0);
#if __STDC_WANT_SECURE_LIB__
            (void)::strcat_s (buf, buf2);
#else
            (void)::strcat (buf, buf2);
#endif
#endif
        }
        Assert (::strlen (buf) < Memory::NEltsOf (buf) / 2); // really just needs to be <, but since this buffer unchecked, break if we get CLOSE
        DoEmit_ (buf);
    }
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
    unsigned int contextDepth = TraceContextBumper::GetCount ();
    for (unsigned int i = 0; i < contextDepth; ++i) {
        DoEmit_ (L"\t");
    }
#endif
    if (bufferLastNChars == 0) {
        DoEmit_ (s, e);
        ++fLastNCharBuf_Token_; // even if not buffering, increment, so other buffers known to be invalid
    }
    else {
        Assert ((e - s) > static_cast<ptrdiff_t> (bufferLastNChars));
        BufferNChars_ (bufferLastNChars, e - bufferLastNChars);
        DoEmit_ (s, e - bufferLastNChars);
        fLastNCharBuf_WriteTickcount_ = curRelativeTime;
        ++fLastNCharBuf_Token_; // even if not buffering, increment, so other buffers known to be invalid
    }
    return fLastNCharBuf_Token_;
}

void Debug::Private_::Emitter::BufferNChars_ (size_t bufferLastNChars, const char* p)
{
    Assert (bufferLastNChars < Memory::NEltsOf (fLastNCharBuf_CHAR_));
    fLastNCharBufCharCount_ = bufferLastNChars;
    (void)::memcpy (fLastNCharBuf_CHAR_, p, bufferLastNChars); // no need to nul-terminate because fLastNCharBufCharCount_ stores length
    fLastNCharBuf_WCHARFlag_ = false;
}

void Debug::Private_::Emitter::BufferNChars_ (size_t bufferLastNChars, const wchar_t* p)
{
    Assert (bufferLastNChars < Memory::NEltsOf (fLastNCharBuf_WCHAR_));
    fLastNCharBufCharCount_ = bufferLastNChars;
    (void)::memcpy (fLastNCharBuf_WCHAR_, p, bufferLastNChars * sizeof (wchar_t)); // no need to nul-terminate because fLastNCharBufCharCount_ stores length
    fLastNCharBuf_WCHARFlag_ = true;
}

void Debug::Private_::Emitter::FlushBufferedCharacters_ ()
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

bool Debug::Private_::Emitter::UnputBufferedCharactersForMatchingToken (TraceLastBufferedWriteTokenType token)
{
    RequireNotNull (sModuleData_);
    [[maybe_unused]] lock_guard critSec{sModuleData_->fModuleMutex};
    // If the fLastNCharBuf_Token_ matches (no new tokens written since the saved one) and the time
    // hasn't been too long (we currently write 1/100th second timestamp resolution).
    // then blank unput (ignore) buffered characters, and return true so caller knows to write
    // funky replacement for those characters.
    if (fLastNCharBuf_Token_ == token and (Time::DisplayedRealtimeClock::now () - fLastNCharBuf_WriteTickcount_ < 20ms)) {
        fLastNCharBufCharCount_ = 0;
        return true;
    }
    return false; // assume old behavior for now
}

void Debug::Private_::Emitter::DoEmit_ (const char* p) noexcept
{
#if qPlatform_Windows
    constexpr size_t kMaxLen_ = 1023; // no docs on limit, but various hints the limit is somewhere between 1k and 4k. Empirically - just chops off after a point...
    if (::strlen (p) < kMaxLen_) {
        ::OutputDebugStringA (p);
    }
    else {
        char buf[1024]; // @todo if/when we always support constexpr can use that here!
        (void)::memcpy (buf, p, sizeof (buf));
        buf[Memory::NEltsOf (buf) - 1] = 0;
        ::OutputDebugStringA (buf);
        ::OutputDebugStringA ("...");
        ::OutputDebugStringA (GetEOL<char> ());
    }
#endif
#if qStroika_Foundation_Debug_Trace_TraceToFile
    Emit2File_ (p);
#endif
}

void Debug::Private_::Emitter::DoEmit_ (const wchar_t* p) noexcept
{
#if qPlatform_Windows
    constexpr size_t kMaxLen_ = 1023; // no docs on limit, but various hints the limit is somewhere between 1k and 4k. Empirically - just chops off after a point...
    if (::wcslen (p) < kMaxLen_) {
        ::OutputDebugStringW (p);
    }
    else {
        wchar_t buf[1024]; // @todo if/when we always support constexpr can use that here!
        (void)::memcpy (buf, p, sizeof (buf));
        buf[Memory::NEltsOf (buf) - 1] = 0;
        ::OutputDebugStringW (buf);
        ::OutputDebugStringW (L"...");
        ::OutputDebugStringW (GetEOL<wchar_t> ());
    }
#endif
#if qStroika_Foundation_Debug_Trace_TraceToFile
    Emit2File_ (p);
#endif
}

void Debug::Private_::Emitter::DoEmit_ (const char* p, const char* e) noexcept
{
    try {
        size_t            len = e - p;
        StackBuffer<char> buf{Memory::eUninitialized, len + 1};
        (void)::memcpy (buf.begin (), p, len);
        buf.begin ()[len] = '\0';
        DoEmit_ (buf.begin ());
    }
    catch (...) {
        AssertNotReached ();
    }
}

/*
    [Succeeded]  (1  seconds)  [42]  Foundation::Execution::Other (scp ../Builds/raspberrypi-g++-12-release-sanitize_address/Tests/Test42...; ssh lewis@192.168.244.20   /tmp/Test42)
=================================================================
==24967==ERROR: AddressSanitizer: stack-use-after-scope on address 0xbefb9b80 at pc 0x0048c10f bp 0xbefb9688 sp 0xbefb9694
WRITE of size 100 at 0xbefb9b80 thread T0
    #0 0x48c10c in __interceptor_memcpy (/tmp/Test43+0x6c10c)
    #1 0x6189ee in memcpy /usr/arm-linux-gnueabihf/include/bits/string_fortified.h:29
    #2 0x6189ee in Stroika::Foundation::Debug::Private_::Emitter::DoEmit_(wchar_t const*, wchar_t const*) /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:553
    #3 0x6236c8 in int Stroika::Foundation::Debug::Private_::Emitter::DoEmitMessage_<wchar_t>(unsigned int, wchar_t const*, wchar_t const*) /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:432
    #4 0x61928e in Stroika::Foundation::Debug::Private_::Emitter::EmitTraceMessage(wchar_t const*, ...) /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:299
    #5 0x6198ba in Stroika::Foundation::Debug::TraceContextBumper::~TraceContextBumper() /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:645
    #6 0x6198ba in Stroika::Foundation::Debug::TraceContextBumper::~TraceContextBumper() /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:634
    #7 0x5423cc in DoTests_ /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Tests/43/Test.cpp:376
    #8 0x54cb52 in DoRegressionTests_ /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Tests/43/Test.cpp:568
    #9 0x56ac3c in Stroika::TestHarness::PrintPassOrFail(void (*)()) ../TestHarness/TestHarness.cpp:89
    #10 0xb6d1a3bc in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #11 0xb6d1a4c4 in __libc_start_main_impl csu/libc-start.c:360

Address 0xbefb9b80 is located in stack of thread T0 at offset 128 in frame
    #0 0x6188e8 in Stroika::Foundation::Debug::Private_::Emitter::DoEmit_(wchar_t const*, wchar_t const*) /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:549

  This frame has 1 object(s):
    [48, 4152) 'buf' (line 552) <== Memory access at offset 128 is inside this variable
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork
*/
#if qCompilerAndStdLib_arm_asan_FaultStackUseAfterScope_Buggy
Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif
    void
    Debug::Private_::Emitter::DoEmit_ (const wchar_t* p, const wchar_t* e) noexcept
{
    try {
        size_t               len = e - p;
        StackBuffer<wchar_t> buf{Memory::eUninitialized, len + 1};
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
 ************************ Debug::GetDbgTraceThreadName_A ************************
 ********************************************************************************
 */
string Debug::GetDbgTraceThreadName_A (thread::id threadID)
{
    return mkThreadLabelForThreadID_ (threadID).second;
}

/*
 ********************************************************************************
 ****************************** TraceContextBumper ******************************
 ********************************************************************************
 */
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
TraceContextBumper::TraceContextBumper (const wchar_t* contextName) noexcept
    : fDoEndMarker{true}
{
    fLastWriteToken_ = Private_::Emitter::Get ().EmitTraceMessage (3 + ::wcslen (GetEOL<wchar_t> ()), L"<%s> {", contextName);
    size_t len       = min (Memory::NEltsOf (fSavedContextName_) - 1, char_traits<wchar_t>::length (contextName));
    char_traits<wchar_t>::copy (fSavedContextName_, contextName, len);
    if (len >= Memory::NEltsOf (fSavedContextName_) - 1) {
        char_traits<wchar_t>::copy (&fSavedContextName_[len - 3], L"...", 3);
    }
    *(std::end (fSavedContextName_) - 1) = '\0';
    fSavedContextName_[len]              = '\0';
    IncCount_ ();
}

TraceContextBumper::TraceContextBumper (const wchar_t* contextName, const wchar_t* extraFmt, ...) noexcept
    : fDoEndMarker{true}
{
    try {
        va_list argsList;
        va_start (argsList, extraFmt);
        fLastWriteToken_ = Emitter::Get ().EmitTraceMessage (3 + ::wcslen (GetEOL<wchar_t> ()), L"<%s (%s)> {", contextName,
                                                             Characters::CString::FormatV (extraFmt, argsList).c_str ());
        va_end (argsList);
        size_t len = min (Memory::NEltsOf (fSavedContextName_) - 1, char_traits<wchar_t>::length (contextName));
        char_traits<wchar_t>::copy (fSavedContextName_, contextName, len);
        if (len >= Memory::NEltsOf (fSavedContextName_) - 1) {
            char_traits<wchar_t>::copy (&fSavedContextName_[len - 3], L"...", 3);
        }
        *(std::end (fSavedContextName_) - 1) = '\0';
        fSavedContextName_[len]              = '\0';
        IncCount_ ();
    }
    catch (...) {
    }
}

TraceContextBumper::TraceContextBumper (const char* contextName) noexcept
    : TraceContextBumper{mkwtrfromascii_ (contextName).data ()}
{
}

unsigned int TraceContextBumper::GetCount ()
{
    return tTraceContextDepth_;
}

void TraceContextBumper::IncCount_ () noexcept
{
    ++tTraceContextDepth_;
}

void TraceContextBumper::DecrCount_ () noexcept
{
    --tTraceContextDepth_;
}

TraceContextBumper::~TraceContextBumper ()
{
    DecrCount_ ();
    if (fDoEndMarker) {
        RequireNotNull (sModuleData_);
        [[maybe_unused]] lock_guard critSec{sModuleData_->fModuleMutex};
        if (Emitter::Get ().UnputBufferedCharactersForMatchingToken (fLastWriteToken_)) {
            Emitter::Get ().EmitUnadornedText ("/>");
            Emitter::Get ().EmitUnadornedText (GetEOL<char> ());
        }
        else {
            Emitter::Get ().EmitTraceMessage (L"} </%s>", fSavedContextName_);
        }
    }
}

auto TraceContextBumper::mkwtrfromascii_ (const char* contextName) -> array<wchar_t, kMaxContextNameLen_ + 1>
{
    // Return item with max size kMaxContextNameLen_+1 so we can tell if we need to add elipsis
    array<wchar_t, kMaxContextNameLen_ + 1> r;
    auto                                    ci = contextName;
    for (; *ci != '\0'; ++ci) {
        Require (isascii (*ci));
        size_t i = ci - contextName;
        if (i < kMaxContextNameLen_) {
            r[i] = *ci;
        }
        else {
            break;
        }
    }
    Assert (ci - contextName <= kMaxContextNameLen_);
    r[ci - contextName] = '\0';
    return r;
}
#endif
