/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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

            //  class   Lockable
            template    <typename BASE, typename LOCKTYPE>
            inline  Lockable<BASE, LOCKTYPE>::Lockable ()
                : BASE ()
                , LOCKTYPE () {
            }
            template    <typename BASE, typename LOCKTYPE>
            inline  Lockable<BASE, LOCKTYPE>::Lockable (const BASE& from)
                : BASE (from)
                , LOCKTYPE () {
            }
            template    <typename BASE, typename LOCKTYPE>
            inline  Lockable<BASE, LOCKTYPE>::Lockable (const Lockable<BASE, LOCKTYPE>& from)
                : BASE (from)       // intentional object slice
                , LOCKTYPE () {
            }
            template    <typename BASE, typename LOCKTYPE>
            inline  const Lockable<BASE, LOCKTYPE>& Lockable<BASE, LOCKTYPE>::operator= (const BASE& rhs) {
                BASE&   THIS_BASE   =   *this;      // intentional object slice
                THIS_BASE = rhs;
                return *this;
            }
            template    <typename BASE, typename LOCKTYPE>
            inline  const Lockable<BASE, LOCKTYPE>& Lockable<BASE, LOCKTYPE>::operator= (const Lockable<BASE, LOCKTYPE>& rhs) {
                BASE&   THIS_BASE   =   *this;
                THIS_BASE = rhs;
                return *this;
            }


            namespace   Private {
                template    <typename BASE, typename LOCKTYPE = CriticalSection>
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
            class   Lockable<bool, CriticalSection> : public Private::Lockable_POD<bool, CriticalSection>  {
            public:
                typedef bool    T;
                Lockable<T, CriticalSection> ()                                                                                          {}
                Lockable<T, CriticalSection> (T from):                                           Lockable_POD<T, CriticalSection> (from)  {}
                Lockable<T, CriticalSection> (const Lockable<T, CriticalSection>& from):          Lockable_POD<T, CriticalSection> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, CriticalSection>& rhs)                                                  { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
            };
            template    <>
            class   Lockable<char, CriticalSection> : public Private::Lockable_POD<char, CriticalSection>  {
            public:
                typedef char    T;
                Lockable<T, CriticalSection> ()                                                                                          {}
                Lockable<T, CriticalSection> (T from):                                           Lockable_POD<T, CriticalSection> (from)  {}
                Lockable<T, CriticalSection> (const Lockable<T, CriticalSection>& from):          Lockable_POD<T, CriticalSection> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, CriticalSection>& rhs)                                                  { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
            };
            template    <>
            class   Lockable<unsigned char, CriticalSection> : public Private::Lockable_POD<unsigned char, CriticalSection>  {
            public:
                typedef unsigned char   T;
                Lockable<T, CriticalSection> ()                                                                                          {}
                Lockable<T, CriticalSection> (T from):                                           Lockable_POD<T, CriticalSection> (from)  {}
                Lockable<T, CriticalSection> (const Lockable<T, CriticalSection>& from):          Lockable_POD<T, CriticalSection> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, CriticalSection>& rhs)                                                  { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
            };
            template    <>
            class   Lockable<int, CriticalSection> : public Private::Lockable_POD<int, CriticalSection>  {
            public:
                typedef int T;
                Lockable<T, CriticalSection> ()                                                                                          {}
                Lockable<T, CriticalSection> (T from):                                           Lockable_POD<T, CriticalSection> (from)  {}
                Lockable<T, CriticalSection> (const Lockable<T, CriticalSection>& from):          Lockable_POD<T, CriticalSection> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, CriticalSection>& rhs)                                                  { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
            };
            template    <>
            class   Lockable<unsigned int, CriticalSection> : public Private::Lockable_POD<unsigned int, CriticalSection>  {
            public:
                typedef unsigned int    T;
                Lockable<T, CriticalSection> ()                                                                                          {}
                Lockable<T, CriticalSection> (T from):                                           Lockable_POD<T, CriticalSection> (from)  {}
                Lockable<T, CriticalSection> (const Lockable<T, CriticalSection>& from):          Lockable_POD<T, CriticalSection> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, CriticalSection>& rhs)                                                  { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
            };
            template    <>
            class   Lockable<double, CriticalSection> : public Private::Lockable_POD<double, CriticalSection>  {
            public:
                typedef double  T;
                Lockable<T, CriticalSection> ()                                                                                          {}
                Lockable<T, CriticalSection> (T from):                                           Lockable_POD<T, CriticalSection> (from)  {}
                Lockable<T, CriticalSection> (const Lockable<T, CriticalSection>& from):          Lockable_POD<T, CriticalSection> (from)  {}
                const Lockable_POD& operator= (const T& rhs)                                                                            { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
                const Lockable_POD& operator= (const Lockable<T, CriticalSection>& rhs)                                                  { return Lockable_POD<T, CriticalSection>::operator= (rhs); }
            };



        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Lockable_inl_*/
