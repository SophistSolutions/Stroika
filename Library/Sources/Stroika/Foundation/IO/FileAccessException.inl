/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessException_inl_
#define _Stroika_Foundation_IO_FileAccessException_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Trace.h"
#include    "../Execution/Exceptions.h"

namespace   Stroika {
    namespace   Foundation  {
        namespace   IO {


            /*
             ********************************************************************************
             ***************************** IO::FileAccessException **************************
             ********************************************************************************
             */
            inline  String         FileAccessException::GetFileName () const
            {
                return fFileName_;
            }
            inline  FileAccessMode  FileAccessException::GetFileAccessMode () const
            {
                return fFileAccessMode_;
            }


        }
        namespace   Execution {


            template    <>
            inline  void    _NoReturn_  DoThrow (const IO::FileAccessException& e2Throw)
            {
                DbgTrace (L"Throwing FileAccessException: fFileName='%s'; FileAccessMode=%s", e2Throw.GetFileName ().c_str (), IO::Stroika_Enum_Names(FileAccessMode).GetName (e2Throw.GetFileAccessMode ()));
                throw e2Throw;
            }


        }



    }
}
#endif  /*_Stroika_Foundation_IO_FileAccessException_inl_*/
