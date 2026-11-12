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

    // its own (dynamic) initialization uses it: MSVC warns there, not just at uses
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    [[deprecated ("Since Stroika v3.0d26 use CORSOptions{}")]] inline const CORSOptions kDefault_CORSOptions{};
    DISABLE_COMPILER_MSC_WARNING_END (4996);

}
