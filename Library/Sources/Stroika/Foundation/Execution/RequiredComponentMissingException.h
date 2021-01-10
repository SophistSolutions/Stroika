/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_RequiredComponentMissingException_h_
#define _Stroika_Foundation_Execution_RequiredComponentMissingException_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"

#include "Exceptions.h"

namespace Stroika::Foundation::Execution {

    using Characters::String;

    class RequiredComponentMissingException : public Execution::RuntimeErrorException<> {
    public:
        static constexpr wchar_t kJava[]                   = L"Java";
        static constexpr wchar_t kPDFViewer[]              = L"PDFViewer";
        static constexpr wchar_t kPDFOCXViewer[]           = L"PDFOCXViewer";
        static constexpr wchar_t kSAXFactory[]             = L"SAXFactory";
        static constexpr wchar_t kIONetworkClientFactory[] = L"IO-Network-Transer-Client-Factory";
        static constexpr wchar_t kOpenSSL[]                = L"OpenSSL";

    public:
        RequiredComponentMissingException (const wstring& component);

    public:
        wstring fComponent;
    };

}

#endif /*_Stroika_Foundation_Execution_RequiredComponentMissingException_h_*/
