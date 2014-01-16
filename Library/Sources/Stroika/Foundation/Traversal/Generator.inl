/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Generator_inl_
#define _Stroika_Foundation_Traversal_Generator_inl_

#include    "../Debug/Assertions.h"
#include    "IterableFromIterator.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            namespace Private_ {

                // @todo - I think we can lose fAtEnd and use the fCur.IsMissing/IsPresent()
                template    <typename T>
                struct   GenItWrapper_ : Iterator<T>::IRep  {
                    using   OwnerID =   typename Iterator<T>::OwnerID;
                    function<Memory::Optional<T>()> fFun_;
                    Memory::Optional<T> fCur_;
                    bool                fAtEnd_;
                    GenItWrapper_ (function<Memory::Optional<T>()> f)
                        : fFun_ (f)
                        , fCur_ ()
                        , fAtEnd_ (false)
                    {
                        fCur_   =   fFun_ ();
                        if (fCur_.IsMissing ()) {
                            fAtEnd_ = true;
                        }
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
                            Assert (fCur_.IsPresent ());
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
                    virtual OwnerID GetOwner () const override
                    {
                        //tmphack but adequate
                        // should NOT require locking is readonly immutable value provided at construction
                        return nullptr;
                    }
                };

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
                struct  MyIterable_ : Iterable<T> {
                    typedef function<Memory::Optional<T>()>         MyContextData_;
                    typedef typename Private_::GenItWrapper_<T>     MyIteratorRep_;
                    struct MyIterableRep_ : Traversal::IterableFromIterator<T, MyIteratorRep_, MyContextData_>::_Rep {
                        using   inherited       = typename Traversal::IterableFromIterator<T, MyIteratorRep_, MyContextData_>::_Rep;
                        using   _SharedPtrIRep  = typename Iterable<T>::_SharedPtrIRep;
                        DECLARE_USE_BLOCK_ALLOCATION(MyIterableRep_);
                        MyIterableRep_ (const MyContextData_& context)
                            : inherited (context)
                        {
                        }
                        virtual _SharedPtrIRep Clone () const override
                        {
                            return _SharedPtrIRep (new MyIterableRep_ (*this));
                        }
                    };
                    MyIterable_ (const function<Memory::Optional<T>()>& getNext)
                        : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (getNext)))
                    {
                    }
                };
                return MyIterable_ (getNext);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Generator_inl_ */
