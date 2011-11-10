/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_BufferedBinaryInputStream_h_
#define	_Stroika_Foundation_Streams_BufferedBinaryInputStream_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Memory/SharedPtr.h"
#include	"BinaryInputStream.h"





namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {


			/*
			 * 
			 */
			class	BufferedBinaryInputStream : public BinaryInputStream {
				public:
					BufferedBinaryInputStream (const Memory::SharedPtr<BinaryInputStream>& realIn);
				
				protected:
					virtual	size_t			_Read (Byte* intoStart, Byte* intoEnd) override;
					virtual	SeekOffsetType	_GetOffset () const override;
					virtual	bool			_CanSeek (Whence whence) const override;
					virtual	void			_Seek (Whence whence, SeekOffsetType offset) override;


				private:
					Memory::SharedPtr<BinaryInputStream>	fRealIn_;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Streams_BufferedBinaryInputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"BufferedBinaryInputStream.inl"
