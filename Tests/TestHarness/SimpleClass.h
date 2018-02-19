/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_SimpleClass_h_
#define _Stroika_Foundation_Tests_SimpleClass_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

// included for qIteratorsRequireNoArgConstructorForT, which should be moved or eliminated
#include "Stroika/Foundation/Traversal/Iterator.h"

#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika {
    class SimpleClass {
    public:
        SimpleClass (size_t v);
        SimpleClass (const SimpleClass& f);
        ~SimpleClass ();

        nonvirtual size_t GetValue () const;
        static size_t     GetTotalLiveCount ();

        SimpleClass operator+ (const SimpleClass& rhs) const
        {
            return SimpleClass (fValue + rhs.fValue);
        }
        explicit operator size_t () const { return fValue; }

        bool operator== (const SimpleClass& rhs) const;
        bool operator< (const SimpleClass& rhs) const;

    private:
        size_t        fValue;
        int           fConstructed;
        static size_t sTotalLiveObjects;
    };

    class SimpleClassWithoutComparisonOperators {
    public:
        SimpleClassWithoutComparisonOperators (size_t v);
        SimpleClassWithoutComparisonOperators (const SimpleClassWithoutComparisonOperators& f);
        ~SimpleClassWithoutComparisonOperators ();

        nonvirtual size_t GetValue () const;
        static size_t     GetTotalLiveCount ();

        explicit operator size_t () const { return fValue; }

        SimpleClassWithoutComparisonOperators operator+ (const SimpleClassWithoutComparisonOperators& rhs) const
        {
            return SimpleClassWithoutComparisonOperators (fValue + rhs.fValue);
        }
#if qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy
        bool operator== (const SimpleClassWithoutComparisonOperators& rhs) const
        {
            AssertNotReached (); // NOTE - NEVER DEFINED - NEVER ACTUALLY called, but sometimes linked
            return false;
        }
        bool operator< (const SimpleClassWithoutComparisonOperators& rhs) const
        {
            AssertNotReached (); // NOTE - NEVER DEFINED - NEVER ACTUALLY called, but sometimes linked
            return false;
        }
#endif

    private:
        size_t        fValue;
        int           fConstructed;
        static size_t sTotalLiveObjects;
    };
};

#endif /* _Stroika_Foundation_Tests_SimpleClass_h_ */
