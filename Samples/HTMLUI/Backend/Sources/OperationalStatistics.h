/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _StroikaSample_OperationalStatistics_h_
#define _StroikaSample_OperationalStatistics_h_ 1

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
        enum DBCommandType {
            eRead,
            eWrite
        };

    public:
        /**
         */
        class ProcessAPICmd;

    public:
        /**
         */
        class ProcessDBCmd;

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
                eDBRead,
                eDBWrite,
                eDBError,
            };
            Kind                   fKind;
            Time::TimePointSeconds fAt;
            Time::DurationSeconds  fDuration;
            size_t                 fLength;
        };
        Rec_ fRollingHistory_[1024]; // @todo see https://github.com/SophistSolutions/Stroika/issues/309 (STK-174) - redo as circular q when available
        size_t fNextHistory_{0}; // circular - can be < first. - first==last implies zero length q

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
    class OperationalStatisticsMgr::ProcessDBCmd {
    public:
        ProcessDBCmd (DBCommandType cmdType);
        ~ProcessDBCmd ();

    public:
        static void NoteError ();

    private:
        Rec_::Kind             fKind_;
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
            optional<float>    fMedianRunningAPITasks;
            unsigned int       fErrors{};
        };
        WSAPI fRecentAPI;

        // Sample doesn't have a database, but often apps like this will, so include some sample stats about it.
        struct DB {
            unsigned int                     fReads{};
            unsigned int                     fWrites{};
            unsigned int                     fErrors{};
            Math::CommonStatistics<Duration> fReadDurationStats;
            Math::CommonStatistics<Duration> fWriteDurationStats;
        };
        DB fRecentDB;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "OperationalStatistics.inl"

#endif /*_StroikaSample_OperationalStatistics_h_*/
