/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	<sys/stat.h>
#include	<ctime>
#include	<limits>
#include	<fstream>
#include	<fcntl.h>
#include	<cstdio>

#if		qPlatform_Windows
	#include	<aclapi.h>
	#include	<io.h>
	#include	<shlobj.h>
	#include	<windows.h>
#endif

#include	"../../Characters/Format.h"
#include	"../../Characters/StringUtils.h"
#include	"../../Execution/Exceptions.h"
#include	"../../Containers/Common.h"
#include	"../../Containers/SetUtils.h"
#include	"../../Debug/Trace.h"
#include	"../../IO/FileAccessException.h"
#include	"../../Memory/SmallStackBuffer.h"
#include	"WellKnownLocations.h"

#include	"Directory.h"
