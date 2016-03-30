/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Finally_inl_
#define _Stroika_Foundation_Execution_Finally_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


#if 0
            /*
             ********************************************************************************
             ********************************* Execution::Finally ***************************
             ********************************************************************************
             */
            inline  Finally::Finally (const function<void()>& cleanupCodeBlock)
                : fCleanupCodeBlock_ (cleanupCodeBlock)
            {
            }
            template <typename FUNCTION>
            inline  Finally::Finally (FUNCTION f, typename is_function<FUNCTION>::type*) :
                Finally (function<void()>(f))
            {
            }
            inline  Finally::~Finally()
            {
                IgnoreExceptionsForCall (fCleanupCodeBlock_ ());
            }
#endif


            /*
             ********************************************************************************
             **************************** Execution::FinallySentry **************************
             ********************************************************************************
             */
            template <typename FUNCTION>
            FinallySentry<FUNCTION>::FinallySentry (FUNCTION f)
                : fCleanupCodeBlock_ (move (f))
            {
#if     !qCompilerAndStdLib_noexcept_Buggy
                static_assert (noexcept (f), "Finally block cannot throw, so caller must mark lambda argument as noexcept");
#endif
            }
            template <typename FUNCTION>
            FinallySentry<FUNCTION>::~FinallySentry ()
            {
                // no need for IgnoreExceptionsForCall if we do static_assert?? Plus, since we assert no-except, it wont do any
                // good using IgnoreExceptionsForCall () - as we would still std::terminate due to violation of noexcept rules
                fCleanupCodeBlock_ ();
            }


            /*
             ********************************************************************************
             ******************************* Execution::mkFinally ***************************
             ********************************************************************************
             */
            template <typename FUNCTION>
            inline  auto    mkFinally (FUNCTION f) -> FinallySentry<FUNCTION> {
                return { std::move (f) };
            }


            /*
             ********************************************************************************
             ******************************* Execution::Finally *****************************
             ********************************************************************************
             */
            template <typename FUNCTION>
            inline  auto    Finally (FUNCTION f) -> FinallySentry<FUNCTION> {
                return { std::move (f) };
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Finally_inl_*/
