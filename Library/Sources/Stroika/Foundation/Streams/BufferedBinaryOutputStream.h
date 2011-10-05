/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_
#define	_Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Memory/SharedPtr.h"
#include	"BinaryOutputStream.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Streams {

			/*
			 * 
			 */
			class	BufferedBinaryOutputStream : public BinaryOutputStream {
				public:
					BufferedBinaryOutputStream (const Memory::SharedPtr<BinaryOutputStream>& realOut);
				
				public:
					nonvirtual	void	Flush ();

				protected:
					// pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
					// Writes always succeed fully or throw.
					virtual	void	_Write (const Byte* start, const Byte* end) override;

				private:
					Memory::SharedPtr<BinaryOutputStream>	fRealOut_;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"BufferedBinaryOutputStream.inl"
