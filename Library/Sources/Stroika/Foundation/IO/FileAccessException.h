/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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


            /**
             *  It often happens that you know a filename in one context, but call something that throws
             *  a file access exception. This helpful macro can rebild the filename, so its captured in the
             *  exception.
             *
             *  See Also:
             *      @see Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAMESONLY_HELPER
             *
             *  EXAMPLE USAGE:
             *      try {
             *          Execution::ThrowErrNoIfNegative (fFD_ = open (fileName.AsNarrowSDKString ().c_str (), O_RDONLY));
             *      }
             *      Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER(fileName, FileAccessMode::eRead);
             *
             *  \note   Design Note:
             *      We chose to use this try/catch approach rather than setting a thread_local variable with the filename
             *      in a stack-based context for performance reasons. The thread_local approach would be more econmical
             *      and look better in the TraceLog when we throw, but at the runtime cost of lots of extra runtime
             *      assignments to the thread_local string variable.
             */
#define     Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER(USEFILENAME,USEACCESSMODE) \
    catch (const Stroika::Foundation::IO::FileAccessException& e) {\
        Stroika::Foundation::Execution::DoThrow (Stroika::Foundation::IO::FileAccessException ((e.GetFileName ().empty ()? USEFILENAME:e.GetFileName ()), USEACCESSMODE));\
    }

            /**
             *  Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAMESONLY_HELPER()
             *  @see Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER
             *
             *  See Also:
             *      @see Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER
             *
             *  EXAMPLE USAGE:
             *      try {
             *          Execution::ThrowErrNoIfNegative (fFD_ = open (fileName.AsNarrowSDKString ().c_str (), O_RDONLY));
             *      }
             *      Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAMESONLY_HELPER(fileName);
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
