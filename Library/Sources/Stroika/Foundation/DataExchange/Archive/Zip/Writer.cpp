/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/DataExchange/Archive/Zip/Private_minizip_.h"
#include "Stroika/Foundation/Execution/Finally.h" // not needed yet, but maybe still - wait til working
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "Writer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Archive;
using namespace Stroika::Foundation::DataExchange::Archive::Writer;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

#if qStroika_HasComponent_zlib
using namespace Stroika::Foundation::DataExchange::Archive::Zip::PrivateMinizip_;

using Memory::BLOB;
using std::byte;

namespace {
    struct MyZipLibOutStream_ : zlib_filefunc64_def {
        OutputStream::Ptr<byte> fOutSteram_;
#if qStroika_Foundation_Debug_AssertionsChecked
        bool fOpened_{false};
#endif
        MyZipLibOutStream_ (const OutputStream::Ptr<byte>& in)
            : fOutSteram_{in}
        {
            this->zopen64_file = [] (voidpf opaqueStream, const void* /*filename*/, int /*mode*/) -> voidpf {
                MyZipLibOutStream_* myThis = reinterpret_cast<MyZipLibOutStream_*> (opaqueStream);
#if qStroika_Foundation_Debug_AssertionsChecked
                Assert (not myThis->fOpened_);
                myThis->fOpened_ = true;
#endif
                return myThis;
            };
            this->zread_file = [] ([[maybe_unused]] voidpf opaqueStream, [[maybe_unused]] voidpf stream, [[maybe_unused]] void* buf,
                                   [[maybe_unused]] uLong size) -> uLong {
                RequireNotReached (); // read only zip
                return static_cast<uLong> (UNZ_PARAMERROR);
            };
            this->zwrite_file = [] (voidpf opaqueStream, [[maybe_unused]] voidpf stream, const void* buf, uLong size) -> uLong {
                Require (opaqueStream == stream); // our use is one stream per zlib_filefunc64_def object
                MyZipLibOutStream_* myThis = reinterpret_cast<MyZipLibOutStream_*> (opaqueStream);
#if qStroika_Foundation_Debug_AssertionsChecked
                Assert (myThis->fOpened_);
#endif
                myThis->fOutSteram_.Write (span{reinterpret_cast<const byte*> (buf), size});
                return static_cast<uLong> (size);
            };
            this->ztell64_file = [] (voidpf opaqueStream, [[maybe_unused]] voidpf stream) -> ZPOS64_T {
                Require (opaqueStream == stream); // our use is one stream per zlib_filefunc64_def object
                MyZipLibOutStream_* myThis = reinterpret_cast<MyZipLibOutStream_*> (opaqueStream);
#if qStroika_Foundation_Debug_AssertionsChecked
                Assert (myThis->fOpened_);
#endif
                return myThis->fOutSteram_.GetOffset ();
            };
            this->zseek64_file = [] (voidpf opaqueStream, [[maybe_unused]] voidpf stream, ZPOS64_T offset, int origin) -> long {
                Require (opaqueStream == stream); // our use is one stream per zlib_filefunc64_def object
                MyZipLibOutStream_* myThis = reinterpret_cast<MyZipLibOutStream_*> (opaqueStream);
#if qStroika_Foundation_Debug_AssertionsChecked
                Assert (myThis->fOpened_);
#endif
                switch (origin) {
                    case ZLIB_FILEFUNC_SEEK_SET:
                        myThis->fOutSteram_.Seek (offset);
                        break;
                    case ZLIB_FILEFUNC_SEEK_CUR:
                        myThis->fOutSteram_.Seek (Streams::eFromCurrent, offset);
                        break;
                    case ZLIB_FILEFUNC_SEEK_END:
                        myThis->fOutSteram_.Seek (Streams::eFromEnd, offset);
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
                MyZipLibOutStream_* myThis = reinterpret_cast<MyZipLibOutStream_*> (opaqueStream);
                Assert (myThis->fOpened_);
                myThis->fOutSteram_.Flush ();
                myThis->fOpened_ = false;
#endif
                return UNZ_OK;
            };
            this->zerror_file = [] (voidpf opaqueStream, [[maybe_unused]] voidpf stream) -> int {
                Require (opaqueStream == stream); // our use is one stream per zlib_filefunc64_def object
                [[maybe_unused]] MyZipLibOutStream_* myThis = reinterpret_cast<MyZipLibOutStream_*> (opaqueStream);
#if qStroika_Foundation_Debug_AssertionsChecked
                Assert (myThis->fOpened_);
#endif
                return UNZ_OK; // @todo - see what this means?
            };
            this->opaque = this;
        }
        ~MyZipLibOutStream_ ()
        {
#if qStroika_Foundation_Debug_AssertionsChecked
            Assert (not fOpened_);
#endif
        }
    };
}

namespace {
    struct MyRep_ : Archive::Writer::IRep {
        MyZipLibOutStream_ fOutZipStream_;
        unzFile            fZipFile_;
        MyRep_ (const OutputStream::Ptr<byte>& out)
            : fOutZipStream_{out}
            , fZipFile_{unzOpen2_64 ("", &fOutZipStream_)}
        {
            if (fZipFile_ == nullptr) [[unlikely]] {
                static const RuntimeErrorException kException_{"failed to open zipfile"sv};
                Throw (kException_);
            }
        }
        ~MyRep_ ()
        {
            AssertNotNull (fZipFile_);
            unzClose (fZipFile_);
        }
        virtual void Add (const String& fileName, const BLOB& data) override
        {
            // NYI
#if 0
            uint32_t crcFile = 0;
            if ((password != NULL) && (err==ZIP_OK))
                    err = getFileCrc(filenameinzip,buf,size_buf,&crcFile);


   err = zipOpenNewFileInZip3_64 (zf, savefilenameinzip, &zi, NULL, 0, NULL, 0, NULL /* comment*/,
                                           (opt_compress_level != 0) ? Z_DEFLATED : 0, opt_compress_level, 0,
                                           /* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
                                           -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, password, crcFile, zip64);
  err = zipWriteInFileInZip (zf,buf,(unsigned)size_read);
                            if (err<0)
                            {
                                printf("error in writing %s in the zipfile\n",
                                                 filenameinzip);
                            }
err = zipCloseFileInZip(zf);
                    if (err!=ZIP_OK)
                        printf("error in closing %s in the zipfile\n",
                                    filenameinzip);
#endif
        }
    };
}

/*
 ********************************************************************************
 ******************* DataExchange::Archive::Zip::Writer::New ********************
 ********************************************************************************
 */
Archive::Writer::Ptr Archive::Zip::Writer::New (const OutputStream::Ptr<byte>& writeTo)
{
    Require (writeTo.IsSeekable ());
    return Archive::Writer::Ptr{make_shared<MyRep_> (writeTo)};
}
#endif