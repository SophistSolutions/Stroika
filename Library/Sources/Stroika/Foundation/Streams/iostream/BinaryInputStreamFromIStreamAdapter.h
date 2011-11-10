/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_
#define	_Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_	1

#include	"../../StroikaPreComp.h"

#include	<istream>

#include	"../../Configuration/Common.h"
#include	"../../Execution/CriticalSection.h"
#include	"../../Memory/SmallStackBuffer.h"

#include	"../BinaryInputStream.h"


/*
 * TODO:
 *			o	Consider mixing in Seekable here. Would be easy
 */




namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {
			namespace	iostream {

				/*
				 * @THREADSAFETY:	BinaryInputStreamFromIStreamAdapter is not necessarily thread safe. Its roughly as safe as the underlying istream implementation, except
				 *	that we call read, followed by gcount () - which could be a race.
				 */
				class	BinaryInputStreamFromIStreamAdapter : public BinaryInputStream {
					public:
						NO_DEFAULT_CONSTRUCTOR(BinaryInputStreamFromIStreamAdapter);
						NO_COPY_CONSTRUCTOR(BinaryInputStreamFromIStreamAdapter);
						NO_ASSIGNMENT_OPERATOR(BinaryInputStreamFromIStreamAdapter);

					public:
						BinaryInputStreamFromIStreamAdapter (istream& originalStream);
				
					protected:
						virtual	size_t			_Read (Byte* intoStart, Byte* intoEnd) override;
						virtual	SeekOffsetType	_GetOffset () const override;
						virtual	bool			_CanSeek (Whence whence) const override;
						virtual	void			_Seek (Whence whence, SeekOffsetType offset) override;

					private:
						istream&	fOriginalStream_;
				};

			}
		}
	}
}

#endif	/*_Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
