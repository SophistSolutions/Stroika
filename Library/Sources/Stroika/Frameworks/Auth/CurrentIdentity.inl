/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Frameworks::Auth {

    /*
     ********************************************************************************
     ************************* Auth::CurrentIdentityManager *************************
     ********************************************************************************
     */
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline CurrentIdentityManager<ID_OBJ>::Establish::Establish (const IDType& id)
    {
#if qCompilerAndStdLib_thread_local_static_inline_twice_Buggy
        Require (!sCurrent_BWA_ ());
        sCurrent_BWA_ () = id;
#else
        Require (!sCurrent_);
        sCurrent_ = id;
#endif
    }
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline CurrentIdentityManager<ID_OBJ>::Establish::~Establish ()
    {
#if qCompilerAndStdLib_thread_local_static_inline_twice_Buggy
        sCurrent_BWA_ () = IDType{};
        Ensure (!sCurrent_BWA_ ());
#else
        sCurrent_ = IDType{};
        Ensure (!sCurrent_);
#endif
    }
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline auto CurrentIdentityManager<ID_OBJ>::Get () -> IDType
    {
#if qCompilerAndStdLib_thread_local_static_inline_twice_Buggy
        return sCurrent_BWA_ ();
#else
        return sCurrent_;
#endif
    }
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline void CurrentIdentityManager<ID_OBJ>::Set (IDType id)
    {
#if qCompilerAndStdLib_thread_local_static_inline_twice_Buggy
        sCurrent_BWA_ () = id;
#else
        sCurrent_ = id;
#endif
    }
    template <IIdentityManagerCompatibleID ID_OBJ>
    inline void CurrentIdentityManager<ID_OBJ>::clear ()
    {
#if qCompilerAndStdLib_thread_local_static_inline_twice_Buggy
        sCurrent_BWA_ () = ID_OBJ{};
#else
        sCurrent_ = ID_OBJ{};
#endif
    }

}
