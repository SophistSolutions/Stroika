/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ***************************** WebServer::Connection ****************************
     ********************************************************************************
     */

}
namespace Stroika::Foundation::Common {
    template <>
    constexpr EnumNames<Frameworks::WebServer::Connection::Stats::State> DefaultNames<Frameworks::WebServer::Connection::Stats::State>::k{{{
        {Frameworks::WebServer::Connection::Stats::State::eNew, L"New"},
        {Frameworks::WebServer::Connection::Stats::State::eReadingHeaders, L"ReadingHeaders"},
        {Frameworks::WebServer::Connection::Stats::State::ePausedIncompleteHeaders, L"PausedIncompleteHeaders"},
        {Frameworks::WebServer::Connection::Stats::State::eProcessingInterceptorChain, L"ProcessingInterceptorChain"},
        {Frameworks::WebServer::Connection::Stats::State::eFlushing, L"Flushing"},
        {Frameworks::WebServer::Connection::Stats::State::eReadyForNextMessage, L"ReadyForNextMessage"},
        {Frameworks::WebServer::Connection::Stats::State::eClosing, L"Closing"},
    }}};
}
