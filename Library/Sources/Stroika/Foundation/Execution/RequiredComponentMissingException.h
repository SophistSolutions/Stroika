/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_RequiredComponentMissingException_h_
#define _Stroika_Foundation_Execution_RequiredComponentMissingException_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"

#include "Exceptions.h"

namespace Stroika::Foundation::Execution {

    using Characters::String;

    /**
     * RequiredComponentMissingException is thrown when a component is required for some operation, but is not available. This is used to indicate that the operation cannot be performed because of a missing component.   
     */
    class RequiredComponentMissingException : public Execution::Exception<runtime_error> {
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
