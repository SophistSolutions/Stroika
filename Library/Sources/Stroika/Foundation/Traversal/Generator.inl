/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Generator_inl_
#define _Stroika_Foundation_Traversal_Generator_inl_

#include    "../Debug/Assertions.h"
#include    "IterableFromIterator.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            namespace Private_ {

                template    <typename T>
                struct   GenItWrapper_ : Iterator<T>::IRep  {
                    function<Memory::Optional<T>()> fFun_;
                    Memory::Optional<T> fCur_;
                    bool fAtEnd_;
                    GenItWrapper_ (function<Memory::Optional<T>()> f)
                        : fFun_ (f)
                        , fCur_ ()
                        , fAtEnd_ (false)
                    {
                    }
                    virtual void    More (Memory::Optional<T>* result, bool advance) override
                    {
                        RequireNotNull (result);
                        result->clear ();
                        if (advance) {
                            Require (not fAtEnd_);
                            Memory::Optional<T> n   =   fFun_ ();
                            if (n.IsMissing ()) {
                                fAtEnd_ = true;
                            }
                            else {
                                fCur_ = n;
                            }
                        }
                        if (not fAtEnd_) {
                            *result = fCur_;
                        }
                    }
                    virtual bool    Equals (const typename Iterator<T>::IRep* rhs) const override
                    {
                        RequireNotNull (rhs);
                        // No way to tell equality (some must rethink definition in Iterator<T>::Equals()!!! @todo
                        AssertMember (rhs, GenItWrapper_);
                        const GenItWrapper_&  rrhs = *dynamic_cast<const GenItWrapper_*> (rhs);
                        return fAtEnd_ == rrhs.fAtEnd_;
                    }
                    virtual shared_ptr<typename Iterator<T>::IRep>    Clone () const override
                    {
                        return shared_ptr<typename Iterator<T>::IRep> (new GenItWrapper_ (*this));
                    }
                };

#if 0
                template    <typename T>
                struct   MyIteratableRep_ : Iterable<T>::_IRep  {
                    function<Memory::Optional<T>()> fFun_;
                    MyIteratableRep_ (function<Memory::Optional<T>()> f)
                        : fFun_ (f)
                    {
                    }
                    virtual typename Iterable<T>::_SharedPtrIRep      Clone () const
                    {
                        return typename Iterable<T>::_SharedPtrIRep(new GenItWrapper_<T> (fFun_));
                    }
                    virtual Iterator<T>         MakeIterator () const
                    {
                        return Iterator<T> (typename Iterator<T>::SharedIRepPtr (new GenItWrapper_<T> (fFun_)));
                    }
                    virtual size_t              GetLength () const
                    {
                        typedef typename TRAITS::SignedDifferenceType    SignedDifferenceType;
                        ///
                        /// not a good idea, but the best we can do is iterate
                        size_t  n = 0;
                        for (auto i = begin (); i != end (); ++i) {
                            n++;
                        }
                        return n;
                    }
                    virtual bool                IsEmpty () const
                    {
                        for (auto i = begin (); i != end (); ++i) {
                            return false;
                        }
                        return true;
                    }
                    virtual void                Apply (typename Iterable<T>::_IRep::_APPLY_ARGTYPE doToElement) const
                    {
                        this->_Apply (doToElement);
                    }
                    virtual Iterator<T>         ApplyUntilTrue (typename Iterable<T>::_IRep::_APPLYUNTIL_ARGTYPE doToElement) const
                    {
                        return this->_ApplyUntilTrue (doToElement);
                    }
                };
#endif


            }


            /**
             */
            template    <typename T>
            Iterator<T> CreateGeneratorIterator (const function<Memory::Optional<T>()>& getNext)
            {
                return Iterator<T> (typename Iterator<T>::SharedIRepPtr (new Private_::GenItWrapper_<T> (getNext)));
            }


            /**
             */
            template    <typename T>
            Iterable<T> CreateGenerator (const function<Memory::Optional<T>()>& getNext)
            {
#if 1
                struct  MyIterable_ : Iterable<T> {
                    typedef function<Memory::Optional<T>()>         MyContextData_;
                    typedef typename Private_::GenItWrapper_<T>     MyIteratorRep_;
                    struct MyIterableRep_ : Traversal::IterableFromIterator<T, MyIteratorRep_, MyContextData_>::_Rep {
                        using   inherited = typename Traversal::IterableFromIterator<T, MyIteratorRep_, MyContextData_>::_Rep;
                        DECLARE_USE_BLOCK_ALLOCATION(MyIterableRep_);
                        MyIterableRep_ (const MyContextData_& context)
                            : inherited (context)
                        {
                        }
                        virtual typename Iterable<T>::_SharedPtrIRep Clone () const override
                        {
                            return typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (*this));
                        }
                    };
                    MyIterable_ (const function<Memory::Optional<T>()>& getNext)
                        : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (getNext)))
                    {
                    }
                };
                return MyIterable_<T> (getNext);
#else
                return Iterable<T> (new Private_::MyIteratableRep_<T> (getNext));
#endif
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Generator_inl_ */
