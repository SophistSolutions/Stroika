/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_StroikaVersion_h_
#define _Stroika_Foundation_Common_StroikaVersion_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if defined(__cplusplus)
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Version.h"
#endif
#include "Stroika/Foundation/Common/VersionDefs.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Release">Release</a>
 */

#if defined(__cplusplus)
namespace Stroika::Foundation::Common {
#endif

#include "Stroika-Current-Version.h"

    /**
     */
#if defined(__cplusplus)
    constexpr Version kStroikaVersion{kStroika_Version_FullVersion};
#endif

#if defined(__cplusplus)
}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StroikaVersion.inl"

#endif /*_Stroika_Foundation_Common_StroikaVersion_h_*/
