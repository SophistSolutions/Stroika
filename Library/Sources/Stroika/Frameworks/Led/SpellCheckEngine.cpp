/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include "SpellCheckEngine.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ******************************** SpellCheckEngine ******************************
 ********************************************************************************
 */
SpellCheckEngine::~SpellCheckEngine ()
{
}

#if qDebug
/*
@METHOD:        SpellCheckEngine::Invariant_
@DESCRIPTION:   <p>Default implementaiton for calls to @'SpellCheckEngine::Invariant'. Overridden in subclasses
        to check validity of SpellCheck engine.</p>
*/
void SpellCheckEngine::Invariant_ () const
{
}
#endif
