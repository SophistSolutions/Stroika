/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
                    virtual typename Iterator<T>::SharedIRepPtr    Clone () const override
                    {
                        return typename Iterator<T>::SharedIRepPtr (new GenItWrapper_ (*this));
                    }
                    virtual IteratorOwnerID GetOwner () const override
                    {
                        /*
                         *  This return value allows any two DiscreteRange iterators (of the same type) to be compared.
                         */
                        return typeid (*this).name ();
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
                    using   MyContextData_      =   function<Memory::Optional<T>()>;
                    using   MyIteratorRep_      =   typename Private_::GenItWrapper_<T>;
                    struct  MyIterableRep_ : IterableFromIterator<T, MyIteratorRep_, MyContextData_>::_Rep {
                        using   inherited       = typename IterableFromIterator<T, MyIteratorRep_, MyContextData_>::_Rep;
                        using   _SharedPtrIRep  = typename Iterable<T>::_SharedPtrIRep;
                        DECLARE_USE_BLOCK_ALLOCATION(MyIterableRep_);
                        MyIterableRep_ (const MyContextData_& context)
                            : inherited (context)
                        {
                        }
                        virtual _SharedPtrIRep Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                        {
                            // For now - generators have no owner, so we ignore forIterableEnvelope
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
