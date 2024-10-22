/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if qStroika_Foundation_Common_Platform_POSIX
#include <unistd.h>
#elif qStroika_Foundation_Common_Platform_Windows
#include <io.h>
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#if qStroika_Foundation_Common_Platform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "Stroika/Foundation/Streams/InternallySynchronizedOutputStream.h"

#include "Exception.h"

#include "FileOutputStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::IO::FileSystem::FileOutputStream;

using Execution::ThrowPOSIXErrNo;
using Execution::ThrowPOSIXErrNoIfNegative;
using Execution::ThrowSystemErrNo;

#if qStroika_Foundation_Common_Platform_Windows
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif

namespace {
    class Rep_ : public Streams::OutputStream::IRep<byte> /*, public Memory::UseBlockAllocationIfAppropriate<Rep_>*/ {
    public:
        Rep_ ()            = delete;
        Rep_ (const Rep_&) = delete;
        Rep_ (const filesystem::path& fileName, AppendFlag appendFlag, FlushFlag flushFlag)
            : fFD_{-1}
            , fFlushFlag{flushFlag}
            , fFileName_{fileName}
        {
            auto activity = LazyEvalActivity ([&] () -> String { return Characters::Format ("opening {} for write access"_f, fFileName_); });
            DeclareActivity currentActivity{&activity};
#if qStroika_Foundation_Common_Platform_Windows
            int     appendFlag2Or = appendFlag == eStartFromStart ? _O_TRUNC : _O_APPEND;
            errno_t e = ::_wsopen_s (&fFD_, fileName.generic_wstring ().c_str (), _O_WRONLY | _O_CREAT | _O_BINARY | appendFlag2Or,
                                     _SH_DENYNO, _S_IREAD | _S_IWRITE);
            if (e != 0) {
                FileSystem::Exception::ThrowPOSIXErrNo (e, fileName);
            }
            if (fFD_ == -1) {
                FileSystem::Exception::ThrowSystemErrNo (fileName);
            }
#else
            int          appendFlag2Or = appendFlag == eStartFromStart ? O_TRUNC : O_APPEND;
            const mode_t kCreateMode_  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
            FileSystem::Exception::ThrowPOSIXErrNoIfNegative (
                fFD_ = ::open (fileName.generic_string ().c_str (), O_WRONLY | O_CREAT | appendFlag2Or, kCreateMode_), fileName);
#endif
        }
        Rep_ (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekableFlag, FlushFlag flushFlag)
            : fFD_{fd}
            , fFlushFlag{flushFlag}
            , fAdoptFDPolicy_{adoptFDPolicy}
            , fSeekable_{seekableFlag == SeekableFlag::eSeekable}
        {
        }
        ~Rep_ ()
        {
            IgnoreExceptionsForCall (Flush ()); // for fFlushFlag == FlushFlag::eToDisk
            if (fAdoptFDPolicy_ == AdoptFDPolicy::eCloseOnDestruction and IsOpenWrite ()) {
#if qStroika_Foundation_Common_Platform_Windows
                ::_close (fFD_);
#else
                ::close (fFD_);
#endif
            }
        }
        nonvirtual Rep_& operator= (const Rep_&) = delete;
        virtual bool     IsSeekable () const override
        {
            return fSeekable_;
        }
        virtual void CloseWrite () override
        {
            Require (IsOpenWrite ());
            if (fAdoptFDPolicy_ == AdoptFDPolicy::eCloseOnDestruction) {
#if qStroika_Foundation_Common_Platform_Windows
                ::_close (fFD_);
#else
                ::close (fFD_);
#endif
            }
            fFD_ = -1;
        }
        virtual bool IsOpenWrite () const override
        {
            return fFD_ >= 0;
        }
        virtual void Write (span<const byte> elts) override
        {
            Require (not elts.empty ());
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            auto            activity = LazyEvalActivity ([&] () -> String { return Characters::Format ("writing to {}"_f, fFileName_); });
            DeclareActivity currentActivity{&activity};
            const byte*     i   = elts.data ();
            const byte*     end = elts.data () + elts.size ();
            while (i < end) {
#if qStroika_Foundation_Common_Platform_Windows
                int n = ThrowPOSIXErrNoIfNegative (_write (fFD_, i, Math::PinToMaxForType<unsigned int> (end - i)));
#else
                int n = ThrowPOSIXErrNoIfNegative (write (fFD_, i, end - i));
#endif
                Assert (n <= (end - i));
                i += n;
            }
        }
        virtual void Flush () override
        {
            // normally nothing todo - write 'writes thru' (except if fFlushFlag)
            if (fFlushFlag == FlushFlag::eToDisk) {
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                auto activity = LazyEvalActivity{[&] () -> String { return Characters::Format ("flushing data to {}"_f, fFileName_); }};
                DeclareActivity currentActivity{&activity};
#if qStroika_Foundation_Common_Platform_POSIX
                ThrowPOSIXErrNoIfNegative (::fsync (fFD_));
#elif qStroika_Foundation_Common_Platform_Windows
                ThrowIfZeroGetLastError (::FlushFileBuffers (reinterpret_cast<HANDLE> (::_get_osfhandle (fFD_))));
#else
                AssertNotImplemented ();
#endif
            }
        }
        virtual Streams::SeekOffsetType GetWriteOffset () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
#if qStroika_Foundation_Common_Platform_Linux
            return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, 0, SEEK_CUR)));
#elif qStroika_Foundation_Common_Platform_Windows
            return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, 0, SEEK_CUR)));
#else
            return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, 0, SEEK_CUR)));
#endif
        }
        virtual Streams::SeekOffsetType SeekWrite (Streams::Whence whence, Streams::SignedSeekOffsetType offset) override
        {
            Require (fSeekable_);
            using namespace Streams;
            static const auto                               kException_ = range_error{"seek"};
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            switch (whence) {
                case Whence::eFromStart: {
                    if (offset < 0) [[unlikely]] {
                        Execution::Throw (kException_);
                    }
#if qStroika_Foundation_Common_Platform_Linux
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_SET)));
#elif qStroika_Foundation_Common_Platform_Windows
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_SET)));
#else
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, offset, SEEK_SET)));
#endif
                } break;
                case Whence::eFromCurrent: {
#if qStroika_Foundation_Common_Platform_Linux
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_CUR)));
#elif qStroika_Foundation_Common_Platform_Windows
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_CUR)));
#else
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, offset, SEEK_CUR)));
#endif
                } break;
                case Whence::eFromEnd: {
#if qStroika_Foundation_Common_Platform_Linux
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_END)));
#elif qStroika_Foundation_Common_Platform_Windows
                    return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_END)));
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
        FlushFlag                                               fFlushFlag;
        AdoptFDPolicy                                           fAdoptFDPolicy_{AdoptFDPolicy::eCloseOnDestruction};
        bool                                                    fSeekable_{true};
        optional<filesystem::path>                              fFileName_;
        [[no_unique_address]] AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };
}

/*
 ********************************************************************************
 ************************* FileSystem::FileOutputStream *************************
 ********************************************************************************
 */
auto FileOutputStream::New (const filesystem::path& fileName, FlushFlag flushFlag) -> Ptr
{
    return Ptr{Memory::MakeSharedPtr<Rep_> (fileName, AppendFlag::eDEFAULT, flushFlag)};
}

auto FileOutputStream::New (const filesystem::path& fileName, AppendFlag appendFlag, FlushFlag flushFlag) -> Ptr
{
    return Ptr{Memory::MakeSharedPtr<Rep_> (fileName, appendFlag, flushFlag)};
}

auto FileOutputStream::New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekableFlag, FlushFlag flushFlag) -> Ptr
{
    return Ptr{Memory::MakeSharedPtr<Rep_> (fd, adoptFDPolicy, seekableFlag, flushFlag)};
}

auto FileOutputStream::New (Execution::InternallySynchronized internallySynchronized, const filesystem::path& fileName, FlushFlag flushFlag) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return Streams::InternallySynchronizedOutputStream::New<Rep_> ({}, fileName, AppendFlag::eDEFAULT, flushFlag);
        case Execution::eNotKnownInternallySynchronized:
            return New (fileName, AppendFlag::eDEFAULT, flushFlag);
        default:
            RequireNotReached ();
            return Ptr{};
    }
}

auto FileOutputStream::New (Execution::InternallySynchronized internallySynchronized, const filesystem::path& fileName,
                            AppendFlag appendFlag, FlushFlag flushFlag) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return Streams::InternallySynchronizedOutputStream::New<Rep_> ({}, fileName, appendFlag, flushFlag);
        case Execution::eNotKnownInternallySynchronized:
            return New (fileName, appendFlag, flushFlag);
        default:
            RequireNotReached ();
            return Ptr{};
    }
}

auto FileOutputStream::New (Execution::InternallySynchronized internallySynchronized, FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy,
                            SeekableFlag seekableFlag, FlushFlag flushFlag) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return Streams::InternallySynchronizedOutputStream::New<Rep_> ({}, fd, adoptFDPolicy, seekableFlag, flushFlag);
        case Execution::eNotKnownInternallySynchronized:
            return New (fd, adoptFDPolicy, seekableFlag, flushFlag);
        default:
            RequireNotReached ();
            return Ptr{};
    }
}
