/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_zlib
#include <zlib.h>
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/DataExchange/Archive/Zip/Private_minizip_.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "Reader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Archive;
using namespace Stroika::Foundation::Execution;

using std::byte;

#if qStroika_HasComponent_zlib
using namespace Stroika::Foundation::DataExchange::Archive::Zip::PrivateMinizip_;

struct MyZipLibInStream_ : zlib_filefunc64_def {
    Streams::InputStream::Ptr<byte> fInStream_;
#if qStroika_Foundation_Debug_AssertionsChecked
    bool fOpened_{false};
#endif
    MyZipLibInStream_ (const Streams::InputStream::Ptr<byte>& in)
        : fInStream_{in}
    {
        this->zopen64_file = [] (voidpf opaqueStream, const void* /*filename*/, int /*mode*/) -> voidpf {
            MyZipLibInStream_* myThis = reinterpret_cast<MyZipLibInStream_*> (opaqueStream);
#if qStroika_Foundation_Debug_AssertionsChecked
            Assert (not myThis->fOpened_);
            myThis->fOpened_ = true;
#endif
            return myThis;
        };
        this->zread_file = [] (voidpf opaqueStream, [[maybe_unused]] voidpf stream, void* buf, uLong size) -> uLong {
            Require (opaqueStream == stream); // our use is one stream per zlib_filefunc64_def object
            MyZipLibInStream_* myThis = reinterpret_cast<MyZipLibInStream_*> (opaqueStream);
#if qStroika_Foundation_Debug_AssertionsChecked
            Assert (myThis->fOpened_);
#endif
            size_t sz = myThis->fInStream_.ReadBlocking (span{reinterpret_cast<byte*> (buf), size}).size ();
            Assert (sz <= size);
            return static_cast<uLong> (sz);
        };
        this->zwrite_file = [] (voidpf /*opaque*/, voidpf /*stream*/, const void* /*buf*/, uLong /*size*/) -> uLong {
            RequireNotReached (); // read only zip
            return static_cast<uLong> (UNZ_PARAMERROR);
        };
        this->ztell64_file = [] (voidpf opaqueStream, [[maybe_unused]] voidpf stream) -> ZPOS64_T {
            Require (opaqueStream == stream); // our use is one stream per zlib_filefunc64_def object
            MyZipLibInStream_* myThis = reinterpret_cast<MyZipLibInStream_*> (opaqueStream);
#if qStroika_Foundation_Debug_AssertionsChecked
            Assert (myThis->fOpened_);
#endif
            return myThis->fInStream_.GetOffset ();
        };
        this->zseek64_file = [] (voidpf opaqueStream, [[maybe_unused]] voidpf stream, ZPOS64_T offset, int origin) -> long {
            Require (opaqueStream == stream); // our use is one stream per zlib_filefunc64_def object
            MyZipLibInStream_* myThis = reinterpret_cast<MyZipLibInStream_*> (opaqueStream);
#if qStroika_Foundation_Debug_AssertionsChecked
            Assert (myThis->fOpened_);
#endif
            switch (origin) {
                case ZLIB_FILEFUNC_SEEK_SET:
                    myThis->fInStream_.Seek (offset);
                    break;
                case ZLIB_FILEFUNC_SEEK_CUR:
                    myThis->fInStream_.Seek (Streams::eFromCurrent, offset);
                    break;
                case ZLIB_FILEFUNC_SEEK_END:
                    myThis->fInStream_.Seek (Streams::eFromEnd, offset);
                    break;
                default:
                    AssertNotReached ();
                    return UNZ_PARAMERROR;
            }
            return UNZ_OK;
        };
        this->zclose_file = [] ([[maybe_unused]] voidpf opaqueStream, [[maybe_unused]] voidpf stream) -> int {
#if qStroika_Foundation_Debug_AssertionsChecked
            Require (opaqueStream == stream); // our use is one stream per zlib_filefunc64_def object
            MyZipLibInStream_* myThis = reinterpret_cast<MyZipLibInStream_*> (opaqueStream);
            Assert (myThis->fOpened_);
            myThis->fOpened_ = false;
#endif
            return UNZ_OK;
        };
        this->zerror_file = [] (voidpf opaqueStream, [[maybe_unused]] voidpf stream) -> int {
            Require (opaqueStream == stream); // our use is one stream per zlib_filefunc64_def object
            [[maybe_unused]] MyZipLibInStream_* myThis = reinterpret_cast<MyZipLibInStream_*> (opaqueStream);
#if qStroika_Foundation_Debug_AssertionsChecked
            Assert (myThis->fOpened_);
#endif
            return UNZ_OK; // @todo - see what this means?
        };
        this->opaque = this;
    }
    ~MyZipLibInStream_ ()
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        Assert (not fOpened_);
#endif
    }
};

struct Rep_ : public Reader::IRep {
private:
    MyZipLibInStream_ fInSeekStream_;
    unzFile           fZipFile_;

public:
    Rep_ (const Streams::InputStream::Ptr<byte>& in)
        : fInSeekStream_{in}
        , fZipFile_{unzOpen2_64 ("", &fInSeekStream_)}
    {
        if (fZipFile_ == nullptr) [[unlikely]] {
            static const RuntimeErrorException kException_{"failed to open zipfile"sv};
            Throw (kException_);
        }
    }
    ~Rep_ ()
    {
        AssertNotNull (fZipFile_);
        unzClose (fZipFile_);
    }
    virtual Set<String> GetContainedFiles () const override
    {
        Set<String>       result;
        unz_global_info64 gi;
        int               err = unzGetGlobalInfo64 (fZipFile_, &gi);
        if (err != UNZ_OK) [[unlikely]] {
            Throw (RuntimeErrorException{Format ("error {} with zipfile in unzGetGlobalInfo"_f, err)});
        }
        for (size_t i = 0; i < gi.number_entry; i++) {
            char            filename_inzip[10 * 1024];
            unz_file_info64 file_info;
            //uLong           ratio = 0;
            //const char* string_method;
            //char charCrypt = ' ';
            err = ::unzGetCurrentFileInfo64 (fZipFile_, &file_info, filename_inzip, sizeof (filename_inzip), NULL, 0, NULL, 0);
            if (err != UNZ_OK) [[unlikely]] {
                Throw (RuntimeErrorException{Format ("error {} with zipfile in unzGetCurrentFileInfo64"_f, err)});
                break;
            }
            if ((i + 1) < gi.number_entry) {
                err = ::unzGoToNextFile_ (fZipFile_);
                if (err != UNZ_OK) [[unlikely]] {
                    Throw (RuntimeErrorException{"error {} with zipfile in unzGoToNextFile"_f(err)});
                    break;
                }
            }
            //tmphac
            if (filename_inzip[::strlen (filename_inzip) - 1] == '/') {
                continue; // only list files - not directories for now
            }
            result.Add (String{filename_inzip}); // not sure about codepage for conversion - for now assume ascii?
        }
#if 0
        // Keep temporarily, because we will want a traversal variant that captures this extra info
        uLong i;
        unz_global_info64 gi;
        int err;

        err = unzGetGlobalInfo64(uf, &gi);
        if (err != UNZ_OK)
            printf("error %d with zipfile in unzGetGlobalInfo \n", err);
        printf("  Length  Method     Size Ratio   Date    Time   CRC-32     Name\n");
        printf("  ------  ------     ---- -----   ----    ----   ------     ----\n");
        for (i = 0; i < gi.number_entry; ++i) {
            char filename_inzip[256];
            unz_file_info64 file_info;
            uLong ratio = 0;
            const char* string_method;
            char charCrypt = ' ';
            err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
            if (err != UNZ_OK) {
                printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
                break;
            }
            if (file_info.uncompressed_size > 0)
                ratio = (uLong)((file_info.compressed_size * 100) / file_info.uncompressed_size);

            /* display a '*' if the file is crypted */
            if ((file_info.flag & 1) != 0)
                charCrypt = '*';

            if (file_info.compression_method == 0)
                string_method = "Stored";
            else if (file_info.compression_method == Z_DEFLATED) {
                uInt iLevel = (uInt)((file_info.flag & 0x6) / 2);
                if (iLevel == 0)
                    string_method = "Defl:N";
                else if (iLevel == 1)
                    string_method = "Defl:X";
                else if ((iLevel == 2) or (iLevel == 3))
                    string_method = "Defl:F"; /* 2:fast , 3 : extra fast*/
            }
            else if (file_info.compression_method == Z_BZIP2ED) {
                string_method = "BZip2 ";
            }
            else
                string_method = "Unkn. ";

            Display64BitsSize(file_info.uncompressed_size, 7);
            printf("  %6s%c", string_method, charCrypt);
            Display64BitsSize(file_info.compressed_size, 7);
            printf(" %3lu%%  %2.2lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n",
                   ratio,
                   (uLong)file_info.tmu_date.tm_mon + 1,
                   (uLong)file_info.tmu_date.tm_mday,
                   (uLong)file_info.tmu_date.tm_year % 100,
                   (uLong)file_info.tmu_date.tm_hour, (uLong)file_info.tmu_date.tm_min,
                   (uLong)file_info.crc, filename_inzip);
            if ((i + 1) < gi.number_entry) {
                err = unzGoToNextFile_ (uf);
                if (err != UNZ_OK) {
                    printf("error %d with zipfile in unzGoToNextFile\n", err);
                    break;
                }
            }
        }

        for (unsigned int i = 0; i < fDB_.NumFiles; i++) {
            if (not SzArEx_IsDir (&fDB_, i)) {
                size_t nameLen = ::SzArEx_GetFileNameUtf16 (&fDB_, i, nullptr);
                if (nameLen < 1) {
                    break;
                }
                Memory::StackBuffer<char16_t> fileName {Memory::eUninitiialized, nameLen};
                size_t z = ::SzArEx_GetFileNameUtf16 (&fDB_, i, reinterpret_cast<UInt16*> (&fileName[0]));
                result.Add (String{&fileName[0]});
            }
        }
#endif
        return result;
    }
    virtual Memory::BLOB GetData (const String& fileName) const override
    {
        if (unzLocateFile_ (fZipFile_, fileName.AsNarrowSDKString ().c_str (), 1) != UNZ_OK) [[unlikely]] {
            Throw (RuntimeErrorException{Format ("File '{}' not found"_f, fileName)});
        }
        const char*                      password = nullptr;
        int                              err      = unzOpenCurrentFilePassword (fZipFile_, password);
        [[maybe_unused]] auto&&          cleanup  = Finally ([this] () noexcept { unzCloseCurrentFile_ (fZipFile_); });
        Streams::MemoryStream::Ptr<byte> tmpBuf   = Streams::MemoryStream::New<byte> ();
        do {
            byte buf[10 * 1024];
            err = unzReadCurrentFile_ (fZipFile_, buf, static_cast<unsigned int> (Memory::NEltsOf (buf)));
            if (err < 0) [[unlikely]] {
                Throw (RuntimeErrorException{Format (L"File '{}' error {} extracting"_f, fileName, err)});
            }
            else if (err > 0) {
                Assert (static_cast<size_t> (err) <= Memory::NEltsOf (buf));
                tmpBuf.Write (span{buf, static_cast<size_t> (err)});
            }
        } while (err > 0);
        return tmpBuf.As<Memory::BLOB> ();
    }
};

/*
 ********************************************************************************
 ***************** DataExchange::Archive::Zip::Reader::New **********************
 ********************************************************************************
 */
Archive::Reader::Ptr Archive::Zip::Reader::New (const Streams::InputStream::Ptr<byte>& readFrom)
{
    return Archive::Reader::Ptr{make_shared<Rep_> (readFrom)};
}
#endif
