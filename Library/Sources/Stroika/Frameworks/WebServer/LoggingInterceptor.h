/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_LoggingInterceptor_h_
#define _Stroika_Framework_WebServer_LoggingInterceptor_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Interceptor.h"

/*
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Add W3CLogger handler.
 *
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    /**
     *  @todo add simple utility class that writes W3C log format to file.
     */
    struct ILogHandler {
        struct MessageInstance {
            MessageInstance (Message* m, Time::TimePointSeconds startedAt);
            Message*               fMessage{};
            Time::TimePointSeconds fStartedAt{};
        };
        virtual shared_ptr<MessageInstance> Started (Message* m);
        virtual void                        Completed (const shared_ptr<MessageInstance>& messageInstance) noexcept = 0;
    };

    /**
     */
    class LoggingInterceptor : public Interceptor {
    private:
        using inherited = Interceptor;

    public:
        /**
         * @todo consider if we should allow a list of loggers to be associated with the interceptor.
         */
        LoggingInterceptor (const shared_ptr<ILogHandler>& logger);

    private:
        struct Rep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LoggingInterceptor.inl"

#endif /*_Stroika_Framework_WebServer_LoggingInterceptor_h_*/
