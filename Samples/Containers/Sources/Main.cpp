/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdlib>

#include "Collection.h"

using namespace std;

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Samples::Containers::Collection::RunTests ();
    return EXIT_SUCCESS;
}
