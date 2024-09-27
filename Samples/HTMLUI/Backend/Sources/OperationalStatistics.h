/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _RFL_AskHealthFrame_OperationalStatistics_h_
#define _RFL_AskHealthFrame_OperationalStatistics_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Time/Duration.h"

/**
 *
 */

namespace Stroika::Samples::HTMLUI {

    using namespace Stroika;
    using namespace Stroika::Foundation;

    using Time::Duration;

    /**
     *  Fully internally synchronized.
     *
     *  Simple API to track recent application statistics.
     */
    class OperationalStatisticsMgr {
    public:
        static OperationalStatisticsMgr sThe;

    public:
        static inline const Time::Duration kLookbackInterval{5min};

    public:
        /**
         */
        class ProcessAPICmd;

    public:
        nonvirtual void RecordActiveRunningTasksCount (size_t length);

    public:
        nonvirtual void RecordOpenConnectionCount (size_t length);

    public:
        nonvirtual void RecordProcessingConnectionCount (size_t length);

    public:
        struct Statistics;

    public:
        /**
         */
        nonvirtual Statistics GetStatistics () const;

    private:
        mutable mutex fMutex_; // protect all data with single quick access mutex
        struct Rec_ {
            enum class Kind {
                eNull,
                eAPI,
                eAPIError,
                eAPIActiveRunningTasks,
                eAPIOpenConnectionCount,
                eAPIProcessingConnectionCount,
            };
            Kind                   fKind;
            Time::TimePointSeconds fAt;
            Time::DurationSeconds  fDuration;
            size_t                 fLength;
        };
        Rec_   fRollingHistory_[1024]; // @todo see https://stroika.atlassian.net/browse/STK-174 - redo as circular q when available
        size_t fNextHistory_{0};       // circular - can be < first. - first==last implies zero length q

        void Add_ (const Rec_& r);
    };
    inline OperationalStatisticsMgr OperationalStatisticsMgr::sThe;

    /**
     */
    class OperationalStatisticsMgr::ProcessAPICmd {
    public:
        ProcessAPICmd ();
        ~ProcessAPICmd ();

    public:
        static void NoteError ();

    private:
        Time::TimePointSeconds fStart_;
    };

    /**
     */
    struct OperationalStatisticsMgr::Statistics {
        struct WSAPI {
            unsigned int       fCallsCompleted{};
            optional<Duration> fMeanDuration;
            optional<Duration> fMedianDuration;
            optional<Duration> fMaxDuration;
            optional<float>    fMedianWebServerConnections;
            optional<float>    fMedianProcessingWebServerConnections;
            optional<float>    fMedianRunningAPITasks;
            unsigned int       fErrors{};
        };

        WSAPI fRecentAPI;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "OperationalStatistics.inl"

#endif /*_RFL_AskHealthFrame_OperationalStatistics_h_*/
