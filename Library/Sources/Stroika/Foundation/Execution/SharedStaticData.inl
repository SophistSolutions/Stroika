/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SharedStaticData_inl_
#define _Stroika_Foundation_Execution_SharedStaticData_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
            ********************************************************************************
            ******************************** SharedStaticData<T> ***************************
            ********************************************************************************
            */
            template    <typename T>
            T*  SharedStaticData<T>::sOnceObj_;

            template    <typename T>
            std::mutex  SharedStaticData<T>::sMutex_;

            template    <typename T>
            unsigned int    SharedStaticData<T>::sCountUses_ = 0;

            template    <typename T>
            SharedStaticData<T>::SharedStaticData ()
            {
                lock_guard<mutex> critSec (sMutex_);
                ++sCountUses_;
                if (sCountUses_ == 1) {
                    sOnceObj_ = new T ();
                }
            }

            template    <typename T>
            SharedStaticData<T>::~SharedStaticData ()
            {
                lock_guard<mutex> critSec (sMutex_);
                --sCountUses_;
                if (sCountUses_ == 0) {
                    delete sOnceObj_;
                    sOnceObj_ = nullptr;
                }
            }

            template    <typename T>
            T*  SharedStaticData<T>::Get ()
            {
                // no need to lock since no way to destroy value while 'this' object still exists
                Ensure (sCountUses_ >= 1);
                EnsureNotNull (sOnceObj_);
                return sOnceObj_;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SharedStaticData_inl_*/
