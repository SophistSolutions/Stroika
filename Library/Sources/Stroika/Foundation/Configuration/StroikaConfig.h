/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_StroikaConfig_h_
#define _Stroika_Foundation_Configuration_StroikaConfig_h_ 1

/*
 *  For the dyanmically generated (from the configuraiton system) defines
 *      OBSOLETE/DEPRECATED AS OF STROIKA v2.1d18, EXCEPT building with visual studio, cuz I dont yet use makefiles
 */
#ifdef _MSC_VER
#include "Stroika-Current-Configuration.h"
#endif

/*
 *  For the defaults based on the above, and compiler defines etc.
 */

#include "Private/Defaults_Configuration_Common_.h"

//Out of alphabetic order because other defaults depend on qDebug
#include "Private/Defaults_Debug_Assertions_.h"

#include "Private/Defaults_Characters_StringUtils_.h"
#include "Private/Defaults_Characters_TChar_.h"
#include "Private/Defaults_CompilerAndStdLib_.h"
#include "Private/Defaults_Cryptography_SSL_.h"
#include "Private/Defaults_DataExchange_XML_Common_.h"
#include "Private/Defaults_Database_ODBCClient_.h"
#include "Private/Defaults_Debug_Trace_.h"
#include "Private/Defaults_Execution_Logging_.h"
#include "Private/Defaults_Execution_Module_.h"
#include "Private/Defaults_Execution_Threads_.h"
#include "Private/Defaults_Memory_BlockAllocated_.h"
#include "Private/Defaults_Memory_Common_.h"

#endif /*_Stroika_Foundation_Configuration_StroikaConfig_h_*/
