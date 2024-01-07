/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <fcntl.h>
#include <sys/types.h>

#if qPlatform_Windows
#include <windows.h>
#elif qPlatform_POSIX
#include <sys/mman.h>
#include <unistd.h>
#endif

#include "../../Execution/Activity.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Throw.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#include "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "../../Debug/Trace.h"
#include "FileSystem.h"
#include "PathName.h"

#include "MemoryMappedFileReader.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

#if qPlatform_Windows
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif

/*
 ********************************************************************************
 ***************************** MemoryMappedFileReader ***************************
 ********************************************************************************
 */
MemoryMappedFileReader::MemoryMappedFileReader (const filesystem::path& fileName)
{
    auto activity = LazyEvalActivity (
        [&] () -> String { return Characters::Format (L"memory mapping %s for read access", Characters::ToString (fileName).c_str ()); });
    DeclareActivity currentActivity{&activity};
#if qPlatform_POSIX
    int fd = -1;
    Execution::ThrowPOSIXErrNoIfNegative (fd = open (fileName.c_str (), O_RDONLY));
    auto fileLength = filesystem::file_size (fileName);
    //WRONG BUT NOT GROSSLY - @todo fix -- AssertNotImplemented (); // size of file - compute -- must check for overlflow and throw...
    //  offset must be a multiple of the page size as returned by sysconf(_SC_PAGE_SIZE). from http://linux.die.net/man/2/mmap
    if (fileLength >= numeric_limits<size_t>::max ()) {
        Execution::Throw (Execution::RuntimeErrorException{"file too large to be memory mapped"sv});
    }
    fSpan_ = span{reinterpret_cast<const byte*> (::mmap (nullptr, fileLength, PROT_READ, MAP_PRIVATE, fd, 0)), static_cast<size_t> (fileLength)};
    ::close (fd); //http://linux.die.net/man/2/mmap says don't need to keep FD open while mmapped
#elif qPlatform_Windows
    try {
        // FILE_READ_DATA fails on WinME - generates ERROR_INVALID_PARAMETER - so use GENERIC_READ
        fFileHandle_ = ::CreateFile (fileName.c_str (), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (fFileHandle_ == INVALID_HANDLE_VALUE) {
            Execution::ThrowSystemErrNo ();
        }
        DWORD fileSize = ::GetFileSize (fFileHandle_, nullptr);
        if (fileSize != 0) {
            fFileMapping_ = ::CreateFileMapping (fFileHandle_, nullptr, PAGE_READONLY, 0, fileSize, 0);
            ThrowIfZeroGetLastError (fFileMapping_);
            AssertNotNull (fFileMapping_);
            fSpan_ = span{reinterpret_cast<const byte*> (::MapViewOfFile (fFileMapping_, FILE_MAP_READ, 0, 0, 0)), fileSize};
            ThrowIfZeroGetLastError (fSpan_.data ());
        }
    }
    catch (...) {
        if (fFileMapping_ != INVALID_HANDLE_VALUE) {
            ::CloseHandle (fFileMapping_);
        }
        if (fFileHandle_ != INVALID_HANDLE_VALUE) {
            ::CloseHandle (fFileHandle_);
        }
        Execution::ReThrow ();
    }
#else
    AssertNotImplemented ();
#endif
}

MemoryMappedFileReader::~MemoryMappedFileReader ()
{
#if qPlatform_POSIX
    if (::munmap (const_cast<byte*> (fSpan_.data ()), fSpan_.size ())) {
        DbgTrace (L"munmap failed: Cannot throw in DTOR, so just DbgTrace log: errno=%d", errno);
    }
#elif qPlatform_Windows
    if (fSpan_.data () != nullptr) {
        (void)::UnmapViewOfFile (fSpan_.data ());
    }
    if (fFileMapping_ != INVALID_HANDLE_VALUE) {
        ::CloseHandle (fFileMapping_);
    }
    if (fFileHandle_ != INVALID_HANDLE_VALUE) {
        ::CloseHandle (fFileHandle_);
    }
#else
    AssertNotImplemented ();
#endif
}
