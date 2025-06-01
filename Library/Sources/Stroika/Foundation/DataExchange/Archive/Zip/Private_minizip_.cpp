/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_zlib
#include <zlib.h>
#endif

#ifdef HAVE_BZIP2
#include "bzlib.h"
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "Private_minizip_.h"

using namespace Stroika::Foundation::DataExchange::Archive::Zip;
using namespace Stroika::Foundation::DataExchange::Archive::Zip::PrivateMinizip_;

///// START OF IOAPI.c //////////////////
/* ioapi.h -- IO base function header for compress/uncompress .zip
   part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications for Zip64 support
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

         For more info read MiniZip_info.txt

*/

voidpf PrivateMinizip_::call_zopen64 (const zlib_filefunc64_32_def* pfilefunc, const void* filename, int mode)
{
    if (pfilefunc->zfile_func64.zopen64_file != NULL)
        return (*(pfilefunc->zfile_func64.zopen64_file)) (pfilefunc->zfile_func64.opaque, filename, mode);
    else {
        return (*(pfilefunc->zopen32_file)) (pfilefunc->zfile_func64.opaque, (const char*)filename, mode);
    }
}
long PrivateMinizip_::call_zseek64 (const zlib_filefunc64_32_def* pfilefunc, voidpf filestream, ZPOS64_T offset, int origin)
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
ZPOS64_T PrivateMinizip_::call_ztell64 (const zlib_filefunc64_32_def* pfilefunc, voidpf filestream)
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
void PrivateMinizip_::fill_zlib_filefunc64_32_def_from_filefunc32 (zlib_filefunc64_32_def* p_filefunc64_32, const zlib_filefunc_def* p_filefunc32)
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
#define TRYFREE(p)                                                                                                                         \
    {                                                                                                                                      \
        if (p)                                                                                                                             \
            free (p);                                                                                                                      \
    }
#endif

#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)

    // const char unz_copyright[] = " unzip 1.01 Copyright 1998-2004 Gilles Vollant - http://www.winimage.com/zLibDll";

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

}
/*
       Compare two filename (fileName1,fileName2).
       If iCaseSenisivity = 1, comparision is case sensitivity (like strcmp)
       If iCaseSenisivity = 2, comparision is not case sensitivity (like strcmpi
                                                                    or strcasecmp)
       If iCaseSenisivity = 0, case sensitivity is defaut of your operating system
            (like 1 on Unix, 2 on Windows)

    */
int PrivateMinizip_::unzStringFileNameCompare (const char* fileName1, const char* fileName2, int iCaseSensitivity)
{
    if (iCaseSensitivity == 0)
        iCaseSensitivity = CASESENSITIVITYDEFAULTVALUE;

    if (iCaseSensitivity == 1)
        return strcmp (fileName1, fileName2);

    return STRCMPCASENOSENTIVEFUNCTION (fileName1, fileName2);
}

namespace {

    constexpr size_t BUFREADCOMMENT = 0x400;

    /*
      Locate the Central directory of a zipfile (at the end, just before
        the global comment)
    */
    ZPOS64_T unz64local_SearchCentralDir (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream)
    {
        ZPOS64_T uPosFound = 0;

        if (ZSEEK64 (*pzlib_filefunc_def, filestream, 0, ZLIB_FILEFUNC_SEEK_END) != 0)
            return 0;

        ZPOS64_T uSizeFile = ZTELL64 (*pzlib_filefunc_def, filestream);

        ZPOS64_T uMaxBack = 0xffff; /* maximum size of global comment */
        if (uMaxBack > uSizeFile)
            uMaxBack = uSizeFile;

        unsigned char* buf = (unsigned char*)ALLOC (BUFREADCOMMENT + 4);
        if (buf == NULL)
            return 0;

        ZPOS64_T uBackRead = 4;
        while (uBackRead < uMaxBack) {
            uLong    uReadSize;
            ZPOS64_T uReadPos;
            int      i;
            if (uBackRead + BUFREADCOMMENT > uMaxBack)
                uBackRead = uMaxBack;
            else
                uBackRead += BUFREADCOMMENT;
            uReadPos = uSizeFile - uBackRead;

            uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ? (BUFREADCOMMENT + 4) : (uLong)(uSizeFile - uReadPos);
            if (ZSEEK64 (*pzlib_filefunc_def, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0)
                break;

            if (ZREAD64 (*pzlib_filefunc_def, filestream, buf, uReadSize) != uReadSize)
                break;

            for (i = (int)uReadSize - 3; i-- > 0;)
                if (((*(buf + i)) == 0x50) and ((*(buf + i + 1)) == 0x4b) && ((*(buf + i + 2)) == 0x05) and ((*(buf + i + 3)) == 0x06)) {
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

            uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ? (BUFREADCOMMENT + 4) : (uLong)(uSizeFile - uReadPos);
            if (ZSEEK64 (*pzlib_filefunc_def, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0)
                break;

            if (ZREAD64 (*pzlib_filefunc_def, filestream, buf, uReadSize) != uReadSize)
                break;

            for (i = (int)uReadSize - 3; i-- > 0;)
                if (((*(buf + i)) == 0x50) and ((*(buf + i + 1)) == 0x4b) && ((*(buf + i + 2)) == 0x06) and ((*(buf + i + 3)) == 0x07)) {
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

        us.filestream = ZOPEN64 (us.z_filefunc, path, ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_EXISTING);
        if (us.filestream == NULL)
            return NULL;

        central_pos = unz64local_SearchCentralDir64 (&us.z_filefunc, us.filestream);
        if (central_pos) {
            uLong    uS;
            ZPOS64_T uL64;

            us.isZip64 = 1;

            if (ZSEEK64 (us.z_filefunc, us.filestream, central_pos, ZLIB_FILEFUNC_SEEK_SET) != 0)
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

            if (ZSEEK64 (us.z_filefunc, us.filestream, central_pos, ZLIB_FILEFUNC_SEEK_SET) != 0)
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

            if ((number_entry_CD != us.gi.number_entry) || (number_disk_with_CD != 0) || (number_disk != 0))
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

        if ((central_pos < us.offset_central_dir + us.size_central_dir) && (err == UNZ_OK))
            err = UNZ_BADZIPFILE;

        if (err != UNZ_OK) {
            ZCLOSE64 (us.z_filefunc, us.filestream);
            return NULL;
        }

        us.byte_before_the_zipfile = central_pos - (us.offset_central_dir + us.size_central_dir);
        us.central_pos             = central_pos;
        us.pfile_in_zip_read       = NULL;
        us.encrypted               = 0;

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
}

unzFile PrivateMinizip_::unzOpen2_64 (const void* path, zlib_filefunc64_def* pzlib_filefunc_def)
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

unzFile PrivateMinizip_::unzOpen (const char* path)
{
    return unzOpenInternal (path, NULL, 0);
}

unzFile PrivateMinizip_::unzOpen64 (const void* path)
{
    return unzOpenInternal (path, NULL, 1);
}

/*
      Close a ZipFile opened with unzOpen.
      If there is files inside the .Zip opened with unzOpenCurrentFile (see later),
        these files MUST be closed with unzCloseCurrentFile_ before call unzClose.
      return UNZ_OK if there is no problem. */
int PrivateMinizip_::unzClose (unzFile file)
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
int PrivateMinizip_::unzGetGlobalInfo64 (unzFile file, unz_global_info64* pglobal_info)
{
    unz64_s* s;
    if (file == NULL)
        return UNZ_PARAMERROR;
    s             = (unz64_s*)file;
    *pglobal_info = s->gi;
    return UNZ_OK;
}

int PrivateMinizip_::unzGetGlobalInfo (unzFile file, unz_global_info* pglobal_info32)
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

namespace {
    /*
       Translate date/time from Dos format to tm_unz (readable more easilty)
    */
    void unz64local_DosDateToTmuDate_ (ZPOS64_T ulDosDate, tm_unz* ptm)
    {
        ZPOS64_T uDate;
        uDate        = (ZPOS64_T)(ulDosDate >> 16);
        ptm->tm_mday = (uInt)(uDate & 0x1f);
        ptm->tm_mon  = (uInt)((((uDate) & 0x1E0) / 0x20) - 1);
        ptm->tm_year = (uInt)(((uDate & 0x0FE00) / 0x0200) + 1980);

        ptm->tm_hour = (uInt)((ulDosDate & 0xF800) / 0x800);
        ptm->tm_min  = (uInt)((ulDosDate & 0x7E0) / 0x20);
        ptm->tm_sec  = (uInt)(2 * (ulDosDate & 0x1f));
    }

    /*
      Get Info about the current file in the zipfile, with internal only info
    */
    int unz64local_GetCurrentFileInfoInternal_ (unzFile file, unz_file_info64* pfile_info, unz_file_info64_internal* pfile_info_internal,
                                                char* szFileName, uLong fileNameBufferSize, void* extraField, uLong extraFieldBufferSize,
                                                char* szComment, uLong commentBufferSize)
    {
        unz64_s*                 s;
        unz_file_info64          file_info;
        unz_file_info64_internal file_info_internal;
        int                      err = UNZ_OK;
        uLong                    uMagic;
        long                     lSeek = 0;

        if (file == NULL)
            return UNZ_PARAMERROR;
        s = (unz64_s*)file;
        if (ZSEEK64 (s->z_filefunc, s->filestream, s->pos_in_central_dir + s->byte_before_the_zipfile, ZLIB_FILEFUNC_SEEK_SET) != 0)
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

        {
            uLong uL;
            if (unz64local_getLong (&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
                err = UNZ_ERRNO;
            file_info.compressed_size = uL;
        }

        {
            uLong uL;
            if (unz64local_getLong (&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
                err = UNZ_ERRNO;
            file_info.uncompressed_size = uL;
        }

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
        {
            uLong uL;
            if (unz64local_getLong (&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
                err = UNZ_ERRNO;
            file_info_internal.offset_curfile = uL;
        }

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
}
/*
      Write info about the ZipFile in the *pglobal_info structure.
      No preparation of the structure is needed
      return UNZ_OK if there is no problem.
    */
int PrivateMinizip_::unzGetCurrentFileInfo64 (unzFile file, unz_file_info64* pfile_info, char* szFileName, uLong fileNameBufferSize,
                                              void* extraField, uLong extraFieldBufferSize, char* szComment, uLong commentBufferSize)
{
    return unz64local_GetCurrentFileInfoInternal_ (file, pfile_info, NULL, szFileName, fileNameBufferSize, extraField, extraFieldBufferSize,
                                                   szComment, commentBufferSize);
}

int PrivateMinizip_::unzGetCurrentFileInfo (unzFile file, unz_file_info* pfile_info, char* szFileName, uLong fileNameBufferSize,
                                            void* extraField, uLong extraFieldBufferSize, char* szComment, uLong commentBufferSize)
{
    int             err;
    unz_file_info64 file_info64;
    err = unz64local_GetCurrentFileInfoInternal_ (file, &file_info64, NULL, szFileName, fileNameBufferSize, extraField,
                                                  extraFieldBufferSize, szComment, commentBufferSize);
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
int PrivateMinizip_::unzGoToFirstFile (unzFile file)
{
    int      err = UNZ_OK;
    unz64_s* s;
    if (file == NULL)
        return UNZ_PARAMERROR;
    s                     = (unz64_s*)file;
    s->pos_in_central_dir = s->offset_central_dir;
    s->num_file           = 0;
    err = unz64local_GetCurrentFileInfoInternal_ (file, &s->cur_file_info, &s->cur_file_info_internal, NULL, 0, NULL, 0, NULL, 0);
    s->current_file_ok = (err == UNZ_OK);
    return err;
}

/*
      Set the current file of the zipfile to the next file.
      return UNZ_OK if there is no problem
      return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
    */
int PrivateMinizip_::unzGoToNextFile_ (unzFile file)
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

    s->pos_in_central_dir += SIZECENTRALDIRITEM + s->cur_file_info.size_filename + s->cur_file_info.size_file_extra + s->cur_file_info.size_file_comment;
    s->num_file++;
    err = unz64local_GetCurrentFileInfoInternal_ (file, &s->cur_file_info, &s->cur_file_info_internal, NULL, 0, NULL, 0, NULL, 0);
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
int PrivateMinizip_::unzLocateFile_ (unzFile file, const char* szFileName, int iCaseSensitivity)
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
        err = unzGetCurrentFileInfo64 (file, NULL, szCurrentFileName, sizeof (szCurrentFileName) - 1, NULL, 0, NULL, 0);
        if (err == UNZ_OK) {
            if (unzStringFileNameCompare (szCurrentFileName, szFileName, iCaseSensitivity) == 0)
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

namespace {
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
}
int PrivateMinizip_::unzGetFilePos64 (unzFile file, unz64_file_pos* file_pos)
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

int PrivateMinizip_::unzGetFilePos (unzFile file, unz_file_pos* file_pos)
{
    unz64_file_pos file_pos64;
    int            err = unzGetFilePos64 (file, &file_pos64);
    if (err == UNZ_OK) {
        file_pos->pos_in_zip_directory = (uLong)file_pos64.pos_in_zip_directory;
        file_pos->num_of_file          = (uLong)file_pos64.num_of_file;
    }
    return err;
}

int PrivateMinizip_::unzGoToFilePos64 (unzFile file, const unz64_file_pos* file_pos)
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
    err = unz64local_GetCurrentFileInfoInternal_ (file, &s->cur_file_info, &s->cur_file_info_internal, NULL, 0, NULL, 0, NULL, 0);
    /* return results */
    s->current_file_ok = (err == UNZ_OK);
    return err;
}

int PrivateMinizip_::unzGoToFilePos (unzFile file, unz_file_pos* file_pos)
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

    if (ZSEEK64 (s->z_filefunc, s->filestream, s->cur_file_info_internal.offset_curfile + s->byte_before_the_zipfile, ZLIB_FILEFUNC_SEEK_SET) != 0)
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
    *poffset_local_extrafield = s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER + size_filename;
    *psize_local_extrafield   = (uInt)size_extra_field;

    *piSizeVar += (uInt)size_extra_field;

    return err;
}

/*
      Open for reading data the current file in the zipfile.
      If there is no error and the file is opened, the return value is UNZ_OK.
    */
int PrivateMinizip_::unzOpenCurrentFile3_ (unzFile file, int* method, int* level, int raw, const char* password)
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
    pfile_in_zip_read_info->rest_read_compressed   = s->cur_file_info.compressed_size;
    pfile_in_zip_read_info->rest_read_uncompressed = s->cur_file_info.uncompressed_size;

    pfile_in_zip_read_info->pos_in_zipfile = s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER + iSizeVar;

    pfile_in_zip_read_info->stream.avail_in = (uInt)0;

    s->pfile_in_zip_read = pfile_in_zip_read_info;
    s->encrypted         = 0;

#ifndef NOUNCRYPT
    if (password != NULL) {
        int i;
        s->pcrc_32_tab = get_crc_table ();
        init_keys (password, s->keys, s->pcrc_32_tab);
        if (ZSEEK64 (s->z_filefunc, s->filestream, s->pfile_in_zip_read->pos_in_zipfile + s->pfile_in_zip_read->byte_before_the_zipfile, SEEK_SET) != 0)
            return UNZ_INTERNALERROR;
        if (ZREAD64 (s->z_filefunc, s->filestream, source, 12) < 12)
            return UNZ_INTERNALERROR;

        for (i = 0; i < 12; ++i)
            zdecode (s->keys, s->pcrc_32_tab, source[i]);

        s->pfile_in_zip_read->pos_in_zipfile += 12;
        s->encrypted = 1;
    }
#endif

    return UNZ_OK;
}

int PrivateMinizip_::unzOpenCurrentFile (unzFile file)
{
    return unzOpenCurrentFile3_ (file, NULL, NULL, 0, NULL);
}

int PrivateMinizip_::unzOpenCurrentFilePassword (unzFile file, const char* password)
{
    return unzOpenCurrentFile3_ (file, NULL, NULL, 0, password);
}

int PrivateMinizip_::unzOpenCurrentFile2 (unzFile file, int* method, int* level, int raw)
{
    return unzOpenCurrentFile3_ (file, method, level, raw, NULL);
}

/** Addition for GDAL : START */

ZPOS64_T PrivateMinizip_::unzGetCurrentFileZStreamPos64 (unzFile file)
{
    unz64_s*                   s;
    file_in_zip64_read_info_s* pfile_in_zip_read_info;
    s = (unz64_s*)file;
    if (file == NULL)
        return 0; //UNZ_PARAMERROR;
    pfile_in_zip_read_info = s->pfile_in_zip_read;
    if (pfile_in_zip_read_info == NULL)
        return 0; //UNZ_PARAMERROR;
    return pfile_in_zip_read_info->pos_in_zipfile + pfile_in_zip_read_info->byte_before_the_zipfile;
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
int PrivateMinizip_::unzReadCurrentFile_ (unzFile file, voidp buf, unsigned len)
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

    if ((len > pfile_in_zip_read_info->rest_read_uncompressed) && (!(pfile_in_zip_read_info->raw)))
        pfile_in_zip_read_info->stream.avail_out = (uInt)pfile_in_zip_read_info->rest_read_uncompressed;

    if ((len > pfile_in_zip_read_info->rest_read_compressed + pfile_in_zip_read_info->stream.avail_in) && (pfile_in_zip_read_info->raw))
        pfile_in_zip_read_info->stream.avail_out = (uInt)pfile_in_zip_read_info->rest_read_compressed + pfile_in_zip_read_info->stream.avail_in;

    while (pfile_in_zip_read_info->stream.avail_out > 0) {
        if ((pfile_in_zip_read_info->stream.avail_in == 0) && (pfile_in_zip_read_info->rest_read_compressed > 0)) {
            uInt uReadThis = UNZ_BUFSIZE;
            if (pfile_in_zip_read_info->rest_read_compressed < uReadThis)
                uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;
            if (uReadThis == 0)
                return UNZ_EOF;
            if (ZSEEK64 (pfile_in_zip_read_info->z_filefunc, pfile_in_zip_read_info->filestream,
                         pfile_in_zip_read_info->pos_in_zipfile + pfile_in_zip_read_info->byte_before_the_zipfile, ZLIB_FILEFUNC_SEEK_SET) != 0)
                return UNZ_ERRNO;
            if (ZREAD64 (pfile_in_zip_read_info->z_filefunc, pfile_in_zip_read_info->filestream, pfile_in_zip_read_info->read_buffer, uReadThis) != uReadThis)
                return UNZ_ERRNO;

#ifndef NOUNCRYPT
            if (s->encrypted) {
                uInt i;
                for (i = 0; i < uReadThis; i++)
                    pfile_in_zip_read_info->read_buffer[i] = zdecode (s->keys, s->pcrc_32_tab, pfile_in_zip_read_info->read_buffer[i]);
            }
#endif

            pfile_in_zip_read_info->pos_in_zipfile += uReadThis;

            pfile_in_zip_read_info->rest_read_compressed -= uReadThis;

            pfile_in_zip_read_info->stream.next_in  = (Bytef*)pfile_in_zip_read_info->read_buffer;
            pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
        }

        if ((pfile_in_zip_read_info->compression_method == 0) || (pfile_in_zip_read_info->raw)) {
            uInt uDoCopy, i;

            if ((pfile_in_zip_read_info->stream.avail_in == 0) && (pfile_in_zip_read_info->rest_read_compressed == 0))
                return (iRead == 0) ? UNZ_EOF : iRead;

            if (pfile_in_zip_read_info->stream.avail_out < pfile_in_zip_read_info->stream.avail_in)
                uDoCopy = pfile_in_zip_read_info->stream.avail_out;
            else
                uDoCopy = pfile_in_zip_read_info->stream.avail_in;

            for (i = 0; i < uDoCopy; i++)
                *(pfile_in_zip_read_info->stream.next_out + i) = *(pfile_in_zip_read_info->stream.next_in + i);

            pfile_in_zip_read_info->total_out_64 = pfile_in_zip_read_info->total_out_64 + uDoCopy;

            pfile_in_zip_read_info->crc32 = crc32 (pfile_in_zip_read_info->crc32, pfile_in_zip_read_info->stream.next_out, uDoCopy);
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

            pfile_in_zip_read_info->crc32 = crc32 (pfile_in_zip_read_info->crc32, bufBefore, (uInt)(uOutThis));
            pfile_in_zip_read_info->rest_read_uncompressed -= uOutThis;
            iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);

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

            pfile_in_zip_read_info->crc32 = crc32 (pfile_in_zip_read_info->crc32, bufBefore, (uInt)(uOutThis));

            pfile_in_zip_read_info->rest_read_uncompressed -= uOutThis;

            iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);

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
z_off_t PrivateMinizip_::unztell (unzFile file)
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

ZPOS64_T PrivateMinizip_::unztell64 (unzFile file)
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
int PrivateMinizip_::unzeof (unzFile file)
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
int PrivateMinizip_::unzGetLocalExtrafield (unzFile file, voidp buf, unsigned len)
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

    size_to_read = (pfile_in_zip_read_info->size_local_extrafield - pfile_in_zip_read_info->pos_local_extrafield);

    if (buf == NULL)
        return (int)size_to_read;

    if (len > size_to_read)
        read_now = (uInt)size_to_read;
    else
        read_now = (uInt)len;

    if (read_now == 0)
        return 0;

    if (ZSEEK64 (pfile_in_zip_read_info->z_filefunc, pfile_in_zip_read_info->filestream,
                 pfile_in_zip_read_info->offset_local_extrafield + pfile_in_zip_read_info->pos_local_extrafield, ZLIB_FILEFUNC_SEEK_SET) != 0)
        return UNZ_ERRNO;

    if (ZREAD64 (pfile_in_zip_read_info->z_filefunc, pfile_in_zip_read_info->filestream, buf, read_now) != read_now)
        return UNZ_ERRNO;

    return (int)read_now;
}

/*
      Close the file in zip opened with unzOpenCurrentFile
      Return UNZ_CRCERROR if all the file was read but the CRC is not good
    */
int PrivateMinizip_::unzCloseCurrentFile_ (unzFile file)
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

    if ((pfile_in_zip_read_info->rest_read_uncompressed == 0) && (!pfile_in_zip_read_info->raw)) {
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
int PrivateMinizip_::unzGetGlobalComment (unzFile file, char* szComment, uLong uSizeBuf)
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
ZPOS64_T PrivateMinizip_::unzGetOffset64 (unzFile file)
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

uLong PrivateMinizip_::unzGetOffset (unzFile file)
{
    ZPOS64_T offset64;

    if (file == NULL)
        return 0; //UNZ_PARAMERROR;
    offset64 = unzGetOffset64 (file);
    return (uLong)offset64;
}

int PrivateMinizip_::unzSetOffset64 (unzFile file, ZPOS64_T pos)
{
    unz64_s* s;
    int      err;

    if (file == NULL)
        return UNZ_PARAMERROR;
    s = (unz64_s*)file;

    s->pos_in_central_dir = pos;
    s->num_file           = s->gi.number_entry; /* hack */
    err = unz64local_GetCurrentFileInfoInternal_ (file, &s->cur_file_info, &s->cur_file_info_internal, NULL, 0, NULL, 0, NULL, 0);
    s->current_file_ok = (err == UNZ_OK);
    return err;
}

int PrivateMinizip_::unzSetOffset (unzFile file, uLong pos)
{
    return unzSetOffset64 (file, pos);
}
///////////////////////////////// END OF unzip.c ////////////////////

/// START OF zip.c

#ifndef VERSIONMADEBY
#define VERSIONMADEBY (0x0) /* platform dependent */
#endif

#ifndef Z_BUFSIZE
#define Z_BUFSIZE (64 * 1024) //(16384)
#endif

#ifndef Z_MAXFILENAMEINZIP
#define Z_MAXFILENAMEINZIP (256)
#endif

#ifndef ALLOC
#define ALLOC(size) (malloc (size))
#endif

/*
#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)
*/

/* I've found an old Unix (a SunOS 4.1.3_U1) without all SEEK_* defined.... */

// NOT sure that this work on ALL platform
#define MAKEULONG64(a, b) ((ZPOS64_T)(((unsigned long)(a)) | ((ZPOS64_T)((unsigned long)(b))) << 32))

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef DEF_MEM_LEVEL
#if MAX_MEM_LEVEL >= 8
#define DEF_MEM_LEVEL 8
#else
#define DEF_MEM_LEVEL MAX_MEM_LEVEL
#endif
#endif
const char zip_copyright[] = " zip 1.01 Copyright 1998-2004 Gilles Vollant - http://www.winimage.com/zLibDll";

#define SIZEDATA_INDATABLOCK (4096 - (4 * 4))

#define LOCALHEADERMAGIC (0x04034b50)
#define CENTRALHEADERMAGIC (0x02014b50)
#define ENDHEADERMAGIC (0x06054b50)
#define ZIP64ENDHEADERMAGIC (0x6064b50)
#define ZIP64ENDLOCHEADERMAGIC (0x7064b50)

#define FLAG_LOCALHEADER_OFFSET (0x06)
#define CRC_LOCALHEADER_OFFSET (0x0e)

#define SIZECENTRALHEADER (0x2e) /* 46 */

typedef struct linkedlist_datablock_internal_s {
    struct linkedlist_datablock_internal_s* next_datablock;
    uLong                                   avail_in_this_block;
    uLong                                   filled_in_this_block;
    uLong                                   unused; /* for future use and alignment */
    unsigned char                           data[SIZEDATA_INDATABLOCK];
} linkedlist_datablock_internal;

typedef struct linkedlist_data_s {
    linkedlist_datablock_internal* first_block;
    linkedlist_datablock_internal* last_block;
} linkedlist_data;

typedef struct {
    z_stream stream; /* zLib stream structure for inflate */
#ifdef HAVE_BZIP2
    bz_stream bstream; /* bzLib stream structure for bziped */
#endif

    int  stream_initialised;   /* 1 is stream is initialised */
    uInt pos_in_buffered_data; /* last written byte in buffered_data */

    ZPOS64_T pos_local_header; /* offset of the local header of the file
                                     currently writing */
    char*    central_header;   /* central header data for the current file */
    uLong    size_centralExtra;
    uLong    size_centralheader;    /* size of the central header for cur file */
    uLong    size_centralExtraFree; /* Extra bytes allocated to the centralheader but that are not used */
    uLong    flag;                  /* flag of the file currently writing */

    int      method;                   /* compression method of file currently wr.*/
    int      raw;                      /* 1 for directly writing raw data */
    Byte     buffered_data[Z_BUFSIZE]; /* buffer contain compressed data to be writ*/
    uLong    dosDate;
    uLong    crc32;
    int      encrypt;
    int      zip64; /* Add ZIP64 extended information in the extra field */
    ZPOS64_T pos_zip64extrainfo;
    ZPOS64_T totalCompressedData;
    ZPOS64_T totalUncompressedData;
#ifndef NOCRYPT
    unsigned long  keys[3]; /* keys defining the pseudo-random sequence */
    const z_crc_t* pcrc_32_tab;
    unsigned       crypt_header_size;
#endif
} curfile64_info;

typedef struct {
    zlib_filefunc64_32_def z_filefunc;
    voidpf                 filestream;           /* io structure of the zipfile */
    linkedlist_data        central_dir;          /* datablock with central dir in construction*/
    int                    in_opened_file_inzip; /* 1 if a file in the zip is currently writ.*/
    curfile64_info         ci;                   /* info on the file currently writing */

    ZPOS64_T begin_pos; /* position of the beginning of the zipfile */
    ZPOS64_T add_position_when_writing_offset;
    ZPOS64_T number_entry;

#ifndef NO_ADDFILEINEXISTINGZIP
    char* globalcomment;
#endif

} zip64_internal;

namespace {

    linkedlist_datablock_internal* allocate_new_datablock (void)
    {
        linkedlist_datablock_internal* ldi;
        ldi = (linkedlist_datablock_internal*)ALLOC (sizeof (linkedlist_datablock_internal));
        if (ldi != NULL) {
            ldi->next_datablock       = NULL;
            ldi->filled_in_this_block = 0;
            ldi->avail_in_this_block  = SIZEDATA_INDATABLOCK;
        }
        return ldi;
    }

    void free_datablock (linkedlist_datablock_internal* ldi)
    {
        while (ldi != NULL) {
            linkedlist_datablock_internal* ldinext = ldi->next_datablock;
            free (ldi);
            ldi = ldinext;
        }
    }

    void init_linkedlist (linkedlist_data* ll)
    {
        ll->first_block = ll->last_block = NULL;
    }

    void free_linkedlist (linkedlist_data* ll)
    {
        free_datablock (ll->first_block);
        ll->first_block = ll->last_block = NULL;
    }

    int add_data_in_datablock (linkedlist_data* ll, const void* buf, uLong len)
    {
        linkedlist_datablock_internal* ldi;
        const unsigned char*           from_copy;

        if (ll == NULL)
            return ZIP_INTERNALERROR;

        if (ll->last_block == NULL) {
            ll->first_block = ll->last_block = allocate_new_datablock ();
            if (ll->first_block == NULL)
                return ZIP_INTERNALERROR;
        }

        ldi       = ll->last_block;
        from_copy = (const unsigned char*)buf;

        while (len > 0) {
            uInt           copy_this;
            uInt           i;
            unsigned char* to_copy;

            if (ldi->avail_in_this_block == 0) {
                ldi->next_datablock = allocate_new_datablock ();
                if (ldi->next_datablock == NULL)
                    return ZIP_INTERNALERROR;
                ldi            = ldi->next_datablock;
                ll->last_block = ldi;
            }

            if (ldi->avail_in_this_block < len)
                copy_this = (uInt)ldi->avail_in_this_block;
            else
                copy_this = (uInt)len;

            to_copy = &(ldi->data[ldi->filled_in_this_block]);

            for (i = 0; i < copy_this; i++)
                *(to_copy + i) = *(from_copy + i);

            ldi->filled_in_this_block += copy_this;
            ldi->avail_in_this_block -= copy_this;
            from_copy += copy_this;
            len -= copy_this;
        }
        return ZIP_OK;
    }

    /****************************************************************************/

#ifndef NO_ADDFILEINEXISTINGZIP
    /* ===========================================================================
   Inputs a long in LSB order to the given file
   nbByte == 1, 2 ,4 or 8 (byte, short or long, ZPOS64_T)
*/

    int zip64local_putValue (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream, ZPOS64_T x, int nbByte)
    {
        unsigned char buf[8];
        int           n;
        for (n = 0; n < nbByte; n++) {
            buf[n] = (unsigned char)(x & 0xff);
            x >>= 8;
        }
        if (x != 0) { /* data overflow - hack for ZIP64 (X Roche) */
            for (n = 0; n < nbByte; n++) {
                buf[n] = 0xff;
            }
        }

        if (ZWRITE64 (*pzlib_filefunc_def, filestream, buf, (uLong)nbByte) != (uLong)nbByte)
            return ZIP_ERRNO;
        else
            return ZIP_OK;
    }

    void zip64local_putValue_inmemory (void* dest, ZPOS64_T x, int nbByte)
    {
        unsigned char* buf = (unsigned char*)dest;
        int            n;
        for (n = 0; n < nbByte; n++) {
            buf[n] = (unsigned char)(x & 0xff);
            x >>= 8;
        }

        if (x != 0) { /* data overflow - hack for ZIP64 */
            for (n = 0; n < nbByte; n++) {
                buf[n] = 0xff;
            }
        }
    }

    /****************************************************************************/

    uLong zip64local_TmzDateToDosDate (const tm_zip* ptm)
    {
        uLong year = (uLong)ptm->tm_year;
        if (year >= 1980)
            year -= 1980;
        else if (year >= 80)
            year -= 80;
        return (uLong)(((uLong)(ptm->tm_mday) + (32 * (uLong)(ptm->tm_mon + 1)) + (512 * year)) << 16) |
               (((uLong)ptm->tm_sec / 2) + (32 * (uLong)ptm->tm_min) + (2048 * (uLong)ptm->tm_hour));
    }

    /****************************************************************************/

    int zip64local_getByte (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream, int* pi)
    {
        unsigned char c;
        int           err = (int)ZREAD64 (*pzlib_filefunc_def, filestream, &c, 1);
        if (err == 1) {
            *pi = (int)c;
            return ZIP_OK;
        }
        else {
            if (ZERROR64 (*pzlib_filefunc_def, filestream))
                return ZIP_ERRNO;
            else
                return ZIP_EOF;
        }
    }

    /* ===========================================================================
   Reads a long in LSB order from the given gz_stream. Sets
*/
    int zip64local_getShort (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream, uLong* pX)
    {
        uLong x;
        int   i = 0;
        int   err;

        err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x   = (uLong)i;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((uLong)i) << 8;

        if (err == ZIP_OK)
            *pX = x;
        else
            *pX = 0;
        return err;
    }

    int zip64local_getLong (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream, uLong* pX)
    {
        uLong x;
        int   i = 0;
        int   err;

        err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x   = (uLong)i;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((uLong)i) << 8;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((uLong)i) << 16;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((uLong)i) << 24;

        if (err == ZIP_OK)
            *pX = x;
        else
            *pX = 0;
        return err;
    }

    int zip64local_getLong64 (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream, ZPOS64_T* pX)
    {
        ZPOS64_T x;
        int      i = 0;
        int      err;

        err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x   = (ZPOS64_T)i;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((ZPOS64_T)i) << 8;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((ZPOS64_T)i) << 16;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((ZPOS64_T)i) << 24;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((ZPOS64_T)i) << 32;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((ZPOS64_T)i) << 40;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((ZPOS64_T)i) << 48;

        if (err == ZIP_OK)
            err = zip64local_getByte (pzlib_filefunc_def, filestream, &i);
        x += ((ZPOS64_T)i) << 56;

        if (err == ZIP_OK)
            *pX = x;
        else
            *pX = 0;

        return err;
    }

#ifndef BUFREADCOMMENT
#define BUFREADCOMMENT (0x400)
#endif
    /*
  Locate the Central directory of a zipfile (at the end, just before
    the global comment)
*/
    ZPOS64_T zip64local_SearchCentralDir (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream)
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

            uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ? (BUFREADCOMMENT + 4) : (uLong)(uSizeFile - uReadPos);
            if (ZSEEK64 (*pzlib_filefunc_def, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0)
                break;

            if (ZREAD64 (*pzlib_filefunc_def, filestream, buf, uReadSize) != uReadSize)
                break;

            for (i = (int)uReadSize - 3; (i--) > 0;)
                if (((*(buf + i)) == 0x50) && ((*(buf + i + 1)) == 0x4b) && ((*(buf + i + 2)) == 0x05) && ((*(buf + i + 3)) == 0x06)) {
                    uPosFound = uReadPos + (unsigned)i;
                    break;
                }

            if (uPosFound != 0)
                break;
        }
        free (buf);
        return uPosFound;
    }

    /*
Locate the End of Zip64 Central directory locator and from there find the CD of a zipfile (at the end, just before
the global comment)
*/
    ZPOS64_T zip64local_SearchCentralDir64 (const zlib_filefunc64_32_def* pzlib_filefunc_def, voidpf filestream)
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

            uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ? (BUFREADCOMMENT + 4) : (uLong)(uSizeFile - uReadPos);
            if (ZSEEK64 (*pzlib_filefunc_def, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0)
                break;

            if (ZREAD64 (*pzlib_filefunc_def, filestream, buf, uReadSize) != uReadSize)
                break;

            for (i = (int)uReadSize - 3; (i--) > 0;) {
                // Signature "0x07064b50" Zip64 end of central directory locater
                if (((*(buf + i)) == 0x50) && ((*(buf + i + 1)) == 0x4b) && ((*(buf + i + 2)) == 0x06) && ((*(buf + i + 3)) == 0x07)) {
                    uPosFound = uReadPos + (unsigned)i;
                    break;
                }
            }

            if (uPosFound != 0)
                break;
        }

        free (buf);
        if (uPosFound == 0)
            return 0;

        /* Zip64 end of central directory locator */
        if (ZSEEK64 (*pzlib_filefunc_def, filestream, uPosFound, ZLIB_FILEFUNC_SEEK_SET) != 0)
            return 0;

        /* the signature, already checked */
        if (zip64local_getLong (pzlib_filefunc_def, filestream, &uL) != ZIP_OK)
            return 0;

        /* number of the disk with the start of the zip64 end of central directory */
        if (zip64local_getLong (pzlib_filefunc_def, filestream, &uL) != ZIP_OK)
            return 0;
        if (uL != 0)
            return 0;

        /* relative offset of the zip64 end of central directory record */
        if (zip64local_getLong64 (pzlib_filefunc_def, filestream, &relativeOffset) != ZIP_OK)
            return 0;

        /* total number of disks */
        if (zip64local_getLong (pzlib_filefunc_def, filestream, &uL) != ZIP_OK)
            return 0;
        if (uL != 1)
            return 0;

        /* Goto Zip64 end of central directory record */
        if (ZSEEK64 (*pzlib_filefunc_def, filestream, relativeOffset, ZLIB_FILEFUNC_SEEK_SET) != 0)
            return 0;

        /* the signature */
        if (zip64local_getLong (pzlib_filefunc_def, filestream, &uL) != ZIP_OK)
            return 0;

        if (uL != 0x06064b50) // signature of 'Zip64 end of central directory'
            return 0;

        return relativeOffset;
    }

    int LoadCentralDirectoryRecord (zip64_internal* pziinit)
    {
        int      err = ZIP_OK;
        ZPOS64_T byte_before_the_zipfile; /* byte before the zipfile, (>0 for sfx)*/

        ZPOS64_T size_central_dir;   /* size of the central directory  */
        ZPOS64_T offset_central_dir; /* offset of start of central directory */
        ZPOS64_T central_pos;
        uLong    uL;

        uLong    number_disk;         /* number of the current disk, used for
                              spanning ZIP, unsupported, always 0*/
        uLong    number_disk_with_CD; /* number of the disk with central dir, used
                              for spanning ZIP, unsupported, always 0*/
        ZPOS64_T number_entry;
        ZPOS64_T number_entry_CD; /* total number of entries in
                                the central dir
                                (same than number_entry on nospan) */
        uLong    VersionMadeBy;
        uLong    VersionNeeded;
        uLong    size_comment;

        int hasZIP64Record = 0;

        // check first if we find a ZIP64 record
        central_pos = zip64local_SearchCentralDir64 (&pziinit->z_filefunc, pziinit->filestream);
        if (central_pos > 0) {
            hasZIP64Record = 1;
        }
        else if (central_pos == 0) {
            central_pos = zip64local_SearchCentralDir (&pziinit->z_filefunc, pziinit->filestream);
        }

        /* disable to allow appending to empty ZIP archive
        if (central_pos==0)
            err=ZIP_ERRNO;
*/

        if (hasZIP64Record) {
            ZPOS64_T sizeEndOfCentralDirectory;
            if (ZSEEK64 (pziinit->z_filefunc, pziinit->filestream, central_pos, ZLIB_FILEFUNC_SEEK_SET) != 0)
                err = ZIP_ERRNO;

            /* the signature, already checked */
            if (zip64local_getLong (&pziinit->z_filefunc, pziinit->filestream, &uL) != ZIP_OK)
                err = ZIP_ERRNO;

            /* size of zip64 end of central directory record */
            if (zip64local_getLong64 (&pziinit->z_filefunc, pziinit->filestream, &sizeEndOfCentralDirectory) != ZIP_OK)
                err = ZIP_ERRNO;

            /* version made by */
            if (zip64local_getShort (&pziinit->z_filefunc, pziinit->filestream, &VersionMadeBy) != ZIP_OK)
                err = ZIP_ERRNO;

            /* version needed to extract */
            if (zip64local_getShort (&pziinit->z_filefunc, pziinit->filestream, &VersionNeeded) != ZIP_OK)
                err = ZIP_ERRNO;

            /* number of this disk */
            if (zip64local_getLong (&pziinit->z_filefunc, pziinit->filestream, &number_disk) != ZIP_OK)
                err = ZIP_ERRNO;

            /* number of the disk with the start of the central directory */
            if (zip64local_getLong (&pziinit->z_filefunc, pziinit->filestream, &number_disk_with_CD) != ZIP_OK)
                err = ZIP_ERRNO;

            /* total number of entries in the central directory on this disk */
            if (zip64local_getLong64 (&pziinit->z_filefunc, pziinit->filestream, &number_entry) != ZIP_OK)
                err = ZIP_ERRNO;

            /* total number of entries in the central directory */
            if (zip64local_getLong64 (&pziinit->z_filefunc, pziinit->filestream, &number_entry_CD) != ZIP_OK)
                err = ZIP_ERRNO;

            if ((number_entry_CD != number_entry) || (number_disk_with_CD != 0) || (number_disk != 0))
                err = ZIP_BADZIPFILE;

            /* size of the central directory */
            if (zip64local_getLong64 (&pziinit->z_filefunc, pziinit->filestream, &size_central_dir) != ZIP_OK)
                err = ZIP_ERRNO;

            /* offset of start of central directory with respect to the
    starting disk number */
            if (zip64local_getLong64 (&pziinit->z_filefunc, pziinit->filestream, &offset_central_dir) != ZIP_OK)
                err = ZIP_ERRNO;

            // TODO..
            // read the comment from the standard central header.
            size_comment = 0;
        }
        else {
            // Read End of central Directory info
            if (ZSEEK64 (pziinit->z_filefunc, pziinit->filestream, central_pos, ZLIB_FILEFUNC_SEEK_SET) != 0)
                err = ZIP_ERRNO;

            /* the signature, already checked */
            if (zip64local_getLong (&pziinit->z_filefunc, pziinit->filestream, &uL) != ZIP_OK)
                err = ZIP_ERRNO;

            /* number of this disk */
            if (zip64local_getShort (&pziinit->z_filefunc, pziinit->filestream, &number_disk) != ZIP_OK)
                err = ZIP_ERRNO;

            /* number of the disk with the start of the central directory */
            if (zip64local_getShort (&pziinit->z_filefunc, pziinit->filestream, &number_disk_with_CD) != ZIP_OK)
                err = ZIP_ERRNO;

            /* total number of entries in the central dir on this disk */
            number_entry = 0;
            if (zip64local_getShort (&pziinit->z_filefunc, pziinit->filestream, &uL) != ZIP_OK)
                err = ZIP_ERRNO;
            else
                number_entry = uL;

            /* total number of entries in the central dir */
            number_entry_CD = 0;
            if (zip64local_getShort (&pziinit->z_filefunc, pziinit->filestream, &uL) != ZIP_OK)
                err = ZIP_ERRNO;
            else
                number_entry_CD = uL;

            if ((number_entry_CD != number_entry) || (number_disk_with_CD != 0) || (number_disk != 0))
                err = ZIP_BADZIPFILE;

            /* size of the central directory */
            size_central_dir = 0;
            if (zip64local_getLong (&pziinit->z_filefunc, pziinit->filestream, &uL) != ZIP_OK)
                err = ZIP_ERRNO;
            else
                size_central_dir = uL;

            /* offset of start of central directory with respect to the starting disk number */
            offset_central_dir = 0;
            if (zip64local_getLong (&pziinit->z_filefunc, pziinit->filestream, &uL) != ZIP_OK)
                err = ZIP_ERRNO;
            else
                offset_central_dir = uL;

            /* zipfile global comment length */
            if (zip64local_getShort (&pziinit->z_filefunc, pziinit->filestream, &size_comment) != ZIP_OK)
                err = ZIP_ERRNO;
        }

        if ((central_pos < offset_central_dir + size_central_dir) && (err == ZIP_OK))
            err = ZIP_BADZIPFILE;

        if (err != ZIP_OK) {
            ZCLOSE64 (pziinit->z_filefunc, pziinit->filestream);
            return ZIP_ERRNO;
        }

        if (size_comment > 0) {
            pziinit->globalcomment = (char*)ALLOC (size_comment + 1);
            if (pziinit->globalcomment) {
                size_comment = ZREAD64 (pziinit->z_filefunc, pziinit->filestream, pziinit->globalcomment, size_comment);
                pziinit->globalcomment[size_comment] = 0;
            }
        }

        byte_before_the_zipfile                   = central_pos - (offset_central_dir + size_central_dir);
        pziinit->add_position_when_writing_offset = byte_before_the_zipfile;

        {
            ZPOS64_T size_central_dir_to_read = size_central_dir;
            size_t   buf_size                 = SIZEDATA_INDATABLOCK;
            void*    buf_read                 = (void*)ALLOC (buf_size);
            if (ZSEEK64 (pziinit->z_filefunc, pziinit->filestream, offset_central_dir + byte_before_the_zipfile, ZLIB_FILEFUNC_SEEK_SET) != 0)
                err = ZIP_ERRNO;

            while ((size_central_dir_to_read > 0) && (err == ZIP_OK)) {
                ZPOS64_T read_this = SIZEDATA_INDATABLOCK;
                if (read_this > size_central_dir_to_read)
                    read_this = size_central_dir_to_read;

                if (ZREAD64 (pziinit->z_filefunc, pziinit->filestream, buf_read, (uLong)read_this) != read_this)
                    err = ZIP_ERRNO;

                if (err == ZIP_OK)
                    err = add_data_in_datablock (&pziinit->central_dir, buf_read, (uLong)read_this);

                size_central_dir_to_read -= read_this;
            }
            free (buf_read);
        }
        pziinit->begin_pos    = byte_before_the_zipfile;
        pziinit->number_entry = number_entry_CD;

        if (ZSEEK64 (pziinit->z_filefunc, pziinit->filestream, offset_central_dir + byte_before_the_zipfile, ZLIB_FILEFUNC_SEEK_SET) != 0)
            err = ZIP_ERRNO;

        return err;
    }

#endif /* !NO_ADDFILEINEXISTINGZIP*/
}

/************************************************************/
zipFile PrivateMinizip_::zipOpen3 (const void* pathname, int append, zipcharpc* globalcomment, zlib_filefunc64_32_def* pzlib_filefunc64_32_def)
{
    zip64_internal  ziinit;
    zip64_internal* zi;
    int             err = ZIP_OK;

    ziinit.z_filefunc.zseek32_file = NULL;
    ziinit.z_filefunc.ztell32_file = NULL;

#if 1
    Require (pzlib_filefunc64_32_def != NULL);
    ziinit.z_filefunc = *pzlib_filefunc64_32_def; // avoid importing fill_fopen64_filefunc if we can
#else
    if (pzlib_filefunc64_32_def == NULL)
        fill_fopen64_filefunc (&ziinit.z_filefunc.zfile_func64);
    else
        ziinit.z_filefunc = *pzlib_filefunc64_32_def;
#endif

    ziinit.filestream =
        ZOPEN64 (ziinit.z_filefunc, pathname,
                 (append == APPEND_STATUS_CREATE) ? (ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_WRITE | ZLIB_FILEFUNC_MODE_CREATE)
                                                  : (ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_WRITE | ZLIB_FILEFUNC_MODE_EXISTING));

    if (ziinit.filestream == NULL)
        return NULL;

    if (append == APPEND_STATUS_CREATEAFTER)
        ZSEEK64 (ziinit.z_filefunc, ziinit.filestream, 0, SEEK_END);

    ziinit.begin_pos                        = ZTELL64 (ziinit.z_filefunc, ziinit.filestream);
    ziinit.in_opened_file_inzip             = 0;
    ziinit.ci.stream_initialised            = 0;
    ziinit.number_entry                     = 0;
    ziinit.add_position_when_writing_offset = 0;
    init_linkedlist (&(ziinit.central_dir));

    zi = (zip64_internal*)ALLOC (sizeof (zip64_internal));
    if (zi == NULL) {
        ZCLOSE64 (ziinit.z_filefunc, ziinit.filestream);
        return NULL;
    }

    /* now we add file in a zipfile */
#ifndef NO_ADDFILEINEXISTINGZIP
    ziinit.globalcomment = NULL;
    if (append == APPEND_STATUS_ADDINZIP) {
        // Read and Cache Central Directory Records
        err = LoadCentralDirectoryRecord (&ziinit);
    }

    if (globalcomment) {
        *globalcomment = ziinit.globalcomment;
    }
#endif /* !NO_ADDFILEINEXISTINGZIP*/

    if (err != ZIP_OK) {
#ifndef NO_ADDFILEINEXISTINGZIP
        free (ziinit.globalcomment);
#endif /* !NO_ADDFILEINEXISTINGZIP*/
        free (zi);
        return NULL;
    }
    else {
        *zi = ziinit;
        return (zipFile)zi;
    }
}

zipFile PrivateMinizip_::zipOpen2 (const char* pathname, int append, zipcharpc* globalcomment, zlib_filefunc_def* pzlib_filefunc32_def)
{
    if (pzlib_filefunc32_def != NULL) {
        zlib_filefunc64_32_def zlib_filefunc64_32_def_fill;
        fill_zlib_filefunc64_32_def_from_filefunc32 (&zlib_filefunc64_32_def_fill, pzlib_filefunc32_def);
        return zipOpen3 (pathname, append, globalcomment, &zlib_filefunc64_32_def_fill);
    }
    else
        return zipOpen3 (pathname, append, globalcomment, NULL);
}

zipFile PrivateMinizip_::zipOpen2_64 (const void* pathname, int append, zipcharpc* globalcomment, zlib_filefunc64_def* pzlib_filefunc_def)
{
    if (pzlib_filefunc_def != NULL) {
        zlib_filefunc64_32_def zlib_filefunc64_32_def_fill;
        zlib_filefunc64_32_def_fill.zfile_func64 = *pzlib_filefunc_def;
        zlib_filefunc64_32_def_fill.ztell32_file = NULL;
        zlib_filefunc64_32_def_fill.zseek32_file = NULL;
        return zipOpen3 (pathname, append, globalcomment, &zlib_filefunc64_32_def_fill);
    }
    else
        return zipOpen3 (pathname, append, globalcomment, NULL);
}

zipFile PrivateMinizip_::zipOpen (const char* pathname, int append)
{
    return zipOpen3 ((const void*)pathname, append, NULL, NULL);
}

zipFile PrivateMinizip_::zipOpen64 (const void* pathname, int append)
{
    return zipOpen3 (pathname, append, NULL, NULL);
}

namespace {

    int Write_LocalFileHeader (zip64_internal* zi, const char* filename, uInt size_extrafield_local, const void* extrafield_local)
    {
        /* write the local header */
        int  err;
        uInt size_filename   = (uInt)strlen (filename);
        uInt size_extrafield = size_extrafield_local;

        err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)LOCALHEADERMAGIC, 4);

        if (err == ZIP_OK) {
            if (zi->ci.zip64)
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)45, 2); /* version needed to extract */
            else
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)20, 2); /* version needed to extract */
        }

        if (err == ZIP_OK)
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)zi->ci.flag, 2);

        if (err == ZIP_OK)
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)zi->ci.method, 2);

        if (err == ZIP_OK)
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)zi->ci.dosDate, 4);

        // CRC / Compressed size / Uncompressed size will be filled in later and rewritten later
        if (err == ZIP_OK)
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0, 4); /* crc 32, unknown */
        if (err == ZIP_OK) {
            if (zi->ci.zip64)
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0xFFFFFFFF, 4); /* compressed size, unknown */
            else
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0, 4); /* compressed size, unknown */
        }
        if (err == ZIP_OK) {
            if (zi->ci.zip64)
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0xFFFFFFFF, 4); /* uncompressed size, unknown */
            else
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0, 4); /* uncompressed size, unknown */
        }

        if (err == ZIP_OK)
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)size_filename, 2);

        if (zi->ci.zip64) {
            size_extrafield += 20;
        }

        if (err == ZIP_OK)
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)size_extrafield, 2);

        if ((err == ZIP_OK) && (size_filename > 0)) {
            if (ZWRITE64 (zi->z_filefunc, zi->filestream, filename, size_filename) != size_filename)
                err = ZIP_ERRNO;
        }

        if ((err == ZIP_OK) && (size_extrafield_local > 0)) {
            if (ZWRITE64 (zi->z_filefunc, zi->filestream, extrafield_local, size_extrafield_local) != size_extrafield_local)
                err = ZIP_ERRNO;
        }

        if ((err == ZIP_OK) && (zi->ci.zip64)) {
            // write the Zip64 extended info
            short    HeaderID         = 1;
            short    DataSize         = 16;
            ZPOS64_T CompressedSize   = 0;
            ZPOS64_T UncompressedSize = 0;

            // Remember position of Zip64 extended info for the local file header. (needed when we update size after done with file)
            zi->ci.pos_zip64extrainfo = ZTELL64 (zi->z_filefunc, zi->filestream);

            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (ZPOS64_T)HeaderID, 2);
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (ZPOS64_T)DataSize, 2);

            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (ZPOS64_T)UncompressedSize, 8);
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (ZPOS64_T)CompressedSize, 8);
        }

        return err;
    }
}
/*
 NOTE.
 When writing RAW the ZIP64 extended information in extrafield_local and extrafield_global needs to be stripped
 before calling this function it can be done with zipRemoveExtraInfoBlock

 It is not done here because then we need to realloc a new buffer since parameters are 'const' and I want to minimize
 unnecessary allocations.
 */
int PrivateMinizip_::zipOpenNewFileInZip4_64 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                              uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                              const char* comment, int method, int level, int raw, int windowBits, int memLevel, int strategy,
                                              const char* password, uLong crcForCrypting, uLong versionMadeBy, uLong flagBase, int zip64)
{
    zip64_internal* zi;
    uInt            size_filename;
    uInt            size_comment;
    uInt            i;
    int             err = ZIP_OK;

#ifdef NOCRYPT
    (crcForCrypting);
    if (password != NULL)
        return ZIP_PARAMERROR;
#endif

    if (file == NULL)
        return ZIP_PARAMERROR;

#ifdef HAVE_BZIP2
    if ((method != 0) && (method != Z_DEFLATED) && (method != Z_BZIP2ED))
        return ZIP_PARAMERROR;
#else
    if ((method != 0) && (method != Z_DEFLATED))
        return ZIP_PARAMERROR;
#endif

    // The filename and comment length must fit in 16 bits.
    if ((filename != NULL) && (strlen (filename) > 0xffff))
        return ZIP_PARAMERROR;
    if ((comment != NULL) && (strlen (comment) > 0xffff))
        return ZIP_PARAMERROR;
    // The extra field length must fit in 16 bits. If the member also requires
    // a Zip64 extra block, that will also need to fit within that 16-bit
    // length, but that will be checked for later.
    if ((size_extrafield_local > 0xffff) || (size_extrafield_global > 0xffff))
        return ZIP_PARAMERROR;

    zi = (zip64_internal*)file;

    if (zi->in_opened_file_inzip == 1) {
        err = zipCloseFileInZip (file);
        if (err != ZIP_OK)
            return err;
    }

    if (filename == NULL)
        filename = "-";

    if (comment == NULL)
        size_comment = 0;
    else
        size_comment = (uInt)strlen (comment);

    size_filename = (uInt)strlen (filename);

    if (zipfi == NULL)
        zi->ci.dosDate = 0;
    else {
        if (zipfi->dosDate != 0)
            zi->ci.dosDate = zipfi->dosDate;
        else
            zi->ci.dosDate = zip64local_TmzDateToDosDate (&zipfi->tmz_date);
    }

    zi->ci.flag = flagBase;
    if ((level == 8) || (level == 9))
        zi->ci.flag |= 2;
    if (level == 2)
        zi->ci.flag |= 4;
    if (level == 1)
        zi->ci.flag |= 6;
    if (password != NULL)
        zi->ci.flag |= 1;

    zi->ci.crc32                = 0;
    zi->ci.method               = method;
    zi->ci.encrypt              = 0;
    zi->ci.stream_initialised   = 0;
    zi->ci.pos_in_buffered_data = 0;
    zi->ci.raw                  = raw;
    zi->ci.pos_local_header     = ZTELL64 (zi->z_filefunc, zi->filestream);

    zi->ci.size_centralheader    = SIZECENTRALHEADER + size_filename + size_extrafield_global + size_comment;
    zi->ci.size_centralExtraFree = 32; // Extra space we have reserved in case we need to add ZIP64 extra info data

    zi->ci.central_header = (char*)ALLOC ((uInt)zi->ci.size_centralheader + zi->ci.size_centralExtraFree);

    zi->ci.size_centralExtra = size_extrafield_global;
    zip64local_putValue_inmemory (zi->ci.central_header, (uLong)CENTRALHEADERMAGIC, 4);
    /* version info */
    zip64local_putValue_inmemory (zi->ci.central_header + 4, (uLong)versionMadeBy, 2);
    zip64local_putValue_inmemory (zi->ci.central_header + 6, (uLong)20, 2);
    zip64local_putValue_inmemory (zi->ci.central_header + 8, (uLong)zi->ci.flag, 2);
    zip64local_putValue_inmemory (zi->ci.central_header + 10, (uLong)zi->ci.method, 2);
    zip64local_putValue_inmemory (zi->ci.central_header + 12, (uLong)zi->ci.dosDate, 4);
    zip64local_putValue_inmemory (zi->ci.central_header + 16, (uLong)0, 4); /*crc*/
    zip64local_putValue_inmemory (zi->ci.central_header + 20, (uLong)0, 4); /*compr size*/
    zip64local_putValue_inmemory (zi->ci.central_header + 24, (uLong)0, 4); /*uncompr size*/
    zip64local_putValue_inmemory (zi->ci.central_header + 28, (uLong)size_filename, 2);
    zip64local_putValue_inmemory (zi->ci.central_header + 30, (uLong)size_extrafield_global, 2);
    zip64local_putValue_inmemory (zi->ci.central_header + 32, (uLong)size_comment, 2);
    zip64local_putValue_inmemory (zi->ci.central_header + 34, (uLong)0, 2); /*disk nm start*/

    if (zipfi == NULL)
        zip64local_putValue_inmemory (zi->ci.central_header + 36, (uLong)0, 2);
    else
        zip64local_putValue_inmemory (zi->ci.central_header + 36, (uLong)zipfi->internal_fa, 2);

    if (zipfi == NULL)
        zip64local_putValue_inmemory (zi->ci.central_header + 38, (uLong)0, 4);
    else
        zip64local_putValue_inmemory (zi->ci.central_header + 38, (uLong)zipfi->external_fa, 4);

    if (zi->ci.pos_local_header >= 0xffffffff)
        zip64local_putValue_inmemory (zi->ci.central_header + 42, (uLong)0xffffffff, 4);
    else
        zip64local_putValue_inmemory (zi->ci.central_header + 42, (uLong)zi->ci.pos_local_header - zi->add_position_when_writing_offset, 4);

    for (i = 0; i < size_filename; i++)
        *(zi->ci.central_header + SIZECENTRALHEADER + i) = *(filename + i);

    for (i = 0; i < size_extrafield_global; i++)
        *(zi->ci.central_header + SIZECENTRALHEADER + size_filename + i) = *(((const char*)extrafield_global) + i);

    for (i = 0; i < size_comment; i++)
        *(zi->ci.central_header + SIZECENTRALHEADER + size_filename + size_extrafield_global + i) = *(comment + i);
    if (zi->ci.central_header == NULL)
        return ZIP_INTERNALERROR;

    zi->ci.zip64                 = zip64;
    zi->ci.totalCompressedData   = 0;
    zi->ci.totalUncompressedData = 0;
    zi->ci.pos_zip64extrainfo    = 0;

    err = Write_LocalFileHeader (zi, filename, size_extrafield_local, extrafield_local);

#ifdef HAVE_BZIP2
    zi->ci.bstream.avail_in       = (uInt)0;
    zi->ci.bstream.avail_out      = (uInt)Z_BUFSIZE;
    zi->ci.bstream.next_out       = (char*)zi->ci.buffered_data;
    zi->ci.bstream.total_in_hi32  = 0;
    zi->ci.bstream.total_in_lo32  = 0;
    zi->ci.bstream.total_out_hi32 = 0;
    zi->ci.bstream.total_out_lo32 = 0;
#endif

    zi->ci.stream.avail_in  = (uInt)0;
    zi->ci.stream.avail_out = (uInt)Z_BUFSIZE;
    zi->ci.stream.next_out  = zi->ci.buffered_data;
    zi->ci.stream.total_in  = 0;
    zi->ci.stream.total_out = 0;
    zi->ci.stream.data_type = Z_BINARY;

#ifdef HAVE_BZIP2
    if ((err == ZIP_OK) && (zi->ci.method == Z_DEFLATED || zi->ci.method == Z_BZIP2ED) && (!zi->ci.raw))
#else
    if ((err == ZIP_OK) && (zi->ci.method == Z_DEFLATED) && (!zi->ci.raw))
#endif
    {
        if (zi->ci.method == Z_DEFLATED) {
            zi->ci.stream.zalloc = (alloc_func)0;
            zi->ci.stream.zfree  = (free_func)0;
            zi->ci.stream.opaque = (voidpf)0;

            if (windowBits > 0)
                windowBits = -windowBits;

            err = deflateInit2 (&zi->ci.stream, level, Z_DEFLATED, windowBits, memLevel, strategy);

            if (err == Z_OK)
                zi->ci.stream_initialised = Z_DEFLATED;
        }
        else if (zi->ci.method == Z_BZIP2ED) {
#ifdef HAVE_BZIP2
            // Init BZip stuff here
            zi->ci.bstream.bzalloc = 0;
            zi->ci.bstream.bzfree  = 0;
            zi->ci.bstream.opaque  = (voidpf)0;

            err = BZ2_bzCompressInit (&zi->ci.bstream, level, 0, 35);
            if (err == BZ_OK)
                zi->ci.stream_initialised = Z_BZIP2ED;
#endif
        }
    }

#ifndef NOCRYPT
    zi->ci.crypt_header_size = 0;
    if ((err == Z_OK) && (password != NULL)) {
        unsigned char bufHead[RAND_HEAD_LEN];
        unsigned int  sizeHead;
        zi->ci.encrypt     = 1;
        zi->ci.pcrc_32_tab = get_crc_table ();
        /*init_keys(password,zi->ci.keys,zi->ci.pcrc_32_tab);*/

        sizeHead                 = crypthead (password, bufHead, RAND_HEAD_LEN, zi->ci.keys, zi->ci.pcrc_32_tab, crcForCrypting);
        zi->ci.crypt_header_size = sizeHead;

        if (ZWRITE64 (zi->z_filefunc, zi->filestream, bufHead, sizeHead) != sizeHead)
            err = ZIP_ERRNO;
    }
#endif

    if (err == Z_OK)
        zi->in_opened_file_inzip = 1;
    return err;
}

int PrivateMinizip_::zipOpenNewFileInZip4 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                           uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                           const char* comment, int method, int level, int raw, int windowBits, int memLevel, int strategy,
                                           const char* password, uLong crcForCrypting, uLong versionMadeBy, uLong flagBase)
{
    return zipOpenNewFileInZip4_64 (file, filename, zipfi, extrafield_local, size_extrafield_local, extrafield_global, size_extrafield_global, comment,
                                    method, level, raw, windowBits, memLevel, strategy, password, crcForCrypting, versionMadeBy, flagBase, 0);
}

int PrivateMinizip_::zipOpenNewFileInZip3 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                           uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                           const char* comment, int method, int level, int raw, int windowBits, int memLevel, int strategy,
                                           const char* password, uLong crcForCrypting)
{
    return zipOpenNewFileInZip4_64 (file, filename, zipfi, extrafield_local, size_extrafield_local, extrafield_global, size_extrafield_global,
                                    comment, method, level, raw, windowBits, memLevel, strategy, password, crcForCrypting, VERSIONMADEBY, 0, 0);
}

int PrivateMinizip_::zipOpenNewFileInZip3_64 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                              uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                              const char* comment, int method, int level, int raw, int windowBits, int memLevel,
                                              int strategy, const char* password, uLong crcForCrypting, int zip64)
{
    return zipOpenNewFileInZip4_64 (file, filename, zipfi, extrafield_local, size_extrafield_local, extrafield_global, size_extrafield_global,
                                    comment, method, level, raw, windowBits, memLevel, strategy, password, crcForCrypting, VERSIONMADEBY, 0, zip64);
}

int PrivateMinizip_::zipOpenNewFileInZip2 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                           uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                           const char* comment, int method, int level, int raw)
{
    return zipOpenNewFileInZip4_64 (file, filename, zipfi, extrafield_local, size_extrafield_local, extrafield_global, size_extrafield_global,
                                    comment, method, level, raw, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, VERSIONMADEBY, 0, 0);
}

int PrivateMinizip_::zipOpenNewFileInZip2_64 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                              uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                              const char* comment, int method, int level, int raw, int zip64)
{
    return zipOpenNewFileInZip4_64 (file, filename, zipfi, extrafield_local, size_extrafield_local, extrafield_global, size_extrafield_global,
                                    comment, method, level, raw, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, VERSIONMADEBY, 0, zip64);
}

int PrivateMinizip_::zipOpenNewFileInZip64 (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                            uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                            const char* comment, int method, int level, int zip64)
{
    return zipOpenNewFileInZip4_64 (file, filename, zipfi, extrafield_local, size_extrafield_local, extrafield_global, size_extrafield_global,
                                    comment, method, level, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, VERSIONMADEBY, 0, zip64);
}

int PrivateMinizip_::zipOpenNewFileInZip (zipFile file, const char* filename, const zip_fileinfo* zipfi, const void* extrafield_local,
                                          uInt size_extrafield_local, const void* extrafield_global, uInt size_extrafield_global,
                                          const char* comment, int method, int level)
{
    return zipOpenNewFileInZip4_64 (file, filename, zipfi, extrafield_local, size_extrafield_local, extrafield_global, size_extrafield_global,
                                    comment, method, level, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, VERSIONMADEBY, 0, 0);
}

namespace {

    int zip64FlushWriteBuffer (zip64_internal* zi)
    {
        int err = ZIP_OK;

        if (zi->ci.encrypt != 0) {
#ifndef NOCRYPT
            uInt i;
            int  t;
            for (i = 0; i < zi->ci.pos_in_buffered_data; i++)
                zi->ci.buffered_data[i] = zencode (zi->ci.keys, zi->ci.pcrc_32_tab, zi->ci.buffered_data[i], t);
#endif
        }

        if (ZWRITE64 (zi->z_filefunc, zi->filestream, zi->ci.buffered_data, zi->ci.pos_in_buffered_data) != zi->ci.pos_in_buffered_data)
            err = ZIP_ERRNO;

        zi->ci.totalCompressedData += zi->ci.pos_in_buffered_data;

#ifdef HAVE_BZIP2
        if (zi->ci.method == Z_BZIP2ED) {
            zi->ci.totalUncompressedData += zi->ci.bstream.total_in_lo32;
            zi->ci.bstream.total_in_lo32 = 0;
            zi->ci.bstream.total_in_hi32 = 0;
        }
        else
#endif
        {
            zi->ci.totalUncompressedData += zi->ci.stream.total_in;
            zi->ci.stream.total_in = 0;
        }

        zi->ci.pos_in_buffered_data = 0;

        return err;
    }
}

int PrivateMinizip_::zipWriteInFileInZip (zipFile file, const void* buf, unsigned int len)
{
    zip64_internal* zi;
    int             err = ZIP_OK;

    if (file == NULL)
        return ZIP_PARAMERROR;
    zi = (zip64_internal*)file;

    if (zi->in_opened_file_inzip == 0)
        return ZIP_PARAMERROR;

    zi->ci.crc32 = crc32 (zi->ci.crc32, reinterpret_cast<const Bytef*> (buf), (uInt)len);

#ifdef HAVE_BZIP2
    if (zi->ci.method == Z_BZIP2ED && (!zi->ci.raw)) {
        zi->ci.bstream.next_in  = (void*)buf;
        zi->ci.bstream.avail_in = len;
        err                     = BZ_RUN_OK;

        while ((err == BZ_RUN_OK) && (zi->ci.bstream.avail_in > 0)) {
            if (zi->ci.bstream.avail_out == 0) {
                if (zip64FlushWriteBuffer (zi) == ZIP_ERRNO)
                    err = ZIP_ERRNO;
                zi->ci.bstream.avail_out = (uInt)Z_BUFSIZE;
                zi->ci.bstream.next_out  = (char*)zi->ci.buffered_data;
            }

            if (err != BZ_RUN_OK)
                break;

            if ((zi->ci.method == Z_BZIP2ED) && (!zi->ci.raw)) {
                uLong uTotalOutBefore_lo = zi->ci.bstream.total_out_lo32;
                //          uLong uTotalOutBefore_hi = zi->ci.bstream.total_out_hi32;
                err = BZ2_bzCompress (&zi->ci.bstream, BZ_RUN);

                zi->ci.pos_in_buffered_data += (uInt)(zi->ci.bstream.total_out_lo32 - uTotalOutBefore_lo);
            }
        }

        if (err == BZ_RUN_OK)
            err = ZIP_OK;
    }
    else
#endif
    {
        zi->ci.stream.next_in  = (Bytef*)(uintptr_t)buf;
        zi->ci.stream.avail_in = len;

        while ((err == ZIP_OK) && (zi->ci.stream.avail_in > 0)) {
            if (zi->ci.stream.avail_out == 0) {
                if (zip64FlushWriteBuffer (zi) == ZIP_ERRNO)
                    err = ZIP_ERRNO;
                zi->ci.stream.avail_out = (uInt)Z_BUFSIZE;
                zi->ci.stream.next_out  = zi->ci.buffered_data;
            }

            if (err != ZIP_OK)
                break;

            if ((zi->ci.method == Z_DEFLATED) && (!zi->ci.raw)) {
                uLong uTotalOutBefore = zi->ci.stream.total_out;
                err                   = deflate (&zi->ci.stream, Z_NO_FLUSH);

                zi->ci.pos_in_buffered_data += (uInt)(zi->ci.stream.total_out - uTotalOutBefore);
            }
            else {
                uInt copy_this, i;
                if (zi->ci.stream.avail_in < zi->ci.stream.avail_out)
                    copy_this = zi->ci.stream.avail_in;
                else
                    copy_this = zi->ci.stream.avail_out;

                for (i = 0; i < copy_this; i++)
                    *(((char*)zi->ci.stream.next_out) + i) = *(((const char*)zi->ci.stream.next_in) + i);
                {
                    zi->ci.stream.avail_in -= copy_this;
                    zi->ci.stream.avail_out -= copy_this;
                    zi->ci.stream.next_in += copy_this;
                    zi->ci.stream.next_out += copy_this;
                    zi->ci.stream.total_in += copy_this;
                    zi->ci.stream.total_out += copy_this;
                    zi->ci.pos_in_buffered_data += copy_this;
                }
            }
        } // while(...)
    }

    return err;
}

int PrivateMinizip_::zipCloseFileInZipRaw (zipFile file, uLong uncompressed_size, uLong crc32)
{
    return zipCloseFileInZipRaw64 (file, uncompressed_size, crc32);
}

int PrivateMinizip_::zipCloseFileInZipRaw64 (zipFile file, ZPOS64_T uncompressed_size, uLong crc32)
{
    zip64_internal* zi;
    ZPOS64_T        compressed_size;
    uLong           invalidValue = 0xffffffff;
    unsigned        datasize     = 0;
    int             err          = ZIP_OK;

    if (file == NULL)
        return ZIP_PARAMERROR;
    zi = (zip64_internal*)file;

    if (zi->in_opened_file_inzip == 0)
        return ZIP_PARAMERROR;
    zi->ci.stream.avail_in = 0;

    if ((zi->ci.method == Z_DEFLATED) && (!zi->ci.raw)) {
        while (err == ZIP_OK) {
            uLong uTotalOutBefore;
            if (zi->ci.stream.avail_out == 0) {
                if (zip64FlushWriteBuffer (zi) == ZIP_ERRNO)
                    err = ZIP_ERRNO;
                zi->ci.stream.avail_out = (uInt)Z_BUFSIZE;
                zi->ci.stream.next_out  = zi->ci.buffered_data;
            }
            uTotalOutBefore = zi->ci.stream.total_out;
            err             = deflate (&zi->ci.stream, Z_FINISH);
            zi->ci.pos_in_buffered_data += (uInt)(zi->ci.stream.total_out - uTotalOutBefore);
        }
    }
    else if ((zi->ci.method == Z_BZIP2ED) && (!zi->ci.raw)) {
#ifdef HAVE_BZIP2
        err = BZ_FINISH_OK;
        while (err == BZ_FINISH_OK) {
            uLong uTotalOutBefore;
            if (zi->ci.bstream.avail_out == 0) {
                if (zip64FlushWriteBuffer (zi) == ZIP_ERRNO)
                    err = ZIP_ERRNO;
                zi->ci.bstream.avail_out = (uInt)Z_BUFSIZE;
                zi->ci.bstream.next_out  = (char*)zi->ci.buffered_data;
            }
            uTotalOutBefore = zi->ci.bstream.total_out_lo32;
            err             = BZ2_bzCompress (&zi->ci.bstream, BZ_FINISH);
            if (err == BZ_STREAM_END)
                err = Z_STREAM_END;

            zi->ci.pos_in_buffered_data += (uInt)(zi->ci.bstream.total_out_lo32 - uTotalOutBefore);
        }

        if (err == BZ_FINISH_OK)
            err = ZIP_OK;
#endif
    }

    if (err == Z_STREAM_END)
        err = ZIP_OK; /* this is normal */

    if ((zi->ci.pos_in_buffered_data > 0) && (err == ZIP_OK)) {
        if (zip64FlushWriteBuffer (zi) == ZIP_ERRNO)
            err = ZIP_ERRNO;
    }

    if ((zi->ci.method == Z_DEFLATED) && (!zi->ci.raw)) {
        int tmp_err = deflateEnd (&zi->ci.stream);
        if (err == ZIP_OK)
            err = tmp_err;
        zi->ci.stream_initialised = 0;
    }
#ifdef HAVE_BZIP2
    else if ((zi->ci.method == Z_BZIP2ED) && (!zi->ci.raw)) {
        int tmperr = BZ2_bzCompressEnd (&zi->ci.bstream);
        if (err == ZIP_OK)
            err = tmperr;
        zi->ci.stream_initialised = 0;
    }
#endif

    if (!zi->ci.raw) {
        crc32             = (uLong)zi->ci.crc32;
        uncompressed_size = zi->ci.totalUncompressedData;
    }
    compressed_size = zi->ci.totalCompressedData;

#ifndef NOCRYPT
    compressed_size += zi->ci.crypt_header_size;
#endif

    // update Current Item crc and sizes,
    if (compressed_size >= 0xffffffff || uncompressed_size >= 0xffffffff || zi->ci.pos_local_header >= 0xffffffff) {
        /*version Made by*/
        zip64local_putValue_inmemory (zi->ci.central_header + 4, (uLong)45, 2);
        /*version needed*/
        zip64local_putValue_inmemory (zi->ci.central_header + 6, (uLong)45, 2);
    }

    zip64local_putValue_inmemory (zi->ci.central_header + 16, crc32, 4); /*crc*/

    if (compressed_size >= 0xffffffff)
        zip64local_putValue_inmemory (zi->ci.central_header + 20, invalidValue, 4); /*compr size*/
    else
        zip64local_putValue_inmemory (zi->ci.central_header + 20, compressed_size, 4); /*compr size*/

    /// set internal file attributes field
    if (zi->ci.stream.data_type == Z_ASCII)
        zip64local_putValue_inmemory (zi->ci.central_header + 36, (uLong)Z_ASCII, 2);

    if (uncompressed_size >= 0xffffffff)
        zip64local_putValue_inmemory (zi->ci.central_header + 24, invalidValue, 4); /*uncompr size*/
    else
        zip64local_putValue_inmemory (zi->ci.central_header + 24, uncompressed_size, 4); /*uncompr size*/

    // Add ZIP64 extra info field for uncompressed size
    if (uncompressed_size >= 0xffffffff)
        datasize += 8;

    // Add ZIP64 extra info field for compressed size
    if (compressed_size >= 0xffffffff)
        datasize += 8;

    // Add ZIP64 extra info field for relative offset to local file header of current file
    if (zi->ci.pos_local_header >= 0xffffffff)
        datasize += 8;

    if (datasize > 0) {
        char* p = NULL;

        if ((uLong)(datasize + 4) > zi->ci.size_centralExtraFree) {
            // we cannot write more data to the buffer that we have room for.
            return ZIP_BADZIPFILE;
        }

        p = zi->ci.central_header + zi->ci.size_centralheader;

        // Add Extra Information Header for 'ZIP64 information'
        zip64local_putValue_inmemory (p, 0x0001, 2); // HeaderID
        p += 2;
        zip64local_putValue_inmemory (p, datasize, 2); // DataSize
        p += 2;

        if (uncompressed_size >= 0xffffffff) {
            zip64local_putValue_inmemory (p, uncompressed_size, 8);
            p += 8;
        }

        if (compressed_size >= 0xffffffff) {
            zip64local_putValue_inmemory (p, compressed_size, 8);
            p += 8;
        }

        if (zi->ci.pos_local_header >= 0xffffffff) {
            zip64local_putValue_inmemory (p, zi->ci.pos_local_header, 8);
            p += 8;
        }

        // Update how much extra free space we got in the memory buffer
        // and increase the centralheader size so the new ZIP64 fields are included
        // ( 4 below is the size of HeaderID and DataSize field )
        zi->ci.size_centralExtraFree -= datasize + 4;
        zi->ci.size_centralheader += datasize + 4;

        // Update the extra info size field
        zi->ci.size_centralExtra += datasize + 4;
        zip64local_putValue_inmemory (zi->ci.central_header + 30, (uLong)zi->ci.size_centralExtra, 2);
    }

    if (err == ZIP_OK)
        err = add_data_in_datablock (&zi->central_dir, zi->ci.central_header, (uLong)zi->ci.size_centralheader);

    free (zi->ci.central_header);

    if (err == ZIP_OK) {
        // Update the LocalFileHeader with the new values.

        ZPOS64_T cur_pos_inzip = ZTELL64 (zi->z_filefunc, zi->filestream);

        if (ZSEEK64 (zi->z_filefunc, zi->filestream, zi->ci.pos_local_header + 14, ZLIB_FILEFUNC_SEEK_SET) != 0)
            err = ZIP_ERRNO;

        if (err == ZIP_OK)
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, crc32, 4); /* crc 32, unknown */

        if (uncompressed_size >= 0xffffffff || compressed_size >= 0xffffffff) {
            if (zi->ci.pos_zip64extrainfo > 0) {
                // Update the size in the ZIP64 extended field.
                if (ZSEEK64 (zi->z_filefunc, zi->filestream, zi->ci.pos_zip64extrainfo + 4, ZLIB_FILEFUNC_SEEK_SET) != 0)
                    err = ZIP_ERRNO;

                if (err == ZIP_OK) /* compressed size, unknown */
                    err = zip64local_putValue (&zi->z_filefunc, zi->filestream, uncompressed_size, 8);

                if (err == ZIP_OK) /* uncompressed size, unknown */
                    err = zip64local_putValue (&zi->z_filefunc, zi->filestream, compressed_size, 8);
            }
            else
                err = ZIP_BADZIPFILE; // Caller passed zip64 = 0, so no room for zip64 info -> fatal
        }
        else {
            if (err == ZIP_OK) /* compressed size, unknown */
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, compressed_size, 4);

            if (err == ZIP_OK) /* uncompressed size, unknown */
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, uncompressed_size, 4);
        }

        if (ZSEEK64 (zi->z_filefunc, zi->filestream, cur_pos_inzip, ZLIB_FILEFUNC_SEEK_SET) != 0)
            err = ZIP_ERRNO;
    }

    zi->number_entry++;
    zi->in_opened_file_inzip = 0;

    return err;
}

int PrivateMinizip_::zipCloseFileInZip (zipFile file)
{
    return zipCloseFileInZipRaw (file, 0, 0);
}

namespace {

    int Write_Zip64EndOfCentralDirectoryLocator (zip64_internal* zi, ZPOS64_T zip64eocd_pos_inzip)
    {
        int      err = ZIP_OK;
        ZPOS64_T pos = zip64eocd_pos_inzip - zi->add_position_when_writing_offset;

        err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)ZIP64ENDLOCHEADERMAGIC, 4);

        /*num disks*/
        if (err == ZIP_OK) /* number of the disk with the start of the central directory */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0, 4);

        /*relative offset*/
        if (err == ZIP_OK) /* Relative offset to the Zip64EndOfCentralDirectory */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, pos, 8);

        /*total disks*/    /* Do not support spawning of disk so always say 1 here*/
        if (err == ZIP_OK) /* number of the disk with the start of the central directory */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)1, 4);

        return err;
    }

    int Write_Zip64EndOfCentralDirectoryRecord (zip64_internal* zi, uLong size_centraldir, ZPOS64_T centraldir_pos_inzip)
    {
        int err = ZIP_OK;

        uLong Zip64DataSize = 44;

        err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)ZIP64ENDHEADERMAGIC, 4);

        if (err == ZIP_OK) /* size of this 'zip64 end of central directory' */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (ZPOS64_T)Zip64DataSize, 8); // why ZPOS64_T of this ?

        if (err == ZIP_OK) /* version made by */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)45, 2);

        if (err == ZIP_OK) /* version needed */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)45, 2);

        if (err == ZIP_OK) /* number of this disk */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0, 4);

        if (err == ZIP_OK) /* number of the disk with the start of the central directory */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0, 4);

        if (err == ZIP_OK) /* total number of entries in the central dir on this disk */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, zi->number_entry, 8);

        if (err == ZIP_OK) /* total number of entries in the central dir */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, zi->number_entry, 8);

        if (err == ZIP_OK) /* size of the central directory */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (ZPOS64_T)size_centraldir, 8);

        if (err == ZIP_OK) /* offset of start of central directory with respect to the starting disk number */
        {
            ZPOS64_T pos = centraldir_pos_inzip - zi->add_position_when_writing_offset;
            err          = zip64local_putValue (&zi->z_filefunc, zi->filestream, (ZPOS64_T)pos, 8);
        }
        return err;
    }

    int Write_EndOfCentralDirectoryRecord (zip64_internal* zi, uLong size_centraldir, ZPOS64_T centraldir_pos_inzip)
    {
        int err = ZIP_OK;

        /*signature*/
        err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)ENDHEADERMAGIC, 4);

        if (err == ZIP_OK) /* number of this disk */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0, 2);

        if (err == ZIP_OK) /* number of the disk with the start of the central directory */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0, 2);

        if (err == ZIP_OK) /* total number of entries in the central dir on this disk */
        {
            {
                if (zi->number_entry >= 0xFFFF)
                    err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0xffff, 2); // use value in ZIP64 record
                else
                    err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)zi->number_entry, 2);
            }
        }

        if (err == ZIP_OK) /* total number of entries in the central dir */
        {
            if (zi->number_entry >= 0xFFFF)
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0xffff, 2); // use value in ZIP64 record
            else
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)zi->number_entry, 2);
        }

        if (err == ZIP_OK) /* size of the central directory */
            err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)size_centraldir, 4);

        if (err == ZIP_OK) /* offset of start of central directory with respect to the starting disk number */
        {
            ZPOS64_T pos = centraldir_pos_inzip - zi->add_position_when_writing_offset;
            if (pos >= 0xffffffff) {
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)0xffffffff, 4);
            }
            else
                err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)(centraldir_pos_inzip - zi->add_position_when_writing_offset), 4);
        }

        return err;
    }

    int Write_GlobalComment (zip64_internal* zi, const char* global_comment)
    {
        int  err                 = ZIP_OK;
        uInt size_global_comment = 0;

        if (global_comment != NULL)
            size_global_comment = (uInt)strlen (global_comment);

        err = zip64local_putValue (&zi->z_filefunc, zi->filestream, (uLong)size_global_comment, 2);

        if (err == ZIP_OK && size_global_comment > 0) {
            if (ZWRITE64 (zi->z_filefunc, zi->filestream, global_comment, size_global_comment) != size_global_comment)
                err = ZIP_ERRNO;
        }
        return err;
    }
}

int PrivateMinizip_::zipClose (zipFile file, const char* global_comment)
{
    zip64_internal* zi;
    int             err             = 0;
    uLong           size_centraldir = 0;
    ZPOS64_T        centraldir_pos_inzip;
    ZPOS64_T        pos;

    if (file == NULL)
        return ZIP_PARAMERROR;

    zi = (zip64_internal*)file;

    if (zi->in_opened_file_inzip == 1) {
        err = zipCloseFileInZip (file);
    }

#ifndef NO_ADDFILEINEXISTINGZIP
    if (global_comment == NULL)
        global_comment = zi->globalcomment;
#endif

    centraldir_pos_inzip = ZTELL64 (zi->z_filefunc, zi->filestream);

    if (err == ZIP_OK) {
        linkedlist_datablock_internal* ldi = zi->central_dir.first_block;
        while (ldi != NULL) {
            if ((err == ZIP_OK) && (ldi->filled_in_this_block > 0)) {
                if (ZWRITE64 (zi->z_filefunc, zi->filestream, ldi->data, ldi->filled_in_this_block) != ldi->filled_in_this_block)
                    err = ZIP_ERRNO;
            }

            size_centraldir += ldi->filled_in_this_block;
            ldi = ldi->next_datablock;
        }
    }
    free_linkedlist (&(zi->central_dir));

    pos = centraldir_pos_inzip - zi->add_position_when_writing_offset;
    if (pos >= 0xffffffff || zi->number_entry >= 0xFFFF) {
        ZPOS64_T Zip64EOCDpos = ZTELL64 (zi->z_filefunc, zi->filestream);
        Write_Zip64EndOfCentralDirectoryRecord (zi, size_centraldir, centraldir_pos_inzip);

        Write_Zip64EndOfCentralDirectoryLocator (zi, Zip64EOCDpos);
    }

    if (err == ZIP_OK)
        err = Write_EndOfCentralDirectoryRecord (zi, size_centraldir, centraldir_pos_inzip);

    if (err == ZIP_OK)
        err = Write_GlobalComment (zi, global_comment);

    if (ZCLOSE64 (zi->z_filefunc, zi->filestream) != 0)
        if (err == ZIP_OK)
            err = ZIP_ERRNO;

#ifndef NO_ADDFILEINEXISTINGZIP
    free (zi->globalcomment);
#endif
    free (zi);

    return err;
}

int PrivateMinizip_::zipRemoveExtraInfoBlock (char* pData, int* dataLen, short sHeader)
{
    char* p    = pData;
    int   size = 0;
    char* pNewHeader;
    char* pTmp;
    short header;
    short dataSize;

    int retVal = ZIP_OK;

    if (pData == NULL || dataLen == NULL || *dataLen < 4)
        return ZIP_PARAMERROR;

    pNewHeader = (char*)ALLOC ((unsigned)*dataLen);
    pTmp       = pNewHeader;

    while (p < (pData + *dataLen)) {
        header   = *(short*)p;
        dataSize = *(((short*)p) + 1);

        if (header == sHeader) // Header found.
        {
            p += dataSize + 4; // skip it. do not copy to temp buffer
        }
        else {
            // Extra Info block should not be removed, So copy it to the temp buffer.
            memcpy (pTmp, p, dataSize + 4);
            p += dataSize + 4;
            size += dataSize + 4;
        }
    }

    if (size < *dataLen) {
        // clean old extra info block.
        memset (pData, 0, *dataLen);

        // copy the new extra info block over the old
        if (size > 0)
            memcpy (pData, pNewHeader, size);

        // set the new extra info size
        *dataLen = size;

        retVal = ZIP_OK;
    }
    else
        retVal = ZIP_ERRNO;

    free (pNewHeader);

    return retVal;
}

/// END OF zip.c
