/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_LazyInitialized_h_
#define _Stroika_Foundation_Execution_LazyInitialized_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <functional>
#include <mutex>
#include <optional>

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Execution {

    /**
     *  Can be used to initialize a static constant object - declared at file scope - dependent on another file-scope data object,
     *  without incurring the pain of static initialization problems (before main). Often this is not needed, if you just
     *  make the dependent objects constexpr. But sometimes you cannot do that.
     * 
     *  LazyInitialized<T> acts mostly like a T (as much as I could figure out how to) - except that its constructor is delayed
     *  until first needed.
     * 
     *  This object (at least the magic init part) - is fully internally synchronized (though other operations of T itself are in general not).
     */
    template <typename T>
    class LazyInitialized {
    public:
        /**
         */
        LazyInitialized (const function<T (void)>& ctor);
        LazyInitialized (const T& v);
        LazyInitialized (const LazyInitialized&) = default;

    public:
        nonvirtual operator T ();

    public:
        nonvirtual T*       operator->();
        nonvirtual const T* operator->() const;

    private:
        nonvirtual void doInit_ ();

    private:
        mutable once_flag   fOneFlag_;
        function<T (void)>  fCTOR_;
        mutable optional<T> fValue_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LazyInitialized.inl"

#endif /*_Stroika_Foundation_Execution_LazyInitialized_h_*/
