/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileFormatException_h_
#define _Stroika_Foundation_IO_FileFormatException_h_   1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Execution/StringException.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {


            using   Characters::String;


            class   FileFormatException : public Execution::StringException {
            public:
                FileFormatException (const String& fileName);

            public:
                nonvirtual  String         GetFileName () const;

            private:
                String fFileName_;
            };


        }
        namespace   Execution {


            // Just pre-declare DoThrow><> template here so we can specailize
            template    <typename T>
            void     _NoReturn_ DoThrow (const T& e2Throw);
            template    <>
            void    _NoReturn_  DoThrow (const IO::FileFormatException& e2Throw);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "FileFormatException.inl"

#endif  /*_Stroika_Foundation_IO_FileFormatException_h_*/
