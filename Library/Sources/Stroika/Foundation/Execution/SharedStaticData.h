/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SharedStaticData_h_
#define _Stroika_Foundation_Execution_SharedStaticData_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Execution/SpinLock.h"
#include "Stroika/Foundation/Memory/Common.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   See about static buffer style from ModuleInit - so no NEW operation!
 *
 *      @todo   use static inline members, but right now that causes VS2k17 compiler to crash
 * 
 *      @todo   Need example use somewhere, and document why this instaed of c++ magic initializers... (and if not good reason, deprecate)
 */

namespace Stroika::Foundation::Execution {

    /**
     *  SharedStaticData<T> is used to safely create a copy of static data shared among various users
     *  in a thread safe way, and where the shared data goes away when the last reference does, and where
     *  the shared data is lazy constructed.
     *
     *  This is very similar to have a single static variable of type T, except that instead of
     *  having T constructed at global execution time, and destroyed at global object destruction time,
     *  it happens when the first owner comes into existence and when the last owner goes out of existence.
     *
     *  For example - if the shared data object creates threads, it can be a problem having this destroyed in
     *  a static (file scope) lifetime.
     *
     *  \note   This is also similar to the @see ModuleInit<> template, except that this is intended to give the
     *          user tighter control over lifetime of the shared data.
     *
     *  \note   Why use this instead of member function returning reference to local static object?
     *          Only real difference here is that this 'shared static' object will be auto-deleted
     *          when the last reference to it is destroyed (as opposed to after we start exiting main for static
     *          data member)
     *
     *          This can be important, if, for example, the shared object contains Thread objects.
     *
     *
     *  \par Example Usage (from HealthFrameWorksServer)
     *      \code
     *          class   AuditLogSink {
     *              ...
     *              private:
     *                  struct SharedData_;
     *                  Execution::SharedStaticData<SharedData_>    fSharedData_;
     *          };
     *          struct  AuditLogSink::SharedData_ {
     *              Execution::Thread::Ptr                   fCleanupThread;
     *
     *              SharedData_ ()
     *                  : fCleanupThread (Thread::New (&AuditLogSink::SimpleCleanupThread_))
     *              {
     *                  fCleanupThread.SetThreadPriority (Thread::Priority::eLowest);
     *                  fCleanupThread.SetThreadName ("AuditTrailCleanupThread"sv);
     *                  fCleanupThread.Start ();
     *              }
     *              ~SharedData_ ()
     *              {
     *                  fCleanupThread.AbortAndWaitForDone ();
     *              }
     *          };
     *      \endcode
     *
     *  So then on the first AuditLogSink construction - we construct the cleanup thread, and on the last the
     *  thread is shutdown. If these objects are all created after main, this assures the thread startup/cleanup
     *  is all done after the start of main and before it completes.
     *
     *  \note   Since this object is in its 'default initialized' state with all zeros, it is safe to use before
     *          the start of main (), and doesn't require the complicated inter-depependency managment of the
     *          @ModuleInit code.
     */
    template <typename T>
    class SharedStaticData {
    public:
        /**
         */
        SharedStaticData ();
        SharedStaticData (const SharedStaticData&) = delete;

    public:
        ~SharedStaticData ();

    public:
        SharedStaticData& operator= (const SharedStaticData&) = delete;

    public:
        /**
         *  Return value guaranteed lifetime at least as long as 'this' object.
         *
         *  Note - though THIS is fully threadsafe, use of the reference T& is only as threadsafe as T itself.
         */
        nonvirtual T&       Get ();
        nonvirtual const T& Get () const;

    private:
        // nb. use mutex instead of atomic<> because must lock sOnceObj_ at same time (block subsequent callers while constructing)
        inline static conditional_t<kSpinLock_IsFasterThan_mutex, SpinLock, mutex> sMutex_{};
        inline static unsigned int                                                 sCountUses_{};
        inline static T*                                                           sOnceObj_{};
        //                alignas (alignof (T)) byte fOnceObj_Storage_[sizeof (T)]; // avoid actual memory allocation call - since only one of these
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SharedStaticData.inl"

#endif /*_Stroika_Foundation_Execution_SharedStaticData_h_*/
