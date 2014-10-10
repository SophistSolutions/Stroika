/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if		qPlatform_POSIX
#include	<unistd.h>
#elif	qPlatform_Windows
#include	<Windows.h>
#endif

#include    "../Characters/SDKString.h"
#if     qPlatform_POSIX
#include    "../Execution/ErrNoException.h"
#elif	qPlatform_Windows
#include    "../Execution/Platform/Windows/Exception.h"
#endif
#include	"../Memory/SmallStackBuffer.h"

#include    "ComputerName.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Configuration;


using	Characters::SDKChar;


#if     0 && qPlatform_POSIX
	// ALTERNATE APPROACH TO CONSIDER
    string  name;
    {
        struct addrinfo* res;
        struct addrinfo hints;
        memset(&hints, '\0', sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_CANONNAME;
        int e = getaddrinfo(nullptr, nullptr, &hints, &res);
        if (e != 0) {
            //printf("failure %s\n", gai_strerror (e));
            return String ();
        }
        int sock = -1;
        for (struct addrinfo* r = res; r != NULL; r = r->ai_next) {
            name = r->ai_canonname ;
            break;
        }
        freeaddrinfo(res);
    }
    return String::FromSDKString (name);
#endif





/*
********************************************************************************
****************** Configuration::GetComputerName ******************************
********************************************************************************
*/
String	Configuration::GetComputerName ()
{
#if		qPlatform_POSIX
	char	nameBuf[1024];
	Execution::ThrowErrNoIfNegative (gethostname (nameBuf, NEltsOf (nameBuf)));
	nameBuf[NEltsOf (nameBuf)-1] = '\0';	// http://linux.die.net/man/2/gethostname says not necessarily nul-terminated
	return String::FromNarrowSDKString (nameBuf);
#elif	qPlatform_Windows
	constexpr	COMPUTER_NAME_FORMAT	kUseNameFormat_ = ComputerNameNetBIOS;	// total WAG -- LGP 2014-10-10
	DWORD	dwSize = 0;
    (void) ::GetComputerNameEx (kUseNameFormat_, nullptr, &dwSize);
	Memory::SmallStackBuffer<SDKChar> buf(dwSize);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (::GetComputerNameEx (kUseNameFormat_, buf, &dwSize));
	return String::FromSDKString (buf);
#else
	AssertNotImplemented ();
	return String ();
#endif
}
