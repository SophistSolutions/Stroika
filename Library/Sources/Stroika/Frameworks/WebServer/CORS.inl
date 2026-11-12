/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     *************************** WebServer::CORSOptions ****************************
     ********************************************************************************
     */

    /// <summary>
    //////////////////////////// DEPRECATED BELOW.../////////////////////////////
    /// </summary>

    // its own (dynamic) initialization uses it: compilers warn there, not just at uses
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    [[deprecated ("Since Stroika v3.0d26 use CORSOptions{}")]] inline const CORSOptions kDefault_CORSOptions{};
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_MSC_WARNING_END (4996);

}
