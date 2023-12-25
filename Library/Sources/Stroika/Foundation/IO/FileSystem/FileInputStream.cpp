/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if qPlatform_Windows
#include <io.h>
#elif qPlatform_POSIX
#include <poll.h>
#include <unistd.h>
#endif

#include "../../Characters/Format.h"
#include "../../Characters/ToString.h"
#include "../../Debug/AssertExternallySynchronizedMutex.h"
#include "../../Debug/Trace.h"
#include "../../Execution/Activity.h"
#include "../../Execution/Common.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Throw.h"
#if qPlatform_POSIX
#include "../../Execution/Exceptions.h"
#elif qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#endif
#include "../../Streams/BufferedInputStream.h"

#include "Exception.h"

#include "FileInputStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::IO::FileSystem::FileInputStream;
using namespace Stroika::Foundation::Streams;

using Streams::SeekOffsetType;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    class Rep_ : public InputStream::IRep<byte> /*, public Memory::UseBlockAllocationIfAppropriate<Rep_>*/ { // @todo figure out why not working!!!
    public:
        Rep_ ()            = delete;
        Rep_ (const Rep_&) = delete;
        Rep_ (const filesystem::path& fileName, SeekableFlag seekable)
            : fFD_{-1}
            , fSeekable_{seekable}
            , fFileName_{fileName}
        {
            auto activity = LazyEvalActivity{
                [&] () -> String { return Characters::Format (L"opening %s for read access", Characters::ToString (fFileName_).c_str ()); }};
            DeclareActivity currentActivity{&activity};
#if qPlatform_Windows
            errno_t e = ::_wsopen_s (&fFD_, fileName.c_str (), (O_RDONLY | O_BINARY), _SH_DENYNO, 0);
            if (e != 0) {
                FileSystem::Exception::ThrowPOSIXErrNo (e, fileName);
            }
            if (fFD_ == -1) {
                FileSystem::Exception::ThrowSystemErrNo (fileName);
            }
#else
            FileSystem::Exception::ThrowPOSIXErrNoIfNegative (fFD_ = ::open (fileName.generic_string ().c_str (), O_RDONLY), fileName);
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"opened fd: %d", fFD_);
#endif
        }
        Rep_ (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable)
            : fFD_{fd}
            , fSeekable_{seekable}
            , fAdoptFDPolicy_{adoptFDPolicy}
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"attached fd: %d", fFD_);
#endif
        }
        ~Rep_ ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"FileInputStream::Rep_::~Rep_"};
            if (fAdoptFDPolicy_ == AdoptFDPolicy::eCloseOnDestruction and IsOpenRead ()) {
                DbgTrace (L"closing %d", fFD_);
            }
#endif
            if (fAdoptFDPolicy_ == AdoptFDPolicy::eCloseOnDestruction and IsOpenRead ()) {
#if qPlatform_Windows
                ::_close (fFD_);
#else
                ::close (fFD_);
#endif
            }
        }
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        virtual bool IsSeekable () const override
        {
            return fSeekable_ == eSeekable;
        }
        virtual void CloseRead () override
        {
            Require (IsOpenRead ());
            if (fAdoptFDPolicy_ == AdoptFDPolicy::eCloseOnDestruction) {
#if qPlatform_Windows
                ::_close (fFD_);
#else
                ::close (fFD_);
#endif
            }
            fFD_ = -1;
        }
        virtual bool IsOpenRead () const override
        {
            return fFD_ >= 0;
        }
        virtual size_t Read (byte* intoStart, byte* intoEnd) override
        {
            RequireNotNull (intoStart);
            RequireNotNull (intoEnd);
            Require (intoStart < intoEnd);
            size_t nRequested = intoEnd - intoStart;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"FileInputStream::Rep_::Read", L"nRequested: %llu", static_cast<unsigned long long> (nRequested)};
#endif
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            auto                                            readingFromFileActivity = LazyEvalActivity{
                [&] () -> String { return Characters::Format (L"reading from %s", Characters::ToString (fFileName_).c_str ()); }};
            DeclareActivity currentActivity{&readingFromFileActivity};
#if qPlatform_Windows
            return static_cast<size_t> (ThrowPOSIXErrNoIfNegative (::_read (fFD_, intoStart, Math::PinToMaxForType<unsigned int> (nRequested))));
#else
            return static_cast<size_t> (ThrowPOSIXErrNoIfNegative (::read (fFD_, intoStart, nRequested)));
#endif
        }
        virtual optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
        {
            Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
#if qPlatform_Windows
            /*
         *  For now, assume all FILE reads are already non-blocking. Not sure about this.
         *
         *  COULD use intptr_t _get_osfhandle (int fd);
         *  to use Windows APIs, but those all seem to require the file to be opened a special way to do async reads.
         *
         *  Tried:
         *      int oldFileFlags = ::fcntl (fFD_, F_GETFL, 0);
         *      if (fcntl (fFD_, F_SETFL, oldFileFlags | O_NONBLOCK))
         *          ;
         *      [[maybe_unused]] auto&& cleanup = Execution::Finally ([this]() noexcept {
         *          fcntl (fFD_, F_SETFL, oldFileFlags);
         *      });
         *
         *  but windows doesn't appear to support fcntl()
         */
            return Read (intoStart, intoEnd);
#elif qPlatform_POSIX
            pollfd pollData{fFD_, POLLIN, 0};
            int    pollResult = Execution::Handle_ErrNoResultInterruption ([&] () { return ::poll (&pollData, 1, 0); });
            Assert (pollResult >= 0);
            if (pollResult == 0) {
                return nullopt; // if no data available, return nullopt
            }
            else {
                // we don't know how much is available, but at least one byte. If not actually reading, just return 1
                if (intoStart == nullptr) {
                    return 1;
                }
                else {
                    // if there is data available, read as much as you can...
                    return Read (intoStart, intoEnd);
                }
            }
#endif
            return {};
        }
        virtual Streams::SeekOffsetType GetReadOffset () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
#if qPlatform_Windows
            return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, 0, SEEK_CUR)));
#elif qPlatform_Linux
            return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, 0, SEEK_CUR)));
#else
            return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, 0, SEEK_CUR)));
#endif
        }
        virtual Streams::SeekOffsetType SeekRead (Streams::Whence whence, Streams::SignedSeekOffsetType offset) override
        {
            using namespace Streams;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"FileInputStream::Rep_::SeekRead", L"whence: %d, offset: %lld", whence, static_cast<long long> (offset)};
#endif
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            switch (whence) {
                case Whence::eFromStart: {
                    if (offset < 0) [[unlikely]] {
                        Execution::Throw (range_error{"seek"});
                    }
#if qPlatform_Windows
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_SET)));
#elif qPlatform_Linux
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_SET)));
#else
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, offset, SEEK_SET)));
#endif
                } break;
                case Whence::eFromCurrent: {
#if qPlatform_Windows
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_CUR)));
#elif qPlatform_Linux
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_CUR)));
#else
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, offset, SEEK_CUR)));
#endif
                } break;
                case Whence::eFromEnd: {
#if qPlatform_Windows
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_END)));
#elif qPlatform_Linux
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_END)));
#else
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, offset, SEEK_END)));
#endif
                } break;
            }
            RequireNotReached ();
            return 0;
        }

    private:
        int                                                     fFD_;
        SeekableFlag                                            fSeekable_;
        AdoptFDPolicy                                           fAdoptFDPolicy_{AdoptFDPolicy::eCloseOnDestruction};
        optional<filesystem::path>                              fFileName_;
        [[no_unique_address]] AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };
}

/*
 ********************************************************************************
 **************************** FileSystem::FileInputStream ***********************
 ********************************************************************************
 */
auto FileInputStream::New (const filesystem::path& fileName, SeekableFlag seekable) -> Ptr
{
    return Ptr{Memory::MakeSharedPtr<Rep_> (fileName, seekable)};
}

auto FileInputStream::New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable) -> Ptr
{
    return Ptr{Memory::MakeSharedPtr<Rep_> (fd, adoptFDPolicy, seekable)};
}

auto FileInputStream::New (Execution::InternallySynchronized internallySynchronized, const filesystem::path& fileName, SeekableFlag seekable) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return Streams::InternallySynchronizedInputStream::New<Rep_> (fileName, seekable);
        case Execution::eNotKnownInternallySynchronized:
            return New (fileName, seekable);
        default:
            RequireNotReached ();
            return Ptr{};
    }
}

auto FileInputStream::New (Execution::InternallySynchronized internallySynchronized, FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy,
                           SeekableFlag seekable) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return Streams::InternallySynchronizedInputStream::New<Rep_> (fd, adoptFDPolicy, seekable);
        case Execution::eNotKnownInternallySynchronized:
            return New (fd, adoptFDPolicy, seekable);
        default:
            RequireNotReached ();
            return New (fd, adoptFDPolicy, seekable);
    }
}

InputStream::Ptr<byte> FileInputStream::New (const filesystem::path& fileName, SeekableFlag seekable, BufferFlag bufferFlag)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{L"FileInputStream::New", L"fileName: %s, seekable: %d, bufferFlag: %d", ToString (fileName).c_str (),
                                  seekable, bufferFlag};
#endif
    InputStream::Ptr<byte> in = FileInputStream::New (fileName, seekable);
    switch (bufferFlag) {
        case eBuffered:
            return Streams::BufferedInputStream::New<byte> (in);
        case eUnbuffered:
            return in;
        default:
            RequireNotReached ();
            return in;
    }
}

InputStream::Ptr<byte> FileInputStream::New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable, BufferFlag bufferFlag)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{L"FileInputStream::New", L"fd: %d, seekable: %d, bufferFlag: %d", fd, seekable, bufferFlag};
#endif
    InputStream::Ptr<byte> in = FileInputStream::New (fd, adoptFDPolicy, seekable);
    switch (bufferFlag) {
        case eBuffered:
            return Streams::BufferedInputStream::New<byte> (in);
        case eUnbuffered:
            return in;
        default:
            AssertNotReached ();
            return in;
    }
}
