/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Support_InstrumentHelpers_h_
#define _Stroika_Framework_SystemPerformance_Support_InstrumentHelpers_h_ 1

#include "../../StroikaPreComp.h"

#include <memory>
#include <optional>

#include "../../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include "../../../Foundation/Execution/Synchronized.h"
#include "../../../Foundation/Time/Realtime.h"

#include "../Instrument.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 */

namespace Stroika::Frameworks::SystemPerformance::Support {

    /**
     *  Optional utility used internally in building Instruments - no need to use it.
     */
    template <typename OPTIONS>
    struct CapturerWithContext_COMMON : Foundation::Debug::AssertExternallySynchronizedLock {
    protected:
        const OPTIONS _fOptions;

    protected:
        struct _Context : Instrument::ICaptureContext {
            optional<DurationSecondsType> fCaptureContextAt{};
        };

    protected:
        Foundation::Execution::Synchronized<shared_ptr<_Context>> _fContext;

    protected:
        CapturerWithContext_COMMON (const OPTIONS& options, const shared_ptr<_Context>& context = make_shared<_Context> ());

    protected:
        /**
         *  use with scoped_lock<> if you want to hold the shared_ptr for a while.
         *  Otherwise, for one-liners, scoped_lock not needed (as the lock defined in an expression will exist til then end of the line).
         */
        template <typename T_SUBCLASS>
        static const shared_ptr<T_SUBCLASS> cContextPtr (typename Foundation::Execution::Synchronized<shared_ptr<_Context>>::ReadableReference&& r);

    protected:
        template <typename T_SUBCLASS>
        static shared_ptr<T_SUBCLASS> rwContextPtr (typename Foundation::Execution::Synchronized<shared_ptr<_Context>>::WritableReference&& r);

    protected:
        optional<DurationSecondsType> _GetCaptureContextTime () const;

    protected:
    public:
        // return true iff actually capture context
        bool _NoteCompletedCapture (DurationSecondsType at = Time::GetTickCount ());
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InstrumentHelpers.inl"

#endif /*_Stroika_Framework_SystemPerformance_Support_InstrumentHelpers_h_*/
