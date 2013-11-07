/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SharedStaticData_h_
#define _Stroika_Foundation_Execution_SharedStaticData_h_ 1

#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Configuration/Common.h"


/**
 * TODO
 *
 *      @todo   Consider how this fits with ModuleInit? Maybe have ModuleInit call / use this?
 *
 *      @todo   See about static buffer style from ModuleInit - so now NEW operation!
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
            *   SharedStaticData<T> is used to safely create a copy of static data shared among various users
            *   in a thread safe way, and where the shared data goes away when the last reference does.
            *
            *   This is very similar to have a single static variable of type T, except that instead of
            *   having T constructed at global execution time, and destroyed at global object destruction time,
            *   it happens when the first owner comes into existence and when teh last owner goes out of existance.
            *
            *   For example - if the shared data object creates threads, it can be a problem having this destroyed in
            *   a static (file scope) lifetime.
            *
            *   This is also similar to the @see ModuleInit<> template, except that this is intended to give the
            *   user tighter control over lifetime of the shared data.
            *
            */
            template <typename T>
            class   SharedStaticData {
            public:
                /*
                */
                SharedStaticData ();
                SharedStaticData (const SharedStaticData&) = delete;
            public:
                ~SharedStaticData ();

            public:
                const SharedStaticData& operator= (const SharedStaticData&) = delete;


            public:
                /**
                no need to lock since no way to destroy value while 'this' object still exists
                 return value gauranteed lifetime at least as long as 'this' object
                 */
                nonvirtual  T*  Get ();

            private:
                static  mutex           sMutex_;
                static  unsigned int    sCountUses_;
                static T*               sOnceObj_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SharedStaticData.inl"

#endif  /*_Stroika_Foundation_Execution_SharedStaticData_h_*/
