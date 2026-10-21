/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Activity_h_
#define _Stroika_Foundation_Execution_Activity_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Stack.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Execution {

    namespace Private_ {
        namespace Activities_ {
            /**
             *  \note intentionally omit virtual DTOR because then the subclasses cannot be constexpr (at least it appears so in c++17).
             *        And they are never destroyed by ptr to a base class type, so this isn't needed.
             */
            struct AsStringObj_ {
                constexpr AsStringObj_ () noexcept           = default;
                virtual Characters::String AsString () const = 0;
            };
            struct StackElt_ {
                const AsStringObj_* fActivity{};
                const StackElt_*    fPrev{};
            };
            thread_local inline const StackElt_* sTop_{nullptr};
        }
    }

    /**
     *  An Activity is typically a static const or sometimes even constexpr object which contains a description of
     *  an ongoing activity. They are generally not meant to be stored or copied, but REFERENCED with DeclareActivity
     *  to form a 'current activity stack'.
     *
     *  \par Example Usage
     *      \code
     *          static constexpr Activity  kBuildingThingy_{ "building thingy"sv };
     *          static const auto kOtherActivity = Activity<String>{  "abc" };
     *                                                                          // automatic variable activity OK as long as its lifetime longer than reference in DeclareActivity
     *          auto otherActivity = Activity<String> { "abc" + argument };    // activities can be stack based, but these cost more to define
     *          auto lazyEvalActivity = LazyEvalActivity ([&] () -> String { return argument.Repeat (5) + "x"; });
     *          // then for how to use activity object - see DeclareActivity
     *      \endcode
     *
     *  \note the intended purpose of this is to provide better 'context' for default exception messages, when exceptions are
     *        thrown (by Execution::Throw ()). @see CaptureCurrentActivities
     */
    template <convertible_to<Characters::String> STRINGISH_T = Characters::String>
    class Activity final : public Private_::Activities_::AsStringObj_ {
    public:
        /**
         *  @todo unclear why we need to have two overloads here - but hard todo CTOR so works for arrays otherwise (see "before Stroika 3.0d21 had trouble compiling this")
         */
        constexpr Activity (const STRINGISH_T& arg)
            requires (is_array_v<STRINGISH_T>);
        constexpr Activity (const STRINGISH_T& arg)
            requires (not is_array_v<STRINGISH_T>);

        Activity (const Activity& src) = default;

    public:
        virtual Characters::String AsString () const override;

    private:
        STRINGISH_T fArg_;
    };

    /**
     */
    template <convertible_to<Characters::String> CTOR_ARG>
    Activity (const CTOR_ARG& b) -> Activity<CTOR_ARG>;

    /**
     *  When creating the activity would be expensive, just capture it in a lambda, and only convert that lambda to
     *  an actual string if/when CaptureCurrentActivities is called.
     *
     *  \note would LIKE to know how to do this as a template specialization of Activity, but haven't figured that out yet.
     * 
     *  \par Example Usage
     *      \code
     *          auto            scanningThisAddress = LazyEvalActivity ([&] () -> String { return "scanning ports on {}"_f (ia); });
     *          DeclareActivity da{&scanningThisAddress};
     *      \endcode
     */
    template <typename CTOR_ARG>
    class LazyEvalActivity final : public Private_::Activities_::AsStringObj_ {
    public:
        constexpr LazyEvalActivity (const CTOR_ARG& arg)
            requires (is_invocable_r_v<Characters::String, CTOR_ARG>);

    public:
        virtual Characters::String AsString () const override;

    private:
        Stroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE CTOR_ARG fArg_;
    };

    /**
     *  \brief Checks if CaptureCurrentActivities() would produce a non-empty stack (but faster)
     */
    bool AnyCurrentActivities ();

    /**
     *  \brief Returns a copyable preservable version of the current activities stack.
     *
     *  'render' each current activity on the current thread's activity stack as an Activity<> (so String based), and return the full
     *  list as a copyable stack of activities.
     *
     *  \note the intended purpose of this is to provide better 'context' for default exception messages, when exceptions are
     *        thrown (by Execution::Throw ()).
     *
     *  \note the TOP of the stack is the most specific activity, and the bottom of the stack is the initial, outermost task.
     */
    Containers::Stack<Activity<>> CaptureCurrentActivities ();

    /**
     *  Push the argument Activity onto the current thread's Activity stack in the constructor, and pop it off in the destructor.
     *
     *  An exception captures the activities in force when it is THROWN with Execution::Throw (), not those in force when
     *  it was constructed. So an exception built in one context (a static const, say) and thrown in another reports the
     *  context of the throw. Throw () fills in only an exception that specified no activities of its own (any Stroika
     *  Exception<>), and a bare `throw X{}` gets none. For a third-party exception, wrap it in a NestedException and
     *  Throw () that.
     *
     *  Each activity's AsString () runs at that Throw (), while its DeclareActivity is still in scope - so a
     *  LazyEvalActivity's lambda may capture by reference.
     *
     *  \par Example Usage
     *      \code
     *          static constexpr Activity   kBuildingThingy_ {"building thingy"sv };
     *          try {
     *              DeclareActivity declareActivity { &kBuildingThingy_ };
     *              doBuildThing  ();   // Execution::Throw () any Exception<>
     *          }
     *          catch (...) {
     *              String exceptionMsg = Characters::ToString (current_exception ());
     *              Assert (exceptionMsg.Contains (kBuildingThingy_.AsString ()));       // exception e while building thingy...
     *          }
     *      \endcode
     *
     *  \note   DeclareActivity objects must be destroyed in the reverse order of their construction, on the thread that
     *          constructed them (Asserted in the destructor) - so in practice automatic variables, and never one that lives
     *          across a co_await, which breaks that order even on one thread. The stack is per-thread: work handed to
     *          another thread (a thread pool task, say) does not see it.
     */
    template <typename ACTIVITY>
    class DeclareActivity {
    public:
        DeclareActivity ()                       = delete;
        DeclareActivity (const DeclareActivity&) = delete;
        /**
         *  The caller must always declare an activity object (an Activity<> or LazyEvalActivity<>) whose address can be
         *  taken, and whose lifetime exceeds that of the DeclareActivity object.
         *
         *  \note   the activity argument MAY be nullptr, in which case this does nothing.
         *
         *          This is allowed to facilitate examples like:
         *              \code
         *                  DeclareActivity declareActivity { flag ? &kSomeActivity_ : nullptr }; // so we only conditionally declare the activity
         *              \endcode
         *
         *          I considered using optional<ACTIVITY> - but tricky since we pass in pointer - and really not much point. This has practically zero
         *          overhead, and is easy enough to understand and use.
         */
        DeclareActivity (const ACTIVITY* activity) noexcept;
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
