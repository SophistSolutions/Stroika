/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessException_h_
#define _Stroika_Foundation_IO_FileAccessException_h_   1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Execution/StringException.h"

#include    "FileAccessMode.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {


            using   Characters::String;


            /**
             * This exception is thrown when a given file is opened, or creation attempted, etc. It is a failure due to
             * file (or directory) access permissions. It nearly always is the result of an operation (attempted and failed)
             * on a given file (which is usually given in the object). It also is the result of a perticular operation/access
             * failure (like read, write, or list).
             *
             * For now inherits from StringException so places that currently don't refer to this will still be caught in a
             * list of exceptions. Probably should do separate handler so can customize messages...
             */
            class   FileAccessException : public Execution::StringException {
            public:
                FileAccessException (const String& fileName = String (), FileAccessMode fileAccessMode = FileAccessMode::eReadWrite);

            public:
                nonvirtual  String          GetFileName () const;
                nonvirtual  FileAccessMode  GetFileAccessMode () const;

            private:
                String          fFileName_;
                FileAccessMode  fFileAccessMode_;
            };

#if 0
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
     
#endif

            /**
             *  It often happens that you know a filename in one context, but call something that throws
             *  a file access exception. This helpful macro can rebild the filename, so its captured in the
             *  exception.
             */
#define     Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER(USEFILENAME,USEACCESSMODE) \
    catch (const Stroika::Foundation::IO::FileAccessException& e) {\
        Stroika::Foundation::Execution::DoThrow (Stroika::Foundation::IO::FileAccessException ((e.GetFileName ().empty ()? USEFILENAME:e.GetFileName ()), USEACCESSMODE));\
    }

            /**
             *  Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAMESONLY_HELPER()
             *  @see Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER
             */
#define     Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAMESONLY_HELPER(USEFILENAME) \
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER(USEFILENAME,e.GetFileAccessMode())


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
