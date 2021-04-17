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
        const OPTIONS fOptions_;
        struct _Context : Instrument::ICaptureContext {
            optional<DurationSecondsType> fCaptureContextAt{};
        };
        Foundation::Execution::Synchronized<shared_ptr<_Context>> _fContext;

        CapturerWithContext_COMMON (const OPTIONS& options, const shared_ptr<_Context>& context = make_shared<_Context> ());

        optional<DurationSecondsType> GetCaptureContextTime () const;

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
