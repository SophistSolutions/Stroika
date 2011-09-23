/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../../StroikaPreComp.h"

#if	qPlatform_Windows
	#include	<atlbase.h>

	#include	<Windows.h>
	#include	<URLMon.h>
#endif

#include	"../../../Characters/Format.h"
#include	"../../../Execution/Exceptions.h"

#include	"Client.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::Network;
using	namespace	Stroika::Foundation::IO::Network::Transfer;




vector<Byte>	Transfer::Fetch (const wstring& url)
{
	return vector<Byte> ();	//tmphack
}
