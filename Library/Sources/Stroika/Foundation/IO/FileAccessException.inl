/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
            [[noreturn]]    inline  void    Throw (const IO::FileAccessException& e2Throw)
            {
#if     qDefaultTracingOn
                Characters::String  msg =   L"Throwing FileAccessException: fFileName='" + e2Throw.GetFileName () + L"'; FileAccessMode=" + Configuration::DefaultNames<IO::FileAccessMode>::k.GetName (e2Throw.GetFileAccessMode ());
                Throw (e2Throw, msg.c_str ());
#else
                throw e2Throw;
#endif
            }


        }



    }
}
#endif  /*_Stroika_Foundation_IO_FileAccessException_inl_*/
