/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Activity_h_
#define _Stroika_Foundation_Execution_Activity_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Containers/Sequence.h"

/*
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 *
 */

namespace Stroika::Foundation::Execution {

    /**
     */
#if 1
    template <typename CTOR_ARG = Characters::String>
    class Activity;
#else
    template <typename CTOR_ARG = Characters::String>
    class Activity : Activity<Characters::String> {
    public:
        Activity (const CTOR_ARG& arg);

    public:
        nonvirtual explicit operator Characters::String () const;

    private:
        CTOR_ARG fArg_;
    };
#endif

    namespace Private_ {
        namespace Activities_ {
            struct AsStringObj_ {
                virtual Characters::String AsString () const = 0;
            };
            struct StackElt_ {
                AsStringObj_* fActivity{};
                StackElt_*    fPrev{};
            };
            static thread_local inline StackElt_* sTop_;
        }
    }

    template <>
    class Activity<Characters::String> : public Private_::Activities_::AsStringObj_ {
    public:
        Activity (const Characters::String& arg);

    public:
        virtual Characters::String AsString () const override;

    private:
        Characters::String fArg_;
    };

    template <>
    class Activity<wstring_view> : public Private_::Activities_::AsStringObj_ {
    public:
        constexpr Activity (const wstring_view& arg);

    public:
        virtual Characters::String AsString () const override;

    private:
        wstring_view fArg_;
    };

    /**
     *  When creating the activity would be expensive, just capture it in a lambda, and only convert that lambda to
     *  an actual string if/when CaptureCurrentActivities is called
     */
    template <typename CTOR_ARG, enable_if_t<is_invocable_r_v<Characters::String, CTOR_ARG>>* = nullptr>
    class LazyEvalActivity : public Private_::Activities_::AsStringObj_ {
    public:
        LazyEvalActivity (const CTOR_ARG& arg);

    public:
        virtual explicit operator Characters::String () const override;

    private:
        CTOR_ARG fArg_;
    };

    /**
     *  'render' each current activity on the current threads activity stack as a Activity<> (so String based), and return the full
     *  list as a copyable sequence.
     */
    Containers::Sequence<Activity<>> CaptureCurrentActivities ();

    /**
     *  Push the argument Activty onto the current threads Activity stack in the constructor, and pop it off in the destructor.
     */
    template <typename ACTIVITY>
    class DeclareActivity {
    public:
        /**
         *  The caller must always declare an Activity object whose address can be taken, and whose lifetime exceeds that of the DeclareActivity object.
         */
        DeclareActivity ()                       = delete;
        DeclareActivity (const DeclareActivity&) = delete;
        DeclareActivity (const ACTIVITY* arg);
        ~DeclareActivity ();
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Activity.inl"

#endif /*_Stroika_Foundation_Execution_Activity_h_*/
