/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_ObjectForSideEffects_h_
#define _Stroika_Foundation_Common_ObjectForSideEffects_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <functional>

#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika::Foundation::Common {

    /**
     *  \todo - see if we can get template guides working and merge this into ObjectForSideEffects
     *        - with templated value for function object.
     */
    class EmptyObjectForSideEffects {
    public:
        /**
         *  \brief Sometimes you want to sequence some code around mem-initializers. SOMETIMES - if those mem-initializers take paraemters, you can
         *         squeeze this in with (fake-value-for-side-effect,real-value) arguments.
         *         But other times that doesn't work out, and this trick solves the same problem, perhaps more cleanly.
         *
         *  \par Example Usage
         *      \code
         *          struct foo { ...
         *              // Mark the activity 'constructing WSAPI webserver' as surrounding the construction of the ConnectionManager object
         *              static constexpr Activity                            kContructing_WSAPI_WebServer_{L"constructing WSAPI webserver"sv};
         *              optional<DeclareActivity<Activity<wstring_view>>>    fEstablishActivity1_{&kContructing_WSAPI_WebServer_};
         *              ConnectionManager                                    fWSConnectionMgr_;
         *              [[no_unique_address]] EmptyObjectForSideEffects fIgnore1_{[this]() { fEstablishActivity1_.reset (); }};
         *              ...
         *          };
         *      \endcode
         */
        template <typename CONSTRUCTOR_CALL>
        EmptyObjectForSideEffects (CONSTRUCTOR_CALL&& ctorCall);
    };

    /**
     */
    class ObjectForSideEffects {
    public:
        /**
         *  \brief Sometimes you want to sequence some code around mem-initializers. SOMETIMES - if those mem-initializers take paraemters, you can
         *         squeeze this in with (fake-value-for-side-effect,real-value) arguments.
         *         But other times that doesn't work out, and this trick solves the same problem, perhaps more cleanly.
         *
         *  \par Example Usage
         *      \code
         *          struct foo { ...
         *              // Mark the activity 'constructing WSAPI webserver' as surrounding the construction of the ConnectionManager object
         *              static constexpr Activity                           kContructing_WSAPI_WebServer_{L"constructing WSAPI webserver"sv};
         *              optional<DeclareActivity<Activity<wstring_view>>>   fEstablishActivity1_{&kContructing_WSAPI_WebServer_};
         *              ObjectForSideEffects                                fIgnore1_{[this]() { DbgTrace ("this far in construction mem-initializers"); }, []() { DbgTrace(L"destroying this far in mem initializers"); }};
         *              ConnectionManager                                   fWSConnectionMgr_;
         *              ...
         *          };
         *      \endcode
         */
        template <typename CONSTRUCTOR_CALL, typename DESTRUCTOR_CALL>
        ObjectForSideEffects (CONSTRUCTOR_CALL&& ctorCall, DESTRUCTOR_CALL&& dtorCall);
        ~ObjectForSideEffects ();

    private:
        function<void ()> fDestructor_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectForSideEffects.inl"

#endif /*_Stroika_Foundation_Common_ObjectForSideEffects_h_*/