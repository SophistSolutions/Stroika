/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_WeakSharedPtr_h_
#define _Stroika_Foundation_Memory_WeakSharedPtr_h_ 1

#include    "../StroikaPreComp.h"

#include    <list>
#include    <memory>

#include    "../Configuration/Common.h"

#include    "../Execution/CriticalSection.h"

#include    "SharedPtr.h"



/*
 *      TODO:
 *
 *          o   Test  a bit more, and assure reasonably solid.
 *
 *          o   Docs.
 *
 */


/*
 * Implementation Strategy:
 *
 *      Basically - the plan is for the SharedPtr (subtype) to maintain a list of WeakPtrs, and then
 *      when a SharedPtr refcount goes to zero - walk that list and zero-out all the pointers from the
 *      WeakSharedPtr back to the SharedPtr. A little careful locking, the the rest is easy!
 *
 *      One key to this is that there is no cost, and no 'weak ptr' functionality builtin to the SharedPtr class,
 *      unless you explicitly want it (by using WeakCapableSharedPtr<>). This COULD be viewed as an advantage
 *      (avoid cost of maintaining weak list, and avoid possible bugs due to accidental weakptr usage) or
 *      a weakness (you need a separate concept of WeakCapableSharedPtr).
 */





namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {



            namespace   Private {
                template    <typename T, typename BASE_SharedPtr_TRAITS>
                class   WeakSharedPtrRep_;
                /*
                 * This is an implementation detail of the WeakSharedPtr's 'SharedPtr' traits class.
                 * It is the extended 'Envelope' class used by the SharedPtr
                 * including info needed for the WeakSharedPtr tracking.
                 */
                template    <typename T, typename BASE_SharedPtr_TRAITS>
                struct  WeakSharedPtrEnvelope_ : BASE_SharedPtr_TRAITS::Envelope {
                    static  Execution::CriticalSection                      sCriticalSection;
                    list<WeakSharedPtrRep_<T, BASE_SharedPtr_TRAITS>*>*     fWeakSharedPtrs;
                    WeakSharedPtrEnvelope_ (T* ptr);
                    template <typename T2, typename T2_BASE_SharedPtr_TRAITS>
                    WeakSharedPtrEnvelope_ (const WeakSharedPtrEnvelope_<T2, T2_BASE_SharedPtr_TRAITS>& from);
                    bool    Decrement ();
                    void    DoDeleteCounter ();
                };
            }




            /*
             * To use the WeakSharedPtr<> - you need to construct a special SharedPtr<> with specially augmented traits.
             * These are the specially augmented traits to use on the SharedPtr<> template. The BASE_SharedPtr_TRAITS
             * are typically equal to SharedPtr_Default_Traits<T>.
             */
            template    <typename T, typename BASE_SharedPtr_TRAITS = SharedPtr_Default_Traits<T>>
        struct  WeakSharedPtrCapableSharedPtrTraits : BASE_SharedPtr_TRAITS {
                        typedef Private::WeakSharedPtrEnvelope_<T, BASE_SharedPtr_TRAITS>               Envelope;
                    };




            namespace   Private {
                template    <typename T, typename BASE_SharedPtr_TRAITS>
                class   WeakSharedPtrRep_ {
        private:
                    NO_DEFAULT_CONSTRUCTOR (WeakSharedPtrRep_);
                    NO_COPY_CONSTRUCTOR (WeakSharedPtrRep_);
                    NO_ASSIGNMENT_OPERATOR (WeakSharedPtrRep_);
        public:
                    typedef SharedPtr<T, WeakSharedPtrCapableSharedPtrTraits<T, BASE_SharedPtr_TRAITS>>   SharedPtrType;
                    WeakSharedPtrRep_ (const SharedPtrType& shared);
                    ~WeakSharedPtrRep_ ();
                    WeakSharedPtrEnvelope_<T, BASE_SharedPtr_TRAITS>    fSharedPtrEnvelope;
                    SharedPtr<T, WeakSharedPtrCapableSharedPtrTraits<T, BASE_SharedPtr_TRAITS>>   Lock ();
                };
            }




            /*
             * WeakSharedPtr<T,BASE_SharedPtr_TRAITS> is usable with ....

            TODO: DOCS...
             */
            template    <typename T, typename BASE_SharedPtr_TRAITS = SharedPtr_Default_Traits<T>>
                    class   WeakSharedPtr {
            public:
                        typedef SharedPtr<T, WeakSharedPtrCapableSharedPtrTraits<T, BASE_SharedPtr_TRAITS>>   SharedPtrType;
            public:
                        WeakSharedPtr ();
                        WeakSharedPtr (const WeakSharedPtr<T, BASE_SharedPtr_TRAITS>& from);
                        explicit WeakSharedPtr (const SharedPtrType& from);
                        nonvirtual  const WeakSharedPtr<T, BASE_SharedPtr_TRAITS>& operator= (const WeakSharedPtr<T, BASE_SharedPtr_TRAITS>& rhs);

            public:
                        /*
                            * This is how you get a 'SharedPtr' from ....
                            */
                        nonvirtual  SharedPtrType   Lock () const;

            private:
                        SharedPtr<Private::WeakSharedPtrRep_<T, BASE_SharedPtr_TRAITS>>  fRep_;
                    };





            /*
             * The builtin SharedPtr<> doesn't support automatic WeakSharedPtr tracking. You must use a specialized
             * 'WeakSharedPtrTraits' todo that. This class template takes the base.
             *
             * Note - WeakCapableSharedPtr is just a convenience. You can just use
             *          typedef SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T,BASE_SharedPtr_TRAITS>>   WeakCapableSharedPtr
             *
             *      or even
             *
             *          typedef SharedPtr<T,WeakSharedPtrCapableSharedPtrTraits<T>> WeakCapableSharedPtr
             *
             *  except that typedefs arent' allowed  as templates (maybe in C++11 with using?).
             */
            template    <typename T, typename BASE_SharedPtr_TRAITS = SharedPtr_Default_Traits<T>>
        class   WeakCapableSharedPtr : public SharedPtr<T, WeakSharedPtrCapableSharedPtrTraits<T, BASE_SharedPtr_TRAITS>> {
            private:
                typedef SharedPtr<T, WeakSharedPtrCapableSharedPtrTraits<T, BASE_SharedPtr_TRAITS>> inherited;
            public:
                        WeakCapableSharedPtr ();
                        explicit WeakCapableSharedPtr (T* from);
                        WeakCapableSharedPtr (const WeakCapableSharedPtr<T, BASE_SharedPtr_TRAITS>& from);
                        template <typename T2, typename T2_TRAITS>
                        WeakCapableSharedPtr (const WeakCapableSharedPtr<T2, T2_TRAITS>& from);
                    };


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_WeakSharedPtr_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "WeakSharedPtr.inl"
