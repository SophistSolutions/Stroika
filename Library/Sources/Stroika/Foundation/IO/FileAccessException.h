/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessException_h_
#define _Stroika_Foundation_IO_FileAccessException_h_   1

#include    "../StroikaPreComp.h"

#include    "../Characters/TString.h"
#include    "../Execution/StringException.h"

#include    "FileAccessMode.h"


namespace   Stroika {
    namespace   Foundation {

        namespace   IO {

            using   Characters::TString;

            // This exception is thrown when a given file is opened, or creation attempted, etc. It is a failure due to
            // file (or directory) access permissions. It nearly always is the result of an operation (attempted and failed)
            // on a given file (which is usually given in the object). It also is the result of a perticular operation/access
            // failure (like read, write, or list).
            //
            // For now inherits from StringException so places that currently don't refer to this will still be caught in a
            // list of exceptions. Probably should do separate handler so can customize messages...
            class   FileAccessException : public Execution::StringException {
            public:
                FileAccessException (const TString& fileName = TString (), FileAccessMode fileAccessMode = FileAccessMode::eReadWrite_FAM);

            public:
                nonvirtual  TString         GetFileName () const;
                nonvirtual  FileAccessMode  GetFileAccessMode () const;

            private:
                TString         fFileName_;
                FileAccessMode  fFileAccessMode_;
            };


            // Use can use this utility macro to 'fill in' the filename for a block of code that could throw a FileAccessException
            // on a given filename (but where that code may not know the filename)
#define FileAccessException_FILENAME_UPDATE_HELPER(_FILENAME_,_CODE_)\
    try {\
        _CODE_;\
    }\
    catch (const IO::FileAccessException& prevFAE) {\
        if (prevFAE.GetFileName ().empty ()) {\
            Execution::DoThrow (FileAccessException (_FILENAME_, prevFAE.GetFileAccessMode ()));\
        }\
        Execution::DoReThrow ();\
    }\
     
        }
    }
}





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "FileAccessException.inl"

#endif  /*_Stroika_Foundation_IO_FileAccessException_h_*/
