/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DelegatedIterator_h_
#define _Stroika_Foundation_Traversal_DelegatedIterator_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterator.h"



/**
 *  \file
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *  Handy helper to combine (or track) iterators
             */
            template    <typename T, typename EXTRA_DATA = void>
            class   DelegatedIterator : public Iterator<T> {
            public:
                struct   Rep : Iterator<T>::IRep {
                    Iterator<T> fDelegateTo;
                    EXTRA_DATA  fExtraData;
                    Rep (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData = EXTRA_DATA ());
                    virtual SharedIRepPtr   Clone () const override;
                    virtual void    More (Memory::Optional<T>* result, bool advance) override;
                    virtual bool    Equals (const IRep* rhs) const override;
                };
            };
            template    <typename T>
            class   DelegatedIterator<T, void> : public Iterator<T> {
            public:
                struct   Rep : Iterator<T>::IRep {
                    Iterator<T> fDelegateTo;
                    Rep (const Iterator<T>& delegateTo);
                    virtual SharedIRepPtr   Clone () const override;
                    virtual void    More (Memory::Optional<T>* result, bool advance) override;
                    virtual bool    Equals (const IRep* rhs) const override;
                };
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "DelegatedIterator.inl"

#endif  /*_Stroika_Foundation_Traversal_DelegatedIterator_h_ */
