/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
/*
 * Note - used PUBLIC DOMAIN http://sourceforge.net/projects/libb64/files/libb64/libb64/libb64-1.2.src.zip/download
 * code as a starting point.
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>

#include	"../../Foundation/Containers/Common.h"
#include	"../../Foundation/DataExchangeFormat/BadFormatException.h"
#include	"../../Foundation/Debug/Assertions.h"
#include	"../../Foundation/Execution/Exceptions.h"
#include	"../../Foundation/Memory/SmallStackBuffer.h"

#include	"Main.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Memory;








/*
 ********************************************************************************
 *********************** Cryptography::EncodeBase64 *****************************
 ********************************************************************************
 */
