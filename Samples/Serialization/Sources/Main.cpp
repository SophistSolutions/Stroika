/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>

#include "Stroika/Foundation/Debug/Visualizations.h"

#include "ObjectVariantMapper.h"

using namespace std;

using namespace Stroika::Foundation;

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Samples::Serialization::ObjectVariantMapper::RunDemo ();
    return EXIT_SUCCESS;
}
