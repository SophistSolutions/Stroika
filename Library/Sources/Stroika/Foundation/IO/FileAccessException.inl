/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessException_inl_
#define _Stroika_Foundation_IO_FileAccessException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Trace.h"
#include "../Execution/Exceptions.h"

namespace Stroika {
    namespace Foundation {
        namespace IO {

            /*
             ********************************************************************************
             ***************************** IO::FileAccessException **************************
             ********************************************************************************
             */
            inline optional<String> FileAccessException::GetFileName () const
            {
                return fFileName_;
            }
            inline optional<FileAccessMode> FileAccessException::GetFileAccessMode () const
            {
                return fFileAccessMode_;
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_FileAccessException_inl_*/
