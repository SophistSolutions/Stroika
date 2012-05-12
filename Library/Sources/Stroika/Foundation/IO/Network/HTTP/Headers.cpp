/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include	"../../../StroikaPreComp.h"

#include	"../../../Characters/Format.h"
#include	"../../../Execution/Exceptions.h"

#include	"Headers.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::Network;
using	namespace	Stroika::Foundation::IO::Network::HTTP;



extern	const	wchar_t	HeaderName::kContentType[]		=	L"Content-Type";
extern	const	wchar_t	HeaderName::kContentLength[]	=	L"Content-Length";
extern	const	wchar_t	HeaderName::kServer[]			=	L"Server";
extern	const	wchar_t	HeaderName::kDate[]				=	L"Date";
extern	const	wchar_t	HeaderName::kLastModified[]		=	L"Last Modified";
extern	const	wchar_t	HeaderName::kUserAgent[]		=	L"User-Agent";
extern	const	wchar_t	HeaderName::kSOAPAction[]		=	L"SOAPAction";
extern	const	wchar_t	HeaderName::kAcceptEncoding[]	=	L"Accept-Encoding";



