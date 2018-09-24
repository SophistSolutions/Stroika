/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdlib>

#include "Collection.h"
#include "Mapping.h"
#include "Sequence.h"
#include "Set.h"

using namespace std;

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Samples::Containers::Collection::RunDemo ();
    Samples::Containers::Mapping::RunDemo ();
    Samples::Containers::Set::RunDemo ();
    Samples::Containers::Sequence::RunDemo ();
    return EXIT_SUCCESS;
}
