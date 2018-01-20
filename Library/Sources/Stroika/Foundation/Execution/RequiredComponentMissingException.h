/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_RequiredComponentMissingException_h_
#define _Stroika_Foundation_Execution_RequiredComponentMissingException_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"

#include "StringException.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            using Characters::String;

            class RequiredComponentMissingException : public StringException {
            public:
                static const wchar_t kJava[];
                static const wchar_t kPDFViewer[];
                static const wchar_t kPDFOCXViewer[];
                static const wchar_t kSAXFactory[];
                static const wchar_t kIONetworkClientFactory[];
                static const wchar_t kOpenSSL[];

            public:
                RequiredComponentMissingException (const wstring& component);

            public:
                wstring fComponent;
            };
        }
    }
}

#endif /*_Stroika_Foundation_Execution_RequiredComponentMissingException_h_*/
