/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileFormatException_inl_
#define _Stroika_Foundation_IO_FileFormatException_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {

            inline  String         FileFormatException::GetFileName () const
            {
                return fFileName_;
            }
        }

        namespace   Execution {
            template    <>
            inline  void    _NoReturn_  DoThrow (const IO::FileFormatException& e2Throw)
            {
                DbgTrace ("Throwing FileFormatException: fFileName = '%s'", e2Throw.GetFileName ().c_str ());
                throw e2Throw;
            }
        }

    }
}
#endif  /*_Stroika_Foundation_IO_FileFormatException_inl_*/
