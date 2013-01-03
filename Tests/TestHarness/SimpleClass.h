/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_SimpleClass_h_
#define _Stroika_Foundation_Tests_SimpleClass_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

// included for qIteratorsRequireNoArgConstructorForT, which should be moved or eliminated
#include    "Stroika/Foundation/Containers/Iterator.h"

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

    private:
        size_t  fValue;
        int     fConstructed;
        static  size_t  sTotalLiveObjects;

        friend  bool    operator== (const SimpleClass& lhs, const SimpleClass& rhs);
        friend  bool    operator< (const SimpleClass& lhs, const SimpleClass& rhs);
    };
    bool    operator== (const SimpleClass& lhs, const SimpleClass& rhs);
    bool    operator< (const SimpleClass& lhs, const SimpleClass& rhs);
};

#endif  /* _Stroika_Foundation_Tests_SimpleClass_h_ */
