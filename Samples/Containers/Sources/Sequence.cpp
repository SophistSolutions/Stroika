/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/Trace.h"

// Not generally included, but you can include these if you want to select a particular backend implementation
#include "Stroika/Foundation/Containers/Concrete/Sequence_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_DoublyLinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_LinkedList.h"

#include "Stroika/Foundation/Debug/Assertions.h"

#include "Sequence.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

namespace {
    void SimplestSequenceTest_ ()
    {
        /*
         *  A sequence is just a collection, where each item has a well-defined (index) order. 
         *  Think of it as a SortedMapping<int,T> where the int is the index. Or maybe a bit
         *  like vector<T>, except that the 'vector' might be implemented with a different data structure, and have
         *  very different performance characteristics.
         * 
         *  For example, use Sequence_DoublyLinkedList, if you want to keep a sequential order, but don't use indexing, and care that
         *  you never need to 'pause for realloc'.
         */
        Sequence<int> s;
        s.Append (3);
        s += 4;
        for ([[maybe_unused]] int i : s) {
            Assert (i == 3 or i == 4);
        }
        Assert (s.size () == 2);
        Assert (s[0] == 3);
        Assert (s[1] == 4);
    }
}

void Samples::Containers::Sequence::RunDemo ()
{
    SimplestSequenceTest_ ();
}