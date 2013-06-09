/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_StringException_inl_
#define _Stroia_Foundation_Execution_StringException_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Trace.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ******************************** StringException *******************************
             ********************************************************************************
             */
            inline  StringException::StringException (const wstring& reasonForError)
                : fError_ (reasonForError)
            {
            }
            template    <>
            inline  wstring StringException::As () const
            {
                return fError_;
            }
            template    <>
            inline  Characters::String StringException::As () const
            {
                return fError_;
            }


            template    <>
            inline  void    _NoReturn_  DoThrow (const StringException& e2Throw)
            {
                DbgTrace (L"Throwing StringException: '%s'", e2Throw.As<wstring> ().substr (0, 20).c_str ());
                throw e2Throw;
            }


        }
    }
}
#endif  /*_Stroia_Foundation_Execution_StringException_inl_*/
