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
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
                DbgTrace (L"Throwing FileAccessException: fFileName='%s'; FileAccessMode=%s from %s", e2Throw.GetFileName ().c_str (), Configuration::DefaultNames<IO::FileAccessMode>::k.GetName (e2Throw.GetFileAccessMode ()), Private_::GetBT_s ().c_str ());
#else
                DbgTrace (L"Throwing FileAccessException: fFileName='%s'; FileAccessMode=%s", e2Throw.GetFileName ().c_str (), Configuration::DefaultNames<IO::FileAccessMode>::k.GetName (e2Throw.GetFileAccessMode ()));
#endif
                throw e2Throw;
            }


        }



    }
}
#endif  /*_Stroika_Foundation_IO_FileAccessException_inl_*/
