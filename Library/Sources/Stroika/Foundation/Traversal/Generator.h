/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Generator_h_
#define _Stroika_Foundation_Traversal_Generator_h_  1

#include    "../StroikaPreComp.h"

#include    "Iterator.h"
#include    "Iterable.h"



/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  TODO:
 *      @todo   See if/how DiscreteRange<> can be made simpler using this generator code.
 *
 *      @todo   I have old docs comment:
 *
 *              "Writeup details on how todo generator – as lambda-from-iteator – like I did for
 *              revision with queie and lambdas – except we have we construct new object with
 *              updstream-get-lambda and pass it to downstream one. I think that works. Try draft…."
 *
 *              I'm now not quite sure what that means. This may have been more for the FunctionApplicaiton module?
 *              But keep for a little bit to see if it makes sense when I review this code later...
 *
 *              -- LGP 2013-10-14
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            //MAKE PRIVATE
            template    <typename T, typename CURRENT_CONTEXT>
            struct   GenItWrapper_ : Iterator<T>::IRep  {
                typedef function<Memory::Optional<T>(shared_ptr<CURRENT_CONTEXT> callContext)>  FUN;

                FUN fFun_;
                shared_ptr<CURRENT_CONTEXT> fCallContext_;
                Memory::Optional<T> fCur_;
                bool fAtEnd_;
                GenItWrapper_ (FUN f)
                    : fFun_ (f)
                    , fCallContext_ (new CURRENT_CONTEXT ())
                    , fCur_ ()
                    , fAtEnd_ (false) {
                }
                virtual void    More (Memory::Optional<T>* result, bool advance) override {
                    RequireNotNull (result);
                    result->clear ();
                    if (advance) {
                        Require (not fAtEnd);
                        Memory::Optional<T> n   =   fFun_ (fCallContext_);
                        if (n.empty ()) {
                            fAtEnd_ = true;
                        }
                        else {
                            fCur_ = n;
                        }
                    }
                    if (not fAtEnd) {
                        *result = fCur_;
                    }
                }
                virtual bool    StrongEquals (const typename Iterator<T>::IRep* rhs) const override {
                    RequireNotNull (rhs);
                    AssertNotImplemented ();
                    return false;
                }
                virtual shared_ptr<typename Iterator<T>::IRep>    Clone () const override {
                    AssertNotImplemented ();
                    return nullptr;
                }
            };

            //MAKE PRIVATE
            template    <typename T, typename CURRENT_CONTEXT>
            struct   MyIteratableRep_ : Iterable<T>::_IRep  {
                typedef function<Memory::Optional<T>(shared_ptr<CURRENT_CONTEXT> callContext)>  FUN;

                FUN fFun_;

                MyIteratableRep_ (FUN f)
                    : fFun_ (f) {
                }
                virtual typename Iterable<T>::_SharedPtrIRep      Clone () const {
                    return typename Iterable<T>::_SharedPtrIRep(new GenItWrapper_<T, CURRENT_CONTEXT> (fFun_));
                }
                virtual Iterator<T>         MakeIterator () const {
                    return Iterator<T> (typename Iterator<T>::SharedIRepPtr (new DiscreteRange<T, TRAITS>::GenItWrapper_<T, CURRENT_CONTEXT> (fFun_)));
                }
                virtual size_t              GetLength () const {
                    typedef typename TRAITS::SignedDifferenceType    SignedDifferenceType;
                    ///
                    /// not a good idea, but the best we can do is iterate
                    size_t  n = 0;
                    for (auto i = begin (); i != end (); ++i) {
                        n++;
                    }
                    return n;
                }
                virtual bool                IsEmpty () const {
                    for (auto i = begin (); i != end (); ++i) {
                        return false;
                    }
                    return true;
                }
                virtual void                Apply (typename Iterable<T>::_IRep::_APPLY_ARGTYPE doToElement) const {
                    return this->_Apply (doToElement);
                }
                virtual Iterator<T>         ApplyUntilTrue (typename Iterable<T>::_IRep::_APPLYUNTIL_ARGTYPE doToElement) const {
                    return this->_ApplyUntilTrue (doToElement);
                }
            };


            /**
             */
            template    <typename T, typename CURRENT_CONTEXT>
            Iterable<T> CreateGenerator (function<Memory::Optional<T> (shared_ptr<CURRENT_CONTEXT> callContext)>)
            {
                return Iterable<T> (new MyIteratableRep_<T, CURRENT_CONTEXT> (callContext));
            }


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Generator.inl"

#endif  /*_Stroika_Foundation_Traversal_Generator_h_ */
