/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Activity_h_
#define _Stroika_Foundation_Execution_Activity_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Containers/Stack.h"

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

    namespace Private_ {
        namespace Activities_ {
            /**
             *  \note intentionally ommit virtual DTOR because then the subclasses cannot be constexpr (at least it appears so in c++17).
             *        And they are never destroyed by ptr to a baseclass type, so this isn't needed.
             */
            struct AsStringObj_ {
                constexpr AsStringObj_ () noexcept           = default;
                virtual Characters::String AsString () const = 0;
            };
            struct StackElt_ {
                const AsStringObj_* fActivity{};
                const StackElt_*    fPrev{};
            };
            thread_local inline const StackElt_* sTop_;
        }
    }

    /**
     *  An Activity is typically a static const or sometimes even constexpr object which contains a description of
     *  an ongoing activity. They are generally not meant to be stored or copied, but REFERENCED with DeclareActivity
     *  to form a 'current activity stack'.
     *
     *  \code
     *      static constexpr Activity<wstring_view>   kBuildingThingy_ {L"Building thingy"sv };
     *      static const auto kOtherActivity = Activity<String>{  L"abc" };
     *                                                                      // automatic variable activity OK as long as it's lifetime longer than reference in DeclareActivity
     *      auto otherActivity = Activity<String> { L"abc" + argument };    // activities can be stack based, but these cost more to define
     *      auto lazyEvalActivity = LazyEvalActivity ([&] () -> String { return argument.Repeat (5) + L"x"; });
     *      // then for how to use activiy - see DeclareActivity
     *  \endcode
     *
     *  \note   for now, constexpr Activity<wstring_view> kActivity;    // FAILS
     *          This is because AsStringObj_ has a virtual destructor (necesary for other types). Thats crazy
     *          becuse constexpr objects are never destroyed, so its crazy to care that they have a virtual DTOR.
     *          Still, g++ and msvc both agree on this; I think this MAYBE fixed in C++20 (not yet tested) - because
     *          there they at least allow virtual constexpr methods. That might impact (indirecly) whether an object
     *          can be constexpr (having a virtual dtor).
     */
    template <typename CTOR_ARG = Characters::String>
    class Activity;

    template <>
    class Activity<Characters::String> final : public Private_::Activities_::AsStringObj_ {
    public:
        Activity (const Characters::String& arg);

    public:
        virtual Characters::String AsString () const override;

    private:
        Characters::String fArg_;
    };
    template <>
    class Activity<wstring_view> final : public Private_::Activities_::AsStringObj_ {
    public:
        constexpr Activity (const wstring_view& arg) noexcept;

    public:
        virtual Characters::String AsString () const override;

    private:
        wstring_view fArg_;
    };

    /**
     *  When creating the activity would be expensive, just capture it in a lambda, and only convert that lambda to
     *  an actual string if/when CaptureCurrentActivities is called.
     *
     *  \note would LIKE to know how to do this as a template specialization of Activity, but haven't figured that out yet.
     */
    template <typename CTOR_ARG, enable_if_t<is_invocable_r_v<Characters::String, CTOR_ARG>>* = nullptr>
    class LazyEvalActivity final : public Private_::Activities_::AsStringObj_ {
    public:
        LazyEvalActivity (const CTOR_ARG& arg)
            : fArg_ (arg)
        {
        }

    public:
        virtual Characters::String AsString () const override
        {
            return fArg_ (); // what makes this more efficient is that we can just capture data in a lambda (by reference)
                             // and just invoke that logic during exception processing when we need to convert the activity to a string rep
        }

    private:
        CTOR_ARG fArg_;
    };

    /**
     *  \brief Returns a copyable preservable version of the current activities stack.
     *
     *  'render' each current activity on the current threads activity stack as a Activity<> (so String based), and return the full
     *  list as a copyable stack of activities.
     */
    Containers::Stack<Activity<>> CaptureCurrentActivities ();

    /**
     *  Push the argument Activty onto the current threads Activity stack in the constructor, and pop it off in the destructor.
     *
     *  \code
     *      static constexpr Activity   kBuildingThingy_ {L"Building thingy"sv };
     *      try {
     *          DeclareActivity declareActivity { &kBuildingThingy_ }
     *          doBuildThing  ();   // throw any exception (that inherits from Exception<>)
     *      }
     *      catch (...) {
     *          String exceptionMsg = Characters::ToString (current_exception ());
     *          Assert (exceptionMsg.Contains (kBuildingThingy_.AsString ());
     *      }
     *
     *  \endcode
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

    private:
        Private_::Activities_::StackElt_ fNewTopOfStackElt_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Activity.inl"

#endif /*_Stroika_Foundation_Execution_Activity_h_*/
