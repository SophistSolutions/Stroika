/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

// @todo add Stroika wrapper on this (thirdpartyproducts)
#ifdef HAVE_BZIP2
#include "bzlib.h"
#endif

#include "../../../Characters/Format.h"
#include "../../../Execution/Finally.h"
#include "../../../Streams/MemoryStream.h"

#include "Reader.h"

/*
 * NOTE/CREDIT:
 *      This code originally cribbed from ZLib (http://www.zlib.net/) 1.2.8 contrib/minizip
 *
                Version 1.1, February 14h, 2010
                part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

                        Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

                        Modifications for Zip64 support
                        Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

                        For more info read MiniZip_info.txt

                        ---------------------------------------------------------------------------

                Condition of use and distribution are the same than zlib :

                This software is provided 'as-is', without any express or implied
                warranty.  In no event will the authors be held liable for any damages
                arising from the use of this software.

                Permission is granted to anyone to use this software for any purpose,
                including commercial applications, and to alter it and redistribute it
                freely, subject to the following restrictions:

                1. The origin of this software must not be misrepresented; you must not
                    claim that you wrote the original software. If you use this software
                    in a product, an acknowledgment in the product documentation would be
                    appreciated but is not required.
                2. Altered source versions must be plainly marked as such, and must not be
                    misrepresented as being the original software.
                3. This notice may not be removed or altered from any source distribution.
....
                Version 1.1, February 14h, 2010
                part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

                        Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

                        Modifications of Unzip for Zip64
                        Copyright (C) 2007-2008 Even Rouault

                        Modifications for Zip64 support on both zip and unzip
                        Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

                        For more info read MiniZip_info.txt

                        ---------------------------------------------------------------------------------

                    Condition of use and distribution are the same than zlib :

                This software is provided 'as-is', without any express or implied
                warranty.  In no event will the authors be held liable for any damages
                arising from the use of this software.

                Permission is granted to anyone to use this software for any purpose,
                including commercial applications, and to alter it and redistribute it
                freely, subject to the following restrictions:

                1. The origin of this software must not be misrepresented; you must not
                    claim that you wrote the original software. If you use this software
                    in a product, an acknowledgment in the product documentation would be
                    appreciated but is not required.
                2. Altered source versions must be plainly marked as such, and must not be
                    misrepresented as being the original software.
                3. This notice may not be removed or altered from any source distribution.
....
                Copyright (C) 1998-2005 Gilles Vollant

                This code is a modified version of crypting code in Infozip distribution

                The encryption/decryption parts of this source code (as opposed to the
                non-echoing password parts) were originally written in Europe.  The
                whole source package can be freely distributed, including from the USA.
                (Prior to January 2000, re-export from the US was a violation of US law.)

                This encryption code is a direct transcription of the algorithm from
                Roger Schlafly, described by Phil Katz in the file appnote.txt.  This
                file (appnote.txt) is distributed with the PKZIP program (even in the
                version without encryption capabilities).

                If you don't need crypting in your application, just define symbols
                NOCRYPT and NOUNCRYPT.

                This code support the "Traditional PKWARE Encryption".

                The new AES encryption added on Zip format by Winzip (see the page
                http://www.winzip.com/aes_info.htm ) and PKWare PKZip 5.x Strong
                Encryption is not supported.
....
                Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

                Modifications of Unzip for Zip64
                Copyright (C) 2007-2008 Even Rouault

                Modifications for Zip64 support on both zip and unzip
                Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

                For more info read MiniZip_info.txt
                ------------------------------------------------------------------------------------
                Decryption code comes from crypt.c by Info-ZIP but has been greatly reduced in terms of
                compatibility with older software. The following is from the original crypt.c.
                Code woven in by Terry Thorsen 1/2003.

                Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

                See the accompanying file LICENSE, version 2000-Apr-09 or later
                (the contents of which are also included in zip.h) for terms of use.
                If, for some reason, all these files are missing, the Info-ZIP license
                also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html

                crypt.c (full version) by Info-ZIP.      Last revised:  [see crypt.h]

                The encryption/decryption parts of this source code (as opposed to the
                non-echoing password parts) were originally written in Europe.  The
                whole source package can be freely distributed, including from the USA.
                (Prior to January 2000, re-export from the US was a violation of US law.)

                This encryption code is a direct transcription of the algorithm from
                Roger Schlafly, described by Phil Katz in the file appnote.txt.  This
                file (appnote.txt) is distributed with the PKZIP program (even in the
                version without encryption capabilities).
...
                Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

                Modifications for Zip64 support
                Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
*/

#if qHasFeature_ZLib
#include <zlib.h>

#if qHasFeature_ZLib && defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment(lib, "zlib.lib")
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Archive;

using std::byte;

namespace {
    constexpr uint32_t MAXU32 = numeric_limits<uint32_t>::max ();
}

///// START OF CRYPT.h
namespace {
#define CRC32(c, b) ((*(pcrc_32_tab + (((int)(c) ^ (b)) & 0xff))) ^ ((c) >> 8))

    //  Return the next byte in the pseudo-random sequence
    int decrypt_byte (unsigned long* pkeys, [[maybe_unused]] const z_crc_t* pcrc_32_tab)
    {
        unsigned temp; /* POTENTIAL BUG:  temp*(temp^1) may overflow in an
                     * unpredictable manner on 16-bit systems; not a problem
                     * with any known compiler so far, though */
        temp = ((unsigned)(*(pkeys + 2)) & 0xffff) | 2;
        return (int)(((temp * (temp ^ 1)) >> 8) & 0xff);
    }

    //  Update the encryption keys with the next byte of plain text
    int update_keys (unsigned long* pkeys, const z_crc_t* pcrc_32_tab, int c)
    {
        (*(pkeys + 0)) = CRC32 ((*(pkeys + 0)), c);
        (*(pkeys + 1)) += (*(pkeys + 0)) & 0xff;
        (*(pkeys + 1)) = (*(pkeys + 1)) * 134775813L + 1;
        {
            int keyshift   = (int)((*(pkeys + 1)) >> 24);
            (*(pkeys + 2)) = CRC32 ((*(pkeys + 2)), keyshift);
        }
        return c;
    }

    //  Initialize the encryption keys and the random header according to the given password.
    void init_keys (const char* passwd, unsigned long* pkeys, const z_crc_t* pcrc_32_tab)
    {
        *(pkeys + 0) = 305419896L;
        *(pkeys + 1) = 591751049L;
        *(pkeys + 2) = 878082192L;
        while (*passwd != '\0') {
            update_keys (pkeys, pcrc_32_tab, (int)*passwd);
            passwd++;
        }
    }

#define zdecode(pkeys, pcrc_32_tab, c) \
    (update_keys (pkeys, pcrc_32_tab, c ^= decrypt_byte (pkeys, pcrc_32_tab)))

#define zencode(pkeys, pcrc_32_tab, c, t) \
    (t = decrypt_byte (pkeys, pcrc_32_tab), update_keys (pkeys, pcrc_32_tab, c), t ^ (c))

#define RAND_HEAD_LEN 12

    /* "last resort" source for second part of crypt seed pattern */
    constexpr unsigned int ZCR_SEED2 = 3141592654UL; /* use PI as default pattern */

    int crypthead (const char*    passwd, /* password string */
                   unsigned char* buf,    /* where to write header */
                   int bufSize, unsigned long* pkeys,
                   const z_crc_t* pcrc_32_tab, unsigned long crcForCrypting)
    {
        int             n;                         /* index in random header */
        int             t;                         /* temporary */
        int             c;                         /* random byte */
        unsigned char   header[RAND_HEAD_LEN - 2]; /* random header */
        static unsigned calls = 0;                 /* ensure different random header each time */

        if (bufSize < RAND_HEAD_LEN)
            return 0;

        /* First generate RAND_HEAD_LEN-2 random bytes. We encrypt the
         * output of rand() to get less predictability, since rand() is
         * often poorly implemented.
         */
        if (++calls == 1) {
            srand ((unsigned)(time (NULL) ^ ZCR_SEED2));
        }
        init_keys (passwd, pkeys, pcrc_32_tab);
        for (n = 0; n < RAND_HEAD_LEN - 2; n++) {
            c         = (rand () >> 7) & 0xff;
            header[n] = (unsigned char)zencode (pkeys, pcrc_32_tab, c, t);
        }
        /* Encrypt random header (last two bytes is high word of crc) */
        init_keys (passwd, pkeys, pcrc_32_tab);
        for (n = 0; n < RAND_HEAD_LEN - 2; n++) {
            buf[n] = (unsigned char)zencode (pkeys, pcrc_32_tab, header[n], t);
        }
        buf[n++] = (unsigned char)zencode (pkeys, pcrc_32_tab, (int)(crcForCrypting >> 16) & 0xff, t);
        buf[n++] = (unsigned char)zencode (pkeys, pcrc_32_tab, (int)(crcForCrypting >> 24) & 0xff, t);
        return n;
    }
}
///////////// END OF CRYPT.h //////////////////

///// START OF ioapi.h////////////////////
namespace {

#ifdef HAVE_MINIZIP64_CONF_H
#include "mz64conf.h"
#endif

    typedef uint64_t ZPOS64_T;

#define ZLIB_FILEFUNC_SEEK_CUR (1)
#define ZLIB_FILEFUNC_SEEK_END (2)
#define ZLIB_FILEFUNC_SEEK_SET (0)

#define ZLIB_FILEFUNC_MODE_READ (1)
#define ZLIB_FILEFUNC_MODE_WRITE (2)
#define ZLIB_FILEFUNC_MODE_READWRITEFILTER (3)

#define ZLIB_FILEFUNC_MODE_EXISTING (4)
#define ZLIB_FILEFUNC_MODE_CREATE (8)

    typedef voidpf (*open_file_func) (voidpf opaque, const char* filename, int mode);
    typedef uLong (*read_file_func) (voidpf opaque, voidpf stream, void* buf, uLong size);
    typedef uLong (*write_file_func) (voidpf opaque, voidpf stream, const void* buf, uLong size);
    typedef int (*close_file_func) (voidpf opaque, voidpf stream);
    typedef int (*testerror_file_func) (voidpf opaque, voidpf stream);

    typedef long (*tell_file_func) (voidpf opaque, voidpf stream);
    typedef long (*seek_file_func) (voidpf opaque, voidpf stream, uLong offset, int origin);

    /* here is the "old" 32 bits structure structure */
    struct zlib_filefunc_def {
        open_file_func      zopen_file;
        read_file_func      zread_file;
        write_file_func     zwrite_file;
        tell_file_func      ztell_file;
        seek_file_func      zseek_file;
        close_file_func     zclose_file;
        testerror_file_func zerror_file;
        voidpf              opaque;
    };

    typedef ZPOS64_T (*tell64_file_func) (voidpf opaque, voidpf stream);
    typedef long (*seek64_file_func) (voidpf opaque, voidpf stream, ZPOS64_T offset, int origin);
    typedef voidpf (*open64_file_func) (voidpf opaque, const void* filename, int mode);

    struct zlib_filefunc64_def {
        open64_file_func    zopen64_file;
        read_file_func      zread_file;
        write_file_func     zwrite_file;
        tell64_file_func    ztell64_file;
        seek64_file_func    zseek64_file;
        close_file_func     zclose_file;
        testerror_file_func zerror_file;
        voidpf              opaque;
    };

    /* now internal definition, only for zip.c and unzip.h */
    struct zlib_filefunc64_32_def {
        zlib_filefunc64_def zfile_func64;
        open_file_func      zopen32_file;
        tell_file_func      ztell32_file;
        seek_file_func      zseek32_file;
    };

#define ZREAD64(filefunc, filestream, buf, size) ((*((filefunc).zfile_func64.zread_file)) ((filefunc).zfile_func64.opaque, filestream, buf, size))
#define ZWRITE64(filefunc, filestream, buf, size) ((*((filefunc).zfile_func64.zwrite_file)) ((filefunc).zfile_func64.opaque, filestream, buf, size))
#define ZCLOSE64(filefunc, filestream) ((*((filefunc).zfile_func64.zclose_file)) ((filefunc).zfile_func64.opaque, filestream))
#define ZERROR64(filefunc, filestream) ((*((filefunc).zfile_func64.zerror_file)) ((filefunc).zfile_func64.opaque, filestream))

    voidpf   call_zopen64 (const zlib_filefunc64_32_def* pfilefunc, const void* filename, int mode);
    long     call_zseek64 (const zlib_filefunc64_32_def* pfilefunc, voidpf filestream, ZPOS64_T offset, int origin);
    ZPOS64_T call_ztell64 (const zlib_filefunc64_32_def* pfilefunc, voidpf filestream);

    void fill_zlib_filefunc64_32_def_from_filefunc32 (zlib_filefunc64_32_def* p_filefunc64_32, const zlib_filefunc_def* p_filefunc32);

#define ZOPEN64(filefunc, filename, mode) (call_zopen64 ((&(filefunc)), (filename), (mode)))
#define ZTELL64(filefunc, filestream) (call_ztell64 ((&(filefunc)), (filestream)))
#define ZSEEK64(filefunc, filestream, pos, mode) (call_zseek64 ((&(filefunc)), (filestream), (pos), (mode)))
}
/////////////////////// END OF IO.h.////////////////////////////////////

///////////////////////////  START OF  UNZIP.h ////////////////////////////////////
namespace {

#define Z_BZIP2ED 12

#if defined(STRICTUNZIP) || defined(STRICTZIPUNZIP)
    /* like the STRICT of WIN32, we define a pointer that cannot be converted
        from (void*) without cast */
    typedef struct TagunzFile__ {
        int unused;
    } unzFile__;
    typedef unzFile__* unzFile;
#else
    typedef voidp unzFile;
#endif

#define UNZ_OK (0)
#define UNZ_END_OF_LIST_OF_FILE (-100)
#define UNZ_ERRNO (Z_ERRNO)
#define UNZ_EOF (0)
#define UNZ_PARAMERROR (-102)
#define UNZ_BADZIPFILE (-103)
#define UNZ_INTERNALERROR (-104)
#define UNZ_CRCERROR (-105)

    /* tm_unz contain date/time info */
    struct tm_unz {
        uInt tm_sec;  /* seconds after the minute - [0,59] */
        uInt tm_min;  /* minutes after the hour - [0,59] */
        uInt tm_hour; /* hours since midnight - [0,23] */
        uInt tm_mday; /* day of the month - [1,31] */
        uInt tm_mon;  /* months since January - [0,11] */
        uInt tm_year; /* years - [1980..2044] */
    };

    /* unz_global_info structure contain global data about the ZIPfile
       These data comes from the end of central dir */
    struct unz_global_info64 {
        ZPOS64_T number_entry; /* total number of entries in
                                     the central dir on this disk */
        uLong    size_comment; /* size of the global comment of the zipfile */
    };

    struct unz_global_info {
        uLong number_entry; /* total number of entries in
                                     the central dir on this disk */
        uLong size_comment; /* size of the global comment of the zipfile */
    };

    /* unz_file_info contain information about a file in the zipfile */
    struct unz_file_info64 {
        uLong    version;            /* version made by                 2 bytes */
        uLong    version_needed;     /* version needed to extract       2 bytes */
        uLong    flag;               /* general purpose bit flag        2 bytes */
        uLong    compression_method; /* compression method              2 bytes */
        uLong    dosDate;            /* last mod file date in Dos fmt   4 bytes */
        uLong    crc;                /* crc-32                          4 bytes */
        ZPOS64_T compressed_size;    /* compressed size                 8 bytes */
        ZPOS64_T uncompressed_size;  /* uncompressed size               8 bytes */
        uLong    size_filename;      /* filename length                 2 bytes */
        uLong    size_file_extra;    /* extra field length              2 bytes */
        uLong    size_file_comment;  /* file comment length             2 bytes */

        uLong disk_num_start; /* disk number start               2 bytes */
        uLong internal_fa;    /* internal file attributes        2 bytes */
        uLong external_fa;    /* external file attributes        4 bytes */

        tm_unz tmu_date;
    };

    struct unz_file_info {
        uLong version;            /* version made by                 2 bytes */
        uLong version_needed;     /* version needed to extract       2 bytes */
        uLong flag;               /* general purpose bit flag        2 bytes */
        uLong compression_method; /* compression method              2 bytes */
        uLong dosDate;            /* last mod file date in Dos fmt   4 bytes */
        uLong crc;                /* crc-32                          4 bytes */
        uLong compressed_size;    /* compressed size                 4 bytes */
        uLong uncompressed_size;  /* uncompressed size               4 bytes */
        uLong size_filename;      /* filename length                 2 bytes */
        uLong size_file_extra;    /* extra field length              2 bytes */
        uLong size_file_comment;  /* file comment length             2 bytes */

        uLong disk_num_start; /* disk number start               2 bytes */
        uLong internal_fa;    /* internal file attributes        2 bytes */
        uLong external_fa;    /* external file attributes        4 bytes */

        tm_unz tmu_date;
    };

    int unzStringFileNameCompare (const char* fileName1, const char* fileName2, int iCaseSensitivity);
    /*
       Compare two filename (fileName1,fileName2).
       If iCaseSenisivity = 1, comparision is case sensitivity (like strcmp)
       If iCaseSenisivity = 2, comparision is not case sensitivity (like strcmpi
                                    or strcasecmp)
       If iCaseSenisivity = 0, case sensitivity is defaut of your operating system
        (like 1 on Unix, 2 on Windows)
    */

    /*
      Open a Zip file. path contain the full pathname (by example,
         on a Windows XP computer "c:\\zlib\\zlib113.zip" or on an Unix computer
         "zlib/zlib113.zip".
         If the zipfile cannot be opened (file don't exist or in not valid), the
           return value is NULL.
         Else, the return value is a unzFile Handle, usable with other function
           of this unzip package.
         the "64" function take a const void* pointer, because the path is just the
           value passed to the open64_file_func callback.
         Under Windows, if UNICODE is defined, using fill_fopen64_filefunc, the path
           is a pointer to a wide unicode string (LPCTSTR is LPCWSTR), so const char*
           does not describe the reality
    */
    unzFile unzOpen (const char* path);
    unzFile unzOpen64 (const void* path);

    /*
       Open a Zip file, like unzOpen, but provide a set of file low level API
          for read/write the zip file (see ioapi.h)
    */
    unzFile unzOpen2 (const char* path, zlib_filefunc_def* pzlib_filefunc_def);

    /*
       Open a Zip file, like unz64Open, but provide a set of file low level API
          for read/write the zip file (see ioapi.h)
    */
    unzFile unzOpen2_64 (const void* path, zlib_filefunc64_def* pzlib_filefunc_def);

    int unzClose (unzFile file);
    /*
      Close a ZipFile opened with unzOpen.
      If there is files inside the .Zip opened with unzOpenCurrentFile (see later),
        these files MUST be closed with unzCloseCurrentFile before call unzClose.
      return UNZ_OK if there is no problem. */

    int unzGetGlobalInfo (unzFile file, unz_global_info* pglobal_info);

    /*
      Write info about the ZipFile in the *pglobal_info structure.
      No preparation of the structure is needed
      return UNZ_OK if there is no problem. */
    int unzGetGlobalInfo64 (unzFile file, unz_global_info64* pglobal_info);

    /*
      Get the global comment string of the ZipFile, in the szComment buffer.
      uSizeBuf is the size of the szComment buffer.
      return the number of byte copied or an error code <0
    */
    int unzGetGlobalComment (unzFile file, char* szComment, uLong uSizeBuf);

    /***************************************************************************/
    /* Unzip package allow you browse the directory of the zipfile */

    /*
      Set the current file of the zipfile to the first file.
      return UNZ_OK if there is no problem
    */
    int unzGoToFirstFile (unzFile file);

    /*
      Set the current file of the zipfile to the next file.
      return UNZ_OK if there is no problem
      return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
    */
    int unzGoToNextFile_ (unzFile file);

    /*
      Try locate the file szFileName in the zipfile.
      For the iCaseSensitivity signification, see unzStringFileNameCompare

      return value :
      UNZ_OK if the file is found. It becomes the current file.
      UNZ_END_OF_LIST_OF_FILE if the file is not found
    */
    int unzLocateFile_ (unzFile file, const char* szFileName, int iCaseSensitivity);

    /* ****************************************** */
    /* Ryan supplied functions */
    /* unz_file_info contain information about a file in the zipfile */
    struct unz_file_pos {
        uLong pos_in_zip_directory; /* offset in zip file directory */
        uLong num_of_file;          /* # of file */
    };

    int unzGetFilePos (unzFile file, unz_file_pos* file_pos);

    int unzGoToFilePos (unzFile file, unz_file_pos* file_pos);

    struct unz64_file_pos {
        ZPOS64_T pos_in_zip_directory; /* offset in zip file directory */
        ZPOS64_T num_of_file;          /* # of file */
    };

    int unzGetFilePos64 (unzFile file, unz64_file_pos* file_pos);

    int unzGoToFilePos64 (unzFile file, const unz64_file_pos* file_pos);

    /* ****************************************** */
    int unzGetCurrentFileInfo64 (unzFile          file,
                                 unz_file_info64* pfile_info,
                                 char*            szFileName,
                                 uLong            fileNameBufferSize,
                                 void*            extraField,
                                 uLong            extraFieldBufferSize,
                                 char*            szComment,
                                 uLong            commentBufferSize);

    int unzGetCurrentFileInfo (unzFile        file,
                               unz_file_info* pfile_info,
                               char*          szFileName,
                               uLong          fileNameBufferSize,
                               void*          extraField,
                               uLong          extraFieldBufferSize,
                               char*          szComment,
                               uLong          commentBufferSize);
    /*
      Get Info about the current file
      if pfile_info!=NULL, the *pfile_info structure will contain somes info about
            the current file
      if szFileName!=NULL, the filemane string will be copied in szFileName
                (fileNameBufferSize is the size of the buffer)
      if extraField!=NULL, the extra field information will be copied in extraField
                (extraFieldBufferSize is the size of the buffer).
                This is the Central-header version of the extra field
      if szComment!=NULL, the comment string of the file will be copied in szComment
                (commentBufferSize is the size of the buffer)
    */

    ZPOS64_T unzGetCurrentFileZStreamPos64 (unzFile file);

    /***************************************************************************/
    /* for reading the content of the current zipfile, you can open it, read data
       from it, and close it (you can close it before reading all the file)
       */
    /*
     Open for reading data the current file in the zipfile.
     If there is no error, the return value is UNZ_OK.
    */
    int unzOpenCurrentFile (unzFile file);

    /*
      Open for reading data the current file in the zipfile.
      password is a crypting password
      If there is no error, the return value is UNZ_OK.
    */
    int unzOpenCurrentFilePassword (unzFile file, const char* password);

    /*
     Same than unzOpenCurrentFile, but open for read raw the file (not uncompress)
       if raw==1
     *method will receive method of compression, *level will receive level of
        compression
     note : you can set level parameter as NULL (if you did not want known level,
            but you CANNOT set method parameter as NULL
    */
    int unzOpenCurrentFile2 (unzFile file, int* method, int* level, int raw);

    /*
      Same than unzOpenCurrentFile, but open for read raw the file (not uncompress)
        if raw==1
      *method will receive method of compression, *level will receive level of
         compression
      note : you can set level parameter as NULL (if you did not want known level,
             but you CANNOT set method parameter as NULL
    */
    int unzOpenCurrentFile3_ (unzFile file, int* method, int* level, int raw, const char* password);

    /*
      Close the file in zip opened with unzOpenCurrentFile
      Return UNZ_CRCERROR if all the file was read but the CRC is not good
    */
    int unzCloseCurrentFile_ (unzFile file);

    /*
     Read bytes from the current file (opened by unzOpenCurrentFile)
     buf contain buffer where data must be copied
     len the size of buf.

     return the number of byte copied if somes bytes are copied
     return 0 if the end of file was reached
     return <0 with error code if there is an error
       (UNZ_ERRNO for IO error, or zLib error for uncompress error)
    */
    int unzReadCurrentFile_ (unzFile file, voidp buf, unsigned len);

    z_off_t unztell (unzFile file);

    ZPOS64_T unztell64 (unzFile file);
    /*
      Give the current position in uncompressed data
    */

    /*
      return 1 if the end of file was reached, 0 elsewhere
    */
    int unzeof (unzFile file);

    /*
     Read extra field from the current file (opened by unzOpenCurrentFile)
     This is the local-header version of the extra field (sometimes, there is
       more info in the local-header version than in the central-header)

     if buf==NULL, it return the size of the local extra field

     if buf!=NULL, len is the size of the buffer, the extra header is copied in
       buf.
     the return value is the number of bytes copied in buf, or (if <0)
       the error code
    */
    int unzGetLocalExtrafield (unzFile file, voidp buf, unsigned len);

    /***************************************************************************/

    /* Get the current file offset */
    ZPOS64_T unzGetOffset64 (unzFile file);
    uLong    unzGetOffset (unzFile file);

    /* Set the current file offset */
    int unzSetOffset64 (unzFile file, ZPOS64_T pos);
    int unzSetOffset (unzFile file, uLong pos);
}
//// END OF unzip.h//////////////////////////////////////////////////////////////

///// START OF IOAPI.c //////////////////
/* ioapi.h -- IO base function header for compress/uncompress .zip
   part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications for Zip64 support
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

         For more info read MiniZip_info.txt

*/

namespace {
    voidpf call_zopen64 (const zlib_filefunc64_32_def* pfilefunc, const void* filename, int mode)
    {
        if (pfilefunc->zfile_func64.zopen64_file != NULL)
            return (*(pfilefunc->zfile_func64.zopen64_file)) (pfilefunc->zfile_func64.opaque, filename, mode);
        else {
            return (*(pfilefunc->zopen32_file)) (pfilefunc->zfile_func64.opaque, (const char*)filename, mode);
        }
    }
    long call_zseek64 (const zlib_filefunc64_32_def* pfilefunc, voidpf filestream, ZPOS64_T offset, int origin)
    {
        if (pfilefunc->zfile_func64.zseek64_file != NULL)
            return (*(pfilefunc->zfile_func64.zseek64_file)) (pfilefunc->zfile_func64.opaque, filestream, offset, origin);
        else {
            uLong offsetTruncated = (uLong)offset;
            if (offsetTruncated != offset)
                return -1;
            else
                return (*(pfilefunc->zseek32_file)) (pfilefunc->zfile_func64.opaque, filestream, offsetTruncated, origin);
        }
    }
    ZPOS64_T call_ztell64 (const zlib_filefunc64_32_def* pfilefunc, voidpf filestream)
    {
        if (pfilefunc->zfile_func64.zseek64_file != NULL)
            return (*(pfilefunc->zfile_func64.ztell64_file)) (pfilefunc->zfile_func64.opaque, filestream);
        else {
            uLong tell_uLong = (*(pfilefunc->ztell32_file)) (pfilefunc->zfile_func64.opaque, filestream);
            if ((tell_uLong) == numeric_limits<uLong>::max ())
                return (ZPOS64_T)-1;
            else
                return tell_uLong;
        }
    }
    void fill_zlib_filefunc64_32_def_from_filefunc32 (zlib_filefunc64_32_def* p_filefunc64_32, const zlib_filefunc_def* p_filefunc32)
    {
        p_filefunc64_32->zfile_func64.zopen64_file = NULL;
        p_filefunc64_32->zopen32_file              = p_filefunc32->zopen_file;
        p_filefunc64_32->zfile_func64.zerror_file  = p_filefunc32->zerror_file;
        p_filefunc64_32->zfile_func64.zread_file   = p_filefunc32->zread_file;
        p_filefunc64_32->zfile_func64.zwrite_file  = p_filefunc32->zwrite_file;
        p_filefunc64_32->zfile_func64.ztell64_file = NULL;
        p_filefunc64_32->zfile_func64.zseek64_file = NULL;
        p_filefunc64_32->zfile_func64.zclose_file  = p_filefunc32->zclose_file;
        p_filefunc64_32->zfile_func64.zerror_file  = p_filefunc32->zerror_file;
        p_filefunc64_32->zfile_func64.opaque       = p_filefunc32->opaque;
        p_filefunc64_32->zseek32_file              = p_filefunc32->zseek_file;
        p_filefunc64_32->ztell32_file              = p_filefunc32->ztell_file;
    }
}
///////////////////////////////////////////////// end of IOAPI.c /////////////////////

///////////////////////////////// START OF unzip.c ////////////////////
namespace {

#ifndef NOUNCRYPT
#define NOUNCRYPT
#endif

#ifndef CASESENSITIVITYDEFAULT_NO
#if !defined(unix) && !defined(CASESENSITIVITYDEFAULT_YES)
#define CASESENSITIVITYDEFAULT_NO
#endif
#endif

#ifndef UNZ_BUFSIZE
#define UNZ_BUFSIZE (16384)
#endif

#ifndef UNZ_MAXFILENAMEINZIP
#define UNZ_MAXFILENAMEINZIP (256)
#endif

#ifndef ALLOC
#define ALLOC(size) (malloc (size))
#endif
#ifndef TRYFREE
#define TRYFREE(p)    \
    {                 \
        if (p)        \
            free (p); \
    }
#endif

#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)

    const char unz_copyright[] = " unzip 1.01 Copyright 1998-2004 Gilles Vollant - http://www.winimage.com/zLibDll";

    /* unz_file_info_interntal contain internal info about a file in zipfile*/
    typedef struct unz_file_info64_internal_s {
        ZPOS64_T offset_curfile; /* relative offset of local header 8 bytes */
    } unz_file_info64_internal;

    /* file_in_zip_read_info_s contain internal information about a file in zipfile,
        when reading and decompress it */
    typedef struct {
        char*    read_buffer; /* internal buffer for compressed data */
        z_stream stream;      /* zLib stream structure for inflate */

#ifdef HAVE_BZIP2
        bz_stream bstream; /* bzLib stream structure for bziped */
#endif

        ZPOS64_T pos_in_zipfile;     /* position in byte on the zipfile, for fseek*/
        uLong    stream_initialised; /* flag set if stream structure is initialised*/

        ZPOS64_T offset_local_extrafield; /* offset of the local extra field */
        uInt     size_local_extrafield;   /* size of the local extra field */
        ZPOS64_T pos_local_extrafield;    /* position in the local extra field in read*/
        ZPOS64_T total_out_64;

        uLong                  crc32;                  /* crc32 of all data uncompressed */
        uLong                  crc32_wait;             /* crc32 we must obtain after decompress all */
        ZPOS64_T               rest_read_compressed;   /* number of byte to be decompressed */
        ZPOS64_T               rest_read_uncompressed; /*number of byte to be obtained after decomp*/
        zlib_filefunc64_32_def z_filefunc;
        voidpf                 filestream;              /* io structore of the zipfile */
        uLong                  compression_method;      /* compression method (0==store) */
        ZPOS64_T               byte_before_the_zipfile; /* byte before the zipfile, (>0 for sfx)*/
        int                    raw;
    } file_in_zip64_read_info_s;

    /* unz64_s contain internal information about the zipfile
    */
    struct unz64_s {
        zlib_filefunc64_32_def z_filefunc;
        int                    is64bitOpenFunction;
        voidpf                 filestream;              /* io structore of the zipfile */
        unz_global_info64      gi;                      /* public global information */
        ZPOS64_T               byte_before_the_zipfile; /* byte before the zipfile, (>0 for sfx)*/
        ZPOS64_T               num_file;                /* number of the current file in the zipfile*/
        ZPOS64_T               pos_in_central_dir;      /* pos of the current file in the central dir*/
        ZPOS64_T               current_file_ok;         /* flag about the usability of the current file*/
        ZPOS64_T               central_pos;             /* position of the beginning of the central dir*/

        ZPOS64_T size_central_dir;   /* size of the central directory  */
        ZPOS64_T offset_central_dir; /* offset of start of central directory with
                                   respect to the starting disk number */

        unz_file_info64            cur_file_info;          /* public info about the current file in zip*/
        unz_file_info64_internal   cur_file_info_internal; /* private info about it*/
        file_in_zip64_read_info_s* pfile_in_zip_read;      /* structure about the current
                                        file if we are decompressing it */
        int                        encrypted;

        int isZip64;

#ifndef NOUNCRYPT
        unsigned long  keys[3]; /* keys defining the pseudo-random sequence */
        const z_crc_t* pcrc_32_tab;
#endif
    };

    /* ===========================================================================
         Read a byte from a gz_stream; update next_in and avail_in. Return EOF
       for end of file.
       IN assertion: the stream s has been sucessfully opened for reading.
    */
    int unz64local_getByte (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream, int* pi)
    {
        unsigned char c;
        int           err = (int)ZREAD64 (*pzlib_filefunc_def, filestream, &c, 1);
        if (err == 1) {
            *pi = (int)c;
            return UNZ_OK;
        }
        else {
            if (ZERROR64 (*pzlib_filefunc_def, filestream))
                return UNZ_ERRNO;
            else
                return UNZ_EOF;
        }
    }

    /* ===========================================================================
       Reads a long in LSB order from the given gz_stream. Sets
    */
    int unz64local_getShort (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream, uLong* pX)
    {
        uLong x;
        int   i = 0;
        int   err;

        err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x   = (uLong)i;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x |= ((uLong)i) << 8;

        if (err == UNZ_OK)
            *pX = x;
        else
            *pX = 0;
        return err;
    }

    int unz64local_getLong (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream, uLong* pX)
    {
        uLong x;
        int   i = 0;
        int   err;

        err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x   = (uLong)i;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x |= ((uLong)i) << 8;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x |= ((uLong)i) << 16;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((uLong)i) << 24;

        if (err == UNZ_OK)
            *pX = x;
        else
            *pX = 0;
        return err;
    }

    int unz64local_getLong64 (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream, ZPOS64_T* pX)
    {
        ZPOS64_T x;
        int      i = 0;
        int      err;

        err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x   = (ZPOS64_T)i;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x |= ((ZPOS64_T)i) << 8;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x |= ((ZPOS64_T)i) << 16;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x |= ((ZPOS64_T)i) << 24;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x |= ((ZPOS64_T)i) << 32;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x |= ((ZPOS64_T)i) << 40;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x |= ((ZPOS64_T)i) << 48;

        if (err == UNZ_OK)
            err = unz64local_getByte (pzlib_filefunc_def, filestream, &i);
        x |= ((ZPOS64_T)i) << 56;

        if (err == UNZ_OK)
            *pX = x;
        else
            *pX = 0;
        return err;
    }

    /* My own strcmpi / strcasecmp */
    int strcmpcasenosensitive_internal (const char* fileName1, const char* fileName2)
    {
        for (;;) {
            char c1 = *(fileName1++);
            char c2 = *(fileName2++);
            if ((c1 >= 'a') and (c1 <= 'z'))
                c1 -= 0x20;
            if ((c2 >= 'a') and (c2 <= 'z'))
                c2 -= 0x20;
            if (c1 == '\0')
                return ((c2 == '\0') ? 0 : -1);
            if (c2 == '\0')
                return 1;
            if (c1 < c2)
                return -1;
            if (c1 > c2)
                return 1;
        }
    }

#ifdef CASESENSITIVITYDEFAULT_NO
#define CASESENSITIVITYDEFAULTVALUE 2
#else
#define CASESENSITIVITYDEFAULTVALUE 1
#endif

#ifndef STRCMPCASENOSENTIVEFUNCTION
#define STRCMPCASENOSENTIVEFUNCTION strcmpcasenosensitive_internal
#endif

    /*
       Compare two filename (fileName1,fileName2).
       If iCaseSenisivity = 1, comparision is case sensitivity (like strcmp)
       If iCaseSenisivity = 2, comparision is not case sensitivity (like strcmpi
                                                                    or strcasecmp)
       If iCaseSenisivity = 0, case sensitivity is defaut of your operating system
            (like 1 on Unix, 2 on Windows)

    */
    int unzStringFileNameCompare (const char* fileName1,
                                  const char* fileName2,
                                  int         iCaseSensitivity)

    {
        if (iCaseSensitivity == 0)
            iCaseSensitivity = CASESENSITIVITYDEFAULTVALUE;

        if (iCaseSensitivity == 1)
            return strcmp (fileName1, fileName2);

        return STRCMPCASENOSENTIVEFUNCTION (fileName1, fileName2);
    }

#ifndef BUFREADCOMMENT
#define BUFREADCOMMENT (0x400)
#endif

    /*
      Locate the Central directory of a zipfile (at the end, just before
        the global comment)
    */
    ZPOS64_T unz64local_SearchCentralDir (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream)
    {
        unsigned char* buf;
        ZPOS64_T       uSizeFile;
        ZPOS64_T       uBackRead;
        ZPOS64_T       uMaxBack  = 0xffff; /* maximum size of global comment */
        ZPOS64_T       uPosFound = 0;

        if (ZSEEK64 (*pzlib_filefunc_def, filestream, 0, ZLIB_FILEFUNC_SEEK_END) != 0)
            return 0;

        uSizeFile = ZTELL64 (*pzlib_filefunc_def, filestream);

        if (uMaxBack > uSizeFile)
            uMaxBack = uSizeFile;

        buf = (unsigned char*)ALLOC (BUFREADCOMMENT + 4);
        if (buf == NULL)
            return 0;

        uBackRead = 4;
        while (uBackRead < uMaxBack) {
            uLong    uReadSize;
            ZPOS64_T uReadPos;
            int      i;
            if (uBackRead + BUFREADCOMMENT > uMaxBack)
                uBackRead = uMaxBack;
            else
                uBackRead += BUFREADCOMMENT;
            uReadPos = uSizeFile - uBackRead;

            uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ? (BUFREADCOMMENT + 4) : (uLong) (uSizeFile - uReadPos);
            if (ZSEEK64 (*pzlib_filefunc_def, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0)
                break;

            if (ZREAD64 (*pzlib_filefunc_def, filestream, buf, uReadSize) != uReadSize)
                break;

            for (i = (int)uReadSize - 3; (i--) > 0;)
                if (((*(buf + i)) == 0x50) and ((*(buf + i + 1)) == 0x4b) &&
                    ((*(buf + i + 2)) == 0x05) and ((*(buf + i + 3)) == 0x06)) {
                    uPosFound = uReadPos + i;
                    break;
                }

            if (uPosFound != 0)
                break;
        }
        TRYFREE (buf);
        return uPosFound;
    }

    /*
      Locate the Central directory 64 of a zipfile (at the end, just before
        the global comment)
    */
    ZPOS64_T unz64local_SearchCentralDir64 (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream)
    {
        unsigned char* buf;
        ZPOS64_T       uSizeFile;
        ZPOS64_T       uBackRead;
        ZPOS64_T       uMaxBack  = 0xffff; /* maximum size of global comment */
        ZPOS64_T       uPosFound = 0;
        uLong          uL;
        ZPOS64_T       relativeOffset;

        if (ZSEEK64 (*pzlib_filefunc_def, filestream, 0, ZLIB_FILEFUNC_SEEK_END) != 0)
            return 0;

        uSizeFile = ZTELL64 (*pzlib_filefunc_def, filestream);

        if (uMaxBack > uSizeFile)
            uMaxBack = uSizeFile;

        buf = (unsigned char*)ALLOC (BUFREADCOMMENT + 4);
        if (buf == NULL)
            return 0;

        uBackRead = 4;
        while (uBackRead < uMaxBack) {
            uLong    uReadSize;
            ZPOS64_T uReadPos;
            int      i;
            if (uBackRead + BUFREADCOMMENT > uMaxBack)
                uBackRead = uMaxBack;
            else
                uBackRead += BUFREADCOMMENT;
            uReadPos = uSizeFile - uBackRead;

            uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ? (BUFREADCOMMENT + 4) : (uLong) (uSizeFile - uReadPos);
            if (ZSEEK64 (*pzlib_filefunc_def, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0)
                break;

            if (ZREAD64 (*pzlib_filefunc_def, filestream, buf, uReadSize) != uReadSize)
                break;

            for (i = (int)uReadSize - 3; (i--) > 0;)
                if (((*(buf + i)) == 0x50) and ((*(buf + i + 1)) == 0x4b) &&
                    ((*(buf + i + 2)) == 0x06) and ((*(buf + i + 3)) == 0x07)) {
                    uPosFound = uReadPos + i;
                    break;
                }

            if (uPosFound != 0)
                break;
        }
        TRYFREE (buf);
        if (uPosFound == 0)
            return 0;

        /* Zip64 end of central directory locator */
        if (ZSEEK64 (*pzlib_filefunc_def, filestream, uPosFound, ZLIB_FILEFUNC_SEEK_SET) != 0)
            return 0;

        /* the signature, already checked */
        if (unz64local_getLong (pzlib_filefunc_def, filestream, &uL) != UNZ_OK)
            return 0;

        /* number of the disk with the start of the zip64 end of  central directory */
        if (unz64local_getLong (pzlib_filefunc_def, filestream, &uL) != UNZ_OK)
            return 0;
        if (uL != 0)
            return 0;

        /* relative offset of the zip64 end of central directory record */
        if (unz64local_getLong64 (pzlib_filefunc_def, filestream, &relativeOffset) != UNZ_OK)
            return 0;

        /* total number of disks */
        if (unz64local_getLong (pzlib_filefunc_def, filestream, &uL) != UNZ_OK)
            return 0;
        if (uL != 1)
            return 0;

        /* Goto end of central directory record */
        if (ZSEEK64 (*pzlib_filefunc_def, filestream, relativeOffset, ZLIB_FILEFUNC_SEEK_SET) != 0)
            return 0;

        /* the signature */
        if (unz64local_getLong (pzlib_filefunc_def, filestream, &uL) != UNZ_OK)
            return 0;

        if (uL != 0x06064b50)
            return 0;

        return relativeOffset;
    }

    /*
      Open a Zip file. path contain the full pathname (by example,
         on a Windows NT computer "c:\\test\\zlib114.zip" or on an Unix computer
         "zlib/zlib114.zip".
         If the zipfile cannot be opened (file doesn't exist or in not valid), the
           return value is NULL.
         Else, the return value is a unzFile Handle, usable with other function
           of this unzip package.
    */
    unzFile unzOpenInternal (const void* path, zlib_filefunc64_32_def* pzlib_filefunc64_32_def, int is64bitOpenFunction)
    {
        unz64_s  us;
        unz64_s* s;
        ZPOS64_T central_pos;
        uLong    uL;

        uLong    number_disk;         /* number of the current dist, used for
                                   spaning ZIP, unsupported, always 0*/
        uLong    number_disk_with_CD; /* number the the disk with central dir, used
                                   for spaning ZIP, unsupported, always 0*/
        ZPOS64_T number_entry_CD;     /* total number of entries in
                                   the central dir
                                   (same than number_entry on nospan) */

        int err = UNZ_OK;

        us.z_filefunc.zseek32_file = NULL;
        us.z_filefunc.ztell32_file = NULL;

        AssertNotNull (pzlib_filefunc64_32_def);
        us.z_filefunc          = *pzlib_filefunc64_32_def;
        us.is64bitOpenFunction = is64bitOpenFunction;

        us.filestream = ZOPEN64 (us.z_filefunc,
                                 path,
                                 ZLIB_FILEFUNC_MODE_READ |
                                     ZLIB_FILEFUNC_MODE_EXISTING);
        if (us.filestream == NULL)
            return NULL;

        central_pos = unz64local_SearchCentralDir64 (&us.z_filefunc, us.filestream);
        if (central_pos) {
            uLong    uS;
            ZPOS64_T uL64;

            us.isZip64 = 1;

            if (ZSEEK64 (us.z_filefunc, us.filestream,
                         central_pos, ZLIB_FILEFUNC_SEEK_SET) != 0)
                err = UNZ_ERRNO;

            /* the signature, already checked */
            if (unz64local_getLong (&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
                err = UNZ_ERRNO;

            /* size of zip64 end of central directory record */
            if (unz64local_getLong64 (&us.z_filefunc, us.filestream, &uL64) != UNZ_OK)
                err = UNZ_ERRNO;

            /* version made by */
            if (unz64local_getShort (&us.z_filefunc, us.filestream, &uS) != UNZ_OK)
                err = UNZ_ERRNO;

            /* version needed to extract */
            if (unz64local_getShort (&us.z_filefunc, us.filestream, &uS) != UNZ_OK)
                err = UNZ_ERRNO;

            /* number of this disk */
            if (unz64local_getLong (&us.z_filefunc, us.filestream, &number_disk) != UNZ_OK)
                err = UNZ_ERRNO;

            /* number of the disk with the start of the central directory */
            if (unz64local_getLong (&us.z_filefunc, us.filestream, &number_disk_with_CD) != UNZ_OK)
                err = UNZ_ERRNO;

            /* total number of entries in the central directory on this disk */
            if (unz64local_getLong64 (&us.z_filefunc, us.filestream, &us.gi.number_entry) != UNZ_OK)
                err = UNZ_ERRNO;

            /* total number of entries in the central directory */
            if (unz64local_getLong64 (&us.z_filefunc, us.filestream, &number_entry_CD) != UNZ_OK)
                err = UNZ_ERRNO;

            if ((number_entry_CD != us.gi.number_entry) or (number_disk_with_CD != 0) or (number_disk != 0))
                err = UNZ_BADZIPFILE;

            /* size of the central directory */
            if (unz64local_getLong64 (&us.z_filefunc, us.filestream, &us.size_central_dir) != UNZ_OK)
                err = UNZ_ERRNO;

            /* offset of start of central directory with respect to the
              starting disk number */
            if (unz64local_getLong64 (&us.z_filefunc, us.filestream, &us.offset_central_dir) != UNZ_OK)
                err = UNZ_ERRNO;

            us.gi.size_comment = 0;
        }
        else {
            central_pos = unz64local_SearchCentralDir (&us.z_filefunc, us.filestream);
            if (central_pos == 0)
                err = UNZ_ERRNO;

            us.isZip64 = 0;

            if (ZSEEK64 (us.z_filefunc, us.filestream,
                         central_pos, ZLIB_FILEFUNC_SEEK_SET) != 0)
                err = UNZ_ERRNO;

            /* the signature, already checked */
            if (unz64local_getLong (&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
                err = UNZ_ERRNO;

            /* number of this disk */
            if (unz64local_getShort (&us.z_filefunc, us.filestream, &number_disk) != UNZ_OK)
                err = UNZ_ERRNO;

            /* number of the disk with the start of the central directory */
            if (unz64local_getShort (&us.z_filefunc, us.filestream, &number_disk_with_CD) != UNZ_OK)
                err = UNZ_ERRNO;

            /* total number of entries in the central dir on this disk */
            if (unz64local_getShort (&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
                err = UNZ_ERRNO;
            us.gi.number_entry = uL;

            /* total number of entries in the central dir */
            if (unz64local_getShort (&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
                err = UNZ_ERRNO;
            number_entry_CD = uL;

            if ((number_entry_CD != us.gi.number_entry) ||
                (number_disk_with_CD != 0) ||
                (number_disk != 0))
                err = UNZ_BADZIPFILE;

            /* size of the central directory */
            if (unz64local_getLong (&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
                err = UNZ_ERRNO;
            us.size_central_dir = uL;

            /* offset of start of central directory with respect to the
                starting disk number */
            if (unz64local_getLong (&us.z_filefunc, us.filestream, &uL) != UNZ_OK)
                err = UNZ_ERRNO;
            us.offset_central_dir = uL;

            /* zipfile comment length */
            if (unz64local_getShort (&us.z_filefunc, us.filestream, &us.gi.size_comment) != UNZ_OK)
                err = UNZ_ERRNO;
        }

        if ((central_pos < us.offset_central_dir + us.size_central_dir) &&
            (err == UNZ_OK))
            err = UNZ_BADZIPFILE;

        if (err != UNZ_OK) {
            ZCLOSE64 (us.z_filefunc, us.filestream);
            return NULL;
        }

        us.byte_before_the_zipfile = central_pos -
                                     (us.offset_central_dir + us.size_central_dir);
        us.central_pos       = central_pos;
        us.pfile_in_zip_read = NULL;
        us.encrypted         = 0;

        s = (unz64_s*)ALLOC (sizeof (unz64_s));
        if (s != NULL) {
            *s = us;
            unzGoToFirstFile ((unzFile)s);
        }
        return (unzFile)s;
    }

    unzFile unzOpen2 (const char* path, zlib_filefunc_def* pzlib_filefunc32_def)
    {
        if (pzlib_filefunc32_def != NULL) {
            zlib_filefunc64_32_def zlib_filefunc64_32_def_fill;
            fill_zlib_filefunc64_32_def_from_filefunc32 (&zlib_filefunc64_32_def_fill, pzlib_filefunc32_def);
            return unzOpenInternal (path, &zlib_filefunc64_32_def_fill, 0);
        }
        else
            return unzOpenInternal (path, NULL, 0);
    }

    unzFile unzOpen2_64 (const void* path, zlib_filefunc64_def* pzlib_filefunc_def)
    {
        if (pzlib_filefunc_def != NULL) {
            zlib_filefunc64_32_def zlib_filefunc64_32_def_fill;
            zlib_filefunc64_32_def_fill.zfile_func64 = *pzlib_filefunc_def;
            zlib_filefunc64_32_def_fill.ztell32_file = NULL;
            zlib_filefunc64_32_def_fill.zseek32_file = NULL;
            return unzOpenInternal (path, &zlib_filefunc64_32_def_fill, 1);
        }
        else
            return unzOpenInternal (path, NULL, 1);
    }

    unzFile unzOpen (const char* path)
    {
        return unzOpenInternal (path, NULL, 0);
    }

    unzFile unzOpen64 (const void* path)
    {
        return unzOpenInternal (path, NULL, 1);
    }

    /*
      Close a ZipFile opened with unzOpen.
      If there is files inside the .Zip opened with unzOpenCurrentFile (see later),
        these files MUST be closed with unzCloseCurrentFile_ before call unzClose.
      return UNZ_OK if there is no problem. */
    int unzClose (unzFile file)
    {
        unz64_s* s;
        if (file == NULL)
            return UNZ_PARAMERROR;
        s = (unz64_s*)file;

        if (s->pfile_in_zip_read != NULL)
            unzCloseCurrentFile_ (file);

        ZCLOSE64 (s->z_filefunc, s->filestream);
        TRYFREE (s);
        return UNZ_OK;
    }

    /*
      Write info about the ZipFile in the *pglobal_info structure.
      No preparation of the structure is needed
      return UNZ_OK if there is no problem. */
    int unzGetGlobalInfo64 (unzFile file, unz_global_info64* pglobal_info)
    {
        unz64_s* s;
        if (file == NULL)
            return UNZ_PARAMERROR;
        s             = (unz64_s*)file;
        *pglobal_info = s->gi;
        return UNZ_OK;
    }

    int unzGetGlobalInfo (unzFile file, unz_global_info* pglobal_info32)
    {
        unz64_s* s;
        if (file == NULL)
            return UNZ_PARAMERROR;
        s = (unz64_s*)file;
        /* to do : check if number_entry is not truncated */
        pglobal_info32->number_entry = (uLong)s->gi.number_entry;
        pglobal_info32->size_comment = s->gi.size_comment;
        return UNZ_OK;
    }
    /*
       Translate date/time from Dos format to tm_unz (readable more easilty)
    */
    void unz64local_DosDateToTmuDate_ (ZPOS64_T ulDosDate, tm_unz* ptm)
    {
        ZPOS64_T uDate;
        uDate        = (ZPOS64_T) (ulDosDate >> 16);
        ptm->tm_mday = (uInt) (uDate & 0x1f);
        ptm->tm_mon  = (uInt) ((((uDate)&0x1E0) / 0x20) - 1);
        ptm->tm_year = (uInt) (((uDate & 0x0FE00) / 0x0200) + 1980);

        ptm->tm_hour = (uInt) ((ulDosDate & 0xF800) / 0x800);
        ptm->tm_min  = (uInt) ((ulDosDate & 0x7E0) / 0x20);
        ptm->tm_sec  = (uInt) (2 * (ulDosDate & 0x1f));
    }

    /*
      Get Info about the current file in the zipfile, with internal only info
    */
    int unz64local_GetCurrentFileInfoInternal_ (unzFile file, unz_file_info64* pfile_info, unz_file_info64_internal* pfile_info_internal, char* szFileName, uLong fileNameBufferSize, void* extraField, uLong extraFieldBufferSize, char* szComment, uLong commentBufferSize)
    {
        unz64_s*                 s;
        unz_file_info64          file_info;
        unz_file_info64_internal file_info_internal;
        int                      err = UNZ_OK;
        uLong                    uMagic;
        long                     lSeek = 0;
        uLong                    uL;

        if (file == NULL)
            return UNZ_PARAMERROR;
        s = (unz64_s*)file;
        if (ZSEEK64 (s->z_filefunc, s->filestream,
                     s->pos_in_central_dir + s->byte_before_the_zipfile,
                     ZLIB_FILEFUNC_SEEK_SET) != 0)
            err = UNZ_ERRNO;

        /* we check the magic */
        if (err == UNZ_OK) {
            if (unz64local_getLong (&s->z_filefunc, s->filestream, &uMagic) != UNZ_OK)
                err = UNZ_ERRNO;
            else if (uMagic != 0x02014b50)
                err = UNZ_BADZIPFILE;
        }

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &file_info.version) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &file_info.version_needed) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &file_info.flag) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &file_info.compression_method) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getLong (&s->z_filefunc, s->filestream, &file_info.dosDate) != UNZ_OK)
            err = UNZ_ERRNO;

        unz64local_DosDateToTmuDate_ (file_info.dosDate, &file_info.tmu_date);

        if (unz64local_getLong (&s->z_filefunc, s->filestream, &file_info.crc) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getLong (&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
            err = UNZ_ERRNO;
        file_info.compressed_size = uL;

        if (unz64local_getLong (&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
            err = UNZ_ERRNO;
        file_info.uncompressed_size = uL;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &file_info.size_filename) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &file_info.size_file_extra) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &file_info.size_file_comment) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &file_info.disk_num_start) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &file_info.internal_fa) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getLong (&s->z_filefunc, s->filestream, &file_info.external_fa) != UNZ_OK)
            err = UNZ_ERRNO;

        // relative offset of local header
        if (unz64local_getLong (&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
            err = UNZ_ERRNO;
        file_info_internal.offset_curfile = uL;

        lSeek += file_info.size_filename;
        if ((err == UNZ_OK) and (szFileName != NULL)) {
            uLong uSizeRead;
            if (file_info.size_filename < fileNameBufferSize) {
                *(szFileName + file_info.size_filename) = '\0';
                uSizeRead                               = file_info.size_filename;
            }
            else
                uSizeRead = fileNameBufferSize;

            if ((file_info.size_filename > 0) and (fileNameBufferSize > 0))
                if (ZREAD64 (s->z_filefunc, s->filestream, szFileName, uSizeRead) != uSizeRead)
                    err = UNZ_ERRNO;
            lSeek -= uSizeRead;
        }

        // Read extrafield
        if ((err == UNZ_OK) and (extraField != NULL)) {
            ZPOS64_T uSizeRead;
            if (file_info.size_file_extra < extraFieldBufferSize)
                uSizeRead = file_info.size_file_extra;
            else
                uSizeRead = extraFieldBufferSize;

            if (lSeek != 0) {
                if (ZSEEK64 (s->z_filefunc, s->filestream, lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0)
                    lSeek = 0;
                else
                    err = UNZ_ERRNO;
            }

            if ((file_info.size_file_extra > 0) and (extraFieldBufferSize > 0))
                if (ZREAD64 (s->z_filefunc, s->filestream, extraField, (uLong)uSizeRead) != uSizeRead)
                    err = UNZ_ERRNO;

            lSeek += file_info.size_file_extra - (uLong)uSizeRead;
        }
        else
            lSeek += file_info.size_file_extra;

        if ((err == UNZ_OK) and (file_info.size_file_extra != 0)) {
            uLong acc = 0;

            // since lSeek now points to after the extra field we need to move back
            lSeek -= file_info.size_file_extra;

            if (lSeek != 0) {
                if (ZSEEK64 (s->z_filefunc, s->filestream, lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0)
                    lSeek = 0;
                else
                    err = UNZ_ERRNO;
            }

            while (acc < file_info.size_file_extra) {
                uLong headerId;
                uLong dataSize;

                if (unz64local_getShort (&s->z_filefunc, s->filestream, &headerId) != UNZ_OK)
                    err = UNZ_ERRNO;

                if (unz64local_getShort (&s->z_filefunc, s->filestream, &dataSize) != UNZ_OK)
                    err = UNZ_ERRNO;

                /* ZIP64 extra fields */
                if (headerId == 0x0001) {
                    uLong uL;

                    if (file_info.uncompressed_size == MAXU32) {
                        if (unz64local_getLong64 (&s->z_filefunc, s->filestream, &file_info.uncompressed_size) != UNZ_OK)
                            err = UNZ_ERRNO;
                    }

                    if (file_info.compressed_size == MAXU32) {
                        if (unz64local_getLong64 (&s->z_filefunc, s->filestream, &file_info.compressed_size) != UNZ_OK)
                            err = UNZ_ERRNO;
                    }

                    if (file_info_internal.offset_curfile == MAXU32) {
                        /* Relative Header offset */
                        if (unz64local_getLong64 (&s->z_filefunc, s->filestream, &file_info_internal.offset_curfile) != UNZ_OK)
                            err = UNZ_ERRNO;
                    }

                    if (file_info.disk_num_start == MAXU32) {
                        /* Disk Start Number */
                        if (unz64local_getLong (&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
                            err = UNZ_ERRNO;
                    }
                }
                else {
                    if (ZSEEK64 (s->z_filefunc, s->filestream, dataSize, ZLIB_FILEFUNC_SEEK_CUR) != 0)
                        err = UNZ_ERRNO;
                }

                acc += 2 + 2 + dataSize;
            }
        }

        if ((err == UNZ_OK) and (szComment != NULL)) {
            uLong uSizeRead;
            if (file_info.size_file_comment < commentBufferSize) {
                *(szComment + file_info.size_file_comment) = '\0';
                uSizeRead                                  = file_info.size_file_comment;
            }
            else
                uSizeRead = commentBufferSize;

            if (lSeek != 0) {
                if (ZSEEK64 (s->z_filefunc, s->filestream, lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0)
                    lSeek = 0;
                else
                    err = UNZ_ERRNO;
            }

            if ((file_info.size_file_comment > 0) and (commentBufferSize > 0))
                if (ZREAD64 (s->z_filefunc, s->filestream, szComment, uSizeRead) != uSizeRead)
                    err = UNZ_ERRNO;
            lSeek += file_info.size_file_comment - uSizeRead;
        }
        else
            lSeek += file_info.size_file_comment;

        if ((err == UNZ_OK) and (pfile_info != NULL))
            *pfile_info = file_info;

        if ((err == UNZ_OK) and (pfile_info_internal != NULL))
            *pfile_info_internal = file_info_internal;

        return err;
    }

    /*
      Write info about the ZipFile in the *pglobal_info structure.
      No preparation of the structure is needed
      return UNZ_OK if there is no problem.
    */
    int unzGetCurrentFileInfo64 (unzFile file, unz_file_info64* pfile_info, char* szFileName, uLong fileNameBufferSize, void* extraField, uLong extraFieldBufferSize, char* szComment, uLong commentBufferSize)
    {
        return unz64local_GetCurrentFileInfoInternal_ (file, pfile_info, NULL, szFileName, fileNameBufferSize, extraField, extraFieldBufferSize, szComment, commentBufferSize);
    }

    int unzGetCurrentFileInfo (unzFile file, unz_file_info* pfile_info, char* szFileName, uLong fileNameBufferSize, void* extraField, uLong extraFieldBufferSize, char* szComment, uLong commentBufferSize)
    {
        int             err;
        unz_file_info64 file_info64;
        err = unz64local_GetCurrentFileInfoInternal_ (file, &file_info64, NULL,
                                                      szFileName, fileNameBufferSize,
                                                      extraField, extraFieldBufferSize,
                                                      szComment, commentBufferSize);
        if ((err == UNZ_OK) and (pfile_info != NULL)) {
            pfile_info->version            = file_info64.version;
            pfile_info->version_needed     = file_info64.version_needed;
            pfile_info->flag               = file_info64.flag;
            pfile_info->compression_method = file_info64.compression_method;
            pfile_info->dosDate            = file_info64.dosDate;
            pfile_info->crc                = file_info64.crc;

            pfile_info->size_filename     = file_info64.size_filename;
            pfile_info->size_file_extra   = file_info64.size_file_extra;
            pfile_info->size_file_comment = file_info64.size_file_comment;

            pfile_info->disk_num_start = file_info64.disk_num_start;
            pfile_info->internal_fa    = file_info64.internal_fa;
            pfile_info->external_fa    = file_info64.external_fa;

            pfile_info->tmu_date = file_info64.tmu_date,

            pfile_info->compressed_size   = (uLong)file_info64.compressed_size;
            pfile_info->uncompressed_size = (uLong)file_info64.uncompressed_size;
        }
        return err;
    }

    /*
      Set the current file of the zipfile to the first file.
      return UNZ_OK if there is no problem
    */
    int unzGoToFirstFile (unzFile file)
    {
        int      err = UNZ_OK;
        unz64_s* s;
        if (file == NULL)
            return UNZ_PARAMERROR;
        s                     = (unz64_s*)file;
        s->pos_in_central_dir = s->offset_central_dir;
        s->num_file           = 0;
        err                   = unz64local_GetCurrentFileInfoInternal_ (file, &s->cur_file_info,
                                                      &s->cur_file_info_internal,
                                                      NULL, 0, NULL, 0, NULL, 0);
        s->current_file_ok    = (err == UNZ_OK);
        return err;
    }

    /*
      Set the current file of the zipfile to the next file.
      return UNZ_OK if there is no problem
      return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
    */
    int unzGoToNextFile_ (unzFile file)
    {
        unz64_s* s;
        int      err;

        if (file == NULL)
            return UNZ_PARAMERROR;
        s = (unz64_s*)file;
        if (!s->current_file_ok)
            return UNZ_END_OF_LIST_OF_FILE;
        if (s->gi.number_entry != 0xffff) /* 2^16 files overflow hack */
            if (s->num_file + 1 == s->gi.number_entry)
                return UNZ_END_OF_LIST_OF_FILE;

        s->pos_in_central_dir += SIZECENTRALDIRITEM + s->cur_file_info.size_filename +
                                 s->cur_file_info.size_file_extra + s->cur_file_info.size_file_comment;
        s->num_file++;
        err                = unz64local_GetCurrentFileInfoInternal_ (file, &s->cur_file_info,
                                                      &s->cur_file_info_internal,
                                                      NULL, 0, NULL, 0, NULL, 0);
        s->current_file_ok = (err == UNZ_OK);
        return err;
    }

    /*
      Try locate the file szFileName in the zipfile.
      For the iCaseSensitivity signification, see unzStringFileNameCompare

      return value :
      UNZ_OK if the file is found. It becomes the current file.
      UNZ_END_OF_LIST_OF_FILE if the file is not found
    */
    int unzLocateFile_ (unzFile file, const char* szFileName, int iCaseSensitivity)
    {
        unz64_s* s;
        int      err;

        /* We remember the 'current' position in the file so that we can jump
         * back there if we fail.
         */
        unz_file_info64          cur_file_infoSaved;
        unz_file_info64_internal cur_file_info_internalSaved;
        ZPOS64_T                 num_fileSaved;
        ZPOS64_T                 pos_in_central_dirSaved;

        if (file == NULL)
            return UNZ_PARAMERROR;

        if (strlen (szFileName) >= UNZ_MAXFILENAMEINZIP)
            return UNZ_PARAMERROR;

        s = (unz64_s*)file;
        if (!s->current_file_ok)
            return UNZ_END_OF_LIST_OF_FILE;

        /* Save the current state */
        num_fileSaved               = s->num_file;
        pos_in_central_dirSaved     = s->pos_in_central_dir;
        cur_file_infoSaved          = s->cur_file_info;
        cur_file_info_internalSaved = s->cur_file_info_internal;

        err = unzGoToFirstFile (file);

        while (err == UNZ_OK) {
            char szCurrentFileName[UNZ_MAXFILENAMEINZIP + 1];
            err = unzGetCurrentFileInfo64 (file, NULL,
                                           szCurrentFileName, sizeof (szCurrentFileName) - 1,
                                           NULL, 0, NULL, 0);
            if (err == UNZ_OK) {
                if (unzStringFileNameCompare (szCurrentFileName,
                                              szFileName, iCaseSensitivity) == 0)
                    return UNZ_OK;
                err = unzGoToNextFile_ (file);
            }
        }

        /* We failed, so restore the state of the 'current file' to where we
         * were.
         */
        s->num_file               = num_fileSaved;
        s->pos_in_central_dir     = pos_in_central_dirSaved;
        s->cur_file_info          = cur_file_infoSaved;
        s->cur_file_info_internal = cur_file_info_internalSaved;
        return err;
    }

    /*
    ///////////////////////////////////////////
    // Contributed by Ryan Haksi (mailto://cryogen@infoserve.net)
    // I need random access
    //
    // Further optimization could be realized by adding an ability
    // to cache the directory in memory. The goal being a single
    // comprehensive file read to put the file I need in a memory.
    */

    /*
    typedef struct unz_file_pos_s
    {
        ZPOS64_T pos_in_zip_directory;   // offset in file
        ZPOS64_T num_of_file;            // # of file
    } unz_file_pos;
    */

    int unzGetFilePos64 (unzFile file, unz64_file_pos* file_pos)
    {
        unz64_s* s;

        if (file == NULL or file_pos == NULL)
            return UNZ_PARAMERROR;
        s = (unz64_s*)file;
        if (!s->current_file_ok)
            return UNZ_END_OF_LIST_OF_FILE;

        file_pos->pos_in_zip_directory = s->pos_in_central_dir;
        file_pos->num_of_file          = s->num_file;

        return UNZ_OK;
    }

    int unzGetFilePos (unzFile file, unz_file_pos* file_pos)
    {
        unz64_file_pos file_pos64;
        int            err = unzGetFilePos64 (file, &file_pos64);
        if (err == UNZ_OK) {
            file_pos->pos_in_zip_directory = (uLong)file_pos64.pos_in_zip_directory;
            file_pos->num_of_file          = (uLong)file_pos64.num_of_file;
        }
        return err;
    }

    int unzGoToFilePos64 (unzFile file, const unz64_file_pos* file_pos)
    {
        unz64_s* s;
        int      err;

        if (file == NULL or file_pos == NULL)
            return UNZ_PARAMERROR;
        s = (unz64_s*)file;

        /* jump to the right spot */
        s->pos_in_central_dir = file_pos->pos_in_zip_directory;
        s->num_file           = file_pos->num_of_file;

        /* set the current file */
        err = unz64local_GetCurrentFileInfoInternal_ (file, &s->cur_file_info,
                                                      &s->cur_file_info_internal,
                                                      NULL, 0, NULL, 0, NULL, 0);
        /* return results */
        s->current_file_ok = (err == UNZ_OK);
        return err;
    }

    int unzGoToFilePos (
        unzFile       file,
        unz_file_pos* file_pos)
    {
        unz64_file_pos file_pos64;
        if (file_pos == NULL)
            return UNZ_PARAMERROR;

        file_pos64.pos_in_zip_directory = file_pos->pos_in_zip_directory;
        file_pos64.num_of_file          = file_pos->num_of_file;
        return unzGoToFilePos64 (file, &file_pos64);
    }

    /*
    // Unzip Helper Functions - should be here?
    ///////////////////////////////////////////
    */

    /*
      Read the local header of the current zipfile
      Check the coherency of the local header and info in the end of central
            directory about this file
      store in *piSizeVar the size of extra info in local header
            (filename and size of extra field data)
    */
    int unz64local_CheckCurrentFileCoherencyHeader (unz64_s* s, uInt* piSizeVar, ZPOS64_T* poffset_local_extrafield, uInt* psize_local_extrafield)
    {
        uLong uMagic, uData, uFlags;
        uLong size_filename;
        uLong size_extra_field;
        int   err = UNZ_OK;

        *piSizeVar                = 0;
        *poffset_local_extrafield = 0;
        *psize_local_extrafield   = 0;

        if (ZSEEK64 (s->z_filefunc, s->filestream, s->cur_file_info_internal.offset_curfile + s->byte_before_the_zipfile,
                     ZLIB_FILEFUNC_SEEK_SET) != 0)
            return UNZ_ERRNO;

        if (err == UNZ_OK) {
            if (unz64local_getLong (&s->z_filefunc, s->filestream, &uMagic) != UNZ_OK)
                err = UNZ_ERRNO;
            else if (uMagic != 0x04034b50)
                err = UNZ_BADZIPFILE;
        }

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &uData) != UNZ_OK)
            err = UNZ_ERRNO;
        /*
            else if ((err==UNZ_OK) and (uData!=s->cur_file_info.wVersion))
                err=UNZ_BADZIPFILE;
        */
        if (unz64local_getShort (&s->z_filefunc, s->filestream, &uFlags) != UNZ_OK)
            err = UNZ_ERRNO;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &uData) != UNZ_OK)
            err = UNZ_ERRNO;
        else if ((err == UNZ_OK) and (uData != s->cur_file_info.compression_method))
            err = UNZ_BADZIPFILE;

        if ((err == UNZ_OK) and (s->cur_file_info.compression_method != 0) &&
            /* #ifdef HAVE_BZIP2 */
            (s->cur_file_info.compression_method != Z_BZIP2ED) &&
            /* #endif */
            (s->cur_file_info.compression_method != Z_DEFLATED))
            err = UNZ_BADZIPFILE;

        if (unz64local_getLong (&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* date/time */
            err = UNZ_ERRNO;

        if (unz64local_getLong (&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* crc */
            err = UNZ_ERRNO;
        else if ((err == UNZ_OK) and (uData != s->cur_file_info.crc) and ((uFlags & 8) == 0))
            err = UNZ_BADZIPFILE;

        if (unz64local_getLong (&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* size compr */
            err = UNZ_ERRNO;
        else if (uData != 0xFFFFFFFF and (err == UNZ_OK) and (uData != s->cur_file_info.compressed_size) and ((uFlags & 8) == 0))
            err = UNZ_BADZIPFILE;

        if (unz64local_getLong (&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* size uncompr */
            err = UNZ_ERRNO;
        else if (uData != 0xFFFFFFFF and (err == UNZ_OK) and (uData != s->cur_file_info.uncompressed_size) and ((uFlags & 8) == 0))
            err = UNZ_BADZIPFILE;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &size_filename) != UNZ_OK)
            err = UNZ_ERRNO;
        else if ((err == UNZ_OK) and (size_filename != s->cur_file_info.size_filename))
            err = UNZ_BADZIPFILE;

        *piSizeVar += (uInt)size_filename;

        if (unz64local_getShort (&s->z_filefunc, s->filestream, &size_extra_field) != UNZ_OK)
            err = UNZ_ERRNO;
        *poffset_local_extrafield = s->cur_file_info_internal.offset_curfile +
                                    SIZEZIPLOCALHEADER + size_filename;
        *psize_local_extrafield = (uInt)size_extra_field;

        *piSizeVar += (uInt)size_extra_field;

        return err;
    }

    /*
      Open for reading data the current file in the zipfile.
      If there is no error and the file is opened, the return value is UNZ_OK.
    */
    int unzOpenCurrentFile3_ (unzFile file, int* method, int* level, int raw, const char* password)
    {
        int                        err = UNZ_OK;
        uInt                       iSizeVar;
        unz64_s*                   s;
        file_in_zip64_read_info_s* pfile_in_zip_read_info;
        ZPOS64_T                   offset_local_extrafield; /* offset of the local extra field */
        uInt                       size_local_extrafield;   /* size of the local extra field */
#ifndef NOUNCRYPT
        char source[12];
#else
        if (password != NULL)
            return UNZ_PARAMERROR;
#endif

        if (file == NULL)
            return UNZ_PARAMERROR;
        s = (unz64_s*)file;
        if (!s->current_file_ok)
            return UNZ_PARAMERROR;

        if (s->pfile_in_zip_read != NULL)
            unzCloseCurrentFile_ (file);

        if (unz64local_CheckCurrentFileCoherencyHeader (s, &iSizeVar, &offset_local_extrafield, &size_local_extrafield) != UNZ_OK)
            return UNZ_BADZIPFILE;

        pfile_in_zip_read_info = (file_in_zip64_read_info_s*)ALLOC (sizeof (file_in_zip64_read_info_s));
        if (pfile_in_zip_read_info == NULL)
            return UNZ_INTERNALERROR;

        pfile_in_zip_read_info->read_buffer             = (char*)ALLOC (UNZ_BUFSIZE);
        pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
        pfile_in_zip_read_info->size_local_extrafield   = size_local_extrafield;
        pfile_in_zip_read_info->pos_local_extrafield    = 0;
        pfile_in_zip_read_info->raw                     = raw;

        if (pfile_in_zip_read_info->read_buffer == NULL) {
            TRYFREE (pfile_in_zip_read_info);
            return UNZ_INTERNALERROR;
        }

        pfile_in_zip_read_info->stream_initialised = 0;

        if (method != NULL)
            *method = (int)s->cur_file_info.compression_method;

        if (level != NULL) {
            *level = 6;
            switch (s->cur_file_info.flag & 0x06) {
                case 6:
                    *level = 1;
                    break;
                case 4:
                    *level = 2;
                    break;
                case 2:
                    *level = 9;
                    break;
            }
        }

        if ((s->cur_file_info.compression_method != 0) &&
            /* #ifdef HAVE_BZIP2 */
            (s->cur_file_info.compression_method != Z_BZIP2ED) &&
            /* #endif */
            (s->cur_file_info.compression_method != Z_DEFLATED))

            err = UNZ_BADZIPFILE;

        pfile_in_zip_read_info->crc32_wait              = s->cur_file_info.crc;
        pfile_in_zip_read_info->crc32                   = 0;
        pfile_in_zip_read_info->total_out_64            = 0;
        pfile_in_zip_read_info->compression_method      = s->cur_file_info.compression_method;
        pfile_in_zip_read_info->filestream              = s->filestream;
        pfile_in_zip_read_info->z_filefunc              = s->z_filefunc;
        pfile_in_zip_read_info->byte_before_the_zipfile = s->byte_before_the_zipfile;

        pfile_in_zip_read_info->stream.total_out = 0;

        if ((s->cur_file_info.compression_method == Z_BZIP2ED) and (!raw)) {
#ifdef HAVE_BZIP2
            pfile_in_zip_read_info->bstream.bzalloc = (void* (*)(void*, int, int))0;
            pfile_in_zip_read_info->bstream.bzfree  = (free_func)0;
            pfile_in_zip_read_info->bstream.opaque  = (voidpf)0;
            pfile_in_zip_read_info->bstream.state   = (voidpf)0;

            pfile_in_zip_read_info->stream.zalloc   = (alloc_func)0;
            pfile_in_zip_read_info->stream.zfree    = (free_func)0;
            pfile_in_zip_read_info->stream.opaque   = (voidpf)0;
            pfile_in_zip_read_info->stream.next_in  = (voidpf)0;
            pfile_in_zip_read_info->stream.avail_in = 0;

            err = BZ2_bzDecompressInit (&pfile_in_zip_read_info->bstream, 0, 0);
            if (err == Z_OK)
                pfile_in_zip_read_info->stream_initialised = Z_BZIP2ED;
            else {
                TRYFREE (pfile_in_zip_read_info);
                return err;
            }
#else
            pfile_in_zip_read_info->raw = 1;
#endif
        }
        else if ((s->cur_file_info.compression_method == Z_DEFLATED) and (!raw)) {
            pfile_in_zip_read_info->stream.zalloc   = (alloc_func)0;
            pfile_in_zip_read_info->stream.zfree    = (free_func)0;
            pfile_in_zip_read_info->stream.opaque   = (voidpf)0;
            pfile_in_zip_read_info->stream.next_in  = 0;
            pfile_in_zip_read_info->stream.avail_in = 0;

            err = inflateInit2 (&pfile_in_zip_read_info->stream, -MAX_WBITS);
            if (err == Z_OK)
                pfile_in_zip_read_info->stream_initialised = Z_DEFLATED;
            else {
                TRYFREE (pfile_in_zip_read_info);
                return err;
            }
            /* windowBits is passed < 0 to tell that there is no zlib header.
             * Note that in this case inflate *requires* an extra "dummy" byte
             * after the compressed stream in order to complete decompression and
             * return Z_STREAM_END.
             * In unzip, i don't wait absolutely Z_STREAM_END because I known the
             * size of both compressed and uncompressed data
             */
        }
        pfile_in_zip_read_info->rest_read_compressed =
            s->cur_file_info.compressed_size;
        pfile_in_zip_read_info->rest_read_uncompressed =
            s->cur_file_info.uncompressed_size;

        pfile_in_zip_read_info->pos_in_zipfile =
            s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER +
            iSizeVar;

        pfile_in_zip_read_info->stream.avail_in = (uInt)0;

        s->pfile_in_zip_read = pfile_in_zip_read_info;
        s->encrypted         = 0;

#ifndef NOUNCRYPT
        if (password != NULL) {
            int i;
            s->pcrc_32_tab = get_crc_table ();
            init_keys (password, s->keys, s->pcrc_32_tab);
            if (ZSEEK64 (s->z_filefunc, s->filestream,
                         s->pfile_in_zip_read->pos_in_zipfile +
                             s->pfile_in_zip_read->byte_before_the_zipfile,
                         SEEK_SET) != 0)
                return UNZ_INTERNALERROR;
            if (ZREAD64 (s->z_filefunc, s->filestream, source, 12) < 12)
                return UNZ_INTERNALERROR;

            for (i = 0; i < 12; i++)
                zdecode (s->keys, s->pcrc_32_tab, source[i]);

            s->pfile_in_zip_read->pos_in_zipfile += 12;
            s->encrypted = 1;
        }
#endif

        return UNZ_OK;
    }

    int unzOpenCurrentFile (unzFile file)
    {
        return unzOpenCurrentFile3_ (file, NULL, NULL, 0, NULL);
    }

    int unzOpenCurrentFilePassword (unzFile file, const char* password)
    {
        return unzOpenCurrentFile3_ (file, NULL, NULL, 0, password);
    }

    int unzOpenCurrentFile2 (unzFile file, int* method, int* level, int raw)
    {
        return unzOpenCurrentFile3_ (file, method, level, raw, NULL);
    }

    /** Addition for GDAL : START */

    ZPOS64_T unzGetCurrentFileZStreamPos64 (unzFile file)
    {
        unz64_s*                   s;
        file_in_zip64_read_info_s* pfile_in_zip_read_info;
        s = (unz64_s*)file;
        if (file == NULL)
            return 0; //UNZ_PARAMERROR;
        pfile_in_zip_read_info = s->pfile_in_zip_read;
        if (pfile_in_zip_read_info == NULL)
            return 0; //UNZ_PARAMERROR;
        return pfile_in_zip_read_info->pos_in_zipfile +
               pfile_in_zip_read_info->byte_before_the_zipfile;
    }

    /** Addition for GDAL : END */

    /*
      Read bytes from the current file.
      buf contain buffer where data must be copied
      len the size of buf.

      return the number of byte copied if somes bytes are copied
      return 0 if the end of file was reached
      return <0 with error code if there is an error
        (UNZ_ERRNO for IO error, or zLib error for uncompress error)
    */
    int unzReadCurrentFile_ (unzFile file, voidp buf, unsigned len)
    {
        int                        err   = UNZ_OK;
        uInt                       iRead = 0;
        unz64_s*                   s;
        file_in_zip64_read_info_s* pfile_in_zip_read_info;
        if (file == NULL)
            return UNZ_PARAMERROR;
        s                      = (unz64_s*)file;
        pfile_in_zip_read_info = s->pfile_in_zip_read;

        if (pfile_in_zip_read_info == NULL)
            return UNZ_PARAMERROR;

        if (pfile_in_zip_read_info->read_buffer == NULL)
            return UNZ_END_OF_LIST_OF_FILE;
        if (len == 0)
            return 0;

        pfile_in_zip_read_info->stream.next_out = (Bytef*)buf;

        pfile_in_zip_read_info->stream.avail_out = (uInt)len;

        if ((len > pfile_in_zip_read_info->rest_read_uncompressed) &&
            (!(pfile_in_zip_read_info->raw)))
            pfile_in_zip_read_info->stream.avail_out =
                (uInt)pfile_in_zip_read_info->rest_read_uncompressed;

        if ((len > pfile_in_zip_read_info->rest_read_compressed +
                       pfile_in_zip_read_info->stream.avail_in) &&
            (pfile_in_zip_read_info->raw))
            pfile_in_zip_read_info->stream.avail_out =
                (uInt)pfile_in_zip_read_info->rest_read_compressed +
                pfile_in_zip_read_info->stream.avail_in;

        while (pfile_in_zip_read_info->stream.avail_out > 0) {
            if ((pfile_in_zip_read_info->stream.avail_in == 0) &&
                (pfile_in_zip_read_info->rest_read_compressed > 0)) {
                uInt uReadThis = UNZ_BUFSIZE;
                if (pfile_in_zip_read_info->rest_read_compressed < uReadThis)
                    uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;
                if (uReadThis == 0)
                    return UNZ_EOF;
                if (ZSEEK64 (pfile_in_zip_read_info->z_filefunc,
                             pfile_in_zip_read_info->filestream,
                             pfile_in_zip_read_info->pos_in_zipfile +
                                 pfile_in_zip_read_info->byte_before_the_zipfile,
                             ZLIB_FILEFUNC_SEEK_SET) != 0)
                    return UNZ_ERRNO;
                if (ZREAD64 (pfile_in_zip_read_info->z_filefunc,
                             pfile_in_zip_read_info->filestream,
                             pfile_in_zip_read_info->read_buffer,
                             uReadThis) != uReadThis)
                    return UNZ_ERRNO;

#ifndef NOUNCRYPT
                if (s->encrypted) {
                    uInt i;
                    for (i = 0; i < uReadThis; i++)
                        pfile_in_zip_read_info->read_buffer[i] =
                            zdecode (s->keys, s->pcrc_32_tab,
                                     pfile_in_zip_read_info->read_buffer[i]);
                }
#endif

                pfile_in_zip_read_info->pos_in_zipfile += uReadThis;

                pfile_in_zip_read_info->rest_read_compressed -= uReadThis;

                pfile_in_zip_read_info->stream.next_in =
                    (Bytef*)pfile_in_zip_read_info->read_buffer;
                pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
            }

            if ((pfile_in_zip_read_info->compression_method == 0) || (pfile_in_zip_read_info->raw)) {
                uInt uDoCopy, i;

                if ((pfile_in_zip_read_info->stream.avail_in == 0) &&
                    (pfile_in_zip_read_info->rest_read_compressed == 0))
                    return (iRead == 0) ? UNZ_EOF : iRead;

                if (pfile_in_zip_read_info->stream.avail_out <
                    pfile_in_zip_read_info->stream.avail_in)
                    uDoCopy = pfile_in_zip_read_info->stream.avail_out;
                else
                    uDoCopy = pfile_in_zip_read_info->stream.avail_in;

                for (i = 0; i < uDoCopy; i++)
                    *(pfile_in_zip_read_info->stream.next_out + i) =
                        *(pfile_in_zip_read_info->stream.next_in + i);

                pfile_in_zip_read_info->total_out_64 = pfile_in_zip_read_info->total_out_64 + uDoCopy;

                pfile_in_zip_read_info->crc32 = crc32 (pfile_in_zip_read_info->crc32,
                                                       pfile_in_zip_read_info->stream.next_out,
                                                       uDoCopy);
                pfile_in_zip_read_info->rest_read_uncompressed -= uDoCopy;
                pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
                pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
                pfile_in_zip_read_info->stream.next_out += uDoCopy;
                pfile_in_zip_read_info->stream.next_in += uDoCopy;
                pfile_in_zip_read_info->stream.total_out += uDoCopy;
                iRead += uDoCopy;
            }
            else if (pfile_in_zip_read_info->compression_method == Z_BZIP2ED) {
#ifdef HAVE_BZIP2
                uLong        uTotalOutBefore, uTotalOutAfter;
                const Bytef* bufBefore;
                uLong        uOutThis;

                pfile_in_zip_read_info->bstream.next_in        = (char*)pfile_in_zip_read_info->stream.next_in;
                pfile_in_zip_read_info->bstream.avail_in       = pfile_in_zip_read_info->stream.avail_in;
                pfile_in_zip_read_info->bstream.total_in_lo32  = pfile_in_zip_read_info->stream.total_in;
                pfile_in_zip_read_info->bstream.total_in_hi32  = 0;
                pfile_in_zip_read_info->bstream.next_out       = (char*)pfile_in_zip_read_info->stream.next_out;
                pfile_in_zip_read_info->bstream.avail_out      = pfile_in_zip_read_info->stream.avail_out;
                pfile_in_zip_read_info->bstream.total_out_lo32 = pfile_in_zip_read_info->stream.total_out;
                pfile_in_zip_read_info->bstream.total_out_hi32 = 0;

                uTotalOutBefore = pfile_in_zip_read_info->bstream.total_out_lo32;
                bufBefore       = (const Bytef*)pfile_in_zip_read_info->bstream.next_out;

                err = BZ2_bzDecompress (&pfile_in_zip_read_info->bstream);

                uTotalOutAfter = pfile_in_zip_read_info->bstream.total_out_lo32;
                uOutThis       = uTotalOutAfter - uTotalOutBefore;

                pfile_in_zip_read_info->total_out_64 = pfile_in_zip_read_info->total_out_64 + uOutThis;

                pfile_in_zip_read_info->crc32 = crc32 (pfile_in_zip_read_info->crc32, bufBefore, (uInt) (uOutThis));
                pfile_in_zip_read_info->rest_read_uncompressed -= uOutThis;
                iRead += (uInt) (uTotalOutAfter - uTotalOutBefore);

                pfile_in_zip_read_info->stream.next_in   = (Bytef*)pfile_in_zip_read_info->bstream.next_in;
                pfile_in_zip_read_info->stream.avail_in  = pfile_in_zip_read_info->bstream.avail_in;
                pfile_in_zip_read_info->stream.total_in  = pfile_in_zip_read_info->bstream.total_in_lo32;
                pfile_in_zip_read_info->stream.next_out  = (Bytef*)pfile_in_zip_read_info->bstream.next_out;
                pfile_in_zip_read_info->stream.avail_out = pfile_in_zip_read_info->bstream.avail_out;
                pfile_in_zip_read_info->stream.total_out = pfile_in_zip_read_info->bstream.total_out_lo32;

                if (err == BZ_STREAM_END)
                    return (iRead == 0) ? UNZ_EOF : iRead;
                if (err != BZ_OK)
                    break;
#endif
            } // end Z_BZIP2ED
            else {
                ZPOS64_T     uTotalOutBefore, uTotalOutAfter;
                const Bytef* bufBefore;
                ZPOS64_T     uOutThis;
                int          flush = Z_SYNC_FLUSH;

                uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
                bufBefore       = pfile_in_zip_read_info->stream.next_out;

                /*
                if ((pfile_in_zip_read_info->rest_read_uncompressed ==
                         pfile_in_zip_read_info->stream.avail_out) &&
                    (pfile_in_zip_read_info->rest_read_compressed == 0))
                    flush = Z_FINISH;
                */
                err = inflate (&pfile_in_zip_read_info->stream, flush);

                if ((err >= 0) and (pfile_in_zip_read_info->stream.msg != NULL))
                    err = Z_DATA_ERROR;

                uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
                uOutThis       = uTotalOutAfter - uTotalOutBefore;

                pfile_in_zip_read_info->total_out_64 = pfile_in_zip_read_info->total_out_64 + uOutThis;

                pfile_in_zip_read_info->crc32 =
                    crc32 (pfile_in_zip_read_info->crc32, bufBefore,
                           (uInt) (uOutThis));

                pfile_in_zip_read_info->rest_read_uncompressed -=
                    uOutThis;

                iRead += (uInt) (uTotalOutAfter - uTotalOutBefore);

                if (err == Z_STREAM_END)
                    return (iRead == 0) ? UNZ_EOF : iRead;
                if (err != Z_OK)
                    break;
            }
        }

        if (err == Z_OK)
            return iRead;
        return err;
    }

    /*
      Give the current position in uncompressed data
    */
    z_off_t unztell (unzFile file)
    {
        unz64_s*                   s;
        file_in_zip64_read_info_s* pfile_in_zip_read_info;
        if (file == NULL)
            return UNZ_PARAMERROR;
        s                      = (unz64_s*)file;
        pfile_in_zip_read_info = s->pfile_in_zip_read;

        if (pfile_in_zip_read_info == NULL)
            return UNZ_PARAMERROR;

        return (z_off_t)pfile_in_zip_read_info->stream.total_out;
    }

    ZPOS64_T unztell64 (unzFile file)
    {

        unz64_s*                   s;
        file_in_zip64_read_info_s* pfile_in_zip_read_info;
        if (file == NULL)
            return (ZPOS64_T)-1;
        s                      = (unz64_s*)file;
        pfile_in_zip_read_info = s->pfile_in_zip_read;

        if (pfile_in_zip_read_info == NULL)
            return (ZPOS64_T)-1;

        return pfile_in_zip_read_info->total_out_64;
    }

    /*
      return 1 if the end of file was reached, 0 elsewhere
    */
    int unzeof (unzFile file)
    {
        unz64_s*                   s;
        file_in_zip64_read_info_s* pfile_in_zip_read_info;
        if (file == NULL)
            return UNZ_PARAMERROR;
        s                      = (unz64_s*)file;
        pfile_in_zip_read_info = s->pfile_in_zip_read;

        if (pfile_in_zip_read_info == NULL)
            return UNZ_PARAMERROR;

        if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
            return 1;
        else
            return 0;
    }

    /*
    Read extra field from the current file (opened by unzOpenCurrentFile)
    This is the local-header version of the extra field (sometimes, there is
    more info in the local-header version than in the central-header)

      if buf==NULL, it return the size of the local extra field that can be read

      if buf!=NULL, len is the size of the buffer, the extra header is copied in
        buf.
      the return value is the number of bytes copied in buf, or (if <0)
        the error code
    */
    int unzGetLocalExtrafield (unzFile file, voidp buf, unsigned len)
    {
        unz64_s*                   s;
        file_in_zip64_read_info_s* pfile_in_zip_read_info;
        uInt                       read_now;
        ZPOS64_T                   size_to_read;

        if (file == NULL)
            return UNZ_PARAMERROR;
        s                      = (unz64_s*)file;
        pfile_in_zip_read_info = s->pfile_in_zip_read;

        if (pfile_in_zip_read_info == NULL)
            return UNZ_PARAMERROR;

        size_to_read = (pfile_in_zip_read_info->size_local_extrafield -
                        pfile_in_zip_read_info->pos_local_extrafield);

        if (buf == NULL)
            return (int)size_to_read;

        if (len > size_to_read)
            read_now = (uInt)size_to_read;
        else
            read_now = (uInt)len;

        if (read_now == 0)
            return 0;

        if (ZSEEK64 (pfile_in_zip_read_info->z_filefunc,
                     pfile_in_zip_read_info->filestream,
                     pfile_in_zip_read_info->offset_local_extrafield +
                         pfile_in_zip_read_info->pos_local_extrafield,
                     ZLIB_FILEFUNC_SEEK_SET) != 0)
            return UNZ_ERRNO;

        if (ZREAD64 (pfile_in_zip_read_info->z_filefunc,
                     pfile_in_zip_read_info->filestream,
                     buf, read_now) != read_now)
            return UNZ_ERRNO;

        return (int)read_now;
    }

    /*
      Close the file in zip opened with unzOpenCurrentFile
      Return UNZ_CRCERROR if all the file was read but the CRC is not good
    */
    int unzCloseCurrentFile_ (unzFile file)
    {
        int err = UNZ_OK;

        unz64_s*                   s;
        file_in_zip64_read_info_s* pfile_in_zip_read_info;
        if (file == NULL)
            return UNZ_PARAMERROR;
        s                      = (unz64_s*)file;
        pfile_in_zip_read_info = s->pfile_in_zip_read;

        if (pfile_in_zip_read_info == NULL)
            return UNZ_PARAMERROR;

        if ((pfile_in_zip_read_info->rest_read_uncompressed == 0) &&
            (!pfile_in_zip_read_info->raw)) {
            if (pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait)
                err = UNZ_CRCERROR;
        }

        TRYFREE (pfile_in_zip_read_info->read_buffer);
        pfile_in_zip_read_info->read_buffer = NULL;
        if (pfile_in_zip_read_info->stream_initialised == Z_DEFLATED)
            inflateEnd (&pfile_in_zip_read_info->stream);
#ifdef HAVE_BZIP2
        else if (pfile_in_zip_read_info->stream_initialised == Z_BZIP2ED)
            BZ2_bzDecompressEnd (&pfile_in_zip_read_info->bstream);
#endif

        pfile_in_zip_read_info->stream_initialised = 0;
        TRYFREE (pfile_in_zip_read_info);

        s->pfile_in_zip_read = NULL;

        return err;
    }

    /*
      Get the global comment string of the ZipFile, in the szComment buffer.
      uSizeBuf is the size of the szComment buffer.
      return the number of byte copied or an error code <0
    */
    int unzGetGlobalComment (unzFile file, char* szComment, uLong uSizeBuf)
    {
        unz64_s* s;
        uLong    uReadThis;
        if (file == NULL)
            return (int)UNZ_PARAMERROR;
        s = (unz64_s*)file;

        uReadThis = uSizeBuf;
        if (uReadThis > s->gi.size_comment)
            uReadThis = s->gi.size_comment;

        if (ZSEEK64 (s->z_filefunc, s->filestream, s->central_pos + 22, ZLIB_FILEFUNC_SEEK_SET) != 0)
            return UNZ_ERRNO;

        if (uReadThis > 0) {
            *szComment = '\0';
            if (ZREAD64 (s->z_filefunc, s->filestream, szComment, uReadThis) != uReadThis)
                return UNZ_ERRNO;
        }

        if ((szComment != NULL) and (uSizeBuf > s->gi.size_comment))
            *(szComment + s->gi.size_comment) = '\0';
        return (int)uReadThis;
    }

    /* Additions by RX '2004 */
    ZPOS64_T unzGetOffset64 (unzFile file)
    {
        unz64_s* s;

        if (file == NULL)
            return 0; //UNZ_PARAMERROR;
        s = (unz64_s*)file;
        if (!s->current_file_ok)
            return 0;
        if (s->gi.number_entry != 0 and s->gi.number_entry != 0xffff)
            if (s->num_file == s->gi.number_entry)
                return 0;
        return s->pos_in_central_dir;
    }

    uLong unzGetOffset (unzFile file)
    {
        ZPOS64_T offset64;

        if (file == NULL)
            return 0; //UNZ_PARAMERROR;
        offset64 = unzGetOffset64 (file);
        return (uLong)offset64;
    }

    int unzSetOffset64 (unzFile file, ZPOS64_T pos)
    {
        unz64_s* s;
        int      err;

        if (file == NULL)
            return UNZ_PARAMERROR;
        s = (unz64_s*)file;

        s->pos_in_central_dir = pos;
        s->num_file           = s->gi.number_entry; /* hack */
        err                   = unz64local_GetCurrentFileInfoInternal_ (file, &s->cur_file_info,
                                                      &s->cur_file_info_internal,
                                                      NULL, 0, NULL, 0, NULL, 0);
        s->current_file_ok    = (err == UNZ_OK);
        return err;
    }

    int unzSetOffset (unzFile file, uLong pos)
    {
        return unzSetOffset64 (file, pos);
    }
}
///////////////////////////////// END OF unzip.c ////////////////////
#endif

#if qHasFeature_ZLib
class Zip::Reader::Rep_ : public Reader::_IRep {
private:
    struct MyISeekInStream : zlib_filefunc64_def {
        Streams::InputStream<Memory::Byte>::Ptr fInStream_;
#if qDebug
        bool fOpened_{false};
#endif
        MyISeekInStream (const Streams::InputStream<Memory::Byte>::Ptr& in)
            : fInStream_ (in)
        {
            this->zopen64_file = [](voidpf opaque, const void* /*filename*/, int /*mode*/) -> voidpf {
                MyISeekInStream* myThis = reinterpret_cast<MyISeekInStream*> (opaque);
#if qDebug
                Assert (not myThis->fOpened_);
                myThis->fOpened_ = true;
#endif
                return myThis;
            };
            this->zread_file = [](voidpf opaque, [[maybe_unused]] voidpf stream, void* buf, uLong size) -> uLong {
                Lambda_Arg_Unused_BWA (stream);
                Require (opaque == stream); // our use is one stream per zlib_filefunc64_def object
                MyISeekInStream* myThis = reinterpret_cast<MyISeekInStream*> (opaque);
                Assert (myThis->fOpened_);
                size_t sz = myThis->fInStream_.Read (reinterpret_cast<byte*> (buf), reinterpret_cast<byte*> (buf) + size);
                Assert (sz <= size);
                return static_cast<uLong> (sz);
            };
            this->zwrite_file = [](voidpf /*opaque*/, voidpf /*stream*/, const void* /*buf*/, uLong /*size*/) -> uLong {
                RequireNotReached (); // read only zip
                return static_cast<uLong> (UNZ_PARAMERROR);
            };
            this->ztell64_file = [](voidpf opaque, [[maybe_unused]] voidpf stream) -> ZPOS64_T {
                Lambda_Arg_Unused_BWA (stream);
                Require (opaque == stream); // our use is one stream per zlib_filefunc64_def object
                MyISeekInStream* myThis = reinterpret_cast<MyISeekInStream*> (opaque);
                Assert (myThis->fOpened_);
                return myThis->fInStream_.GetOffset ();
            };
            this->zseek64_file = [](voidpf opaque, [[maybe_unused]] voidpf stream, ZPOS64_T offset, int origin) -> long {
                Lambda_Arg_Unused_BWA (stream);
                Require (opaque == stream); // our use is one stream per zlib_filefunc64_def object
                MyISeekInStream* myThis = reinterpret_cast<MyISeekInStream*> (opaque);
                Assert (myThis->fOpened_);
                switch (origin) {
                    case ZLIB_FILEFUNC_SEEK_SET:
                        myThis->fInStream_.Seek (offset);
                        break;
                    case ZLIB_FILEFUNC_SEEK_CUR:
                        myThis->fInStream_.Seek (Streams::Whence::eFromCurrent, offset);
                        break;
                    case ZLIB_FILEFUNC_SEEK_END:
                        myThis->fInStream_.Seek (Streams::Whence::eFromEnd, offset);
                        break;
                    default:
                        AssertNotReached ();
                        return UNZ_PARAMERROR;
                }
                return UNZ_OK;
            };
            this->zclose_file = []([[maybe_unused]] voidpf opaque, [[maybe_unused]] voidpf stream) -> int {
                Lambda_Arg_Unused_BWA (opaque);
                Lambda_Arg_Unused_BWA (stream);
#if qDebug
                Require (opaque == stream); // our use is one stream per zlib_filefunc64_def object
                MyISeekInStream* myThis = reinterpret_cast<MyISeekInStream*> (opaque);
                Assert (myThis->fOpened_);
                myThis->fOpened_ = false;
#endif
                return UNZ_OK;
            };
            this->zerror_file = [](voidpf opaque, [[maybe_unused]] voidpf stream) -> int {
                Lambda_Arg_Unused_BWA (stream);
                Require (opaque == stream); // our use is one stream per zlib_filefunc64_def object
                [[maybe_unused]] MyISeekInStream* myThis = reinterpret_cast<MyISeekInStream*> (opaque);
                Assert (myThis->fOpened_);
                return UNZ_OK; // @todo - see what this means?
            };
            this->opaque = this;
        }
        ~MyISeekInStream ()
        {
            Assert (not fOpened_);
        }
    };
    MyISeekInStream fInSeekStream_;
    unzFile         fZipFile_;

public:
    Rep_ (const Streams::InputStream<Memory::Byte>::Ptr& in)
        : fInSeekStream_ (in)
        , fZipFile_ (unzOpen2_64 ("", &fInSeekStream_))
    {
        if (fZipFile_ == nullptr) {
            Execution::Throw (Execution::StringException (L"failed to open zipfile"));
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
        if (err != UNZ_OK) {
            Execution::Throw (Execution::StringException (Characters::Format (L"error %d with zipfile in unzGetGlobalInfo", err)));
        }
        for (size_t i = 0; i < gi.number_entry; i++) {
            char            filename_inzip[10 * 1024];
            unz_file_info64 file_info;
            //uLong           ratio = 0;
            //const char* string_method;
            //char charCrypt = ' ';
            err = unzGetCurrentFileInfo64 (fZipFile_, &file_info, filename_inzip, sizeof (filename_inzip), NULL, 0, NULL, 0);
            if (err != UNZ_OK) {
                Execution::Throw (Execution::StringException (Characters::Format (L"error %d with zipfile in unzGetCurrentFileInfo64", err)));
                break;
            }
            if ((i + 1) < gi.number_entry) {
                err = unzGoToNextFile_ (fZipFile_);
                if (err != UNZ_OK) {
                    Execution::Throw (Execution::StringException (Characters::Format (L"error %d with zipfile in unzGoToNextFile", err)));
                    break;
                }
            }
            //tmphac
            if (filename_inzip[::strlen (filename_inzip) - 1] == '/') {
                continue; // only list files - not directories for now
            }
            result.Add (String::FromASCII (filename_inzip)); // not sure about codepage for conversion?
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
        for (i = 0; i < gi.number_entry; i++) {
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
                Memory::SmallStackBuffer<char16_t> fileName (nameLen);
                size_t z = ::SzArEx_GetFileNameUtf16 (&fDB_, i, reinterpret_cast<UInt16*> (&fileName[0]));
                result.Add (String (&fileName[0]));
            }
        }
#endif
        return result;
    }
    virtual Memory::BLOB GetData (const String& fileName) const override
    {
        if (unzLocateFile_ (fZipFile_, fileName.AsNarrowSDKString ().c_str (), 1) != UNZ_OK) {
            Execution::Throw (Execution::StringException (Characters::Format (L"File '%s' not found", fileName.c_str ())));
        }
        const char*                      password = nullptr;
        int                              err      = unzOpenCurrentFilePassword (fZipFile_, password);
        [[maybe_unused]] auto&&          cleanup  = Execution::Finally ([this]() { unzCloseCurrentFile_ (fZipFile_); });
        Streams::MemoryStream<Byte>::Ptr tmpBuf   = Streams::MemoryStream<Byte>::New ();
        do {
            Byte buf[10 * 1024];
            err = unzReadCurrentFile_ (fZipFile_, buf, static_cast<unsigned int> (NEltsOf (buf)));
            if (err < 0) {
                Execution::Throw (Execution::StringException (Characters::Format (L"File '%s' error %d extracting", fileName.c_str (), err)));
            }
            else if (err > 0) {
                Assert (static_cast<size_t> (err) <= NEltsOf (buf));
                tmpBuf.Write (buf, buf + err);
            }
        } while (err > 0);
        return tmpBuf.As<Memory::BLOB> ();
    }
};

Zip::Reader::Reader (const Streams::InputStream<Memory::Byte>::Ptr& in)
    : DataExchange::Archive::Reader (make_shared<Rep_> (in))
{
}
#endif
