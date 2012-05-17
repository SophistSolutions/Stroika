/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_StringException_h_
#define _Stroia_Foundation_Execution_StringException_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "Exceptions.h"

/*
 * TODO:
 *          Consider if StringException should inherit from std::exception - and map its string value to 'what' char* value?
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            class   StringException {
            public:
                StringException (const wstring& reasonForError);

                // Only implemented for wstring
                template    <typename T>
                nonvirtual  T   As () const;

            private:
                wstring fError;
            };

            template    <>
            wstring StringException::As () const;

            template    <>
            void    _NoReturn_  DoThrow (const StringException& e2Throw);
        }
    }
}
#endif  /*_Stroia_Foundation_Execution_StringException_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "StringException.inl"
