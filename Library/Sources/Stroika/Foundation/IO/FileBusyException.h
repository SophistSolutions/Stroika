/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileBusyException_h_
#define _Stroika_Foundation_IO_FileBusyException_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Execution/StringException.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {


            using   Characters::String;


            class   FileBusyException : public Execution::StringException {
            public:
                FileBusyException (const String& fileName = String ());

            public:
                nonvirtual  String         GetFileName () const;

            private:
                String fFileName_;
            };


        }


        namespace   Execution {


            template    <>
            inline  void    _NoReturn_  Throw (const IO::FileBusyException& e2Throw);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "FileBusyException.inl"

#endif  /*_Stroika_Foundation_IO_FileBusyException_h_*/
