/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_SimpleClass_h_
#define _Stroika_Foundation_Tests_SimpleClass_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

// included for qIteratorsRequireNoArgConstructorForT, which should be moved or eliminated
#include    "Stroika/Foundation/Traversal/Iterator.h"

#include    "Stroika/Foundation/Configuration/Common.h"

namespace   Stroika {
    class   SimpleClass {
    public:
#if qIteratorsRequireNoArgContructorForT
        SimpleClass ();
#endif
        SimpleClass (size_t v);
        SimpleClass (const SimpleClass& f);
        ~SimpleClass ();

        nonvirtual  size_t  GetValue () const;
        static      size_t  GetTotalLiveCount ();

        SimpleClass operator+ (const SimpleClass& rhs) const {
            return SimpleClass (fValue + rhs.fValue);
        }

        bool    operator== (const SimpleClass& rhs) const;
        bool    operator< (const SimpleClass& rhs) const;
    private:
        size_t  fValue;
        int     fConstructed;
        static  size_t  sTotalLiveObjects;

    };


};

#endif  /* _Stroika_Foundation_Tests_SimpleClass_h_ */
