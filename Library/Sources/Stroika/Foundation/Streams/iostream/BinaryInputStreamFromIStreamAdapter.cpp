/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	"BinaryInputStreamFromIStreamAdapter.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Streams;
using	namespace	Stroika::Foundation::Streams::iostream;





/*
 ********************************************************************************
 ******* Streams::iostream::BinaryInputStreamFromIStreamAdapter *****************
 ********************************************************************************
 */
BinaryInputStreamFromIStreamAdapter::BinaryInputStreamFromIStreamAdapter (std::istream& originalStream)
	: fOriginalStream_ (originalStream)
{
}

size_t	BinaryInputStreamFromIStreamAdapter::_Read (Byte* intoStart, Byte* intoEnd)
{
	RequireNotNull (intoStart);
	RequireNotNull (intoEnd);
	Require (intoStart < intoEnd);

	size_t	maxToRead	=	intoEnd - intoStart;
	fOriginalStream_.read (reinterpret_cast<char*> (intoStart), maxToRead);
	return static_cast<size_t> (fOriginalStream_.gcount ());		// cast safe cuz amount asked to read was also size_t
}
