/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<io.h>
#elif	qPlatform_POSIX
	#include	<unistd.h>
#endif
#include	<cstdlib>

#include	"Socket.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::Network;



class	Socket::Rep_ {
	public:
		NativeSocket	fSD_;
	public:
		Rep_ ()
			{
			}
		Rep_ (NativeSocket sd)
			: fSD_ (sd)
			{
			}

	public:
		void	Close ()
			{
				#if		qPlatform_Windows
					::_close (fSD_);
				#elif	qPlatform_POSIX
					::close (fSD_);
				#else
					AssertNotImplemented ();
				#endif
			}
	public:
		size_t	Read (Byte* buffer, size_t bufSize) override
			{
				// Must do erorr checking and throw exceptions!!!
				#if		qPlatform_Windows
					return ::_read (fSD_, buffer, bufSize);
				#elif	qPlatform_POSIX
					return ::read (fSD_, buffer, bufSize);
				#else
					AssertNotImplemented ();
				#endif
			}
	public:
		void	Write (const Byte* buffer, size_t bufSize) override
			{
				// Must do erorr checking and throw exceptions!!!
				#if		qPlatform_Windows
					int		n	=	::_write (fSD_, buffer, bufSize);
				#elif	qPlatform_POSIX
					int		n	=	::write (fSD_, buffer, bufSize);
				#else
					AssertNotImplemented ();
				#endif
			}
	public:
		void	Bind (const Socket::BindProperties& bindProperties)
			{
			//
			}

	public:
		Socket	Accept ()
			{
				return Socket ();
			}

};





Socket::Socket ()
{
}

Socket::Socket (const Socket& s)
{
}

Socket::Socket (NativeSocket sd)
{
}

Socket::~Socket ()
{
}

const Socket& Socket::operator= (const Socket& s)
{
	return *this;
}

void	Socket::Bind (const BindProperties& bindProperties)
{
	fRep_->Bind (bindProperties);
}

Socket	Socket::Accept ()
{
	return fRep_->Accept ();
}

size_t	Socket::Read (Byte* buffer, size_t bufSize)
{
	return fRep_->Read (buffer, bufSize);
}

void	Socket::Write (const Byte* buffer, size_t bufSize)
{
	fRep_->Write (buffer, bufSize);
}

void	Socket::Close ()
{
	fRep_->Close ();
}
