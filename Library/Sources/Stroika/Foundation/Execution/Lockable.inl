/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Lockable_inl_
#define _Stroika_Foundation_Execution_Lockable_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


			/*
			 ********************************************************************************
			 ********************** Execution::Lockable<BASE, LOCKTYPE> *********************
			 ********************************************************************************
			 */
            template    <typename BASE, typename LOCKTYPE>
            inline  Lockable<BASE, LOCKTYPE>::Lockable ()
                : BASE ()
                , LOCKTYPE ()
            {
            }
            template    <typename BASE, typename LOCKTYPE>
            inline  Lockable<BASE, LOCKTYPE>::Lockable (const BASE& from)
                : BASE (from)
                , LOCKTYPE ()
            {
            }
            template    <typename BASE, typename LOCKTYPE>
            inline  Lockable<BASE, LOCKTYPE>::Lockable (const Lockable<BASE, LOCKTYPE>& from)
                : BASE (from)       // intentional object slice
                , LOCKTYPE ()
            {
            }
            template    <typename BASE, typename LOCKTYPE>
            inline  const Lockable<BASE, LOCKTYPE>& Lockable<BASE, LOCKTYPE>::operator= (const BASE& rhs)
            {
                BASE&   THIS_BASE   =   *this;      // intentional object slice
                THIS_BASE = rhs;
                return *this;
            }
            template    <typename BASE, typename LOCKTYPE>
            inline  const Lockable<BASE, LOCKTYPE>& Lockable<BASE, LOCKTYPE>::operator= (const Lockable<BASE, LOCKTYPE>& rhs)
            {
                BASE&   THIS_BASE   =   *this;
                THIS_BASE = rhs;
                return *this;
            }


            namespace   Private {
                template    <typename BASE, typename LOCKTYPE = recursive_mutex>
                class   Lockable_POD : public LOCKTYPE {
                public:
                    Lockable_POD ()
                        : fBase (BASE ()) {
                    }
                    Lockable_POD (const BASE& from)
                        : fBase (from) {
                    }
                    Lockable_POD (const Lockable_POD<BASE, LOCKTYPE>& from)
                        : fBase (from) {        // intentional object slice
                    }
                    const Lockable_POD& operator= (const BASE& rhs) {
                        fBase = rhs;
                        return *this;
                    }
                    const Lockable_POD& operator= (const Lockable_POD<BASE, LOCKTYPE>& rhs) {
                        fBase = rhs;        // intentional object slice
                        return *this;
                    }
                    operator BASE () const {
                        return fBase;
                    }
                    BASE    fBase;
                };
            }
            //  Specailizations for POD types
            template    <>
            class   Lockable<bool, recursive_mutex> : public Private::Lockable_POD<bool, recursive_mutex>  {
            public:
                typedef bool    T;
                Lockable<T, recursive_mutex> ()                                                                                          {}
                Lockable<T, recursive_mutex> (T from):                                           Lockable_POD<T, recursive_mutex> (from)  {}
                Lockable<T, recursive_mutex> (const Lockable<T, recursive_mutex>& from):          Lockable_POD<T, recursive_mutex> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, recursive_mutex>& rhs)                                                  { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
            };
            template    <>
            class   Lockable<char, recursive_mutex> : public Private::Lockable_POD<char, recursive_mutex>  {
            public:
                typedef char    T;
                Lockable<T, recursive_mutex> ()                                                                                          {}
                Lockable<T, recursive_mutex> (T from):                                           Lockable_POD<T, recursive_mutex> (from)  {}
                Lockable<T, recursive_mutex> (const Lockable<T, recursive_mutex>& from):          Lockable_POD<T, recursive_mutex> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, recursive_mutex>& rhs)                                                  { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
            };
            template    <>
            class   Lockable<unsigned char, recursive_mutex> : public Private::Lockable_POD<unsigned char, recursive_mutex>  {
            public:
                typedef unsigned char   T;
                Lockable<T, recursive_mutex> ()                                                                                          {}
                Lockable<T, recursive_mutex> (T from):                                           Lockable_POD<T, recursive_mutex> (from)  {}
                Lockable<T, recursive_mutex> (const Lockable<T, recursive_mutex>& from):          Lockable_POD<T, recursive_mutex> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, recursive_mutex>& rhs)                                                  { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
            };
            template    <>
            class   Lockable<int, recursive_mutex> : public Private::Lockable_POD<int, recursive_mutex>  {
            public:
                typedef int T;
                Lockable<T, recursive_mutex> ()                                                                                          {}
                Lockable<T, recursive_mutex> (T from):                                           Lockable_POD<T, recursive_mutex> (from)  {}
                Lockable<T, recursive_mutex> (const Lockable<T, recursive_mutex>& from):          Lockable_POD<T, recursive_mutex> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, recursive_mutex>& rhs)                                                 { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
            };
            template    <>
            class   Lockable<unsigned int, recursive_mutex> : public Private::Lockable_POD<unsigned int, recursive_mutex>  {
            public:
                typedef unsigned int    T;
                Lockable<T, recursive_mutex> ()                                                                                          {}
                Lockable<T, recursive_mutex> (T from):                                           Lockable_POD<T, recursive_mutex> (from)  {}
                Lockable<T, recursive_mutex> (const Lockable<T, recursive_mutex>& from):          Lockable_POD<T, recursive_mutex> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, recursive_mutex>& rhs)                                                  { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
            };
            template    <>
            class   Lockable<double, recursive_mutex> : public Private::Lockable_POD<double, recursive_mutex>  {
            public:
                typedef double  T;
                Lockable<T, recursive_mutex> ()                                                                                          {}
                Lockable<T, recursive_mutex> (T from):                                           Lockable_POD<T, recursive_mutex> (from)  {}
                Lockable<T, recursive_mutex> (const Lockable<T, recursive_mutex>& from):          Lockable_POD<T, recursive_mutex> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, recursive_mutex>& rhs)                                                  { return Lockable_POD<T, recursive_mutex>::operator= (rhs); }
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Lockable_inl_*/
