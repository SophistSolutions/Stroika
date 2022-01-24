/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
        static constexpr wstring_view kJava                   = L"Java"sv;
        static constexpr wstring_view kPDFViewer              = L"PDFViewer"sv;
        static constexpr wstring_view kPDFOCXViewer           = L"PDFOCXViewer"sv;
        static constexpr wstring_view kSAXFactory             = L"SAXFactory"sv;
        static constexpr wstring_view kIONetworkClientFactory = L"IO-Network-Transer-Client-Factory"sv;
        static constexpr wstring_view kOpenSSL                = L"OpenSSL"sv;

    public:
        RequiredComponentMissingException (const String& component);

    public:
        String fComponent;
    };

}

#endif /*_Stroika_Foundation_Execution_RequiredComponentMissingException_h_*/
