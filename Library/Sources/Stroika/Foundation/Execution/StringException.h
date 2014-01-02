/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_StringException_h_
#define _Stroia_Foundation_Execution_StringException_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "../Characters/String.h"

#include    "Exceptions.h"



/*
 * TODO:
 *          @todo   Consider if StringException should inherit from std::exception - and map its string value to 'what' char* value?
 *                  Not sure of the answer - about what our base SB, but char* should be ddefined to be a c-string in the PlatformSDK string codepage!
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             */
            class   StringException {
            public:
                StringException (const Characters::String& reasonForError);

                /**
                 * Only implemented for
                 *      o   wstring
                 *      o   String
                 */
                template    <typename T>
                nonvirtual  T   As () const;

            private:
                Characters::String fError_;
            };

            template    <>
            wstring StringException::As () const;
            template    <>
            Characters::String StringException::As () const;

            template    <>
            void    _NoReturn_  DoThrow (const StringException& e2Throw);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "StringException.inl"

#endif  /*_Stroia_Foundation_Execution_StringException_h_*/
