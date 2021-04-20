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

    struct Context : Instrument::ICaptureContext {
        optional<DurationSecondsType> fCaptureContextAt{};
    };

    /**
     *  Optional utility used internally in building Instruments - no need to use it.
     */
    template <typename OPTIONS, typename CONTEXT>
    struct InstrumentRep_COMMON : Instrument::IRep, Foundation::Debug::AssertExternallySynchronizedLock {
    protected:
        using _Context = CONTEXT;

    protected:
        const OPTIONS _fOptions;

    protected:
        Foundation::Execution::Synchronized<shared_ptr<CONTEXT>> _fContext;

    protected:
        InstrumentRep_COMMON (const OPTIONS& options, const shared_ptr<CONTEXT>& context = make_shared<CONTEXT> ());

    protected:
        optional<DurationSecondsType> _GetCaptureContextTime () const;

    protected:
        // return true iff actually capture context
        bool _NoteCompletedCapture (DurationSecondsType at = Time::GetTickCount ());

    public:
        virtual shared_ptr<Instrument::ICaptureContext> GetContext () const override
        {
            EnsureNotNull (_fContext.load ());
            return _fContext.load ();
        }
        virtual void SetContext (const shared_ptr<Instrument::ICaptureContext>& context) override
        {
            _fContext.store ((context == nullptr) ? make_shared<CONTEXT> () : dynamic_pointer_cast<CONTEXT> (context));
        }
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InstrumentHelpers.inl"

#endif /*_Stroika_Framework_SystemPerformance_Support_InstrumentHelpers_h_*/
