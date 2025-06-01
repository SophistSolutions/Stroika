/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <limits>

#if qStroika_HasComponent_zlib
#ifdef HAVE_BZIP2
#include "bzlib.h"
#endif
#include <zlib.h>
#ifdef HAVE_MINIZIP64_CONF_H
#include "mz64conf.h"
#endif
#endif

#include "Stroika/Foundation/Debug/Assertions.h"

#if qStroika_HasComponent_zlib
namespace Stroika::Foundation::DataExchange::Archive::Zip::PrivateMinizip_ {

    /*
     *  Part of Stroika IMPLEMENTATION - but not included in any Stroika headers.
     * 
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

    using namespace std;

    constexpr uint32_t MAXU32 = numeric_limits<uint32_t>::max ();

///// START OF CRYPT.h
#define CRC32(c, b) ((*(pcrc_32_tab + (((int)(c) ^ (b)) & 0xff))) ^ ((c) >> 8))

    //  Return the next byte in the pseudo-random sequence
    inline int decrypt_byte (unsigned long* pkeys, [[maybe_unused]] const z_crc_t* pcrc_32_tab)
    {
        unsigned temp; /* POTENTIAL BUG:  temp*(temp^1) may overflow in an
                     * unpredictable manner on 16-bit systems; not a problem
                     * with any known compiler so far, though */
        temp = ((unsigned)(*(pkeys + 2)) & 0xffff) | 2;
        return (int)(((temp * (temp ^ 1)) >> 8) & 0xff);
    }

    //  Update the encryption keys with the next byte of plain text
    inline int update_keys (unsigned long* pkeys, const z_crc_t* pcrc_32_tab, int c)
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
    inline void init_keys (const char* passwd, unsigned long* pkeys, const z_crc_t* pcrc_32_tab)
    {
        *(pkeys + 0) = 305419896L;
        *(pkeys + 1) = 591751049L;
        *(pkeys + 2) = 878082192L;
        while (*passwd != '\0') {
            update_keys (pkeys, pcrc_32_tab, (int)*passwd);
            passwd++;
        }
    }

#define zdecode(pkeys, pcrc_32_tab, c) (update_keys (pkeys, pcrc_32_tab, c ^= decrypt_byte (pkeys, pcrc_32_tab)))

#define zencode(pkeys, pcrc_32_tab, c, t) (t = decrypt_byte (pkeys, pcrc_32_tab), update_keys (pkeys, pcrc_32_tab, c), t ^ (c))

#define RAND_HEAD_LEN 12

    /* "last resort" source for second part of crypt seed pattern */
    constexpr unsigned int ZCR_SEED2 = 3141592654UL; /* use PI as default pattern */

    inline int crypthead (const char*    passwd, /* password string */
                          unsigned char* buf,    /* where to write header */
                          int bufSize, unsigned long* pkeys, const z_crc_t* pcrc_32_tab, unsigned long crcForCrypting)
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
    ///////////// END OF CRYPT.h //////////////////

    ///// START OF ioapi.h////////////////////

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

#define ZREAD64(filefunc, filestream, buf, size)                                                                                           \
    ((*((filefunc).zfile_func64.zread_file)) ((filefunc).zfile_func64.opaque, filestream, buf, size))
#define ZWRITE64(filefunc, filestream, buf, size)                                                                                          \
    ((*((filefunc).zfile_func64.zwrite_file)) ((filefunc).zfile_func64.opaque, filestream, buf, size))
#define ZCLOSE64(filefunc, filestream) ((*((filefunc).zfile_func64.zclose_file)) ((filefunc).zfile_func64.opaque, filestream))
#define ZERROR64(filefunc, filestream) ((*((filefunc).zfile_func64.zerror_file)) ((filefunc).zfile_func64.opaque, filestream))

    voidpf   call_zopen64 (const zlib_filefunc64_32_def* pfilefunc, const void* filename, int mode);
    long     call_zseek64 (const zlib_filefunc64_32_def* pfilefunc, voidpf filestream, ZPOS64_T offset, int origin);
    ZPOS64_T call_ztell64 (const zlib_filefunc64_32_def* pfilefunc, voidpf filestream);

    void fill_zlib_filefunc64_32_def_from_filefunc32 (zlib_filefunc64_32_def* p_filefunc64_32, const zlib_filefunc_def* p_filefunc32);

#define ZOPEN64(filefunc, filename, mode) (call_zopen64 ((&(filefunc)), (filename), (mode)))
#define ZTELL64(filefunc, filestream) (call_ztell64 ((&(filefunc)), (filestream)))
#define ZSEEK64(filefunc, filestream, pos, mode) (call_zseek64 ((&(filefunc)), (filestream), (pos), (mode)))
    /////////////////////// END OF IO.h.////////////////////////////////////

    ///////////////////////////  START OF  UNZIP.h ////////////////////////////////////

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
    int unzGetCurrentFileInfo64 (unzFile file, unz_file_info64* pfile_info, char* szFileName, uLong fileNameBufferSize, void* extraField,
                                 uLong extraFieldBufferSize, char* szComment, uLong commentBufferSize);

    int unzGetCurrentFileInfo (unzFile file, unz_file_info* pfile_info, char* szFileName, uLong fileNameBufferSize, void* extraField,
                               uLong extraFieldBufferSize, char* szComment, uLong commentBufferSize);
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

     return the number of byte copied if some bytes are copied
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
    //// END OF unzip.h//////////////////////////////////////////////////////////////

    //// FROM zip.h
#define Z_BZIP2ED 12

#if defined(STRICTZIP) || defined(STRICTZIPUNZIP)
    /* like the STRICT of WIN32, we define a pointer that cannot be converted
    from (void*) without cast */
    typedef struct TagzipFile__ {
        int unused;
    } zipFile__;
    typedef zipFile__* zipFile;
#else
    typedef voidp zipFile;
#endif

#define ZIP_OK (0)
#define ZIP_EOF (0)
#define ZIP_ERRNO (Z_ERRNO)
#define ZIP_PARAMERROR (-102)
#define ZIP_BADZIPFILE (-103)
#define ZIP_INTERNALERROR (-104)

#ifndef DEF_MEM_LEVEL
#if MAX_MEM_LEVEL >= 8
#define DEF_MEM_LEVEL 8
#else
#define DEF_MEM_LEVEL MAX_MEM_LEVEL
#endif
#endif
    /* default memLevel */

    /* tm_zip contain date/time info */
    typedef struct tm_zip_s {
        int tm_sec;  /* seconds after the minute - [0,59] */
        int tm_min;  /* minutes after the hour - [0,59] */
        int tm_hour; /* hours since midnight - [0,23] */
        int tm_mday; /* day of the month - [1,31] */
        int tm_mon;  /* months since January - [0,11] */
        int tm_year; /* years - [1980..2044] */
    } tm_zip;

    typedef struct {
        tm_zip tmz_date;                  /* date in understandable format           */
        uLong  dosDate;                   /* if dos_date == 0, tmu_date is used      */
        /*    uLong       flag;        */ /* general purpose bit flag        2 bytes */

        uLong internal_fa; /* internal file attributes        2 bytes */
        uLong external_fa; /* external file attributes        4 bytes */
    } zip_fileinfo;

    typedef const char* zipcharpc;

#define APPEND_STATUS_CREATE (0)
#define APPEND_STATUS_CREATEAFTER (1)
#define APPEND_STATUS_ADDINZIP (2)

    extern zipFile ZEXPORT zipOpen (const char* pathname, int append);
    extern zipFile ZEXPORT zipOpen64 (const void* pathname, int append);
    /*
  Create a zipfile.
     pathname contain on Windows XP a filename like "c:\\zlib\\zlib113.zip" or on
       an Unix computer "zlib/zlib113.zip".
     if the file pathname exist and append==APPEND_STATUS_CREATEAFTER, the zip
       will be created at the end of the file.
         (useful if the file contain a self extractor code)
     if the file pathname exist and append==APPEND_STATUS_ADDINZIP, we will
       add files in existing zip (be sure you don't add file that doesn't exist)
     If the zipfile cannot be opened, the return value is NULL.
     Else, the return value is a zipFile Handle, usable with other function
       of this zip package.
*/

    /* Note : there is no delete function into a zipfile.
   If you want delete file into a zipfile, you must open a zipfile, and create another
   Of course, you can use RAW reading and writing to copy the file you did not want delete
*/

    extern zipFile ZEXPORT zipOpen2 (const char* pathname, int append, zipcharpc* globalcomment, zlib_filefunc_def* pzlib_filefunc_def);

    extern zipFile ZEXPORT zipOpen2_64 (const void* pathname, int append, zipcharpc* globalcomment, zlib_filefunc64_def* pzlib_filefunc_def);

    extern zipFile ZEXPORT zipOpen3 (const void* pathname, int append, zipcharpc* globalcomment, zlib_filefunc64_32_def* pzlib_filefunc64_32_def);

    extern int ZEXPORT zipOpenNewFileInZip (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                            uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                            const char* comment, int method, int level);

    extern int ZEXPORT zipOpenNewFileInZip64 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                              uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                              const char* comment, int method, int level, int zip64);

    /*
  Open a file in the ZIP for writing.
  filename : the filename in zip (if NULL, '-' without quote will be used
  *zipfi contain supplemental information
  if extrafield_local!=NULL and size_extrafield_local>0, extrafield_local
    contains the extrafield data for the local header
  if extrafield_global!=NULL and size_extrafield_global>0, extrafield_global
    contains the extrafield data for the global header
  if comment != NULL, comment contain the comment string
  method contain the compression method (0 for store, Z_DEFLATED for deflate)
  level contain the level of compression (can be Z_DEFAULT_COMPRESSION)
  zip64 is set to 1 if a zip64 extended information block should be added to the local file header.
                    this MUST be '1' if the uncompressed size is >= 0xffffffff.

*/

    extern int ZEXPORT zipOpenNewFileInZip2 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                             uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                             const char* comment, int method, int level, int raw);

    extern int ZEXPORT zipOpenNewFileInZip2_64 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                                uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                                const char* comment, int method, int level, int raw, int zip64);
    /*
  Same than zipOpenNewFileInZip, except if raw=1, we write raw file
 */

    extern int ZEXPORT zipOpenNewFileInZip3 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                             uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                             const char* comment, int method, int level, int raw, int windowBits, int memLevel,
                                             int strategy, const char* password, uLong crcForCrypting);

    extern int ZEXPORT zipOpenNewFileInZip3_64 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                                uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                                const char* comment, int method, int level, int raw, int windowBits, int memLevel,
                                                int strategy, const char* password, uLong crcForCrypting, int zip64);

    /*
  Same than zipOpenNewFileInZip2, except
    windowBits,memLevel,,strategy : see parameter strategy in deflateInit2
    password : crypting password (NULL for no crypting)
    crcForCrypting : crc of file to compress (needed for crypting)
 */

    extern int ZEXPORT zipOpenNewFileInZip4 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                             uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                             const char* comment, int method, int level, int raw, int windowBits, int memLevel,
                                             int strategy, const char* password, uLong crcForCrypting, uLong versionMadeBy, uLong flagBase);

    extern int ZEXPORT zipOpenNewFileInZip4_64 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                                uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                                const char* comment, int method, int level, int raw, int windowBits, int memLevel, int strategy,
                                                const char* password, uLong crcForCrypting, uLong versionMadeBy, uLong flagBase, int zip64);
    /*
  Same than zipOpenNewFileInZip4, except
    versionMadeBy : value for Version made by field
    flag : value for flag field (compression level info will be added)
 */

    extern int ZEXPORT zipWriteInFileInZip (zipFile file, const void* buf, unsigned len);
    /*
  Write data in the zipfile
*/

    extern int ZEXPORT zipCloseFileInZip (zipFile file);
    /*
  Close the current file in the zipfile
*/

    extern int ZEXPORT zipCloseFileInZipRaw (zipFile file, uLong uncompressed_size, uLong crc32);

    extern int ZEXPORT zipCloseFileInZipRaw64 (zipFile file, ZPOS64_T uncompressed_size, uLong crc32);

    /*
  Close the current file in the zipfile, for file opened with
    parameter raw=1 in zipOpenNewFileInZip2
  uncompressed_size and crc32 are value for the uncompressed size
*/

    extern int ZEXPORT zipClose (zipFile file, const char* global_comment);
    /*
  Close the zipfile
*/

    extern int ZEXPORT zipRemoveExtraInfoBlock (char* pData, int* dataLen, short sHeader);
    /*
  zipRemoveExtraInfoBlock -  Added by Mathias Svensson

  Remove extra information block from a extra information data for the local file header or central directory header

  It is needed to remove ZIP64 extra information blocks when before data is written if using RAW mode.

  0x0001 is the signature header for the ZIP64 extra information blocks

  usage.
                        Remove ZIP64 Extra information from a central director extra field data
              zipRemoveExtraInfoBlock(pCenDirExtraFieldData, &nCenDirExtraFieldDataLen, 0x0001);

                        Remove ZIP64 Extra information from a Local File Header extra field data
        zipRemoveExtraInfoBlock(pLocalHeaderExtraFieldData, &nLocalHeaderExtraFieldDataLen, 0x0001);
*/
    /// END OF ZIP.h

}
#endif
