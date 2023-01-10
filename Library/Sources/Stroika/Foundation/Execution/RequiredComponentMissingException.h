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
        static constexpr string_view kJava                   = "Java"sv;
        static constexpr string_view kPDFViewer              = "PDFViewer"sv;
        static constexpr string_view kPDFOCXViewer           = "PDFOCXViewer"sv;
        static constexpr string_view kSAXFactory             = "SAXFactory"sv;
        static constexpr string_view kIONetworkClientFactory = "IO-Network-Transer-Client-Factory"sv;
        static constexpr string_view kOpenSSL                = "OpenSSL"sv;

    public:
        RequiredComponentMissingException (const String& component);

    public:
        String fComponent;
    };

}

#endif /*_Stroika_Foundation_Execution_RequiredComponentMissingException_h_*/
