/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"

#include    "Writer.h"


/*
 * Design Note:
 *
 *      o   Chose to include lineNumber etc stuff in message by default because in apps where this is not desired - (fancy gui apps) they are
 *          more likely to be the ones to override the exception mapping to message anyhow, and tune it themselves. Its the simple apps
 *          that do little but dump the string representations of a message that are more likely to want to know the line number (a bit of a guess).
 */


using   namespace   Stroika::Foundation;
