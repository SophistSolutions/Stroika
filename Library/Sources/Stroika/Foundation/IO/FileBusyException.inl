/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileBusyException_inl_
#define _Stroika_Foundation_IO_FileBusyException_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Trace.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            inline  String         FileBusyException::GetFileName () const
            {
                return fFileName_;
            }
        }
        namespace   Execution {
            template    <>
            inline  void    _NoReturn_  DoThrow (const IO::FileBusyException& e2Throw)
            {
                DbgTrace ("Throwing FileBusyException: fFileName = '%s'", e2Throw.GetFileName ().c_str ());
                throw e2Throw;
            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileBusyException_inl_*/
