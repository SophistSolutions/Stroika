/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SharedStaticData_h_
#define _Stroika_Foundation_Execution_SharedStaticData_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>

#include "../Configuration/Common.h"
#include "../Memory/Common.h"
#include "SpinLock.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   Consider how this fits with ModuleInit? Maybe have ModuleInit call / use this?
 *
 *      @todo   See about static buffer style from ModuleInit - so now NEW operation!
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             *  SharedStaticData<T> is used to safely create a copy of static data shared among various users
             *  in a thread safe way, and where the shared data goes away when the last reference does.
             *
             *  This is very similar to have a single static variable of type T, except that instead of
             *  having T constructed at global execution time, and destroyed at global object destruction time,
             *  it happens when the first owner comes into existence and when the last owner goes out of existance.
             *
             *  For example - if the shared data object creates threads, it can be a problem having this destroyed in
             *  a static (file scope) lifetime.
             *
             *  This is also similar to the @see ModuleInit<> template, except that this is intended to give the
             *  user tighter control over lifetime of the shared data.
             *
             *  \par Example Usage
             *      \code
             *              class   AuditLogSink {
             *                  ...
             *                  private:
             *                      struct SharedData_;
             *                      Execution::SharedStaticData<SharedData_>    fSharedData_;
             *              };
             *              struct  AuditLogSink::SharedData_ {
             *                  Execution::Thread::Ptr                   fCleanupThread;
             *
             *                  SharedData_ ()
             *                      : fCleanupThread (Thread::New (&AuditLogSink::SimpleCleanupThread_))
             *                  {
             *                      fCleanupThread.SetThreadPriority (Thread::Priority::eLowest);
             *                      fCleanupThread.SetThreadName (L"AuditTrailCleanupThread");
             *                      fCleanupThread.Start ();
             *                  }
             *                  ~SharedData_ ()
             *                  {
             *                      fCleanupThread.AbortAndWaitForDone ();
             *                  }
             *              };
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
            class [[deprecated ("as of Stroika v2.0a230 - use static member function -inline - returning static object reference - like AssertExternallySynchronizedLock::GetSharedLockMutex_")]] SharedStaticData {
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
                nonvirtual T&    Get ();
                nonvirtual const T& Get () const;

            private:
// nb. use mutex instead of atomic<> because must lock sOnceObj_ at same time (block subsequent callers while constructing)
#if qStroika_Foundation_Execution_SpinLock_IsFasterThan_mutex
                static SpinLock sMutex_;
#else
                static mutex sMutex_;
#endif
                static unsigned int sCountUses_;
                static T*           sOnceObj_;
            };

#if 0
            // NO NEED - 

            /**
            SharedStaticData_NEW<T>::Object - 
            thred safety - fully thread-safe. ONLY legal to declare as static (app lifetime) object. Its just reserves storage.

            Then use USE it - with a single paired SharedStaticData_NEW<T>::Reference. You can have as many instances of this as you want
            and they are fully threadsafe in their bookkeeping, but the underlying object they own - must be externally synchronized.

            Access to the underlying object is via SharedStaticData_NEW<T>::Reference::Get () - and the threadsafty of that underling object exactly matches that of "T"
            This class only manages doing 



            mutable Execution::SharedStaticData<mutex> fSharedLockThreadsMutex_;

            // EXACMPLY WHEN DO USE

            >   Class where you want to just use a static member, but you worry about it being initialized just in time. Inside a method, you can 
            >   use a static variable, and it will be initialized just in time. BUt if you want those just in time semantics for the object init, but want it
            >   scoped like a static variable, use this.

            >>  NOTE - why not implement this just using a function that returns the static object?
            https://stackoverflow.com/questions/185624/static-variables-in-an-inlined-function

            [..] An inline function with external linkage shall have the same address in all translation units. A static local variable in an extern inline function always refers to the same object. A string literal in an extern inline function is the same object in different translation units.

             */

            template <typename T>
            class SharedStaticData_NEW2 {

                class Reference;

                class Object {
                public:
                    /**
                    */
                    Object ()              = default;
                    Object (const Object&) = delete;

                public:
                    ~Object ()
                    {
                        Assert (fCountUses_ == 0);
                    }

                public:
                    Object& operator= (const Object&) = delete;

                private:
                    friend class Reference;

                private:
                    // nb. use mutex instead of atomic<> because must lock sOnceObj_ at same time (block subsequent callers while constructing)
#if qStroika_Foundation_Execution_SpinLock_IsFasterThan_mutex
                    SpinLock fMutex_;
#else
                    mutex fMutex_;
#endif
                    unsigned int fCountUses_{0};
                    alignas (alignof (T)) Memory::Byte fOnceObj_Storage_[sizeof (T)]; // avoid actual memory allocation call - since only one of these
                };

                class Reference {
                public:
                    /**
                    */
                    Reference (Object& sharedStaticDataObject)
                        : fSharedStaticDataObject (sharedStaticDataObject)
                    {
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (sharedStaticDataObject.fMutex_);
#else
                        auto critSec{make_unique_lock (sharedStaticDataObject.fMutex_)};
#endif
                        sharedStaticDataObject.fCountUses_++;
                        if (sharedStaticDataObject.fCountUses_ == 1) {
                            new (&sharedStaticDataObject.fOnceObj_Storage_) T ();
                        }
                    }
                    Reference (const Reference&) = delete;

                public:
                    ~Reference ()
                    {
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fSharedStaticDataObject.fMutex_);
#else
                        auto critSec{make_unique_lock (fSharedStaticDataObject.fMutex_)};
#endif
                        if (--fSharedStaticDataObject.fCountUses_ == 0) {
                            reinterpret_cast<T*> (&sharedStaticDataObject.fOnceObj_Storage_)->~T ();
                        }
                    }

                public:
                    Reference& operator= (const Reference&) = delete;

                public:
                    /**
                    *  Return value guaranteed lifetime at least as long as 'this' object.
                    *
                    *  Note - though THIS is fully threadsafe, use of the reference T& is only as threadsafe as T itself.
                    */
                    nonvirtual T& Get ()
                    {
                        Assert (fSharedStaticDataObject.fCountUses_ > 0);
                        return *(reinterpret_cast<T*> (&fSharedStaticDataObject.fOnceObj_Storage_));
                    }
                    nonvirtual const T& Get () const
                    {
                        Assert (fSharedStaticDataObject.fCountUses_ > 0);
                        return *(reinterpret_cast<T*> (&fSharedStaticDataObject.fOnceObj_Storage_));
                    }

                private:
                    Object& fSharedStaticDataObject;
                };
            };
#endif
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SharedStaticData.inl"

#endif /*_Stroika_Foundation_Execution_SharedStaticData_h_*/
