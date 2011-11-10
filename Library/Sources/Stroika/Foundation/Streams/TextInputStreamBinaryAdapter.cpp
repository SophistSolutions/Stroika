/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"



#include	"../Containers/Common.h"
#include	"../Execution/OperationNotSupportedException.h"
#include	"../Memory/SmallStackBuffer.h"

#include	"TextInputStreamBinaryAdapter.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Streams;


/*
 ********************************************************************************
 ************************** Streams::TextInputStream ****************************
 ********************************************************************************
 */
TextInputStreamBinaryAdapter::TextInputStreamBinaryAdapter (BinaryInputStream& src)
	: fSource_ (src)
{
}

size_t	TextInputStreamBinaryAdapter::_Read (Character* intoStart, Character* intoEnd) override
{
	Memory::SmallStackBuffer<Byte>	buf (intoEnd - intoStart);
	size_t	n	=	fSource_.Read (buf.begin (), buf.end ());
	size_t	outN	=	0;
	for (size_t i = 0; i < n; ++i) {
		intoStart[i] = Characters::Character ((char)*(buf.begin () + i));
		outN++;
	}
	Ensure (outN <= static_cast<size_t> (intoEnd - intoStart));
	return outN;
}

Streams::SeekOffsetType	TextInputStreamBinaryAdapter::_GetOffset () const override
{
	// REALLY JUST NOT YET IMPLEMENTED - BUT IT SHOULD BE
	Execution::DoThrow (Execution::OperationNotSupportedException (L"SocketStream::GetOffset"));
}

bool	TextInputStreamBinaryAdapter::_CanSeek (Streams::Whence whence) const override
{
	// REALLY JUST NOT YET IMPLEMENTED - BUT IT SHOULD BE
	return false;
}

void	TextInputStreamBinaryAdapter::_Seek (Streams::Whence whence, Streams::SeekOffsetType offset) override
{
	// REALLY JUST NOT YET IMPLEMENTED - BUT IT SHOULD BE
	Execution::DoThrow (Execution::OperationNotSupportedException (L"SocketStream::Seek"));
}
