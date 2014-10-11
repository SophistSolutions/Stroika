/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_StringException_h_
#define _Stroia_Foundation_Execution_StringException_h_ 1

#include    "../StroikaPreComp.h"

#include    <exception>
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
             *  This takes a 'String' argument, and maps it to the 'what()' in std::exception.
             *  This maps using the default native SDK characterset.
             */
            class   StringException : public std::exception {
            private:
                using   inherited = exception;

            public:
                StringException (const Characters::String& reasonForError);

            public:
                /**
                 * Only implemented for
                 *      o   wstring
                 *      o   String
                 */
                template    <typename T>
                nonvirtual  T   As () const;

            public:
                /**
                 *  Provide a 'c string' variant of the exception message. Convert the UNICODE
                 *  string argument to a narrow-string (multibyte) in the SDK code page.
                 *  @see GetDefaultSDKCodePage()
                 */
                virtual const char* what () const noexcept override;

            private:
                Characters::String  fError_;
                string              fSDKCharString_;
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
